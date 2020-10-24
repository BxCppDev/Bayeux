// datatools/urn_db_service.cc
//
// Copyright (c) 2017 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of datatools.
//
// datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools. If not, see <http://www.gnu.org/licenses/>.

// Ourselves:
#include <datatools/urn_db_service.h>

// Standard library:
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>

// This project:
#include <datatools/exception.h>
#include <datatools/multi_properties.h>
#include <datatools/utils.h>
#include <datatools/urn.h>
#include <datatools/urn_query_service.h>
#include <datatools/kernel.h>
#include <datatools/dependency_graph.h>

namespace datatools {

  /* Auto-registration of this service class in a central service Db */
  DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(urn_db_service, "datatools::urn_db_service")

  urn_db_service::dependee_db_entry::dependee_db_entry()
  {
    _db_ = nullptr;
    return;
  }

  urn_db_service::dependee_db_entry::dependee_db_entry(const urn_db_service & db_)
  {
    _db_ = &db_;
    return;
  }

  void urn_db_service::dependee_db_entry::make(const urn_db_service & db_)
  {
    _db_ = &db_;
    return;
  }

  const urn_db_service & urn_db_service::dependee_db_entry::get_db() const
  {
    return *_db_;
  }

  /* ============================================== */

  bool urn_db_service::urn_record::is_local() const
  {
    return _uinfo_.is_valid();
  }

  bool urn_db_service::urn_record::is_mounted() const
  {
    return _mounted_uinfo_ != nullptr and _mounted_db_ref_ != nullptr;
  }

  bool urn_db_service::urn_record::is_valid() const
  {
    return is_local() || is_mounted();
  }

  void urn_db_service::urn_record::make_local(const std::string & urn_,
                                              const std::string & category_,
                                              const std::string & description_)
  {
    _uinfo_ = urn_info(urn_, category_, description_);
    _mounted_db_ref_ = nullptr;
    _mounted_uinfo_  = nullptr;
    return;
  }

  void urn_db_service::urn_record::make_mounted(const urn_db_service & mounted_db_,
                                                const std::string & mounted_urn_)
  {
    _uinfo_.reset();
    _mounted_db_ref_ = &mounted_db_;
    _mounted_uinfo_  = &_mounted_db_ref_->get(mounted_urn_);
    return;
  }

  const urn_info & urn_db_service::urn_record::get() const
  {
    DT_THROW_IF(!is_valid(), std::logic_error, "Invalid URN record!");
    if (is_local()) return _uinfo_;
    return *_mounted_uinfo_;
  }

  urn_info & urn_db_service::urn_record::_grab_()
  {
    DT_THROW_IF(!is_local(), std::logic_error, "Non local URN record!");
    return _uinfo_;
  }

  /* ============================================== */

  std::size_t urn_db_service::backward_links::get_number_of_links() const
  {
    return _urns_.size();
  }

  bool urn_db_service::backward_links::is_linked_to_urn(const std::string & urn_) const
  {
    return _urns_.count(urn_);
  }

  void urn_db_service::backward_links::add_link_to_urn(const std::string & urn_)
  {
    _urns_.insert(urn_);
    return;
  }

  void urn_db_service::backward_links::remove_link_to_urn(const std::string & urn_)
  {
    _urns_.erase(urn_);
    return;
  }

  void urn_db_service::backward_links::clear()
  {
    _urns_.clear();
    return;
  }

  const std::set<std::string> & urn_db_service::backward_links::get_urns() const
  {
    return _urns_;
  }

  /* ============================================== */
  urn_db_service::urn_db_service()
  {
    return;
  }

  urn_db_service::~urn_db_service()
  {
    if (this->is_initialized()) this->urn_db_service::reset();
    return;
  }

  bool urn_db_service::is_allow_mounted() const
  {
    return _allow_mounted_;
  }

  void urn_db_service::set_allow_mounted(bool m_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Service is already initialized!");
    _allow_mounted_ = m_;
    return;
  }

  bool urn_db_service::has_explicit_allowed_categories() const
  {
    return _allowed_categories_.size() > 0;
  }

  bool urn_db_service::has_allowed_category(const std::string & category_) const
  {
    return _allowed_categories_.count(category_) == 1;
  }

  bool urn_db_service::check_category(const std::string & category_) const
  {
    if (_allowed_categories_.size() == 0) return true;
    return has_allowed_category(category_);
  }

  void urn_db_service::add_allowed_category(const std::string & category_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Service is already initialized!");
    DT_THROW_IF(!urn_info::validate_category(category_),
                std::logic_error,
                "Invalid category name '" << category_ << "'!");
    _allowed_categories_.insert(category_);
    return;
  }

  void urn_db_service::remove_allowed_category(const std::string & category_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Service is already initialized!");
    DT_THROW_IF(!has_allowed_category(category_), std::logic_error,
                "No allowed category '" << category_ << "'!");
    _allowed_categories_.erase(category_);
    return;
  }

  const std::set<std::string> & urn_db_service::get_allowed_categories() const
  {
    return _allowed_categories_;
  }

  bool urn_db_service::has_explicit_allowed_link_topics() const
  {
    return _allowed_link_topics_.size() > 0;
  }

  bool urn_db_service::has_allowed_link_topic(const std::string & link_topic_) const
  {
    return _allowed_link_topics_.count(link_topic_) == 1;
  }

  bool urn_db_service::check_link_topic(const std::string & link_topic_) const
  {
    if (_allowed_link_topics_.size() == 0) return true;
    return has_allowed_link_topic(link_topic_);
  }

  void urn_db_service::add_allowed_link_topic(const std::string & link_topic_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Service is already initialized!");
    DT_THROW_IF(!urn_info::validate_topic(link_topic_),
                std::logic_error,
                "Invalid link topic '" << link_topic_ << "'!");
    _allowed_link_topics_.insert(link_topic_);
    return;
  }

  void urn_db_service::remove_allowed_link_topic(const std::string & link_topic_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Service is already initialized!");
    DT_THROW_IF(!has_allowed_link_topic(link_topic_), std::logic_error,
                "No allowed link topic '" << link_topic_ << "'!");
    _allowed_link_topics_.erase(link_topic_);
    return;
  }

  const std::set<std::string> & urn_db_service::get_allowed_link_topics() const
  {
    return _allowed_link_topics_;
  }

  bool urn_db_service::has_dependees() const
  {
    if (_dependee_dbs_.size()) return true;
    return false;
  }

  std::set<std::string> urn_db_service::get_dependees() const
  {
    std::set<std::string> depees;
    for (dependee_db_dict_type::const_iterator it = _dependee_dbs_.begin();
         it != _dependee_dbs_.end();
         it++) {
      depees.insert(it->second.get_db().get_name());
    }
    return depees;
  }

  void urn_db_service::connect_db(const urn_db_service & db_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Service is already initialized!");
    DT_THROW_IF(this == &db_, std::logic_error,
                "Service cannot connect to itself!");
    DT_THROW_IF(!db_.is_initialized(), std::logic_error,
                "Cannot connect uninitialized URN database service '" << db_.get_name() << "'!");
    DT_THROW_IF(!db_.is_locked(), std::logic_error,
                "Cannot connect unlocked URN database service '" << db_.get_name() << "'!");
    dependee_db_entry dbe(db_);
    _dependee_dbs_[db_.get_name()] = dbe;
    return;
  }

  bool urn_db_service::is_initialized() const
  {
    return _initialized_;
  }

  int urn_db_service::initialize(const datatools::properties & config_,
                                 datatools::service_dict_type & services_)
  {
    DT_LOG_TRACE_ENTERING(get_logging_priority());
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Service is already initialized!");
    base_service::_common_initialize(config_);

    if (config_.has_key("allow_mounted")) {
      set_allow_mounted(config_.fetch_boolean("allow_mounted"));
    }

    {
      std::set<std::string> allowed_categories;
      if (config_.has_key("allowed_categories")) {
        config_.fetch("allowed_categories", allowed_categories);
      }
      for (const std::string & cat : allowed_categories) {
        add_allowed_category(cat);
      }
    }

    {
      std::set<std::string> allowed_link_topics;
      if (config_.has_key("allowed_link_topics")) {
        config_.fetch("allowed_link_topics", allowed_link_topics);
      }
      for (const std::string & topic : allowed_link_topics) {
        add_allowed_link_topic(topic);
      }
    }

    {
      // std::cerr << "[DEVEL] " << "urn_db_service::initialize: "
      //           << "external URN database services..."
      //           << std::endl;
      std::vector<std::string> dbs;
      if (config_.has_key("external_databases")) {
        // std::cerr << "[DEVEL] " << "urn_db_service::initialize: "
        //           << "found 'external_databases' key..."
        //           << std::endl;
        DT_THROW_IF(!is_allow_mounted(), std::logic_error,
                    "Connections to external URN database services are not allowed!");
        config_.fetch("external_databases", dbs);
      }
      // std::cerr << "[DEVEL] " << "urn_db_service::initialize: "
      //           << "Number of requested external URN database services :" << dbs.size()
      //           << std::endl;
      // for (auto name : dbs) {
      //   std::cerr << "[DEVEL] " << "urn_db_service::initialize: "
      //             << " - URN database service: '" << name << "'"
      //             << std::endl;
      // }
      if (dbs.size()) {
        // We need to explore the dictionary of external services only
        // if some external databases are explicitely required above:
        std::vector<std::string> db_names;
        DT_THROW_IF(!find_service_names_with_id(services_,
                                                "datatools::urn_db_service",
                                                db_names),
                    std::logic_error,
                    "Cannot find URN database services!");
        for (std::size_t idb = 0; idb < dbs.size(); idb++) {
          std::string db_name = dbs[idb];
          if (std::find(db_names.begin(), db_names.end(), db_name) == db_names.end()) {
            DT_THROW(std::logic_error,
                     "Cannot find URN database service named '" << db_name << "'!");
          }
          const urn_db_service & db
            = datatools::get<datatools::urn_db_service>(services_, db_name);
          connect_db(db);
        }
      }

      // if (config_.has_key("mounted_urn_regex")) {
      //   DT_THROW_IF(!is_allow_mounted(), std::logic_error,
      //               "Mounted URN from external URN database services is not allowed!");
      //   /*
      //    * mounted_urns_regex : string = "@UrnDb1:urn:datatools:(.*):1"
      //    */
      //   std::string mounted_urn_regex = config_.fetch("mounted_urn_regex");
      // }

      if (config_.has_key("mounted_urns")) {
        DT_THROW_IF(!is_allow_mounted(), std::logic_error,
                    "Mounted URN from external URN database services is not allowed!");
        /*
         * mounted_urns : string[2] = \
         *   "@UrnDb1:urn:datatools:test:1" \
         *   "@UrnDb2:urn:datatools:test:2"
         *
         */
        std::vector<std::string> mounted_urns;
        config_.fetch("mounted_urns", mounted_urns);
        for (std::size_t i = 0; i < mounted_urns.size(); i++) {
          std::string external_db;
          std::string mounted_urn;
          if (! parse_mount_rule(mounted_urns[i], external_db, mounted_urn)) {
            DT_THROW(std::logic_error,
                     "Invalid format for mount rule '" << mounted_urns[i] << "'");
          }
          add_mounted(external_db, mounted_urn);
        }
      }
    }

    {
      std::vector<std::string> leaves_csv_filenames;
      if (config_.has_key("urn_infos.csv_leaves")) {
        DT_LOG_DEBUG(get_logging_priority(), "Parsing 'urn_infos.csv_leaves'...");
        config_.fetch("urn_infos.csv_leaves", leaves_csv_filenames);
      }
      for (const std::string & filename : leaves_csv_filenames) {
        std::string file = filename;
        datatools::fetch_path_with_env(file);
        load_csv(file);
      }
    }

    {
      std::vector<std::string> urn_infos_def_filenames;
      if (config_.has_key("urn_infos.definitions")) {
        DT_LOG_DEBUG(get_logging_priority(), "Parsing 'urn_infos.definitions'...");
        config_.fetch("urn_infos.definitions", urn_infos_def_filenames);
      }
      for (const std::string & filename : urn_infos_def_filenames) {
        std::string file = filename;
        datatools::fetch_path_with_env(file);
        load(file);
      }
    }

    _init_();
    _initialized_ = true;

    if (config_.has_key("lock")) {
      if (config_.fetch_boolean("lock")) {
        lock();
      }
    }

    // Publish the service in the Kernel's URN system singleton:
    if (config_.has_flag("kernel.push")) {
      DT_LOG_DEBUG(get_logging_priority(), "Parsing 'kernel.push'...");
      DT_THROW_IF(!datatools::kernel::is_instantiated(),
                  std::logic_error,
                  "Bayeux/datatools' kernel is not instantiated! Cannot apply the kernel push op!");
      std::string name;
      if (config_.has_key("kernel.push.name")) {
        DT_LOG_DEBUG(get_logging_priority(), "Parsing 'kernel.push.name'...");
        name = config_.fetch_string("kernel.push.name");
      }
      kernel_push(name);
    }
    DT_LOG_TRACE_EXITING(get_logging_priority());
    return datatools::SUCCESS;
  }

  int urn_db_service::reset()
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Service is not initialized!");

    if (is_kernel_pushed()) {
      kernel_pop();
    }
    if (is_locked()) {
      unlock();
    }
    _initialized_ = false;
    _urn_records_.clear();
    _allow_mounted_ = false;
    _dependee_dbs_.clear();
    return datatools::SUCCESS;
  }

  void urn_db_service::tree_dump(std::ostream& out_,
                                 const std::string& title_,
                                 const std::string& indent_,
                                 bool inherit_) const
  {
    this->base_service::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << i_tree_dumpable::tag
         << "Allow mounted  : "
         << std::boolalpha << is_allow_mounted() << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Allow categories : ";
    if (_allowed_categories_.size() == 0) out_ << "<any>";
    // out_ << '[' << _allowed_categories_.size() << ']';
    out_ << std::endl;
    for (std::set<std::string>::const_iterator i = _allowed_categories_.begin();
         i != _allowed_categories_.end();
         i++) {
      out_ << indent_ << i_tree_dumpable::skip_tag;
      std::set<std::string>::const_iterator j = i;
      if (++j == _allowed_categories_.end()) {
        out_ << i_tree_dumpable::last_tag;
      } else {
        out_ << i_tree_dumpable::tag;
      }
      out_ << "Category '" << *i << "' ";
      out_ << std::endl;
    }

    out_ << indent_ << i_tree_dumpable::tag
         << "Allow link topics : ";
    if (_allowed_link_topics_.size() == 0) out_ << "<any>";
    // out_ << '[' << _allowed_link_topics_.size() << ']';
    out_ << std::endl;
    for (std::set<std::string>::const_iterator i = _allowed_link_topics_.begin();
         i != _allowed_link_topics_.end();
         i++) {
      out_ << indent_ << i_tree_dumpable::skip_tag;
      std::set<std::string>::const_iterator j = i;
      if (++j == _allowed_link_topics_.end()) {
        out_ << i_tree_dumpable::last_tag;
      } else {
        out_ << i_tree_dumpable::tag;
      }
      out_ << "Link topic '" << *i << "' ";
      out_ << std::endl;
    }

    out_ << indent_ << i_tree_dumpable::tag
         << "Connected database services : ";
    out_ << '[' << _dependee_dbs_.size() << ']';
    out_ << std::endl;
    for (dependee_db_dict_type::const_iterator i = _dependee_dbs_.begin();
         i != _dependee_dbs_.end();
         i++) {
      out_ << indent_ << i_tree_dumpable::skip_tag;
      dependee_db_dict_type::const_iterator j = i;
      if (++j == _dependee_dbs_.end()) {
        out_ << i_tree_dumpable::last_tag;
      } else {
        out_ << i_tree_dumpable::tag;
      }
      out_ << "URN database service '" << i->first << "' ";
      out_ << std::endl;
    }

    out_ << indent_ << i_tree_dumpable::tag
         << "URN nodes : "
         << _urn_records_.size() << std::endl;
    for (urn_record_dict_type::const_iterator i = _urn_records_.begin();
         i != _urn_records_.end();
         i++) {
      out_ << indent_ << i_tree_dumpable::skip_tag;
      std::ostringstream indent2;
      indent2 << indent_ << i_tree_dumpable::skip_tag;
      urn_record_dict_type::const_iterator j = i;
      j++;
      if (j == _urn_records_.end()) {
        out_ << i_tree_dumpable::last_tag;
        indent2 << i_tree_dumpable::last_skip_tag;
      } else {
        out_ << i_tree_dumpable::tag;
        indent2 << i_tree_dumpable::skip_tag;
      }
      out_ << "Linked from URN '" << i->first << "'";
      if (i->second.is_mounted()) out_ << " [mounted from '" << i->second._mounted_db_ref_->get_name()  << "']";
      out_ << " : " << std::endl;
      std::ostringstream buf;
      i->second.get().tree_dump(buf, "", indent2.str());
      std::string bufs = buf.str();
      std::size_t eolpos = bufs.find('\n');
      bufs = bufs.substr(eolpos+1);
      // bufs = std::regex_replace(bufs, std::regex(" Components : "), " Links : ");
      out_ << bufs;
    }

    out_ << indent_ << i_tree_dumpable::tag
         << "Backward links : "
         << '[' << _backward_links_.size() << ']' << std::endl;
    for (backward_links_dict_type::const_iterator i = _backward_links_.begin();
         i != _backward_links_.end();
         i++) {
      out_ << indent_ << i_tree_dumpable::skip_tag;
      std::ostringstream indent2;
      indent2 << indent_ << i_tree_dumpable::skip_tag;
      backward_links_dict_type::const_iterator j = i;
      std::string i2;
      if (++j == _backward_links_.end()) {
        out_ << i_tree_dumpable::last_tag;
        indent2 << i_tree_dumpable::last_skip_tag;
      } else {
        out_ << i_tree_dumpable::tag;
        indent2 << i_tree_dumpable::skip_tag;
      }
      out_ << "URN '" << i->first << "' ";
      if (is_mounted(i->first)) out_ << "[mounted]";
      out_ << " : ";
      const std::set<std::string> & linked_urns = i->second.get_urns();
      // out_ << '[' << linked_urns.size() << "'";
      out_ << std::endl;
      for (std::set<std::string>::const_iterator il = linked_urns.begin();
           il != linked_urns.end();
           il++) {
        out_ << indent2.str();
        std::set<std::string>::const_iterator jl = il;
        if (++jl == linked_urns.end()) {
          out_ << i_tree_dumpable::last_tag;
        } else {
          out_ << i_tree_dumpable::tag;
        }
        out_ << "URN : '" << *il << "'" << std::endl;
      }
    }

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Locked : "
         << std::boolalpha << is_locked() << std::endl;

    return;
  }

  void urn_db_service::load(const std::string & filename_)
  {
    datatools::multi_properties mp("urn", "category");
    DT_LOG_DEBUG(get_logging_priority(),
                 "Loading file '" << filename_ << "' with items' URN infos...");
    datatools::multi_properties::read_config(filename_, mp);
    std::vector<std::string> urns;
    mp.ordered_keys(urns);
    for (const std::string & urn : urns) {
      const datatools::multi_properties::entry & mpe = mp.get(urn);
      const std::string & category = mpe.get_meta();
      const datatools::properties & props = mpe.get_properties();
      add(urn, category, "");
      urn_info & uinfo = _grab_(urn);
      uinfo.initialize(props);
      if (!uinfo.is_locked()) {
        uinfo.lock();
      }
      _process_links_(urn);
    }
    return;
  }

  void urn_db_service::load_csv(const std::string & filename_)
  {
    std::string csvfile = filename_;
    datatools::fetch_path_with_env(csvfile);
    DT_LOG_DEBUG(get_logging_priority(),
                 "Loading CSV file '" << csvfile << "' with leaf items' URN infos...");
    std::ifstream fcsv(csvfile.c_str());
    fcsv >> std::ws;
    std::string default_category;
    while (fcsv && !fcsv.eof()) {
      std::string line;
      std::getline(fcsv, line);
      {
        // Skip blank lines and lines starting with '#':
        std::istringstream testline(line);
        std::string word;
        testline >> word;
        if (word.empty()) continue;
        if (word[0] == '#') {
          if (word == "#@category") {
            std::string ignore;
            testline >> default_category >> std::ws;
            if (!testline.eof()) {
              std::getline(testline, ignore);
            }
          }
          continue;
        }
      }
      std::string urn_key;
      std::string urn_category;
      std::string urn_description;
      std::istringstream linein(line);
      linein >> urn_key >> std::ws;
      bool dead_line = false;
      if (! linein.eof()) {
        std::string uc;
        linein >> uc >> std::ws;
        if (!uc.empty() and uc[0] != '#') {
          urn_category = uc;
        } else {
          dead_line = true;
        }
      } else {
        urn_category = default_category;
      }
      if (!dead_line) {
        std::getline(linein, urn_description);
      }
      /* urn_info & ui = */
      add(urn_key, urn_category, urn_description);
    }
    return;
  }

  void urn_db_service::_init_()
  {
    if (_urn_records_.size() == 0) {
      DT_LOG_WARNING(get_logging_priority(),
                     "URN database service '" << get_name() << "' has no record!");
    }
    return;
  }

  bool urn_db_service::has(const std::string & urn_) const
  {
    return _urn_records_.count(urn_) == 1;
  }

  bool urn_db_service::is_mounted(const std::string & urn_) const
  {
    DT_THROW_IF(!has(urn_), std::logic_error,
                "URN='" << urn_ << "' does not exist!");
    const urn_record & ur = _urn_records_.find(urn_)->second;
    return ur.is_mounted();
  }

  bool urn_db_service::is_local(const std::string & urn_) const
  {
    DT_THROW_IF(!has(urn_), std::logic_error,
                "URN='" << urn_ << "' does not exist!");
    const urn_record & ur = _urn_records_.find(urn_)->second;
    return ur.is_local();
  }

  // static
  bool urn_db_service::parse_mount_rule(const std::string & rule_,
                                        std::string & external_db_,
                                        std::string & mounted_urn_)
  {
    external_db_.clear();
    mounted_urn_.clear();
    if (rule_.empty()) return false;
    if (rule_[0] != '@') return false;
    std::size_t cpos = rule_.find(':');
    if (cpos == rule_.npos) return false;
    std::string extdb = rule_.substr(1, cpos - 1);
    std::string murn = rule_.substr(cpos + 1);
    external_db_ = extdb;
    mounted_urn_ = murn;
    return true;
  }

  // // static
  // bool urn_db_service::parse_mount_rule(const std::string & rule_,
  //                                       std::string & external_db_,
  //                                       std::set<std::string> & mounted_urns_)
  // {
  //   external_db_.clear();
  //   mounted_urns_.clear();
  //   if (rule_.empty()) return false;
  //   if (rule_[0] != '@') return false;
  //   std::size_t cpos = rule_.find(':');
  //   if (cpos == rule_.npos) return false;
  //   std::string extdb = rule_.substr(1, cpos - 1);
  //   std::string murnregex = rule_.substr(cpos + 1);
  //   external_db_ = extdb;
  //   mounted_urn_ = murn;
  //   return true;
  // }

  void urn_db_service::add_mounted(const std::string & external_db_,
                                   const std::string & mounted_urn_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Service is locked!");
    DT_THROW_IF(!is_allow_mounted(), std::logic_error,
                "Mounted URN from external database is not allowed!");
    dependee_db_dict_type::const_iterator found = _dependee_dbs_.find(external_db_);
    DT_THROW_IF(found == _dependee_dbs_.end(), std::logic_error,
                "No connected URN database service with name '" << external_db_ << "'!");
    const urn_db_service & db = found->second.get_db();
    DT_THROW_IF(!db.has(mounted_urn_), std::logic_error,
                "Attempt to mount non existing URN='" << mounted_urn_ << "'"
                << " from URN database '" << external_db_ << "'!");
    std::string urn = mounted_urn_;
    DT_THROW_IF(has(urn), std::logic_error, "Mount URN='" << urn << "' already exists!");
    urn_record ur;
    ur.make_mounted(db, mounted_urn_);
    _urn_records_[urn] = ur;
    return;
  }

  void urn_db_service::_process_links_(const std::string & urn_)
  {
    DT_THROW_IF(!has(urn_), std::logic_error,
                "No URN record '" << urn_ << "'!");
    if (is_local(urn_)) {
      urn_info & uinfo = _grab_(urn_);
      if (uinfo.has_components()) {
        std::vector<std::string> topics;
        uinfo.build_topics(topics);
        for (std::size_t i = 0; i < topics.size(); i++) {
          const std::string & topic = topics[i];
          const std::vector<std::string> & components = uinfo.get_components_by_topic(topic);
          for (std::size_t j = 0; j < components.size(); j++) {
            const std::string & component_urn = components[j];
            _add_backward_link_(component_urn, urn_);
          }
        }
      }
    }
    return;
  }

  const urn_info & urn_db_service::add(const std::string & urn_,
                                       const std::string & category_,
                                       const std::string & description_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Service is locked!");
    DT_THROW_IF(!check_category(category_), std::logic_error,
                "Attempt to add URN='" << urn_ << "' with not allowed category '" << category_ << "'!");
    urn_record ur;
    ur.make_local(urn_, category_, description_);
    DT_THROW_IF(has(urn_), std::logic_error,
                "URN record with URN='" << urn_ << "' already exists!");
    _urn_records_[urn_] = ur;
    return _urn_records_.find(urn_)->second.get();
  }

  void urn_db_service::remove_all_links(const std::string & urn_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Service is locked!");
    DT_THROW_IF(!has(urn_), std::logic_error, "URN database has no URN '" << urn_ << "'!");
    DT_THROW_IF(!is_local(urn_), std::logic_error, "URN record '" << urn_ << "' is not local!");
    urn_info & to_be_erased = _grab_(urn_);
    std::vector<std::string> topics;
    to_be_erased.build_topics(topics);
    if (to_be_erased.is_locked()) {
      to_be_erased.unlock();
    }
    for (const std::string & topic : topics) {
      const std::vector<std::string> & vcomps = to_be_erased.get_components_by_topic(topic);
      for (std::size_t icomp = 0; icomp < vcomps.size(); icomp++) {
        remove_link(urn_, vcomps[icomp]);
      }
    }
    return;
  }

  void urn_db_service::remove(const std::string & urn_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Service is locked!");
    DT_THROW_IF(!has(urn_), std::logic_error, "URN database has no URN '" << urn_ << "'!");
    if (is_local(urn_)) {
      remove_all_links(urn_);
    }
    _urn_records_.erase(urn_);
    return;
  }

  bool urn_db_service::has_backward_links(const std::string & urn_) const
  {
    backward_links_dict_type::const_iterator found = _backward_links_.find(urn_);
    if (found != _backward_links_.end()) {
      const backward_links & bl = found->second;
      if (bl.get_number_of_links() > 0) return true;
    }
    return false;
  }

  void urn_db_service::_add_backward_link_(const std::string & urn_to_,
                                           const std::string & urn_from_)
  {
    if (!has_backward_links(urn_to_)) {
      backward_links new_bl;
      _backward_links_[urn_to_] = new_bl;
    }
    backward_links & bl = _backward_links_.find(urn_to_)->second;
    bl.add_link_to_urn(urn_from_);
    return;
  }

  void urn_db_service::_remove_backward_link_(const std::string & urn_to_,
                                              const std::string & urn_from_)
  {
    backward_links_dict_type::iterator found = _backward_links_.find(urn_to_);
    if (found == _backward_links_.end()) {
      DT_THROW(std::logic_error,
               "URN record '" << urn_to_ << "' has no backward links!");
    }
    backward_links & bl = found->second;
    if (!bl.is_linked_to_urn(urn_from_)) {
      DT_THROW(std::logic_error,
               "URN record '" << urn_to_ << "' has no backward link to URN record '"
               << urn_from_ << "'!");
    }
    bl.remove_link_to_urn(urn_from_);
    if (bl.get_number_of_links() == 0) {
      // No more backward links, removing URN entry:
      _backward_links_.erase(found);
    }
    return;
  }

  void urn_db_service::add_link(const std::string & urn_from_,
                                const std::string & urn_to_,
                                const std::string & forward_topic_)
  {
    std::string errmsg;
    if (! add_link(urn_from_, urn_to_, forward_topic_, errmsg)) {
      DT_THROW(std::logic_error, errmsg);
    }
    return;
  }

  bool urn_db_service::add_link(const std::string & urn_from_,
                                const std::string & urn_to_,
                                const std::string & forward_topic_,
                                std::string & errmsg_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Service is locked!");
    if (!has(urn_from_)) {
      errmsg_ = "No URN info record with URN='" + urn_from_ + "'";
      return false;
    }
    if (is_mounted(urn_from_)) {
      errmsg_ = "Cannot link mounted URN='" + urn_from_ + " to URN='" + urn_to_ + "'";
      return false;
    }
    if (!has(urn_to_)) {
      errmsg_ = "No URN info record with URN='" + urn_to_ + "'";
      return false;
    }
    if (!check_link_topic(forward_topic_)) {
      errmsg_ = "Topic '" + forward_topic_ + "' is not allowed from '" + urn_from_ + "'";
      return false;
    }
    try {
      urn_info & from = _grab_(urn_from_);
      from.add_component(urn_to_, forward_topic_);
      _add_backward_link_(urn_to_, urn_from_);
    } catch (std::exception & err) {
      errmsg_ = err.what();
      return false;
    }
    return true;
  }

  void urn_db_service::remove_link(const std::string & urn_from_,
                                   const std::string & urn_to_)
  {
    std::string errmsg;
    if (!remove_link(urn_from_, urn_to_, errmsg)) {
      DT_THROW(std::logic_error, errmsg)
        }
    return;
  }

  bool urn_db_service::remove_link(const std::string & urn_from_,
                                   const std::string & urn_to_,
                                   std::string & errmsg_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Service is locked!");
    if (!has(urn_from_)) {
      errmsg_ = "No from URN info record with URN='" + urn_from_ + "'";
      return false;
    }
    if (!has(urn_to_)) {
      errmsg_ = "No to URN info record with URN='" + urn_to_ + "'";
      return false;
    }
    urn_info & from = _grab_(urn_from_);
    // urn_info & to   = _grab_(urn_to_);
    if (!from.has_component(urn_to_)) {
      errmsg_ = "URN info '" + urn_from_ + "' has no component URN='" + urn_to_ + "'";
      return false;
    }
    try {
      from.remove_component(urn_to_);
      _remove_backward_link_(urn_to_, urn_from_);
    } catch (std::exception & err) {
      errmsg_ = err.what();
      return false;
    }
    return true;
  }


  bool urn_db_service::is_linked_to(const std::string & urn_,
                                    const std::string & other_urn_,
                                    const std::string & topic_) const
  {
    DT_THROW_IF(!has(urn_), std::logic_error,  "URN database has no URN '" << urn_ << "'!");
    DT_THROW_IF(!has(other_urn_), std::logic_error,  "URN database has no URN '" << other_urn_ << "'!");
    const urn_info & u1 = get(urn_);
    if (topic_.empty()) {
      return u1.has_component(other_urn_);
    }
    return u1.has_component_in_topic(other_urn_, topic_);
  }

  urn_info & urn_db_service::_grab_(const std::string & urn_)
  {
    urn_record_dict_type::iterator found = _urn_records_.find(urn_);
    DT_THROW_IF(found == _urn_records_.end(), std::logic_error,
                "URN database has no URN '" << urn_ << "'!");
    return found->second._grab_();
  }

  const urn_info & urn_db_service::get(const std::string & urn_) const
  {
    urn_record_dict_type::const_iterator found = _urn_records_.find(urn_);
    DT_THROW_IF(found == _urn_records_.end(), std::logic_error,
                "URN database has no URN '" << urn_ << "'!");
    return found->second.get();
  }

  std::size_t urn_db_service::get_number_of_urn_records() const
  {
    return _urn_records_.size();
  }

  bool urn_db_service::is_removable(const std::string & urn_) const
  {
    if (is_locked()) return false;
    return !has_backward_links(urn_);
  }

  bool urn_db_service::has_links(const std::string & urn_) const
  {
    DT_THROW_IF(!has(urn_), std::logic_error,  "URN database has no URN '" << urn_ << "'!");
    const urn_info & u = get(urn_);
    return u.has_components();
  }

  void urn_db_service::build_list_of_urns(std::vector<std::string> & urns_,
                                          const uint32_t flags_) const
  {
    bool with_clear = true;
    bool with_local = false;
    bool with_mounted = false;
    bool with_removable = false;
    bool with_linked = false;
    if (flags_ & SELECT_NOCLEAR) {
      with_clear = false;
    }
    if (flags_ & SELECT_LOCAL) {
      with_local = true;
    }
    if (flags_ & SELECT_REMOVABLE) {
      with_removable = true;
    }
    if (flags_ & SELECT_LINKED) {
      with_linked = true;
    }
    if (with_clear) {
      urns_.clear();
    }
    urns_.reserve(_urn_records_.size());
    for (urn_record_dict_type::const_iterator i = _urn_records_.begin();
         i != _urn_records_.end();
         i++) {
      bool selected = false;
      if (i->second.is_local() && with_local) {
        selected = true;
      }
      if (i->second.is_mounted() && with_mounted) {
        selected = true;
      }
      if (is_removable(i->first) && with_removable) {
        selected = true;
      }
      if (has_links(i->first) && with_linked) {
        selected = true;
      }
      if (selected) {
        urns_.push_back(i->first);
      }
    }
    return;
  }

  bool urn_db_service::is_locked() const
  {
    return _locked_;
  }

  void urn_db_service::lock()
  {
    if (_locked_) return;
    _at_lock_();
    _locked_ = true;
    return;
  }

  void urn_db_service::unlock()
  {
    if (!_locked_) return;
    _locked_ = false;
    _at_unlock_();
    return;
  }

  void urn_db_service::_at_lock_()
  {
    for (urn_record_dict_type::iterator i = _urn_records_.begin();
         i != _urn_records_.end();
         i++) {
      if (i->second.is_local()) {
        // Lock all local URN nodes:
        if (!i->second._grab_().is_locked()) {
          i->second._grab_().lock();
        }
      }
    }
    return;
  }

  void urn_db_service::_at_unlock_()
  {
    for (urn_record_dict_type::iterator i = _urn_records_.begin();
         i != _urn_records_.end();
         i++) {
      if (i->second.is_local()) {
        i->second._grab_().unlock();
      }
    }
    return;
  }

  bool urn_db_service::is_kernel_pushed() const
  {
    if (datatools::kernel::is_instantiated()) {
      return datatools::kernel::instance().get_urn_query().has_db(*this);
    }
    return false;
  }

  void urn_db_service::kernel_push(const std::string & name_)
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Cannot register in the library's URN query service if not initialized!");
    if (datatools::kernel::is_instantiated()) {
      if (!datatools::kernel::instance().get_urn_query().has_db(*this)) {
        datatools::kernel::instance().grab_urn_query().add_db(*this, name_);
      }
    }
    return;
  }

  void urn_db_service::kernel_pop()
  {
    if (datatools::kernel::is_instantiated()) {
      if (datatools::kernel::instance().get_urn_query().has_db(*this)) {
        datatools::kernel::instance().grab_urn_query().remove_db(*this);
      }
    }
    return;
  }

  urn_db_service::dependency_graph_builder::dependency_graph_builder(const urn_db_service & db_,
                                                                     const logger::priority & logging_)
    : _db_(db_)
    , _logging_(logging_)
  {
    return;
  }

  void urn_db_service::dependency_graph_builder::add_topic(const std::string & topic_)
  {
    DT_LOG_DEBUG(_logging_, "Using topic '" << topic_ << "'...");
    _topics_.insert(topic_);
    return;
  }

  void urn_db_service::dependency_graph_builder::_process_node_components_(const urn_record & urec_,
                                                                           dependency_graph & deps_,
                                                                           std::set<std::string> & processed_) const
  {
    const urn_info & uinfo = urec_.get();
    DT_LOG_DEBUG(_logging_, "Processing node '" << uinfo.get_urn() << "'...");
    std::vector<std::string> topics;
    uinfo.build_topics(topics);
    for (auto topic : topics) {
      if (_topics_.size() && _topics_.count(topic) == 0) {
        DT_LOG_DEBUG(_logging_, "Skip components with topic '" << topic << "'...");
        continue;
      }
      std::size_t ncomps = uinfo.get_number_of_components_by_topic(topic);
      for (std::size_t icomp = 0; icomp < ncomps; icomp++) {
        std::string comp_urn = uinfo.get_component(topic, icomp);
        DT_LOG_DEBUG(_logging_, "Processing dependee node '" << comp_urn << "'...");
        const urn_record & comp_rec = _db_._urn_records_.find(comp_urn)->second;
        const urn_info & comp = comp_rec.get();
        if (!deps_.has_vertex(comp_urn)) {
          DT_LOG_DEBUG(_logging_, "Adding vertex for dependee node '" << comp_urn << "'...");
          deps_.add_vertex(comp_urn, comp.get_category());
          _process_node_components_(comp_rec, deps_, processed_);
        } else {
          DT_LOG_DEBUG(_logging_, "Dependee node '" << comp_urn << "' has been already processed.");
        }
        DT_LOG_DEBUG(_logging_, "Adding dependency out-edge from '" << uinfo.get_urn() << "' to '" << comp_urn << "'...");
        deps_.add_out_edge(uinfo.get_urn(), comp_urn, topic);
      }
    }
    return;
  }

  void urn_db_service::dependency_graph_builder::make_deps(dependency_graph & deps_) const
  {
    if (_topics_.size() == 0) {
      DT_LOG_DEBUG(_logging_, "Import topics from '" << _db_.get_name() << "'...");
      dependency_graph_builder * mutable_this = const_cast<dependency_graph_builder *>(this);
      for (auto t : _db_.get_allowed_link_topics()) {
        DT_LOG_DEBUG(_logging_, " - topic '" << t << "'");
      }
      mutable_this->_topics_ = _db_.get_allowed_link_topics();
    }
    std::set<std::string> processed_records;
    for (urn_record_dict_type::const_iterator it = _db_._urn_records_.begin();
         it != _db_._urn_records_.end();
         it++) {
      const std::string & urn = it->first;
      const urn_record & urec = it->second;
      const urn_info & uinfo = urec.get();
      if (!deps_.has_vertex(urn)) {
        if (urec.is_local()) {
          deps_.add_vertex(urn, uinfo.get_category());
          if (processed_records.count(urn) == 0) {
            _process_node_components_(urec, deps_, processed_records);
          }
        }
      }
      processed_records.insert(urn);
    }
    return;
  }

} // end of namespace datatools

/***************
 * OCD support *
 ***************/

#include <datatools/ocd_macros.h>

// OCD support for class '::datatools::urn_db_service' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::datatools::urn_db_service, ocd_)
{
  ocd_.set_class_name("datatools::urn_db_service");
  ocd_.set_class_description("URN information database service");
  ocd_.set_class_library("datatools");
  ocd_.set_class_documentation("A *service* which host a dictionary of items addressed  \n"
                               "through their unique URNs and classified in *categories* \n");


  // Invoke OCD support from parent class :
  ::datatools::base_service::common_ocd(ocd_);

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("allow_mounted")
      .set_terse_description("Flag to allow the mounting of URNs records from external database service")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("allowed_categories")
      .set_terse_description("List of allowed categories")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(false)
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("allowed_link_topics")
      .set_terse_description("List of allowed link topics")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(false)
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("external_databases")
      .set_terse_description("List of external URN database services to be connected")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(false)
      .add_example("Specify external URN database services:: \n"
                   "                                         \n"
                   "  external_databases : string[2] = \\    \n"
                   "    \"UrnDb1\" \\ \n"
                   "    \"UrnDb2\"    \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("mounted_urns")
      .set_terse_description("List of directives to mount URNs from external URN database service")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(false)
      .add_example("Mount some external URNs::             \n"
                   "                                       \n"
                   " mounted_urns : string[2] = \\         \n"
                   "   \"@UrnDb1:urn:datatools:test:1\" \\ \n"
                   "   \"@UrnDb2:urn:datatools:test:2\"    \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("urn_infos.csv_leaves")
      .set_terse_description("A  list of CSV formatted files which contains leaf URN definitions")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_path(true)
      .set_mandatory(false)
      .set_long_description("Each file use a CSV ASCII format:  \n"
                            "                                                                 \n"
                            "Example:                                                         \n"
                            "                                                                 \n"
                            "#@category data                                                  \n"
                            "urn:fooexp:data:run:0                                            \n"
                            "urn:fooexp:data:run:1                                            \n"
                            "urn:fooexp:data:run:2                                            \n"
                            "urn:fooexp:data:run:3                                            \n"
                            "urn:fooexp:data:run:4                                            \n"
                            "                                                                 \n"
                            "urn:fooexp:data:metadata:1.0 metadata Metadata associated to the runs\n"
                            "urn:fooexp:data:run:5                                            \n"
                            "urn:fooexp:data:run:6                                            \n"
                            "urn:fooexp:data:run:7                                            \n"
                            "urn:fooexp:data:run:7                                            \n"
                            "                                                                 \n"
                            )
      .add_example("Use an explicit list of leaf URNs CSV formatted files::      \n"
                   "                                                             \n"
                   "    urn_infos.csv_leaves : string[2] as path = \\            \n"
                   "      \"${DATA_REPOSITORY_DIR}/fooexp/phase1/runs.lis\" \\   \n"
                   "      \"${DATA_REPOSITORY_DIR}/fooexp/phase2/runs.lis\"      \n"
                   "                                                             \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("urn_infos.defintions")
      .set_terse_description("A  list of files which contains definitions of URN informations")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_path(true)
      .set_mandatory(false)
      .set_long_description("Each file use the ``datatools::multi_properties`` format:        \n"
                            "                                                                 \n"
                            "Example:                                                         \n"
                            "                                                                 \n"
                            "#@description Definition list for URN infos                      \n"
                            "#@key_label   \"urn\"                                            \n"
                            "#@meta_label  \"category\"                                       \n"
                            "                                                                 \n"
                            "[urn=\"urn:bayeux:images:logo:small\" category=\"graphics\"]     \n"
                            "description : string = \"Bayeux logo (small)\"                   \n"
                            "                                                                 \n"
                            "[urn=\"urn:bayeux:images:logo:large\" category=\"graphics\"]     \n"
                            "description : string = \"Bayeux logo (large)\"                   \n"
                            "                                                                 \n"
                            "                                                                 \n"
                            )
      .add_example("Use an explicit list of URNs definition files::                            \n"
                   "                                                                  \n"
                   "    urn_infos.definitions : string[2] as path = \\                \n"
                   "      \"${DATA_REPOSITORY_DIR}/fooexp/config/simulation.defs\" \\ \n"
                   "      \"${DATA_REPOSITORY_DIR}/fooexp/config/calibration.defs\"   \n"
                   "                                                                  \n")
      ;
  }

  ocd_.set_configuration_hints("A geometry manager is configured through a configuration file that  \n"
                               "uses the format of 'datatools::properties' setup file.              \n"
                               "Example: ::         .                                               \n"
                               "  name : string = \"Foo\"                                           \n"
                               "  allow_mounted : boolean = false                                   \n"
                               "  allowed_categories : string[2] = \"bar\" \"baz\"                  \n"
                               "  allowed_link_topics : string[1] = \"dependency\"                  \n"
                               "  logging.priority : string = \"warning\"                           \n"
                               "  urn_infos.csv_leaves : string[2] as path = \\                     \n"
                               "      \"${DATA_REPOSITORY_DIR}/fooexp/phase1/runs.lis\" \\          \n"
                               "      \"${DATA_REPOSITORY_DIR}/fooexp/phase2/runs.lis\"             \n"
                               "  urn_infos.defintions : string[2] as path = \\                     \n"
                               "      \"${DATA_REPOSITORY_DIR}/fooexp/config/simulation.defs\" \\   \n"
                               "      \"${DATA_REPOSITORY_DIR}/fooexp/config/calibration.defs\"     \n"
                               );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}

DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::datatools::urn_db_service,"datatools::urn_db_service")

// urn_to_path_resolver_service.cc - Implementation of URN to path resolver service
//
// Copyright (c) 2017 by Francois Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2017 by Université de Caen
//
// This file is part of Bayeux/datatools.
//
// Bayeux/datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux/datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux/datatools.  If not, see <http://www.gnu.org/licenses/>.

// Ourselves
#include <datatools/urn_to_path_resolver_service.h>

// Standard Library
#include <sstream>

// Third party
// - Boost
#include <boost/algorithm/string.hpp>
// - Bayeux
#include <datatools/exception.h>
#include <datatools/multi_properties.h>
#include <datatools/urn.h>
#include <datatools/urn_query_service.h>
#include <datatools/kernel.h>

namespace datatools {

  /** Auto-registration of this service class in a central service Db */
  DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(urn_to_path_resolver_service, "datatools::urn_to_path_resolver_service")

  // static
  const std::set<std::string> & urn_to_path_resolver_service::default_known_categories()
  {
    static std::unique_ptr<std::set<std::string> > _s;
    if (! _s) {
      _s.reset(new std::set<std::string>);
      _s->insert("configuration");
      _s->insert("data");
      _s->insert("graphics");
      _s->insert("archive");
      _s->insert("log");
    }
    return *_s;
  }

  urn_to_path_resolver_service::urn_to_path_resolver_service()
  {
    return;
  }

  urn_to_path_resolver_service::~urn_to_path_resolver_service()
  {
    if (this->is_initialized()) this->urn_to_path_resolver_service::reset();
    return;
  }

  void urn_to_path_resolver_service::add_known_category(const std::string & cat_)
  {
    static const uint32_t nv_flags = datatools::NV_MODEL;
    DT_THROW_IF(!datatools::name_validation(cat_, nv_flags),
                std::logic_error,
                "Invalid category name '" << cat_ << "'!");
    _known_categories_.insert(cat_);
  }

  void urn_to_path_resolver_service::remove_known_category(const std::string & cat_)
  {
    _known_categories_.erase(cat_);
  }

  void urn_to_path_resolver_service::clear_known_categories()
  {
    _known_categories_.clear();
  }

  const std::set<std::string> & urn_to_path_resolver_service::get_known_categories() const
  {
    return _known_categories_;
  }

  bool urn_to_path_resolver_service::is_allow_overloading() const
  {
    return _allow_overloading_;
  }

  void urn_to_path_resolver_service::set_allow_overloading(bool a_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Service is already initialized!");
    _allow_overloading_ = a_;
    return;
  }

  void urn_to_path_resolver_service::add_map(const std::string & filename_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Service is already initialized!");
    _map_filenames_.push_back(filename_);
    return;
  }

  bool urn_to_path_resolver_service::is_initialized() const
  {
    return _initialized_;
  }

  int urn_to_path_resolver_service::initialize(const datatools::properties & config_,
                                               datatools::service_dict_type & /*services_*/)
  {
    DT_LOG_TRACE_ENTERING(get_logging_priority());

    DT_THROW_IF(is_initialized(), std::logic_error,
                "Service is already initialized!");
    base_service::_common_initialize(config_);

    if (datatools::logger::is_debug(get_logging_priority())) {
      config_.tree_dump(std::cerr, "Configuration for URN =>path resolver '" + get_name() + "' : ", "[debug] ");
    }

    if (config_.has_key("allow_overloading")) {
      DT_LOG_DEBUG(get_logging_priority(), "Parsing 'allow_overloading'...");
      bool ao = config_.fetch_boolean("allow_overloading");
      set_allow_overloading(ao);
    }

    if (config_.has_key("known_categories")) {
      DT_LOG_DEBUG(get_logging_priority(), "Parsing 'known_categories'...");
      std::set<std::string> kc;
      config_.fetch("known_categories", kc);
      for (auto c : kc) add_known_category(c);
    }

    std::vector<std::string> map_filenames;
    if (config_.has_key("maps")) {
      DT_LOG_DEBUG(get_logging_priority(), "Parsing 'maps'...");
      config_.fetch("maps", map_filenames);
    }
    for (const std::string & mapfile : map_filenames) {
      add_map(mapfile);
    }

    _init_();
    _initialized_ = true;

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

  void urn_to_path_resolver_service::_reset_()
  {
    _urn_lookup_table_.clear();
    _known_categories_.clear();
    _map_filenames_.clear();
    _allow_overloading_ = false;
    return;
  }

  int urn_to_path_resolver_service::reset()
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Service is not initialized!");
    kernel_pop();
    _initialized_ = false;
    _reset_();
    return datatools::SUCCESS;
  }

  void urn_to_path_resolver_service::tree_dump(std::ostream& out_,
                                               const std::string& a_title,
                                               const std::string& a_indent,
                                               bool a_inherit) const
  {
    this->base_service::tree_dump(out_, a_title, a_indent, true);

    out_ << a_indent << i_tree_dumpable::tag
          << "Allow overloading : "
          << std::boolalpha << _allow_overloading_ << std::endl;

    out_ << a_indent << i_tree_dumpable::tag
         << "Map filenames : " << _map_filenames_.size() << std::endl;

    out_ << a_indent << i_tree_dumpable::tag
         << "Known categories : " << _known_categories_.size() << std::endl;

    for (std::set<std::string>::const_iterator i = _known_categories_.begin();
         i != _known_categories_.end();
         i++ ) {
      out_ << a_indent << i_tree_dumpable::skip_tag;
      std::set<std::string>::const_iterator j = i;
      if (++j == _known_categories_.end()) {
        out_ << i_tree_dumpable::last_tag;
      } else {
        out_ << i_tree_dumpable::tag;
      }
      out_ << "Category '" << *i << "'" << std::endl;
    }

    out_ << a_indent << i_tree_dumpable::inherit_tag(a_inherit)
         << "URN/Path lookup table  : "
         << _urn_lookup_table_.size() << std::endl;

    return;
  }

  void urn_to_path_resolver_service::load_map(const std::string & map_filename_)
  {
    datatools::multi_properties mp("urn", "category");
    datatools::multi_properties::read_config(map_filename_, mp);
    std::vector<std::string> urns;
    mp.ordered_keys(urns);
    for (const std::string & urn : urns) {
      const datatools::multi_properties::entry & mpe = mp.get(urn);
      const std::string & category = mpe.get_meta();
      const datatools::properties & prop = mpe.get_properties();
      add_entry(urn, category, prop);
    }
    return;
  }

  void urn_to_path_resolver_service::_init_()
  {
    if (_known_categories_.size() == 0) {
      _known_categories_ = default_known_categories();
    }
    for (const std::string & mapfile : _map_filenames_) {
      std::string map_file = mapfile;
      datatools::fetch_path_with_env(map_file);
      load_map(map_file);
    }
    if (_urn_lookup_table_.size() == 0) {
      DT_LOG_WARNING(datatools::logger::PRIO_WARNING, "URN path lookup table for '" << get_name() << "' has no record!");
    }
    return;
  }

  void urn_to_path_resolver_service::add_entry(const std::string & urn_,
                                               const std::string & category_,
                                               const datatools::properties & infos_)
  {
    resolver_entry re;
    re.set_urn(urn_);
    re.set_category(category_);
    if (infos_.has_key("path")) {
      re.set_path(infos_.fetch_string("path"));
    }
    if (infos_.has_key("mime")) {
      re.set_mime(infos_.fetch_string("mime"));
    }
    add_entry(urn_, category_, re.get_path(), re.get_mime());
    return;
  }

  void urn_to_path_resolver_service::remove(const std::string & urn_)
  {
    _urn_lookup_table_.erase(urn_);
    return;
  }

  void urn_to_path_resolver_service::add_entry(const std::string & urn_,
                                               const std::string & category_,
                                               const std::string & path_,
                                               const std::string & mime_)
  {
    DT_THROW_IF(urn_.empty(), std::logic_error, "Missing URN!");
    {
      datatools::urn u;
      DT_THROW_IF(!u.from_string(urn_) || u.get_depth() < 2,
                  std::logic_error,
                  "Invalid URN!");
    }
    if (urn_is_known(urn_)) {
      if (!is_allow_overloading()) {
        DT_THROW(std::logic_error,
                 "Forbidden URN overloading for '" << urn_ << "'!");
      } else {
        DT_LOG_WARNING(get_logging_priority(), "Overloading URN '" << urn_ << "'!");
        _urn_lookup_table_.erase(urn_);
      }
    }
    DT_THROW_IF(_known_categories_.count(category_) == 0,
                std::logic_error,
                "Unsupported category '" << category_ << "'!");
    DT_THROW_IF(path_.empty(), std::logic_error, "Missing path!");
    resolver_entry re;
    re.set_urn(urn_);
    re.set_category(category_);
    re.set_path(path_);
    re.set_mime(mime_);
    _urn_lookup_table_[urn_] = re;
    return;
  }

  bool urn_to_path_resolver_service::urn_is_known(const std::string & urn_) const
  {
    return _urn_lookup_table_.count(urn_) == 1;
  }

  const urn_to_path &
  urn_to_path_resolver_service::get_entry(const std::string & urn_) const
  {
    urn_lookup_table_type::const_iterator found
      = _urn_lookup_table_.find(urn_);
    DT_THROW_IF(found == _urn_lookup_table_.end(), std::logic_error,
                "URN '" << urn_ << "' is not registered!");
    return found->second;
  }

  const std::string & urn_to_path_resolver_service::get_category(const std::string & urn_) const
  {
    return get_entry(urn_).get_category();
  }

  const std::string & urn_to_path_resolver_service::get_path(const std::string & urn_) const
  {
    return get_entry(urn_).get_path();
  }

  const std::string & urn_to_path_resolver_service::get_mime(const std::string & urn_) const
  {
    return get_entry(urn_).get_mime();
  }

  std::size_t urn_to_path_resolver_service::size() const
  {
    return _urn_lookup_table_.size();
  }

  const urn_to_path_resolver_service::urn_lookup_table_type &
  urn_to_path_resolver_service::get_urn_lookup_table() const
  {
    return _urn_lookup_table_;
  }

  void urn_to_path_resolver_service::print_lookup_table(std::ostream & out_,
                                                        const uint32_t /* flags_ */) const
  {
    for (const auto & uepair : _urn_lookup_table_) {
      out_ << uepair.first << " "
           << uepair.second.get_path() << " "
           << uepair.second.get_category() << " "
           << uepair.second.get_mime() << " "
           << std::endl;
    }
    return;
  }

  bool urn_to_path_resolver_service::is_kernel_pushed() const
  {
    if (datatools::kernel::is_instantiated()) {
      return datatools::kernel::instance().get_urn_query().has_path_resolver(*this);
    }
    return false;
  }

  void urn_to_path_resolver_service::kernel_push(const std::string & name_)
  {
    datatools::logger::priority logging = get_logging_priority();
    DT_LOG_TRACE_ENTERING(logging);
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Cannot register in the library's URN query service if not initialized!");
    if (datatools::kernel::is_instantiated()) {
      if (!datatools::kernel::instance().get_urn_query().has_path_resolver(*this)) {
         datatools::kernel::instance().grab_urn_query().add_path_resolver(*this, name_);
      }
    }
    DT_LOG_TRACE_EXITING(logging);
    return;
  }

  void urn_to_path_resolver_service::kernel_pop()
  {
    datatools::logger::priority logging = get_logging_priority();
    DT_LOG_TRACE_ENTERING(logging);
    DT_LOG_TRACE(logging, "Check for registration in kernel...");
    if (datatools::kernel::is_instantiated()) {
      if (datatools::kernel::instance().get_urn_query().has_path_resolver(*this)) {
        DT_LOG_TRACE(logging, "Removing from kernel...");
        datatools::kernel::instance().grab_urn_query().remove_path_resolver(*this);
        DT_LOG_TRACE(logging, "Done.");
      }
    }
    DT_LOG_TRACE_EXITING(logging);
    return;
  }

} // end of namespace datatools

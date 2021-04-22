// id_mgr.cc

// Ourselves:
#include <geomtools/id_mgr.h>

// Standard library:
#include <stdexcept>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <map>
#include <vector>

// Third party:
// - Boost:
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/multi_properties_config.h>
#include <datatools/utils.h>

namespace geomtools {

  // static
  const uint32_t id_mgr::WORD_TYPE;
  const uint32_t id_mgr::PLAIN_TYPE;

  // static
  const std::string & id_mgr::category_key_label()
  {
    static std::string token = "category";
    return token;
  }

  const std::string & id_mgr::type_meta_label()
  {
    static std::string token = "type";
    return token;
  }

  const std::string & id_mgr::default_world_category()
  {
    static std::string token = "world";
    return token;
  }

  const std::string & id_mgr::default_plain_category()
  {
    static std::string token = "plain";
    return token;
  }

  const std::string & id_mgr::plain_item_address()
  {
    static std::string token = "item";
    return token;
  }

  bool id_mgr::category_info::is_locked() const
  {
    return _locked_;
  }

  void id_mgr::category_info::lock()
  {
    _locked_ = true;
    return;
  }

  bool id_mgr::category_info::is_valid() const
  {
    return _type_ != geom_id::INVALID_TYPE;
  }

  bool id_mgr::category_info::has_group() const
  {
    return ! _group_.empty();
  }

  void id_mgr::category_info::set_group(const std::string & grp_)
  {
    _group_ = grp_;
    return;
  }

  const std::string & id_mgr::category_info::get_group() const
  {
    return _group_;
  }

  bool id_mgr::category_info::is_final_category() const
  {
    return _final_category_;
  }

  void id_mgr::category_info::set_final_category(bool fc_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Geometry category is locked!");
    _final_category_ = fc_;
    return;
  }

  bool id_mgr::category_info::has_subaddress(const std::string & a_label, int /*a_count*/) const
  {
    for (size_t i = 0; i < _addresses_.size (); i++) {
      if (_addresses_[i] == a_label) return true;
    }
    return false;
  }

  int id_mgr::category_info::get_subaddress_index(const std::string & a_label, int /*a_count*/) const
  {
    for (size_t i = 0; i < _addresses_.size (); i++) {
      if (_addresses_[i] == a_label) return i;
    }
    return -1;
  }

  void id_mgr::category_info::set_type(uint32_t t_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Geometry category is locked!");
    _type_ = t_;
    return;
  }

  void id_mgr::category_info::set_category(const std::string & cat_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Geometry category is locked!");
    _category_ = cat_;
    return;
  }

  const std::string & id_mgr::category_info::get_category() const
  {
    return _category_;
  }

  bool id_mgr::category_info::has_ancestor(const std::string & cat_) const
  {
    return (std::find (_ancestors_.begin (), _ancestors_.end(), cat_)
            != _ancestors_.end ());
  }

  void id_mgr::category_info::add_ancestor(const std::string & cat_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Geometry category is locked!");
    if (! has_ancestor(cat_)) {
      _ancestors_.push_back(cat_);
    }
    return;
  }

  const std::string & id_mgr::category_info::get_inherits() const
  {
    return _inherits_;
  }

  void id_mgr::category_info::set_inherits(const std::string & p_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Geometry category is locked!");
    _inherits_ = p_;
    return;
  }

  bool id_mgr::category_info::is_inherited() const
  {
    return ! _inherits_.empty ();
  }

  void id_mgr::category_info::set_extends(const std::string & p_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Geometry category is locked!");
    _extends_ = p_;
    return;
  }

  bool id_mgr::category_info::is_extension() const
  {
    return ! _extends_.empty ();
  }

  const std::string & id_mgr::category_info::get_extends() const
  {
    return _extends_;
  }

  uint32_t id_mgr::category_info::get_type() const
  {
    return _type_;
  }

  void id_mgr::category_info::add_address(const std::string & address_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Geometry category is locked!");
    _addresses_.push_back(address_);
    return;
  }

  void id_mgr::category_info::add_extends_by(const std::string & by_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Geometry category is locked!");
    _extends_by_.push_back(by_);
    return;
  }

  id_mgr::category_info::category_info()
  {
    _locked_ = false;
    _category_ = "";
    _type_ = geom_id::INVALID_TYPE;
    _final_category_ = false;
    _extends_ = "";
    _inherits_ = "";
    return;
  }

  id_mgr::category_info::~category_info()
  {
    reset();
    return;
  }

  void id_mgr::category_info::reset()
  {
    _locked_ = false;
    _ancestors_.clear();
    _extends_by_.clear();
    _addresses_.clear();
    _nbits_.clear();
    _category_ = "";
    _type_ = geom_id::INVALID_TYPE;
    _extends_ = "";
    _inherits_ = "";
    _final_category_ = false;
    _group_.clear();
    return;
  }

  size_t id_mgr::category_info::get_depth() const
  {
    return _addresses_.size ();
  }

  size_t id_mgr::category_info::get_by_depth() const
  {
    return _extends_by_.size ();
  }

  void id_mgr::category_info::set_ancestors(const std::vector<std::string> & a_)
  {
    _ancestors_ = a_;
    return;
  }

  const std::vector<std::string> & id_mgr::category_info::get_ancestors() const
  {
    return _ancestors_;
  }

  void id_mgr::category_info::set_extends_by(const std::vector<std::string> & e_)
  {
    _extends_by_ = e_;
    return;
  }

  const std::vector<std::string> & id_mgr::category_info::get_extends_by() const
  {
    return _extends_by_;
  }

  void id_mgr::category_info::set_addresses(const std::vector<std::string> & a_)
  {
    _addresses_ = a_;
    return;
  }

  const std::vector<std::string> & id_mgr::category_info::get_addresses() const
  {
    return _addresses_;
  }

  void id_mgr::category_info::print_tree(std::ostream & out_,
                                         const boost::property_tree::ptree & options_) const
  {
    datatools::i_tree_dumpable::base_print_options popts;
    popts.configure_from(options_);

    const std::string & indent = popts.indent;
    if (! popts.title.empty ()) {
      out_ << indent << popts.title << std::endl;
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Category  : '" << _category_ << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Type      : " << _type_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Group     : ";
    if (has_group()) {
      out_ << "'" << _group_ << "'";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Final     : " << _final_category_ << std::endl;

    if (! _inherits_.empty()) {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Inherits  : \"" << _inherits_ << "\"" << std::endl;
    }
    if (! _ancestors_.empty()) {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Ancestors ["
           <<  _ancestors_.size() << "] :";
      for (size_t i = 0; i < _ancestors_.size(); i++) {
        out_ << ' ' << '"' << _ancestors_[i] << '"';
      }
      out_ << std::endl;
    }

    if (! _extends_.empty()) {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Extends   : \"" << _extends_ << "\" by ["
           << _extends_by_.size() << "] : ";
      for (size_t i = 0; i < _extends_by_.size(); i++) {
        out_ << ' ' << '"' << _extends_by_[i] << '"';
      }
      out_ << std::endl;
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Addresses [" << get_depth() << "] :";
    for (size_t i = 0; i < _addresses_.size(); i++) {
      out_ << ' ' << '"' << _addresses_[i] << '"';
    }
    out_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::inherit_tag(popts.inherit)
         << "Locked    : " << is_locked() << std::endl;

    return;
  }

  void id_mgr::category_info::dump(std::ostream & out_) const
  {
    tree_dump(out_, "id_mgr::category_info: ");
    return;
  }

  void id_mgr::category_info::create(geom_id & id_) const
  {
    id_.reset();
    id_.make(_type_, get_depth());
    return;
  }

  /***************************************/

  void id_mgr::set_world_category(const std::string & n_)
  {
    _world_category_ = n_;
    return;
  }

  const std::string & id_mgr::get_world_category() const
  {
    return _world_category_;
  }

  void id_mgr::set_plain_category(const std::string & n_)
  {
    _plain_category_ = n_;
    return;
  }

  const std::string & id_mgr::get_plain_category() const
  {
    return _plain_category_;
  }

  datatools::logger::priority id_mgr::get_logging_priority () const
  {
    return _logging_priority_;
  }

  void id_mgr::set_logging_priority (datatools::logger::priority p)
  {
    _logging_priority_ = p;
    return;
  }

  bool id_mgr::is_debug () const
  {
    return datatools::logger::is_debug(_logging_priority_);
  }

  void id_mgr::set_debug (bool new_value_)
  {
    if (new_value_) {
      _logging_priority_ = datatools::logger::PRIO_DEBUG;
    } else {
      _logging_priority_ = datatools::logger::PRIO_FATAL;
    }
    return;
  }

  const id_mgr::categories_by_type_col_type & id_mgr::categories_by_type () const
  {
    return _categories_by_type_;
  }

  const id_mgr::categories_by_name_col_type & id_mgr::categories_by_name () const
  {
    return _categories_by_name_;
  }

  id_mgr::id_mgr ()
  {
    _logging_priority_ = datatools::logger::PRIO_FATAL;
    _initialized_ = false;
    _force_world_ = true;
    _force_plain_ = false;
    _world_category_ = default_world_category();
    _plain_category_ = default_plain_category();
    return;
  }

  id_mgr::~id_mgr ()
  {
    return;
  }

  bool id_mgr::is_initialized() const
  {
    return _initialized_;
  }

  void id_mgr::set_force_world(bool fw_)
  {
    _force_world_ = fw_;
    return;
  }

  bool id_mgr::is_force_world() const
  {
    return _force_world_;
  }

  void id_mgr::set_force_plain(bool fp_)
  {
    _force_plain_ = fp_;
    return;
  }

  bool id_mgr::is_force_plain() const
  {
    return _force_plain_;
  }

  void id_mgr::initialize(const datatools::properties & config_)
  {
    std::vector<std::string> categories_lists;

    if (config_.has_key("logging.priority")) {
      const std::string & log_label = config_.fetch_string("logging.priority");
      datatools::logger::priority p = datatools::logger::get_priority(log_label);
      DT_THROW_IF(p == datatools::logger::PRIO_UNDEFINED,
                  std::logic_error,
                  "Invalid logging priority '" << log_label << "'!");
      set_logging_priority(p);
    }

    if (config_.has_key("force_world")) {
      bool fw = config_.fetch_boolean("force_world");
      set_force_world(fw);
    }
    DT_LOG_DEBUG(get_logging_priority(), "force_world = " << _force_world_);

    if (config_.has_key("world_category")) {
      const std::string & wc = config_.fetch_string("world_category");
      set_world_category(wc);
    }
    DT_LOG_DEBUG(get_logging_priority(), "world_category = '" << _world_category_ << "'");

    if (config_.has_key("force_plain")) {
      bool fp = config_.fetch_boolean("force_plain");
      set_force_plain(fp);
    }
    DT_LOG_DEBUG(get_logging_priority(), "force_plain = " << _force_plain_);

    if (config_.has_key("plain_category")) {
      const std::string & pc = config_.fetch_string("plain_category");
      set_plain_category(pc);
    }
    DT_LOG_DEBUG(get_logging_priority(), "plain_category = '" << _plain_category_ << "'");

    if (config_.has_key("categories_list")) {
      const std::string & categories_list = config_.fetch_string("categories_list");
      _categories_lists_.push_back(categories_list);
    } else if (config_.has_key("categories_lists")) {
      config_.fetch("categories_lists", _categories_lists_);
    }

    _at_initialize();

    _initialized_ = true;
    return;
  }

  void id_mgr::_at_initialize()
  {
    for (size_t i(0); i < _categories_lists_.size(); ++i) {
      std::string categories_list = _categories_lists_[i];
      datatools::fetch_path_with_env(categories_list);
      load(categories_list);
    }

    // Check world category:
    if (!has_category_info(WORD_TYPE) && _force_world_) {
      category_info world_cat;
      world_cat.set_type(WORD_TYPE);
      world_cat.set_category(_world_category_);
      world_cat.add_address(_world_category_);
      _categories_by_name_[world_cat.get_category()] = world_cat;
      categories_by_name_col_type::const_iterator found
        = _categories_by_name_.find(world_cat.get_category());
      _categories_by_type_[world_cat.get_type()] = &(found->second);
    }

    // Check plain category:
    if (!has_category_info(PLAIN_TYPE) && _force_plain_) {
      category_info plain_cat;
      plain_cat.set_type(PLAIN_TYPE);
      plain_cat.set_category(_plain_category_);
      plain_cat.add_address(plain_item_address());
      plain_cat.set_final_category(true);
      _categories_by_name_[plain_cat.get_category()] = plain_cat;
      categories_by_name_col_type::const_iterator found
        = _categories_by_name_.find(plain_cat.get_category());
      _categories_by_type_[plain_cat.get_type()] = &(found->second);
    }

    return;
  }

  void id_mgr::reset()
  {
    _initialized_ = false;
    _categories_by_type_.clear ();
    _categories_by_name_.clear ();
    _world_category_   = default_world_category();
    _plain_category_   = default_plain_category();
    _logging_priority_ = datatools::logger::PRIO_FATAL;
    return;
  }

  void id_mgr::init_from(const datatools::multi_properties & mp_)
  {
    DT_THROW_IF(mp_.get_key_label() != id_mgr::category_key_label(),
                std::logic_error,
                "Key label '" << mp_.get_key_label() << "' is not valid !");

    DT_THROW_IF(mp_.get_meta_label() != id_mgr::type_meta_label(),
                std::logic_error,
                "Meta label '" << mp_.get_meta_label() << "' is not valid !");

    for (datatools::multi_properties::entries_ordered_col_type::const_iterator iter =
           mp_.ordered_entries().begin();
         iter != mp_.ordered_entries().end();
         iter++) {
      const datatools::multi_properties::entry & the_entry = *(*iter);
      DT_LOG_TRACE(get_logging_priority(), "Entry: ");
      if (get_logging_priority() >= datatools::logger::PRIO_TRACE) {
        the_entry.tree_dump(std::cerr);
      }

      const std::string & category = the_entry.get_key();
      const std::string & type_meta = the_entry.get_meta();
      const datatools::properties & props = the_entry.get_properties();

      DT_THROW_IF(_categories_by_name_.find(category) != _categories_by_name_.end(),
                  std::logic_error,
                  "Category '" << category << "' already exists !");

      DT_THROW_IF(type_meta.empty(),
                  std::logic_error,
                  "Missing '" << type_meta_label() << "' info !");

      int type = geom_id::INVALID_TYPE;
      {
        std::istringstream iss(type_meta);
        iss >> type;
        DT_THROW_IF(! iss,std::logic_error,"Invalid format for type '" << type_meta << "' !");
        DT_THROW_IF(type < 0,std::logic_error,"Invalid value for type '" << type << "' !");
      }

      DT_THROW_IF(_categories_by_type_.find(type) != _categories_by_type_.end(),
                  std::logic_error, "Type '" << type << "' already exists !");

      category_info cat_entry;
      cat_entry.set_category(category);
      cat_entry.set_type(type);

      if (props.has_flag("final")) {
        cat_entry.set_final_category(true);
      }

      if (props.has_key("group")) {
        cat_entry.set_group(props.fetch_string("group"));
      }

      std::vector<std::string> addresses_with_nbits;
      if (props.has_key("addresses")) {
        std::vector<std::string> addresses;
        props.fetch("addresses", addresses);
        cat_entry.set_addresses(addresses);
      } else {
        if (props.has_key("inherits")) {
          DT_LOG_TRACE(get_logging_priority(), "Inherits...");
          const std::string local_inherits = props.fetch_string("inherits");
          categories_by_name_col_type::const_iterator i_inherits
            = _categories_by_name_.find(local_inherits);
          DT_LOG_TRACE(get_logging_priority(), "inherits='" << local_inherits << "'");
          DT_THROW_IF(i_inherits == _categories_by_name_.end(),
                      std::logic_error,
                      "Category '" << local_inherits << "' does not exist !");
          const category_info & inherits_entry = i_inherits->second;
          DT_THROW_IF(inherits_entry.is_final_category(),
                      std::logic_error,
                      "Category '" << local_inherits << "' cannot be inherited!");
          cat_entry.set_inherits(local_inherits);
          cat_entry.set_addresses(inherits_entry.get_addresses());
          cat_entry.set_ancestors(inherits_entry.get_ancestors());
          cat_entry.add_ancestor(local_inherits);
        } else if (props.has_key("extends")) {
          const std::string extends = props.fetch_string("extends");
          DT_LOG_TRACE(get_logging_priority(), "extends category '" << extends << "'");
          categories_by_name_col_type::const_iterator i_extends
            = _categories_by_name_.find(extends);
          DT_THROW_IF(i_extends == _categories_by_name_.end(),
                      std::logic_error, "Category '" << extends << "' does not exist !");
          const category_info & extends_entry = i_extends->second;
          DT_LOG_TRACE(get_logging_priority(), "Found category: ");
          if (get_logging_priority() >= datatools::logger::PRIO_TRACE) {
            extends_entry.dump();
          }
          DT_THROW_IF(extends_entry.is_final_category(),
                      std::logic_error,
                      "Category '" << extends << "' cannot be extended!");
          cat_entry.set_addresses(extends_entry.get_addresses());
          cat_entry.set_ancestors(extends_entry.get_ancestors());
          cat_entry.add_ancestor(extends);
          DT_THROW_IF(! props.has_key("by"),
                      std::logic_error, "Missing 'by' address extension property !");
          cat_entry.set_extends(extends);
          std::vector<std::string> by;
          props.fetch("by", by);
          for (size_t i = 0; i < by.size(); i++) {
            cat_entry.add_extends_by(by[i]);
            cat_entry.add_address(by[i]);
          }
          DT_LOG_TRACE(get_logging_priority(), "Category entry: ");
          if (get_logging_priority() >= datatools::logger::PRIO_TRACE) {
            cat_entry.dump(std::cerr);
          }
        }
      }
      cat_entry.lock();
      _categories_by_name_[cat_entry.get_category()] = cat_entry;
      categories_by_name_col_type::const_iterator found
        = _categories_by_name_.find(cat_entry.get_category());
      _categories_by_type_[cat_entry.get_type()] = &(found->second);
    } // for
    return;
  }

  void id_mgr::load(const std::string & filename_)
  {
    datatools::multi_properties mp(category_key_label(), type_meta_label());
    uint32_t reader_opts = 0;
    reader_opts |= datatools::multi_properties_config::RESOLVE_PATH;
    datatools::multi_properties_config reader(reader_opts);
    reader.read(filename_, mp);
    init_from(mp);
    return;
  }

  void id_mgr::print_tree(std::ostream & out_,
                          const boost::property_tree::ptree & options_) const
  {
    datatools::i_tree_dumpable::base_print_options popts;
    popts.configure_from(options_);
    bool categories_list  = options_.get<bool>("list_categories", false);
    bool categories_full  = options_.get<bool>("full_categories", false);

    const std::string & indent = popts.indent;
    if (! popts.title.empty ())   out_ << indent << popts.title << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Logging priority  : " <<  datatools::logger::get_priority_label(_logging_priority_) << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "World category : '" << _world_category_ << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Plain category : '" << _plain_category_ << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Force world category : '" << _force_world_ << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Force plain category : '" << _force_plain_ << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::inherit_tag(popts.inherit)
         << "Categories      : ";
    if (_categories_by_name_.size () == 0) {
      out_ << "<empty>";
    } else {
      out_ << "[" << _categories_by_name_.size () << "]";
    }
    out_ << std::endl;
    if (categories_list) {
      for (categories_by_name_col_type::const_iterator i = _categories_by_name_.begin ();
           i != _categories_by_name_.end () ;
           i++) {
        const std::string & name = i->first;
        const category_info & a_entry = i->second;
        out_ << indent;
        std::ostringstream indent_oss;
        indent_oss << indent;
        categories_by_name_col_type::const_iterator j = i;
        j++;
        out_ << datatools::i_tree_dumpable::inherit_skip_tag(popts.inherit);
        indent_oss << datatools::i_tree_dumpable::inherit_skip_tag(popts.inherit);
        if (j == _categories_by_name_.end ()) {
          out_ << datatools::i_tree_dumpable::last_tag;
          indent_oss << datatools::i_tree_dumpable::inherit_skip_tag(popts.inherit);
        } else {
          out_ << datatools::i_tree_dumpable::tag;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
        }
        out_ << "Category : " << '"' << name << '"';
        out_ << std::endl;
        if (categories_full) {
          a_entry.tree_dump (out_, "", indent_oss.str ());
        }
      }
    }
    return;
  }

  bool id_mgr::has_category_info (int type_) const
  {
    return _categories_by_type_.find (type_) != _categories_by_type_.end ();
  }

  bool id_mgr::has_category_info (const std::string & name_) const
  {
    return _categories_by_name_.find (name_) != _categories_by_name_.end ();
  }

  const id_mgr::category_info &
  id_mgr::get_category_info (int type_) const
  {
    categories_by_type_col_type::const_iterator found
      = _categories_by_type_.find (type_);
    DT_THROW_IF (found == _categories_by_type_.end (), std::logic_error,
                 "Unregistered category with type '" << type_ << "' !");
    return *(found->second);
  }

  uint32_t id_mgr::get_category_type (const std::string & a_category) const
  {
    categories_by_name_col_type::const_iterator found
      = _categories_by_name_.find (a_category);
    DT_THROW_IF (found == _categories_by_name_.end (), std::logic_error,
                 "Unregistered category with name '" << a_category << "' !");
    return found->second.get_type ();
  }

  const id_mgr::category_info &
  id_mgr::get_category_info (const std::string & name_) const
  {
    categories_by_name_col_type::const_iterator found
      = _categories_by_name_.find (name_);
    DT_THROW_IF (found == _categories_by_name_.end (), std::logic_error,
                 "Unregistered category with name '" << name_ << "' !");
    return found->second;
  }

  bool id_mgr::check_inheritance (const geom_id & mother_id_,
                                  const geom_id & id_) const
  {
    categories_by_type_col_type::const_iterator found
      = _categories_by_type_.end ();

    // check geom ID:
    int type = id_.get_type ();
    found = _categories_by_type_.find (type);
    DT_THROW_IF (found == _categories_by_type_.end (), std::logic_error,
                 "No type '" << type << "' !");
    const category_info & ci = *(found->second);

    // check mother geom ID:
    int mother_type = mother_id_.get_type ();
    found = _categories_by_type_.find (mother_type);
    DT_THROW_IF (found == _categories_by_type_.end (), std::logic_error,
                 "No mother type '" << mother_type << "' !");
    const category_info & mci = *(found->second);
    if (ci.has_ancestor (mci.get_category ())) return true;
    return false;
  }

  void id_mgr::extract (const geom_id & child_id_, geom_id & mother_id_) const
  {
    if (! check_inheritance (mother_id_, child_id_)) {
      // if child ID does not inherit/extend the mother ID:
      mother_id_.reset_address ();
      return;
    }
    mother_id_.extract_from (child_id_);
    return;
  }

  void id_mgr::make_id (geom_id & id_,
                        uint32_t address_) const
  {
    id_.set (0, address_);
    return;
  }

  void id_mgr::make_id (const std::string & category_, geom_id & id_) const
  {
    id_.reset ();
    DT_THROW_IF (! has_category_info (category_), std::logic_error,
                 "No category '" << category_ << "' !");
    const category_info & ci = get_category_info (category_);
    id_.set_type (ci.get_type ());
    id_.set_depth (ci.get_depth ());
    return;
  }

  bool id_mgr::is_category (const geom_id & id_,
                            const std::string & category_) const
  {
    int id_type = id_.get_type ();
    const category_info & ci = get_category_info (id_type);
    return ci.get_category () == category_;
  }

  bool id_mgr::inherits (const geom_id & id_,
                         const std::string & category_) const
  {
    int id_type = id_.get_type ();
    const category_info & ci = get_category_info (id_type);
    return ci.has_ancestor (category_);
  }

  bool id_mgr::has (const geom_id & id_, const std::string & what_) const
  {
    int id_type = id_.get_type ();
    categories_by_type_col_type::const_iterator found =
      _categories_by_type_.find (id_type);
    const category_info & ci = *found->second;
    for (size_t j = 0; j < ci.get_addresses().size (); j++) {
      if (ci.get_addresses()[j] == what_) {
        return true;
      }
    }
    return false;
  }

  const std::string & id_mgr::get_category (const geom_id & id_) const
  {
    int id_type = id_.get_type ();
    categories_by_type_col_type::const_iterator found =
      _categories_by_type_.find (id_type);
    DT_THROW_IF (found == _categories_by_type_.end (), std::logic_error,
                 "No category associated to ID '" << id_ << "' !");
    const category_info & ci = *found->second;
    return ci.get_category();
  }


  int id_mgr::get (const geom_id & id_, const std::string & what_) const
  {
    int id_type = id_.get_type ();
    categories_by_type_col_type::const_iterator found =
      _categories_by_type_.find (id_type);
    const category_info & ci = *found->second;
    int i = -1;
    for (size_t j = 0; j < ci.get_addresses().size (); j++) {
      if (ci.get_addresses()[j] == what_) {
        i = j;
        break;
      }
    }
    if (i >= 0) {
      return id_.get (i);
    } else {
      DT_THROW_IF(true,std::logic_error, "Invalid address label '" << what_ << "' "
                  << "for category '" << ci.get_category() << "' !");
    }
    return geom_id::INVALID_ADDRESS;
  }

  void id_mgr::set (geom_id & id_, const std::string & what_, uint32_t value_) const
  {
    int id_type = id_.get_type ();
    categories_by_type_col_type::const_iterator found =
      _categories_by_type_.find (id_type);
    const category_info & ci = *found->second;
    int i = -1;
    for (size_t j = 0; j < ci.get_addresses().size (); j++) {
      DT_LOG_TRACE (get_logging_priority (), "what=" << what_);
      if (ci.get_addresses()[j] == what_) {
        i = j;
        DT_LOG_TRACE (get_logging_priority (), "break!");
        break;
      }
    }
    if (i >= 0) {
      return id_.set (i, value_);
    } else {
      DT_THROW_IF(true,std::logic_error, "Invalid address label '" << what_ << "' "
                  << "for category '" << ci.get_category() << "' !");
    }
    return;
  }

  std::string id_mgr::id_to_human_readable_format_string (const geom_id & id_) const
  {
    std::string s;
    id_to_human_readable_format_string (id_, s);
    return s;
  }

  void id_mgr::id_to_human_readable_format_string (const geom_id & id_, std::string & format_) const
  {
    std::ostringstream oss;
    if (! has_category_info (id_.get_type ())) {
      oss << "category=`" << geom_id::IO_TYPE_INVALID << "' : ";
      for (size_t i = 0; i < id_.get_depth (); i++) {
        oss << ' ' << "address[" << i << "]" << "=";
        if (id_.get (i) == geom_id::INVALID_ADDRESS) {
          oss << geom_id::IO_ADDRESS_INVALID;
        } else {
          oss << id_.get (i);
        }
      }
    } else {
      const category_info & ci = get_category_info (id_.get_type());
      oss << "category=`" << ci.get_category() << "' : ";
      for (size_t i = 0; i < id_.get_depth (); i++) {
        oss << ' ' << ci.get_addresses()[i] << "=";
        if (id_.get (i) == geom_id::INVALID_ADDRESS) {
          oss << geom_id::IO_ADDRESS_INVALID;
        } else {
          oss << id_.get (i);
        }
      }
    }
    format_ = oss.str ();
    return;
  }

  bool id_mgr::validate_id (const geom_id & id_) const
  {
    if (! has_category_info (id_.get_type ())) {
      return false;
    }
    const category_info & ci = get_category_info (id_.get_type ());
    if (id_.get_depth () != ci.get_depth ()) {
      return false;
    }
    return id_.is_valid ();
  }

  int id_mgr::compute_id_from_info (geom_id & id_,
                                    const geom_id & mother_id_,
                                    const std::string & id_info_,
                                    uint32_t nitem0_,
                                    uint32_t nitem1_,
                                    uint32_t nitem2_) const
  {
    std::vector<uint32_t> items_index;
    if (nitem0_ != geom_id::INVALID_ADDRESS) {
      items_index.push_back (nitem0_);
      if (nitem1_ != geom_id::INVALID_ADDRESS) {
        items_index.push_back (nitem1_);
        if (nitem2_ != geom_id::INVALID_ADDRESS) {
          items_index.push_back (nitem2_);
        }
      }
    }
    return compute_id_from_info (id_, mother_id_, id_info_, items_index);
  }

  int id_mgr::compute_id_from_info (geom_id & id_,
                                    const geom_id & mother_id_,
                                    const std::string & id_info_,
                                    const std::vector<uint32_t> & items_index_) const
  {
    // parse ID info:
    std::string id_info = id_info_;
    boost::trim (id_info);
    DT_LOG_TRACE (get_logging_priority (), "Entering...");
    DT_LOG_TRACE (get_logging_priority (), "ID info is `" << id_info << "'");
    DT_THROW_IF (id_info.size () < 5, std::logic_error,
                 "Invalid syntax for ID info `" << id_info_ << "' !");
    DT_THROW_IF (id_info[0] != '[', std::logic_error,
                 "Invalid syntax for ID info `" << id_info_ << "'; missing opening '[' character !");
    DT_THROW_IF (id_info[id_info.size () - 1 ] != ']', std::logic_error,
                 "Invalid syntax for ID info `"
                 << id_info_ << "'; missing closing ']' character !");
    /* Format :
     *
     *     id_info == "[core_info]"
     */
    std::string core_info = id_info.substr (1, id_info.size () - 2);
    DT_LOG_TRACE (get_logging_priority (), "Core info is `" << core_info << "'");

    typedef std::vector<std::string> split_vector_type;
    split_vector_type split_vec;
    boost::split (split_vec, core_info, boost::algorithm::is_any_of (":"));
    DT_THROW_IF (split_vec.size () < 1 || split_vec.size () > 2, std::logic_error,
                 "Invalid syntax for ID info `"
                 << id_info_ << "'; Invalid number of `:' separator !");

    /* Format :
     *
     *     core_info == "[category]"
     * or
     *     core_info == "[category:address rules...]"
     */

    // Parse the category:
    std::string category = split_vec[0];
    boost::trim (category);
    DT_THROW_IF (category.empty (), std::logic_error, "Missing category token !");
    DT_LOG_TRACE (get_logging_priority (), "Category is `" << category << "'");

    DT_THROW_IF (! has_category_info (category), std::logic_error,
                 "Unknown category `" << category << "' !");
    const category_info & ci = get_category_info (category);
    DT_LOG_TRACE (get_logging_priority (), "Category info:");
    if (get_logging_priority () >= datatools::logger::PRIO_TRACE) ci.tree_dump (std::cerr);
    ci.create (id_);
    DT_LOG_TRACE (get_logging_priority (), "Preliminary geometry ID is `" << id_ << "'");

    // Eventually import mother ID stuff:
    size_t current_address_index = 0;
    std::string mother_category;
    if (mother_id_.is_valid ()) {
      const category_info & mother_ci = get_category_info (mother_id_.get_type ());
      DT_LOG_TRACE (get_logging_priority (), "Mother ID `" << mother_id_ << "'");
      DT_LOG_TRACE (get_logging_priority (), "Mother category info:");
      if (get_logging_priority () >= datatools::logger::PRIO_TRACE) mother_ci.tree_dump (std::cerr);
      bool import_mother = false;
      if (ci.has_ancestor (mother_ci.get_category ())) {
        import_mother = true;
        mother_category = mother_ci.get_category ();
      }
      if (import_mother) {
        id_.inherits_from (mother_id_);
        current_address_index = mother_id_.get_depth ();
      }
    } // end of mother ID import.
    DT_LOG_TRACE (get_logging_priority (), "Preliminary geometry ID after mother import is `"
                  << id_ << "' (" << current_address_index << " addresses already set)");

    size_t number_of_remaining_addresses = id_.get_depth () - current_address_index;
    DT_LOG_TRACE (get_logging_priority (), "Number of remaining addresses = " << number_of_remaining_addresses);

    // Parse the address rules:
    bool parse_address = false;
    std::string addresses_token;
    if (split_vec.size() > 1) {
      addresses_token = split_vec[1];
      boost::trim (addresses_token);
      if (! addresses_token.empty ()) {
        parse_address = true;
      }
    }
    if (parse_address) {
      /* Format :
       *
       *     address rules == "address rule 0"
       * or
       *     address rules == "address rule 1, address rule 2..."
       */
      std::string local_addresses_token = split_vec[1];
      DT_LOG_TRACE (get_logging_priority (), "Addresses info is `" << local_addresses_token << "'");
      split_vector_type split_addr;
      boost::split (split_addr, local_addresses_token, boost::algorithm::is_any_of (","));
      if (split_addr.size () != number_of_remaining_addresses) {
        DT_LOG_FATAL (get_logging_priority (), "Category info:");
        if (get_logging_priority () >= datatools::logger::PRIO_FATAL) ci.tree_dump (std::cerr);
        DT_LOG_FATAL(get_logging_priority(),
                     "Number of remaining addresses = " << number_of_remaining_addresses);
        std::ostringstream message;
        message << "geomtools::id_mgr::compute_id_from_info: "
                << "Invalid number of remaining addresses info for category `"
                << category << "' from token `" << local_addresses_token << "' "
                << " with mother ID " << mother_id_ << " (mother=`" << mother_category << "') [";
        for (std::vector<uint32_t>::const_iterator ii = items_index_.begin ();
             ii !=  items_index_.end ();
             ii++) {
          message << " " << *ii;
        }
        message << "] for ID info `"
                << id_info_ << "' !";
        DT_THROW_IF(true, std::logic_error, message.str ());
      }
      // parse each address rule:
      for (size_t i = 0; i < split_addr.size (); i++) {
        /* Format :
         *
         *   address rule == "address=N"
         * or
         *   address rule == "address+N"
         * or
         *   address rule == "address-N"
         */
        std::string add_info = split_addr[i];
        boost::trim (add_info);
        split_vector_type split_add_info;
        boost::split (split_add_info, add_info, boost::algorithm::is_any_of ("=+-"));
        const int no_skip = 0;
        const int skip_plus = 1;
        const int skip_minus = -1;
        int skip = no_skip;
        if (add_info.find ('+') != add_info.npos) {
          skip = skip_plus;
        } else if (add_info.find ('-') != add_info.npos)  {
          skip = skip_minus;
        }

        DT_THROW_IF (split_add_info.size () != 2, std::logic_error,
                     "Invalid address information `" << add_info << "' for ID info `"
                     << id_info_ << "' !");
        std::string addr_label = split_add_info[0];
        DT_THROW_IF (ci.get_addresses()[i + current_address_index] != addr_label, std::logic_error,
                     "Invalid address label `" << addr_label << "' for category '"
                     << ci.get_category() << "' for ID info `"
                     << id_info_ << "' !");
        std::string addr_val_str = split_add_info[1];
        boost::trim (addr_val_str);
        uint32_t addr_val = 0;
        {
          std::istringstream iss (addr_val_str);
          iss >> addr_val;
          DT_THROW_IF (! iss, std::logic_error,
                       "Invalid address value `" << addr_val_str << "' for ID info `"
                       << id_info_ << "' !");
        }
        uint32_t address_value = addr_val;
        if (skip != no_skip){
          if (i < items_index_.size()) {
            if (skip == skip_plus) {
              address_value += items_index_[i];
            } else if (skip == skip_minus) {
              DT_THROW_IF (items_index_[i] > address_value, std::logic_error,
                           "Invalid address rule at index `" << i << "' for ID info `"
                           << id_info_ << "' ! Address index is negative !");
              address_value -= items_index_[i];
            }
          } else {
            DT_THROW_IF(true,std::logic_error, "Missing address at index `" << i << "' for ID info `"
                        << id_info_ << "' !");
          }
        }
        id_.set (i + current_address_index, address_value);
      }
    }

    return EXIT_SUCCESS;
  }

  int id_mgr::print_list_of_categories(const std::string & command_,
                                       const std::vector<std::string> & argv_,
                                       std::ostream & out_) const
  {
    bool with_title = true;
    bool with_type = true;
    std::vector<std::string> requested_patterns;
    size_t argcount = 0;
    while (argcount < argv_.size()) {
      const std::string & token = argv_[argcount++];
      if (token.empty()) break;
      if (token[0] == '-') {
        std::string option = token;
        if (option  == "-h" || option  == "--help") {
          out_ << "  Usage: \n";
          out_ << "    " << command_ << " [OPTIONS...]  \n"
               << "\n";
          out_ << "  Options: \n";
          out_ << "    -h | --help             Print this help\n"
               << "    -y | --with-type        Print type\n"
               << "    -Y | --without-type     Do not print type\n"
               << "    -t | --with-title       Print a title line\n"
               << "    -T | --without-title    Do not print a title line\n"
               << "    -p | --pattern PATTERN  Select categories matching pattern PATTERN\n"
               << "\n";
          out_ << std::flush;
          return -1;
        } else if (option == "--with-type" || option=="-y") {
          with_type = true;
        } else if (option == "--without-type" || option=="-Y") {
          with_type = false;
        } else if (option == "--with-title" || option=="-t") {
          with_title = true;
        } else if (option == "--without-title" || option=="-T") {
          with_title = false;
        } else if (option == "-p" || option == "--pattern") {
          std::string pattern = argv_[argcount++];
          datatools::remove_quotes(pattern);
          requested_patterns.push_back(pattern);
        } else {
          DT_LOG_ERROR(get_logging_priority(), "Invalid option '" << option << "' !");
          return -1;
        }
      } else {
        std::string argument = token;
        {
          DT_LOG_ERROR(get_logging_priority(), "Invalid argument '" << argument << "' !");
          return -1;
        }
      }
    } // while

    std::vector<const std::string*> selected_categories;
    for (categories_by_name_col_type::const_iterator i
           = _categories_by_name_.begin();
         i != _categories_by_name_.end();
         i++) {
      const std::string & category = i->first;
      if (requested_patterns.size() == 0) {
        selected_categories.push_back(&category);
      } else {
        bool selected = false;
        for (size_t j = 0; j < requested_patterns.size(); j++) {
          try {
            boost::regex expression(requested_patterns[j]);
            if (boost::regex_search(category.begin(), category.end(), expression)) {
              selected = true;
              break;
            }
          } catch (std::exception & error) {
            DT_LOG_ERROR(get_logging_priority(), "Regular expression error: " << error.what());
            return 1;
          }
        }
        if (selected) {
          selected_categories.push_back(&category);
        }
      }
    }
    unsigned int longest = 0;
    for (std::vector<const std::string*>::const_iterator i
           = selected_categories.begin();
         i != selected_categories.end();
         i++) {
      const std::string & category = **i;
      if (category.length() > longest) longest = category.length();
    }
    if (with_title) {
      out_ << std::flush << "List of available geometry categories : " << std::endl;
    }
    for (std::vector<const std::string*>::const_iterator i
           = selected_categories.begin();
         i != selected_categories.end();
         i++) {
      const std::string & category = **i;
      out_ << std::left << std::setw(longest + 1) << category;
      if (with_type) {
        out_ << " [type=" << _categories_by_name_.find(category)->second.get_type() << "] ";
      }
      // bool desc = false;
      // if (desc) {
      //        out_<< " : " << _categories_by_name_[category].second.get_description();
      // }
      out_ << std::endl;
    }
    return 0;
  }

} // end of namespace geomtools

/****************************************************************/
// OCD support for class '::geomtools::id_mgr' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::geomtools::id_mgr,ocd_)
{
  ocd_.set_class_name("geomtools::id_mgr");
  ocd_.set_class_library("geomtools");
  ocd_.set_class_description("A virtual geometry id_mgr");
  ocd_.set_class_documentation("A manager of geometry IDs in a virtual geometry");

  ocd_.set_configuration_hints("A geometry ID manager is configured through a configuration file     \n"
                               "that obeys the format of a 'datatools::multi_properties' setup file. \n"
                               "Each section describes a 'geometry category' and its relationship    \n"
                               "with respect to a geometry hierarchy. In the geomtools framework, it \n"
                               "is possible to associate any geometry volume in the virtual geometry \n"
                               "to an unique identifier : the geometry ID (GID). A geometry ID is    \n"
                               "defined through its 'type' (associated to the geometry category the  \n"
                               "volume belongs to) and its 'addresses' which consists in a list of   \n"
                               "integers that is built automatically by a 'mapping' object.          \n"
                               "The syntax uses the following format: ::                             \n"
                               "                                                                     \n"
                               "   [category=\"STRING\" type=\"POSITIVE INTEGER\"]                   \n"
                               "   PROPERTY DEFINITION 1                                             \n"
                               "   (PROPERTY DEFINITION 2)                                           \n"
                               "                                                                     \n"
                               "The 'category' key is an unique human friendly character string      \n"
                               "that identifies unambiguously a given geometry category. It is       \n"
                               "associated to a 'type' which is an unique positive integer.          \n"
                               "The 'type' of a geometry category is used within geometry ID objects \n"
                               "('geomtools::geom_id' class) for efficient storage and manipulation  \n"
                               "of GIDs.                                                             \n"
                               "                                                                   \n"
                               "Example : ::                                                       \n"
                               "                                                                   \n"
                               "   #@description List of geometry ID categories/types              \n"
                               "   #@key_label   \"category\"                                      \n"
                               "   #@meta_label  \"type\"                                          \n"
                               "                                                                   \n"
                               "   [category=\"world\" type=\"0\"]                                 \n"
                               "   addresses : string[1] = \"universe\"                            \n"
                               "                                                                   \n"
                               "   [category=\"experimental_area.gc\" type=\"100\"]                \n"
                               "   addresses : string[1] = \"hall\"                                \n"
                               "                                                                   \n"
                               "   [category=\"vacuum_chamber.gc\" type=\"200\"]                   \n"
                               "   addresses : string[1] = \"vessel\"                              \n"
                               "                                                                   \n"
                               "   [category=\"phoswich_detector.gc\" type=\"300\"]                \n"
                               "   addresses : string[1] = \"sensor\"                              \n"
                               "                                                                   \n"
                               "   [category=\"scintillator.gc\" type=\"310\"]                     \n"
                               "   extends : string = \"detector.gc\"                              \n"
                               "   by      : string[1] = \"plate\"                                 \n"
                               "                                                                   \n"
                               "   [category=\"light_guide.gc\" type=\"320\"]                      \n"
                               "   inherits : string = \"detector.gc\"                             \n"
                               "                                                                   \n"
                               "   [category=\"pmt.gc\" type=\"330\"]                              \n"
                               "   inherits : string = \"detector.gc\"                             \n"
                               "                                                                   \n"
                               "   [category=\"calibration_source.gc\" type=\"400\"]               \n"
                               "   inherits : string = \"vacuum_chamber.gc\"                       \n"
                               "                                                                   \n"
                               );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::geomtools::id_mgr,"geomtools::id_mgr")

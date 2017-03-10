// library_info.cc

// Ourselves:
#include <datatools/library_info.h>

// Standard Library:
#include <sstream>
#include <stdexcept>
#include <cstdlib>

// Third Party:
// - Boost:
#include <boost/scoped_ptr.hpp>

// This project:
#include <datatools/exception.h>
#include <datatools/properties.h>
#include <datatools/multi_properties.h>
#include <datatools/types.h>
#include <datatools/utils.h>

namespace datatools {

  // Constructor
  library_info::library_info()
  {
    _initialized_ = false;
    _logging_ = datatools::logger::PRIO_WARNING;
    _db_.set_meta_label("type");
    return;
  }

  // Destructor
  library_info::~library_info()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  // Clear
  void library_info::clear()
  {
    _db_.clear();
    return;
  }

  // Check the library info register
  bool library_info::is_initialized() {
    return _initialized_;
  }

  // Initialize the library info register
  void library_info::initialize()
  {
    // Protection:
    DT_THROW_IF (library_info::is_initialized(),
                 std::logic_error,
                 "Already initialized !");
    _initialized_ = true;
    return;
  }

  // Reset the library info register
  void library_info::reset()
  {
    // Protection:
    DT_THROW_IF (!library_info::is_initialized(),
                 std::logic_error,
                 "Not initialized !");
    _initialized_ = false;
    _db_aliases_.clear();
    _db_.reset();
    return;
  }

  const std::string &library_info::keys::lib_label()
  {
    static std::string key;
    if (key.empty()) key = "library";
    return key;
  }

  const std::string &library_info::keys::exec_label()
  {
    static std::string key;
    if (key.empty()) key = "executable";
    return key;
  }

  const std::string &library_info::keys::resource_label()
  {
    static std::string key;
    if (key.empty()) key = "resource";
    return key;
  }

  const std::string &library_info::keys::name()
  {
    static std::string key;
    if (key.empty()) key = "name";
    return key;
  }

  const std::string &library_info::keys::libname()
  {
    static std::string key;
    if (key.empty()) key = "libname";
    return key;
  }

  const std::string &library_info::keys::description()
  {
    static std::string key;
    if (key.empty()) key = "description";
    return key;
  }

  const std::string &library_info::keys::url()
  {
    static std::string key;
    if (key.empty()) key = "url";
    return key;
  }

  const std::string &library_info::keys::urn()
  {
    static std::string key;
    if (key.empty()) key = "urn";
    return key;
  }

  const std::string &library_info::keys::authors()
  {
    static std::string key;
    if (key.empty()) key = "authors";
    return key;
  }

  const std::string &library_info::keys::copyright()
  {
    static std::string key;
    if (key.empty()) key = "copyright";
    return key;
  }

  const std::string &library_info::keys::version()
  {
    static std::string key;
    if (key.empty()) key = "version";
    return key;
  }

  const std::string &library_info::keys::version_major()
  {
    static std::string key;
    if (key.empty()) key = "version.major";
    return key;
  }

  const std::string &library_info::keys::version_minor()
  {
    static std::string key;
    if (key.empty()) key = "version.minor";
    return key;
  }

  const std::string &library_info::keys::version_patch()
  {
    static std::string key;
    if (key.empty()) key = "version.patch";
    return key;
  }

  const std::string &library_info::keys::build_type()
  {
    static std::string key;
    if (key.empty()) key = "build_type";
    return key;
  }

  const std::string &library_info::keys::modules()
  {
    static std::string key;
    if (key.empty()) key = "modules";
    return key;
  }

  const std::string &library_info::keys::install_prefix()
  {
    static std::string key;
    if (key.empty()) key = "install.prefix";
    return key;
  }

  const std::string &library_info::keys::install_include_dir()
  {
    static std::string key;
    if (key.empty()) key = "install.include_dir";
    return key;
  }

  const std::string &library_info::keys::install_lib_dir()
  {
    static std::string key;
    if (key.empty()) key = "install.library_dir";
    return key;
  }

  const std::string &library_info::keys::install_bin_dir()
  {
    static std::string key;
    if (key.empty()) key = "install.bin_dir";
    return key;
  }

  const std::string &library_info::keys::install_data_dir()
  {
    static std::string key;
    if (key.empty()) key = "install.data_dir";
    return key;
  }

  const std::string &library_info::keys::install_resource_dir()
  {
    static std::string key;
    if (key.empty()) key = "install.resource_dir";
    return key;
  }

  const std::string &library_info::keys::install_plugin_lib_dir()
  {
    static std::string key;
    if (key.empty()) key = "install.plugin_library_dir";
    return key;
  }

  const std::string &library_info::keys::install_doc_dir()
  {
    static std::string key;
    if (key.empty()) key = "install.doc_dir";
    return key;
  }

  const std::string &library_info::keys::env_prefix()
  {
    static std::string key;
    if (key.empty()) key = "env.prefix";
    return key;
  }

  const std::string &library_info::keys::env_include_dir()
  {
    static std::string key;
    if (key.empty()) key = "env.include_dir";
    return key;
  }

  const std::string &library_info::keys::env_lib_dir()
  {
    static std::string key;
    if (key.empty()) key = "env.library_dir";
    return key;
  }

  const std::string &library_info::keys::env_bin_dir()
  {
    static std::string key;
    if (key.empty()) key = "env.bin_dir";
    return key;
  }

  const std::string &library_info::keys::env_resource_dir()
  {
    static std::string key;
    if (key.empty()) key = "env.resource_dir";
    return key;
  }

  const std::string &library_info::keys::env_plugin_lib_dir()
  {
    static std::string key;
    if (key.empty()) key = "env.plugin_library_dir";
    return key;
  }

  const std::string &library_info::keys::env_doc_dir()
  {
    static std::string key;
    if (key.empty()) key = "env.doc_dir";
    return key;
  }

  logger::priority library_info::get_logging() const
  {
    return _logging_;
  }


  void library_info::set_logging(logger::priority p)
  {
    _logging_ = p;
    return;
  }


  void library_info::names(std::vector<std::string> & v_) const
  {
    _db_.ordered_keys(v_);
    return;
  }

  void library_info::aliases(std::vector<std::string> & as_) const
  {
    as_.clear();
    for (const auto & ap : _db_aliases_) {
      as_.push_back(ap.first);
    }
    return;
  }

  void library_info::update(const std::string & library_name_,
                            const std::string & info_key_,
                            const std::string & info_value_,
                            const std::string & info_type_,
                            bool locked_)
  {
    DT_THROW_IF(library_name_.empty(),
                std::logic_error,
                "Missing library name !");
    DT_THROW_IF(info_key_.empty(),
                std::logic_error,
                "Missing info key name for library '"
                << library_name_ << "' !");
    DT_THROW_IF(info_value_.empty(),
                std::logic_error,
                "Missing value for info key '" << info_key_ << "' in library '"
                << library_name_ << "' !");
    std::string libname = resolve(library_name_);
    DT_THROW_IF(! has(libname),
                std::logic_error,
                "Library '" << libname << "' is not registered !");
    properties & lib_info = _db_.grab_section(libname);

    int t = TYPE_STRING;
    if (! info_type_.empty()) {
      t = get_type_from_label(info_type_);
      DT_THROW_IF(t == TYPE_NONE,
                  std::range_error,
                  "Invalid type '" << info_type_ << "' for key '"
                  << info_key_ << "' in library '" << libname << "' !");
    }

    if (t == TYPE_BOOLEAN) {
      bool value = false;
      std::istringstream iss(info_value_);
      std::string token;
      iss >> token;
      if (token == "0" || token == "false") {
        value = false;
      } else if (token == "1" || token == "true") {
        value = true;
      } else {
        DT_THROW_IF(true, std::runtime_error,
                    "Invalid boolean format '" << info_value_
                    << "' for  key '" << info_key_
                    << "' in library '" << libname << "' !");
      }
      lib_info.update_boolean(info_key_, value);
    } else if (t == TYPE_INTEGER) {
      int value = 0;
      std::istringstream iss(info_value_);
      iss >> value;
      DT_THROW_IF(!iss, std::runtime_error,
                  "Invalid integer number format '" << info_value_
                  << "' for  key '" << info_key_
                  << "' in library '" << libname << "' !");
      lib_info.update_integer(info_key_, value);
    } else if (t == TYPE_REAL) {
      double value = std::numeric_limits<double>::quiet_NaN();
      std::istringstream iss(info_value_);
      iss >> value;
      DT_THROW_IF(!iss, std::runtime_error,
                  "Invalid real number format '" << info_value_
                  << "' for  key '" << info_key_
                  << "' in library '" << libname << "' !");
      lib_info.update_real(info_key_, value);
    } else if (t == TYPE_STRING) {
      lib_info.update_string(info_key_, info_value_);
    }

    if (locked_) lib_info.key_lock(info_key_);
    return;
  }


  properties & library_info::registration(const std::string & library_name_,
                                          const std::string & library_desc_,
                                          const std::string & library_version_,
                                          const std::string & install_prefix_,
                                          const std::string & install_lib_path_,
                                          const std::string & install_resource_path_)
  {
    uint32_t libname_validation_flags = NV_NO_COLON | NV_NO_DOT | NV_NO_HYPHEN;
    DT_THROW_IF(!name_validation(library_name_, libname_validation_flags),
                std::logic_error,
                "Invalid library name '" << library_name_ << "'!");
    DT_THROW_IF(is_alias(library_name_),
                std::logic_error,
                "Name '" << library_name_ << "' is already used as an alias !");
    DT_THROW_IF(_db_.has_key(library_name_),
                std::logic_error,
                "Library '" << library_name_ << "' is already registered !");
    properties & lib_info = _db_.add_section(library_name_, keys::lib_label());
    {
      std::ostringstream desc;
      desc << "Runtime information store for library '" << library_name_ << "'";
      lib_info.set_description(desc.str());
    }
    update(library_name_,
           library_info::keys::name(),
           library_name_,
           "string",
           true);

    if (!library_desc_.empty()) {
      update(library_name_,
             library_info::keys::description(),
             library_desc_,
             "string",
             true);
    }

    if (!library_version_.empty()) {
      update(library_name_,
             library_info::keys::version(),
             library_version_,
             "string",
             true);
    }

    if (!install_prefix_.empty()) {
      update(library_name_,
             library_info::keys::install_prefix(),
             install_prefix_,
             "string",
             false);
    }

    if (!install_lib_path_.empty()) {
      update(library_name_,
             library_info::keys::install_lib_dir(),
             install_lib_path_,
             "string",
             false);
    }

    if (!install_resource_path_.empty()) {
      update(library_name_,
             library_info::keys::install_resource_dir(),
             install_resource_path_,
             "string",
             false);
    }

    if (this->get_logging() >= ::datatools::logger::PRIO_TRACE) {
      lib_info.tree_dump(std::cerr, "Library informations: ", "[trace] ");
    }

    DT_LOG_TRACE(this->get_logging(),
                 "Library information store for library '" << library_name_
                 << "' is now registered.");
    return lib_info;
  }


  std::string library_info::resolve(const std::string & library_key_,
                                    bool check_) const
  {
    std::string libname = library_key_;
    if (is_alias(libname)) {
      libname = _db_aliases_.find(library_key_)->second;
    }
    DT_THROW_IF(check_ && !_db_.has_key(libname), std::logic_error,
                "Library name '" << libname << "' does not exist!");
    return libname;
  }


  bool library_info::is_alias(const std::string & library_key_) const
  {
    DT_THROW_IF(library_key_.empty(),
                std::logic_error,
                "Missing library identifier !");
    return _db_aliases_.count(library_key_) > 0;
  }

  bool library_info::is_library(const std::string & library_name_) const
  {
    return _db_.has_key(library_name_);
  }

  bool library_info::has(const std::string & library_name_) const
  {
    DT_THROW_IF(library_name_.empty(),
                std::logic_error,
                "Missing library name !");
    if (_db_aliases_.count(library_name_)) {
      return true;
    }
    return _db_.has_key(library_name_);
  }


  void library_info::print(const std::string & library_key_,
                           std::ostream & out_) const
  {
    DT_THROW_IF(!has(library_key_),
                std::logic_error,
                "Library '" << library_key_ << "' is not registered !");
    const properties & lib_info = get(library_key_);
    std::ostringstream title_oss;
    title_oss << "Library information store for '" << library_key_ << "' : ";
    lib_info.tree_dump(out_,title_oss.str());
    return;
  }

  void library_info::add_alias(const std::string & library_alias_,
                               const std::string & library_name_)
  {
    static  const uint32_t libalias_validation_flags = NV_NO_COLON
      | NV_NO_DOT
      | NV_NO_HYPHEN;
    DT_THROW_IF(!name_validation(library_alias_, libalias_validation_flags),
                std::logic_error,
                "Invalid library alias '" << library_alias_ << "'!");
    DT_THROW_IF(!has(library_name_), std::logic_error,
                "Library name '" << library_name_ << "' does not exist!");
    DT_THROW_IF(has(library_alias_), std::logic_error,
                "Alias  '" << library_alias_ << "' is already used as a library name!");
    DT_THROW_IF(_db_aliases_.count(library_alias_) > 0,
                std::logic_error,
                "Alias  '" << library_alias_ << "' is already used!");
    _db_aliases_[library_alias_] = library_name_;
    return;
  }

  void library_info::remove_alias(const std::string & library_alias_)
  {
    if (_db_aliases_.count(library_alias_) > 0) {
      _db_aliases_.erase(library_alias_);
    }
    return;
  }

  void library_info::unregistration(const std::string & library_name_)
  {
    DT_THROW_IF(library_name_.empty(),
                std::logic_error,
                "Missing library name !");
    DT_THROW_IF(! _db_.has_key(library_name_),
                std::logic_error,
                "Library '" << library_name_ << "' is not registered !");
    // Build the list of aliases to be removed:
    std::set<std::string> removable_aliases;
    for (const auto & ap : _db_aliases_) {
      if (ap.second == library_name_) removable_aliases.insert(ap.first);
    }
    for (const std::string & alias : removable_aliases) {
      _db_aliases_.erase(alias);
    }
    // Remove the library entry:
    _db_.remove(library_name_);
    DT_LOG_TRACE(::datatools::library_info::get_logging(),
                 "Library information store for library '" << library_name_
                 << "' is now unregistered.");
    return;
  }


  void library_info::erase(const std::string & library_key_,
                           const std::string & info_key_)
  {
    DT_THROW_IF(info_key_.empty(),
                std::logic_error,
                "Missing info key name for library '" << library_key_ << "' !");
    DT_THROW_IF(!has(library_key_),
                std::logic_error,
                "Library '" << library_key_ << "' is not registered !");
    properties & lib_info = grab(library_key_);
    DT_THROW_IF(!lib_info.has_key(info_key_),
                std::logic_error,
                "Library '" << library_key_ << "' information has no key '"
                << info_key_ << "' !");
    lib_info.erase(info_key_);
    return;
  }


  properties & library_info::grab(const std::string & library_key_)
  {
    DT_THROW_IF(!has(library_key_),
                std::logic_error,
                "Library '" << library_key_ << "' is not registered !");
    properties & lib_info = _db_.grab_section(resolve(library_key_));
    return lib_info;
  }


  const properties & library_info::get(const std::string & library_key_) const
  {
    DT_THROW_IF(!has(library_key_),
                std::logic_error,
                "Library '" << library_key_ << "' is not registered !");
    const properties & lib_info = _db_.get_section(resolve(library_key_));
    return lib_info;
  }

  void library_info::tree_dump(std::ostream& out_,
                               const std::string& title_,
                               const std::string& indent_,
                               bool inherit_) const
  {
    std::string indent;
    if (!indent_.empty()) indent = indent_;
    if (!title_.empty()) {
      out_ << indent << title_ << std::endl;
    }
    out_ << indent << i_tree_dumpable::tag
         << "Initialized   : " << _initialized_ << std::endl;
    std::vector<std::string> libnames;
    datatools::library_info::names(libnames);

    out_ << indent << i_tree_dumpable::tag
         << "Registered libraries/components : " << libnames.size() << std::endl;
    for (size_t i = 0; i < libnames.size(); i++) {
      out_ << indent << i_tree_dumpable::skip_tag;
      if (i < libnames.size() - 1 ) {
        out_ << i_tree_dumpable::tag;
      } else {
        out_ << i_tree_dumpable::last_tag;
      }
      out_ << "Library: '" << libnames[i] << "'" << std::endl;
    }

    out_ << indent << i_tree_dumpable::tag
         << "Aliases : " << _db_aliases_.size() << std::endl;
    for (std::map<std::string,std::string>::const_iterator i = _db_aliases_.begin();
         i != _db_aliases_.end();
         i++) {
      std::map<std::string,std::string>::const_iterator j = i;
      out_ << indent << i_tree_dumpable::skip_tag;
      if (++j == _db_aliases_.end()) {
        out_ << i_tree_dumpable::last_tag;
      } else {
        out_ << i_tree_dumpable::tag;
      }
      out_ << "Alias: '" << i->first << "' -> Library: '" << i->second << "'" << std::endl;
    }

    {
      out_ << indent << i_tree_dumpable::tag
           << "DB   : " << std::endl;
      std::ostringstream indent2;
      indent2 << indent <<  i_tree_dumpable::skip_tag;
      _db_.tree_dump(out_, "", indent2.str());
    }

    out_ << indent << i_tree_dumpable::inherit_tag(inherit_)
         << "Logging   : '"
         << datatools::logger::get_priority_label(_logging_) << "'"
         << std::endl;
    return;
  }

} // end of namespace datatools

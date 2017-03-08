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
    DT_THROW_IF(! _db_.has_key(library_name_),
                std::logic_error,
                "Library '" << library_name_ << "' is not registered !");
    properties & lib_info = _db_.grab_section(library_name_);

    int t = TYPE_STRING;
    if (! info_type_.empty()) {
      t = get_type_from_label(info_type_);
      DT_THROW_IF(t == TYPE_NONE,
                  std::range_error,
                  "Invalid type '" << info_type_ << "' for key '"
                  << info_key_ << "' in library '" << library_name_ << "' !");
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
                    << "' in library '" << library_name_ << "' !");
      }
      lib_info.update_boolean(info_key_, value);
    } else if (t == TYPE_INTEGER) {
      int value = 0;
      std::istringstream iss(info_value_);
      iss >> value;
      DT_THROW_IF(!iss, std::runtime_error,
                  "Invalid integer number format '" << info_value_
                  << "' for  key '" << info_key_
                  << "' in library '" << library_name_ << "' !");
      lib_info.update_integer(info_key_, value);
    } else if (t == TYPE_REAL) {
      double value = std::numeric_limits<double>::quiet_NaN();
      std::istringstream iss(info_value_);
      iss >> value;
      DT_THROW_IF(!iss, std::runtime_error,
                  "Invalid real number format '" << info_value_
                  << "' for  key '" << info_key_
                  << "' in library '" << library_name_ << "' !");
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
    DT_THROW_IF(library_name_.empty(),
                std::logic_error,
                "Missing library name !");
    DT_THROW_IF(library_name_.find('.') != std::string::npos,
                std::logic_error,
                "Invalid library name (cannot contain a dot '.') !");
    DT_THROW_IF(library_name_.find(':') != std::string::npos,
                std::logic_error,
                "Invalid library name (cannot contain a semicolum ':') !");
    DT_THROW_IF(library_name_.find('@') != std::string::npos,
                std::logic_error,
                "Invalid library name (cannot contain '@') !");

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


  bool library_info::has(const std::string & library_name_) const
  {
    DT_THROW_IF(library_name_.empty(),
                std::logic_error,
                "Missing library name !");
     return _db_.has_key(library_name_);
  }


  void library_info::print(const std::string & library_name_,
                           std::ostream & out_) const
  {
    DT_THROW_IF(library_name_.empty(),
                std::logic_error,
                "Missing library name !");
    DT_THROW_IF(!_db_.has_key(library_name_),
                std::logic_error,
                "Library '" << library_name_ << "' is not registered !");
    const properties & lib_info = _db_.get_section(library_name_);
    std::ostringstream title_oss;
    title_oss << "Library information store for '" << library_name_ << "' : ";
    lib_info.tree_dump(out_,title_oss.str());
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
    _db_.remove(library_name_);
    DT_LOG_TRACE(::datatools::library_info::get_logging(),
                 "Library information store for library '" << library_name_
                 << "' is now unregistered.");
    return;
  }


  void library_info::erase(const std::string & library_name_,
                           const std::string & info_key_)
  {
    DT_THROW_IF(library_name_.empty(),
                std::logic_error,
                "Missing library name !");
    DT_THROW_IF(info_key_.empty(),
                std::logic_error,
                "Missing info key name for library '" << library_name_ << "' !");
    DT_THROW_IF(! _db_.has_key(library_name_),
                std::logic_error,
                "Library '" << library_name_ << "' is not registered !");
    properties & lib_info = _db_.grab_section(library_name_);
    DT_THROW_IF(!lib_info.has_key(info_key_),
                std::logic_error,
                "Library '" << library_name_ << "' information has no key '"
                << info_key_ << "' !");
    lib_info.erase(info_key_);
    return;
  }


  properties & library_info::grab(const std::string & library_name_)
  {
    DT_THROW_IF(library_name_.empty(),
                std::logic_error,
                "Missing library name !");
    DT_THROW_IF(!_db_.has_key(library_name_),
                std::logic_error,
                "Library '" << library_name_ << "' is not registered !");
    properties & lib_info = _db_.grab_section(library_name_);
    return lib_info;
  }


  const properties & library_info::get(const std::string & library_name_) const
  {
    DT_THROW_IF(library_name_.empty(),
                std::logic_error,
                "Missing library name !");
    DT_THROW_IF(!_db_.has_key(library_name_),
                std::logic_error,
                "Library '" << library_name_ << "' is not registered !");
    const properties & lib_info = _db_.get_section(library_name_);
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

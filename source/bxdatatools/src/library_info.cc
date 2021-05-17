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
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/path.hpp>

// This project:
#include <datatools/exception.h>
#include <datatools/properties.h>
#include <datatools/multi_properties.h>
#include <datatools/types.h>
#include <datatools/utils.h>

namespace datatools {

  // static
  const std::set<std::string> & library_info::topic_labels()
  {
    static std::set<std::string> _lbl;
    if (_lbl.size() == 0) {
      _lbl.insert(std::string("resources"));
      _lbl.insert(std::string("prefix"));
      _lbl.insert(std::string("libraries"));
      _lbl.insert(std::string("binaries"));
      _lbl.insert(std::string("plugins"));
      _lbl.insert(std::string("docs"));
      _lbl.insert(std::string("data"));
      _lbl.insert(std::string("includes"));
    }
    return _lbl;
  }

  // static
  std::string library_info::default_topic_label()
  {
    return "resources";
  }

  // static
  std::string library_info::topic_label_to_install_key(const std::string & topic_label_)
  {
    std::string topic_key;
    if (topic_label_ == "resources") {
      topic_key = datatools::library_info::keys::install_resource_dir();
    } else if (topic_label_ == "libraries") {
      topic_key = datatools::library_info::keys::install_lib_dir();
    } else if (topic_label_ == "binaries") {
      topic_key = datatools::library_info::keys::install_bin_dir();
    } else if (topic_label_ == "plugins") {
      topic_key = datatools::library_info::keys::install_plugin_lib_dir();
    } else if (topic_label_ == "docs") {
      topic_key = datatools::library_info::keys::install_doc_dir();
    } else if (topic_label_ == "prefix") {
      topic_key = datatools::library_info::keys::install_prefix();
    } else if (topic_label_ == "data") {
      topic_key = datatools::library_info::keys::install_data_dir();
    } else if (topic_label_ == "include") {
      topic_key = datatools::library_info::keys::install_include_dir();
    }
    return topic_key;
  }

  // static
  std::string library_info::topic_label_to_env_key(const std::string & topic_label_)
  {
    std::string topic_key;
    if (topic_label_ == "resources") {
      topic_key = datatools::library_info::keys::env_resource_dir();
    } else if (topic_label_ == "libraries") {
      topic_key = datatools::library_info::keys::env_lib_dir();
    } else if (topic_label_ == "binaries") {
      topic_key = datatools::library_info::keys::env_bin_dir();
    } else if (topic_label_ == "plugins") {
      topic_key = datatools::library_info::keys::env_plugin_lib_dir();
    } else if (topic_label_ == "docs") {
      topic_key = datatools::library_info::keys::env_doc_dir();
    } else if (topic_label_ == "prefix") {
      topic_key = datatools::library_info::keys::env_prefix();
    } else if (topic_label_ == "data") {
      topic_key = datatools::library_info::keys::env_data_dir();
    } else if (topic_label_ == "include") {
      topic_key = datatools::library_info::keys::env_include_dir();
    }
    return topic_key;
  }

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

  const std::string &library_info::keys::env_data_dir()
  {
    static std::string key;
    if (key.empty()) key = "env.data_dir";
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
    bool string_is_path = false;
    std::string info_type_value = info_type_;
    if (! info_type_value.empty()) {
      if (info_type_value == "path") {
        info_type_value = get_label_from_type(TYPE_STRING);
        string_is_path = true;
      }
      t = get_type_from_label(info_type_value);
      DT_THROW_IF(t == TYPE_NONE,
                  std::range_error,
                  "Invalid type '" << info_type_value << "' for key '"
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
      if (string_is_path) {
        lib_info.set_explicit_path(info_key_, true);
      }
    }
    if (locked_) lib_info.key_lock(info_key_);
    return;
  }

  bool library_info::resolve_path(const std::string & path_,
                                  std::string & resolved_path_,
                                  std::string & error_msg_) const
  {
    bool devel = false;
    if (devel) std::cerr << "[devel] ************** library_info::resolve_path : " << path_ << std::endl;
    bool failure = false;
    resolved_path_.clear();
    error_msg_.clear();
    if (path_.empty()) {
      error_msg_ = "Empty path";
      return failure;
    }
    if (path_[0] != '@') {
      error_msg_ = "Missing leading '@' symbol in '" + path_ + "'";
      return failure;
    }
    std::string path1 = path_.substr(1);
    size_t cpos = path1.find(':');
    if (cpos == path1.npos) {
      error_msg_ = "Invalid format; missing ':' separator in '" + path_ + "'";
      return failure;
    }
    std::string library_directive = path1.substr(0, cpos);
    if (devel) std::cerr << "[devel] library_directive = " << library_directive << std::endl;
    std::vector<std::string> lib_tokens;
    boost::split(lib_tokens, library_directive, boost::is_any_of("."));
    if (lib_tokens.size() == 0 || lib_tokens.size() > 2) {
      error_msg_ = "Invalid format; malformed library name/topic in '" + path_ + "'";
      return  failure;
    }
    std::string library_name = lib_tokens[0];
    if (devel) std::cerr << "  [devel] ************** library_name = " << library_name << std::endl;
    std::string library_topic = library_info::default_topic_label();
    if (lib_tokens.size() > 1) {
      library_topic = lib_tokens[1];
    }
    if (devel) std::cerr << "  [devel] ************** library_topic = " << library_topic << std::endl;
    if (!library_info::topic_labels().count(library_topic)) {
      error_msg_ = "Unsupported topic '" + library_topic + "' in '" + path_ + "'";
      return  failure;
    }
    std::string install_path_key = library_info::topic_label_to_install_key(library_topic);
    std::string environ_path_key = library_info::topic_label_to_env_key(library_topic);
    if (devel) {
      std::cerr << "  [devel] ************** install_path_key = " << install_path_key << std::endl;
      std::cerr << "  [devel] ************** environ_path_key = " << environ_path_key << std::endl;
    }
    
    if (!this->has(library_name)) {
      error_msg_ = "Unregistered library/component '" + library_name + "' in the kernel library register";
      if (devel) std::cerr << "  [devel] ************** error: " << error_msg_ << std::endl;
      return failure;
    }
    const datatools::properties & lib_infos = this->get(library_name);
    if (devel) {
      std::cerr << "  [devel] ************** lib_infos=" << std::hex << &lib_infos << std::endl;
      lib_infos.tree_dump(std::cerr, "Lib info: ", "  [devel] ");
    }
    
    // Search for the topic directory path from the register:
    std::string topic_dir_str;
    // Attempt 1: from the registered environment variable name (if any):
    if (topic_dir_str.empty() && !environ_path_key.empty()) {
      if ( lib_infos.has_key(environ_path_key)) {
        if (devel) std::cerr << "  [devel] ************** using environ_path_key = " << environ_path_key << std::endl;
        std::string env_topic_dir = lib_infos.fetch_string(environ_path_key);
        if (devel) std::cerr << "  [devel] ************** env_topic_dir = " << env_topic_dir << std::endl;
        if (! env_topic_dir.empty()) {
           if (devel) std::cerr << "  [devel] ************** Searching env_value ..." << std::endl;
          const char *env_value = getenv(env_topic_dir.c_str());
          if (env_value != 0) {
            // Found the 'env_topic_dir' environment variable:
            topic_dir_str = std::string(env_value);
            if (devel) std::cerr << "  [devel] ************** topic_dir_str=" << topic_dir_str << std::endl;
          }
        }
      } else {
        if (devel) std::cerr << "  [devel] ************** No environ_path_key" << std::endl;
      }
    }

    // Attempt 2: from the registered installation path (if any):
    if (topic_dir_str.empty() && !install_path_key.empty()) {
      if (lib_infos.has_key(install_path_key)) {
        topic_dir_str = lib_infos.fetch_string(install_path_key);
      }
    }

    if (topic_dir_str.empty()) {
      error_msg_ = "No '" + library_topic + "' directory for library '" + library_name
        + "' as been resolved from path '" + path_ + "'";
      return failure;
    }
    boost::filesystem::path topic_dir = topic_dir_str;
    boost::filesystem::path topic_relative_path = path1.substr(cpos+1);
    boost::filesystem::path topic_full_path = topic_dir / topic_relative_path;
    resolved_path_ = topic_full_path.string();
    return !failure;
  }

  bool library_info::parse_path_registration_directive(const std::string & directive_,
                                                       std::string & library_name_,
                                                       std::string & topic_,
                                                       std::string & path_,
                                                       std::string & error_msg_)
  {
    bool failure = false;
    library_name_.clear();
    topic_.clear();
    path_.clear();
    error_msg_.clear();
    // parse the rule:
    std::string path_registration = directive_;
    size_t apos = path_registration.find('@');
    if (apos == path_registration.npos) {
      error_msg_ = "Invalid syntax in mount point registration directive ('"
        + path_registration + "') !";
      return failure;
    }
    std::string the_libname = path_registration.substr(0, apos);
    std::string the_topic;
    {
      size_t dpos = the_libname.find('.');
      if (dpos != the_libname.npos) {
         the_topic = the_libname.substr(dpos+1);
         the_libname = the_libname.substr(0, dpos);
      }
    }
    if (the_libname.empty()) {
      error_msg_ = "Empty library name!";
      return failure;
    }
    if (! the_topic.empty()) {
      if (topic_labels().count(the_topic) == 0) {
        error_msg_ = "Unsupported library info topic '" + the_topic + "'!";
        return failure;
      }
    }
    std::string the_path = path_registration.substr(apos+1);
    if (the_path.empty()) {
      error_msg_ = "Empty path!";
      return failure;
    }
    library_name_ = the_libname;
    topic_ = the_topic;
    path_ = the_path;
    return !failure;
  }

  void library_info::path_unregistration(const std::string & library_name_,
                                         const std::string & topic_)
  {
    if (library_name_.empty()) {
      DT_THROW(std::logic_error, "Missing library name!");
    }
    if (topic_.empty()) {
      DT_THROW(std::logic_error, "Missing topic!");
    }
    if (topic_labels().count(topic_) == 0) {
      DT_THROW(std::logic_error, "Unsupported library info topic '" + topic_ + "'!");
    }
    std::string topic_install_key = topic_label_to_install_key(topic_);
    DT_THROW_IF(!this->has(library_name_), std::logic_error,
                "No library info record with name '" << library_name_ << "'!");
    datatools::properties & lib_infos = this->grab(library_name_);
    DT_THROW_IF(!lib_infos.has_key(topic_install_key), std::logic_error,
                "No topic '" << topic_ << "' associated to library info record '" << library_name_ << "'!");
    DT_THROW_IF(!lib_infos.is_explicit_path(topic_install_key), std::logic_error,
                "Topic '" << topic_ << "' is not associated to an explicit path in library info record '" << library_name_ << "'!");
    lib_infos.clean(topic_install_key);
    return;
  }

  void library_info::path_registration(const std::string & library_name_,
                                       const std::string & topic_,
                                       const std::string & path_,
                                       bool overwrite_)
  {
    if (library_name_.empty()) {
      DT_THROW(std::logic_error, "Missing library name!");
    }
    if (topic_.empty()) {
      DT_THROW(std::logic_error, "Missing topic!");
    }
    if (path_.empty()) {
      DT_THROW(std::logic_error, "Missing path!");
    }
    datatools::properties * ptr_lib_infos = nullptr;
    if (this->has(library_name_)) {
      // Grab the already registered library info record
      ptr_lib_infos = &grab(library_name_);
    } else {
      // Add a new library info record
      ptr_lib_infos = &this->registration(library_name_);
    }
    datatools::properties & lib_infos = *ptr_lib_infos;
    std::string topic_install_key = topic_label_to_install_key(topic_);
    if (lib_infos.has_key(topic_install_key)) {
      // Topic key already exists:
      const std::string & existing_path = lib_infos.fetch_path(topic_install_key);
      if (!overwrite_) {
        // Error, overwriting is forbidden !
        DT_THROW(std::logic_error,
                 "Cannot overwrite path '" << existing_path << "' in already defined topic ('"
                 << topic_ << "') with new path = '" << path_ << "' in library info '" << library_name_ << "'!");
      }
      lib_infos.clean(topic_install_key);
    }
    // Register the path
    lib_infos.store_path(topic_install_key, path_);
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
             "path",
             false);
    }

    if (!install_lib_path_.empty()) {
      update(library_name_,
             library_info::keys::install_lib_dir(),
             install_lib_path_,
             "path",
             false);
    }

    if (!install_resource_path_.empty()) {
      update(library_name_,
             library_info::keys::install_resource_dir(),
             install_resource_path_,
             "path",
             false);
    }

    if (this->get_logging() >= ::datatools::logger::PRIO_TRACE) {
      lib_info.tree_dump(std::clog, "Library informations: ", "[trace]: ");
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
         << "@Address      : " << std::hex << this << std::endl;
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

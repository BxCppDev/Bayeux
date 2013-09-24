// -*- mode: c++; -*-
/* library_info.cc
 */
// Ourselves
#include <datatools/library_info.h>

// Standard Library
#include <sstream>
#include <stdexcept>
#include <cstdlib>

// Third Party
// - Boost
#include <boost/scoped_ptr.hpp>

// Datatools
#include <datatools/exception.h>
#include <datatools/properties.h>
#include <datatools/multi_properties.h>
#include <datatools/types.h>


namespace {

  datatools::multi_properties & _get_db()
  {
    static boost::scoped_ptr<datatools::multi_properties> _db;
    if (!_db) {
      _db.reset(new datatools::multi_properties);
    }
    return *_db.get();
  }

  datatools::logger::priority & _logging()
  {
    static bool _initialized = false;
    static datatools::logger::priority _logging = datatools::logger::PRIO_WARNING;
    if (! _initialized) {
      char * c = getenv("DATATOOLS_LIBINFO_TRACE");
      if (c != NULL) {
        std::string dt_value = c;
        if (dt_value == "1") {
          _logging = datatools::logger::PRIO_TRACE;
        }
      }
      _initialized = true;
    }
    return _logging;
  }

}


namespace datatools {

  const std::string & library_info::keys::lib_label()
  {
    static std::string key;
    if (key.empty()) key = "library";
    return key;
  }

  const std::string & library_info::keys::exec_label()
  {
    static std::string key;
    if (key.empty()) key = "executable";
    return key;
  }

  const std::string & library_info::keys::resource_label()
  {
    static std::string key;
    if (key.empty()) key = "resource";
    return key;
  }

  const std::string & library_info::keys::name()
  {
    static std::string key;
    if (key.empty()) key = "name";
    return key;
  }

  const std::string & library_info::keys::libname()
  {
    static std::string key;
    if (key.empty()) key = "libname";
    return key;
  }

  const std::string & library_info::keys::description()
  {
    static std::string key;
    if (key.empty()) key = "description";
    return key;
  }

  const std::string & library_info::keys::url()
  {
    static std::string key;
    if (key.empty()) key = "url";
    return key;
  }

  const std::string & library_info::keys::authors()
  {
    static std::string key;
    if (key.empty()) key = "authors";
    return key;
  }

  const std::string & library_info::keys::copyright()
  {
    static std::string key;
    if (key.empty()) key = "copyright";
    return key;
  }

  const std::string & library_info::keys::version()
  {
    static std::string key;
    if (key.empty()) key = "version";
    return key;
  }

  const std::string & library_info::keys::version_major()
  {
    static std::string key;
    if (key.empty()) key = "version.major";
    return key;
  }

  const std::string & library_info::keys::version_minor()
  {
    static std::string key;
    if (key.empty()) key = "version.minor";
    return key;
  }

  const std::string & library_info::keys::version_patch()
  {
    static std::string key;
    if (key.empty()) key = "version.patch";
    return key;
  }

  const std::string & library_info::keys::build_type()
  {
    static std::string key;
    if (key.empty()) key = "build_type";
    return key;
  }

  const std::string & library_info::keys::modules()
  {
    static std::string key;
    if (key.empty()) key = "modules";
    return key;
  }

  const std::string & library_info::keys::install_prefix()
  {
    static std::string key;
    if (key.empty()) key = "install.prefix";
    return key;
  }

  const std::string & library_info::keys::install_include_dir()
  {
    static std::string key;
    if (key.empty()) key = "install.include_dir";
    return key;
  }

  const std::string & library_info::keys::install_lib_dir()
  {
    static std::string key;
    if (key.empty()) key = "install.library_dir";
    return key;
  }

  const std::string & library_info::keys::install_bin_dir()
  {
    static std::string key;
    if (key.empty()) key = "install.bin_dir";
    return key;
  }

  const std::string & library_info::keys::install_resource_dir()
  {
    static std::string key;
    if (key.empty()) key = "install.resource_dir";
    return key;
  }

  const std::string & library_info::keys::install_doc_dir()
  {
    static std::string key;
    if (key.empty()) key = "install.doc_dir";
    return key;
  }

  const std::string & library_info::keys::env_lib_dir()
  {
    static std::string key;
    if (key.empty()) key = "env.library_dir";
    return key;
  }

  const std::string & library_info::keys::env_resource_dir()
  {
    static std::string key;
    if (key.empty()) key = "env.resource_dir";
    return key;
  }


  logger::priority library_info::logging()
  {
    return _logging();
  }


  void library_info::logging(logger::priority p)
  {
    _logging() = p;
    return;
  }


  multi_properties & library_info::grab_db()
  {
    return _get_db();
  }


  const multi_properties & library_info::get_db()
  {
    return _get_db();
  }


  void library_info::names(std::vector<std::string> & v_)
  {
    get_db().ordered_keys(v_);
    return;
  }

  void library_info::status(std::ostream & out_)
  {
    out_ << "Library info register contents: " << std::endl;
    std::vector<std::string> libnames;
    datatools::library_info::names(libnames);
    for (int i = 0; i < libnames.size(); i++) {
      if (i < libnames.size() - 1 ) {
        out_ << "|-- ";
      } else {
        out_ << "`-- ";
      }
      out_ << "Library: '" << libnames[i] << "'" << std::endl;
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
                "Missing info key name for library '" << library_name_ << "' !");
    DT_THROW_IF(info_value_.empty(),
                std::logic_error,
                "Missing value for info key '" << info_key_ << "' in library '"
                << library_name_ << "' !");
    multi_properties & db = grab_db();
    DT_THROW_IF(! db.has_key(library_name_),
                std::logic_error,
                "Library '" << library_name_ << "' is not registered !");
    properties & lib_info = db.grab_section(library_name_);

    int t = TYPE_STRING;
    if (! info_type_.empty()) {
      int t = get_type_from_label(info_type_);
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
    multi_properties & db = grab_db();
    DT_THROW_IF(db.has_key(library_name_),
                std::logic_error,
                "Library '" << library_name_ << "' is already registered !");
    properties & lib_info = db.add_section(library_name_, keys::lib_label());
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

    update(library_name_,
           library_info::keys::version(),
           library_version_,
           "string",
           true);

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

    if (::datatools::library_info::logging() >= ::datatools::logger::PRIO_TRACE) {
      lib_info.tree_dump(std::cerr, "Library informations: ", "[trace] ");
    }

    DT_LOG_TRACE(::datatools::library_info::logging(),
                 "Library information store for library '" << library_name_
                 << "' is now registered.");
    return lib_info;
  }


  bool library_info::has(const std::string & library_name_)
  {
    DT_THROW_IF(library_name_.empty(),
                std::logic_error,
                "Missing library name !");
    const multi_properties & db = get_db();
    return db.has_key(library_name_);
  }


  void library_info::print(const std::string & library_name_,
                           std::ostream & out_)
  {
    DT_THROW_IF(library_name_.empty(),
                std::logic_error,
                "Missing library name !");
    multi_properties & db = grab_db();
    DT_THROW_IF(!db.has_key(library_name_),
                std::logic_error,
                "Library '" << library_name_ << "' is not registered !");
    const properties & lib_info = db.get_section(library_name_);
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
    multi_properties & db = grab_db();
    DT_THROW_IF(! db.has_key(library_name_),
                std::logic_error,
                "Library '" << library_name_ << "' is not registered !");
    db.remove(library_name_);
    DT_LOG_TRACE(::datatools::library_info::logging(),
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
    multi_properties & db = grab_db();
    DT_THROW_IF(! db.has_key(library_name_),
                std::logic_error,
                "Library '" << library_name_ << "' is not registered !");
    properties & lib_info = db.grab_section(library_name_);
    DT_THROW_IF(!lib_info.has_key(info_key_),
                std::logic_error,
                "Library '" << library_name_ << "' iinformation has no key '"
                << info_key_ << "' !");
    lib_info.erase(info_key_);
    return;
  }


  properties & library_info::grab(const std::string & library_name_)
  {
    DT_THROW_IF(library_name_.empty(),
                std::logic_error,
                "Missing library name !");
    multi_properties & db = grab_db();
    DT_THROW_IF(!db.has_key(library_name_),
                std::logic_error,
                "Library '" << library_name_ << "' is not registered !");
    properties & lib_info = db.grab_section(library_name_);
    return lib_info;
  }


  const properties & library_info::get(const std::string & library_name_)
  {
    DT_THROW_IF(library_name_.empty(),
                std::logic_error,
                "Missing library name !");
    multi_properties & db = grab_db();
    DT_THROW_IF(!db.has_key(library_name_),
                std::logic_error,
                "Library '" << library_name_ << "' is not registered !");
    const properties & lib_info = db.get_section(library_name_);
    return lib_info;
  }

} // end of namespace datatools

// end of library_info.cc

// -*- mode: c++; -*-
/* library_loader.cc
 */
// Ourselves
#include <datatools/library_loader.h>

// Standard Library
#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <vector>

// Third Party
// - Boost
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/scoped_ptr.hpp>

// This Project
// #if DATATOOLS_WITH_EMBEDDED_KWSYS == 0
// #include <kwsys/DynamicLoader.hxx>
// #define KWSYS_NAMESPACE kwsys
// #else
#include <datatools/detail/DynamicLoader.h>
//define KWSYS_NAMESPACE datatools_sys
// #endif

#include <datatools/utils.h>
#include <datatools/handle.h>
#include <datatools/multi_properties.h>

namespace datatools {


bool library_loader::g_devel = false;
bool library_loader::g_test  = false;

//----------------------------------------------------------------------
// library_entry struct
// ctor :
library_entry_type::library_entry_type(const std::string& lib_name,
                                       const std::string& lib_directory,
                                       const std::string& lib_filename,
                                       const std::string& lib_full_path,
                                       const std::string& lib_version,
                                       bool lib_autoload)
    : name(lib_name), directory(lib_directory), filename(lib_filename),
      full_path(lib_full_path), version(lib_version),
      autoload(lib_autoload), status(0), handle(0) {}

// dtor :
library_entry_type::~library_entry_type() {
  if (handle != 0) {
    int status = datatools::detail::DynamicLoader::CloseLibrary(handle);
    if (status != 1) {
      std::ostringstream message;
      message << "library_entry_type::dtor: The '"
              << name << "' library was not closed ! "
              << BOOST_PP_STRINGIZE(DATATOOLS_SYS_NAMESPACE) << " says: '"
              << datatools::detail::DynamicLoader::LastError() 
              << "' !";
      std::cerr << "ERROR: " << message.str() << std::endl;
    }
  }
  handle = 0;
}

void library_entry_type::print(std::ostream& out,
                               const std::string& indent) const {
  out << indent << "|-- Name      : " << name << std::endl;
  if (!directory.empty()) {
    out << indent << "|-- Directory : '" << directory << "'" << std::endl;
  }
  if (!filename.empty()) {
    out << indent << "|-- Filename  : '" << filename << "'" << std::endl;
  }
  out << indent << "|-- Full path : '" << full_path << "'" << std::endl;
  {
    std::string fpe = full_path;
    fetch_path_with_env(fpe);
    if (fpe != full_path) {
      out << indent << "|    `-- Expanded to : '" << fpe << "'" << std::endl;
    }
  }
  if (!version.empty()) {
    out << indent << "|-- Version : '" << version << "'" << std::endl;
  }
  out << indent << "|-- Autoload  : " << (autoload ? "Yes" : "No") << std::endl;
  out << indent << "`-- Loaded    : " << (handle ? "Yes" : "No");
  if (handle != 0) {
    out << " [" << handle << "]";
  }
  out << std::endl;
}


//----------------------------------------------------------------------
// library_loader class

bool library_loader::is_debug() const {
  return flags_ & debug;
}


void library_loader::set_debug(bool allow) {
  if (allow) {
    flags_ |= debug;
  } else {
    flags_ &= debug;
  }
}


bool library_loader::is_test() const {
  return flags_ & test;
}


void library_loader::set_allow_unregistered(bool allow) {
  if (allow) {
    flags_ |= allow_unregistered;
  } else {
    flags_ &= allow_unregistered;
  }
}


bool library_loader::allowing_unregistered() const {
  return flags_ & allow_unregistered;
}


symbol_ptr library_loader::get_symbol_address(const std::string& lib_name_,
                                              const std::string& symbol_) {
  // shorter typename?
  handle_library_entry_dict_type::const_iterator found = libraries_.find(lib_name_);
  if (found == libraries_.end()) {
    std::ostringstream message;
    message << "datatools::library_loader::get_symbol_address: "
            << "The shared library file '"
            << lib_name_ << "' is not registered !";
    throw std::logic_error(message.str());
  }

  if (found->second.get().handle == 0) {
    std::ostringstream message;
    message << "datatools::library_loader::get_symbol_address: "
            << "The shared library file '"
            << lib_name_ << "' is not loaded !";
    throw std::logic_error(message.str());
  }

  return datatools::detail::DynamicLoader::GetSymbolAddress(found->second.get().handle, symbol_.c_str());
}


// ctor :
library_loader::library_loader(uint32_t flags,
                               const std::string& config_file) {
  if (g_devel) {
    std::clog << "DEVEL: datatools::library_loader::ctor: Entering...\n";
  }
  
  flags_ = flags;
  if (library_loader::g_test) {
    flags_ |= test;
  }
  config_filename_ = config_file;
  this->init();
  if (g_devel) {
    std::clog << "DEVEL: datatools::library_loader::ctor: Exiting.\n";
  }
}


int library_loader::close_all() {
  while (!stacked_libraries_.empty()) {
    if (g_devel) {
      std::clog << "DEVEL: " 
                << "datatools::library_loader::close_all: "
                << "library_loader::close_all: LOOP ****************" 
                << std::endl;
    }

    handle_library_entry_type& hle = stacked_libraries_.front();
    if (!hle.has_data()) {
      if (g_devel) {
        std::clog << "DEVEL: " 
                  << "datatools::library_loader::close_all: "
                  << "library_loader::close_all: NO DATA ****************" 
                  << std::endl;
      }
      stacked_libraries_.pop_front(); // remove top element
    } else {
      if (g_devel) {
        std::clog << "DEVEL: " 
                  << "datatools::library_loader::close_all: "
                  << "library_loader::close_all: DATA *****************" 
                  << std::endl;
      }

      library_entry_type& le = hle.get();
      if (le.handle != 0) {
        if (this->is_debug()) {
          std::clog << "DEBUG: " 
                    << "datatools::library_loader::close_all: "
                    << "Closing library '" 
                    << le.name << "'..." << std::endl;
        }

        int status = datatools::detail::DynamicLoader::CloseLibrary(le.handle);
        if (status != 1) {
          std::ostringstream message;
          message << "datatools::library_loader::close_all: The '"
                  << le.name 
                  << "' library was not closed ! " 
                  << BOOST_PP_STRINGIZE(DATATOOLS_SYS_NAMESPACE) 
                  << " says: '"
                  << datatools::detail::DynamicLoader::LastError() 
                  << "' !";
          std::cerr << "ERROR: " << message.str() << std::endl;
          //return;
        }

        // 2011/09/19: F.M & X.G: Even if the return status
        // from DATATOOLS_SYS_NAMESPACE::DynamicLoader::CloseLibrary is not 0,
        // the library is closed by DATATOOLS_SYS_NAMESPACE, so we removed the
        // library entry from the library stack. Actually
        // the return status is really not explicit and can
        // not be used.

        // else
        //   {
        le.handle = 0;
        stacked_libraries_.pop_front (); // remove top element
        if (g_devel) {
          std::clog << "DEVEL: " 
                    << "datatools::library_loader::close_all: "
                    << "Stack size " 
                    << stacked_libraries_.size() << std::endl;
        }
        // }
        if (g_devel) {
          std::clog << "DEVEL: " 
                    << "datatools::library_loader::close_all: "
                    << "Removing library stacked entry '" 
                    << le.name << "'..."
                    << std::endl;
        }
      }
    }
  }
  return EXIT_SUCCESS;
}


// dtor :
library_loader::~library_loader() {
  if (g_devel) {
    std::clog << "DEVEL: datatools::library_loader::dtor: Entering...\n";
    this->print(std::cerr);
    std::clog << "DEVEL: datatools::library_loader::dtor: " << "close_all...\n";
  }
  
  this->close_all();
  
  if (g_devel) {
    std::clog << "DEVEL: datatools::library_loader::dtor: " 
              << "clear...\n";
  }
  
  libraries_.clear();
  
  if (g_devel) {
    std::clog << "DEVEL: datatools::library_loader::dtor: Exiting.\n";
  }
}


bool library_loader::has(const std::string& name) const {
  return libraries_.find(name) != libraries_.end();
}


bool library_loader::is_loaded(const std::string& name) const {
  handle_library_entry_dict_type::const_iterator found = libraries_.find(name);

  if (found != libraries_.end()) {
    if (found->second.get().handle != 0) {
      return true;
    }
  }
  return false;
}


int library_loader::registration(const std::string& lib_name_,
                                 const std::string& lib_directory_,
                                 const std::string& lib_filename_,
                                 const std::string& lib_full_path_,
                                 const std::string& lib_version_,
                                 bool           autoload_) {
  if (this->has(lib_name_)) {
    return EXIT_FAILURE;
  }

  handle_library_entry_type le_handle(new library_entry_type);
  libraries_[lib_name_] = le_handle;
  library_entry_type& le = libraries_[lib_name_].get();
  le.name         = lib_name_;
  le.filename     = lib_filename_;
  le.version      = lib_version_;
  le.directory    = lib_directory_;
  le.full_path    = lib_full_path_;

  if (le.filename.empty()) {
    std::ostringstream filename_ss;
    filename_ss << datatools::detail::DynamicLoader::LibPrefix();
    filename_ss << le.name;
    filename_ss << datatools::detail::DynamicLoader::LibExtension();

    if (!le.version.empty()) {
      filename_ss << le.version;
    }
    le.filename = filename_ss.str();
  }

  if (le.full_path.empty()) {
    std::ostringstream full_path_ss;
    if (!le.directory.empty()) {
      full_path_ss << le.directory;
      if (!boost::algorithm::ends_with(le.directory, "/")) {
        full_path_ss << '/';
      }
    }
    full_path_ss << le.filename;
    le.full_path = full_path_ss.str();
  }

  if (le.full_path.empty()) {
    std::ostringstream message;
    message << "datatools::library_loader::registration: The shared library file '"
            << le.name << "' has no valid path !";
    throw std::logic_error(message.str());
  }

  if (!le.directory.empty()) {
    // Only check the existence of the DLL file if a directory is
    // explicitely given :
    std::string check = le.full_path;
    fetch_path_with_env(check);
    if (!boost::filesystem::exists(check)) {
      std::ostringstream message;
      message << "datatools::library_loader::registration: The shared library file '"
              << le.name 
              << "' has no valid file '" 
              << le.full_path << "' !";
      throw std::logic_error(message.str());
    }
  }
  le.handle = 0;
  le.autoload = autoload_;
  return EXIT_SUCCESS;
}


int library_loader::load(const std::string& lib_name_,
                         const std::string& directory_,
                         const std::string& filename_,
                         const std::string& full_lib_path_,
                         const std::string& version_) {
  handle_library_entry_dict_type::iterator found = libraries_.find(lib_name_);
  if (found == libraries_.end()) {
    if (!this->allowing_unregistered()) {
      std::cerr << "ERROR: "
                << "datatools::library_loader::load: "
                << "Library '"
                << lib_name_ 
                << "' is not registered !" << std::endl;
      return EXIT_FAILURE;
    } else {
      int status = this->registration(lib_name_,
                                      directory_,
                                      filename_,
                                      full_lib_path_,
                                      version_,
                                      true);
      if (status != EXIT_SUCCESS) {
        std::ostringstream message;
        message << "datatools::library_loader::load: "
                << "Cannot register library '" 
                << lib_name_ << "' !";
        std::cerr << "ERROR: " << message.str() << std::endl;
        return EXIT_FAILURE;
      }
      found = libraries_.find(lib_name_);
    }
  }
  library_entry_type& le = found->second.get();
  std::string check = le.full_path;
  fetch_path_with_env(check);
  le.handle = datatools::detail::DynamicLoader::OpenLibrary(check.c_str());

  if (le.handle != 0) {
    stacked_libraries_.push_front(found->second);
  } else {
    std::ostringstream message;
    message << "datatools::library_loader::load: "
            << "The '" << lib_name_ << "' library was not loaded ! " 
            << BOOST_PP_STRINGIZE(DATATOOLS_SYS_NAMESPACE) << " says: '"
            << datatools::detail::DynamicLoader::LastError() << "' !";
    std::cerr << "ERROR: " << message.str() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}


int library_loader::close(const std::string& lib_name_) {
  if (!this->is_loaded(lib_name_)) {
    std::cerr << "ERROR: "
              << "datatools::library_loader::close: "
              << "No loaded library '" 
              << lib_name_ 
              << "' to be closed !" << std::endl;
    return EXIT_FAILURE;
  }

  if (stacked_libraries_.front().get().name != lib_name_) {
    std::cerr << "ERROR: "
              << "datatools::library_loader::close: "
              << "Cannot close library '"
              << lib_name_ << "' !" << std::endl;
    return EXIT_FAILURE;
  }

  handle_library_entry_dict_type::iterator found = libraries_.find(lib_name_);
  handle_library_entry_type& hle = found->second;
  library_entry_type& le = hle.get();

  int status = datatools::detail::DynamicLoader::CloseLibrary(le.handle);
  if (status != 1) {
    std::ostringstream message;
    message << "The '" << le.name << "' library was not closed ! " 
            << BOOST_PP_STRINGIZE(DATATOOLS_SYS_NAMESPACE) 
            << " says: '"
            << datatools::detail::DynamicLoader::LastError() << "' !";
    std::clog << "ERROR: " << message.str() << std::endl;
    return EXIT_FAILURE;
  } else {
    stacked_libraries_.front ().get().handle = 0;
    stacked_libraries_.pop_front();
  }
  return EXIT_SUCCESS;
}


void library_loader::init() {
  // checking test mode :
  {
    std::string env_name = "DATATOOLS_LIBRARY_LOADER_TEST";
    const char* env_check = getenv(env_name.c_str());
    if (env_check != 0) {
      std::string ec = env_check;
      if (ec == "1") {
        flags_ |= test;
      }
    }
  }

  // configuration file :
  if (config_filename_.empty()) {
    std::string env_name = "DATATOOLS_LIBRARY_LOADER_CONFIG";
    const char* env_check = getenv(env_name.c_str());
    if (env_check != 0) {
      config_filename_ = env_check;
    } else if (this->is_test()) {
      config_filename_ = "${DATATOOLS_ROOT}/resources/test/test_library_loader.conf";
    }
  }

  if (g_devel) {
    std::clog << "DEVEL: " << "datatools::library_loader::_init: "
              << "Using library loader config file = '"
              << config_filename_<< "' !" << std::endl;
  }

  if (config_filename_.empty()) {
    /*
    std::clog << "WARNING: " 
              << "datatools::library_loader::_init: "
              << "No library loader config file is available !"
              << std::endl;
    */
    return;
  }

  namespace du = datatools;
  fetch_path_with_env(config_filename_);
  du::multi_properties config("name", "filename");
  config.read(config_filename_);

  BOOST_FOREACH(const multi_properties::entry* ptr, 
                config.ordered_entries()) {
    const multi_properties::entry& e = *ptr;
    if (g_devel) {
      std::clog << "DEVEL: "
                << "datatools::library_loader::_init: "
                << "Settings for library '"
                << e.get_key() << "'..." << std::endl;
    }

    const properties& lib_properties = e.get_properties();
    std::string lib_name       = e.get_key ();
    std::string lib_filename   = e.get_meta ();
    std::string lib_directory  = "";
    std::string lib_full_path  = "";
    std::string lib_version    = "";
    bool lib_autoload   = false;

    if (lib_properties.has_key("full_path")) {
      lib_full_path = lib_properties.fetch_string("full_path");
    } else {
      if (lib_properties.has_key("directory")) {
        lib_directory = lib_properties.fetch_string("directory");
      }
      if (lib_properties.has_key("filename")) {
        lib_filename = lib_properties.fetch_string("filename");
      }
    }

    if (lib_properties.has_flag ("autoload")) {
      lib_autoload = lib_properties.fetch_boolean("autoload");
    }

    if (lib_properties.has_key ("version")) {
      lib_version = lib_properties.fetch_string("version");
    }

    if (g_devel) {
      std::clog << "DEVEL: "
                << "datatools::library_loader::_init: "
                << "Registration of library='" << lib_name 
                << "'..." << std::endl;
    }

    int status = this->registration(lib_name, lib_directory,
                                    lib_filename, lib_full_path,
                                    lib_version, lib_autoload);
    if (status != EXIT_SUCCESS) {
      std::ostringstream message;
      message << "library_entry_type::load: "
              << "Cannot register library '" 
              << lib_name << "' !";
      throw std::logic_error(message.str());
    }

    if (lib_autoload) {
      std::clog << "NOTICE: "
                << "datatools::library_loader::_init: "
                << "Automatic loading of library  '" << lib_name << "'..."
                << std::endl;
      int load_status = this->load(lib_name);
      if (load_status != EXIT_SUCCESS) {
        std::ostringstream message;
        message << "datatools::library_loader::_init: "
                << "Automatic loading of library='"
                << lib_name << "' failed !";
        throw std::logic_error(message.str());
      }
    }
    continue;
  } // BOOST_FOREACH

  if (this->is_debug()) this->print(std::clog);
}


void library_loader::print(std::ostream& out) const {
  out << "Library loader : " << std::endl;
  out << "Flags              : " << flags_ << std::endl;
  out << "Configuration file : '" << config_filename_ << "'" << std::endl;
  out << "List of registered shared libraries :" << std::endl;
  for (handle_library_entry_dict_type::const_iterator i
       = libraries_.begin();
       i != libraries_.end();
       ++i) {
    handle_library_entry_dict_type::const_iterator j = i;
    j++;
    const library_entry_type& le = i->second.get();
    std::string tag = "|   ";
    if (j == libraries_.end()) {
      out << "`-- ";
      tag = "    ";
    } else {
      out << "|-- ";
    }
    out << "Library : '" << i->first << "'" << std::endl;
    le.print(out, tag);
  }
  out << "List of loaded shared libraries :" << std::endl;
  for (handle_library_entry_stack_type::const_iterator i = stacked_libraries_.begin();
       i != stacked_libraries_.end();
       ++i) {
    handle_library_entry_stack_type::const_iterator j = i;
    j++;
    const library_entry_type& le = i->get();
    std::string tag = "|   ";
    if (j == stacked_libraries_.end()) {
      out << "`-- ";
      tag = "    ";
    } else {
      out << "|-- ";
    }
    out << "Library : '" << le.name << "'" << std::endl;
  }
}


} // end of namespace datatools


/***************
 * OCD support *
 ***************/

#include <datatools/ocd_macros.h>

// OCD support for class '::datatools::library_loader' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::datatools::library_loader,ocd_)
{
  ocd_.set_class_name ("datatools::library_loader");
  ocd_.set_class_description ("A shared library (DLL) loader");
  ocd_.set_class_library ("datatools");
  ocd_.set_class_documentation ("A library loader object is responsible of the dynamic loading\n"
                                "of shared libraries (DLL).                                   \n"
                                );
 
  ocd_.set_configuration_hints ("A library loader object may load directives from a file that  \n"
                                "uses the format of the 'datatools::multi_properties' object.  \n"
                                "Each section corresponds to the loading rules of a specific   \n"
                                "DLL.                                                          \n"
                                "Example :                                                     \n"
                                " |                                                            \n"
                                " | #@key_label   \"name\"                                     \n"
                                " | #@meta_label  \"filename\"                                 \n"
                                " |                                                            \n"
                                " | # A DLL with fully specified path :                        \n"
                                " | [name=\"z\" filename=\"\"]                                 \n"
                                " | #@config The zlib library                                  \n"
                                " | #@description The full path of the shared library file     \n"
                                " | full_path : string = \"/usr/lib/libz.so\"                  \n"
                                " |                                                            \n"
                                " |                                                            \n"
                                " | # Another DLL with fully specified path :                  \n"
                                " | [name=\"crypto\" filename=\"\"]                            \n"
                                " | #@config The crypto library                                \n"
                                " | #@description The full path of the shared library file     \n"
                                " | full_path : string = \"/usr/lib/libcrypto.so\"             \n"
                                " |                                                            \n"
                                " |                                                            \n"
                                " | # Yet another DLL with implicit path from LD_LIBRARY_PATH :\n"
                                " | [name=\"ssl\" filename=\"\"]                               \n"
                                " | #@config The libssl library                                \n"
                                " |                                                            \n"
                                " |                                                            \n"
                                " | # A DLL with explicit directory and filename :             \n"
                                " | [name=\"gsl\" filename=\"\"]                               \n"
                                " | #@config The GSL library                                   \n"
                                " | #@description The directory where the shared library file is stored \n"
                                " | directory : string = \"/usr/lib\"                          \n"
                                " |                                                            \n"
                                " | #@description The shared library file                      \n"
                                " | filename : string = \"libgsl.so\"                          \n"
                                " |                                                            \n"
                                " |                                                            \n"
                                " | # A DLL with explicit filename and autoload at startup :   \n"
                                " | [name=\"genvtx\" filename=\"libgenvtx.so\"]                \n"
                                " | #@config The genvtx library                                \n"
                                " | #@description Automatic loading at startup                 \n"
                                " | autoload : boolean = 1                                     \n"
                                " |                                                            \n"
                                " |                                                            \n"
                                " | # A DLL with implicit path :                               \n"
                                " | [name=\"foo\" filename=\"\"]                               \n"
                                " | #@config The foo library                                   \n"
                                " |                                                            \n"
                                );

  ocd_.set_validation_support(false);
  ocd_.lock(); 
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()

DOCD_CLASS_SYSTEM_REGISTRATION(datatools::library_loader,"datatools::library_loader")

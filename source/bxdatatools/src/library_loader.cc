// -*- mode: c++; -*-
/* library_loader.cc
 */

#include <datatools/utils/library_loader.h>

#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/foreach.hpp>

// #if DATATOOLS_WITH_EMBEDDED_KWSYS == 0
// #include <kwsys/DynamicLoader.hxx>
// #define KWSYS_NAMESPACE kwsys
// #else
#include <datatools_sys/DynamicLoader.hxx>
#//define KWSYS_NAMESPACE datatools_sys
// #endif

#include <datatools/utils/utils.h>
#include <datatools/utils/handle.h>
#include <datatools/utils/multi_properties.h>

namespace datatools {

  namespace utils {

    using namespace std;

    bool library_loader::g_devel = false;
    bool library_loader::g_test  = false;

    /***********************
     *  library_entry_type *
     ***********************/

    // ctor :
    library_entry_type::library_entry_type (const string & name_,
                                            const string & directory_,
                                            const string & filename_,
                                            const string & full_path_,
                                            const string & version_,
                                            bool           autoload_)
    {
      name      = name_;
      directory = directory_;
      filename  = filename_;
      full_path = full_path_;
      version   = version_;
      autoload  = autoload_;
      handle    = 0;
      status    = 0;
      return;
    }

    // dtor :
    library_entry_type::~library_entry_type ()
    {
      if (handle != 0)
        {
          int status = DATATOOLS_SYS_NAMESPACE::DynamicLoader::CloseLibrary (handle);
          if (status != 1)
            {
              ostringstream message;
              message << "library_entry_type::dtor: The '"
                      << name << "' library was not closed ! "
                      << "DATATOOLS_SYS_NAMESPACE" << " says: '"
                      << DATATOOLS_SYS_NAMESPACE::DynamicLoader::LastError () << "' !";
              cerr << "ERROR: " << message.str () << endl;
            }
        }
      handle = 0;
      return;
    }

    void library_entry_type::print (ostream      & out_,
                                    const string & indent_) const
    {
      out_ << indent_ << "|-- Name      : " << name << endl;
      if (! directory.empty ())
        {
          out_ << indent_ << "|-- Directory : '" << directory << "'" << endl;
        }
      if (! filename.empty ())
        {
          out_ << indent_ << "|-- Filename  : '" << filename << "'" << endl;
        }
      out_ << indent_ << "|-- Full path : '" << full_path << "'" << endl;
      {
        string fpe = full_path;
        fetch_path_with_env (fpe);
        if (fpe != full_path)
          {
            out_ << indent_ << "|    `-- Expanded to : '" << fpe << "'" << endl;
          }
      }
      if (! version.empty ())
        {
          out_ << indent_ << "|-- Version : '" << version << "'" << endl;
        }
      out_ << indent_ << "|-- Autoload  : " << (autoload ? "Yes": "No") << endl;
      out_ << indent_ << "`-- Loaded    : " << (handle ? "Yes": "No");
      if (handle != 0)
        {
          out_ << " [" << handle << "]";
        }
      out_ << endl;
      return;
    }

    /*******************
     *  library_loader *
     *******************/

    bool library_loader::is_debug () const
    {
      return _flags_ & debug;
    }

    void library_loader::set_debug (bool debug_)
    {
      if (debug_)
        {
          _flags_ |= debug;
        }
      else
        {
          _flags_ &= debug;
        }
      return;
    }

    bool library_loader::is_test () const
    {
      return _flags_ & test;
    }

    void library_loader::set_allow_unregistered (bool allow_unregistered_)
    {
      if (allow_unregistered_)
        {
          _flags_ |= allow_unregistered;
        }
      else
        {
          _flags_ &= allow_unregistered;
        }
      return;
    }

    bool library_loader::allowing_unregistered () const
    {
      return _flags_ & allow_unregistered;
    }

    symbol_ptr library_loader::get_symbol_address (const string & lib_name_,
                                                   const string & symbol_)
    {
      /*
        if (! is_loaded (lib_name_))
        {
        ostringstream message;
        message << "datatools::utils::library_loader::get_symbol_address: "
        << "The shared library file '"
        << lib_name_ << "' is not loaded !";
        throw logic_error (message.str ());
        }
      */
      handle_library_entry_dict_type::const_iterator found = _libraries_.find (lib_name_);
      if (found == _libraries_.end ())
        {
          ostringstream message;
          message << "datatools::utils::library_loader::get_symbol_address: "
                  << "The shared library file '"
                  << lib_name_ << "' is not registered !";
          throw logic_error (message.str ());
        }
      if (found->second.get ().handle == 0)
        {
          ostringstream message;
          message << "datatools::utils::library_loader::get_symbol_address: "
                  << "The shared library file '"
                  << lib_name_ << "' is not loaded !";
          throw logic_error (message.str ());
        }
      symbol_ptr the_symptr = DATATOOLS_SYS_NAMESPACE::DynamicLoader::GetSymbolAddress (found->second.get ().handle, symbol_.c_str());
      return the_symptr;
    }

    // ctor :
    library_loader::library_loader (uint32_t flags_,
                                    const string & dll_config_file_)
    {
      if (g_devel) clog << "DEVEL: datatools::utils::library_loader::ctor: Entering...\n";
      _flags_ = flags_;
      if (library_loader::g_test)
        {
          _flags_ |= test;
        }
      _config_filename_ = dll_config_file_;
      _init ();
      if (g_devel) clog << "DEVEL: datatools::utils::library_loader::ctor: Exiting.\n";
      return;
    }

    int library_loader::close_all ()
    {
      while (! _stacked_libraries_.empty ())
        {
          if (g_devel)
            {
              clog << "DEVEL: " << "datatools::utils::library_loader::close_all: "
                   << "library_loader::close_all: LOOP ****************" << endl;
            }
          handle_library_entry_type & hle = _stacked_libraries_.front ();
          if (! hle.has_data ())
            {
              if (g_devel)
                {
                  clog << "DEVEL: " << "datatools::utils::library_loader::close_all: "
                       << "library_loader::close_all: NO DATA ****************" << endl;
                }
              _stacked_libraries_.pop_front (); // remove top element
            }
          else
            {
              if (g_devel)
                {
                  clog << "DEVEL: " << "datatools::utils::library_loader::close_all: "
                       << "library_loader::close_all: DATA *****************" << endl;
                }
              library_entry_type & le = hle.get ();
              if (le.handle != 0)
                {
                  if (is_debug ())
                    {
                      clog << "DEBUG: " << "datatools::utils::library_loader::close_all: "
                           << "Closing library '" << le.name << "'..." << endl;
                    }
                  int status = DATATOOLS_SYS_NAMESPACE::DynamicLoader::CloseLibrary (le.handle);
                  if (status != 1)
                    {
                      ostringstream message;
                      message << "datatools::utils::library_loader::close_all: The '"
                              << le.name << "' library was not closed ! " << "DATATOOLS_SYS_NAMESPACE" << " says: '"
                              << DATATOOLS_SYS_NAMESPACE::DynamicLoader::LastError () << "' !";
                      cerr << "ERROR: " << message.str () << endl;
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
                  _stacked_libraries_.pop_front (); // remove top element
                  if (g_devel)
                    {
                      clog << "DEVEL: " << "datatools::utils::library_loader::close_all: "
                           << "Stack size " << _stacked_libraries_.size () << endl;
                    }
                  // }
                  if (g_devel)
                    {
                      clog << "DEVEL: " << "datatools::utils::library_loader::close_all: "
                           << "Removing library stacked entry '" << le.name << "'..."
                           << endl;
                    }
                }
            }
        }
      return EXIT_SUCCESS;
    }

    // dtor :
    library_loader::~library_loader ()
    {
      if (g_devel) clog << "DEVEL: datatools::utils::library_loader::dtor: Entering...\n";
      if (g_devel) print (cerr);
      if (g_devel) clog << "DEVEL: datatools::utils::library_loader::dtor: " << "close_all...\n";
      close_all ();
      if (g_devel) clog << "DEVEL: datatools::utils::library_loader::dtor: " << "clear...\n";
      _libraries_.clear ();
      if (g_devel) clog << "DEVEL: datatools::utils::library_loader::dtor: Exiting.\n";
      return;
    }

    bool library_loader::has (const string & lib_name_) const
    {
      return _libraries_.find (lib_name_) != _libraries_.end ();
    }

    bool library_loader::is_loaded (const string & lib_name_) const
    {
      handle_library_entry_dict_type::const_iterator found = _libraries_.find (lib_name_);
      if (found != _libraries_.end ())
        {
          if (found->second.get ().handle != 0)
            {
              return true;
            }
        }
      return false;
    }

    int library_loader::registration (const string & lib_name_,
                                      const string & lib_directory_,
                                      const string & lib_filename_,
                                      const string & lib_full_path_,
                                      const string & lib_version_,
                                      bool           autoload_)
    {
      if (has (lib_name_))
        {
          return EXIT_FAILURE;
        }
      handle_library_entry_type le_handle (new library_entry_type);
      _libraries_[lib_name_] = le_handle;
      library_entry_type & le = _libraries_[lib_name_].get ();
      le.name         = lib_name_;
      le.filename     = lib_filename_;
      le.version      = lib_version_;
      le.directory    = lib_directory_;
      le.full_path    = lib_full_path_;

      if (le.filename.empty ())
        {
          ostringstream filename_ss;
          filename_ss << DATATOOLS_SYS_NAMESPACE::DynamicLoader::LibPrefix();
          filename_ss << le.name;
          filename_ss << DATATOOLS_SYS_NAMESPACE::DynamicLoader::LibExtension();
          if (! le.version.empty ())
            {
              filename_ss << le.version;
            }
          le.filename = filename_ss.str ();
        }

      if (le.full_path.empty ())
        {
          ostringstream full_path_ss;
          if (! le.directory.empty ())
            {
              full_path_ss << le.directory;
              if (! boost::algorithm::ends_with (le.directory, "/"))
                {
                  full_path_ss << '/';
                }
            }
          full_path_ss << le.filename;
          le.full_path = full_path_ss.str ();
        }

      if (le.full_path.empty ())
        {
          ostringstream message;
          message << "datatools::utils::library_loader::registration: The shared library file '"
                  << le.name << "' has no valid path !";
          throw logic_error (message.str ());
        }

      if (! le.directory.empty ())
        {
          // Only check the existence of the DLL file if a directory is
          // explicitely given :
          string check = le.full_path;
          fetch_path_with_env (check);
          if (! boost::filesystem::exists (check))
            {
              ostringstream message;
              message << "datatools::utils::library_loader::registration: The shared library file '"
                      << le.name << "' has no valid file '" << le.full_path << "' !";
              throw logic_error (message.str ());
            }
        }
      le.handle = 0;
      le.autoload = autoload_;
      return EXIT_SUCCESS;
    }

    int library_loader::load (const string & lib_name_,
                              const string & directory_,
                              const string & filename_,
                              const string & full_lib_path_,
                              const string & version_)
    {
      handle_library_entry_dict_type::iterator found
        = _libraries_.find (lib_name_);
      if (found == _libraries_.end ())
        {
          if (! allowing_unregistered ())
            {
              cerr << "ERROR: "
                   << "datatools::utils::library_loader::load: "
                   << "Library '"
                   << lib_name_ << "' is not registered !" << endl;
              return EXIT_FAILURE;
            }
          else
            {
              int status = registration (lib_name_,
                                         directory_,
                                         filename_,
                                         full_lib_path_,
                                         version_,
                                         true);
              if (status != EXIT_SUCCESS)
                {
                  ostringstream message;
                  message << "datatools::utils::library_loader::load: "
                          << "Cannot register library '" << lib_name_ << "' !";
                  cerr << "ERROR: " << message.str () << endl;
                  return EXIT_FAILURE;
                }
              found = _libraries_.find (lib_name_);
            }
        }
      library_entry_type & le = found->second.get ();
      string check = le.full_path;
      fetch_path_with_env (check);
      le.handle = DATATOOLS_SYS_NAMESPACE::DynamicLoader::OpenLibrary (check.c_str ());
      if (le.handle != 0)
        {
          _stacked_libraries_.push_front (found->second);
        }
      else
        {
          ostringstream message;
          message << "datatools::utils::library_loader::load: "
                  << "The '" <<lib_name_ << "' library was not loaded ! " 
                  << "DATATOOLS_SYS_NAMESPACE" << " says: '"
                  << DATATOOLS_SYS_NAMESPACE::DynamicLoader::LastError () << "' !";
          cerr << "ERROR: " << message.str () << endl;
          return EXIT_FAILURE;
        }
      return EXIT_SUCCESS;
    }

    int library_loader::close (const string & lib_name_)
    {
      if (! is_loaded (lib_name_))
        {
          cerr << "ERROR: "
               << "datatools::utils::library_loader::close: "
               << "No loaded library '" << lib_name_ << "' to be closed !" << endl;
          return EXIT_FAILURE;
        }
      if (_stacked_libraries_.front ().get ().name != lib_name_)
        {
          cerr << "ERROR: "
               << "datatools::utils::library_loader::close: "
               << "Cannot close library '"
               << lib_name_ << "' !" << endl;
          return EXIT_FAILURE;
        }
      handle_library_entry_dict_type::iterator found
        = _libraries_.find (lib_name_);
      handle_library_entry_type & hle = found->second;
      library_entry_type & le = hle.get ();

      int status = DATATOOLS_SYS_NAMESPACE::DynamicLoader::CloseLibrary (le.handle);
      if (status != 1)
        {
          ostringstream message;
          message << "The '" << le.name << "' library was not closed ! " << "DATATOOLS_SYS_NAMESPACE" << " says: '"
                  << DATATOOLS_SYS_NAMESPACE::DynamicLoader::LastError () << "' !";
          clog << "ERROR: " << message.str () << endl;
          return EXIT_FAILURE;
        }
      else
        {
          _stacked_libraries_.front ().get ().handle = 0;
          _stacked_libraries_.pop_front ();
        }
      return EXIT_SUCCESS;
    }

    void library_loader::_init ()
    {
      // checking test mode :
      {
        string env_name = "DATATOOLS_LIBRARY_LOADER_TEST";
        const char * env_check = getenv (env_name.c_str ());
        if (env_check != 0)
          {
            string ec = env_check;
            if (ec == "1")
              {
                _flags_ |= test;
              }
          }
      }

      // configuration file :
      if (_config_filename_.empty ())
        {
          string env_name = "DATATOOLS_LIBRARY_LOADER_CONFIG";
          const char * env_check = getenv (env_name.c_str ());
          if (env_check != 0)
            {
              _config_filename_ = env_check;
            }
          else if (is_test ())
            {
              _config_filename_ = "${DATATOOLS_ROOT}/resources/test/test_library_loader.conf";
            }
        }
      if (g_devel)
        {
          clog << "DEVEL: " << "datatools::utils::library_loader::_init: "
               << "Using library loader config file = '"
               << _config_filename_<< "' !" << endl;
        }

      if (_config_filename_.empty ())
        {
          clog << "WARNING: " << "datatools::utils::library_loader::_init: "
               << "No library loader config file is available !"
               << endl;
          return;
        }
      using namespace datatools::utils;
      fetch_path_with_env (_config_filename_);
      multi_properties config ("name", "filename");
      config.read (_config_filename_);

      BOOST_FOREACH(const multi_properties::entry * ptr, config.ordered_entries ())
        {
          const multi_properties::entry & e = *ptr;
          if (g_devel)
            {
              clog << "DEVEL: "
                   << "datatools::utils::library_loader::_init: "
                   << "Settings for library '"
                   << e.get_key () << "'..." << endl;
            }
          const properties & lib_properties = e.get_properties ();
          string             lib_name       = e.get_key ();
          string             lib_filename   = e.get_meta ();
          string             lib_directory  = "";
          string             lib_full_path  = "";
          string             lib_version    = "";
          bool               lib_autoload   = false;

          if (lib_properties.has_key ("full_path"))
            {
              lib_full_path = lib_properties.fetch_string ("full_path");
            }
          else
            {
              if (lib_properties.has_key ("directory"))
                {
                  lib_directory = lib_properties.fetch_string ("directory");
                }
              if (lib_properties.has_key ("filename"))
                {
                  lib_filename = lib_properties.fetch_string ("filename");
                }
            }

          if (lib_properties.has_flag ("autoload"))
            {
              lib_autoload = lib_properties.fetch_boolean ("autoload");
            }

          if (lib_properties.has_key ("version"))
            {
              lib_version = lib_properties.fetch_string ("version");
            }

          if (g_devel)
            {
              clog << "DEVEL: "
                   << "datatools::utils::library_loader::_init: "
                   << "Registration of library='" << lib_name << "'..." << endl;
            }
          int status = registration (lib_name,
                                     lib_directory,
                                     lib_filename,
                                     lib_full_path,
                                     lib_version,
                                     lib_autoload);
          if (status != EXIT_SUCCESS)
            {
              ostringstream message;
              message << "library_entry_type::load: "
                      << "Cannot register library '" << lib_name << "' !";
              throw logic_error (message.str ());
            }

          if (lib_autoload)
            {
              clog << "NOTICE: "
                   << "datatools::utils::library_loader::_init: "
                   << "Automatic loading of library  '" << lib_name << "'..."
                   << endl;
              int load_status = load (lib_name);
              if (load_status != EXIT_SUCCESS)
                {
                  ostringstream message;
                  message << "datatools::utils::library_loader::_init: "
                          << "Automatic loading of library='"
                          << lib_name << "' failed !";
                  throw logic_error (message.str ());
                }
            }
          continue;
        } // BOOST_FOREACH

      if (is_debug ()) print (clog);
      return;
    }

    void library_loader::print (ostream & out_) const
    {
      out_ << "Library loader : " << endl;
      out_ << "Flags              : " << _flags_ << endl;
      out_ << "Configuration file : '" << _config_filename_ << "'" << endl;
      out_ << "List of registered shared libraries :" << endl;
      for (handle_library_entry_dict_type::const_iterator i
             = _libraries_.begin ();
           i != _libraries_.end ();
           i++)
        {
          handle_library_entry_dict_type::const_iterator j = i;
          j++;
          const library_entry_type & le = i->second.get ();
          string tag = "|   ";
          if (j == _libraries_.end ())
            {
              out_ << "`-- ";
              tag = "    ";
            }
          else
            {
              out_ << "|-- ";
            }
          out_ << "Library : '" << i->first << "'" << endl;
          le.print (out_, tag);
        }
      out_ << "List of loaded shared libraries :" << endl;
      for (handle_library_entry_stack_type::const_iterator i = _stacked_libraries_.begin ();
           i != _stacked_libraries_.end ();
           i++)
        {
          handle_library_entry_stack_type::const_iterator j = i;
          j++;
          const library_entry_type & le = i->get ();
          string tag = "|   ";
          if (j == _stacked_libraries_.end ())
            {
              out_ << "`-- ";
              tag = "    ";
            }
          else
            {
              out_ << "|-- ";
            }
          out_ << "Library : '" << le.name << "'" << endl;
        }
      return;
    }

    /*
      library_loader &
      library_loader::get_library_loader ()
      {
      if (! g_library_loader_)
      {
      if (g_devel)
      {
      clog << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
      clog << "DEVEL: datatools::utils::library_loader::get_library_loader: "
      << "Instantiating the global library loader..."
      << endl;
      }
      g_library_loader_.reset (new library_loader ());
      if (g_devel)
      {
      clog << "DEVEL: datatools::utils::library_loader::get_library_loader: "
      << "Done."
      << endl;
      clog << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
      }
      }
      return *g_library_loader_;
      }
    */

  } // end of namespace utils

} // end of namespace datatools

// end of properties.cc

// -*- mode: c++; -*-
/* library_loader.cc
 */

#include <datatools/utils/library_loader.h>

#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/foreach.hpp>

#include <kwsys/DynamicLoader.hxx>
#include <datatools/utils/utils.h>
#include <datatools/utils/handle.h>
#include <datatools/utils/multi_properties.h>

namespace datatools {

  namespace utils {

    bool library_loader::g_devel = false;
    bool library_loader::g_test  = false;

    /***********************
     *  library_entry_type *
     ***********************/

    // ctor :
    library_entry_type::library_entry_type (const string & a_name,
                                            const string & a_directory,
					    const string & a_filename,
                                            const string & a_full_path,
                                            const string & a_version,
					    bool a_autoload)
    {
      name      = a_name;
      directory = a_directory;
      filename  = a_filename;
      full_path = a_full_path;
      version   = a_version;
      autoload  = a_autoload;
      handle    = 0;
      status    = 0;
      return;
    }

    // dtor :
    library_entry_type::~library_entry_type ()
    {
      if (handle != 0)
	{
          int status = kwsys::DynamicLoader::CloseLibrary (handle);
	  if (status != 1)
	    {
	      ostringstream message;
	      message << "library_entry_type::dtor: The '"
		      << name << "' library was not closed ! kwsys says: '"
		      << kwsys::DynamicLoader::LastError () << "' !";
	      cerr << "ERROR: " << message.str () << endl;
	    }
	}
      handle = 0;
      return;
    }

    void library_entry_type::print (ostream & a_out,
                                    const string & a_indent) const
    {
      a_out << a_indent << "|-- Name      : " << name << endl;
      if (! directory.empty ())
        {
          a_out << a_indent << "|-- Directory : '" << directory << "'" << endl;
        }
      if (! filename.empty ())
        {
          a_out << a_indent << "|-- Filename  : '" << filename << "'" << endl;
        }
      a_out << a_indent << "|-- Full path : '" << full_path << "'" << endl;
      {
	string fpe = full_path;
	fetch_path_with_env (fpe);
	if (fpe != full_path)
	  {
	    a_out << a_indent << "|    `-- Expanded to : '" << fpe << "'" << endl;
	  }
      }
      if (! version.empty ())
        {
          a_out << a_indent << "|-- Version : '" << version << "'" << endl;
        }
      a_out << a_indent << "|-- Autoload  : " << (autoload ? "Yes": "No") << endl;
      a_out << a_indent << "`-- Loaded    : " << (handle ? "Yes": "No");
      if (handle != 0)
	{
	  a_out << " [" << handle << "]";
	}
      a_out << endl;
      return;
    }

    /*******************
     *  library_loader *
     *******************/

    bool library_loader::is_debug () const
    {
      return flags_ & debug;
    }

    void library_loader::set_debug (bool a_debug)
    {
      if (a_debug)
	{
	  flags_ |= debug;
 	}
      else
	{
	  flags_ ^= debug;
	}
      return;
    }

    bool library_loader::is_test () const
    {
      return flags_ & test;
    }

    void library_loader::set_allow_unregistered (bool a_allow_unregistered)
    {
      if (a_allow_unregistered)
	{
	  flags_ |= allow_unregistered;
	}
      else
	{
	  flags_ ^= allow_unregistered;
	}
      return;
    }

    bool library_loader::allowing_unregistered () const
    {
      return flags_ & allow_unregistered;
    }

    symbol_ptr library_loader::get_symbol_address (const string & a_lib_name,
						   const string & a_symbol)
    {
      /*
      if (! is_loaded (a_lib_name))
	{
	  ostringstream message;
	  message << "datatools::utils::library_loader::get_symbol_address: "
		  << "The shared library file '"
		  << a_lib_name << "' is not loaded !";
	  throw logic_error (message.str ());
	}
      */
      handle_library_entry_dict_type::const_iterator found = libraries_.find (a_lib_name);
      if (found == libraries_.end ())
	{
	  ostringstream message;
	  message << "datatools::utils::library_loader::get_symbol_address: "
		  << "The shared library file '"
		  << a_lib_name << "' is not registered !";
	  throw logic_error (message.str ());
	}
      if (found->second.get ().handle == 0)
	{
	  ostringstream message;
	  message << "datatools::utils::library_loader::get_symbol_address: "
		  << "The shared library file '"
		  << a_lib_name << "' is not loaded !";
	  throw logic_error (message.str ());
	}
      symbol_ptr the_symptr = kwsys::DynamicLoader::GetSymbolAddress (found->second.get ().handle, a_symbol.c_str());
      return the_symptr;
    }

    // ctor :
    library_loader::library_loader (uint32_t the_flags,
				    const string & a_dll_config_file)
    {
      if (g_devel) clog << "DEVEL: datatools::utils::library_loader::ctor: Entering...\n";
      flags_ = the_flags;
      if (library_loader::g_test)
	{
	  flags_ |= test;
	}
      config_filename_ = a_dll_config_file;
      init_ ();
      if (g_devel) clog << "DEVEL: datatools::utils::library_loader::ctor: Exiting.\n";
      return;
    }

    int library_loader::close_all ()
    {
      while (! stacked_libraries_.empty ())
	{
	  if (g_devel)
	    {
	      clog << "DEVEL: " << "datatools::utils::library_loader::close_all: "
		   << "library_loader::close_all: LOOP ****************" << endl;
	    }
	  handle_library_entry_type & hle = stacked_libraries_.front ();
	  if (! hle.has_data ())
	    {
	      if (g_devel)
		{
		  clog << "DEVEL: " << "datatools::utils::library_loader::close_all: "
		       << "library_loader::close_all: NO DATA ****************" << endl;
		}
	      stacked_libraries_.pop_front (); // remove top element
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
		  int status = kwsys::DynamicLoader::CloseLibrary (le.handle);
		  if (status != 1)
		    {
		      ostringstream message;
		      message << "datatools::utils::library_loader::close_all: The '"
			      << le.name << "' library was not closed ! kwsys says: '"
			      << kwsys::DynamicLoader::LastError () << "' !";
		      cerr << "ERROR: " << message.str () << endl;
		      //return;
		    }

                  // 2011/09/19: F.M & X.G: Even if the return status
		  // from kwsys::DynamicLoader::CloseLibrary is not 0,
		  // the library is closed by kwsys, so we removed the
		  // library entry from the library stack. Actually
		  // the return status is really not explicit and can
		  // not be used.

                  // else
		  //   {
                  le.handle = 0;
                  stacked_libraries_.pop_front (); // remove top element
                  if (g_devel)
		    {
		      clog << "DEVEL: " << "datatools::utils::library_loader::close_all: "
			   << "Stack size " << stacked_libraries_.size () << endl;
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
      libraries_.clear ();
      if (g_devel) clog << "DEVEL: datatools::utils::library_loader::dtor: Exiting.\n";
      return;
    }

    bool library_loader::has (const string & a_lib_name) const
    {
      return libraries_.find (a_lib_name) != libraries_.end ();
    }

    bool library_loader::is_loaded (const string & a_lib_name) const
    {
      handle_library_entry_dict_type::const_iterator found = libraries_.find (a_lib_name);
      if (found != libraries_.end ())
	{
	  if (found->second.get ().handle != 0)
	    {
	      return true;
	    }
	}
      return false;
    }

    int library_loader::registration (const string & a_lib_name,
				      const string & a_lib_directory,
				      const string & a_lib_filename,
				      const string & a_lib_full_path,
				      const string & a_lib_version,
				      bool           a_autoload)
    {
      if (has (a_lib_name))
	{
	  return EXIT_FAILURE;
	}
      handle_library_entry_type le_handle (new library_entry_type);
      libraries_[a_lib_name] = le_handle;
      library_entry_type & le = libraries_[a_lib_name].get ();
      le.name         = a_lib_name;
      le.filename     = a_lib_filename;
      le.version      = a_lib_version;
      le.directory    = a_lib_directory;
      le.full_path    = a_lib_full_path;

      if (le.filename.empty ())
	{
	  ostringstream filename_ss;
	  filename_ss << kwsys::DynamicLoader::LibPrefix();
	  filename_ss << le.name;
	  filename_ss << kwsys::DynamicLoader::LibExtension();
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
      le.autoload = a_autoload;
      return EXIT_SUCCESS;
    }

    int library_loader::load (const string & a_lib_name,
			      const string & a_directory,
			      const string & a_filename,
			      const string & a_full_lib_path,
			      const string & a_version)
    {
      handle_library_entry_dict_type::iterator found
        = libraries_.find (a_lib_name);
      if (found == libraries_.end ())
        {
	  if (! allowing_unregistered ())
	    {
	      cerr << "ERROR: "
		   << "datatools::utils::library_loader::load: "
		   << "Library '"
		   << a_lib_name << "' is not registered !" << endl;
	      return EXIT_FAILURE;
	    }
	  else
	    {
	      int status = registration (a_lib_name,
					 a_directory,
					 a_filename,
					 a_full_lib_path,
					 a_version,
					 true);
	      if (status != EXIT_SUCCESS)
		{
		  ostringstream message;
                  message << "library_entry_type::load: "
			  << "Cannot register library '" << a_lib_name << "' !";
		  cerr << "ERROR: " << message.str () << endl;
		  return EXIT_FAILURE;
		}
	      found = libraries_.find (a_lib_name);
	    }
        }
      library_entry_type & le = found->second.get ();
      string check = le.full_path;
      fetch_path_with_env (check);
      le.handle = kwsys::DynamicLoader::OpenLibrary (check.c_str ());
      if (le.handle != 0)
	{
	  stacked_libraries_.push_front (found->second);
	}
      else
	{
	  ostringstream message;
	  message << a_lib_name << " library was not loaded ! kwsys says: '"
		  << kwsys::DynamicLoader::LastError () << "' !";
	  cerr << "ERROR: " << message.str () << endl;
	}
      return EXIT_SUCCESS;
    }

    int library_loader::close (const string & a_lib_name)
    {
       if (! is_loaded (a_lib_name))
        {
	  cerr << "ERROR: "
	       << "datatools::utils::library_loader::close: "
	       << "No loaded library '" << a_lib_name << "' to be closed !" << endl;
	  return EXIT_FAILURE;
        }
      if (stacked_libraries_.front ().get ().name != a_lib_name)
	{
	  cerr << "ERROR: "
	       << "datatools::utils::library_loader::close: "
	       << "Cannot close library '"
	       << a_lib_name << "' !" << endl;
	  return EXIT_FAILURE;
	}
      handle_library_entry_dict_type::iterator found
        = libraries_.find (a_lib_name);
      handle_library_entry_type & hle = found->second;
      library_entry_type & le = hle.get ();

      int status = kwsys::DynamicLoader::CloseLibrary (le.handle);
      if (status != 1)
	{
	  ostringstream message;
	  message << "The '" << le.name << "' library was not closed ! kwsys says: '"
		  << kwsys::DynamicLoader::LastError () << "' !";
	  clog << "ERROR: " << message.str () << endl;
	  return EXIT_FAILURE;
	}
      else
	{
	  stacked_libraries_.front ().get ().handle = 0;
	  stacked_libraries_.pop_front ();
	}
      return EXIT_SUCCESS;
    }

    void library_loader::init_ ()
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
                flags_ |= test;
              }
          }
      }

      // configuration file :
      if (config_filename_.empty ())
	{
	  string env_name = "DATATOOLS_LIBRARY_LOADER_CONFIG";
	  const char * env_check = getenv (env_name.c_str ());
	  if (env_check != 0)
	    {
	      config_filename_ = env_check;
	    }
	  else if (is_test ())
	    {
	      config_filename_ = "${DATATOOLS_ROOT}/resources/test/test_library_loader.conf";
	    }
	}
      if (g_devel)
	{
	  clog << "DEVEL: " << "datatools::utils::library_loader::init_: "
	       << "Using library loader config file = '"
	       << config_filename_<< "' !" << endl;
	}

      if (config_filename_.empty ())
        {
	  clog << "WARNING: " << "datatools::utils::library_loader::init_: "
	       << "No library loader config file is available !"
	       << endl;
          return;
        }
      using namespace datatools::utils;
      fetch_path_with_env (config_filename_);
      multi_properties config ("name", "filename");
      config.read (config_filename_);

      BOOST_FOREACH(const multi_properties::entry * ptr, config.ordered_entries ())
       {
	 const multi_properties::entry & e = *ptr;
	 if (g_devel)
	   {
	      clog << "DEVEL: "
		   << "datatools::utils::library_loader::init_: "
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
		   << "datatools::utils::library_loader::init_: "
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
		   << "datatools::utils::library_loader::init_: "
		   << "Automatic loading of library  '" << lib_name << "'..."
		   << endl;
	      int load_status = load (lib_name);
	      if (load_status != EXIT_SUCCESS)
		{
                  ostringstream message;
                  message << "datatools::utils::library_loader::init_: "
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

    void library_loader::print (ostream & a_out) const
    {
      a_out << "Library loader : " << endl;
      a_out << "Flags              : " << flags_ << endl;
      a_out << "Configuration file : '" << config_filename_ << "'" << endl;
      a_out << "List of registered shared libraries :" << endl;
      for (handle_library_entry_dict_type::const_iterator i
             = libraries_.begin ();
           i != libraries_.end ();
           i++)
	  {
          handle_library_entry_dict_type::const_iterator j = i;
          j++;
          const library_entry_type & le = i->second.get ();
          string tag = "|   ";
          if (j == libraries_.end ())
            {
              a_out << "`-- ";
              tag = "    ";
            }
          else
            {
              a_out << "|-- ";
            }
          a_out << "Library : '" << i->first << "'" << endl;
          le.print (a_out, tag);
        }
      a_out << "List of loaded shared libraries :" << endl;
      for (handle_library_entry_stack_type::const_iterator i = stacked_libraries_.begin ();
	   i != stacked_libraries_.end ();
	   i++)
	{
          handle_library_entry_stack_type::const_iterator j = i;
          j++;
          const library_entry_type & le = i->get ();
          string tag = "|   ";
          if (j == stacked_libraries_.end ())
            {
              a_out << "`-- ";
              tag = "    ";
            }
          else
            {
              a_out << "|-- ";
            }
          a_out << "Library : '" << le.name << "'" << endl;
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

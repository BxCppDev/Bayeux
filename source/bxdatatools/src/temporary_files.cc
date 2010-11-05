// -*- mode: c++; -*- 
// temporary_files.cc

#include <datatools/utils/temporary_files.h>

namespace datatools {

  namespace utils {

    const string temp_file::DEFAULT_PATTERN = "tmp_";
    bool temp_file::g_devel = false;

    void temp_file::_set_defaults ()
    {
      __remove_at_destroy = true;
      __template = 0;
      __read_open = false;
      __write_open = false;
      return;
    }

    bool temp_file::is_valid () const
    {
      return __template != 0;
    }

    bool temp_file::is_read_open () const
    {
      return __read_open;
    }

    bool temp_file::is_write_open () const
    {
      return __write_open;
    }

    ofstream & temp_file::out ()
    {
      if (g_devel) clog << "DEVEL: datatools::utils::temp_file::out: Entering..." << endl;
      if (! is_valid ())
	{
	  ostringstream message;
	  message << "datatools::utils::temp_file::out: "
		  << "Temporary file has not been created yet !";
	  throw runtime_error (message.str ());	  
	}
      if (__read_open)
	{
	  ostringstream message;
	  message << "datatools::utils::temp_file::out: "
		  << "Temporary file '" << __filename << "' is already open in read mode !";
	  throw runtime_error (message.str ());	  
	}
      if (! __write_open)
	{
	  __out.open (__filename.c_str ());
	  if (! __out)
	    {
	      ostringstream message;
	      message << "datatools::utils::temp_file::out: "
		      << "Cannot open temporary file '" << __filename << "' in write mode !";
	      throw runtime_error (message.str ());	  	      
	    }
	  __write_open = true;
	}
      if (g_devel) clog << "DEVEL: datatools::utils::temp_file::out: Exiting." << endl;
      return __out;
    }
    
    ifstream & temp_file::in ()
    {
      if (g_devel) clog << "DEVEL: datatools::utils::temp_file::in: Entering..." << endl;
      if (! is_valid ())
	{
	  ostringstream message;
	  message << "datatools::utils::temp_file::in: "
		  << "Temporary file has not been created yet !";
	  throw runtime_error (message.str ());
	}
      if (__write_open)
	{
	  ostringstream message;
	  message << "datatools::utils::temp_file::in: "
		  << "Temporary file '" << __filename << "' is already open in write mode !";
	  throw runtime_error (message.str ());	  
	}
      if (! __read_open)
	{
	  __in.open (__filename.c_str ());
	  if (! __out)
	    {
	      ostringstream message;
	      message << "datatools::utils::temp_file::in: "
		      << "Cannot open temporary file '" << __filename << "' in read mode !";
	      throw runtime_error (message.str ());	  	      
	    }
	  __read_open = true;
	}
      if (g_devel) clog << "DEVEL: datatools::utils::temp_file::in: Exiting." << endl;
      return __in;
    }

    // ctor:
    temp_file::temp_file ()
    {
      _set_defaults ();
      return;
    }

    // ctor:
    temp_file::temp_file (const char * pattern_, bool remove_at_destroy_)
    {
      _set_defaults ();
      set_remove_at_destroy  (remove_at_destroy_);
      string pattern = pattern_;
      create ("", pattern);
      return;
    }

    // ctor:
    temp_file::temp_file (string pattern_, bool remove_at_destroy_)
    {
      _set_defaults ();
      set_remove_at_destroy  (remove_at_destroy_);
      create ("", pattern_);
      return;
    }
    
    // ctor:
    temp_file::temp_file (string path_dir_, const char * pattern_, bool remove_at_destroy_)
    {
      _set_defaults ();
      set_remove_at_destroy  (remove_at_destroy_);
      string pattern = pattern_;
      create (path_dir_, pattern);
      return;
    }
    
    // ctor:
    temp_file::temp_file (const char * path_dir_, const char * pattern_, bool remove_at_destroy_)
    {
      _set_defaults ();
      set_remove_at_destroy  (remove_at_destroy_);
      string pattern = pattern_;
      string path_dir = path_dir_;
      create (path_dir, pattern);
      return;
    }
    
    // ctor:
    temp_file::temp_file (string path_dir_, string pattern_, bool remove_at_destroy_)
    {
      _set_defaults ();
      set_remove_at_destroy  (remove_at_destroy_);
      create (path_dir_, pattern_);
      return;
    }

    // dtor:
    temp_file::~temp_file ()
    {
      if (__remove_at_destroy)
	{
	  if (g_devel)
	    {
	      clog << "DEVEL: datatools::utils::temp_file::dtor: "
		   << "Deleting the file with name '"
		   << __filename << "'" << endl;
	    }
	  remove ();
	}
      if (__template != 0)
	{
	  delete __template;
	  __template = 0;
	}
      return;
    }

    void temp_file::create (string path_dir_, string pattern_)
    {
      bool devel = g_devel;
      if (is_valid ())
	{
	  ostringstream message;
	  message << "datatools::utils::temp_file::create: "
		  << "Temporary file with name '"
		  << __filename << "' has already been created !";
	  throw runtime_error (message.str ());
	} 
      if (path_dir_.empty ())
	{
	  __path_dir = ".";
	}
      else
	{
	  __path_dir = path_dir_;
	  fetch_path_with_env (__path_dir);
	}
      if (! boost::filesystem::is_directory (__path_dir))
	{
	  ostringstream message;
	  message << "datatools::utils::temp_file::create: "
		  << "Directory with name '"
		  << __path_dir << "' does not exist !";
	  throw runtime_error (message.str ());	  
	}

      if (pattern_.empty ())
	{
	  __pattern = DEFAULT_PATTERN;
	}
      else 
	{
	  fetch_path_with_env (pattern_);
	  if (pattern_.find_first_of ('/') != pattern_.npos)
	    {
	      ostringstream message;
	      message << "datatools::utils::temp_file::create: "
		      << "Invalid pattern for filename ! Pattern '"
		      << pattern_ << "' contains invalid character(s) !";
	      throw runtime_error (message.str ());	  
	    }
	  __pattern = pattern_;
	}
      ostringstream filename_oss;
      filename_oss << __path_dir << '/' << __pattern << "XXXXXX";
      __full_pattern = filename_oss.str ();
      if (devel) clog << "DEVEL: datatools::utils::temp_file::create: full_pattern is '" << __full_pattern << "' !" << endl;

      __template = new char [__full_pattern.size () + 10];
      __template[0] = '\0';
      if (devel) clog << "DEVEL: datatools::utils::temp_file::create: buffer is allocated !" << endl;
      for (int i = 0; i < __full_pattern.size (); i++)
	{
	  __template[i] = __full_pattern[i];
	}
      __template[__full_pattern.size ()] = '\0';
      if (devel) clog << "DEVEL: datatools::utils::temp_file::create: template= '" << __template << "'" << endl;

      int err = mkstemp (__template);
      if (err == -1)
	{
	  ostringstream message;
	  message << "datatools::utils::temp_file::create: "
		  << "Cannot create temporary file using pattern '"
		  << __full_pattern << "' !";
	  if (__template != 0) 
	    {
	      delete __template;
	    }
	  __template = 0;
	  throw runtime_error (message.str ());
	} 
      if (devel) clog << "DEVEL: datatools::utils::temp_file::create: Template is '" << __template << "' !" << endl;
      __filename = __template;
      if (devel) clog << "DEVEL: datatools::utils::temp_file::create: Filename is '" << __filename << "' !" << endl;
      return;
    }

    const string & temp_file::get_filename () const
    {
      return __filename;
    }

    void temp_file::set_remove_at_destroy (bool r_)
    {
      __remove_at_destroy = r_;
    }
      
    void temp_file::close ()
    {
      if (is_read_open ())
	{
	  if (g_devel) clog << "DEVEL: datatools::utils::temp_file::close: closing read file mode !" << endl;
	  __in.close ();
	  __read_open = false;
	}      
      if (is_write_open ())
	{
	  if (g_devel) clog << "DEVEL: datatools::utils::temp_file::close: closing write file mode !" << endl;
	  __out.close ();
	  __write_open = false;
	}      
      return;
    }
    
    void temp_file::remove ()
    {
      if (g_devel) clog << "DEVEL: datatools::utils::temp_file::remove: Entering..." << endl;
      close ();
      if (! __filename.empty ())
	{
	  int err = unlink (__filename.c_str ());
	  if (err == -1)
	    {
	      ostringstream message;
	      message << "datatools::utils::temp_file::remove: "
		      << "Cannot delete temporary file '"
		      << __filename << "' !";
	      throw runtime_error (message.str ());
	    } 
	}
      if (__template != 0)
	{
	  delete __template;
	  __template = 0;
	}     
      _set_defaults ();
      if (g_devel) clog << "DEVEL: datatools::utils::temp_file::remove: Exiting." << endl;
      return;
    }

  } // namespace utils

} // namespace datatools 

// end of temporary_files.cc

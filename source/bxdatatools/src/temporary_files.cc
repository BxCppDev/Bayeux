// -*- mode: c++; -*- 
// temporary_files.cc

#include <datatools/utils/temporary_files.h>

#include <boost/filesystem.hpp>

#include <cstdlib>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <datatools/utils/utils.h>

namespace datatools {

  namespace utils {

    const string temp_file::DEFAULT_PATTERN = "tmp_";
    bool temp_file::g_devel = false;

    void temp_file::set_defaults_ ()
    {
      remove_at_destroy_ = true;
      template_ = 0;
      read_open_ = false;
      write_open_ = false;
      return;
    }

    bool temp_file::is_valid () const
    {
      return template_ != 0;
    }

    bool temp_file::is_read_open () const
    {
      return read_open_;
    }

    bool temp_file::is_write_open () const
    {
      return write_open_;
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
      if (read_open_)
	{
	  ostringstream message;
	  message << "datatools::utils::temp_file::out: "
		  << "Temporary file '" << filename_ << "' is already open in read mode !";
	  throw runtime_error (message.str ());	  
	}
      if (! write_open_)
	{
	  out_.open (filename_.c_str ());
	  if (! out_)
	    {
	      ostringstream message;
	      message << "datatools::utils::temp_file::out: "
		      << "Cannot open temporary file '" << filename_ << "' in write mode !";
	      throw runtime_error (message.str ());	  	      
	    }
	  write_open_ = true;
	}
      if (g_devel) clog << "DEVEL: datatools::utils::temp_file::out: Exiting." << endl;
      return out_;
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
      if (write_open_)
	{
	  ostringstream message;
	  message << "datatools::utils::temp_file::in: "
		  << "Temporary file '" << filename_ << "' is already open in write mode !";
	  throw runtime_error (message.str ());	  
	}
      if (! read_open_)
	{
	  in_.open (filename_.c_str ());
	  if (! out_)
	    {
	      ostringstream message;
	      message << "datatools::utils::temp_file::in: "
		      << "Cannot open temporary file '" << filename_ << "' in read mode !";
	      throw runtime_error (message.str ());	  	      
	    }
	  read_open_ = true;
	}
      if (g_devel) clog << "DEVEL: datatools::utils::temp_file::in: Exiting." << endl;
      return in_;
    }

    // ctor:
    temp_file::temp_file ()
    {
      set_defaults_ ();
      return;
    }

    // ctor:
    temp_file::temp_file (const char * a_pattern, bool a_remove_at_destroy)
    {
      set_defaults_ ();
      set_remove_at_destroy  (a_remove_at_destroy);
      string pattern = a_pattern;
      create ("", pattern);
      return;
    }

    // ctor:
    temp_file::temp_file (string a_pattern, bool a_remove_at_destroy)
    {
      set_defaults_ ();
      set_remove_at_destroy  (a_remove_at_destroy);
      create ("", a_pattern);
      return;
    }
    
    // ctor:
    temp_file::temp_file (string a_path_dir, const char * a_pattern, bool a_remove_at_destroy)
    {
      set_defaults_ ();
      set_remove_at_destroy  (a_remove_at_destroy);
      string pattern = a_pattern;
      create (a_path_dir, pattern);
      return;
    }
    
    // ctor:
    temp_file::temp_file (const char * a_path_dir, const char * a_pattern, bool a_remove_at_destroy)
    {
      set_defaults_ ();
      set_remove_at_destroy  (a_remove_at_destroy);
      string pattern = a_pattern;
      string path_dir = a_path_dir;
      create (path_dir, pattern);
      return;
    }
    
    // ctor:
    temp_file::temp_file (string a_path_dir, string a_pattern, bool a_remove_at_destroy)
    {
      set_defaults_ ();
      set_remove_at_destroy  (a_remove_at_destroy);
      create (a_path_dir, a_pattern);
      return;
    }

    // dtor:
    temp_file::~temp_file ()
    {
      if (remove_at_destroy_)
	{
	  if (g_devel)
	    {
	      clog << "DEVEL: datatools::utils::temp_file::dtor: "
		   << "Deleting the file with name '"
		   << filename_ << "'" << endl;
	    }
	  remove ();
	}
      if (template_ != 0)
	{
	  delete template_;
	  template_ = 0;
	}
      return;
    }

    void temp_file::create (string a_path_dir, string a_pattern)
    {
      bool devel = g_devel;
      if (is_valid ())
	{
	  ostringstream message;
	  message << "datatools::utils::temp_file::create: "
		  << "Temporary file with name '"
		  << filename_ << "' has already been created !";
	  throw runtime_error (message.str ());
	} 
      if (a_path_dir.empty ())
	{
	  path_dir_ = ".";
	}
      else
	{
	  path_dir_ = a_path_dir;
	  fetch_path_with_env (path_dir_);
	}
      if (! boost::filesystem::is_directory (path_dir_))
	{
	  ostringstream message;
	  message << "datatools::utils::temp_file::create: "
		  << "Directory with name '"
		  << path_dir_ << "' does not exist !";
	  throw runtime_error (message.str ());	  
	}

      if (a_pattern.empty ())
	{
	  pattern_ = DEFAULT_PATTERN;
	}
      else 
	{
	  fetch_path_with_env (a_pattern);
	  if (a_pattern.find_first_of ('/') != a_pattern.npos)
	    {
	      ostringstream message;
	      message << "datatools::utils::temp_file::create: "
		      << "Invalid pattern for filename ! Pattern '"
		      << a_pattern << "' contains invalid character(s) !";
	      throw runtime_error (message.str ());	  
	    }
	  pattern_ = a_pattern;
	}
      ostringstream filename_oss;
      filename_oss << path_dir_ << '/' << pattern_ << "XXXXXX";
      full_pattern_ = filename_oss.str ();
      if (devel) clog << "DEVEL: datatools::utils::temp_file::create: full_pattern is '" << full_pattern_ << "' !" << endl;

      template_ = new char [full_pattern_.size () + 10];
      template_[0] = '\0';
      if (devel) clog << "DEVEL: datatools::utils::temp_file::create: buffer is allocated !" << endl;
      for (int i = 0; i < (int) full_pattern_.size (); i++)
	{
	  template_[i] = full_pattern_[i];
	}
      template_[full_pattern_.size ()] = '\0';
      if (devel) clog << "DEVEL: datatools::utils::temp_file::create: template= '" << template_ << "'" << endl;

      int err = mkstemp (template_);
      if (err == -1)
	{
	  ostringstream message;
	  message << "datatools::utils::temp_file::create: "
		  << "Cannot create temporary file using pattern '"
		  << full_pattern_ << "' !";
	  if (template_ != 0) 
	    {
	      delete template_;
	    }
	  template_ = 0;
	  throw runtime_error (message.str ());
	} 
      if (devel) clog << "DEVEL: datatools::utils::temp_file::create: Template is '" << template_ << "' !" << endl;
      filename_ = template_;
      if (devel) clog << "DEVEL: datatools::utils::temp_file::create: Filename is '" << filename_ << "' !" << endl;
      return;
    }

    const string & temp_file::get_filename () const
    {
      return filename_;
    }

    void temp_file::set_remove_at_destroy (bool r_)
    {
      remove_at_destroy_ = r_;
    }
      
    void temp_file::close ()
    {
      if (is_read_open ())
	{
	  if (g_devel) clog << "DEVEL: datatools::utils::temp_file::close: closing read file mode !" << endl;
	  in_.close ();
	  read_open_ = false;
	}      
      if (is_write_open ())
	{
	  if (g_devel) clog << "DEVEL: datatools::utils::temp_file::close: closing write file mode !" << endl;
	  out_.close ();
	  write_open_ = false;
	}      
      return;
    }
    
    void temp_file::remove ()
    {
      if (g_devel) clog << "DEVEL: datatools::utils::temp_file::remove: Entering..." << endl;
      close ();
      if (! filename_.empty ())
	{
	  int err = unlink (filename_.c_str ());
	  if (err == -1)
	    {
	      ostringstream message;
	      message << "datatools::utils::temp_file::remove: "
		      << "Cannot delete temporary file '"
		      << filename_ << "' !";
	      throw runtime_error (message.str ());
	    } 
	}
      if (template_ != 0)
	{
	  delete template_;
	  template_ = 0;
	}     
      set_defaults_ ();
      if (g_devel) clog << "DEVEL: datatools::utils::temp_file::remove: Exiting." << endl;
      return;
    }

  } // namespace utils

} // namespace datatools 

// end of temporary_files.cc

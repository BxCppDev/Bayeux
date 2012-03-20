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

    using namespace std;

    const string temp_file::DEFAULT_PATTERN = "tmp_";
    bool temp_file::g_devel = false;

    void temp_file::_set_defaults ()
    {
      _remove_at_destroy_ = true;
      _template_ = 0;
      _read_open_ = false;
      _write_open_ = false;
      return;
    }

    bool temp_file::is_valid () const
    {
      return _template_ != 0;
    }

    bool temp_file::is_read_open () const
    {
      return _read_open_;
    }

    bool temp_file::is_write_open () const
    {
      return _write_open_;
    }

    ofstream & temp_file::out ()
    {
      if (g_devel) clog << "DEVEL: datatools::utils::temp_file::out: Entering..." << endl;
      if (! is_valid ())
        {
          ostringstream message;
          message << "datatools::utils::temp_file::out: "
                  << "Temporary file has not been created yet !";
          throw logic_error (message.str ());     
        }
      if (_read_open_)
        {
          ostringstream message;
          message << "datatools::utils::temp_file::out: "
                  << "Temporary file '" << _filename_ << "' is already open in read mode !";
          throw logic_error (message.str ());     
        }
      if (! _write_open_)
        {
          _out_.open (_filename_.c_str ());
          if (! _out_)
            {
              ostringstream message;
              message << "datatools::utils::temp_file::out: "
                      << "Cannot open temporary file '" << _filename_ << "' in write mode !";
              throw runtime_error (message.str ());                   
            }
          _write_open_ = true;
        }
      if (g_devel) clog << "DEVEL: datatools::utils::temp_file::out: Exiting." << endl;
      return _out_;
    }
    
    ifstream & temp_file::in ()
    {
      if (g_devel) clog << "DEVEL: datatools::utils::temp_file::in: Entering..." << endl;
      if (! is_valid ())
        {
          ostringstream message;
          message << "datatools::utils::temp_file::in: "
                  << "Temporary file has not been created yet !";
          throw logic_error (message.str ());
        }
      if (_write_open_)
        {
          ostringstream message;
          message << "datatools::utils::temp_file::in: "
                  << "Temporary file '" << _filename_ << "' is already open in write mode !";
          throw logic_error (message.str ());     
        }
      if (! _read_open_)
        {
          _in_.open (_filename_.c_str ());
          if (! _out_)
            {
              ostringstream message;
              message << "datatools::utils::temp_file::in: "
                      << "Cannot open temporary file '" << _filename_ << "' in read mode !";
              throw runtime_error (message.str ());                   
            }
          _read_open_ = true;
        }
      if (g_devel) clog << "DEVEL: datatools::utils::temp_file::in: Exiting." << endl;
      return _in_;
    }

    // ctor:
    temp_file::temp_file ()
    {
      _set_defaults ();
      return;
    }

    // ctor:
    temp_file::temp_file (const char * a_pattern, bool a_remove_at_destroy)
    {
      _set_defaults ();
      set_remove_at_destroy  (a_remove_at_destroy);
      string pattern = a_pattern;
      create ("", pattern);
      return;
    }

    // ctor:
    temp_file::temp_file (string a_pattern, bool a_remove_at_destroy)
    {
      _set_defaults ();
      set_remove_at_destroy  (a_remove_at_destroy);
      create ("", a_pattern);
      return;
    }
    
    // ctor:
    temp_file::temp_file (string a_path_dir, const char * a_pattern, bool a_remove_at_destroy)
    {
      _set_defaults ();
      set_remove_at_destroy  (a_remove_at_destroy);
      string pattern = a_pattern;
      create (a_path_dir, pattern);
      return;
    }
    
    // ctor:
    temp_file::temp_file (const char * a_path_dir, const char * a_pattern, bool a_remove_at_destroy)
    {
      _set_defaults ();
      set_remove_at_destroy  (a_remove_at_destroy);
      string pattern = a_pattern;
      string path_dir = a_path_dir;
      create (path_dir, pattern);
      return;
    }
    
    // ctor:
    temp_file::temp_file (string a_path_dir, string a_pattern, bool a_remove_at_destroy)
    {
      _set_defaults ();
      set_remove_at_destroy  (a_remove_at_destroy);
      create (a_path_dir, a_pattern);
      return;
    }

    // dtor:
    temp_file::~temp_file ()
    {
      if (_remove_at_destroy_)
        {
          if (g_devel)
            {
              clog << "DEVEL: datatools::utils::temp_file::dtor: "
                   << "Deleting the file with name '"
                   << _filename_ << "'" << endl;
            }
          remove ();
        }
      if (_template_ != 0)
        {
          delete _template_;
          _template_ = 0;
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
                  << _filename_ << "' has already been created !";
          throw logic_error (message.str ());
        } 
      if (a_path_dir.empty ())
        {
          _path_dir_ = ".";
        }
      else
        {
          _path_dir_ = a_path_dir;
          fetch_path_with_env (_path_dir_);
        }
      if (! boost::filesystem::is_directory (_path_dir_))
        {
          ostringstream message;
          message << "datatools::utils::temp_file::create: "
                  << "Directory with name '"
                  << _path_dir_ << "' does not exist !";
          throw runtime_error (message.str ());   
        }

      if (a_pattern.empty ())
        {
          _pattern_ = DEFAULT_PATTERN;
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
              throw logic_error (message.str ());         
            }
          _pattern_ = a_pattern;
        }
      ostringstream filename_oss;
      filename_oss << _path_dir_ << '/' << _pattern_ << "XXXXXX";
      _full_pattern_ = filename_oss.str ();
      if (devel) clog << "DEVEL: datatools::utils::temp_file::create: full_pattern is '" << _full_pattern_ << "' !" << endl;

      _template_ = new char [_full_pattern_.size () + 10];
      _template_[0] = '\0';
      if (devel) clog << "DEVEL: datatools::utils::temp_file::create: buffer is allocated !" << endl;
      for (int i = 0; i < (int) _full_pattern_.size (); i++)
        {
          _template_[i] = _full_pattern_[i];
        }
      _template_[_full_pattern_.size ()] = '\0';
      if (devel) clog << "DEVEL: datatools::utils::temp_file::create: template= '" << _template_ << "'" << endl;

      int err = mkstemp (_template_);
      if (err == -1)
        {
          ostringstream message;
          message << "datatools::utils::temp_file::create: "
                  << "Cannot create temporary file using pattern '"
                  << _full_pattern_ << "' !";
          if (_template_ != 0) 
            {
              delete _template_;
            }
          _template_ = 0;
          throw runtime_error (message.str ());
        } 
      if (devel) clog << "DEVEL: datatools::utils::temp_file::create: Template is '" << _template_ << "' !" << endl;
      _filename_ = _template_;
      if (devel) clog << "DEVEL: datatools::utils::temp_file::create: Filename is '" << _filename_ << "' !" << endl;
      return;
    }

    const string & temp_file::get_filename () const
    {
      return _filename_;
    }

    void temp_file::set_remove_at_destroy (bool r_)
    {
      _remove_at_destroy_ = r_;
      return;
    }
      
    void temp_file::close ()
    {
      if (is_read_open ())
        {
          if (g_devel) clog << "DEVEL: datatools::utils::temp_file::close: closing read file mode !" << endl;
          _in_.close ();
          _read_open_ = false;
        }      
      if (is_write_open ())
        {
          if (g_devel) clog << "DEVEL: datatools::utils::temp_file::close: closing write file mode !" << endl;
          _out_.close ();
          _write_open_ = false;
        }      
      return;
    }
    
    void temp_file::remove ()
    {
      if (g_devel) clog << "DEVEL: datatools::utils::temp_file::remove: Entering..." << endl;
      close ();
      if (! _filename_.empty ())
        {
          int err = unlink (_filename_.c_str ());
          if (err == -1)
            {
              ostringstream message;
              message << "datatools::utils::temp_file::remove: "
                      << "Cannot delete temporary file '"
                      << _filename_ << "' !";
              throw runtime_error (message.str ());
            } 
        }
      if (_template_ != 0)
        {
          delete _template_;
          _template_ = 0;
        }     
      _set_defaults ();
      if (g_devel) clog << "DEVEL: datatools::utils::temp_file::remove: Exiting." << endl;
      return;
    }

  } // namespace utils

} // namespace datatools 

// end of temporary_files.cc

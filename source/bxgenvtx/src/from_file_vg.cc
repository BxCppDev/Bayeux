// -*- mode: c++ ; -*- 
/* from_file_vg.cc
 */

#include <genvtx/from_file_vg.h>

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <list>

#include <boost/filesystem.hpp>

#include <datatools/utils.h>
#include <datatools/units.h>

namespace genvtx {

  using namespace std;

  GENVTX_VG_REGISTRATION_IMPLEMENT(from_file_vg,"genvtx::from_file_vg");

  const double from_file_vg::LENGTH_UNIT = 1.0;

  bool from_file_vg::is_open  () const
  {
    return _open_;
  }

  const string & from_file_vg::get_filename () const
  {
    return _filename_;
  }
  
  void from_file_vg::set_filename (const string & filename_)
  {
    _assert_lock ("genvtx::from_file_vg::set_filename",
                  "Cannot change the vertex source file name for a source file is already opened !");
   if (_open_)
      {
        throw logic_error ("genvtx::from_file_vg::set_filename: Cannot change source file name as source file is already opened !");
      }
    _filename_ = filename_;
    return;
  }

  void from_file_vg::_open_source ()
  {
    if (_open_)
      {
        throw logic_error ("genvtx::from_file_vg::_open_source: Source file is already opened !");
      }
    if (_filename_.empty ())
      {
        throw logic_error ("genvtx::from_file_vg::_open_source: Missing source file name !");
      } 

    datatools::fetch_path_with_env (_filename_);

    if (! boost::filesystem::exists (_filename_)) 
      {
        ostringstream message;
        message << "genvtx::from_file_vg::_open_source: " 
                << "Source file '" << _filename_ << "' does not exist !";
        throw logic_error (message.str ());
      }
    _source_.open (_filename_.c_str ());
    if (! _source_)
      {
        ostringstream message;
        message << "genvtx::from_file_vg::_open_source: " 
                << "Cannot open source file '" << _filename_ << "' !";
        throw logic_error (message.str ());
      }
    _open_ = true;
    _read_next ();
    return;
  }

  void from_file_vg::_close_source ()
  {
    if (! _open_)
      {
        throw logic_error ("genvtx::from_file_vg::_close_source: Source file is not opened !");
      }
    _source_.close ();
    _filename_ = "";
    geomtools::invalidate (_next_);
    _open_ = true;
    return;
  }

  void from_file_vg::set_length_unit (double lu_)
  {
    _assert_lock ("genvtx::from_file_vg::set_length_unit"); 
    if (_length_unit_ <= 0.0)
      {
        throw logic_error ("genvtx::from_file_vg::set_length_unit: Invalid length unit !");   
      }
    _length_unit_ = lu_;
    return;
  }
  
  double from_file_vg::get_length_unit () const
  {
    return _length_unit_;
  }

  GENVTX_VG_IS_INITIALIZED_IMPLEMENT_HEAD(from_file_vg)
  {
    return is_open ();
  }

  GENVTX_VG_INITIALIZE_IMPLEMENT_HEAD(from_file_vg,
                                      configuration_,
                                      service_manager_,
                                      vgens_)
  {
    _assert_lock ("genvtx::from_file_vg::initialize");
    using namespace std;
    bool devel = false;
    double lunit = LENGTH_UNIT;
    string lunit_str;

    // parameters of the cut:
    string filename;

    if (_filename_.empty ())
      {
        if (configuration_.has_key ("filename"))
          {
            filename = configuration_.fetch_string ("filename");
          }
        
        if (configuration_.has_key ("length_unit"))
          {
            lunit_str = configuration_.fetch_string ("length_unit");
            lunit = datatools::units::get_length_unit_from (lunit_str);
          }
        
        set_length_unit (lunit);
      }

    _open_source ();
    return;
  }

  GENVTX_VG_RESET_IMPLEMENT_HEAD(from_file_vg)
  {
    if (_open_)
      {
        _close_source ();
      }    
    return;
  }

  // Constructor :
  GENVTX_VG_CONSTRUCTOR_IMPLEMENT_HEAD(from_file_vg)
  {
    _filename_ = "";
    _open_ = false;
    geomtools::invalidate (_next_);
    _length_unit_ = LENGTH_UNIT;
    return;
  }
  
  // Destructor :
  GENVTX_VG_DEFAULT_DESTRUCTOR_IMPLEMENT(from_file_vg)

  /*
  from_file_vg::from_file_vg (const std::string & filename_)
  {
    _filename_ = "";
    _open_ = false;
    geomtools::invalidate (_next_);
    _length_unit_ = LENGTH_UNIT;
    set_filename (filename_);
    _open_source ();
    return;
  }
  */

  void from_file_vg::_read_next ()
  {
    if (! _open_)
      {
        throw logic_error ("genvtx::from_file_vg::_read_next: Source file is not opened !");
        //_open_source ();
      }
    double x, y, z;
    geomtools::invalidate (_next_);

    bool goon = true;
    while (goon)
      {
        goon = false;
        string line;
        std::getline (_source_, line);
        // detect special cases:
        {
          istringstream iss (line);
          string word;
          iss >> word;
          // Skip blank lines:
          if (word.empty ())
            {
              goon = true;
            }
          // Skip commented line:
          else if (word[0] == '#')
            {
              goon = true;
              continue;
            }
        }
        if (! goon) 
          {
            istringstream iss (line);
            iss >> x >> y >> z >> ws;
            if (! iss)
              {
                ostringstream message;
                message << "genvtx::from_file_vg::_read_next: " 
                        << "'x y z' format error at invalid line '" << line << "' !";
                throw logic_error (message.str ());
              }
            _next_.set (x, y, z);
            break;
          }
        if (_source_.eof ())
          {
            break;
          }
      }
    return;
  }

  bool from_file_vg::_has_next ()
  {
    return geomtools::is_valid (_next_);
  }

  GENVTX_VG_HAS_NEXT_VERTEX_IMPLEMENT_HEAD(from_file_vg)
  {
    return const_cast<from_file_vg*>(this)->_has_next ();
  }
 
  GENVTX_VG_SHOOT_VERTEX_IMPLEMENT_HEAD(from_file_vg,random_,vertex_)
  {
    if (! is_initialized ())
      {
        throw logic_error ("genvtx::from_file_vg::_shoot_vertex: Not initialized !"); 
      }
    // here apply the length unit:
    vertex_ = _next_ * _length_unit_;
    _read_next ();
    return;
  }

} // end of namespace genvtx

// end of from_file_vg.cc

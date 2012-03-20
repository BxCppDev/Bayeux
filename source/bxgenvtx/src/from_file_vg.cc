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

#include <datatools/utils/utils.h>
#include <datatools/utils/units.h>

namespace genvtx {

  using namespace std;

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

    datatools::utils::fetch_path_with_env (_filename_);

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
  
  // ctor:
  from_file_vg::from_file_vg ()
  {
    _filename_ = "";
    _open_ = false;
    geomtools::invalidate (_next_);
    _length_unit_ = LENGTH_UNIT;
    return;
  }
  
  from_file_vg::from_file_vg (const string & filename_)
  {
    _filename_ = "";
    _open_ = false;
    geomtools::invalidate (_next_);
    _length_unit_ = LENGTH_UNIT;
    set_filename (filename_);
    _open_source ();
    return;
  }
  
  // dtor:
  from_file_vg::~from_file_vg ()
  {
    if (_open_)
      {
        _close_source ();
      }
    return;
  }

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

  bool from_file_vg::has_next_vertex () const
  {
    return const_cast<from_file_vg*>(this)->_has_next ();
  }

  bool from_file_vg::_has_next ()
  {
    return geomtools::is_valid (_next_);
  }
 
  void from_file_vg::_shoot_vertex (mygsl::rng & random_, 
                                    geomtools::vector_3d & vertex_)
  {
    // here apply the length unit:
    vertex_ = _next_ * _length_unit_;
    _read_next ();
    return;
  }

  /**********************************************************************/

  // static method used within a vertex generator factory:
  i_vertex_generator * 
  from_file_vg::create (const datatools::utils::properties & configuration_, void * user_)
  {
    //cerr << "DEVEL: genvtx::from_file_vg::create: Entering..." << endl;
    //configuration_.tree_dump (cerr, "from_file_vg::create: configuration:", "DEVEL: ");
    using namespace std;
    bool devel = false;
    double lunit = LENGTH_UNIT;
    string lunit_str;

    // parameters of the cut:
    string filename;

    if (configuration_.has_key ("filename"))
      {
        filename = configuration_.fetch_string ("filename");
      }

    if (configuration_.has_key ("length_unit"))
      {
        lunit_str = configuration_.fetch_string ("length_unit");
        lunit = datatools::utils::units::get_length_unit_from (lunit_str);
      }

    // create a new parameterized 'from_file_vg' instance:
    from_file_vg * ptr = new from_file_vg (filename);
    ptr->set_length_unit (lunit);
    return ptr; 
  }

  string from_file_vg::vg_id () const
  {
    return "genvtx::from_file_vg";
  }

  vg_creator_type from_file_vg::vg_creator () const
  {
    return from_file_vg::create;
  }

  // register this creator:   
  i_vertex_generator::creator_registration<from_file_vg> from_file_vg::g_cr_;
  
} // end of namespace genvtx

// end of from_file_vg.cc

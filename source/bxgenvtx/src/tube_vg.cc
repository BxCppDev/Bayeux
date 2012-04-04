// -*- mode: c++ ; -*- 
/* tube_vg.cc 
 */

#include <genvtx/tube_vg.h>

#include <stdexcept>
#include <sstream>

#include <datatools/utils/units.h>
#include <datatools/utils/utils.h>
#include <datatools/utils/properties.h>

namespace genvtx {

  using namespace std;
  bool tube_vg::g_debug = false;

  void tube_vg::_assert_lock_ (const string & where_)
  {
    if (_initialized_)
      {
        ostringstream message;
        message << "genvtx::tube_vg::_assert_lock_: " << where_ << ": "
                << "Object is locked !";
        throw logic_error (message.str());
      }
    return;
  }

  bool tube_vg::is_initialized () const
  {
    return _initialized_;
  }

  int tube_vg::get_mode () const
  {
    return _mode_;
  }

  void tube_vg::set_mode (int mode_)
  {
    if ((mode_ !=  MODE_BULK) && (mode_ !=  MODE_SURFACE))
      {
        throw logic_error ("genvtx::tube_vg::set_mode: Invalid mode !");
      }
    _mode_ = mode_;
    return;
  }

  void tube_vg::set_surface_mask (int surface_mask_)
  {
    _assert_lock_ ("genvtx::tube_vg::set_surface_mask");
    _surface_mask_ = surface_mask_;
    return;
  }

  void tube_vg::set_skin_skip (double skin_skip_)
  {
    _assert_lock_ ("genvtx::tube_vg::set_surface_mask");
    _skin_skip_ = skin_skip_;
    return;
  }

  void tube_vg::set_skin_thickness (double skin_thickness_)
  {
    _assert_lock_ ("genvtx::tube_vg::set_skin_thickness");
    _skin_thickness_ = skin_thickness_;
    return;
  }

  void tube_vg::set_bulk (double skin_thickness_)
  {
    _assert_lock_ ("genvtx::tube_vg::set_bulk");
    _mode_ = MODE_BULK;
    return;
  }

  void tube_vg::set_surface (int surface_mask_)
  {
    _assert_lock_ ("genvtx::tube_vg::set_surface");
    _mode_ = MODE_SURFACE;
    set_surface_mask (surface_mask_);
    return;
  }

  void tube_vg::set_tube (const geomtools::tube & tube_)
  {
    _assert_lock_ ("genvtx::tube_vg::set_tube");
    _tube_ = tube_;
    return;
  }

  const geomtools::tube & tube_vg::get_tube () const
  {
    return _tube_;
  }
  
  // ctor:
  tube_vg::tube_vg ()
  {
    _initialized_ = false;
    _reset_ ();
    return;
  }
  
  // dtor:
  tube_vg::~tube_vg ()
  {
    return;
  }

  void tube_vg::init ()
  {
    if (_initialized_)
      {
        throw logic_error ("genvtx::tube_vg::init: Already initialized !");
      }
    _init_ ();
    _initialized_ = true;
    return;
  }

  void tube_vg::reset ()
  {
    if (! _initialized_)
      {
        throw logic_error ("genvtx::tube_vg::reset: Not initialized !");
      }
    _reset_ ();
    _initialized_ = false;
    return;
  }

  void tube_vg::_init_ ()
  {
    bool devel = g_debug;
    if (_mode_ == MODE_SURFACE) 
      {
        if (_surface_mask_ == 0)
          {
            throw logic_error ("genvtx::tube_vg::_init_: Surface mask is null !");
          }
        double s = _tube_.get_surface (_surface_mask_);
        if (devel) clog << "DEVEL: genvtx::tube_vg::_init_: Total surface = " << s << endl;
        _sum_weight_[0] = _tube_.get_surface (_surface_mask_ & geomtools::tube::FACE_OUTER_SIDE);
        _sum_weight_[1] = _tube_.get_surface (_surface_mask_ & geomtools::tube::FACE_BOTTOM);
        _sum_weight_[2] = _tube_.get_surface (_surface_mask_ & geomtools::tube::FACE_TOP);
        _sum_weight_[3] = _tube_.get_surface (_surface_mask_ & geomtools::tube::FACE_INNER_SIDE);
        for (int i = 0; i < 3; i++) 
          {
            _sum_weight_[i] /= s;
            if (i > 0) 
              {
                _sum_weight_[i] += _sum_weight_[i - 1];
              }
            if (devel) clog << "DEVEL: genvtx::tube_vg::_init_: Surface weight [" << i << "] = " << _sum_weight_[i] << endl;
          }
      }
    /*
    _r1_sqr_ = std::pow (_tube_.get_inner_r () + 0.5 * _skin_thickness_, 2);
    _r2_sqr_ = std::pow (_tube_.get_outer_r () - 0.5 * _skin_thickness_, 2);
    if (_r1_sqr_ > _r2_sqr_)
      {
        throw logic_error ("genvtx::tube_vg::_init_: Thickness is too large !");
      }
    */
   return;
  }

  void tube_vg::_reset_ ()
  {
    _mode_ = MODE_DEFAULT;
    _surface_mask_ = geomtools::tube::FACE_ALL;
    _skin_skip_ = 0.0;
    _skin_thickness_ = 0.0;
    for (int i = 0; i < 3; i++)
      {
        _sum_weight_[i] = 0.0;
      } 
    //datatools::utils::invalidate(_r1_sqr_);
    //datatools::utils::invalidate(_r2_sqr_);
    return;
  }

  void tube_vg::dump (ostream & out_) const
  {
    out_ << "genvtx::tube_vg::dump: " << endl;
    out_ << "|-- " << "Tube: " << _tube_ << endl;
    out_ << "|-- " << "Mode: " << _mode_ << endl;
    out_ << "|-- " << "Surface mask: " << _surface_mask_ << endl;
    out_ << "|-- " << "Skin skip: " << _skin_skip_ << endl;
    out_ << "`-- " << "Skin thickness: " << _skin_thickness_ << endl;
    return;
  }
  
  void tube_vg::_shoot_vertex (mygsl::rng & random_, 
                               geomtools::vector_3d & vertex_)
  {
    bool devel = g_debug;
    if (! _initialized_)
      {
        init ();
      }
    geomtools::invalidate (vertex_);
    double r, t;
    double t_max = 2. * M_PI;
    t = random_.uniform () * t_max;
    double x, y, z;
    if (_mode_ == MODE_BULK) 
      {
        if (devel) 
          {
            clog << "DEVEL: genvtx::tube_vg::_shoot_vertex: "
                 << "Bulk mode..." << endl;
          }
        double r_min = _tube_.get_inner_r () + 0.5 * _skin_thickness_;
        double r_max = _tube_.get_outer_r () - 0.5 * _skin_thickness_;
        double r_min2 = r_min * r_min;
        double r_max2 = r_max * r_max;
        r = std::sqrt (r_min2 + random_.uniform () * (r_max2 - r_min2));
        z = (random_.uniform () - 0.5) * (_tube_.get_z () - _skin_thickness_);
      }

    if (_mode_ == MODE_SURFACE) 
      {
        if (devel) 
          {
            clog << "DEVEL: genvtx::tube_vg::_shoot_vertex: "
                 << "Surface mode..." << endl;
          }
        double r0 = random_.uniform ();

        double delta_thick = 0.0;
        if (_skin_thickness_ > 0.0) 
          {
            double r3 = random_.uniform ();
            delta_thick = (r3 - 0.5) * _skin_thickness_;
          }
        
        // outer surface :
        if (r0 < _sum_weight_[0]) 
          {
            double r_min  = _tube_.get_outer_r () 
              + _skin_skip_
              - 0.5 * _skin_thickness_;
            double r_max  = r_min + _skin_thickness_;
            double r_min2 = r_min * r_min;
            double r_max2 = r_max * r_max;
            r = std::sqrt (r_min2 + random_.uniform () * (r_max2 - r_min2));
            z = +(random_.uniform () - 0.5) * (_tube_.get_z ());
          }
        // bottom surface :
        else if (r0 < _sum_weight_[1]) 
          {
            double r_max = _tube_.get_outer_r ();
            double r_min = _tube_.get_inner_r ();
            double r_max2 = r_max * r_max;
            double r_min2 = r_min * r_min;
            r = std::sqrt (r_min2 + random_.uniform () * (r_max2 - r_min2));
            z = -(_tube_.get_half_z () + _skin_skip_ + delta_thick);
          }
        // top_surface :
        else if (r0 < _sum_weight_[2]) 
          {
            double r_max = _tube_.get_outer_r ();
            double r_min = _tube_.get_inner_r ();
            double r_max2 = r_max * r_max;
            double r_min2 = r_min * r_min;
            r = std::sqrt (r_min2 + random_.uniform () * (r_max2 - r_min2));
            z = +(_tube_.get_half_z () + _skin_skip_ + delta_thick);
          }
        // inner surface :
        else 
          {
            double r_min  = _tube_.get_inner_r () 
              - _skin_skip_
              - 0.5 * _skin_thickness_;
            double r_max  = r_min + _skin_thickness_;
            double r_min2 = r_min * r_min;
            double r_max2 = r_max * r_max;
            r = (sqrt (r_min2 + random_.uniform () * (r_max2 - r_min2)));
            z = +(random_.uniform () - 0.5) * (_tube_.get_z ());
          }
     }
    x = r * cos (t); 
    y = r * sin (t); 
    vertex_.set (x,y, z);
    return;
  }

  /**********************************************************************/

  // static method used within a vertex generator factory:
  i_vertex_generator * 
  tube_vg::create (const datatools::utils::properties & configuration_, void * user_)
  {
    cerr << "DEVEL: genvtx::tube_vg::create: Entering..." << endl;
    configuration_.tree_dump (cerr, "genvtx::tube_vg::create: configuration:", "DEVEL: ");
    using namespace std;
    bool devel = false;
    //devel = true;

    // parameters of the tube vertex generator:
    double x, y, z;
    x = y = z = 1.0;
    int mode =  tube_vg::MODE_DEFAULT;
    double skin_skip = 0.0;
    double skin_thickness = 0.0;
    int surface_mask = 0;

    if (configuration_.has_key ("mode"))
      {
        string mode_str = configuration_.fetch_string ("mode");
        if (mode_str == "surface")
          {
            mode = MODE_SURFACE;
          }
        else if (mode_str == "bulk")
          {
            mode = MODE_BULK;
          }
        else
          {
            ostringstream message;
            message << "genvtx::tube_vg::create: Invalid mode '"
                    << mode_str << "' !";
            throw logic_error (message.str ());
          }
      }

    if (configuration_.has_key ("skin_skip"))
      {
        skin_skip = configuration_.fetch_real ("skin_skip");
      }

    if (configuration_.has_key ("skin_thickness"))
      {
        skin_thickness = configuration_.fetch_real ("skin_thickness");
      }

    // create a new parameterized 'tube_vg' instance:
    tube_vg * ptr = new tube_vg;
    //ptr->set_spot (x, y, z);
    return ptr; 
  }

  string tube_vg::vg_id () const
  {
    return "genvtx::tube_vg";
  }

  vg_creator_type tube_vg::vg_creator () const
  {
    return tube_vg::create;
  }

  // register this creator:   
  i_vertex_generator::creator_registration<tube_vg> tube_vg::g_cr_;
  
} // end of namespace genvtx

// end of tube_vg.cc

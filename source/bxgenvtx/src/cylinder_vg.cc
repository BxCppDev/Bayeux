// -*- mode: c++ ; -*- 
/* cylinder_vg.cc 
 */

#include <genvtx/cylinder_vg.h>

#include <stdexcept>
#include <sstream>

#include <datatools/utils/units.h>
#include <datatools/utils/properties.h>

namespace genvtx {

  using namespace std;

  bool cylinder_vg::g_debug = false;

  void cylinder_vg::_assert_lock_ (const string & where_)
  {
    if (_initialized_)
      {
        ostringstream message;
        message << "genvtx::cylinder_vg::_assert_lock_: " << where_ << ": "
                << "Object is locked !";
        throw runtime_error (message.str());
      }
    return;
  }

  bool cylinder_vg::is_initialized () const
  {
    return _initialized_;
  }

  int cylinder_vg::get_mode () const
  {
    return _mode_;
  }

  void cylinder_vg::set_mode (int mode_)
  {
    if ((mode_ !=  MODE_BULK) && (mode_ !=  MODE_SURFACE))
      {
        throw runtime_error ("genvtx::cylinder_vg::set_mode: Invalid mode !");
      }
    _mode_ = mode_;
    return;
  }

  void cylinder_vg::set_surface_mask (int surface_mask_)
  {
    _assert_lock_ ("genvtx::cylinder_vg::set_surface_mask");
    _surface_mask_ = surface_mask_;
    return;
  }

  void cylinder_vg::set_skin_skip (double skin_skip_)
  {
    _assert_lock_ ("genvtx::cylinder_vg::set_surface_mask");
    _skin_skip_ = skin_skip_;
    return;
  }

  void cylinder_vg::set_skin_thickness (double skin_thickness_)
  {
    _assert_lock_ ("genvtx::cylinder_vg::set_skin_thickness");
    _skin_thickness_ = skin_thickness_;
    return;
  }

  void cylinder_vg::set_bulk (double skin_thickness_)
  {
    _assert_lock_ ("genvtx::cylinder_vg::set_bulk");
    _mode_ = MODE_BULK;
    return;
  }

  void cylinder_vg::set_surface (int surface_mask_)
  {
    _assert_lock_ ("genvtx::cylinder_vg::set_surface");
    _mode_ = MODE_SURFACE;
    set_surface_mask (surface_mask_);
    return;
  }

  void cylinder_vg::set_cylinder (const geomtools::cylinder & cylinder_)
  {
    _assert_lock_ ("genvtx::cylinder_vg::set_cylinder");
    _cylinder_ = cylinder_;
    return;
  }

  const geomtools::cylinder & cylinder_vg::get_cylinder () const
  {
    return _cylinder_;
  }
  
  // ctor:
  cylinder_vg::cylinder_vg ()
  {
    _initialized_ = false;
    _reset_ ();
    return;
  }
  
  // dtor:
  cylinder_vg::~cylinder_vg ()
  {
    return;
  }

  void cylinder_vg::init ()
  {
    if (_initialized_)
      {
        throw runtime_error ("genvtx::cylinder_vg::init: Already initialized !");
      }
    _init_ ();
    _initialized_ = true;
    return;
  }

  void cylinder_vg::reset ()
  {
    if (! _initialized_)
      {
        throw runtime_error ("genvtx::cylinder_vg::reset: Not initialized !");
      }
    _reset_ ();
    _initialized_ = false;
    return;
  }

  void cylinder_vg::_init_ ()
  {
    bool devel = g_debug;
    if (_mode_ == MODE_SURFACE) 
      {
        if (_surface_mask_ == 0)
          {
            throw runtime_error ("genvtx::cylinder_vg::_init_: Surface mask is null !");
          }
        double s = _cylinder_.get_surface (_surface_mask_);
        if (devel) clog << "DEVEL: genvtx::cylinder_vg::_init_: Total surface = " << s << endl;
        _sum_weight_[0] = _cylinder_.get_surface (_surface_mask_ & geomtools::cylinder::FACE_SIDE);
        _sum_weight_[1] = _cylinder_.get_surface (_surface_mask_ & geomtools::cylinder::FACE_BOTTOM);
        _sum_weight_[2] = _cylinder_.get_surface (_surface_mask_ & geomtools::cylinder::FACE_TOP);
        for (int i = 0; i < 3; i++) 
          {
            _sum_weight_[i] /= s;
            if (i > 0) 
              {
                _sum_weight_[i] += _sum_weight_[i - 1];
              }
            if (devel) clog << "DEVEL: genvtx::cylinder_vg::_init_: Surface weight [" << i << "] = " << _sum_weight_[i] << endl;
          }
      }
    return;
  }

  void cylinder_vg::_reset_ ()
  {
    _mode_ = MODE_DEFAULT;
    _surface_mask_ = geomtools::cylinder::FACE_ALL;
    _skin_skip_ = 0.0;
    _skin_thickness_ = 0.0;
    for (int i = 0; i < 3; i++)
      {
        _sum_weight_[i] = 0.0;
      }
    return;
  }

  void cylinder_vg::dump (ostream & out_) const
  {
    out_ << "genvtx::cylinder_vg::dump: " << endl;
    out_ << "|-- " << "Cylinder: " << _cylinder_ << endl;
    out_ << "|-- " << "Mode: " << _mode_ << endl;
    out_ << "|-- " << "Surface mask: " << _surface_mask_ << endl;
    out_ << "|-- " << "Skin skip: " << _skin_skip_ << endl;
    out_ << "`-- " << "Skin thickness: " << _skin_thickness_ << endl;
    return;
  }
  
  void cylinder_vg::_shoot_vertex (mygsl::rng & random_, 
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
            clog << "DEVEL: genvtx::cylinder_vg::_shoot_vertex: "
                 << "Bulk mode..." << endl;
          }
        double r_max = _cylinder_.get_r () - 0.5 * _skin_thickness_;
        r = sqrt (random_.uniform ()) * r_max;
        z = (random_.uniform () - 0.5) * (_cylinder_.get_z () - _skin_thickness_);
      }

    if (_mode_ == MODE_SURFACE) 
      {
        if (devel) 
          {
            clog << "DEVEL: genvtx::cylinder_vg::_shoot_vertex: "
                 << "Surface mode..." << endl;
          }
        double r0 = random_.uniform ();

        double delta_thick = 0.0;
        if (_skin_thickness_ > 0.0) 
          {
            double r3 = random_.uniform ();
            delta_thick = (r3 - 0.5) * _skin_thickness_;
          }
        
        if (r0 < _sum_weight_[0]) 
          {
            double r_min  = _cylinder_.get_r () 
              + _skin_skip_
              - 0.5 * _skin_thickness_;
            double r_max  = r_min + _skin_thickness_;
            double r_min2 = r_min * r_min;
            double r_max2 = r_max * r_max;
            r = (sqrt (r_min2 + random_.uniform () * (r_max2 - r_min2)));
            z = +(random_.uniform () - 0.5) * (_cylinder_.get_z ());
          }
        else if (r0 < _sum_weight_[1]) 
          {
            double r_max = _cylinder_.get_r ();
            r = sqrt (random_.uniform ()) * r_max;
            z = -(_cylinder_.get_half_z () + _skin_skip_ + delta_thick);
          }
        else if (r0 < _sum_weight_[2]) 
          {
            double r_max = _cylinder_.get_r ();
            r = sqrt (random_.uniform ()) * r_max;
            z = +(_cylinder_.get_half_z () + _skin_skip_ + delta_thick);
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
  cylinder_vg::create (const datatools::utils::properties & configuration_, void * user_)
  {
    cerr << "DEVEL: genvtx::cylinder_vg::create: Entering..." << endl;
    configuration_.tree_dump (cerr, "genvtx::cylinder_vg::create: configuration:", "DEVEL: ");
    using namespace std;
    bool devel = false;
    //devel = true;

    // parameters of the cylinder vertex generator:
    double x, y, z;
    x = y = z = 1.0;
    int mode =  cylinder_vg::MODE_DEFAULT;
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
            message << "genvtx::cylinder_vg::create: Invalid mode '"
                    << mode_str << "' !";
            throw runtime_error (message.str ());
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

    // create a new parameterized 'cylinder_vg' instance:
    cylinder_vg * ptr = new cylinder_vg;
    //ptr->set_spot (x, y, z);
    return ptr; 
  }

  string cylinder_vg::vg_id () const
  {
    return "genvtx::cylinder_vg";
  }

  vg_creator_type cylinder_vg::vg_creator () const
  {
    return cylinder_vg::create;
  }

  // register this creator:   
  i_vertex_generator::creator_registration<cylinder_vg> cylinder_vg::g_cr_;
  
} // end of namespace genvtx

// end of cylinder_vg.cc

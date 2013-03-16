// -*- mode: c++ ; -*- 
/* tube_vg.cc 
 */

#include <genvtx/tube_vg.h>

#include <stdexcept>
#include <sstream>

#include <mygsl/rng.h>

#include <datatools/units.h>
#include <datatools/utils.h>
#include <datatools/properties.h>

namespace genvtx {

  using namespace std;

  GENVTX_VG_REGISTRATION_IMPLEMENT(tube_vg,"genvtx::tube_vg");

  int tube_vg::get_mode () const
  {
    return _mode_;
  }

  void tube_vg::set_mode (int mode_)
  {
    _assert_lock ("genvtx::tube_vg::set_mode");
    if ((mode_ !=  MODE_BULK) && (mode_ !=  MODE_SURFACE))
      {
        throw logic_error ("genvtx::tube_vg::set_mode: Invalid mode !");
      }
    _mode_ = mode_;
    return;
  }

  void tube_vg::set_surface_mask (int surface_mask_)
  {
    _assert_lock ("genvtx::tube_vg::set_surface_mask");
    _surface_mask_ = surface_mask_;
    return;
  }

  void tube_vg::set_skin_skip (double skin_skip_)
  {
    _assert_lock ("genvtx::tube_vg::set_surface_mask");
    _skin_skip_ = skin_skip_;
    return;
  }

  void tube_vg::set_skin_thickness (double skin_thickness_)
  {
    _assert_lock ("genvtx::tube_vg::set_skin_thickness");
    _skin_thickness_ = skin_thickness_;
    return;
  }

  void tube_vg::set_bulk (double skin_thickness_)
  {
    _assert_lock ("genvtx::tube_vg::set_bulk");
    _mode_ = MODE_BULK;
    return;
  }

  void tube_vg::set_surface (int surface_mask_)
  {
    _assert_lock ("genvtx::tube_vg::set_surface");
    _mode_ = MODE_SURFACE;
    set_surface_mask (surface_mask_);
    return;
  }

  void tube_vg::set_tube (const geomtools::tube & tube_)
  {
    _assert_lock ("genvtx::tube_vg::set_tube");
    _tube_ = tube_;
    return;
  }

  const geomtools::tube & tube_vg::get_tube () const
  {
    return _tube_;
  }
  
  GENVTX_VG_IS_INITIALIZED_IMPLEMENT_HEAD(tube_vg)
  {
    return _initialized_;
  }
  
  // Constructor :
  GENVTX_VG_CONSTRUCTOR_IMPLEMENT_HEAD(tube_vg)
  {
    _initialized_ = false;
    _tube_.reset ();
    _set_defaults_ ();
    return;
  }
  
  // Destructor :
  GENVTX_VG_DEFAULT_DESTRUCTOR_IMPLEMENT(tube_vg)
 

  GENVTX_VG_INITIALIZE_IMPLEMENT_HEAD(tube_vg,setup_,service_manager_,vgens_)
  {
    if (is_debug ()) std::cerr << "DEBUG: genvtx::tube_vg::initialize: Entering..." << std::endl;
    using namespace std;
    bool devel = false;
    if (is_initialized ())
      {
        throw logic_error ("genvtx::tube_vg::initialize: Already initialized !");
      }

    // parameters of the tube vertex generator:
    int    mode = MODE_INVALID;
    double skin_skip = 0.0;
    double skin_thickness = 0.0;
    int    surface_mask = geomtools::tube::FACE_NONE;
    double lunit = CLHEP::mm;
    string lunit_str;
    bool treat_mode           = false;
    bool treat_surface_mask   = false;
    bool treat_skin_skip      = false;
    bool treat_skin_thickness = false;

    if (_mode_ == MODE_INVALID)
      {
        if (setup_.has_key ("mode"))
          {
            string mode_str = setup_.fetch_string ("mode");
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
                message << "genvtx::tube_vg::initialize: Invalid mode '"
                        << mode_str << "' !";
                throw logic_error (message.str ());
              }
            treat_mode = true;
          }
      }

    if (setup_.has_key ("length_unit"))
      {
        lunit_str = setup_.fetch_string ("length_unit");
        lunit = datatools::units::get_length_unit_from (lunit_str);
      }
    
    if (setup_.has_key ("skin_skip"))
      {
        skin_skip = setup_.fetch_real ("skin_skip");
        skin_skip *= lunit;
        treat_skin_skip = true;
      }

    if (setup_.has_key ("skin_thickness"))
      {
        skin_thickness = setup_.fetch_real ("skin_thickness");
        skin_thickness *= lunit;
        treat_skin_thickness = true;
      }

    if (mode == MODE_SURFACE)
      {
        std::vector<std::string> surfaces;
        if (setup_.has_key ("surfaces"))
          {
            setup_.fetch ("surfaces", surfaces);
            treat_surface_mask = true;
          }

        for (int i = 0; i < surfaces.size (); i++)
          {
            if (surfaces[i] == "all")
              {
                surface_mask = geomtools::tube::FACE_ALL;
                break;
              }
            else if (surfaces[i] == "outer_side")
              {
                surface_mask |= geomtools::tube::FACE_OUTER_SIDE;
              }
            else if (surfaces[i] == "inner_side")
              {
                surface_mask |= geomtools::tube::FACE_INNER_SIDE;
              }
            else if (surfaces[i] == "bottom")
              {
                surface_mask |= geomtools::tube::FACE_BOTTOM;
              }
            else if (surfaces[i] == "top")
              {
                surface_mask |= geomtools::tube::FACE_TOP;
              }
          }
      }
 
    if (treat_mode) set_mode (mode);
    if (treat_skin_skip) set_skin_skip (skin_skip);
    if (treat_skin_thickness) set_skin_thickness (skin_thickness);
    if (mode == MODE_SURFACE && treat_surface_mask) 
      {
        set_surface_mask (surface_mask);
      }

    if (! _tube_.is_valid ())
      {
        double tube_inner_r, tube_outer_r, tube_z;
        datatools::invalidate (tube_inner_r);
        datatools::invalidate (tube_outer_r);
        datatools::invalidate (tube_z);
        double length_unit = CLHEP::millimeter;
        
        if (setup_.has_key ("length_unit"))
          {
            std::string length_unit_str = setup_.fetch_string ("length_unit");
            length_unit = datatools::units::get_length_unit_from (length_unit_str);
          }

        if (setup_.has_key ("tube.inner_r"))
          {
            tube_inner_r = setup_.fetch_real ("tube.inner_r");
          }

        if (setup_.has_key ("tube.outer_r"))
          {
            tube_outer_r = setup_.fetch_real ("tube.outer_r");
          }

        if (setup_.has_key ("tube.z"))
          {
            tube_z = setup_.fetch_real ("tube.z");
          }

        {
          tube_inner_r *= lunit;
          tube_outer_r *= lunit;
          tube_z *= lunit;
        }
        geomtools::tube cyl (tube_inner_r,
                             tube_outer_r,  
                             tube_z);
        set_tube (cyl);
      }

    _init_ ();
    _initialized_ = true;
    return;
  }

  GENVTX_VG_RESET_IMPLEMENT_HEAD(tube_vg)
  {
    if (! is_initialized ())
      {
        throw logic_error ("genvtx::tube_vg::reset: Not initialized !");
      }
    _reset_ ();
    _initialized_ = false;
    return;
  }

  void tube_vg::_init_ ()
  {
    bool devel = false;
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
    _set_defaults_ ();
    return;
  }

  void tube_vg::_set_defaults_ ()
  {
    _mode_ = MODE_DEFAULT;
    _surface_mask_ = geomtools::tube::FACE_ALL;
    _skin_skip_ = 0.0;
    _skin_thickness_ = 0.0;
    for (int i = 0; i < 3; i++)
      {
        _sum_weight_[i] = 0.0;
      } 
    //datatools::invalidate(_r1_sqr_);
    //datatools::invalidate(_r2_sqr_);
    return;
  }

  void tube_vg::tree_dump (std::ostream & out_, 
                               const std::string & title_, 
                               const std::string & indent_, 
                               bool inherit_) const
  {
    namespace du = datatools;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    i_vertex_generator::tree_dump (out_, title_, indent_, true);
    out_ << indent << du::i_tree_dumpable::tag;
    out_ << "Tube : " << _tube_ << endl;
    out_ << indent << du::i_tree_dumpable::tag << "Mode :        " << _mode_ << endl;
    out_ << indent << du::i_tree_dumpable::tag << "Surface mask: " << _surface_mask_ << endl;
    out_ << indent << du::i_tree_dumpable::tag << "Skin skip:    " << _skin_skip_ << endl;
    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_) 
         << "Skin thickness: " << _skin_thickness_ << endl;
    return;
  }
  
  GENVTX_VG_SHOOT_VERTEX_IMPLEMENT_HEAD(tube_vg,random_,vertex_)
  {
    bool devel = false;
    if (! is_initialized ())
      {
        throw logic_error ("genvtx::tube_vg::_shoot_vertex: Not initialized !"); 
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

} // end of namespace genvtx

// end of tube_vg.cc

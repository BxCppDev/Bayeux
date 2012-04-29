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

  GENVTX_VG_REGISTRATION_IMPLEMENT(cylinder_vg,"genvtx::cylinder_vg");

  int cylinder_vg::get_mode () const
  {
    return _mode_;
  }

  void cylinder_vg::set_mode (int mode_)
  {
    if ((mode_ !=  MODE_BULK) && (mode_ !=  MODE_SURFACE))
      {
        throw logic_error ("genvtx::cylinder_vg::set_mode: Invalid mode !");
      }
    _mode_ = mode_;
    return;
  }

  void cylinder_vg::set_surface_mask (int surface_mask_)
  {
    _assert_lock ("genvtx::cylinder_vg::set_surface_mask");
    _surface_mask_ = surface_mask_;
    return;
  }

  void cylinder_vg::set_skin_skip (double skin_skip_)
  {
    _assert_lock ("genvtx::cylinder_vg::set_surface_mask");
    _skin_skip_ = skin_skip_;
    return;
  }

  void cylinder_vg::set_skin_thickness (double skin_thickness_)
  {
    _assert_lock ("genvtx::cylinder_vg::set_skin_thickness");
    _skin_thickness_ = skin_thickness_;
    return;
  }

  void cylinder_vg::set_bulk (double skin_thickness_)
  {
    _assert_lock ("genvtx::cylinder_vg::set_bulk");
    _mode_ = MODE_BULK;
    return;
  }

  void cylinder_vg::set_surface (int surface_mask_)
  {
    _assert_lock ("genvtx::cylinder_vg::set_surface");
    _mode_ = MODE_SURFACE;
    set_surface_mask (surface_mask_);
    return;
  }

  void cylinder_vg::set_cylinder (const geomtools::cylinder & cylinder_)
  {
    _assert_lock ("genvtx::cylinder_vg::set_cylinder");
    _cylinder_ = cylinder_;
    return;
  }

  const geomtools::cylinder & cylinder_vg::get_cylinder () const
  {
    return _cylinder_;
  }
  
  GENVTX_VG_IS_INITIALIZED_IMPLEMENT_HEAD(cylinder_vg)
  {
    return _initialized_;
  }
  
  // Constructor :
  GENVTX_VG_CONSTRUCTOR_IMPLEMENT_HEAD(cylinder_vg)
  {
    _initialized_ = false;
    _cylinder_.reset ();
    _set_defaults_ ();
    return;
  }
  
  // Destructor :
  GENVTX_VG_DEFAULT_DESTRUCTOR_IMPLEMENT(cylinder_vg)

  GENVTX_VG_INITIALIZE_IMPLEMENT_HEAD(cylinder_vg,setup_,service_manager_,vgens_)
  {
    if (is_debug ()) std::cerr << "DEBUG: genvtx::cylinder_vg::initialize: Entering..." << std::endl;
    using namespace std;
    bool devel = false;
    if (is_initialized ())
      {
        throw logic_error ("genvtx::cylinder_vg::initialize: Already initialized !");
      }

    // parameters of the cylinder vertex generator:
    int    mode = MODE_INVALID;
    double skin_skip = 0.0;
    double skin_thickness = 0.0;
    int    surface_mask = geomtools::cylinder::FACE_NONE;
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
                message << "genvtx::cylinder_vg::initialize: Invalid mode '"
                        << mode_str << "' !";
                throw logic_error (message.str ());
              }
            treat_mode = true;
          }
      }

    if (setup_.has_key ("length_unit"))
      {
        lunit_str = setup_.fetch_string ("length_unit");
        lunit = datatools::utils::units::get_length_unit_from (lunit_str);
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
                surface_mask = geomtools::cylinder::FACE_ALL;
                break;
              }
            else if (surfaces[i] == "side")
              {
                surface_mask |= geomtools::cylinder::FACE_SIDE;
              }
            else if (surfaces[i] == "bottom")
              {
                surface_mask |= geomtools::cylinder::FACE_BOTTOM;
              }
            else if (surfaces[i] == "top")
              {
                surface_mask |= geomtools::cylinder::FACE_TOP;
              }
          }
      }
 
    if (treat_mode) set_mode (mode);
    if (treat_skin_skip) set_skin_skip (skin_skip);
    if (treat_skin_thickness) set_skin_thickness (skin_thickness);
    if (mode == MODE_SURFACE && treat_surface_mask) set_surface_mask (surface_mask);

    if (! _cylinder_.is_valid ())
      {
        double cylinder_r, cylinder_z;
        datatools::utils::invalidate (cylinder_r);
        datatools::utils::invalidate (cylinder_z);
        double length_unit = CLHEP::millimeter;
        
        if (setup_.has_key ("length_unit"))
          {
            std::string length_unit_str = setup_.fetch_string ("length_unit");
            length_unit = datatools::utils::units::get_length_unit_from (length_unit_str);
          }

        if (setup_.has_key ("cylinder.r"))
          {
            cylinder_r = setup_.fetch_real ("cylinder.r");
          }

        if (setup_.has_key ("cylinder.z"))
          {
            cylinder_z = setup_.fetch_real ("cylinder.z");
          }
        {
          cylinder_r *= lunit;
          cylinder_z *= lunit;
        }
        geomtools::cylinder cyl (cylinder_r, cylinder_z);
        set_cylinder (cyl);
      }

    _init_ ();
    _initialized_ = true;
    return;
  }

  GENVTX_VG_RESET_IMPLEMENT_HEAD(cylinder_vg)
  {
    if (! is_initialized ())
      {
        throw logic_error ("genvtx::cylinder_vg::reset: Not initialized !");
      }
    _reset_ ();
    _initialized_ = false;
    return;
  }

  void cylinder_vg::_init_ ()
  {
    if (_mode_ == MODE_SURFACE) 
      {
        if (_surface_mask_ == 0)
          {
            throw logic_error ("genvtx::cylinder_vg::_init_: Surface mask is null !");
          }
        double s = _cylinder_.get_surface (_surface_mask_);
        if (is_debug()) clog << "DEBUG: genvtx::cylinder_vg::_init_: Total surface = " << s << endl;
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
            if (is_debug()) clog << "DEBUG: genvtx::cylinder_vg::_init_: Surface weight [" << i << "] = " << _sum_weight_[i] << endl;
          }
      }
    return;
  }

  void cylinder_vg::_reset_ ()
  {
    _set_defaults_ ();
    return;
  }

  void cylinder_vg::_set_defaults_ ()
  {
    _cylinder_.reset ();
    _mode_ = MODE_INVALID;
    _surface_mask_ = geomtools::cylinder::FACE_ALL;
    _skin_skip_ = 0.0;
    _skin_thickness_ = 0.0;
    for (int i = 0; i < 3; i++)
      {
        _sum_weight_[i] = 0.0;
      }
    return;
  }

  void cylinder_vg::tree_dump (std::ostream & out_, 
                               const std::string & title_, 
                               const std::string & indent_, 
                               bool inherit_) const
  {
    namespace du = datatools::utils;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    i_vertex_generator::tree_dump (out_, title_, indent_, true);
    out_ << indent << du::i_tree_dumpable::tag;
    out_ << "Cylinder : " << _cylinder_ << endl;
    out_ << indent << du::i_tree_dumpable::tag << "Mode :        " << _mode_ << endl;
    out_ << indent << du::i_tree_dumpable::tag << "Surface mask: " << _surface_mask_ << endl;
    out_ << indent << du::i_tree_dumpable::tag << "Skin skip:    " << _skin_skip_ << endl;
    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_) 
         << "Skin thickness: " << _skin_thickness_ << endl;
    return;
  }
  
  GENVTX_VG_SHOOT_VERTEX_IMPLEMENT_HEAD(cylinder_vg,random_,vertex_)
  {
    bool devel = false;
    if (! is_initialized ())
      {
        throw logic_error ("genvtx::cylinder_vg::_shoot_vertex: Not initialized !"); 
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

} // end of namespace genvtx

// end of cylinder_vg.cc

// cylinder_vg.cc

// Ourselves:
#include <genvtx/cylinder_vg.h>

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools
#include <datatools/units.h>
#include <datatools/properties.h>
#include <datatools/exception.h>
// - Bayeux/mygsl
#include <mygsl/rng.h>
// - Bayeux/geomtools:
#include <geomtools/logical_volume.h>

// This project:
#include <genvtx/vertex_validation.h>

namespace genvtx {

  GENVTX_VG_REGISTRATION_IMPLEMENT(cylinder_vg,"genvtx::cylinder_vg");

  // static
  const int cylinder_vg::MODE_INVALID;
  const int cylinder_vg::MODE_BULK;
  const int cylinder_vg::MODE_SURFACE;
  const int cylinder_vg::MODE_DEFAULT;

  int cylinder_vg::get_mode () const
  {
    return _mode_;
  }

  void cylinder_vg::set_mode (int mode_)
  {
    DT_THROW_IF ((mode_ !=  MODE_BULK) && (mode_ !=  MODE_SURFACE),
                 std::logic_error, "Invalid mode !");
    _mode_ = mode_;
    return;
  }

  void cylinder_vg::set_surface_mask (int surface_mask_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _surface_mask_ = surface_mask_;
    return;
  }

  void cylinder_vg::set_skin_skip (double skin_skip_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _skin_skip_ = skin_skip_;
    return;
  }

  void cylinder_vg::set_skin_thickness (double skin_thickness_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _skin_thickness_ = skin_thickness_;
    return;
  }

  void cylinder_vg::set_bulk (double /*skin_thickness_*/)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _mode_ = MODE_BULK;
    return;
  }

  void cylinder_vg::set_surface (int surface_mask_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _mode_ = MODE_SURFACE;
    set_surface_mask (surface_mask_);
    return;
  }

  bool cylinder_vg::has_logical() const
  {
    return _log_vol_ != 0;
  }

  void cylinder_vg::set_logical (const geomtools::logical_volume & lv_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    DT_THROW_IF (_cylinder_ref_ != 0, std::logic_error, "Already has a referenced cylinder !");
    DT_THROW_IF (lv_.get_shape().get_shape_name() != "cylinder", std::logic_error, "Logical volume has a wrong shape !");
    _log_vol_ = &lv_;
    _cylinder_ref_ = dynamic_cast<const geomtools::cylinder *>(&_log_vol_->get_shape());
    return;
  }

  void cylinder_vg::reset_logical()
  {
    _log_vol_ = 0;
    _cylinder_ref_ = 0;
    return;
  }

  void cylinder_vg::set_cylinder (const geomtools::cylinder & cylinder_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    DT_THROW_IF (_cylinder_ref_ != 0, std::logic_error, "Already has a referenced cylinder !");
    _cylinder_ = cylinder_;
    return;
  }

  const geomtools::cylinder & cylinder_vg::get_cylinder () const
  {
    return _cylinder_;
  }

  bool cylinder_vg::has_cylinder_ref () const
  {
    return _cylinder_ref_ != 0;
  }

  void cylinder_vg::set_cylinder_ref (const geomtools::cylinder & cylinder_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    DT_THROW_IF (has_logical(), std::logic_error, "Already has a logical volume !");
    DT_THROW_IF (! cylinder_.is_valid (), std::logic_error, "Invalid cylinder !");
    _cylinder_ref_ = &cylinder_;
    return;
  }

  const geomtools::cylinder & cylinder_vg::get_cylinder_ref () const
  {
    DT_THROW_IF (_cylinder_ref_ == 0, std::logic_error, "No cylinder ref !");
    return *_cylinder_ref_;
  }

  bool cylinder_vg::has_cylinder_safe () const
  {
    if (_cylinder_.is_valid ()) return true;
    if (_cylinder_ref_ != 0 && _cylinder_ref_->is_valid ()) return true;
    return false;
  }

  const geomtools::cylinder & cylinder_vg::get_cylinder_safe () const
  {
    if (has_cylinder_ref ()) {
      return get_cylinder_ref ();
    }
    return get_cylinder ();
  }

  bool cylinder_vg::is_initialized () const
  {
    return _initialized_;
  }

  cylinder_vg::cylinder_vg() : genvtx::i_vertex_generator()
  {
    _initialized_ = false;
    _log_vol_ = 0;
    _cylinder_ref_ = 0;
    _cylinder_.reset ();
    _set_defaults_ ();
    return;
  }

  cylinder_vg::~cylinder_vg()
  {
    if (is_initialized ()) reset ();
    return;
  }

  void cylinder_vg::initialize (const ::datatools::properties & setup_,
                                ::datatools::service_manager & service_manager_,
                                ::genvtx::vg_dict_type & /*vgens_*/)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");

    this->::genvtx::i_vertex_generator::_initialize_basics(setup_, service_manager_);

    // parameters of the cylinder vertex generator:
    int    mode = MODE_INVALID;
    double skin_skip = 0.0;
    double skin_thickness = 0.0;
    int    surface_mask = geomtools::cylinder::FACE_NONE;
    double lunit = CLHEP::mm;
    std::string lunit_str;
    bool treat_mode           = false;
    bool treat_surface_mask   = false;
    bool treat_skin_skip      = false;
    bool treat_skin_thickness = false;

    if (_mode_ == MODE_INVALID) {
      if (setup_.has_key ("mode")) {
        const std::string mode_str = setup_.fetch_string ("mode");
        if (mode_str == "surface") {
          mode = MODE_SURFACE;
        } else if (mode_str == "bulk") {
          mode = MODE_BULK;
        } else {
          DT_THROW_IF (true,std::logic_error ,"Invalid mode '" << mode_str << "' !");
        }
        treat_mode = true;
      }
    }

    if (setup_.has_key ("length_unit")) {
      lunit_str = setup_.fetch_string ("length_unit");
      lunit = datatools::units::get_length_unit_from (lunit_str);
    }

    if (setup_.has_key ("skin_skip")) {
      skin_skip = setup_.fetch_real ("skin_skip");
      skin_skip *= lunit;
      treat_skin_skip = true;
    }

    if (setup_.has_key ("skin_thickness")) {
      skin_thickness = setup_.fetch_real ("skin_thickness");
      skin_thickness *= lunit;
      treat_skin_thickness = true;
    }

    if (mode == MODE_SURFACE) {
      std::vector<std::string> surfaces;
      if (setup_.has_key ("surfaces")) {
        setup_.fetch ("surfaces", surfaces);
        treat_surface_mask = true;
      }

      for (size_t i = 0; i < surfaces.size (); i++) {
        if (surfaces[i] == "all") {
          surface_mask = geomtools::cylinder::FACE_ALL;
          break;
        } else if (surfaces[i] == "side") {
          surface_mask |= geomtools::cylinder::FACE_SIDE;
        } else if (surfaces[i] == "bottom") {
          surface_mask |= geomtools::cylinder::FACE_BOTTOM;
        } else if (surfaces[i] == "top") {
          surface_mask |= geomtools::cylinder::FACE_TOP;
        }
      }
    }

    if (treat_mode) set_mode (mode);
    if (treat_skin_skip) set_skin_skip (skin_skip);
    if (treat_skin_thickness) set_skin_thickness (skin_thickness);
    if (mode == MODE_SURFACE && treat_surface_mask) set_surface_mask (surface_mask);

    if (! _cylinder_.is_valid ()) {
      double cylinder_r, cylinder_z;
      datatools::invalidate (cylinder_r);
      datatools::invalidate (cylinder_z);
      // double length_unit = CLHEP::millimeter;

      // if (setup_.has_key("length_unit")) {
      //   std::string length_unit_str = setup_.fetch_string("length_unit");
      //   length_unit = datatools::units::get_length_unit_from(length_unit_str);
      // }

      if (setup_.has_key("cylinder.r")) {
        cylinder_r = setup_.fetch_real("cylinder.r");
        if (!setup_.has_explicit_unit("cylinder.r")) {
          cylinder_r *= lunit;
        }
      }

      if (setup_.has_key("cylinder.z")) {
        cylinder_z = setup_.fetch_real("cylinder.z");
        if (!setup_.has_explicit_unit("cylinder.z")) {
          cylinder_z *= lunit;
        }
      }
      geomtools::cylinder cyl(cylinder_r, cylinder_z);
      set_cylinder(cyl);
    }

    _init_();
    _initialized_ = true;
    return;
  }

  void cylinder_vg::reset()
  {
    DT_THROW_IF (! is_initialized(), std::logic_error, "Not initialized !");
    _reset_();
    _initialized_ = false;
    return;
  }

  void cylinder_vg::_init_()
  {
    if (_mode_ == MODE_SURFACE) {
      DT_THROW_IF(_surface_mask_ == 0, std::logic_error, "Surface mask is zero !");
      const double s = get_cylinder_safe().get_surface(_surface_mask_);
      DT_LOG_DEBUG(get_logging_priority(), "Total surface = " << s);
      _sum_weight_[0] = get_cylinder_safe().get_surface(_surface_mask_ & geomtools::cylinder::FACE_SIDE);
      _sum_weight_[1] = get_cylinder_safe().get_surface(_surface_mask_ & geomtools::cylinder::FACE_BOTTOM);
      _sum_weight_[2] = get_cylinder_safe().get_surface(_surface_mask_ & geomtools::cylinder::FACE_TOP);
      for (size_t i = 0; i < 3; i++) {
        _sum_weight_[i] /= s;
        if (i > 0) {
          _sum_weight_[i] += _sum_weight_[i - 1];
        }
        DT_LOG_TRACE(get_logging_priority(), "Surface weight [" << i << "] = " << _sum_weight_[i]);
      }
    }
    return;
  }

  void cylinder_vg::_reset_()
  {
    if (has_logical()) {
      reset_logical();
    }
    _cylinder_.reset();
    this->i_vertex_generator::_reset();
    _set_defaults_();
    return;
  }

  void cylinder_vg::_set_defaults_()
  {
    _cylinder_ref_ = 0;
    _log_vol_ = 0;
    _mode_ = MODE_INVALID;
    _surface_mask_ = geomtools::cylinder::FACE_ALL;
    _skin_skip_ = 0.0;
    _skin_thickness_ = 0.0;
    for (size_t i = 0; i < 3; i++) {
      _sum_weight_[i] = 0.0;
    }
    return;
  }

  void cylinder_vg::tree_dump (std::ostream & out_,
                               const std::string & title_,
                               const std::string & indent_,
                               bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty()) indent = indent_;
    i_vertex_generator::tree_dump(out_, title_, indent_, true);
    out_ << indent << datatools::i_tree_dumpable::tag;
    if (has_logical()) {
      out_ << "Logical volume : '" << _log_vol_->get_name() << "'" << std::endl;
    }
    out_ << indent << datatools::i_tree_dumpable::tag;
    if (has_cylinder_ref()) {
      out_ << "External cylinder : " << *_cylinder_ref_ << " [" << _cylinder_ref_ << ']';
    } else {
      out_ << "Embedded cylinder : " << _cylinder_;
    }
    out_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Mode :        " << _mode_ << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Surface mask: " << _surface_mask_ << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Skin skip:    " << _skin_skip_ << std::endl;
    out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Skin thickness: " << _skin_thickness_ << std::endl;
    return;
  }

  void cylinder_vg::_shoot_vertex(::mygsl::rng & random_,
                                  ::geomtools::vector_3d & vertex_)
  {
    DT_THROW_IF(! is_initialized(), std::logic_error, "Not initialized !");
    geomtools::invalidate(vertex_);
    double r = 0.0;
    double t_max = 2. * M_PI;
    double t = random_.uniform() * t_max;
    double x = 0.0, y = 0.0, z = 0.0;
    const geomtools::cylinder * the_cylinder = &_cylinder_;
    if (has_cylinder_ref()) {
      the_cylinder = _cylinder_ref_;
    }
    if (_mode_ == MODE_BULK) {
      double r_max = the_cylinder->get_r () - 0.5 * _skin_thickness_;
      r = std::sqrt (random_.uniform ()) * r_max;
      z = (random_.uniform () - 0.5) * (the_cylinder->get_z () - _skin_thickness_);
    }

    if (_mode_ == MODE_SURFACE) {
      double r0 = random_.uniform ();

      double delta_thick = 0.0;
      if (_skin_thickness_ > 0.0) {
        double r3 = random_.uniform ();
        delta_thick = (r3 - 0.5) * _skin_thickness_;
      }

      if (r0 < _sum_weight_[0]) {
        double r_min  = the_cylinder->get_r ()
          + _skin_skip_
          - 0.5 * _skin_thickness_;
        double r_max  = r_min + _skin_thickness_;
        double r_min2 = r_min * r_min;
        double r_max2 = r_max * r_max;
        r = (std::sqrt (r_min2 + random_.uniform () * (r_max2 - r_min2)));
        z = +(random_.uniform () - 0.5) * (the_cylinder->get_z ());
      } else if (r0 < _sum_weight_[1]) {
        double r_max = the_cylinder->get_r ();
        r = std::sqrt (random_.uniform ()) * r_max;
        z = -(the_cylinder->get_half_z () + _skin_skip_ + delta_thick);
      } else if (r0 < _sum_weight_[2]) {
        double r_max = the_cylinder->get_r ();
        r = std::sqrt (random_.uniform ()) * r_max;
        z = +(the_cylinder->get_half_z () + _skin_skip_ + delta_thick);
      }
    }
    x = r * std::cos(t);
    y = r * std::sin(t);
    vertex_.set(x,y, z);

    if (has_vertex_validation()) {
      // Setup the geometry context for the vertex validation system:
      DT_THROW_IF(!has_logical(), std::logic_error, "Missing logical volume in '" << get_name() << "' vertex generator!");
      _grab_vertex_validation().grab_geometry_context().set_local_candidate_vertex(vertex_);
      _grab_vertex_validation().grab_geometry_context().set_logical_volume(*_log_vol_);
    }
    return;
  }

} // end of namespace genvtx

// tube_vg.cc

// Ourselves:
#include <genvtx/tube_vg.h>

// Standard library:
#include <stdexcept>
#include <sstream>


// Third party:
// - Bayeux/datatools
#include <datatools/units.h>
#include <datatools/utils.h>
#include <datatools/properties.h>
// - Bayeux/mygsl
#include <mygsl/rng.h>
// - Bayeux/geomtools:
#include <geomtools/logical_volume.h>

// This project:
#include <genvtx/vertex_validation.h>

namespace genvtx {

  GENVTX_VG_REGISTRATION_IMPLEMENT(tube_vg,"genvtx::tube_vg")

  // static
  const int tube_vg::MODE_INVALID;
  const int tube_vg::MODE_BULK;
  const int tube_vg::MODE_SURFACE;
  const int tube_vg::MODE_DEFAULT;

  int tube_vg::get_mode () const
  {
    return _mode_;
  }

  void tube_vg::set_mode (int mode_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    DT_THROW_IF ((mode_ !=  MODE_BULK) && (mode_ !=  MODE_SURFACE),
                 std::logic_error, "Invalid mode !");
    _mode_ = mode_;
    return;
  }

  void tube_vg::set_surface_mask (int surface_mask_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _surface_mask_ = surface_mask_;
    return;
  }

  void tube_vg::set_skin_skip (double skin_skip_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _skin_skip_ = skin_skip_;
    return;
  }

  void tube_vg::set_skin_thickness (double skin_thickness_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _skin_thickness_ = skin_thickness_;
    return;
  }

  void tube_vg::set_bulk (double /*skin_thickness_*/)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _mode_ = MODE_BULK;
    return;
  }

  void tube_vg::set_surface (int surface_mask_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _mode_ = MODE_SURFACE;
    set_surface_mask (surface_mask_);
    return;
  }

  void tube_vg::set_tube (const geomtools::tube & tube_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _tube_ = tube_;
    return;
  }

  const geomtools::tube & tube_vg::get_tube () const
  {
    return _tube_;
  }

  bool tube_vg::has_logical() const
  {
    return _log_vol_ != 0;
  }

  void tube_vg::set_logical (const geomtools::logical_volume & lv_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    DT_THROW_IF (_tube_ref_ != 0, std::logic_error, "Already has a referenced tube !");
    DT_THROW_IF (lv_.get_shape().get_shape_name() != "tube", std::logic_error, "Logical volume has a wrong shape !");
    _log_vol_ = &lv_;
    _tube_ref_ = dynamic_cast<const geomtools::tube *>(&_log_vol_->get_shape());
    return;
  }

  void tube_vg::reset_logical()
  {
    _log_vol_ = 0;
    _tube_ref_ = 0;
    return;
  }

  bool tube_vg::has_tube_ref () const
  {
    return _tube_ref_ != 0;
  }

  void tube_vg::set_tube_ref (const geomtools::tube & tube_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    DT_THROW_IF (has_logical(), std::logic_error, "Already has a logical volume !");
    DT_THROW_IF (! tube_.is_valid (), std::logic_error, "Invalid tube !");
    _tube_ref_ = &tube_;
    return;
  }

  const geomtools::tube & tube_vg::get_tube_ref () const
  {
    DT_THROW_IF (_tube_ref_ == 0, std::logic_error, "No tube ref !");
    return *_tube_ref_;
  }

  bool tube_vg::has_tube_safe () const
  {
    if (_tube_.is_valid ()) return true;
    if (_tube_ref_ != 0 && _tube_ref_->is_valid ()) return true;
    return false;
  }

  const geomtools::tube & tube_vg::get_tube_safe () const
  {
    if (has_tube_ref ()) {
      return get_tube_ref ();
    }
    return get_tube ();
  }

  bool tube_vg::is_initialized () const
  {
    return _initialized_;
  }

  tube_vg::tube_vg() : genvtx::i_vertex_generator()
  {
    _initialized_ = false;
    _log_vol_ = 0;
    _tube_ref_ = 0;
    _tube_.reset ();
    _set_defaults_ ();
    return;
  }

  tube_vg::~tube_vg()
  {
    if (is_initialized ()) reset ();
    return;
  }


  void tube_vg::initialize (const ::datatools::properties & setup_,
                            ::datatools::service_manager & /*service_manager_*/,
                            ::genvtx::vg_dict_type & /*vgens_*/)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");

    // parameters of the tube vertex generator:
    int    mode = MODE_INVALID;
    double skin_skip = 0.0;
    double skin_thickness = 0.0;
    int    surface_mask = geomtools::tube::FACE_NONE;
    double lunit = CLHEP::mm;
    std::string lunit_str;
    bool treat_mode           = false;
    bool treat_surface_mask   = false;
    bool treat_skin_skip      = false;
    bool treat_skin_thickness = false;

    if (setup_.has_key ("length_unit")) {
      lunit_str = setup_.fetch_string ("length_unit");
      lunit = datatools::units::get_length_unit_from (lunit_str);
    }

    if (_mode_ == MODE_INVALID) {
      if (setup_.has_key ("mode")) {
        const std::string mode_str = setup_.fetch_string ("mode");
        if (mode_str == "surface") {
          mode = MODE_SURFACE;
        } else if (mode_str == "bulk") {
          mode = MODE_BULK;
        } else {
          DT_THROW_IF(true,std::logic_error, "Invalid mode '" << mode_str << "' !");
        }
        treat_mode = true;
      }
    }

    if (setup_.has_key ("skin_skip")) {
      skin_skip = setup_.fetch_real ("skin_skip");
      if (! setup_.has_explicit_unit("skin_skip")) skin_skip *= lunit;
      treat_skin_skip = true;
    }

    if (setup_.has_key ("skin_thickness")) {
      skin_thickness = setup_.fetch_real ("skin_thickness");
      if (! setup_.has_explicit_unit("skin_thickness")) skin_thickness *= lunit;
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
          surface_mask = geomtools::tube::FACE_ALL;
          break;
        } else if (surfaces[i] == "outer_side") {
          surface_mask |= geomtools::tube::FACE_OUTER_SIDE;
        } else if (surfaces[i] == "inner_side") {
          surface_mask |= geomtools::tube::FACE_INNER_SIDE;
        } else if (surfaces[i] == "bottom") {
          surface_mask |= geomtools::tube::FACE_BOTTOM;
        } else if (surfaces[i] == "top") {
          surface_mask |= geomtools::tube::FACE_TOP;
        }
      }
    }

    if (treat_mode) set_mode (mode);
    if (treat_skin_skip) set_skin_skip (skin_skip);
    if (treat_skin_thickness) set_skin_thickness (skin_thickness);
    if (mode == MODE_SURFACE && treat_surface_mask) {
      set_surface_mask (surface_mask);
    }

    if (! _tube_.is_valid ()) {
      double tube_inner_r, tube_outer_r, tube_z;
      datatools::invalidate (tube_inner_r);
      datatools::invalidate (tube_outer_r);
      datatools::invalidate (tube_z);

      if (setup_.has_key ("tube.inner_r")) {
        tube_inner_r = setup_.fetch_real ("tube.inner_r");
        if (! setup_.has_explicit_unit("tube.inner_r")) tube_inner_r *= lunit;
      }

      if (setup_.has_key ("tube.outer_r")) {
        tube_outer_r = setup_.fetch_real ("tube.outer_r");
        if (! setup_.has_explicit_unit("tube.outer_r")) tube_outer_r *= lunit;
      }

      if (setup_.has_key ("tube.z")) {
        tube_z = setup_.fetch_real ("tube.z");
        if (! setup_.has_explicit_unit("tube.z")) tube_z *= lunit;
      }

      geomtools::tube cyl(tube_inner_r,
                          tube_outer_r,
                          tube_z);
      set_tube(cyl);
    }

    _init_ ();
    _initialized_ = true;
    return;
  }

  void tube_vg::reset ()
  {
    DT_THROW_IF (! is_initialized(), std::logic_error, "Not initialized !");
    _reset_ ();
    _initialized_ = false;
    return;
  }

  void tube_vg::_init_ ()
  {
    if (_mode_ == MODE_SURFACE) {
      DT_THROW_IF (_surface_mask_ == 0, std::logic_error, "Surface mask is null !");
      const double s = _tube_.get_surface (_surface_mask_);
      DT_LOG_DEBUG (get_logging_priority (), "Total surface = " << s);
      _sum_weight_[0] = _tube_.get_surface (_surface_mask_ & geomtools::tube::FACE_OUTER_SIDE);
      _sum_weight_[1] = _tube_.get_surface (_surface_mask_ & geomtools::tube::FACE_BOTTOM);
      _sum_weight_[2] = _tube_.get_surface (_surface_mask_ & geomtools::tube::FACE_TOP);
      _sum_weight_[3] = _tube_.get_surface (_surface_mask_ & geomtools::tube::FACE_INNER_SIDE);
      for (size_t i = 0; i < 4; i++) {
        _sum_weight_[i] /= s;
        if (i > 0) {
          _sum_weight_[i] += _sum_weight_[i - 1];
        }
        DT_LOG_TRACE (get_logging_priority (), "Surface weight [" << i << "] = " << _sum_weight_[i]);
      }
    }
    /*
      _r1_sqr_ = std::pow (_tube_.get_inner_r () + 0.5 * _skin_thickness_, 2);
      _r2_sqr_ = std::pow (_tube_.get_outer_r () - 0.5 * _skin_thickness_, 2);
      if (_r1_sqr_ > _r2_sqr_)
      {
      th row logic_error ("genvtx::tube_vg::_init_: Thickness is too large !");
      }
    */
    return;
  }

  void tube_vg::_reset_ ()
  {
    if (has_logical()) {
      reset_logical();
    }
    _tube_.reset();
    this->i_vertex_generator::_reset();
    _set_defaults_ ();
    return;
  }

  void tube_vg::_set_defaults_ ()
  {
    _mode_ = MODE_DEFAULT;
    _log_vol_ = 0;
    _tube_ref_ = 0;
    _surface_mask_ = geomtools::tube::FACE_ALL;
    _skin_skip_ = 0.0;
    _skin_thickness_ = 0.0;
    for (size_t i = 0; i < 4; i++) {
      _sum_weight_[i] = 0.0;
    }
    return;
  }

  void tube_vg::tree_dump (std::ostream & out_,
                           const std::string & title_,
                           const std::string & indent_,
                           bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    i_vertex_generator::tree_dump (out_, title_, indent_, true);
    out_ << indent << datatools::i_tree_dumpable::tag;
    if (has_logical()) {
      out_ << "Logical volume : '" << _log_vol_->get_name() << "'" << std::endl;
    }
    out_ << indent << datatools::i_tree_dumpable::tag;
    if (has_tube_ref()) {
      out_ << "External tube : " << *_tube_ref_ << " [" << _tube_ref_ << ']';
    } else {
      out_ << "Embedded tube : " << _tube_;
    }
    out_ << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Mode :        " << _mode_ << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Surface mask: " << _surface_mask_ << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Skin skip:    " << _skin_skip_ << std::endl;
    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Skin thickness: " << _skin_thickness_ << std::endl;
    return;
  }

  void tube_vg::_shoot_vertex(::mygsl::rng & random_,
                              ::geomtools::vector_3d & vertex_)
  {
    DT_THROW_IF (! is_initialized (), std::logic_error, "Not initialized !");
    geomtools::invalidate (vertex_);
    const double t_max = 2. * M_PI;
    const double t = random_.uniform () * t_max;
    double r = 0.0;
    double x = 0.0, y = 0.0, z = 0.0;
    if (_mode_ == MODE_BULK) {
      const double r_min = _tube_.get_inner_r () + 0.5 * _skin_thickness_;
      const double r_max = _tube_.get_outer_r () - 0.5 * _skin_thickness_;
      const double r_min2 = r_min * r_min;
      const double r_max2 = r_max * r_max;
      r = std::sqrt (r_min2 + random_.uniform () * (r_max2 - r_min2));
      z = (random_.uniform () - 0.5) * (_tube_.get_z () - _skin_thickness_);
    }

    if (_mode_ == MODE_SURFACE) {
      const double r0 = random_.uniform ();

      double delta_thick = 0.0;
      if (_skin_thickness_ > 0.0) {
        const double r3 = random_.uniform ();
        delta_thick = (r3 - 0.5) * _skin_thickness_;
      }

      // outer surface :
      if (r0 < _sum_weight_[0]) {
        const double r_min  = _tube_.get_outer_r ()
          + _skin_skip_
          - 0.5 * _skin_thickness_;
        const double r_max  = r_min + _skin_thickness_;
        const double r_min2 = r_min * r_min;
        const double r_max2 = r_max * r_max;
        r = std::sqrt (r_min2 + random_.uniform () * (r_max2 - r_min2));
        z = +(random_.uniform () - 0.5) * (_tube_.get_z ());
      }
      // bottom surface :
      else if (r0 < _sum_weight_[1]) {
        const double r_max = _tube_.get_outer_r ();
        const double r_min = _tube_.get_inner_r ();
        const double r_max2 = r_max * r_max;
        const double r_min2 = r_min * r_min;
        r = std::sqrt (r_min2 + random_.uniform () * (r_max2 - r_min2));
        z = -(_tube_.get_half_z () + _skin_skip_ + delta_thick);
      }
      // top_surface :
      else if (r0 < _sum_weight_[2]) {
        const double r_max = _tube_.get_outer_r ();
        const double r_min = _tube_.get_inner_r ();
        const double r_max2 = r_max * r_max;
        const double r_min2 = r_min * r_min;
        r = std::sqrt (r_min2 + random_.uniform () * (r_max2 - r_min2));
        z = +(_tube_.get_half_z () + _skin_skip_ + delta_thick);
      }
      // inner surface :
      else {
        const double r_min  = _tube_.get_inner_r ()
          - _skin_skip_
          - 0.5 * _skin_thickness_;
        const double r_max  = r_min + _skin_thickness_;
        const double r_min2 = r_min * r_min;
        const double r_max2 = r_max * r_max;
        r = (sqrt (r_min2 + random_.uniform () * (r_max2 - r_min2)));
        z = +(random_.uniform () - 0.5) * (_tube_.get_z ());
      }
    }
    x = r * cos (t);
    y = r * sin (t);
    vertex_.set (x,y, z);

    if (has_vertex_validation()) {
      // Setup the geometry context for the vertex validation system:
      DT_THROW_IF(!has_logical(), std::logic_error, "Missing logical volume in '"
                  << get_name() << "' vertex generator!");
      _grab_vertex_validation().grab_geometry_context().set_local_candidate_vertex(vertex_);
      _grab_vertex_validation().grab_geometry_context().set_logical_volume(*_log_vol_);
    }
    return;
  }

} // end of namespace genvtx

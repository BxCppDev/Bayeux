// box_vg.cc

// Ourselves:
#include <genvtx/box_vg.h>

// Standard library:
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

// Third party:
// - Bayeux/datatools:
#include <datatools/units.h>
#include <datatools/exception.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>
// - Bayeux/geomtools:
#include <geomtools/logical_volume.h>

// This project:
#include <genvtx/vertex_validation.h>

namespace genvtx {

  GENVTX_VG_REGISTRATION_IMPLEMENT(box_vg,"genvtx::box_vg")

  // static
  const int box_vg::MODE_INVALID;
  const int box_vg::MODE_BULK;
  const int box_vg::MODE_SURFACE;
  const int box_vg::MODE_DEFAULT;

  int box_vg::get_mode () const
  {
    return _mode_;
  }

  void box_vg::set_mode (int mode_)
  {
    DT_THROW_IF ((mode_ !=  MODE_BULK) && (mode_ !=  MODE_SURFACE),
                 std::logic_error, "Invalid mode !");
    _mode_ = mode_;
    return;
  }

  void box_vg::set_surface_mask (int surface_mask_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _surface_mask_ = surface_mask_;
    return;
  }

  void box_vg::set_skin_skip (double skin_skip_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _skin_skip_ = skin_skip_;
    return;
  }

  void box_vg::set_skin_thickness (double skin_thickness_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _skin_thickness_ = skin_thickness_;
    return;
  }

  void box_vg::set_bulk (double /*skin_thickness_*/)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _mode_ = MODE_BULK;
    return;
  }

  void box_vg::set_surface (int surface_mask_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _mode_ = MODE_SURFACE;
    set_surface_mask (surface_mask_);
    return;
  }

  bool box_vg::has_logical() const
  {
    return _log_vol_ != 0;
  }

  void box_vg::set_logical (const geomtools::logical_volume & lv_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    DT_THROW_IF (_box_ref_ != 0, std::logic_error, "Already has a referenced box !");
    DT_THROW_IF (lv_.get_shape().get_shape_name() != "box", std::logic_error, "Logical volume has a wrong shape !");
    _log_vol_ = &lv_;
    _box_ref_ = dynamic_cast<const geomtools::box *>(&_log_vol_->get_shape());
    return;
  }

  void box_vg::reset_logical()
  {
    _log_vol_ = 0;
    _box_ref_ = 0;
    return;
  }

  bool box_vg::has_box_ref () const
  {
    return _box_ref_ != 0;
  }

  void box_vg::set_box_ref (const geomtools::box & box_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    DT_THROW_IF (has_logical(), std::logic_error, "Already has a logical volume !");
    DT_THROW_IF (! box_.is_valid (), std::logic_error, "Invalid box !");
    _box_ref_ = &box_;
    return;
  }

  const geomtools::box & box_vg::get_box_ref () const
  {
    DT_THROW_IF (_box_ref_ == 0, std::logic_error, "No box ref !");
    return *_box_ref_;
  }

  void box_vg::set_box (const geomtools::box & box_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    DT_THROW_IF (_box_ref_ != 0, std::logic_error, "Already has a referenced box !");
    _box_ = box_;
    return;
  }

  const geomtools::box & box_vg::get_box () const
  {
    return _box_;
  }

  bool box_vg::has_box_safe () const
  {
    if (_box_.is_valid ()) return true;
    if (_box_ref_ != 0 && _box_ref_->is_valid ()) return true;
    return false;
  }

  const geomtools::box & box_vg::get_box_safe () const
  {
    if (has_box_ref ()) {
      return get_box_ref ();
    }
    return get_box ();
  }

  bool box_vg::is_initialized () const
  {
    return _initialized_;
  }

  box_vg::box_vg() : genvtx::i_vertex_generator()
  {
    _initialized_ = false;
    _log_vol_ = 0;
    _box_ref_ = 0;
    _box_.reset();
    _set_defaults_();
    return;
  }

  box_vg::~box_vg()
  {
    if (is_initialized()) reset();
    return;
  }

  void box_vg::initialize (const ::datatools::properties & setup_,
                           ::datatools::service_manager & service_manager_,
                           ::genvtx::vg_dict_type & /*vgens_*/)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");

    // Initialize from parent class:
    this->i_vertex_generator::_initialize_basics(setup_, service_manager_);
    this->i_vertex_generator::_initialize_vertex_validation(setup_, service_manager_);

    // Parameters of the box vertex generator:
    int    mode = MODE_INVALID;
    int    surface_mask = geomtools::box::FACE_NONE;
    double skin_skip = 0.0;
    double skin_thickness = 0.0;
    double lunit = CLHEP::mm;
    std::string lunit_str;
    bool treat_mode           = false;
    bool treat_surface_mask   = false;
    bool treat_skin_skip      = false;
    bool treat_skin_thickness = false;

    if (_mode_ == MODE_INVALID) {
      if (setup_.has_key ("mode")) {
        std::string mode_str = setup_.fetch_string ("mode");
        if (mode_str == "surface") {
          mode = MODE_SURFACE;
        } else if (mode_str == "bulk") {
          mode = MODE_BULK;
        } else {
          DT_THROW_IF(true, std::logic_error, "Invalid mode '" << mode_str << "' !");
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
      if (! setup_.has_explicit_unit ("skin_skip")) skin_skip *= lunit;
      treat_skin_skip = true;
    }

    if (setup_.has_key ("skin_thickness")) {
      skin_thickness = setup_.fetch_real ("skin_thickness");
      if (! setup_.has_explicit_unit ("skin_thickness")) skin_thickness *= lunit;
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
          surface_mask = geomtools::box::FACE_ALL;
          break;
        } else if (surfaces[i] == "back") {
          surface_mask |= geomtools::box::FACE_BACK;
        } else if (surfaces[i] == "front") {
          surface_mask |= geomtools::box::FACE_FRONT;
        } else if (surfaces[i] == "left") {
          surface_mask |= geomtools::box::FACE_LEFT;
        } else if (surfaces[i] == "right") {
          surface_mask |= geomtools::box::FACE_RIGHT;
        } else if (surfaces[i] == "bottom") {
          surface_mask |= geomtools::box::FACE_BOTTOM;
        } else if (surfaces[i] == "top") {
          surface_mask |= geomtools::box::FACE_TOP;
        }
      }

    }

    if (treat_mode) set_mode (mode);
    if (treat_skin_skip) set_skin_skip (skin_skip);
    if (treat_skin_thickness) set_skin_thickness (skin_thickness);
    if (mode == MODE_SURFACE && treat_surface_mask) set_surface_mask (surface_mask);

    if (! has_box_safe ()) {
      double box_x, box_y, box_z;
      datatools::invalidate (box_x);
      datatools::invalidate (box_y);
      datatools::invalidate (box_z);
      std::string box_user_ref = vg_tools::SHAPE_REF_NONE;
      std::string box_user_ref_name;

      if (setup_.has_key ("box.x")) {
        box_x = setup_.fetch_real ("box.x");
        if (! setup_.has_explicit_unit ("box.x")) box_x *= lunit;
      }

      if (setup_.has_key ("box.y")) {
        box_y = setup_.fetch_real ("box.y");
        if (! setup_.has_explicit_unit ("box.y")) box_y *= lunit;
      }

      if (setup_.has_key ("box.z")) {
        box_z = setup_.fetch_real ("box.z");
        if (! setup_.has_explicit_unit ("box.z")) box_z *= lunit;
      }

      geomtools::box box (box_x, box_y, box_z);
      set_box (box);
    }

    _init_ ();
    _initialized_ = true;
    return;
  }

  void box_vg::reset ()
  {
    DT_THROW_IF (! is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is not initialized !");
    _reset_ ();
    _initialized_ = false;
    return;
  }

  void box_vg::_init_ ()
  {
    if (_mode_ == MODE_SURFACE) {
      DT_THROW_IF (_surface_mask_ == 0, std::logic_error, "Surface mask is zero !");
      // std::cerr << "DEVEL *** Surface mask = " << _surface_mask_ << std::endl;
      const double s = get_box_safe().get_surface(_surface_mask_);
      DT_LOG_DEBUG (get_logging_priority(), "Total surface = " << s);
      /*
      std::cerr << "DEVEL *** Total surface s = " << s << std::endl;
      std::cerr << "DEVEL *** Surface back   = " <<  get_box_safe().get_parameter("surface.back") << std::endl;
      std::cerr << "DEVEL *** Surface front  = " <<  get_box_safe().get_parameter("surface.front") << std::endl;
      std::cerr << "DEVEL *** Surface left   = " <<  get_box_safe().get_parameter("surface.left") << std::endl;
      std::cerr << "DEVEL *** Surface right  = " <<  get_box_safe().get_parameter("surface.right") << std::endl;
      std::cerr << "DEVEL *** Surface bottom = " <<  get_box_safe().get_parameter("surface.bottom") << std::endl;
      std::cerr << "DEVEL *** Surface top    = " <<  get_box_safe().get_parameter("surface.top") << std::endl;

      std::cerr << "DEVEL *** Surface mask&back   = " << (int) (_surface_mask_ & geomtools::box::FACE_BACK) << std::endl;
      std::cerr << "DEVEL *** Surface mask&front  = " << (int) (_surface_mask_ & geomtools::box::FACE_FRONT) << std::endl;
      std::cerr << "DEVEL *** Surface mask&left  = " << (int) (_surface_mask_ & geomtools::box::FACE_LEFT) << std::endl;
      std::cerr << "DEVEL *** Surface mask&right  = " << (int) (_surface_mask_ & geomtools::box::FACE_RIGHT) << std::endl;
      std::cerr << "DEVEL *** Surface mask&bottom  = " << (int) (_surface_mask_ & geomtools::box::FACE_BOTTOM) << std::endl;
      std::cerr << "DEVEL *** Surface mask&top  = " << (int) (_surface_mask_ & geomtools::box::FACE_TOP) << std::endl;
      */
      _sum_weight_[0] = get_box_safe().get_surface(_surface_mask_ & geomtools::box::FACE_BACK);
      _sum_weight_[1] = get_box_safe().get_surface(_surface_mask_ & geomtools::box::FACE_FRONT);
      _sum_weight_[2] = get_box_safe().get_surface(_surface_mask_ & geomtools::box::FACE_LEFT);
      _sum_weight_[3] = get_box_safe().get_surface(_surface_mask_ & geomtools::box::FACE_RIGHT);
      _sum_weight_[4] = get_box_safe().get_surface(_surface_mask_ & geomtools::box::FACE_BOTTOM);
      _sum_weight_[5] = get_box_safe().get_surface(_surface_mask_ & geomtools::box::FACE_TOP);
      for (int i = 0; i < 6; i++) {
        // std::cerr << "DEVEL *** Surface absolute weight [" << i << "] = " << _sum_weight_[i] << std::endl;
        _sum_weight_[i] /= s;
        if (i > 0) {
          _sum_weight_[i] += _sum_weight_[i - 1];
        }
        DT_LOG_TRACE (get_logging_priority (), "Surface weight [" << i << "] = " << _sum_weight_[i]);
        //std::cerr << "DEVEL *** Surface cumulated normalized weight [" << i << "] = " << _sum_weight_[i] << std::endl;
      }
    }
    return;
  }

  void box_vg::_reset_ ()
  {
    if (has_logical()) {
      reset_logical();
    }
    if (_box_.is_valid ()) {
      _box_.reset ();
    }
    this->i_vertex_generator::_reset();
    _set_defaults_();
    return;
  }

  void box_vg::_set_defaults_ ()
  {
    _box_ref_ = 0;
    _log_vol_ = 0;
    _mode_ = MODE_INVALID;
    _surface_mask_ = geomtools::box::FACE_ALL;
    _skin_skip_ = 0.0;
    _skin_thickness_ = 0.0;
    for (size_t i = 0; i < 6; i++) {
      _sum_weight_[i] = 0.0;
    }
    return;
  }

  void box_vg::tree_dump (std::ostream & out_,
                          const std::string & title_,
                          const std::string & indent_,
                          bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    i_vertex_generator::tree_dump (out_, title_, indent_, true);
    if (has_logical()) {
      out_ << indent << datatools::i_tree_dumpable::tag;
      out_ << "Logical volume : '" << _log_vol_->get_name() << "'" << std::endl;
    }
    out_ << indent << datatools::i_tree_dumpable::tag;
    if (has_box_ref ()) {
      out_ << "External box : " << *_box_ref_ << " [" << _box_ref_ << ']';
    } else {
      out_ << "Embedded box : " << _box_;
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

  void box_vg::_shoot_vertex(::mygsl::rng & random_,
                             ::geomtools::vector_3d & vertex_)
  {
    DT_THROW_IF (! is_initialized (), std::logic_error, "Not initialized !");
    geomtools::invalidate (vertex_);
    double x = 0.0, y = 0.0, z = 0.0;
    const geomtools::box * the_box = &_box_;
    if (has_box_ref ()) {
      the_box = _box_ref_;
    }

    if (_mode_ == MODE_BULK) {
      x = (random_.uniform () - 0.5) * (the_box->get_x () - _skin_thickness_);
      y = (random_.uniform () - 0.5) * (the_box->get_y () - _skin_thickness_);
      z = (random_.uniform () - 0.5) * (the_box->get_z () - _skin_thickness_);
    }

    if (_mode_ == MODE_SURFACE) {
      const double r0 = random_.uniform ();
      const double r1 = random_.uniform ();
      const double r2 = random_.uniform ();
      double delta_thick = 0.0;
      if (_skin_thickness_ > 0.0) {
        double r3 = random_.uniform ();
        delta_thick = (r3 - 0.5) * _skin_thickness_;
      }

      if (r0 < _sum_weight_[0]) {
        y = (r1 - 0.5) * the_box->get_y ();
        z = (r2 - 0.5) * the_box->get_z ();
        x = -(the_box->get_half_x () + _skin_skip_ + delta_thick);
      } else if (r0 < _sum_weight_[1]) {
        y = (r1 - 0.5) * the_box->get_y ();
        z = (r2 - 0.5) * the_box->get_z ();
        x = +(the_box->get_half_x () + _skin_skip_ + delta_thick);
      } else if (r0 < _sum_weight_[2]) {
        x = (r1 - 0.5) * the_box->get_x ();
        z = (r2 - 0.5) * the_box->get_z ();
        y = -(the_box->get_half_y () + _skin_skip_ + delta_thick);
      } else if (r0 < _sum_weight_[3]) {
        x = (r1 - 0.5) * the_box->get_x ();
        z = (r2 - 0.5) * the_box->get_z ();
        y = +(the_box->get_half_y () + _skin_skip_ + delta_thick);
      } else if (r0 < _sum_weight_[4]) {
        x = (r1 - 0.5) * the_box->get_x ();
        y = (r2 - 0.5) * the_box->get_y ();
        z = -(the_box->get_half_z () + _skin_skip_ + delta_thick);
      } else {
        x = (r1 - 0.5) * the_box->get_x ();
        y = (r2 - 0.5) * the_box->get_y ();
        z = +(the_box->get_half_z () + _skin_skip_ + delta_thick);
      }
    }
    vertex_.set (x, y, z);

    if (has_vertex_validation()) {
      // Setup the geometry context for the vertex validation system:
      DT_THROW_IF(!has_logical(), std::logic_error, "Missing logical volume in '" << get_name() << "' vertex generator!");
      _grab_vertex_validation().grab_geometry_context().set_local_candidate_vertex(vertex_);
      _grab_vertex_validation().grab_geometry_context().set_logical_volume(*_log_vol_);
    }

    return;
  }

} // end of namespace genvtx

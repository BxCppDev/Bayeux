/* polycone_vg.cc
 */

// Ourselves
#include <genvtx/polycone_vg.h>

// Standard library
#include <stdexcept>
#include <sstream>

// Third party
// - datatools
#include <datatools/units.h>
#include <datatools/properties.h>
#include <datatools/exception.h>
// - mygsl
#include <mygsl/rng.h>
// - geomtools
#include <geomtools/cone.h>

// This project
#include <genvtx/ring_random_tools.h>
#include <genvtx/conical_frustrum_random_tools.h>

namespace genvtx {

  GENVTX_VG_REGISTRATION_IMPLEMENT(polycone_vg,"genvtx::polycone_vg");

  int polycone_vg::get_mode () const
  {
    return _mode_;
  }

  void polycone_vg::set_mode (int mode_)
  {
    DT_THROW_IF ((mode_ !=  MODE_BULK) && (mode_ !=  MODE_SURFACE),
                 std::logic_error, "Invalid mode !");
    _mode_ = mode_;
    return;
  }

  void polycone_vg::set_angles (double theta_min_, double theta_max_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    DT_THROW_IF (theta_min_ >= theta_max_, std::range_error,
                 "Invalid angles (" << theta_min_ << " >=  " << theta_max_ << ") !");
    _theta_min_ = theta_min_;
    _theta_max_ = theta_max_;
    return;
  }

  void polycone_vg::set_surface_mask (int surface_mask_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _surface_mask_ = surface_mask_;
    return;
  }

  void polycone_vg::set_bulk_mask (int bulk_mask_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _bulk_mask_ = bulk_mask_;
    return;
  }

  void polycone_vg::set_skin_skip (double skin_skip_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _skin_skip_ = skin_skip_;
    return;
  }

  void polycone_vg::set_skin_thickness (double skin_thickness_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _skin_thickness_ = skin_thickness_;
    return;
  }

  void polycone_vg::set_bulk (int bulk_mask_, double /*skin_thickness_*/)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _mode_ = MODE_BULK;
    set_bulk_mask (bulk_mask_);
    return;
  }

  void polycone_vg::set_surface (int surface_mask_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _mode_ = MODE_SURFACE;
    set_surface_mask (surface_mask_);
    return;
  }

  void polycone_vg::set_polycone (const geomtools::polycone & polycone_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _polycone_ = polycone_;
    return;
  }

  const geomtools::polycone & polycone_vg::get_polycone () const
  {
    return _polycone_;
  }

  void polycone_vg::set_active_frustrum(size_t index_, bool active_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
    if (! _active_frustra_.size()) {
      DT_THROW_IF(! _polycone_.is_valid(), std::logic_error, "No valid polycone !");
      _active_frustra_.assign(_polycone_.number_of_frustra(), false);
    }
    DT_THROW_IF(index_ >= _active_frustra_.size(),
                 std::range_error,
                "Invalid frustrum index (" << index_ << ") !");
    DT_LOG_TRACE(get_logging_priority(), "_active_frustra_[" << index_ << "] = " << active_);
    _active_frustra_[index_] = active_;
    return;
  }

  void polycone_vg::set_active_all_frustrum()
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
    if (! _active_frustra_.size()) {
      DT_THROW_IF(! _polycone_.is_valid(), std::logic_error, "No valid polycone !");
      _active_frustra_.assign(_polycone_.number_of_frustra(), false);
    }
    for (size_t ifrustrum = 0; ifrustrum < _active_frustra_.size(); ifrustrum++) {
      set_active_frustrum(ifrustrum, true);
    }
    return;
  }

  bool polycone_vg::is_initialized () const
  {
    return _initialized_;
  }

  polycone_vg::polycone_vg() : genvtx::i_vertex_generator()
  {
    _initialized_ = false;
    _polycone_.reset();
    _set_defaults_();
    return;
  }

  polycone_vg::~polycone_vg()
  {
    if (is_initialized ()) reset ();
    return;
  }

  void polycone_vg::initialize (const ::datatools::properties & setup_,
                                ::datatools::service_manager & /*service_manager_*/,
                                ::genvtx::vg_dict_type & /*vgens_*/)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");

    // Parameters of the polycone vertex generator:
    int    mode           = _mode_;
    double skin_skip      = _skin_skip_;
    double skin_thickness = _skin_thickness_;
    int    surface_mask   = _surface_mask_;
    int    bulk_mask      = _bulk_mask_;
    double theta_min      = _theta_min_;
    double theta_max      = _theta_max_;
    double lunit = CLHEP::mm;
    double aunit = CLHEP::radian;
    bool treat_mode           = false;
    bool treat_surface_mask   = false;
    bool treat_bulk_mask      = false;
    bool treat_skin_skip      = false;
    bool treat_skin_thickness = false;
    //bool treat_angles         = false;

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
      std::string lunit_str = setup_.fetch_string ("length_unit");
      lunit = datatools::units::get_length_unit_from (lunit_str);
    }

    if (setup_.has_key ("angle_unit")) {
      std::string aunit_str = setup_.fetch_string ("angle_unit");
      aunit = datatools::units::get_angle_unit_from (aunit_str);
    }

    if (setup_.has_key ("theta_min")) {
      theta_min = setup_.fetch_real ("theta_min");
      if (setup_.has_explicit_unit("theta_min")) {
        theta_min *= aunit;
      }
    }

    if (setup_.has_key ("theta_max")) {
      theta_max = setup_.fetch_real ("theta_max");
      if (setup_.has_explicit_unit("theta_max")) {
        theta_max *= aunit;
      }
    }

    set_angles(theta_min, theta_max);

    if (setup_.has_key ("skin_skip")) {
      skin_skip = setup_.fetch_real ("skin_skip");
      if (! setup_.has_explicit_unit ("skin_skip")) {
        skin_skip *= lunit;
      }
      treat_skin_skip = true;
    }

    if (setup_.has_key ("skin_thickness")) {
      skin_thickness = setup_.fetch_real ("skin_thickness");
      if (! setup_.has_explicit_unit ("skin_skip")) {
        skin_thickness *= lunit;
      }
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
          surface_mask = geomtools::polycone::FACE_ALL;
          break;
        } else if (surfaces[i] == "inner_side") {
          surface_mask |= geomtools::polycone::FACE_INNER_SIDE;
        } else if (surfaces[i] == "outer_side") {
          surface_mask |= geomtools::polycone::FACE_OUTER_SIDE;
        } else if (surfaces[i] == "bottom") {
          surface_mask |= geomtools::polycone::FACE_BOTTOM;
        } else if (surfaces[i] == "top") {
          surface_mask |= geomtools::polycone::FACE_TOP;
        } else {
          DT_THROW_IF(true, std::logic_error, "Invalid surface label '" << surfaces[i] << "' !");
        }
      }
    }

    if (mode == MODE_BULK) {
      std::vector<std::string> bulks;
     if (setup_.has_key ("bulks")) {
        setup_.fetch ("bulks", bulks);
        treat_bulk_mask = true;
      }
      for (size_t i = 0; i < bulks.size (); i++) {
        if (bulks[i] == "all") {
          bulk_mask = BULK_ALL;
          break;
        } else if (bulks[i] == "body") {
          bulk_mask |= BULK_BODY;
        } else if (bulks[i] == "cavity") {
          bulk_mask |= BULK_CAVITY;
        } else {
          DT_THROW_IF(true, std::logic_error, "Invalid bulk label '" << bulks[i] << "' !");
        }
      }
    }

    if (treat_mode) set_mode(mode);
    if (treat_skin_skip) set_skin_skip(skin_skip);
    if (treat_skin_thickness) set_skin_thickness(skin_thickness);
    if (mode == MODE_SURFACE && treat_surface_mask) set_surface_mask(surface_mask);
    if (mode == MODE_BULK && treat_bulk_mask) set_bulk_mask(bulk_mask);

    if (! _polycone_.is_valid ()) {
      geomtools::polycone pol;
      pol.initialize(setup_);
      set_polycone(pol);
    }

    // Dimension the array if not already done:
    if (! _active_frustra_.size()) {
      _active_frustra_.assign(_polycone_.number_of_frustra(), false);
    }

    if (setup_.has_flag("polycone.all_frustrum")) {
      set_active_all_frustrum();
    } else {
      // Check fustrum activation:
      for (size_t ifrustrum = 0; ifrustrum < _active_frustra_.size(); ifrustrum++) {
        std::ostringstream frustrum_label;
        frustrum_label << "polycone.frustrum." << ifrustrum;
        if (setup_.has_flag(frustrum_label.str())) {
          set_active_frustrum(ifrustrum, true);
        }
      }
    }

    // Check activation of frustrum
    DT_THROW_IF(std::find(_active_frustra_.begin(),
                          _active_frustra_.end(), true) == _active_frustra_.end(),
                std::logic_error, "No polycone frustrum has been activated !");

    _init_ ();
    _initialized_ = true;
    return;
  }

  void polycone_vg::reset()
  {
    DT_THROW_IF (! is_initialized (), std::logic_error, "Not initialized !");
    _reset_ ();
    _initialized_ = false;
    return;
  }

  void polycone_vg::_init_ ()
  {
    DT_LOG_TRACE(get_logging_priority(), "Entering...");
    _volume_inner_frustra_.assign(_active_frustra_.size(), 0.0);
    _volume_frustra_.assign(_active_frustra_.size(), 0.0);
    _surface_inner_frustra_.assign(_active_frustra_.size(), 0.0);
    _surface_outer_frustra_.assign(_active_frustra_.size(), 0.0);
    double total_inner_surface = 0.0;
    double total_outer_surface = 0.0;
    double total_inner_volume = 0.0;
    double total_volume = 0.0;
    {
      int frustrum_index = 0;
      geomtools::polycone::rz_col_type::const_iterator i = _polycone_.points().begin ();
      double z0    = i->first;
      double rmin0 = i->second.rmin;
      double rmax0 = i->second.rmax;
      geomtools::polycone::rz_col_type::const_iterator j = i;
      j++;
      while (j != _polycone_.points().end ()) {
        double z1    = j->first;
        double rmin1 = j->second.rmin;
        double rmax1 = j->second.rmax;
        if (_active_frustra_[frustrum_index]) {
          double v_outer = geomtools::cone::compute_frustrum_volume(z0, z1, rmax0, rmax1);
          double v_inner = geomtools::cone::compute_frustrum_volume(z0, z1, rmin0, rmin1);
          double s_outer = geomtools::cone::compute_frustrum_lateral_surface(z0, z1, rmax0, rmax1);
          double s_inner = geomtools::cone::compute_frustrum_lateral_surface(z0, z1, rmin0, rmin1);
          DT_LOG_TRACE(get_logging_priority(), "frustrum_index =" << frustrum_index );
          DT_LOG_TRACE(get_logging_priority(), "  v_outer =" << v_outer);
          DT_LOG_TRACE(get_logging_priority(), "  v_inner =" << v_inner);
          DT_LOG_TRACE(get_logging_priority(), "  s_outer =" << s_outer);
          DT_LOG_TRACE(get_logging_priority(), "  s_inner =" << s_inner);
          if (_mode_ == MODE_BULK) {
            if (_bulk_mask_ & BULK_CAVITY) {
              _volume_inner_frustra_[frustrum_index] = v_inner;
              total_inner_volume  += v_inner;
            }
            if (_bulk_mask_ & BULK_BODY) {
              _volume_frustra_[frustrum_index] = (v_outer - v_inner);
              total_volume        += (v_outer - v_inner);
            }
          } // MODE_BULK
          if (_mode_ == MODE_SURFACE) {
            if (_surface_mask_ & geomtools::polycone::FACE_INNER_SIDE) {
              _surface_inner_frustra_[frustrum_index] = s_inner;
              total_inner_surface += s_inner;
            }
            if (_surface_mask_ & geomtools::polycone::FACE_OUTER_SIDE) {
              _surface_outer_frustra_[frustrum_index] = s_outer;
              total_outer_surface += s_outer;
            }
          } // MODE_SURFACE
        } // if
        // Increment:
        j++;
        frustrum_index++;
        z0    = z1;
        rmax0 = rmax1;
        rmin0 = rmin1;
      } // while
    }
    DT_LOG_TRACE(get_logging_priority(), "total_inner_surface = " << total_inner_surface);
    DT_LOG_TRACE(get_logging_priority(), "total_outer_surface = " << total_outer_surface);
    DT_LOG_TRACE(get_logging_priority(), "total_inner_volume = " << total_inner_volume);
    DT_LOG_TRACE(get_logging_priority(), "total_volume = " << total_volume);
    DT_LOG_TRACE(get_logging_priority(), "polycone inner side surface = " << _polycone_.get_parameter("surface.inner_side"));
    DT_LOG_TRACE(get_logging_priority(), "polycone outer side surface = " << _polycone_.get_parameter("surface.outer_side"));
    DT_LOG_TRACE(get_logging_priority(), "polycone bottom surface = " << _polycone_.get_parameter("surface.bottom"));
    DT_LOG_TRACE(get_logging_priority(), "polycone top surface = " << _polycone_.get_parameter("surface.top"));
    DT_LOG_TRACE(get_logging_priority(), "polycone volume = " << _polycone_.get_parameter("volume"));
    if (_mode_ == MODE_SURFACE) {
      DT_THROW_IF (_surface_mask_ == 0, std::logic_error, "Surface mask is zero !");
      if (_surface_mask_ & geomtools::polycone::FACE_INNER_SIDE) {
        _sum_surface_weight_[0] = total_inner_surface;
      }
      if (_surface_mask_ & geomtools::polycone::FACE_OUTER_SIDE) {
        _sum_surface_weight_[1] = total_outer_surface;
      }
      if (_surface_mask_ & geomtools::polycone::FACE_BOTTOM) {
        _surface_bottom_ = _polycone_.get_parameter("surface.bottom");
        _sum_surface_weight_[2] = _surface_bottom_;
      }
      if (_surface_mask_ & geomtools::polycone::FACE_TOP) {
        _surface_top_ = _polycone_.get_parameter("surface.top");
        _sum_surface_weight_[3] = _surface_top_;
      }
      const double s =
          total_inner_surface
        + total_outer_surface
        + _surface_bottom_
        + _surface_top_;
      DT_LOG_DEBUG (get_logging_priority (), "Total surface = " << s);
      for (size_t i = 0; i < 4; i++) {
        _sum_surface_weight_[i] /= s;
        if (i > 0) {
          _sum_surface_weight_[i] += _sum_surface_weight_[i - 1];
        }
        DT_LOG_TRACE (get_logging_priority (), "Surface weight [" << i << "] = " << _sum_surface_weight_[i]);
      }
      // Compute probability cumulative distributions:
      for (size_t i = 0; i < _surface_inner_frustra_.size(); i++) {
        _surface_inner_frustra_[i] /= total_inner_surface;
        if (i > 0) _surface_inner_frustra_[i] += _surface_inner_frustra_[i-1];
      }
      for (size_t i = 0; i < _surface_outer_frustra_.size(); i++) {
        _surface_outer_frustra_[i] /= total_outer_surface;
        if (i > 0) _surface_outer_frustra_[i] += _surface_outer_frustra_[i-1];
      }
    }

    if (_mode_ == MODE_BULK) {
      DT_THROW_IF (_bulk_mask_ == 0, std::logic_error, "Bulk mask is zero !");
      double v = total_volume + total_inner_volume;
      DT_LOG_DEBUG (get_logging_priority (), "Total bulk = " << v);
      _sum_bulk_weight_[0] = total_volume;       // BULK_BODY
      _sum_bulk_weight_[1] = total_inner_volume; // BULK_CAVITY
      for (size_t i = 0; i < 2; i++) {
        _sum_bulk_weight_[i] /= v;
        if (i > 0) {
          _sum_bulk_weight_[i] += _sum_bulk_weight_[i - 1];
        }
        DT_LOG_TRACE (get_logging_priority (), "Bulk weight [" << i << "] = " << _sum_bulk_weight_[i]);
      }
      // Compute probability cumulative distributions:
      if (_bulk_mask_ & BULK_CAVITY) {
        for (size_t i = 0; i < _volume_inner_frustra_.size(); i++) {
          _volume_inner_frustra_[i] /= total_inner_volume;
          if (i > 0) _volume_inner_frustra_[i] += _volume_inner_frustra_[i-1];
        }
      }
      if (_bulk_mask_ & BULK_BODY) {
        for (size_t i = 0; i < _volume_frustra_.size(); i++) {
          _volume_frustra_[i] /= total_volume;
          if (i > 0) _volume_frustra_[i] += _volume_frustra_[i-1];
        }
      }
    }

    DT_LOG_TRACE(get_logging_priority(), "Exiting.");
    return;
  }


  /****************/

  void polycone_vg::_set_defaults_ ()
  {
    _polycone_.reset ();
    _mode_ = MODE_INVALID;
    _surface_mask_ = 0;
    _bulk_mask_ = 0;
    _skin_skip_ = 0.0;
    _skin_thickness_ = 0.0;
    _theta_min_ = 0.0;
    _theta_max_ = 2 * M_PI;
    for (size_t i = 0; i < 4; i++) {
      _sum_surface_weight_[i] = 0.0;
    }
    for (size_t i = 0; i < 2; i++) {
      _sum_bulk_weight_[i] = 0.0;
    }
    _active_frustra_.clear();
    _volume_inner_frustra_.clear();
    _volume_frustra_.clear();
    _surface_inner_frustra_.clear();
    _surface_outer_frustra_.clear();
    _surface_bottom_ = 0.0;
    _surface_top_ = 0.0;

    return;
  }

  void polycone_vg::_reset_ ()
  {
    _set_defaults_ ();
    return;
  }


  void polycone_vg::tree_dump (std::ostream & out_,
                               const std::string & title_,
                               const std::string & indent_,
                               bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    i_vertex_generator::tree_dump (out_, title_, indent_, true);
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Polycone    : " << _polycone_ << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Initialized : " << _initialized_ << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Mode :        " << _mode_ << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Surface mask: " << _surface_mask_ << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Bulk mask:    " << _bulk_mask_ << std::endl;
    for (size_t i = 0; i < _active_frustra_.size(); i++) {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Frustum #" << i << " : " << _active_frustra_[i] << std::endl;
    }
    if (_mode_ == MODE_BULK) {
      if (_bulk_mask_ & BULK_CAVITY) {
        for (size_t i = 0; i < _volume_inner_frustra_.size(); i++) {
          out_ << indent << datatools::i_tree_dumpable::tag
               << "Cavity volume frustrum #" << i << " : " << _volume_inner_frustra_[i] << std::endl;
        }
      }
      if (_bulk_mask_ & BULK_BODY) {
        for (size_t i = 0; i < _volume_frustra_.size(); i++) {
          out_ << indent << datatools::i_tree_dumpable::tag
               << "Body volume frustrum   #" << i << " : " << _volume_frustra_[i] << std::endl;
        }
      }
      for (int i = 0; i < 2; i++) {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Sum bulk weight #" << i << " : " << _sum_bulk_weight_[i] << std::endl;
      }
    }
    if (_mode_ == MODE_SURFACE) {
      if (_surface_mask_ & geomtools::polycone::FACE_INNER_SIDE) {
        for (size_t i = 0; i < _surface_inner_frustra_.size(); i++) {
          out_ << indent << datatools::i_tree_dumpable::tag
               << "Inner surface frustrum #" << i << " : " << _surface_inner_frustra_[i] << std::endl;
        }
      }
      if (_surface_mask_ & geomtools::polycone::FACE_OUTER_SIDE) {
        for (size_t i = 0; i < _surface_outer_frustra_.size(); i++) {
          out_ << indent << datatools::i_tree_dumpable::tag
               << "Outer surface frustrum #" << i << " : " << _surface_outer_frustra_[i] << std::endl;
        }
      }
      if (_surface_mask_ & geomtools::polycone::FACE_BOTTOM) {
          out_ << indent << datatools::i_tree_dumpable::tag
               << "Bottom surface : " << _surface_bottom_ << std::endl;
      }
      if (_surface_mask_ & geomtools::polycone::FACE_TOP) {
          out_ << indent << datatools::i_tree_dumpable::tag
               << "Top surface : " << _surface_top_ << std::endl;
      }
      for (int i = 0; i < 4; i++) {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Sum surface weight #" << i << " : " << _sum_surface_weight_[i] << std::endl;
      }
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Skin skip:    " << _skin_skip_ << std::endl;
    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Skin thickness: " << _skin_thickness_ << std::endl;
    return;
  }

  void polycone_vg::_shoot_vertex(::mygsl::rng & random_,
                                   ::geomtools::vector_3d & vertex_)
  {
    DT_THROW_IF (! is_initialized (), std::logic_error, "Not initialized !");
    geomtools::invalidate (vertex_);
    //double r = 0.0;
    double theta_min = _theta_min_;
    double theta_max = _theta_max_;

    if (_mode_ == MODE_BULK) {
      double r0 = random_.uniform ();
      double thickness = 0.0;
      if (_skin_thickness_ > 0.0) {
        thickness = _skin_thickness_;
      }
      if (r0 < _sum_bulk_weight_[0]) {
        // Volume:
        double r1 = r0 / _sum_bulk_weight_[0];
        for (size_t i = 0; i < _active_frustra_.size(); i++) {
          if (r1 <= _volume_frustra_[i]) {
            // std::cerr << "*** DEVEL *** polycone_vg::shoot_vertex:  -> Found frustrum " << i << std::endl;
            geomtools::polycone::frustrum_data fd;
            _polycone_.get_frustrum(i, fd);
            geomtools::vector_3d vtx
              = genvtx::right_circular_conical_frustrum::random_volume(random_,
                                                                       fd.a1, fd.b1,
                                                                       fd.a2, fd.b2,
                                                                       fd.z2 - fd.z1,
                                                                       thickness,
                                                                       theta_min, theta_max);
            vertex_ = vtx + geomtools::vector_3d(0.0, 0.0, fd.z1);
            break;
          }
        }
      } else {
        // Inner cavity
        double r1 = (r0 - _sum_bulk_weight_[0]) / (_sum_bulk_weight_[1] - _sum_bulk_weight_[0]);
        for (size_t i = 0; i < _active_frustra_.size(); i++) {
          if (r1 <= _volume_inner_frustra_[i]) {
            // std::cerr << "*** DEVEL *** polycone_vg::shoot_vertex:  -> Found frustrum " << i << std::endl;
            geomtools::polycone::frustrum_data fd;
            _polycone_.get_frustrum(i, fd);
            geomtools::vector_3d vtx
              = genvtx::right_circular_conical_frustrum::random_volume(random_,
                                                                       0.0, fd.a1,
                                                                       0.0, fd.a2,
                                                                       fd.z2 - fd.z1,
                                                                       thickness,
                                                                       theta_min, theta_max);
            vertex_ = vtx + geomtools::vector_3d(0.0, 0.0, fd.z1);
            break;
          }
        }
     }

      // double r_max = _polycone_.get_r () - 0.5 * _skin_thickness_;
      // r = sqrt (random_.uniform ()) * r_max;
      // z = (random_.uniform () - 0.5) * (_polycone_.get_z () - _skin_thickness_);
    }

    if (_mode_ == MODE_SURFACE) {
      // std::cerr << "*** DEVEL *** polycone_vg::shoot_vertex: MODE_SURFACE..." << std::endl;
      double r0 = random_.uniform ();
      // std::cerr << "*** DEVEL *** polycone_vg::shoot_vertex: r0 = " << r0 << std::endl;
      double thickness = 0.0;
      if (_skin_thickness_ > 0.0) {
        thickness = _skin_thickness_;
      }
      if (r0 < _sum_surface_weight_[0]) {
        // Inner surface:
        // std::cerr << "*** DEVEL *** polycone_vg::shoot_vertex: Inner surface..." << std::endl;
        double r1 = r0 / _sum_surface_weight_[0];
        // std::cerr << "*** DEVEL *** polycone_vg::shoot_vertex:  -> r1 = " << r1 << std::endl;
        for (size_t i = 0; i < _active_frustra_.size(); i++) {
          if (r1 <= _surface_inner_frustra_[i]) {
            // std::cerr << "*** DEVEL *** polycone_vg::shoot_vertex:  -> Found inner frustrum " << i << std::endl;
            geomtools::polycone::frustrum_data fd;
            _polycone_.get_frustrum(i, fd);
            geomtools::vector_3d vtx
              = genvtx::right_circular_conical_frustrum::random_surface(random_,
                                                                        fd.a1, fd.a2,
                                                                        fd.z2 - fd.z1,
                                                                        thickness,
                                                                        theta_min, theta_max);
            vertex_ = vtx + geomtools::vector_3d(0.0, 0.0, fd.z1);
            break;
          }
        }
      } else if (r0 < _sum_surface_weight_[1]) {
        // Outer surface:
        // std::cerr << "*** DEVEL *** polycone_vg::shoot_vertex: Outer surface..." << std::endl;
        double r1 = (r0 - _sum_surface_weight_[0]) / (_sum_surface_weight_[1] - _sum_surface_weight_[0]);
        // std::cerr << "*** DEVEL *** polycone_vg::shoot_vertex:  -> r1 = " << r1 << std::endl;
        for (size_t i = 0; i < _active_frustra_.size(); i++) {
          if (r1 <= _surface_outer_frustra_[i]) {
            // std::cerr << "*** DEVEL *** polycone_vg::shoot_vertex:  -> Found outer frustrum " << i << std::endl;
            geomtools::polycone::frustrum_data fd;
            _polycone_.get_frustrum(i, fd);
            geomtools::vector_3d vtx
              = genvtx::right_circular_conical_frustrum::random_surface(random_,
                                                                        fd.b1, fd.b2,
                                                                        fd.z2 - fd.z1,
                                                                        thickness,
                                                                        theta_min, theta_max);
            vertex_ = vtx + geomtools::vector_3d(0.0, 0.0, fd.z1);
            break;
          }
        }
      } else if (r0 < _sum_surface_weight_[2]) {
        // Bottom surface:
        // std::cerr << "*** DEVEL *** polycone_vg::shoot_vertex: Bottom surface..." << std::endl;
        double z0 = _polycone_.points().begin()->first + _skin_skip_;
        double r0_min = _polycone_.points().begin()->second.rmin;
        double r0_max = _polycone_.points().begin()->second.rmax;
        geomtools::vector_3d vtx =
          genvtx::ring_random_surface (random_, r0_max, r0_min, thickness, 0.0, 2*M_PI);
        vertex_ = vtx -  geomtools::vector_3d(0.0, 0.0, z0);
      } else {
        // Top surface:
        // std::cerr << "*** DEVEL *** polycone_vg::shoot_vertex: Top surface..." << std::endl;
        double zn = _polycone_.points().rbegin()->first + _skin_skip_;
        double rn_min = _polycone_.points().rbegin()->second.rmin;
        double rn_max = _polycone_.points().rbegin()->second.rmax;
        geomtools::vector_3d vtx =
          genvtx::ring_random_surface (random_, rn_max, rn_min, thickness, 0.0, 2*M_PI);
        vertex_ = vtx +  geomtools::vector_3d(0.0, 0.0, zn);
      }
    }
    return;
  }

} // end of namespace genvtx

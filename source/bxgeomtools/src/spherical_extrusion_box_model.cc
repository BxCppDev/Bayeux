// spherical_extrusion_box_model.cc

// Ourselves:
#include <geomtools/spherical_extrusion_box_model.h>

// Standard library:
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <cstdlib>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/units.h>

// This project:
#include <geomtools/circle.h>

namespace geomtools {

  // registration :
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(spherical_extrusion_box_model,
                                         "geomtools::spherical_extrusion_box_model")

  std::string spherical_extrusion_box_model::get_model_id () const
  {
    return "geomtools::spherical_extrusion_box_model";
  }

  const std::string & spherical_extrusion_box_model::get_material () const
  {
    return _material_;
  }

  const geomtools::subtraction_3d & spherical_extrusion_box_model::get_solid () const
  {
    return _solid_;
  }

  spherical_extrusion_box_model::spherical_extrusion_box_model ()
    : i_model("spherical_extrusion_box_model")
  {
    _material_ = "";
    _bottom_ = false;
    datatools::invalidate(_x_);
    datatools::invalidate(_y_);
    datatools::invalidate(_z_);
    datatools::invalidate(_r_extrusion_);
    datatools::invalidate(_r_sphere_);
    datatools::invalidate(_h_);
    return;
  }

  spherical_extrusion_box_model::~spherical_extrusion_box_model ()
  {
    return;
  }

  void spherical_extrusion_box_model::_at_construct (const datatools::properties & config_,
                                                     geomtools::models_col_type * /*models_*/)
  {
    DT_LOG_TRACE (get_logging_priority (), "Entering...");

    /*** parse properties ***/

    double lunit = CLHEP::mm;
    if (config_.has_key ("length_unit")) {
      const std::string lunit_str = config_.fetch_string ("length_unit");
      lunit = datatools::units::get_length_unit_from (lunit_str);
    }

    DT_THROW_IF (! config_.has_key ("material.ref"), std::logic_error, "Missing 'material.ref' property in spherical extrusion box model '" << get_name() << "' !");
    _material_ = config_.fetch_string ("material.ref");

    DT_THROW_IF (! config_.has_key ("x"), std::logic_error, "Missing 'x' property in spherical extrusion box model '" << get_name() << "' !");
    double x = config_.fetch_real ("x");
    if (! config_.has_explicit_unit ("x")) x *= lunit;

    DT_THROW_IF (! config_.has_key ("y"), std::logic_error, "Missing 'y' property in spherical extrusion box model '" << get_name() << "' !");
    double y = config_.fetch_real ("y");
    if (! config_.has_explicit_unit ("y")) y  *= lunit;

    DT_THROW_IF (! config_.has_key ("z"), std::logic_error, "Missing 'z' property in spherical extrusion box model '" << get_name() << "' !");
    double z = config_.fetch_real ("z");
    if (! config_.has_explicit_unit ("z")) z *= lunit;

    DT_THROW_IF (! config_.has_key ("r_extrusion"), std::logic_error, "Missing 'r_extrusion' property in spherical extrusion box model '" << get_name() << "' !");
    double re = config_.fetch_real ("r_extrusion");
    if (! config_.has_explicit_unit ("r_extrusion")) re *= lunit;

    DT_THROW_IF (! config_.has_key ("r_sphere"), std::logic_error, "Missing 'r_sphere' property in spherical extrusion box model '" << get_name() << "' !");
    double rs = config_.fetch_real ("r_sphere");
    if (! config_.has_explicit_unit ("r_sphere")) rs *= lunit;

    DT_LOG_TRACE (get_logging_priority (), "Properties are parsed !");

    _x_ = x;
    _y_ = y;
    _z_ = z;
    _r_extrusion_ = re;
    _r_sphere_    = rs;
    if (config_.has_flag ("bottom")) {
      _bottom_ = true;
    }

    DT_THROW_IF (_r_extrusion_ > _x_ || _r_extrusion_ > _y_,
                 std::logic_error,
                 "Extrusion radius is larger than mother box dimension in spherical extrusion box model '" << get_name() << "' !");

    DT_THROW_IF (_r_extrusion_ > _r_sphere_,
                 std::logic_error,
                 "Extrusion radius is larger than the spherical radius in spherical extrusion box model '" << get_name() << "' !");

    _mother_.set_x (_x_);
    _mother_.set_y (_y_);
    _mother_.set_z (_z_);
    _mother_.lock();
    DT_THROW_IF (! _mother_.is_valid (), std::logic_error, "Invalid dimension(s) for the mother box in spherical extrusion box model '" << get_name() << "' !");

    _extrusion_.set (_r_sphere_);
    _extrusion_.lock();

    const double a = std::sqrt (_r_sphere_ * _r_sphere_
                                - _r_extrusion_ * _r_extrusion_);
    const double c = _r_sphere_ - a;
    DT_THROW_IF (c > _z_, std::logic_error, "Mother box is not long enough (Z ) to host the extrusion in spherical extrusion box model '" << get_name() << "' !");
    _h_ = _z_ - c;
    double zsphere = 0.5 * _z_ + a;
    if (_bottom_) zsphere *= -1;
    DT_LOG_TRACE (get_logging_priority (), "x       =" << x / CLHEP::mm << " mm");
    DT_LOG_TRACE (get_logging_priority (), "y       =" << y / CLHEP::mm << " mm");
    DT_LOG_TRACE (get_logging_priority (), "z       =" << z / CLHEP::mm << " mm");
    DT_LOG_TRACE (get_logging_priority (), "re      =" << re / CLHEP::mm << " mm");
    DT_LOG_TRACE (get_logging_priority (), "rs      =" << rs / CLHEP::mm << " mm");
    DT_LOG_TRACE (get_logging_priority (), "h       =" << _h_ / CLHEP::mm << " mm");
    DT_LOG_TRACE (get_logging_priority (), "zsphere =" << zsphere / CLHEP::mm << " mm");

    const geomtools::placement sphere_extrusion_placement (0, 0, zsphere, 0, 0, 0);
    _solid_.set_shapes(_mother_, _extrusion_, sphere_extrusion_placement);
    {
      double r = _r_sphere_;
      double h = c;
      double vol = _mother_.get_volume();
      double extrusion_vol = M_PI * h * h * (3 * r - h) / 3;
      vol -= extrusion_vol;
      _solid_.set_forced_volume(vol);
    }

    // Install proposed 'stackable data' pointer in the shape:
    {
      geomtools::stackable_data * sd_ptr = new geomtools::stackable_data;
      sd_ptr->xmin = -0.5*_x_;
      sd_ptr->xmax = +0.5*_x_;
      sd_ptr->ymin = -0.5*_y_;
      sd_ptr->ymax = +0.5*_y_;
      if (! _bottom_) {
        sd_ptr->zmax = +0.5 * z - c;
        sd_ptr->zmin = -0.5 * z;
      } else {
        sd_ptr->zmin = -0.5 * z + c;
        sd_ptr->zmax = +0.5 * z;
      }
      _solid_.set_stackable_data (sd_ptr);
      DT_LOG_TRACE (get_logging_priority (), "Stackable data:");
      if (get_logging_priority () >= datatools::logger::PRIO_TRACE) {
        sd_ptr->tree_dump (std::cerr);
      }
    }

    // Install a dedicated drawer:
    _drawer_.reset(new wires_drawer(*this));
    _solid_.set_wires_drawer(*_drawer_);
    _solid_.lock();

    grab_logical ().set_name (i_model::make_logical_volume_name (get_name()));
    grab_logical ().set_shape (_solid_);
    grab_logical ().set_material_ref (_material_);

    DT_LOG_TRACE (get_logging_priority (), "Exiting.");
    return;
  }

  void spherical_extrusion_box_model::tree_dump (std::ostream & out_,
                                                 const std::string & title_,
                                                 const std::string & indent_,
                                                 bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    i_model::tree_dump (out_, title_, indent, true);

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Material : '" << _material_ << "'" << std::endl;

    {
      // Mother scintillator box:
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Mother box : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _mother_.tree_dump (out_,"",indent_oss.str ());
      }
    }

    {
      // Spherical extrusion:
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Spherical extrusion : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _extrusion_.tree_dump (out_,"",indent_oss.str ());
      }
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Bottom : '" << _bottom_ << "'" << std::endl;

    {
      // Solid:
      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "Solid : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
        _solid_.tree_dump (out_,"",indent_oss.str ());
      }
    }

    return;
  }

  spherical_extrusion_box_model::wires_drawer::wires_drawer(const spherical_extrusion_box_model & model_)
    : i_wires_drawer<spherical_extrusion_box_model>(model_)
  {
    return;
  }

  spherical_extrusion_box_model::wires_drawer::~wires_drawer()
  {
    return;
  }

  void spherical_extrusion_box_model::wires_drawer::generate_wires_self(wires_type & wires_,
                                                                        uint32_t options_) const
  {
    // The top level subtraction solid shape:
    const geomtools::subtraction_3d & solid = get().get_solid();

    // Extract specific rendering options for this model:
    const bool draw_mother    = ! (options_ & WR_SEBM_NO_MOTHER_FACES);
    const bool draw_extrusion = ! (options_ & WR_SEBM_NO_EXTRUSION_FACES);

    // Keep only base rendering bits:
    uint32_t base_options = options_ & WR_BASE_MASK;

    // Extract useful stuff (shapes and properties):
    const geomtools::i_composite_shape_3d::shape_type & s1 = solid.get_shape1();
    const geomtools::i_shape_3d & sh1 = s1.get_shape();
    const geomtools::i_composite_shape_3d::shape_type & s2 = solid.get_shape2();
    const geomtools::i_shape_3d & sh2 = s2.get_shape();
    const geomtools::box & mother_box = dynamic_cast<const geomtools::box &> (sh1);
    const geomtools::sphere & extrusion_sphere = dynamic_cast<const geomtools::sphere &> (sh2);
    const bool   bottom = get()._bottom_;
    const double h      = get()._h_;

    if (draw_mother) {
      // Draw mother box shape:
      uint32_t options = base_options;
      if (bottom) options |= box::WR_BOX_NO_BOTTOM_FACE;
      else options |= box::WR_BOX_NO_TOP_FACE;
      // Extract mother shape:
      mother_box.generate_wires_self(wires_, options);
    }

    if (draw_extrusion) {

      const double zcyl = mother_box.get_z();
      const double c = zcyl - h;
      const double rs = extrusion_sphere.get_r();
      const double a = rs - c;
      const double re = std::sqrt(rs * rs - a * a);

      // Surface extrusion circle:
      {
        double z = 0.5 * zcyl;
        if (bottom) z *= -1.0;
        geomtools::placement c1_placement(0., 0., z, 0. , 0. , 0.);
        circle c1(re);
        uint32_t options = base_options;
        c1.generate_wires(wires_, c1_placement, options);
      }

      // Deep extrusion circles:
      {
        double rfactor[2];
        rfactor[0]= 0.5;
        rfactor[1]= 0.25;
        for (size_t i = 0; i < 2; i++) {
          const double c2 = rfactor[i] * c;
          double z = 0.5 * mother_box.get_z() - c + c2;
          if (bottom) z *= -1.0;
          const double local_rs = extrusion_sphere.get_r();
          const double a2 = rs - c2;
          const double r2 = std::sqrt(local_rs *rs - a2 * a2);
          circle c1(r2);
          uint32_t options = base_options;
          placement c1_placement(0.0, 0.0, z, 0.0, 0.0, 0.0);
          c1.generate_wires(wires_, c1_placement, options);
        }
      }

      // Extrusion arcs:
      {
        uint32_t options = base_options;
        const double z = 0.5 * zcyl;
        const double theta0 = std::asin(re / rs);
        circle arc(rs, 2. * M_PI - theta0, 2 * theta0);
        double zs = z - c + rs;
        if (bottom) zs *= -1.0;
        placement arc1_placement;
        placement arc2_placement;
        if (! bottom) {
          arc1_placement.set(0.0, 0.0, zs,      0.0, M_PI/2.0, 0.0);
          arc2_placement.set(0.0, 0.0, zs, M_PI/2.0, M_PI/2.0, 0.0);
        } else {
          arc1_placement.set(0.0, 0.0, zs,      0.0, -M_PI/2.0, 0.0);
          arc2_placement.set(0.0, 0.0, zs, M_PI/2.0, -M_PI/2.0, 0.0);
        }
        arc.generate_wires(wires_, arc1_placement, options);
        arc.generate_wires(wires_, arc2_placement, options);
      }
    }

    return;
  }

} // end of namespace geomtools

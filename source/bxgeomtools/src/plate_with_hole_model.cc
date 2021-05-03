// plate_with_hole_model.cc

// Ourselves:
#include <geomtools/plate_with_hole_model.h>

// Standard library:
#include <cmath>
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/units.h>
#include <datatools/exception.h>

// This project:
// #include <geomtools/gnuplot_draw.h>

namespace geomtools {

  // registration :
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(plate_with_hole_model,
                                         "geomtools::plate_with_hole_model")

  std::string plate_with_hole_model::get_model_id() const
  {
    return "geomtools::plate_with_hole_model";
  }

  const std::string & plate_with_hole_model::get_material() const
  {
    return _material_;
  }

  const geomtools::subtraction_3d & plate_with_hole_model::get_solid() const
  {
    return _solid_;
  }

  plate_with_hole_model::plate_with_hole_model() : i_model()
  {
    _material_ = "";
    _box_hole_.reset();
    _cyl_hole_.reset();
    datatools::invalidate(_x_);
    datatools::invalidate(_y_);
    datatools::invalidate(_z_);
    datatools::invalidate(_r_hole_);
    datatools::invalidate(_x_hole_);
    datatools::invalidate(_y_hole_);
    datatools::invalidate(_x_pos_hole_);
    datatools::invalidate(_y_pos_hole_);
    return;
  }

  plate_with_hole_model::~plate_with_hole_model()
  {
    return;
  }

  void plate_with_hole_model::_at_construct(const datatools::properties & config_,
                                            geomtools::models_col_type * /*models_*/)
  {
    /* Parse properties */

    double lunit = CLHEP::mm;
    if (config_.has_key ("length_unit")) {
      const std::string lunit_str = config_.fetch_string("length_unit");
      lunit = datatools::units::get_length_unit_from(lunit_str);
    }

    DT_THROW_IF (!config_.has_key ("material.ref"), std::logic_error, "Missing 'material.ref' property in plate with hole model '" << get_name() << "' !");
    _material_ = config_.fetch_string ("material.ref");

    DT_THROW_IF (! config_.has_key ("x"), std::logic_error, "Missing 'x' property in plate with hole model '" << get_name() << "' !");
    double x = config_.fetch_real ("x");
    if (! config_.has_explicit_unit ("x")) {
      x *= lunit;
    }

    DT_THROW_IF (! config_.has_key ("y"), std::logic_error, "Missing 'y' property in plate with hole model '" << get_name() << "' !");
    double y = config_.fetch_real ("y");
    if (! config_.has_explicit_unit ("y")) {
      y *= lunit;
    }

    DT_THROW_IF (! config_.has_key ("z"), std::logic_error, "Missing 'z' property in plate with hole model '" << get_name() << "' !");
    double z = config_.fetch_real ("z");
    if (! config_.has_explicit_unit ("z")) {
      z *= lunit;
    }

    double r_hole;
    datatools::invalidate (r_hole);
    if (config_.has_key ("r_hole"))
      {
        r_hole = config_.fetch_real ("r_hole");
        if (! config_.has_explicit_unit ("r_hole")) {
          r_hole *= lunit;
        }
      }

    double x_hole;
    double y_hole;
    datatools::invalidate (x_hole);
    datatools::invalidate (y_hole);
    if (! datatools::is_valid (r_hole)) {
      DT_THROW_IF (! config_.has_key ("x_hole"), std::logic_error, "Missing 'x_hole' property in plate with hole model '" << get_name() << "' !");
      x_hole = config_.fetch_real ("x_hole");
      if (! config_.has_explicit_unit ("x_hole")) {
        x_hole *= lunit;
      }
      DT_THROW_IF (! config_.has_key ("y_hole"), std::logic_error, "Missing 'y_hole' property in plate with hole model '" << get_name() << "' !");
      y_hole = config_.fetch_real ("y_hole");
      if (! config_.has_explicit_unit ("y_hole")) {
        y_hole *= lunit;
      }
    }

    double x_pos_hole = 0.0;
    if (config_.has_key ("x_pos_hole")) {
      x_pos_hole = config_.fetch_real ("x_pos_hole");
      if (! config_.has_explicit_unit ("x_pos_hole")) {
        x_pos_hole *= lunit;
      }
    }

    double y_pos_hole = 0.0;
    if (config_.has_key ("y_pos_hole")) {
      y_pos_hole = config_.fetch_real ("y_pos_hole");
      if (! config_.has_explicit_unit ("y_pos_hole")) {
        y_pos_hole *= lunit;
      }
    }

    _x_ = x;
    _y_ = y;
    _z_ = z;
    _r_hole_ = r_hole;
    _x_hole_ = x_hole;
    _y_hole_ = y_hole;
    _z_hole_ = _z_ + CLHEP::micrometer;
    _x_pos_hole_ = x_pos_hole;
    _y_pos_hole_ = y_pos_hole;

    // Checks :
    double hole_x_min = x_pos_hole;
    double hole_x_max = x_pos_hole;
    double hole_y_min = y_pos_hole;
    double hole_y_max = y_pos_hole;
    if (datatools::is_valid (_r_hole_)) {
      hole_x_min -= _r_hole_;
      hole_x_max += _r_hole_;
      hole_y_min -= _r_hole_;
      hole_y_max += _r_hole_;
    } else {
      hole_x_min -= 0.5 * _x_hole_;
      hole_x_max += 0.5 * _x_hole_;
      hole_y_min -= 0.5 * _y_hole_;
      hole_y_max += 0.5 * _y_hole_;
    }

    DT_THROW_IF ((hole_x_min < -0.5 * _x_) || (hole_x_max > 0.5 * _x_),
                 std::logic_error,
                 "Hole size is too large for mother box dimension in plate with hole model '" << get_name() << "' !");

    DT_THROW_IF ((hole_y_min < -0.5 * _y_) || (hole_y_max > 0.5 * _y_),
                 std::logic_error,
                 "Hole size is too large for mother box dimension in plate with hole model '" << get_name() << "' !");

    _mother_.set_x (_x_);
    _mother_.set_y (_y_);
    _mother_.set_z (_z_);
    _mother_.lock();

    DT_THROW_IF (! _mother_.is_valid (),
                 std::logic_error,
                 "Invalid dimension(s) for the mother box in plate with hole model '" << get_name() << "' !");

    geomtools::placement hole_placement (_x_pos_hole_, _y_pos_hole_, 0.0, 0, 0, 0);
    double hole_vol = 0.0;
    if (datatools::is_valid (_r_hole_)) {
      _cyl_hole_.set_r (_r_hole_);
      _cyl_hole_.set_z (_z_hole_);
      _cyl_hole_.lock();
      _solid_.set_shapes(_mother_, _cyl_hole_, hole_placement);
      hole_vol = _cyl_hole_.get_volume();
    } else if (datatools::is_valid (_x_hole_) && datatools::is_valid (_y_hole_)) {
      _box_hole_.set_x (_x_hole_);
      _box_hole_.set_y (_y_hole_);
      _box_hole_.set_z (_z_hole_);
      _box_hole_.lock();
      _solid_.set_shapes(_mother_, _box_hole_, hole_placement);
      hole_vol = _box_hole_.get_volume();
    } else {
      DT_THROW_IF (true, std::logic_error, "No defined shape for hole in plate with hole model '" << get_name() << "' !");
    }
    hole_vol *= (_z_ / _z_hole_);
    _solid_.set_forced_volume(_mother_.get_volume() - hole_vol);

    // Install proposed 'stackable data' pointer in the shape:
    {
      geomtools::stackable_data * sd_ptr = new geomtools::stackable_data;
      sd_ptr->xmin = -0.5*_x_;
      sd_ptr->xmax = +0.5*_x_;
      sd_ptr->ymin = -0.5*_y_;
      sd_ptr->ymax = +0.5*_y_;
      sd_ptr->zmin = -0.5*_z_;
      sd_ptr->zmax = +0.5*_z_;
      _solid_.set_stackable_data (sd_ptr);
    }

    // Install a dedicated wires drawer:
    _drawer_.reset(new wires_drawer(*this));
    _solid_.set_wires_drawer(*_drawer_);
    _solid_.lock();

    grab_logical().set_name(i_model::make_logical_volume_name(get_name()));
    grab_logical().set_shape(_solid_);
    grab_logical().set_material_ref(_material_);

    return;
  }

  plate_with_hole_model::wires_drawer::wires_drawer(const plate_with_hole_model & model_)
    : i_wires_drawer<plate_with_hole_model>(model_)
  {
    return;
  }

  plate_with_hole_model::wires_drawer::~wires_drawer()
  {
    return;
  }

  void plate_with_hole_model::wires_drawer::generate_wires_self(wires_type & wires_,
                                                                uint32_t options_) const
  {
    // The top level subtraction solid shape:
    const geomtools::subtraction_3d & solid = get().get_solid();

    // Extract specific rendering options for this model:
    const bool draw_plate = ! (options_ & WR_PWHM_NO_PLATE_FACES);
    const bool draw_hole  = ! (options_ & WR_PWHM_NO_HOLE_FACES);

    // Extract base rendering options:
    uint32_t base_options = options_ & WR_BASE_MASK;

    if (draw_plate) {
      // Draw mother plate shape:
      uint32_t options = base_options;
      options |= box::WR_BOX_NO_BOTTOM_FACE;
      options |= box::WR_BOX_NO_TOP_FACE;
      const geomtools::i_composite_shape_3d::shape_type & s1 = solid.get_shape1();
      const geomtools::i_shape_3d & sh1 = s1.get_shape();
      // Extract mother plate shape:
      const geomtools::box & mother_box = dynamic_cast<const geomtools::box &>(sh1);
      mother_box.generate_wires_self(wires_, options);
    }

    double hole_x_pos = 0.0;
    double hole_y_pos = 0.0;
    double hole_z_pos = 0.0; // Always ZERO
    hole_x_pos = get()._x_pos_hole_;
    hole_y_pos = get()._y_pos_hole_;

    if (draw_hole) {
      // Draw extrusion shape:
      geomtools::placement hole_placement(hole_x_pos, hole_y_pos, hole_z_pos, 0., 0. , 0.);
      // Extract extrusion shape:
      const geomtools::i_composite_shape_3d::shape_type & s2 = solid.get_shape2();
      const geomtools::i_shape_3d & sh2 = s2.get_shape();
      if (sh2.get_shape_name() == "box") {
        const box & hole_box = dynamic_cast<const geomtools::box &>(sh2);
        uint32_t options = base_options;
        options |= box::WR_BOX_NO_BOTTOM_FACE;
        options |= box::WR_BOX_NO_TOP_FACE;
        hole_box.generate_wires(wires_, hole_placement, options);
      } else  if (sh2.get_shape_name() == "cylinder") {
        const cylinder & hole_cylinder = dynamic_cast<const geomtools::cylinder &>(sh2);
        uint32_t options = base_options;
        options |= cylinder::WR_CYL_NO_BOTTOM_FACE;
        options |= cylinder::WR_CYL_NO_TOP_FACE;
        hole_cylinder.generate_wires(wires_, hole_placement, options);
      }
    }

    return;
  }

  void plate_with_hole_model::tree_dump(std::ostream & out_,
                                        const std::string & title_,
                                        const std::string & indent_,
                                        bool inherit_) const
  {
    i_model::tree_dump (out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Material : '" << _material_ << "'" << std::endl;

    {
      // Mother scintillator box:
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Mother box : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent_;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _mother_.tree_dump (out_,"",indent_oss.str ());
      }
    }

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Hole X-position : " << _x_pos_hole_ / CLHEP::mm << " mm" << std::endl;
    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Hole Y-position : " << _y_pos_hole_ / CLHEP::mm << " mm" << std::endl;

    {
      // Hole:
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Hole : " << std::endl;
      if (_box_hole_.is_valid ())
        {
          std::ostringstream indent_oss;
          indent_oss << indent_;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
          _box_hole_.tree_dump (out_,"",indent_oss.str ());
        }
      if (_cyl_hole_.is_valid ())
        {
          std::ostringstream indent_oss;
          indent_oss << indent_;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
          _cyl_hole_.tree_dump (out_,"",indent_oss.str ());
        }
    }

    {
      // Solid:
      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "Solid : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent_;
        indent_oss << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
        _solid_.tree_dump (out_,"",indent_oss.str ());
      }
    }

    return;
  }

} // end of namespace geomtools

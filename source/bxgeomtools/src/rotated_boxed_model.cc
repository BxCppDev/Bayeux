// -*- mode: c++ ; -*-
/* rotated_boxed_model.cc
 */

#include <geomtools/rotated_boxed_model.h>

#include <exception>
#include <limits>

#include <datatools/units.h>

namespace geomtools {

  const geomtools::box & rotated_boxed_model::get_box () const
  {
    return _solid_;
  }

  const geomtools::box & rotated_boxed_model::get_solid () const
  {
    return _solid_;
  }

  const placement & rotated_boxed_model::get_boxed_placement () const
  {
    return _boxed_placement_;
  }

  void rotated_boxed_model::set_boxed_model (const i_model & model_)
  {
    DT_THROW_IF (is_constructed (), std::logic_error, "Operation not allowed ! Model has already been constructed");
    _boxed_model_ = &model_;
  }

  const i_model & rotated_boxed_model::get_boxed_model () const
  {
    return *_boxed_model_;
  }

  std::string rotated_boxed_model::get_model_id () const
  {
    return "geomtools::rotated_boxed_model";
  }

  rotated_boxed_model::rotated_boxed_model () : i_boxed_model ()
  {
    _boxed_model_ = 0;
    return;
  }

  rotated_boxed_model::~rotated_boxed_model ()
  {
    return;
  }

  void rotated_boxed_model::_at_construct (const datatools::properties & config_,
                                           models_col_type * models_)
  {
    DT_LOG_TRACE (get_logging_priority (), "Entering...");

    DT_THROW_IF (! config_.has_key ("material.ref"),
                 std::logic_error,
                 "Missing 'material.ref' property in rotated boxed model '" << get_name() << "' !");
    const std::string material_name = config_.fetch_string ("material.ref");

    /*** length unit ***/
    double lunit = CLHEP::mm;
    if (config_.has_key ("length_unit")) {
      const std::string length_unit_str = config_.fetch_string ("length_unit");
      lunit = datatools::units::get_length_unit_from (length_unit_str);
    }

    double aunit = CLHEP::degree;
    /*** angle unit ***/
    if (config_.has_key ("angle_unit")) {
      const std::string angle_unit_str = config_.fetch_string ("angle_unit");
      aunit = datatools::units::get_angle_unit_from (angle_unit_str);
    }

    // fetch the label of the rotated boxed model:
    std::string rotated_label = "rotated";
    if (config_.has_key ("rotated.label")) {
      rotated_label = config_.fetch_string ("rotated.label");
    }

    // fetch the rotation axis:
    DT_THROW_IF (! config_.has_key ("rotated.axis"), std::logic_error,
                 "Missing 'rotated.axis' property for model '" << get_name() << "' !");
    const std::string rotation_axis_label = config_.fetch_string ("rotated.axis");

    bool use_special_angle = false;
    std::string special_rotation_angle_label;
    double rotation_angle;
    datatools::invalidate (rotation_angle);
    if (config_.has_key ("rotated.special_angle")) {
      special_rotation_angle_label = config_.fetch_string ("rotated.special_angle");
      use_special_angle = true;
    } else if (config_.has_key ("rotated.angle")) {
      rotation_angle = config_.fetch_real ("rotated.angle");
      if (! config_.has_explicit_unit ("rotated.angle")) {
        rotation_angle *= aunit;
      }
      use_special_angle = false;
    } else {
      DT_THROW (std::logic_error,
                "Missing 'rotated.special_angle' or 'rotation.angle' property for model '" << get_name() << "' !");
    }

    DT_THROW_IF (! config_.has_key ("rotated.model"),
                 std::logic_error,
                 "Missing 'boxed_model' property for model '" << get_name() << "' !");
    const std::string boxed_model_name = config_.fetch_string ("rotated.model");

    const int rotation_axis = get_rotation_axis_from_label (rotation_axis_label);
    DT_THROW_IF (! check_rotation_axis (rotation_axis),
                 std::logic_error,
                 "Invalid rotation axis for model '" << get_name() << "' !");

    // XXXX
    // dimension of the mother box:
    double x, y, z;
    datatools::invalidate (x);
    datatools::invalidate (y);
    datatools::invalidate (z);
    // special angles: 0, 90, 180, 270 degrees
    int special_rotation_angle = ROTATION_ANGLE_INVALID;
    if (use_special_angle) {
      special_rotation_angle =
        get_special_rotation_angle_from_label (special_rotation_angle_label);
      DT_THROW_IF (! check_special_rotation_angle (special_rotation_angle),
                   std::logic_error,
                   "Invalid rotation angle (" << special_rotation_angle_label << ") in model '" << get_name() << "'!");
    } else { // arbitrary angles:
      if (config_.has_key ("x")) {
        x = config_.fetch_real ("x");
        if (! config_.has_explicit_unit ("x")) {
          x *= lunit;
        }
      } else {
        DT_THROW_IF ((rotation_axis == ROTATION_AXIS_Y) || (rotation_axis == ROTATION_AXIS_Z),
                     std::logic_error,
                     "Missing 'x' property  in model '" << get_name() << "'!");
      }

      if (config_.has_key ("y")) {
        y = config_.fetch_real ("y");
        if (! config_.has_explicit_unit ("y")) {
          y *= lunit;
        }
      } else {
        DT_THROW_IF ((rotation_axis == ROTATION_AXIS_X) || (rotation_axis == ROTATION_AXIS_Z),
                     std::logic_error,
                     "Missing 'y' property in model '" << get_name() << "' !");
      }

      if (config_.has_key ("z")) {
        z = config_.fetch_real ("z");
        if (! config_.has_explicit_unit ("z")) {
          z *= lunit;
        }
      } else {
        DT_THROW_IF ((rotation_axis == ROTATION_AXIS_X) || (rotation_axis == ROTATION_AXIS_Y),
                     std::logic_error,
                     "Missing 'z' property in model '" << get_name() << "'!");
      }
    }

    DT_THROW_IF (! models_, std::logic_error, "Missing dictionary of models!");
    // Boxed model:
    {
      models_col_type::const_iterator found = models_->find (boxed_model_name);
      i_model * the_model = nullptr;
      // 2022-08-16 FM: fix crappy test on ptr
      DT_THROW_IF (found == models_->end(), std::logic_error,
                   "No model named '" << boxed_model_name << "' has been found!");
      the_model = found->second;
      // check if the model is stackable:
      DT_THROW_IF (! i_shape_3d::check_stackability(the_model->get_logical().get_shape(),
                                                    stackable::STACKABILITY_STRONG),
                   std::logic_error,
                   "The rotating model '" << the_model->get_name() << "' is not stackable on X/Y/Z axis in model '" << get_name() << "'!");
      set_boxed_model (*the_model);
    }

    const i_shape_3d & the_shape = _boxed_model_->get_logical ().get_shape ();

    // try to get stackable data from the shape:
    stackable_data the_SD;
    DT_THROW_IF (! i_shape_3d::pickup_stackable (the_shape, the_SD),
                 std::logic_error,
                 "Cannot stack '" << the_shape.get_shape_name () << "' shape !");
    const double gxmin = the_SD.get_xmin ();
    const double gxmax = the_SD.get_xmax ();
    const double gymin = the_SD.get_ymin ();
    const double gymax = the_SD.get_ymax ();
    const double gzmin = the_SD.get_zmin ();
    const double gzmax = the_SD.get_zmax ();
    double x0, y0, z0;
    x0 = y0 = z0 = 0.0;
    _boxed_placement_.set_translation (x0, y0, z0);
    _solid_.reset ();
    // if some special angle is used (0, 90, 180, 270 degrees):
    if (use_special_angle) {
      rotation_angle = get_special_rotation_angle (special_rotation_angle);
      // compute the new stackable data attached to the rotated solid:
      stackable_data the_new_SD;
      the_new_SD.xmin = gxmin;
      the_new_SD.xmax = gxmax;
      the_new_SD.ymin = gymin;
      the_new_SD.ymax = gymax;
      the_new_SD.zmin = gzmin;
      the_new_SD.zmax = gzmax;
      if (rotation_axis == ROTATION_AXIS_X) {
        if (special_rotation_angle ==  ROTATION_ANGLE_90) {
          the_new_SD.ymax = -gzmin;
          the_new_SD.zmax =  gymax;
          the_new_SD.ymin = -gzmax;
          the_new_SD.zmin =  gymin;
        }
        if (special_rotation_angle ==  ROTATION_ANGLE_180) {
          the_new_SD.ymax = -gymin;
          the_new_SD.zmax = -gzmin;
          the_new_SD.ymin = -gymax;
          the_new_SD.zmin = -gzmax;
        }
        if (special_rotation_angle ==  ROTATION_ANGLE_270) {
          the_new_SD.ymax =  gzmax;
          the_new_SD.zmax = -gymin;
          the_new_SD.ymin =  gzmin;
          the_new_SD.zmin = -gymax;
        }
      }

      if (rotation_axis == ROTATION_AXIS_Y) {
        if (special_rotation_angle ==  ROTATION_ANGLE_90) {
          the_new_SD.zmax = -gxmin;
          the_new_SD.xmax =  gzmax;
          the_new_SD.zmin = -gxmax;
          the_new_SD.xmin =  gzmin;
        }
        if (special_rotation_angle ==  ROTATION_ANGLE_180) {
          the_new_SD.zmax = -gzmin;
          the_new_SD.xmax = -gxmin;
          the_new_SD.zmin = -gzmax;
          the_new_SD.xmin = -gxmax;
        }
        if (special_rotation_angle ==  ROTATION_ANGLE_270) {
          the_new_SD.zmax =  gxmax;
          the_new_SD.xmax = -gzmin;
          the_new_SD.zmin =  gxmin;
          the_new_SD.xmin = -gzmax;
        }
      }

      if (rotation_axis == ROTATION_AXIS_Z) {
        if (special_rotation_angle ==  ROTATION_ANGLE_90) {
          the_new_SD.xmax = -gymin;
          the_new_SD.ymax =  gxmax;
          the_new_SD.xmin = -gymax;
          the_new_SD.ymin =  gxmin;
        }
        if (special_rotation_angle ==  ROTATION_ANGLE_180) {
          the_new_SD.xmax = -gxmin;
          the_new_SD.ymax = -gymin;
          the_new_SD.xmin = -gxmax;
          the_new_SD.ymin = -gymax;
        }
        if (special_rotation_angle ==  ROTATION_ANGLE_270) {
          the_new_SD.xmax =  gymax;
          the_new_SD.ymax = -gxmin;
          the_new_SD.xmin =  gymin;
          the_new_SD.ymin = -gxmax;
        }
      }
      DT_LOG_TRACE(get_logging_priority (), "New SD:");
      if (datatools::logger::is_trace(get_logging_priority())) {
        the_new_SD.tree_dump(std::clog, "", "[trace]: ");
      }
      x = (the_new_SD.xmax - the_new_SD.xmin);
      y = (the_new_SD.ymax - the_new_SD.ymin);
      z = (the_new_SD.zmax - the_new_SD.zmin);
    } else { // use an arbitrary angle:
      if (rotation_axis == ROTATION_AXIS_X) {
        x = gxmax - gxmin;
      }
      if (rotation_axis == ROTATION_AXIS_Y) {
        y = gymax - gymin;
      }
      if (rotation_axis == ROTATION_AXIS_Z) {
        z = gzmax - gzmin;
      }
    }
    _boxed_placement_.set_orientation(rotation_axis, rotation_angle);
    _solid_.set_x(x);
    _solid_.set_y(y);
    _solid_.set_z(z);
    _solid_.lock();

    DT_THROW_IF (! _solid_.is_valid (), std::logic_error, "Invalid solid in model '" << get_name() << "'!");

    grab_logical().set_name (i_model::make_logical_volume_name (get_name()));
    grab_logical().set_shape (_solid_);
    grab_logical().set_material_ref (material_name);
    // >>> Special treatment:
    // 2013-06-27 FM: add support for effective geometry informations:
    // Set effective shape:
    if (_boxed_model_->get_logical().has_shape()) {
      grab_logical ().set_effective_shape (_boxed_model_->get_logical().get_shape());
    }
    if (_boxed_model_->get_logical().has_effective_shape()) {
      grab_logical ().set_effective_shape (_boxed_model_->get_logical().get_effective_shape());
    }
    // Set effective material reference:
    if (_boxed_model_->get_logical().has_material_ref()) {
      grab_logical ().set_effective_material_ref (_boxed_model_->get_logical().get_material_ref());
    }
    if (_boxed_model_->get_logical().has_effective_material_ref()) {
      grab_logical ().set_effective_material_ref (_boxed_model_->get_logical().get_effective_material_ref());
    }
    // Set effective placement:
    grab_logical ().set_effective_relative_placement (_boxed_placement_);
    /*
    if (_boxed_model_->get_logical().has_effective_relative_placement()) {
      grab_logical ().set_effective_relative_placement (_boxed_model_->get_logical().get_effective_relative_placement());
    }
    */

    // <<<
    _boxed_phys_.set_name (i_model::make_physical_volume_name (rotated_label));
    _boxed_phys_.set_placement (_boxed_placement_);
    _boxed_phys_.set_logical (_boxed_model_->get_logical ());
    _boxed_phys_.set_mother (grab_logical ());

    DT_LOG_TRACE (get_logging_priority (), "Exiting.");
    return;
  }

  void rotated_boxed_model::tree_dump (std::ostream & out_,
                                       const std::string & title_ ,
                                       const std::string & indent_,
                                       bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    i_model::tree_dump (out_, title_, indent, true);

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Rotated placement (box) : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << skip_tag;
        _boxed_placement_.tree_dump (out_, "", indent_oss.str ());
      }
    }

    {
      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "Solid : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
        _solid_.tree_dump (out_, "", indent_oss.str ());
      }
    }

    return;
  }

  // registration :
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(rotated_boxed_model,"geomtools::rotated_boxed_model")

} // end of namespace geomtools

// end of rotated_boxed_model.cc

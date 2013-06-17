// -*- mode: c++ ; -*-
/* cylindric_extrusion_boxed_model.cc
 */

#include <geomtools/cylindric_extrusion_boxed_model.h>

#include <mygsl/min_max.h>

#include <datatools/units.h>
#include <datatools/exception.h>

#include <geomtools/i_boxed_model.h>
#include <geomtools/physical_volume.h>
#include <geomtools/placement.h>
#include <geomtools/visibility.h>
#include <geomtools/gnuplot_draw.h>


namespace geomtools {

  const geomtools::i_shape_3d & cylindric_extrusion_boxed_model::get_solid () const
  {
    return *_solid_;
  }

  void cylindric_extrusion_boxed_model::set_extrusion_x_position (double x_)
  {
    DT_THROW_IF (is_constructed (), std::logic_error, "Operation not allowed ! Model has already been constructed");
    _extrusion_x_ = x_;
  }

  void cylindric_extrusion_boxed_model::set_extrusion_y_position (double y_)
  {
    DT_THROW_IF (is_constructed (), std::logic_error, "Operation not allowed ! Model has already been constructed");
    _extrusion_y_ = y_;
  }

  void cylindric_extrusion_boxed_model::set_extrusion (double x_, double y_, double r_)
  {
    set_extrusion_x_position(x_);
    set_extrusion_y_position(y_);
    set_extrusion_radius(r_);
  }

  void cylindric_extrusion_boxed_model::set_extrusion_radius (double r_)
  {
    DT_THROW_IF (is_constructed (), std::logic_error, "Operation not allowed ! Model has already been constructed");
    DT_THROW_IF (r_ <= 0.0 * CLHEP::mm, std::domain_error, "Invalid R " << r_ / CLHEP::mm << " mm !");
    _extrusion_radius_ = r_;
    return;
  }

  void cylindric_extrusion_boxed_model::set_mother_x (double x_)
  {
    DT_THROW_IF (is_constructed (), std::logic_error, "Operation not allowed ! Model has already been constructed");
    DT_THROW_IF (x_ <= 0.0 * CLHEP::mm, std::domain_error, "Invalid X " << x_ / CLHEP::mm << " mm !");
    _mother_x_ = x_;
    return;
  }

  void cylindric_extrusion_boxed_model::set_mother_y (double y_)
  {
    DT_THROW_IF (is_constructed (), std::logic_error, "Operation not allowed ! Model has already been constructed");
    DT_THROW_IF (y_ <= 0.0 * CLHEP::mm, std::domain_error, "Invalid Y " << y_ / CLHEP::mm << " mm !");
    _mother_y_ = y_;
    return;
  }

  void cylindric_extrusion_boxed_model::set_mother_z (double z_)
  {
    DT_THROW_IF (is_constructed (), std::logic_error, "Operation not allowed ! Model has already been constructed");
    DT_THROW_IF (z_ <= 0.0 * CLHEP::mm, std::domain_error, "Invalid Z " << z_ / CLHEP::mm << " mm !");
    _mother_z_ = z_;
    return;
  }

  double cylindric_extrusion_boxed_model::get_extrusion_radius () const
  {
    return _extrusion_radius_;
  }

  double cylindric_extrusion_boxed_model::get_extrusion_x_position () const
  {
    return _extrusion_x_;
  }

  double cylindric_extrusion_boxed_model::get_extrusion_y_position () const
  {
    return _extrusion_y_;
  }

  double cylindric_extrusion_boxed_model::get_mother_x () const
  {
    return _mother_x_;
  }

  double cylindric_extrusion_boxed_model::get_mother_y () const
  {
    return _mother_y_;
  }

  double cylindric_extrusion_boxed_model::get_mother_z () const
  {
    return _mother_z_;
  }

  void cylindric_extrusion_boxed_model::set_material_name (const std::string & mn_)
  {
    DT_THROW_IF (is_constructed (), std::logic_error, "Operation not allowed ! Model has already been constructed");
    _material_name_ = mn_;
    return;
  }

  const std::string & cylindric_extrusion_boxed_model::get_material_name () const
  {
    return _material_name_;
  }

  std::string cylindric_extrusion_boxed_model::get_model_id () const
  {
    return "geomtools::cylindric_extrusion_boxed_model";
  }

  cylindric_extrusion_boxed_model::cylindric_extrusion_boxed_model ()
    : i_model ()
  {
    _material_name_ = "";
    _mother_x_ = std::numeric_limits<double>::quiet_NaN ();
    _mother_y_ = std::numeric_limits<double>::quiet_NaN ();
    _mother_z_ = std::numeric_limits<double>::quiet_NaN ();
    _extrusion_x_ = 0.0;
    _extrusion_y_ = 0.0;
    _extrusion_radius_ = std::numeric_limits<double>::quiet_NaN ();
    return;
  }

  cylindric_extrusion_boxed_model::~cylindric_extrusion_boxed_model ()
  {
    return;
  }

  void cylindric_extrusion_boxed_model::_at_construct (const std::string & name_,
                                                       const datatools::properties & config_,
                                                       models_col_type * models_)
  {
    DT_LOG_TRACE (get_logging_priority (), "Entering...");

    double lunit = CLHEP::mm;
    if (config_.has_key ("length_unit")) {
      const std::string lunit_str = config_.fetch_string ("length_unit");
      lunit = datatools::units::get_length_unit_from (lunit_str);
    }

    DT_THROW_IF (! config_.has_key ("x"), std::logic_error,
                 "Missing 'x' property in cylindric extrusion boxed model '" << name_ << "' !");
    double mother_x = config_.fetch_real ("x");
    if (! config_.has_explicit_unit ("x")) mother_x *= lunit;

    DT_THROW_IF (! config_.has_key ("y"), std::logic_error,
                 "Missing 'y' property in cylindric extrusion boxed model '" << name_ << "' !");
    double mother_y = config_.fetch_real ("y");
    if (! config_.has_explicit_unit ("y")) mother_y *= lunit;

    DT_THROW_IF (! config_.has_key ("z"), std::logic_error,
                 "Missing 'z' property in cylindric extrusion boxed model '" << name_ << "' !");
    double mother_z = config_.fetch_real ("z");
    if (! config_.has_explicit_unit ("z")) mother_z *= lunit;

    DT_THROW_IF (! config_.has_key ("extrusion_radius"), std::logic_error,
                 "Missing 'extrusion_radius' property in cylindric extrusion boxed model '" << name_ << "' !");
    double extrusion_radius = config_.fetch_real ("extrusion_radius");
    if (! config_.has_explicit_unit ("extrusion_radius")) extrusion_radius *= lunit;

    double extrusion_x = 0.0;
    if (config_.has_key ("extrusion_x")) {
      extrusion_x = config_.fetch_real ("extrusion_x");
      if (! config_.has_explicit_unit ("extrusion_x")) extrusion_x *= lunit;
    }

    double extrusion_y = 0.0;
    if (config_.has_key ("extrusion_y")) {
      extrusion_y = config_.fetch_real ("extrusion_y");
      if (! config_.has_explicit_unit ("extrusion_y")) extrusion_y *= lunit;
    }

    DT_THROW_IF (! config_.has_key ("material.ref"), std::logic_error,
                 "Missing 'material.ref' property in cylindric extrusion boxed model '" << name_ << "' !");
    const std::string material_name = config_.fetch_string ("material.ref");

    DT_THROW_IF (extrusion_radius >= 0.5 * mother_x, std::logic_error,
                 "Extrusion radius (" << extrusion_radius / CLHEP::mm << " mm) is too large (X-axis) in cylindric extrusion boxed model '" << name_ << "' !");
    DT_THROW_IF (extrusion_radius >= 0.5 * mother_y, std::logic_error,
                 "Extrusion radius (" << extrusion_radius / CLHEP::mm << " mm) is too large (Y-axis) in cylindric extrusion boxed model '" << name_ << "' !");

    set_material_name (material_name);
    set_mother_x (mother_x);
    set_mother_y (mother_y);
    set_mother_z (mother_z);
    set_extrusion (extrusion_x, extrusion_y, extrusion_radius);

    _mother_box_.reset ();
    _mother_box_.set_x (get_mother_x ());
    _mother_box_.set_y (get_mother_y ());
    _mother_box_.set_z (get_mother_z ());
    DT_THROW_IF (! _mother_box_.is_valid (), std::logic_error,
                 "Invalid box dimensions in cylindric extrusion boxed model '" << name_ << "' !");

    _extrusion_cylinder_.set_diameter (2 * get_extrusion_radius ());
    const double eps = 1.0e-5 * CLHEP::mm;
    _extrusion_cylinder_.set_z (get_mother_z () + eps);

    const placement p1 (vector_3d (0, 0, 0), 0, 0, 0);
    const placement p2 (vector_3d (_extrusion_x_, _extrusion_y_, 0), 0, 0, 0);
    _extruded_solid_.set_shape1 (_mother_box_, p1);
    _extruded_solid_.set_shape2 (_extrusion_cylinder_, p2);
    DT_LOG_TRACE (get_logging_priority (), "Extruded solid:");
    if (get_logging_priority () >= datatools::logger::PRIO_TRACE) {
      _extruded_solid_.dump (std::cerr);
    }

    const std::string extruded_label = "extruded_box";
    const std::string inner_name = "__" + get_logical ().get_name () + "." + extruded_label;
    _extruded_log_.set_name (i_model::make_logical_volume_name (inner_name));
    _extruded_log_.set_shape (_extruded_solid_);
    _extruded_log_.set_material_ref (_material_name_);
    _extruded_log_.set_geometry_model(*this);

    _extruded_placement_.set (vector_3d (0, 0, 0), 0, 0, 0);

    _extruded_phys_.set_name (i_model::make_physical_volume_name (extruded_label));
    _extruded_phys_.set_placement (_extruded_placement_);
    _extruded_phys_.set_logical (_extruded_log_);
    _extruded_phys_.set_mother (get_logical ());
    // Install default 'stackable data' pointer in the shape:
    {
      geomtools::stackable_data * sd_ptr = new geomtools::stackable_data;
      sd_ptr->xmin = -0.5 * _mother_x_;
      sd_ptr->xmax = +0.5 * _mother_x_;
      sd_ptr->ymin = -0.5 * _mother_y_;
      sd_ptr->ymax = +0.5 * _mother_y_;
      sd_ptr->zmin = -0.5 * _mother_z_;
      sd_ptr->zmax = +0.5 * _mother_z_;
      _extruded_solid_.set_stackable_data (sd_ptr);
      DT_LOG_TRACE (get_logging_priority (), "Stackable data:");
      if (get_logging_priority () >= datatools::logger::PRIO_TRACE) {
        sd_ptr->tree_dump (std::cerr);
      }
    }
    _extruded_solid_.set_user_draw ((void *) &cylindric_extrusion_boxed_model::gnuplot_draw_user_function);
    _solid_ = &_extruded_solid_;

    grab_logical ().set_name (i_model::make_logical_volume_name (name_));
    grab_logical ().set_shape (_extruded_solid_);
    grab_logical ().set_material_ref (material_name);

    DT_LOG_TRACE (get_logging_priority (), "Exiting.");
    return;
  }

  void cylindric_extrusion_boxed_model::tree_dump (std::ostream & out_,
                                                   const std::string & title_ ,
                                                   const std::string & indent_,
                                                   bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    i_model::tree_dump (out_, title_, indent, true);

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Material name : '" << get_material_name () << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Mother X : " << get_mother_x () / CLHEP::mm << " mm" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Mother Y : " << get_mother_y () / CLHEP::mm << " mm" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Mother Z : " << get_mother_z () / CLHEP::mm << " mm" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Extrusion radius : " << get_extrusion_radius () / CLHEP::mm << " mm" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Extrusion X position : " << get_extrusion_x_position () / CLHEP::mm << " mm" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Extrusion Y position : " << get_extrusion_y_position () / CLHEP::mm << " mm" << std::endl;

    {
      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "Solid : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
        _extruded_solid_.tree_dump (out_, "", indent_oss.str ());
      }
    }

    return;
  }

  void cylindric_extrusion_boxed_model::gnuplot_draw_user_function (std::ostream & out_,
                                                                    const geomtools::vector_3d & position_,
                                                                    const geomtools::rotation_3d & rotation_,
                                                                    const geomtools::i_object_3d & obj_,
                                                                    void *)
  {
    const geomtools::subtraction_3d * solid = dynamic_cast<const geomtools::subtraction_3d *>(&obj_);
    DT_THROW_IF (solid == 0,
                 std::logic_error,
                 "3D-object of '" << obj_.get_shape_name ()
                 << "' shape type has not the right type !");
    const geomtools::i_composite_shape_3d::shape_type & s1 = solid->get_shape1 ();
    const geomtools::i_composite_shape_3d::shape_type & s2 = solid->get_shape2 ();
    const geomtools::i_shape_3d & sh1 = s1.get_shape ();
    const geomtools::i_shape_3d & sh2 = s2.get_shape ();

    // Extract useful stuff (shapes and properties):
    const geomtools::box & mother_box = dynamic_cast<const geomtools::box &> (sh1);
    const geomtools::cylinder & extrusion_cylinder = dynamic_cast<const geomtools::cylinder &> (sh2);

    // Draw first shape:
    {
      geomtools::placement mother_world_placement;
      mother_world_placement.set_translation (position_);
      mother_world_placement.set_orientation (rotation_);

      geomtools::placement world_item_placement;
      mother_world_placement.child_to_mother (s1.get_placement (),
                                              world_item_placement);
      const geomtools::vector_3d   & sh1_pos = world_item_placement.get_translation ();
      const geomtools::rotation_3d & sh1_rot = world_item_placement.get_rotation ();
      geomtools::gnuplot_draw::draw_box (out_,
                                         sh1_pos,
                                         sh1_rot,
                                         mother_box);
    }

    // Draw second:
    {
      const geomtools::cylinder cyl (extrusion_cylinder.get_radius (), mother_box.get_z ());
      geomtools::placement mother_world_placement;
      mother_world_placement.set_translation (position_);
      mother_world_placement.set_orientation (rotation_);

      geomtools::placement world_item_placement;
      mother_world_placement.child_to_mother (s1.get_placement (),
                                              world_item_placement);
      const geomtools::vector_3d   & sh1_pos = world_item_placement.get_translation ();
      const geomtools::rotation_3d & sh1_rot = world_item_placement.get_rotation ();
      geomtools::gnuplot_draw::draw_cylinder (out_,
                                              sh1_pos,
                                              sh1_rot,
                                              cyl);
    }
    return;
  }

  // registration :
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(cylindric_extrusion_boxed_model,"geomtools::cylindric_extrusion_boxed_model");

} // end of namespace geomtools

// end of cylindric_extrusion_boxed_model.cc

// cylindric_extrusion_boxed_model.cc

// Ourselves:
#include <geomtools/cylindric_extrusion_boxed_model.h>

// Third party libraries:
// - Bayeux/datatools:
#include <datatools/units.h>
#include <datatools/exception.h>
// - Bayeux/mygsl:
#include <mygsl/min_max.h>

// This project:
#include <geomtools/i_boxed_model.h>
#include <geomtools/physical_volume.h>
#include <geomtools/placement.h>
#include <geomtools/visibility.h>
#include <geomtools/gnuplot_draw.h>

namespace geomtools {

  const geomtools::i_shape_3d & cylindric_extrusion_boxed_model::get_solid () const
  {
    return _extruded_solid_;
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

  void cylindric_extrusion_boxed_model::_at_construct (const datatools::properties & config_,
                                                       models_col_type * /*models_*/)
  {
    DT_LOG_TRACE (get_logging_priority (), "Entering...");

    double lunit = CLHEP::mm;
    if (config_.has_key ("length_unit")) {
      const std::string lunit_str = config_.fetch_string ("length_unit");
      lunit = datatools::units::get_length_unit_from (lunit_str);
    }

    DT_THROW_IF (! config_.has_key ("x"), std::logic_error,
                 "Missing 'x' property in cylindric extrusion boxed model '" << get_name() << "' !");
    double mother_x = config_.fetch_real ("x");
    if (! config_.has_explicit_unit ("x")) mother_x *= lunit;

    DT_THROW_IF (! config_.has_key ("y"), std::logic_error,
                 "Missing 'y' property in cylindric extrusion boxed model '" << get_name() << "' !");
    double mother_y = config_.fetch_real ("y");
    if (! config_.has_explicit_unit ("y")) mother_y *= lunit;

    DT_THROW_IF (! config_.has_key ("z"), std::logic_error,
                 "Missing 'z' property in cylindric extrusion boxed model '" << get_name() << "' !");
    double mother_z = config_.fetch_real ("z");
    if (! config_.has_explicit_unit ("z")) mother_z *= lunit;

    DT_THROW_IF (! config_.has_key ("extrusion_radius"), std::logic_error,
                 "Missing 'extrusion_radius' property in cylindric extrusion boxed model '" << get_name() << "' !");
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
                 "Missing 'material.ref' property in cylindric extrusion boxed model '" << get_name() << "' !");
    const std::string material_name = config_.fetch_string ("material.ref");

    // TODO: better check : for now we do not take into account extrusion_x and extrusion_y shifts !!!
    DT_THROW_IF (extrusion_radius >= 0.5 * mother_x, std::logic_error,
                 "Extrusion radius (" << extrusion_radius / CLHEP::mm
                 << " mm) is too large (X-axis) in cylindric extrusion boxed model '"
                 << get_name() << "' !");
    DT_THROW_IF (extrusion_radius >= 0.5 * mother_y, std::logic_error,
                 "Extrusion radius (" << extrusion_radius / CLHEP::mm
                 << " mm) is too large (Y-axis) in cylindric extrusion boxed model '"
                 << get_name() << "' !");
    DT_THROW_IF(extrusion_x + extrusion_radius >= +0.5 * mother_x, std::logic_error,
                 "Extrusion radius (" << extrusion_radius / CLHEP::mm
                 << " mm) is too large (X-axis) in cylindric extrusion boxed model '"
                 << get_name() << "' !");
    DT_THROW_IF(extrusion_x - extrusion_radius <= -0.5 * mother_x, std::logic_error,
                 "Extrusion radius (" << extrusion_radius / CLHEP::mm
                 << " mm) is too large (X-axis) in cylindric extrusion boxed model '"
                 << get_name() << "' !");
    DT_THROW_IF(extrusion_y + extrusion_radius >= +0.5 * mother_y, std::logic_error,
                 "Extrusion radius (" << extrusion_radius / CLHEP::mm
                 << " mm) is too large (Y-axis) in cylindric extrusion boxed model '"
                 << get_name() << "' !");
    DT_THROW_IF(extrusion_y - extrusion_radius <= -0.5 * mother_y, std::logic_error,
                 "Extrusion radius (" << extrusion_radius / CLHEP::mm
                 << " mm) is too large (y-axis) in cylindric extrusion boxed model '"
                 << get_name() << "' !");

    set_material_name (material_name);
    set_mother_x(mother_x);
    set_mother_y(mother_y);
    set_mother_z(mother_z);
    set_extrusion(extrusion_x, extrusion_y, extrusion_radius);

    _mother_box_.reset();
    _mother_box_.set_x(get_mother_x());
    _mother_box_.set_y(get_mother_y());
    _mother_box_.set_z(get_mother_z());
    _mother_box_.lock();
    DT_THROW_IF(! _mother_box_.is_valid(), std::logic_error,
                "Invalid box dimensions in cylindric extrusion boxed model '" << get_name() << "' !");

    _extrusion_cylinder_.set_diameter(2 * get_extrusion_radius());
    const double eps = 1.0e-5 * CLHEP::mm;
    _extrusion_cylinder_.set_z(get_mother_z() + eps);
    _extrusion_cylinder_.lock();

    const placement p1(vector_3d(0, 0, 0), 0, 0, 0);
    const placement p2(vector_3d(_extrusion_x_, _extrusion_y_, 0), 0, 0, 0);
    _extruded_solid_.set_shape1(_mother_box_, p1);
    _extruded_solid_.set_shape2(_extrusion_cylinder_, p2);
    {
      double vol = _mother_box_.get_volume();
      vol -= _extrusion_cylinder_.get_volume() * _mother_box_.get_z() / _extrusion_cylinder_.get_z();
      _extruded_solid_.set_forced_volume(vol);
    }
    _extruded_solid_.lock();
    DT_LOG_TRACE (get_logging_priority(), "Extruded solid:");
    if (get_logging_priority() >= datatools::logger::PRIO_TRACE) {
      _extruded_solid_.dump(std::cerr);
    }

    /*
    const std::string extruded_label = "extruded_box";
    const std::string inner_name = "__" + get_name () + "." + extruded_label;
    _extruded_log_.set_name (i_model::make_logical_volume_name (inner_name));
    _extruded_log_.set_shape (_extruded_solid_);
    _extruded_log_.set_material_ref (_material_name_);
    _extruded_log_.set_geometry_model(*this);
    _extruded_placement_.set (vector_3d (0, 0, 0), 0, 0, 0);

    _extruded_phys_.set_name (i_model::make_physical_volume_name (extruded_label));
    _extruded_phys_.set_placement (_extruded_placement_);
    _extruded_phys_.set_logical (_extruded_log_);
    _extruded_phys_.set_mother (get_logical ());
    */

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
    // Install a dedicated drawer:
    _drawer_.reset(new wires_drawer(*this));
    _extruded_solid_.set_wires_drawer(*_drawer_);
    _extruded_solid_.lock();

    grab_logical ().set_name (i_model::make_logical_volume_name (get_name()));
    grab_logical ().set_shape (_extruded_solid_);
    grab_logical ().set_material_ref (material_name);
    grab_logical ().set_geometry_model (*this);

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

    // out_ << indent << datatools::i_tree_dumpable::tag
    //      << "Material name : '" << get_material_name () << "'" << std::endl;

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

  cylindric_extrusion_boxed_model::wires_drawer::wires_drawer(const cylindric_extrusion_boxed_model & model_)
    : i_wires_drawer<cylindric_extrusion_boxed_model>(model_)
  {
    return;
  }

  cylindric_extrusion_boxed_model::wires_drawer::~wires_drawer()
  {
    return;
  }

  void cylindric_extrusion_boxed_model::wires_drawer::generate_wires_self(wires_type & wires_,
                                                                          uint32_t options_) const
  {
    // The top level subtraction solid shape:
    const geomtools::subtraction_3d & solid = get()._extruded_solid_;

    // Extract specific rendering options for this model:
    const bool draw_plate = ! (options_ & WR_CEBM_NO_MOTHER_FACES);
    const bool draw_hole  = ! (options_ & WR_CEBM_NO_EXTRUSION_FACES);

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

    if (draw_hole) {
      const double hole_x_pos = 0.0;
      const double hole_y_pos = 0.0;
      const double hole_z_pos = 0.0; // Always ZERO
      // Draw extrusion shape:
      geomtools::placement hole_placement(hole_x_pos, hole_y_pos, hole_z_pos, 0., 0. , 0.);
      // Extract extrusion shape:
      const geomtools::i_composite_shape_3d::shape_type & s2 = solid.get_shape2();
      const geomtools::i_shape_3d & sh2 = s2.get_shape();
      const cylinder & hole_cylinder = dynamic_cast<const geomtools::cylinder &>(sh2);
      uint32_t options = base_options;
      options |= cylinder::WR_CYL_NO_BOTTOM_FACE;
      options |= cylinder::WR_CYL_NO_TOP_FACE;
      hole_cylinder.generate_wires(wires_, hole_placement, options);
    }

    return;
  }

  // registration :
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(cylindric_extrusion_boxed_model,"geomtools::cylindric_extrusion_boxed_model")

} // end of namespace geomtools


// OCD support for class '::geomtools::cylindric_extrusion_boxed_model' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::geomtools::cylindric_extrusion_boxed_model,
                                ocd_)
{
  ocd_.set_class_name("geomtools::cylindric_extrusion_boxed_model");
  ocd_.set_class_description("A geometry model implementing a box with a cylindric extrusion centered along its Z axis");
  ocd_.set_class_library("geomtools");
  /*
  ocd_.set_class_documentation("  \n"
                               );
  */

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("length_unit")
      .set_terse_description("The length unit symbol")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_default_value_string("mm")
      .set_long_description("This property set the symbol of the default length\n"
                            "unit.                                             \n"
                            "Example ::                                        \n"
                            "                                                  \n"
                            "   length_unit : string = \"cm\"                  \n"
                            "                                                  \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("x")
      .set_terse_description("The X dimension of the box")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .set_long_description("This property sets X dimension of the box.      \n"
                            "Example ::                                      \n"
                            "                                                \n"
                            "   x : real = 24.5 cm                           \n"
                            "                                                \n"
                            "or ::                                           \n"
                            "                                                \n"
                            "   length_unit : string = \"cm\"                \n"
                            "   x : real = 24.5                              \n"
                            "                                                \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("y")
      .set_terse_description("The Y dimension of the box")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .set_long_description("This property sets Y dimension of the box.      \n"
                            "Example ::                                      \n"
                            "                                                \n"
                            "   y : real = 24.5 cm                           \n"
                            "                                                \n"
                            "                                                \n"
                            "or ::                                           \n"
                            "                                                \n"
                            "   length_unit : string = \"cm\"                \n"
                            "   y : real = 24.5                              \n"
                            "                                                \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("z")
      .set_terse_description("The Z dimension of the box")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .set_long_description("This property sets Z dimension of the box.      \n"
                            "Example ::                                      \n"
                            "                                                \n"
                            "   z : real = 24.5 cm                           \n"
                            "                                                \n"
                            "or ::                                           \n"
                            "                                                \n"
                            "   length_unit : string = \"cm\"                \n"
                            "   z : real = 24.5                              \n"
                            "                                                \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("extrusion_radius")
      .set_terse_description("The radius in the XY plane of the cylindric extrusion")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .set_long_description("This property sets Z dimension of the box.       \n"
                            "Example ::                                        \n"
                            "                                                \n"
                            "   extrusion_radius : real = 4.6 cm             \n"
                            "                                                \n"
                            "or ::                                             \n"
                            "                                                \n"
                            "   length_unit : string = \"cm\"                \n"
                            "   extrusion_radius : real = 4.6                \n"
                            "                                                \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("material.ref")
      .set_terse_description("The label of the material the volume is made of")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("This label of the material the volume is made of.\n")
      .add_example("Use 'inox' as the material::                    \n"
                   "                                                \n"
                   "    material.ref : string = \"inox\"            \n"
                   "                                                \n"
                   )
      ;
  }

  ocd_.set_configuration_hints("This model is configured through a configuration file that  \n"
                               "obeys the format of 'datatools::properties' setup file.     \n"
                               "Example ::                                                  \n"
                               "                                                           \n"
                               "  length_unit      : string = \"cm\"                       \n"
                               "  x                : real   =  30.0                        \n"
                               "  y                : real   =  30.0                        \n"
                               "  z                : real   =  10.0 mm                     \n"
                               "  extrusion_radius : real   =  40.0 mm                     \n"
                               "  material.ref     : string = \"inox\"                     \n"
                               "  \n"
                               );

  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::geomtools::cylindric_extrusion_boxed_model,
                               "geomtools::cylindric_extrusion_boxed_model")

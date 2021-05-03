// extruded_box_model.cc

// Ourselves:
#include <geomtools/extruded_box_model.h>

// Standard library:
#include <list>

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

  // Registration :
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(extruded_box_model, "geomtools::extruded_box_model")

  const geomtools::i_shape_3d & extruded_box_model::get_solid() const
  {
    return _extruded_box_;
  }

  const geomtools::extruded_box & extruded_box_model::get_extruded_box() const
  {
    return _extruded_box_;
  }

  void extruded_box_model::set_material_name(const std::string & mn_)
  {
    DT_THROW_IF(is_constructed(), std::logic_error,
                "Operation not allowed ! Model has already been constructed");
    _material_name_ = mn_;
    return;
  }

  const std::string & extruded_box_model::get_material_name() const
  {
    return _material_name_;
  }

  std::string extruded_box_model::get_model_id() const
  {
    return "geomtools::extruded_box_model";
  }

  extruded_box_model::extruded_box_model()
    : i_model()
  {
    _material_name_ = "";
    return;
  }

  extruded_box_model::~extruded_box_model()
  {
    return;
  }

  void extruded_box_model::_at_construct(const datatools::properties & config_,
                                         models_col_type * /*models_*/)
  {
    DT_LOG_TRACE(get_logging_priority(), "Entering...");

    double lunit = CLHEP::mm;
    if (config_.has_key("length_unit")) {
      const std::string lunit_str = config_.fetch_string("length_unit");
      lunit = datatools::units::get_length_unit_from(lunit_str);
    }

    DT_THROW_IF(! config_.has_key("material.ref"), std::logic_error,
                "Missing 'material.ref' property in extruded box model '" << get_name() << "' !");
    const std::string material_name = config_.fetch_string("material.ref");

    DT_THROW_IF(! config_.has_key("x"), std::logic_error,
                "Missing 'x' property in extruded box model '" << get_name() << "' !");
    double x = config_.fetch_real("x");
    if(! config_.has_explicit_unit("x")) x *= lunit;

    DT_THROW_IF(! config_.has_key("y"), std::logic_error,
                "Missing 'y' property in extruded box model '" << get_name() << "' !");
    double y = config_.fetch_real("y");
    if(! config_.has_explicit_unit("y")) y *= lunit;

    DT_THROW_IF(! config_.has_key("z"), std::logic_error,
                "Missing 'z' property in extruded box model '" << get_name() << "' !");
    double z = config_.fetch_real("z");
    if(! config_.has_explicit_unit("z")) z *= lunit;

    DT_THROW_IF(! config_.has_key("thickness"), std::logic_error,
                "Missing 'thickness' property in extruded box model '" << get_name() << "' !");
    double thickness = config_.fetch_real("thickness");
    if(! config_.has_explicit_unit("thickness")) thickness *= lunit;

    bool has_top    = true;
    bool has_bottom = true;

    if (config_.has_flag("remove_top")) {
      has_top = false;
    }
    if (config_.has_key("has_top")) {
      has_top = config_.fetch_boolean("has_top");
    }

    if (config_.has_flag("remove_bottom")) {
      has_bottom = false;
    }
    if (config_.has_key("has_bottom")) {
      has_bottom = config_.fetch_boolean("has_bottom");
    }

    _extruded_box_.set(x, y, z, thickness);
    _extruded_box_.set_top(has_top);
    _extruded_box_.set_bottom(has_bottom);
    _extruded_box_.lock();

    _mother_box_.reset();
    _mother_box_.set_x(_extruded_box_.get_x());
    _mother_box_.set_y(_extruded_box_.get_y());
    _mother_box_.set_z(_extruded_box_.get_z());
    DT_THROW_IF(! _mother_box_.is_valid(), std::logic_error,
                "Invalid mother box dimensions in extruded box model '" << get_name() << "' !");
    _mother_box_.lock();

    double daughter_z   = _extruded_box_.get_z();
    int    daughter_pos = 0;
    if(_extruded_box_.has_top()) {
      daughter_z -= _extruded_box_.get_thickness();
      daughter_pos--;
    }
    if(_extruded_box_.has_bottom()) {
      daughter_z -= _extruded_box_.get_thickness();
      daughter_pos++;
    }

    _daughter_box_.reset();
    _daughter_box_.set_x(_extruded_box_.get_x() - 2. * _extruded_box_.get_thickness());
    _daughter_box_.set_y(_extruded_box_.get_y() - 2. * _extruded_box_.get_thickness());
    _daughter_box_.set_z(daughter_z);
    _daughter_box_.lock();
    DT_THROW_IF(! _daughter_box_.is_valid(), std::logic_error,
                "Invalid daughter box dimensions in extruded box model '" << get_name() << "' !");

    const placement p2(vector_3d(0.0, 0.0, daughter_pos * _extruded_box_.get_thickness() / 2.),
                       0.0, 0.0, 0.0);
    _subtraction_box_.set_shapes(_mother_box_, _daughter_box_, p2);
    _subtraction_box_.lock();

    DT_LOG_TRACE(get_logging_priority(), "Extruded solid:");
    if(get_logging_priority() >= datatools::logger::PRIO_TRACE) {
      _subtraction_box_.dump(std::cerr);
    }

    {
      geomtools::stackable_data * sd_ptr = new geomtools::stackable_data;
      sd_ptr->xmin = -0.5 * _extruded_box_.get_x();
      sd_ptr->xmax = +0.5 * _extruded_box_.get_x();
      sd_ptr->ymin = -0.5 * _extruded_box_.get_y();
      sd_ptr->ymax = +0.5 * _extruded_box_.get_y();
      if (_extruded_box_.has_top() && _extruded_box_.has_bottom()) {
        /*          zmax
         *  +---------o---------+
         *  |  +-------------+- |
         *  |  |             |  |
         *  |  |             |  |
         *  |  +-------------+  |
         *  +---------o---------+
         *          zmin
         */
        sd_ptr->zmin = -0.5 * _extruded_box_.get_z();
        sd_ptr->zmax = +0.5 * _extruded_box_.get_z();
      }
      if (!_extruded_box_.has_top() && _extruded_box_.has_bottom()) {
        /*
         *  +--+ - - - - - - +--+
         *  |  |             |  |
         *  |  |             |  |
         *  |  |    zmax     |  |
         *  |  +------o------+  |
         *  +---------o---------+
         *          zmin
         */
        sd_ptr->zmin = -0.5 * _extruded_box_.get_z();
        sd_ptr->zmax = -0.5 * _extruded_box_.get_z() + _extruded_box_.get_thickness();
      }
      if (_extruded_box_.has_top() && !_extruded_box_.has_bottom()) {
        /*
         *          zmax
         *  +---------o---------+
         *  |  +------o------+- |
         *  |  |    zmin     |  |
         *  |  |             |  |
         *  |  |             |  |
         *  +--+ - - - - - - +--+
         *
         */
        sd_ptr->zmax = +0.5 * _extruded_box_.get_z();
        sd_ptr->zmin = +0.5 * _extruded_box_.get_z() - _extruded_box_.get_thickness();
      }
      if (!_extruded_box_.has_top() && !_extruded_box_.has_bottom()) {
        /*
         *
         *  +--+ - - - - - - +--+
         *  |  |             |  |
         *  |  |    zmax     |  |
         * -| -|- - - o - - -|- |-
         *  |  |    zmin     |  |
         *  |  |             |  |
         *  +--+ - - - - - - +--+
         *
         */
        sd_ptr->zmax = 0.0;
        sd_ptr->zmin = 0.0;
      }
      _subtraction_box_.set_stackable_data(sd_ptr);
      DT_LOG_TRACE(get_logging_priority(), "Stackable data:");
      if(get_logging_priority() >= datatools::logger::PRIO_TRACE) {
        sd_ptr->tree_dump(std::cerr);
      }
    }

    // Install a dedicated wires drawer:
    _drawer_.reset(new wires_drawer(_extruded_box_));
    _subtraction_box_.set_wires_drawer(*_drawer_);
    _subtraction_box_.lock();

    grab_logical().set_name(i_model::make_logical_volume_name(get_name()));
    grab_logical().set_shape(_subtraction_box_);
    grab_logical().set_material_ref(material_name);
    grab_logical().set_geometry_model(*this);

    DT_LOG_TRACE(get_logging_priority(), "Exiting.");
    return;
  }

  void extruded_box_model::tree_dump(std::ostream & out_,
                                     const std::string & title_ ,
                                     const std::string & indent_,
                                     bool inherit_) const
  {
    std::string indent;
    if(! indent_.empty()) indent = indent_;
    i_model::tree_dump(out_, title_, indent, true);

    // out_ << indent << datatools::i_tree_dumpable::tag
    //      << "Material name : '" << get_material_name() << "'" << std::endl;

    {
      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Solid : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
        _extruded_box_.tree_dump(out_, "", indent_oss.str());
      }
    }

    return;
  }

  extruded_box_model::wires_drawer::wires_drawer(const extruded_box & eb_)
    : i_wires_drawer<extruded_box>(eb_)
  {
    DT_THROW_IF(!eb_.is_locked(), std::logic_error, "Extruded box is not locked!");
    return;
  }

  extruded_box_model::wires_drawer::~wires_drawer()
  {
    return;
  }

  void extruded_box_model::wires_drawer::generate_wires_self(wires_type & wires_,
                                                             uint32_t options_) const
  {
    get().generate_wires_self(wires_, options_);
    return;
  }

} // end of namespace geomtools


// OCD support for class '::geomtools::extruded_box_model' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::geomtools::extruded_box_model, ocd_)
{
  ocd_.set_class_name("geomtools::extruded_box_model");
  ocd_.set_class_description("A geometry model implementing a box with a top and/or bottom cover along its Z axis");
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
                            "   x : real = 30.0 cm                           \n"
                            "                                                \n"
                            "or ::                                           \n"
                            "                                                \n"
                            "   length_unit : string = \"cm\"                \n"
                            "   x : real = 30.0                              \n"
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
                            "   y : real = 30.0 cm                           \n"
                            "                                                \n"
                            "                                                \n"
                            "or ::                                           \n"
                            "                                                \n"
                            "   length_unit : string = \"cm\"                \n"
                            "   y : real = 30.0                              \n"
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
                            "   z : real = 50.0 cm                           \n"
                            "                                                \n"
                            "or ::                                           \n"
                            "                                                \n"
                            "   length_unit : string = \"cm\"                \n"
                            "   z : real = 50.0                              \n"
                            "                                                \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("thickness")
      .set_terse_description("The thickness of the box")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .set_long_description("This property sets the thickness of the box.    \n"
                            "Example ::                                      \n"
                            "                                                \n"
                            "   thickness : real = 5.0 cm                    \n"
                            "                                                \n"
                            "or ::                                           \n"
                            "                                                \n"
                            "   length_unit : string = \"cm\"                \n"
                            "   thickness : real = 5.0                       \n"
                            "                                                \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("has_top")
      .set_terse_description("The flag to set a top cover to the box along its Z axis")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("This property sets the top cover to the box.    \n"
                            "Example ::                                      \n"
                            "                                                \n"
                            "   has_top : boolean = 0 [default is: true]     \n"
                            "                                                \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("has_top")
      .set_terse_description("The flag to set a bottom cover to the box along its Z axis")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("This property sets the bottom cover to the box. \n"
                            "Example ::                                      \n"
                            "                                                \n"
                            "   has_bottom : boolean = 1 [default is: true]  \n"
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
      .add_example("Use 'iron' as the material::                    \n"
                   "                                                \n"
                   "    material.ref : string = \"iron\"            \n"
                   "                                                \n"
                   )
      ;
  }

  ocd_.set_configuration_hints("This model is configured through a configuration file that \n"
                               "obeys the format of 'datatools::properties' setup file.    \n"
                               "Example ::                                                 \n"
                               "                                                           \n"
                               "  length_unit      : string  = \"cm\"                      \n"
                               "  x                : real    =  30.0                       \n"
                               "  y                : real    =  30.0                       \n"
                               "  z                : real    =  100.0 mm                   \n"
                               "  thickness        : real    =  10.0 mm                    \n"
                               "  has_top          : boolean =  1                          \n"
                               "  has_bottom       : boolean =  1                          \n"
                               "  material.ref     : string  = \"iron\"                    \n"
                               "                                                           \n"
                               );

  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::geomtools::extruded_box_model,
                               "geomtools::extruded_box_model")

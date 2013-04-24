// -*- mode: c++ ; -*-
/* ocd_support.cc
 */

#include <geomtools/ocd_support.h>

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
      .set_long_description("This property set the symbol of the default length\n"
                            "unit.                                             \n"
                            "Default value: \"mm\"                             \n"
                            "Example :                                         \n"
                            "  |                                               \n"
                            "  | length_unit : string = \"cm\"                 \n"
                            "  |                                               \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("x")
      .set_terse_description("The X dimension of the box")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .set_long_description("This property sets X dimension of the box.       \n"
                            "Default value: \"mm\"                            \n"
                            "Example :                                        \n"
                            "  |                                              \n"
                            "  | x : real = 24.5 cm                           \n"
                            "  |                                              \n"
                            "or :                                             \n"
                            "  | length_unit : string = \"cm\"                \n"
                            "  | x : real = 24.5                              \n"
                            "  |                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("y")
      .set_terse_description("The Y dimension of the box")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .set_long_description("This property sets Y dimension of the box.       \n"
                            "Default value: \"mm\"                            \n"
                            "Example :                                        \n"
                            "  |                                              \n"
                            "  | y : real = 24.5 cm                           \n"
                            "  |                                              \n"
                            "or :                                             \n"
                            "  | length_unit : string = \"cm\"                \n"
                            "  | y : real = 24.5                              \n"
                            "  |                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("z")
      .set_terse_description("The Z dimension of the box")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .set_long_description("This property sets Z dimension of the box.       \n"
                            "Default value: \"mm\"                            \n"
                            "Example :                                        \n"
                            "  |                                              \n"
                            "  | z : real = 24.5 cm                           \n"
                            "  |                                              \n"
                            "or :                                             \n"
                            "  | length_unit : string = \"cm\"                \n"
                            "  | z : real = 24.5                              \n"
                            "  |                                              \n"
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
                            "Default value: \"mm\"                            \n"
                            "Example :                                        \n"
                            "  |                                              \n"
                            "  | extrusion_radius : real = 4.6 cm             \n"
                            "  |                                              \n"
                            "or :                                             \n"
                            "  | length_unit : string = \"cm\"                \n"
                            "  | extrusion_radius : real = 4.6                \n"
                            "  |                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("material.ref")
      .set_terse_description("The label of the material the volume is made of")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("This label of the material the volume is made of.\n"
                            "Example :                                        \n"
                            "  |                                              \n"
                            "  |  material.ref : string = \"inox\"            \n"
                            "  |                                              \n"
                            )
      ;
  }

  ocd_.set_configuration_hints("This model is configured through a configuration file that  \n"
                               "obeys the format of 'datatools::properties' setup file.     \n"
                               "Example :                                                   \n"
                               " |                                                          \n"
                               " | length_unit      : string = \"cm\"                       \n"
                               " | x                : real   =  30.0                        \n"
                               " | y                : real   =  30.0                        \n"
                               " | z                : real   =  10.0 mm                     \n"
                               " | extrusion_radius : real   =  40.0 mm                     \n"
                               " | material.ref     : string = \"inox\"                     \n"
                               " | \n"
                               );

  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::geomtools::cylindric_extrusion_boxed_model,
                               "geomtools::cylindric_extrusion_boxed_model")

/*************************************************************************/

// OCD support for class '::geomtools::grid_model' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::geomtools::grid_model,
                                ocd_)
{
  ocd_.set_class_name("geomtools::grid_model");
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
      .set_long_description("This property set the symbol of the default length\n"
                            "unit.                                             \n"
                            "Default value: \"mm\"                             \n"
                            "Example :                                         \n"
                            "  |                                               \n"
                            "  | length_unit : string = \"cm\"                 \n"
                            "  |                                               \n"
                            )
      ;
  }


  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("material.ref")
      .set_terse_description("The label of the material the grid mother volume is made of")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("This label of the material the grid mother volume is made of.\n"
                            "Example :                                        \n"
                            "  |                                              \n"
                            "  |  material.ref : string = \"air\"             \n"
                            "  |                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("grid.force_stackable")
      .set_terse_description("Flag to force the use of explicit stacking informations while stacking the geometry model")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("This property triggers the use of some stacking  \n"
                            "informations for the geometry model being placed \n"
                            "on the grid layout.                              \n"
                            "Example :                                        \n"
                            "  |                                              \n"
                            "  | grid.force_stackable : boolean = 1           \n"
                            "  |                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("grid.plane")
      .set_terse_description("The label of the grid plane")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("This property set the label of the grid plane.    \n"
                            "Possible values are :  'xy' ,'xz' and 'yz'.       \n"
                            "Example :                                         \n"
                            "  |                                               \n"
                            "  | grid.plane : string = \"xy\"                  \n"
                            "  |                                               \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("grid.plane")
      .set_terse_description("The label of the grid plane")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("This property set the label of the grid plane.    \n"
                            "Possible values are :  'xy' ,'xz' and 'yz'.       \n"
                            "Example :                                         \n"
                            "  |                                               \n"
                            "  | grid.plane : string = \"xy\"                  \n"
                            "  |                                               \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("grid.x.number_of_items")
      .set_terse_description("The number of columns along the X axis")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(true)
      .set_triggered_by_label("grid.plane", "xy;xz")
      .set_long_description("The number of columns along the X axis.          \n"
                            "Example :                                        \n"
                            "  |                                              \n"
                            "  | grid.plane             : string = \"xy\"     \n"
                            "  | grid.x.number_of_items : integer = 4         \n"
                            "  |                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("grid.x.step")
      .set_terse_description("The step dimension along the X axis")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .set_triggered_by_label("grid.plane", "xy;xz")
      .set_long_description("The step dimension along the X axis.             \n"
                            "Example :                                        \n"
                            "  |                                              \n"
                            "  | grid.plane             : string = \"xy\"     \n"
                            "  | grid.x.number_of_items : integer = 4         \n"
                            "  | grid.x.step            : real    = 23.4 cm   \n"
                            "  |                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("grid.y.number_of_items")
      .set_terse_description("The number of columns along the Y axis")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(true)
      .set_triggered_by_label("grid.plane", "xy;yz")
      .set_long_description("The number of columns along the Y axis.          \n"
                            "Example :                                        \n"
                            "  |                                              \n"
                            "  | grid.plane : string = \"yz\"                 \n"
                            "  | grid.y.number_of_items : integer = 7         \n"
                            "  |                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("grid.y.step")
      .set_terse_description("The step dimension along the Y axis")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .set_triggered_by_label("grid.plane", "xy;yz")
      .set_long_description("The step dimension along the Y axis.             \n"
                            "Example :                                        \n"
                            "  |                                              \n"
                            "  | grid.plane             : string = \"xy\"     \n"
                            "  | grid.y.number_of_items : integer = 4         \n"
                            "  | grid.y.step            : real    = 7.3 mm    \n"
                            "  |                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("grid.z.number_of_items")
      .set_terse_description("The number of columns along the Z axis")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(true)
      .set_triggered_by_label("grid.plane", "xz;yz")
      .set_long_description("The number of columns along the Z axis.          \n"
                            "Example :                                        \n"
                            "  |                                              \n"
                            "  | grid.plane : string = \"yz\"                 \n"
                            "  | grid.z.number_of_items : integer = 3         \n"
                            "  |                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("grid.z.step")
      .set_terse_description("The step dimension along the Z axis")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .set_triggered_by_label("grid.plane", "xz;yz")
      .set_long_description("The step dimension along the Z axis.             \n"
                            "Example :                                        \n"
                            "  |                                              \n"
                            "  | grid.plane             : string = \"xz\"     \n"
                            "  | grid.z.number_of_items : integer = 2         \n"
                            "  | grid.z.step            : real    = 12.4 mm   \n"
                            "  |                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("grid.model")
      .set_terse_description("The name of the geometry model to be placed on the grid layout")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("The geometry model is searched for an external   \n"
                            "dictionnary of models, typically from a model    \n"
                            "factory.                                         \n"
                            "Example :                                        \n"
                            "  |                                              \n"
                            "  | grid.model : string = \"block\"              \n"
                            "  |                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("x")
      .set_terse_description("The X dimension of the box")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_long_description("This property sets X dimension of the box.       \n"
                            "Default value: \"mm\"                            \n"
                            "Example :                                        \n"
                            "  |                                              \n"
                            "  | x : real = 24.5 cm                           \n"
                            "  |                                              \n"
                            "or :                                             \n"
                            "  | length_unit : string = \"cm\"                \n"
                            "  | x : real = 24.5                              \n"
                            "  |                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("y")
      .set_terse_description("The Y dimension of the box")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_long_description("This property sets Y dimension of the box.       \n"
                            "Default value: \"mm\"                            \n"
                            "Example :                                        \n"
                            "  |                                              \n"
                            "  | y : real = 24.5 cm                           \n"
                            "  |                                              \n"
                            "or :                                             \n"
                            "  | length_unit : string = \"cm\"                \n"
                            "  | y : real = 24.5                              \n"
                            "  |                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("z")
      .set_terse_description("The Z dimension of the box")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_long_description("This property sets Z dimension of the box.       \n"
                            "Default value: \"mm\"                            \n"
                            "Example :                                        \n"
                            "  |                                              \n"
                            "  | z : real = 24.5 cm                           \n"
                            "  |                                              \n"
                            "or :                                             \n"
                            "  | length_unit : string = \"cm\"                \n"
                            "  | z : real = 24.5                              \n"
                            "  |                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("material.ref")
      .set_terse_description("The label of the material the volume is made of")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("This label of the material the volume is made of.\n"
                            "Example :                                        \n"
                            "  |                                              \n"
                            "  |  material.ref : string = \"inox\"            \n"
                            "  |                                              \n"
                            )
      ;
  }

  ocd_.set_configuration_hints("This model is configured through a configuration file that  \n"
                               "obeys the format of 'datatools::properties' setup file.     \n"
                               "Example :                                                   \n"
                               " |                                                          \n"
                               " | length_unit      : string = \"cm\"                       \n"
                               " | x                : real   =  30.0 mm                     \n"
                               " | y                : real   =  30.0 mm                     \n"
                               " | z                : real   =  1.3                         \n"
                               " | grid.model       : string =  \"brick\"                   \n"
                               " | grid.plane       : string =  \"xy\"                      \n"
                               " | grid.x.number_of_items : integer = 2                     \n"
                               " | grid.x.step            : real    = 12.4 mm               \n"
                               " | grid.y.number_of_items : integer = 4                     \n"
                               " | grid.y.step            : real    = 5.2 mm                \n"
                               " | material.ref     : string = \"air\"                      \n"
                               " |                                                          \n"
                               "It is also possible to add stacking informations to change  \n"
                               "the default stacking behaviour of the model :               \n"
                               "Example :                                                   \n"
                               " |                                                          \n"
                               " | grid.force_stackable       : boolean = 1                 \n"
                               " | grid.force_stackable.length_unit : string = \"mm\"       \n"
                               " | grid.force_stackable.xmin : real =  1.3                  \n"
                               " | grid.force_stackable.xmax : real = -3.4                  \n"
                               " | grid.force_stackable.ymin : real =  4.3                  \n"
                               " | grid.force_stackable.ymax : real =  2.4                  \n"
                               " |                                                          \n"
                               );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::geomtools::grid_model,
                               "geomtools::grid_model")


// end of ocd_support.cc

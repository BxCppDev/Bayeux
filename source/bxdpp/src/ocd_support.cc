// -*- mode: c++ ; -*-
/* ocd_support.cc
 */

#include <dpp/ocd_support.h>

/****************************************************************/
// OCD support for class '::dpp::dump_module' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::dpp::dump_module,ocd_)
{
  ocd_.set_class_name ("dpp::dump_module");
  ocd_.set_class_library ("dpp");
  ocd_.set_class_description ("A module that dumps the structure of the data record");

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("debug")
      .set_terse_description("The debug flag")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("The debug flag.                                \n"
                            "Default value is false.                        \n"
                            "Example :                                      \n"
                            "  |                                            \n"
                            "  | debug : boolean = 0                        \n"
                            "  |                                            \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("title")
      .set_terse_description("The title of the dump")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("The title of the display.                      \n"
                            "Default value is empty.                        \n"
                            "Example :                                      \n"
                            "  |                                            \n"
                            "  | title : string = \"The event record\"      \n"
                            "  |                                            \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("indent")
      .set_terse_description("The indent string of the dump")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("The indent string.                             \n"
                            "Default value is empty.                        \n"
                            "Example :                                      \n"
                            "  |                                            \n"
                            "  | indent : string = \"DEBUG: \"              \n"
                            "  |                                            \n"
                           )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("output")
      .set_terse_description("The output stream of the dump")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("A string that indicates the output stream       \n"
                            "where to print the structure of the data record.\n"
                            "Possible values are :                           \n"
                            "  * \"clog\" : print to 'std::clog'             \n"
                            "  * \"cout\" : print to 'std::cout'             \n"
                            "  * \"file\" : print to a file                  \n"
                            "Example :                                       \n"
                            "  |                                             \n"
                            "  | output : string = \"clog\"                  \n"
                            "  |                                             \n"
                           )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("output.filename")
      .set_terse_description("The name of the output file")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_mandatory(true)
      .set_triggered_by_label("output", "file")
      .set_long_description("The name of the file where to print the structure              \n"
                            "of the data record.                                            \n"
                            "The filename may contain an environment variable.              \n"
                            "Example :                                                      \n"
                            "  |                                                            \n"
                            "  | output          : string         = \"file\"                \n"
                            "  | output.filename : string as path = \"${LOG_DIR}/data.log\" \n"
                            "  |                                                            \n"
                           )
      ;
  }

  ocd_.set_configuration_hints ("A 'dpp::dump_module' object can be setup with the following syntax    \n"
                                "in a 'datatools::multi_properties' configuration file, typically from \n"
                                "a module manager object.                                              \n"
                                "Examples :                                                            \n"
                                "  |                                                                   \n"
                                "  | #@key_label   \"name\"                                            \n"
                                "  | #@meta_label  \"type\"                                            \n"
                                "  |                                                                   \n"
                                "  | [name=\"dump_file\" type=\"dpp::dump_module\"]                    \n"
                                "  | #@config A dump module                                            \n"
                                "  | output          : string         = \"file\"                       \n"
                                "  | output.filename : string as path = \"${LOG_DIR}/data.log\"        \n"
                                "  |                                                                   \n"
                                "  | [name=\"dump_stdout\" type=\"dpp::dump_module\"]                  \n"
                                "  | #@config A dump module                                            \n"
                                "  | output          : string         = \"cout\"                       \n"
                                "  |                                                                   \n"
                                );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::dpp::dump_module,"dpp::dump_module")



/****************************************************************/
// OCD support for class '::dpp::chain_module' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::dpp::chain_module,ocd_)
{
  ocd_.set_class_name ("dpp::chain_module");
  ocd_.set_class_library ("dpp");
  ocd_.set_class_description ("A module that chains several data processing modules");
  ocd_.set_class_documentation ("The chain module organizes the data processing along  \n"
                                "an ordered queue of data processing modules.          \n"
                                );

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("debug")
      .set_terse_description("Flag to activate debugging output")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("This flag activates debugging output.          \n"
                            "It is not recommended for a production run.    \n"
                            "Default value is false.                        \n"
                            "Example :                                      \n"
                            "  |                                            \n"
                            "  | debug : boolean = 0                        \n"
                            "  |                                            \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("modules")
      .set_terse_description("The list of names of the modules to be chained")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_long_description("The modules are searched by name from an external    \n"
                            "dictionnary, typically from a module manager object. \n"
                            "Example :                                      \n"
                            "  |                                            \n"
                            "  | modules : string[3] = \\                   \n"
                            "  |   \"read_from_file\"    \\                 \n"
                            "  |   \"calibration\"       \\                 \n"
                            "  |   \"write_in_file\"                        \n"
                            "  |                                            \n"
                            )
      ;
  }

  ocd_.set_configuration_hints ("A 'dpp::chain_module' object can be setup with the following syntax   \n"
                                "in a 'datatools::multi_properties' configuration file, typically from \n"
                                "a module manager object.                                              \n"
                                "Examples :                                                            \n"
                                "  |                                                                   \n"
                                "  | #@key_label   \"name\"                                            \n"
                                "  | #@meta_label  \"type\"                                            \n"
                                "  |                                                                   \n"
                                "  | [name=\"my_pipeline\" type=\"dpp::chain_module\"]                 \n"
                                "  | #@config A full data processing chain                             \n"
                                "  | debug : boolean = 0                                               \n"
                                "  | modules : string[5] = \\                                          \n"
                                "  |   \"read_from_file\"    \\                                        \n"
                                "  |   \"calibration\"       \\                                        \n"
                                "  |   \"reconstruction\"    \\                                        \n"
                                "  |   \"analysis\"          \\                                        \n"
                                "  |   \"write_in_file\"                                               \n"
                                "  |                                                                   \n"
                                );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::dpp::chain_module,"dpp::chain_module")



/****************************************************************/
// OCD support for class '::dpp::if_module' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::dpp::if_module,ocd_)
{
  ocd_.set_class_name ("dpp::if_module");
  ocd_.set_class_library ("dpp");
  ocd_.set_class_description ("A module that conditionnaly process data processing modules");

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("debug")
      .set_terse_description("The debug flag")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("The debug flag.                                \n"
                            "Default value is false.                        \n"
                            "Example :                                      \n"
                            "  |                                            \n"
                            "  | debug : boolean = 0                        \n"
                            "  |                                            \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("cut_service.label")
      .set_terse_description("The name of the cut service")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("The name of the cut service is searched from an external  \n"
                            "dictionnary, typically addressed through a service manager\n"
                            "handled by a module manager object.                       \n"
                            "Default value is \"Cuts\".                     \n"
                            "Example :                                      \n"
                            "  |                                            \n"
                            "  | cut_service.label : string = \"Cuts\"      \n"
                            "  |                                            \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("condition_cut")
      .set_terse_description("The name of the cut used as a condition")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("The name of the cut is searched from an external  \n"
                            "dictionnary, typically addressed through the cut  \n"
                            "service handled by a module manager object.       \n"
                            "Example :                                         \n"
                            "  |                                               \n"
                            "  | condition_cut : string = \"Energy>=3MeV\"     \n"
                            "  |                                               \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("then_status")
      .set_terse_description("The strategy used when the condition is fulfilled")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("The 'then_status' property takes its value in :   \n"
                            " * \"continue\" : the data record processing      \n"
                            "                  continues along the pipeline.   \n"
                            " * \"stop\"     : the data record is stopped.     \n"
                            "Example :                                         \n"
                            "  |                                               \n"
                            "  | then_status : string = \"continue\"           \n"
                            "  |                                               \n"
                            "This property is not compatible with the 'then_module'\n"
                            "property.                                             \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("else_status")
      .set_terse_description("The strategy used when the condition is not fulfilled")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("The 'else_status' property takes its value in :   \n"
                            " * \"continue\" : the data record processing      \n"
                            "                  continues along the pipeline.   \n"
                            " * \"stop\"     : the data record is stopped.     \n"
                            "Example :                                         \n"
                            "  |                                               \n"
                            "  | else_status : string = \"continue\"           \n"
                            "  |                                               \n"
                            "This property is not compatible with the 'else_module'\n"
                            "property.                                             \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("then_module")
      .set_terse_description("The name of the module used when the condition is fulfilled")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("The name of the module is searched from an external  \n"
                            "dictionnary, typically addressed through the module  \n"
                            "manager object.                                      \n"
                            "Example :                                            \n"
                            "  |                                                  \n"
                            "  | then_module : string = \"calibration\"           \n"
                            "  |                                                  \n"
                            "This property is not compatible with the 'then_status'\n"
                            "property.                                             \n"
                           )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("else_module")
      .set_terse_description("The name of the module used when the condition is not fulfilled")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("The name of the module is searched from an external  \n"
                            "dictionnary, typically addressed through the module  \n"
                            "manager object.                                      \n"
                            "Example :                                            \n"
                            "  |                                                  \n"
                            "  | else_module : string = \"calibration\"           \n"
                            "  |                                                  \n"
                            "This property is not compatible with the 'else_status'\n"
                            "property.                                             \n"
                           )
      ;
  }

  ocd_.set_configuration_hints ("A 'dpp::if_module' object can be setup with the following syntax   \n"
                                "in a 'datatools::multi_properties' configuration file, typically from \n"
                                "a module manager object.                                              \n"
                                "Examples :                                                            \n"
                                "  |                                                                   \n"
                                "  | #@key_label   \"name\"                                            \n"
                                "  | #@meta_label  \"type\"                                            \n"
                                "  |                                                                   \n"
                                "  | [name=\"high_energy_processing\" type=\"dpp::if_module\"]         \n"
                                "  | #@config A full data processing chain                             \n"
                                "  | debug : boolean = 0                                               \n"
                                "  | cut_service.label : string = \"Cuts\"                             \n"
                                "  | condition_cut     : string = \"HasHighEnergy\"                    \n"
                                "  | then_module       : string = \"process_he_event\"                 \n"
                                "  | else_status       : string = \"stop\"                             \n"
                                "  |                                                                   \n"
                                "  | [name=\"fork_track_length_processing\" type=\"dpp::if_module\"]   \n"
                                "  | #@config A full data processing chain                             \n"
                                "  | debug : boolean = 0                                               \n"
                                "  | cut_service.label : string = \"Cuts\"                             \n"
                                "  | condition_cut     : string = \"HasLongTrack\"                     \n"
                                "  | then_module       : string = \"process_long_track\"               \n"
                                "  | else_module       : string = \"process_short_track\"              \n"
                                "  |                                                                   \n"
                                );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::dpp::if_module,"dpp::if_module")




/****************************************************************/
// OCD support for class '::dpp::io_module' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::dpp::io_module,ocd_)
{
  ocd_.set_class_name ("dpp::io_module");
  ocd_.set_class_library ("dpp");
  ocd_.set_class_description ("An input/ouput module able to load/store data record in/from files");


  ocd_.set_configuration_hints ("A 'dpp::io_module' object can be setup with the following syntax      \n"
                                "in a 'datatools::multi_properties' configuration file, typically from \n"
                                "a module manager object.                                              \n"
                                "The input/output files setup directives use the 'datatools::smart_file_name'\n"
                                "class. See OCD support for the 'datatools::smart_filename' class.           \n"
                                "Examples :                                                            \n"
                                "  |                                                                   \n"
                                "  | #@key_label   \"name\"                                            \n"
                                "  | #@meta_label  \"type\"                                            \n"
                                "  |                                                                   \n"
                                "  | [name=\"load_data\" type=\"dpp::io_module\"]                      \n"
                                "  | #@config An input data processing module                          \n"
                                "  | debug                       : boolean = 0                         \n"
                                "  | mode                        : string = \"input\"                  \n"
                                "  | input.preserve_existing     : boolean = 1                         \n"
                                "  | input.max_record_per_file   : integer = 10000                     \n"
                                "  | input.max_files             : integer = 10                        \n"
                                "  | input.max_record_total      : integer = 100000                    \n"
                                "  | input.mode                  : string = \"incremental\"            \n"
                                "  | input.incremental.path      : string = \"${RAW_DATA_DIR}\"        \n"
                                "  | input.incremental.prefix    : string = \"run_\"                   \n"
                                "  | input.incremental.extension : string = \".data.gz\"               \n"
                                "  | input.incremental.start     : integer = 0                         \n"
                                "  | input.incremental.increment : integer = 1                         \n"
                                "  | input.incremental.stop      : integer = 10                        \n"
                                "  |                                                                   \n"
                                "  | [name=\"store_data\" type=\"dpp::io_module\"]                     \n"
                                "  | #@config An output data processing module                         \n"
                                "  | debug                        : boolean = 0                        \n"
                                "  | mode                         : string = \"output\"                \n"
                                "  | output.preserve_existing     : boolean = 1                        \n"
                                "  | output.max_record_per_file   : integer = 20000                    \n"
                                "  | output.max_files             : integer = 5                        \n"
                                "  | output.max_record_total      : integer = 100000                   \n"
                                "  | output.mode                  : string = \"incremental\"           \n"
                                "  | output.incremental.path      : string = \"${REC_DATA_DIR}\"       \n"
                                "  | output.incremental.prefix    : string = \"recons_\"               \n"
                                "  | output.incremental.extension : string = \".data.gz\"              \n"
                                "  | output.incremental.start     : integer = 0                        \n"
                                "  | output.incremental.increment : integer = 1                        \n"
                                "  | output.incremental.stop      : integer = 5                        \n"
                                "  |                                                                   \n"
                                );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::dpp::io_module,"dpp::io_module")


// OCD support for class '::dpp::utils_module' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::dpp::utils_module,ocd_)
{
  ocd_.set_class_name ("dpp::utils_module");
  ocd_.set_class_library ("dpp");
  ocd_.set_class_description ("A generic module that performs basic operations on the data");
  ocd_.set_class_documentation ("This module is capable of limited operations the data record.\n"
                                "Typicaly, it is possible to remove some data bank(s), add some \n"
                                "arbitrary property in bank of type 'datatools::properties'.  \n"
                                );


  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("debug")
      .set_terse_description("Flag to activate debugging output")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("This flag activates debugging output.             \n"
                            "It is not recommended for a production run.       \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("mode")
      .set_terse_description("The running mode")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("This module can perform one special action chosen from \n"
                            "the following list :                                   \n"
                            " 'clear'  : remove all data banks from the data record \n"
                            " 'remove_one_typed_bank'  :                            \n"
                            "            remove a bank given its name and optionnaly\n"
                            "            its type                                   \n"
                            " 'remove_banks' :                                      \n"
                            "            remove a list of banks given their names   \n"
                            " 'add_property' :                                      \n"
                            "            radd a kay/valu property in a bank         \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("mode.remove_one_typed_bank.label")
      .set_terse_description("The name of the bank to be removed")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_triggered_by_label("mode", "remove_one_typed_bank")
      .set_long_description("This property specifies the name of the data bank to be\n"
                            "removed from the data record while using the           \n"
                            "'remove_one_typed_bank' mode.                          \n"
                            "Example to remove a bank named 'CD' (calibration data):\n"
                            " |                                                     \n"
                            " | mode : string = \"remove_one_typed_bank\"           \n"
                            " | mode.remove_one_typed_bank.label : string = \"CD\"  \n"
                            " |                                                     \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("mode.remove_one_typed_bank.type")
      .set_terse_description("The type of the bank to be removed")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "remove_one_typed_bank")
      .set_long_description("This property specifies the type of the data bank to be \n"
                            "removed from the data record while using the            \n"
                            "'remove_one_typed_bank' mode.                           \n"
                            "As each data bank stored in a 'datatools::things' data  \n"
                            "record implement the serializable interface, we une here\n"
                            "the data bank's class' serial tag character string to   \n"
                            "identify the type of the bank object.                   \n"
                            "This property is optional.                              \n"
                            "Example to remove a bank named 'CD' of type 'foo::calibration_data' :\n"
                            " |                                                     \n"
                            " | mode : string = \"remove_one_typed_bank\"           \n"
                            " | mode.remove_one_typed_bank.label : string = \"CD\"  \n"
                            " | mode.remove_one_typed_bank.type : string = \\       \n"
                            " |   \"foo::calibration_data\"                         \n"
                            " |                                                     \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("mode.remove_banks.labels")
      .set_terse_description("The labels of the banks to be removed")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(true)
      .set_triggered_by_label("mode", "remove_banks")
      .set_long_description("This property specifies the array of names of the data  \n"
                            "bansk to be removed from the data record while using the\n"
                            "'remove_banks' mode.                                    \n"
                            "Example to remove two banks named 'RD' and 'CD' :       \n"
                            " |                                                      \n"
                            " | mode : string = \"remove_banks\"                     \n"
                            " | mode.remove_banks.labels : string[2] = \\            \n"
                            " |   \"RD\" \\                                          \n"
                            " |   \"CD\"                                             \n"
                            " |                                                      \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("mode.add_property.bank_label")
      .set_terse_description("The label of the bank where to add a property")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_triggered_by_label("mode", "add_property")
      .set_long_description("This property specifies the name of the data  \n"
                            "bank to be enriched with one arbitrary property.       \n"
                            "Example :                                               \n"
                            " |                                                      \n"
                            " | mode : string = \"add_property\"                     \n"
                            " | mode.add_property.bank_label : string = \"Header\"   \n"
                            " |                                                      \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("mode.add_property.key")
      .set_terse_description("The key of the property to be added")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_triggered_by_label("mode", "add_property")
      .set_long_description("This property specifies the name/key of the property    \n"
                            "to be added within the bank.                            \n"
                            "Example :                                               \n"
                            " |                                                      \n"
                            " | mode : string = \"add_property\"                     \n"
                            " | mode.add_property.bank_label : string = \"Header\"   \n"
                            " | mode.add_property.key        : string = \"my_name\"  \n"
                            " |                                                      \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("mode.add_property.value")
      .set_terse_description("The value to be added to be added")
      .set_traits(datatools::TYPE_BOOLEAN|datatools::TYPE_INTEGER|datatools::TYPE_REAL|datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_triggered_by_label("mode", "add_property")
      .set_long_description("This property specifies the property value to be added  \n"
                            "in the data bank. It must be one of the 4 basic types:  \n"
                            " - boolean                                              \n"
                            " - integer                                              \n"
                            " - real                                                 \n"
                            " - string                                               \n"
                            "It may be a scalar or an array.                         \n"
                            "Examples :                                              \n"
                            " |                                                      \n"
                            " | mode : string = \"add_property\"                     \n"
                            " | mode.add_property.bank_label : string = \"Header\"   \n"
                            " | mode.add_property.key   : string  = \"number\"       \n"
                            " | mode.add_property.value : integer = 4                \n"
                            " |                                                      \n"
                            "or :                                                    \n"
                            " | mode : string = \"add_property\"                     \n"
                            " | mode.add_property.bank_label : string = \"Header\"   \n"
                            " | mode.add_property.key   : string    = \"tokens\"     \n"
                            " | mode.add_property.value : string[2] = \"foo\" \"bar\"\n"
                            " |                                                      \n"
                            )
      ;
  }




  //ocd_.set_configuration_hints ("Nothing special.");
  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(dpp::utils_module,"dpp::utils_module")

// end of ocd_support.cc

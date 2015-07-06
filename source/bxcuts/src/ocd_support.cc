// ocd_support.cc

// Ourselves:
#include <cuts/ocd_support.h>

/****************************************************************/
// OCD support for class '::cuts::accept_cut' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::cuts::accept_cut,ocd_)
{
  ocd_.set_class_name ("cuts::accept_cut");
  ocd_.set_class_description ("A cut that always accepts");
  ocd_.set_class_library ("cuts");
  ocd_.set_class_documentation ("Whatever user data is transmitted to ``cuts::accept_cut`` cut object, \n"
                                "it is always accepted.                                              \n"
                                "This cut class has no specific configuration parameters.            \n"
                                );

  cuts::i_cut::common_ocd(ocd_);

  ocd_.set_configuration_hints ("A ``cuts::accept_cut`` object can be setup with the following syntax\n"
                                "in a ``datatools::multi_properties`` configuration file :           \n"
                                "\nExample::                                                         \n"
                                "                                                                \n"
                                "    #@key_label   \"name\"                                      \n"
                                "    #@meta_label  \"type\"                                      \n"
                                "                                                                \n"
                                "    [name=\"yes\" type=\"cuts::accept_cut\"]                    \n"
                                "    #@config A cut that always accepts                          \n"
                                "                                                                \n"
                                );
  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::cuts::accept_cut,"cuts::accept_cut")

/****************************************************************/
// OCD support for class '::cuts::reject_cut' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::cuts::reject_cut,ocd_)
{
  ocd_.set_class_name ("cuts::reject_cut");
  ocd_.set_class_description ("A cut that always rejects");
  ocd_.set_class_library ("cuts");
  ocd_.set_class_documentation ("Whatever user data is transmitted to ``cuts::reject_cut`` cut object, \n"
                                "it is always rejected.                                                \n"
                                "This cut class has no specific configuration parameters.              \n"
                                );

  cuts::i_cut::common_ocd(ocd_);

  ocd_.set_configuration_hints ("A ``cuts::reject_cut`` object can be setup with the following syntax\n"
                                "in a ``datatools::multi_properties`` configuration file :           \n"
                                "\nExample::                                                         \n"
                                "                                                                \n"
                                "    #@key_label   \"name\"                                      \n"
                                "    #@meta_label  \"type\"                                      \n"
                                "                                                                \n"
                                "    [name=\"no\" type=\"cuts::reject_cut\"]                     \n"
                                "    #@config A cut that always rejects                          \n"
                                "                                                                \n"
                                );
  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::cuts::reject_cut,"cuts::reject_cut")


/****************************************************************/
// OCD support for class '::cuts::random_cut' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::cuts::random_cut,ocd_)
{
  ocd_.set_class_name ("cuts::random_cut");
  ocd_.set_class_description ("A cut that randomly accepts using some given probability");
  ocd_.set_class_library ("cuts");
  ocd_.set_class_documentation ("Whatever user data is transmitted to ``cuts::random_cut`` cut object,  \n"
                                "it is randomly accepted with a given probability.                      \n"
                                "This cut may be provided a pseudo-random event number generator (PRNG) \n"
                                "and given seed.                                                        \n"
                                );

  cuts::i_cut::common_ocd(ocd_);

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("seed")
      .set_terse_description("The seed of the internal PRNG")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(false)
      .set_default_value_integer(0)
      .set_long_description("The seed used to initialize the internal PRNG. \n"
                            "It must be a positive integer.                 \n"
                            )
      .add_example("Set an arbitrary seed: ::              \n"
                   "                                       \n"
                   "  seed : integer = 314159              \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("accept_probability")
      .set_terse_description("The acceptance probability")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_default_value_real(0.5)
      .set_long_description("The acceptance probability used to accept the user data. \n"
                            "It must be a real between 0 and 1.         \n"
                            )
      .add_example("Set an arbitrary 13 % acceptance: ::     \n"
                   "                                         \n"
                   "  accept_probability : real = 0.13       \n"
                   "                                         \n"
                   )
      ;
  }

  ocd_.set_configuration_hints ("A ``cuts::random_cut`` object can be setup with the following syntax \n"
                                "in a ``datatools::multi_properties`` configuration file :            \n"
                                "\nExample::                                                          \n"
                                "                                                                     \n"
                                "    #@key_label   \"name\"                                           \n"
                                "    #@meta_label  \"type\"                                           \n"
                                "                                                                     \n"
                                "    [name=\"random_0.17_percent\" type=\"cuts::random_cut\"]         \n"
                                "    #@config A cut that randomly accept a data with 17 % probablility\n"
                                "    #@description Acceptance probability                             \n"
                                "    seed               : integer = 314159                            \n"
                                "    #@description Acceptance probability                             \n"
                                "    accept_probability : real    = 0.13                              \n"
                                "                                                                     \n"
                                );
  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::cuts::random_cut,"cuts::random_cut")


/****************************************************************/
// OCD support for class '::cuts::not_cut' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::cuts::not_cut,ocd_)
{
  ocd_.set_class_name ("cuts::not_cut");
  ocd_.set_class_description ("A negation cut");
  ocd_.set_class_library ("cuts");
  ocd_.set_class_documentation ("Whatever user data is transmitted to ``cuts::not_cut`` cut object, \n"
                                "the cut always negates the result of the cut it references.        \n"
                                "This cut must be given another cut of which the result is negated. \n"
                                );

  cuts::i_cut::common_ocd(ocd_);

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("cut")
      .set_terse_description("The name of a cut to be negated")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("The name of a cut to be negated is extracted from    \n"
                            "an external dictionary of predefined cuts. This is   \n"
                            "typically done from within a cut manager (see the    \n"
                            "``cuts::cut_manager`` class), used as a repository of\n"
                            "many cut objects.                                    \n"
                            )
      .add_example("Negates a range cut on hit multiplicity: ::  \n"
                   "                                             \n"
                   " cut : string = \"nhits_from_3_to_5\"        \n"
                   "                                             \n"
                   )

      ;
  }

  ocd_.set_configuration_hints ("A ``cuts::not_cut`` object can be setup with the following syntax      \n"
                                "in a ``datatools::multi_properties`` configuration file :              \n"
                                "                                                                     \n"
                                "Example of a NOT cut than negates the selection of a random cut::    \n"
                                "                                                                     \n"
                                "    #@key_label   \"name\"                                           \n"
                                "    #@meta_label  \"type\"                                           \n"
                                "                                                                     \n"
                                "    [name=\"random_17_percent\" type=\"cuts::random_cut\"]           \n"
                                "    #@config A random cut with 17% acceptance                        \n"
                                "    seed               : integer = 314159                            \n"
                                "    accept_probability : real    = 0.17                              \n"
                                "                                                                     \n"
                                "    [name=\"random_83_percent\" type=\"cuts::not_cut\"]              \n"
                                "    #@config A cut that negates the acceptance of the cut above      \n"
                                "    #@description  The name of the cut to be negated                 \n"
                                "    cut : string = \"random_17_percent\"                             \n"
                                "                                                                     \n"
                                "This is equivalent to a cut that randomly selects data with          \n"
                                "acceptance probability of 83%.                                       \n"
                                "                                                                     \n"
                                "Example of a NOT cut that selects data by negating another           \n"
                                "cut that selects itself data with energy greater than or equal       \n"
                                "to 3 MeV::                                                           \n"
                                "                                                                     \n"
                                "    #@key_label   \"name\"                                           \n"
                                "    #@meta_label  \"type\"                                           \n"
                                "                                                                     \n"
                                "    [name=\"energy>=3MeV\" type=\"my::energy_cut\"]                  \n"
                                "    energy_min  : real as energy = 3.0 MeV                           \n"
                                "                                                                     \n"
                                "    [name=\"energy<3MeV\" type=\"cuts::not_cut\"]                    \n"
                                "    cut : string = \"energy>=3MeV\"                                  \n"
                                "                                                                     \n"
                                "This is equivalent to a cut that selects data with energy            \n"
                                "lower than 3 MeV .                                                   \n"
                                "Here, the ``my::energy_cut`` class is supposed to be provided        \n"
                                "and registered by some user library.                                 \n"
                                );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::cuts::not_cut,"cuts::not_cut")



/****************************************************************/
// OCD support for class '::cuts::and_cut' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::cuts::and_cut,ocd_)
{
  ocd_.set_class_name ("cuts::and_cut");
  ocd_.set_class_description ("A logical AND cut combining two cuts");
  ocd_.set_class_library ("cuts");
  ocd_.set_class_documentation ("This cut perform a logical AND of two other cuts objects.  \n"
                                "This cut must be given two other cuts of which the result  \n"
                                "is combined.                                               \n"
                                );

  cuts::i_cut::common_ocd(ocd_);

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("cut_1")
      .set_terse_description("The name of the first cut")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("The name of the first cut to be combined with a second cut   \n"
                            "is extracted from an external dictionary of predefined cuts. \n"
                            "This is typically done within a cut manager.                 \n"
                            )
      .add_example("Request the first cut to select events with  \n"
                   "more than 2 calorimeter hits: ::             \n"
                   "                                             \n"
                   " cut_1 : string = \"ncalohits>2\"            \n"
                   "                                             \n"
                   "where the ``\"ncalohits>2\"`` cut object is  \n"
                   "available from the external dictionary.      \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("cut_2")
      .set_terse_description("The name of the second cut")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("The name of the second cut to be combined with the first cut \n"
                            "is extracted from an external dictionary of predefined cuts. \n"
                            "This is typically done within a cut manager.                 \n"
                            )
      .add_example("Request the second cut to select events with \n"
                   "more than 10 tracker hits: ::                \n"
                   "                                             \n"
                   " cut_2 : string = \"ntrackerhits>10\"        \n"
                   "                                             \n"
                   "where the ``\"ntrackerhits>10\"`` cut object is  \n"
                   "available from the external dictionary.      \n"
                   )
       ;
  }

  ocd_.set_configuration_hints ("A ``cuts::and_cut`` object can be setup with the following syntax    \n"
                                "in a ``datatools::multi_properties`` configuration file :            \n"
                                "\nExample::                                                          \n"
                                "                                                                     \n"
                                "    #@key_label   \"name\"                                           \n"
                                "    #@meta_label  \"type\"                                           \n"
                                "                                                                     \n"
                                "    [name=\"in_energy_roi\" type=\"my::energy_cut\"]                 \n"
                                "    energy_min  : real as energy = 2.6 MeV                           \n"
                                "    energy_max  : real as energy = 3.1 MeV                           \n"
                                "                                                                     \n"
                                "    [name=\"ntracks==2\" type=\"my::track_cut\"]                     \n"
                                "    ntracks_min : integer = 2                                        \n"
                                "    ntracks_max : integer = 2                                        \n"
                                "                                                                     \n"
                                "    [name=\"golden_event\" type=\"cuts::and_cut\"]                   \n"
                                "    cut_1       : string = \"in_energy_roi\"                         \n"
                                "    cut_2       : string = \"ntracks==2\"                            \n"
                                "                                                                     \n"
                                "Here, the ``my::energy_cut`` and ``my::track_cut`` classes are supposed  \n"
                                "to be provided and registered by some user library.                  \n"
                                );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::cuts::and_cut,"cuts::and_cut")




/****************************************************************/
// OCD support for class '::cuts::or_cut' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::cuts::or_cut,ocd_)
{
  ocd_.set_class_name ("cuts::or_cut");
  ocd_.set_class_description ("A logical OR cut combining two cuts");
  ocd_.set_class_library ("cuts");
  ocd_.set_class_documentation ("This cut perform a logical OR of two other cuts objects.   \n"
                                "This cut must be given two other cuts of which the result  \n"
                                "is combined.                                               \n"
                                );

  cuts::i_cut::common_ocd(ocd_);

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("cut_1")
      .set_terse_description("The name of the first cut")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("The name of the first cut to be combined with a second cut    \n"
                            "is extracted from an external dictionnary of predefined cuts. \n"
                            "This is typically done within a cut manager.                  \n"
                            )
      .add_example("Request the first cut to select events with  \n"
                   "more than 2 calorimeter hits: ::             \n"
                   "                                             \n"
                   " cut_1 : string = \"ncalohits>2\"            \n"
                   "                                             \n"
                   "where the ``\"ncalohits>2\"`` cut object is  \n"
                   "available from the external dictionary.      \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("cut_2")
      .set_terse_description("The name of the second cut")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("The name of the second cut to be combined with the first cut  \n"
                            "is extracted from an external dictionnary of predefined cuts. \n"
                            "This is typically done within a cut manager.                  \n"
                            )
      .add_example("Request the second cut to select events with \n"
                   "more than 1 muon veto hit: ::                \n"
                   "                                             \n"
                   " cut_2 : string = \"nmuveto>1\"              \n"
                   "                                             \n"
                   "where the ``\"nmuveto>1\"`` cut object is    \n"
                   "available from the external dictionary.      \n"
                   )
      ;
  }

  ocd_.set_configuration_hints ("A ``cuts::or_cut`` object can be setup with the following syntax     \n"
                                "in a ``datatools::multi_properties`` configuration file :            \n"
                                "\nExample::                                                          \n"
                                "                                                                     \n"
                                "    #@key_label   \"name\"                                           \n"
                                "    #@meta_label  \"type\"                                           \n"
                                "                                                                     \n"
                                "    [name=\"lower_energy\" type=\"my::energy_cut\"]                  \n"
                                "    energy_min  : real as energy = 0.1 MeV                           \n"
                                "    energy_max  : real as energy = 0.5 MeV                           \n"
                                "                                                                     \n"
                                "    [name=\"high_energy\" type=\"my::energy_cut\"]                   \n"
                                "    energy_min  : real as energy = 4.5 MeV                           \n"
                                "                                                                     \n"
                                "    [name=\"extreme_event\" type=\"cuts::or_cut\"]                   \n"
                                "    cut_1       : string = \"lower_energy\"                          \n"
                                "    cut_2       : string = \"high_energy\"                           \n"
                                "                                                                     \n"
                                "Here, the ``my::energy_cut`` class is supposed to be provided and    \n"
                                "registered by some user library.                                     \n"
                                );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::cuts::or_cut,"cuts::or_cut")



/****************************************************************/
// OCD support for class '::cuts::xor_cut' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::cuts::xor_cut,ocd_)
{
  ocd_.set_class_name ("cuts::xor_cut");
  ocd_.set_class_description ("A logical XOR cut combining two cuts");
  ocd_.set_class_library ("cuts");
  ocd_.set_class_documentation ("This cut perform a logical XOR of two other cuts objects.  \n"
                                "This cut must be given two other cuts of which the result  \n"
                                "is combined.                                               \n"
                                );

  cuts::i_cut::common_ocd(ocd_);

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("cut_1")
      .set_terse_description("The name of the first cut")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("The name of the first cut to be combined with a second cut    \n"
                            "is extracted from an external dictionnary of predefined cuts. \n"
                            "This is typically done within a cut manager.                  \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("cut_2")
      .set_terse_description("The name of the second cut")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("The name of the second cut to be combined with the first cut  \n"
                            "is extracted from an external dictionnary of predefined cuts. \n"
                            "This is typically done within a cut manager.                  \n"
                            )
      ;
  }

  ocd_.set_configuration_hints ("A ``cuts::xor_cut`` object can be setup with the following syntax    \n"
                                "in a ``datatools::multi_properties`` configuration file :            \n"
                                "\nExample::                                                          \n"
                                "                                                                     \n"
                                "    #@key_label   \"name\"                                           \n"
                                "    #@meta_label  \"type\"                                           \n"
                                "                                                                     \n"
                                "    [name=\"in_energy_roi\" type=\"my::energy_cut\"]                 \n"
                                "    energy_min  : real as energy = 2.6 MeV                           \n"
                                "    energy_max  : real as energy = 3.1 MeV                           \n"
                                "                                                                     \n"
                                "    [name=\"out_energy_roi\" type=\"cuts::not_cut\"]                 \n"
                                "    cut         : string = \"in_energy_roi\"                         \n"
                                "                                                                     \n"
                                "    [name=\"ntracks==2\" type=\"my::track_cut\"]                     \n"
                                "    ntracks_min : integer = 2                                        \n"
                                "    ntracks_max : integer = 2                                        \n"
                                "                                                                     \n"
                                "    [name=\"ntracks!=2\" type=\"cuts::not_cut\"]                     \n"
                                "    cut         : string = \"ntracks==2\"                            \n"
                                "                                                                     \n"
                                "    [name=\"cross_event\" type=\"cuts::xor_cut\"]                    \n"
                                "    cut_1       : string = \"out_energy_roi\"                        \n"
                                "    cut_2       : string = \"ntracks!=2\"                            \n"
                                "                                                                     \n"
                                "Here, the ``my::energy_cut`` and ``my::track_cut`` classes are       \n"
                                "supposed to be provided and registered by some user library.         \n"
                                );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::cuts::xor_cut,"cuts::xor_cut")



/****************************************************************/
// OCD support for class '::cuts::nand_cut' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::cuts::nand_cut,ocd_)
{
  ocd_.set_class_name ("cuts::nand_cut");
  ocd_.set_class_description ("A logical NAND cut combining two cuts");
  ocd_.set_class_library ("cuts");
  ocd_.set_class_documentation ("This cut perform a logical NAND of two other cuts objects. \n"
                                "This cut must be given two other cuts of which the result  \n"
                                "is combined.                                               \n"
                                );

  cuts::i_cut::common_ocd(ocd_);

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("cut_1")
      .set_terse_description("The name of the first cut")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("The name of the first cut to be combined with a second cut    \n"
                            "is extracted from an external dictionary of predefined cuts.  \n"
                            "This is typically done within a cut manager.                  \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("cut_2")
      .set_terse_description("The name of the second cut")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("The name of the second cut to be combined with the first cut  \n"
                            "is extracted from an external dictionary of predefined cuts.  \n"
                            "This is typically done in a cut manager.                      \n"
                            )
      ;
  }

  ocd_.set_configuration_hints ("A ``cuts::nand_cut`` object can be setup with the following syntax   \n"
                                "in a ``datatools::multi_properties`` configuration file :            \n"
                                "\nExample::                                                          \n"
                                "                                                                     \n"
                                "    [name=\"in_energy_roi\" type=\"my::energy_cut\"]                 \n"
                                "    energy_min  : real as energy = 2.6 MeV                           \n"
                                "    energy_max  : real as energy = 3.1 MeV                           \n"
                                "                                                                     \n"
                                "    [name=\"ntracks==2\" type=\"my::track_cut\"]                     \n"
                                "    ntracks_min : integer = 2                                        \n"
                                "    ntracks_max : integer = 2                                        \n"
                                "                                                                     \n"
                                "    [name=\"bad_event\" type=\"cuts::nand_cut\"]                     \n"
                                "    cut_1       : string = \"in_energy_roi\"                         \n"
                                "    cut_2       : string = \"ntracks==2\"                            \n"
                                "                                                                     \n"
                                "Here, the ``my::energy_cut`` nand ``my::track_cut`` classes are      \n"
                                "supposed to be provided and registered by some user library.         \n"
                                );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::cuts::nand_cut,"cuts::nand_cut")



/****************************************************************/
// OCD support for class ``::cuts::nor_cut`` :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::cuts::nor_cut,ocd_)
{
  ocd_.set_class_name ("cuts::nor_cut");
  ocd_.set_class_description ("A logical NOR cut combining two cuts");
  ocd_.set_class_library ("cuts");
  ocd_.set_class_documentation ("This cut perform a logical NOR of two other cuts objects.  \n"
                                "This cut must be given two other cuts of which the result  \n"
                                "is combined.                                               \n"
                                );

  cuts::i_cut::common_ocd(ocd_);

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("cut_1")
      .set_terse_description("The name of the first cut")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("The name of the first cut to be combined with a second cut    \n"
                            "is extracted from an external dictionnary of predefined cuts. \n"
                            "This is typically done within a cut manager.                  \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("cut_2")
      .set_terse_description("The name of the second cut")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("The name of the second cut to be combined with the first cut  \n"
                            "is extracted from an external dictionnary of predefined cuts. \n"
                            "This is typically done within a cut manager.                  \n"
                            )
      ;
  }

  ocd_.set_configuration_hints ("A ``cuts::nor_cut`` object can be setup with the following syntax    \n"
                                "in a ``datatools::multi_properties`` configuration file :            \n"
                                "\nExample::                                                          \n"
                                "                                                                     \n"
                                "    #@key_label   \"name\"                                           \n"
                                "    #@meta_label  \"type\"                                           \n"
                                "                                                                     \n"
                                "    [name=\"in_energy_roi\" type=\"my::energy_cut\"]                 \n"
                                "    energy_min  : real as energy = 2.6 MeV                           \n"
                                "    energy_max  : real as energy = 3.1 MeV                           \n"
                                "                                                                     \n"
                                "    [name=\"ntracks==2\" type=\"my::track_cut\"]                     \n"
                                "    ntracks_min : integer = 2                                        \n"
                                "    ntracks_max : integer = 2                                        \n"
                                "                                                                     \n"
                                "    [name=\"very_bad_event\" type=\"cuts::nor_cut\"]                 \n"
                                "    cut_1       : string = \"in_energy_roi\"                         \n"
                                "    cut_2       : string = \"ntracks==2\"                            \n"
                                "                                                                     \n"
                                "Here, the ``my::energy_cut`` nor ``my::track_cut`` classes are       \n"
                                "supposed to be provided and registered by some user library.         \n"
                                );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::cuts::nor_cut,"cuts::nor_cut")




/****************************************************************/
// OCD support for class ``::cuts::xnor_cut`` :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::cuts::xnor_cut,ocd_)
{
  ocd_.set_class_name ("cuts::xnor_cut");
  ocd_.set_class_description ("A logical XNOR cut combining two cuts");
  ocd_.set_class_library ("cuts");
  ocd_.set_class_documentation ("This cut perform a logical XNOR of two other cuts objects. \n"
                                "This cut must be given two other cuts of which the result  \n"
                                "is combined.                                               \n"
                                );

  cuts::i_cut::common_ocd(ocd_);

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("cut_1")
      .set_terse_description("The name of the first cut")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("The name of the first cut to be combined with a second cut    \n"
                            "is extracted from an external dictionnary of predefined cuts. \n"
                            "This is typically done within a cut manager.                  \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("cut_2")
      .set_terse_description("The name of the second cut")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("The name of the second cut to be combined with the first cut  \n"
                            "is extracted from an external dictionnary of predefined cuts. \n"
                            "This is typically done within a cut manager.                  \n"
                            )
      ;
  }

  ocd_.set_configuration_hints ("A ``cuts::xnor_cut`` object can be setup with the following syntax   \n"
                                "in a ``datatools::multi_properties`` configuration file :            \n"
                                "\nExample::                                                          \n"
                                "                                                                     \n"
                                "    #@key_label   \"name\"                                           \n"
                                "    #@meta_label  \"type\"                                           \n"
                                "                                                                     \n"
                                "    [name=\"in_energy_roi\" type=\"my::energy_cut\"]                 \n"
                                "    energy_min  : real as energy = 2.6 MeV                           \n"
                                "    energy_max  : real as energy = 3.1 MeV                           \n"
                                "                                                                     \n"
                                "    [name=\"ntracks==2\" type=\"my::track_cut\"]                     \n"
                                "    ntracks_min : integer = 2                                        \n"
                                "    ntracks_max : integer = 2                                        \n"
                                "                                                                     \n"
                                "    [name=\"very_bad_or_golden_event\" type=\"cuts::xnor_cut\"]      \n"
                                "    cut_1       : string = \"in_energy_roi\"                         \n"
                                "    cut_2       : string = \"ntracks==2\"                            \n"
                                "                                                                     \n"
                                "Here, the ``my::energy_cut`` xnor ``my::track_cut`` classes are      \n"
                                "supposed  to be provided and registered by some user library.        \n"
                                );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::cuts::xnor_cut,"cuts::xnor_cut")



/****************************************************************/
// OCD support for class ``::cuts::multi_and_cut`` :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::cuts::multi_and_cut,ocd_)
{
  ocd_.set_class_name ("cuts::multi_and_cut");
  ocd_.set_class_description ("A logical AND cut combining several cuts");
  ocd_.set_class_library ("cuts");
  ocd_.set_class_documentation ("This cut perform a logical AND of an ordered collection    \n"
                                "of other cuts objects.                                     \n"
                                "This cut must be given the list of cuts of which the result\n"
                                "is combined.                                               \n"
                                );

  cuts::i_cut::common_ocd(ocd_);

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("cuts")
      .set_terse_description("The list of name of the cuts to be combined")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(true)
      .set_long_description("The list of all cuts' names to be combined with a logical AND.\n"
                            "Each cut's name is extracted from an external dictionary of   \n"
                            "predefined cuts. This is typically done within a cut manager. \n"
                            "The list of cuts must contains at least one cut name.         \n"
                            )
      .add_example("Combine 3 cuts: ::                                            \n"
                   "                                                              \n"
                   "    cuts : string[3] = \"in_energy_roi\" \"2e\" \"no_alpha\"  \n"
                   "                                                              \n"
                   )
      ;
  }

  ocd_.set_configuration_hints ("A ``cuts::multi_and_cut`` object can be setup with the following     \n"
                                "syntax in a ``datatools::multi_properties`` configuration file :     \n"
                                "\nExample::                                                          \n"
                                "                                                                     \n"
                                "    #@key_label   \"name\"                                           \n"
                                "    #@meta_label  \"type\"                                           \n"
                                "                                                                     \n"
                                "    [name=\"in_energy_roi\" type=\"my::energy_cut\"]                 \n"
                                "    energy_min  : real as energy = 2.6 MeV                           \n"
                                "    energy_max  : real as energy = 3.1 MeV                           \n"
                                "                                                                     \n"
                                "    [name=\"2e\" type=\"my::track_cut\"]                             \n"
                                "    ntracks_min : integer = 2                                        \n"
                                "    ntracks_max : integer = 2                                        \n"
                                "                                                                     \n"
                                "    [name=\"no_alpha\" type=\"my::track_cut\"]                       \n"
                                "    ndelayedtracks_max : integer = 0                                 \n"
                                "                                                                     \n"
                                "    [name=\"0vbb_event\" type=\"cuts::multi_and_cut\"]               \n"
                                "    cuts : string[3] =    \\                                         \n"
                                "        \"in_energy_roi\"   \\                                       \n"
                                "        \"2e\"              \\                                       \n"
                                "        \"no_alpha\"                                                 \n"
                                "                                                                     \n"
                                "Here, the ``my::energy_cut`` and ``my::track_cut`` classes are       \n"
                                "supposed to be provided and registered by some user library.         \n"
                                );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::cuts::multi_and_cut,"cuts::multi_and_cut")





/****************************************************************/
// OCD support for class ``::cuts::multi_or_cut`` :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::cuts::multi_or_cut,ocd_)
{
  ocd_.set_class_name ("cuts::multi_or_cut");
  ocd_.set_class_description ("A logical OR cut combining several cuts");
  ocd_.set_class_library ("cuts");
  ocd_.set_class_documentation ("This cut perform a logical OR of an ordered collection     \n"
                                "of other cuts objects.                                     \n"
                                "This cut must be given the list of cuts of which the result\n"
                                "is combined.                                               \n"
                                );

  cuts::i_cut::common_ocd(ocd_);

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("cuts")
      .set_terse_description("The list of name of the cuts to be combined")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(true)
      .set_long_description("The list of all cuts' names to be combined with a logical OR. \n"
                            "Each cut's name is extracted from an external dictionary of   \n"
                            "predefined cuts. This is typically done within a cut manager. \n"
                            "The list of cuts must contains at least one cut name.         \n"
                            )
      .add_example("Combine 3 cuts: ::                                            \n"
                   "                                                              \n"
                   "    cuts : string[3] = \"1e1g\" \"2e1g\" \"1e2g\"             \n"
                   "                                                              \n"
                   )
      ;
  }

  ocd_.set_configuration_hints ("A ``cuts::multi_or_cut`` object can be setup with the following      \n"
                                " syntax in a ``datatools::multi_properties`` configuration file :    \n"
                                "\nExample::                                                          \n"
                                "                                                                     \n"
                                "    #@key_label   \"name\"                                           \n"
                                "    #@meta_label  \"type\"                                           \n"
                                "                                                                     \n"
                                "    [name=\"1e1g\" type=\"my::topology_cut\"]                        \n"
                                "    number_of_electrons : integer = 1                                \n"
                                "    number_of_gammas    : integer = 1                                \n"
                                "    number_of_alphas    : integer = 0                                \n"
                                "                                                                     \n"
                                "    [name=\"1e2g\" type=\"my::topology_cut\"]                        \n"
                                "    number_of_electrons : integer = 1                                \n"
                                "    number_of_gammas    : integer = 2                                \n"
                                "    number_of_alphas    : integer = 0                                \n"
                                "                                                                     \n"
                                "    [name=\"2e1g\" type=\"my::topology_cut\"]                        \n"
                                "    number_of_electrons : integer = 2                                \n"
                                "    number_of_gammas    : integer = 1                                \n"
                                "    number_of_alphas    : integer = 0                                \n"
                                "                                                                     \n"
                                "    [name=\"1e1g1a\" type=\"my::topology_cut\"]                      \n"
                                "    number_of_electrons : integer = 1                                \n"
                                "    number_of_gammas    : integer = 1                                \n"
                                "    number_of_alphas    : integer = 1                                \n"
                                "                                                                     \n"
                                "    [name=\"background_event\" type=\"cuts::multi_or_cut\"]          \n"
                                "    cuts : string[4] =  \\                                           \n"
                                "        \"1e1g\"          \\                                         \n"
                                "        \"1e2g\"          \\                                         \n"
                                "        \"2e1g\"          \\                                         \n"
                                "        \"1e1g1a\"                                                   \n"
                                "                                                                     \n"
                                "Here, the ``my::topology_cut`` class is supposed to be provided      \n"
                                "and registered by some user library.                                 \n"
                                );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::cuts::multi_or_cut,"cuts::multi_or_cut")





/****************************************************************/
// OCD support for class '::cuts::multi_xor_cut' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::cuts::multi_xor_cut,ocd_)
{
  ocd_.set_class_name ("cuts::multi_xor_cut");
  ocd_.set_class_description ("A logical XOR cut combining several cuts");
  ocd_.set_class_library ("cuts");
  ocd_.set_class_documentation ("This cut perform a logical XOR of an ordered collection    \n"
                                "of other cuts objects.                                     \n"
                                "This cut must be given the list of cuts of which the result\n"
                                "is combined.                                               \n"
                                );

  cuts::i_cut::common_ocd(ocd_);

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("cuts")
      .set_terse_description("The list of name of the cuts to be combined")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(true)
      .set_long_description("The list of all cuts' names to be combined with a logical XOR.\n"
                            "Each cut's name is extracted from an external dictionary of   \n"
                            "predefined cuts. This is typically done in a cut manager.     \n"
                            "The list of cuts must contains at least one cut name.         \n"
                            )
      .add_example("\nCombine 3 cuts: ::                                          \n"
                   "                                                              \n"
                   "    cuts : string[3] = \"1e1g\" \"2e1g\" \"1e2g\"             \n"
                   "                                                              \n"
                   )
      ;
  }

  ocd_.set_configuration_hints ("A ``cuts::multi_xor_cut`` object can be setup with the following     \n"
                                "syntax in a ``datatools::multi_properties`` configuration file :     \n"
                                "\nExample::                                                          \n"
                                "                                                                     \n"
                                "    #@key_label   \"name\"                                           \n"
                                "    #@meta_label  \"type\"                                           \n"
                                "                                                                     \n"
                                "    [name=\"tall\" type=\"my::people_cut\"]                          \n"
                                "    size_min   : real as length = 190 cm                             \n"
                                "                                                                     \n"
                                "    [name=\"fat\" type=\"my::people_cut\"]                           \n"
                                "    weight_min : real as weight = 140 kg                             \n"
                                "                                                                     \n"
                                "    [name=\"bb_staff\" type=\"my::people_cut\"]                      \n"
                                "    job        : string = \"nemo_group\"                             \n"
                                "                                                                     \n"
                                "    [name=\"only_one_minority_trait\" type=\"cuts::multi_xor_cut\"]  \n"
                                "    cuts : string[4] =  \\                                           \n"
                                "        \"tall\"          \\                                         \n"
                                "        \"fat\"           \\                                         \n"
                                "        \"bb_staff\"      \\                                         \n"
                                "                                                                     \n"
                                "Here, the ``my::people_cut`` class is supposed to be provided        \n"
                                "and registered by some user library.                                 \n"
                                );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::cuts::multi_xor_cut,"cuts::multi_xor_cut")

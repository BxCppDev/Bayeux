// combined_vg.cc

// Ourselves:
#include <genvtx/combined_vg.h>

// Standard library:
#include <limits>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/ioutils.h>
#include <datatools/units.h>
#include <datatools/exception.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>

// This project:
#include <genvtx/manager.h>

namespace genvtx {

  GENVTX_VG_REGISTRATION_IMPLEMENT(combined_vg, "genvtx::combined_vg")

  combined_vg::entry_type::entry_type ()
  {
    weight = 0.0;
    cumulated_weight = 0.0;
    vg_handle.reset ();
    return;
  }

  combined_vg::combined_vg() : genvtx::i_vertex_generator()
  {
    _initialized_ = false;
    _set_defaults_ ();
    return;
  }

  bool combined_vg::is_initialized () const
  {
    return _initialized_;
  }

  combined_vg::~combined_vg()
  {
    if (is_initialized ()) reset ();
    return;
  }

  void combined_vg::_set_defaults_ ()
  {
    _entries_.clear ();
    this->i_vertex_generator::_reset ();
    return;
  }

  void combined_vg::_reset_ ()
  {
    _set_defaults_ ();
    return;
  }

  void combined_vg::reset ()
  {
    DT_THROW_IF (! is_initialized (), std::logic_error, "Vertex generator '" << get_name() << "' is not initialized !");
    _reset_ ();
    _initialized_ = false;
    return;
  }

  void combined_vg::_shoot_vertex(::mygsl::rng & random_,
                                  ::geomtools::vector_3d & vertex_)
  {
    DT_THROW_IF (! is_initialized (), std::logic_error, "Vertex generator '" << get_name() << "' is not initialized !");
    geomtools::invalidate (vertex_);
    this->_shoot_vertex_combined (random_, vertex_);
    return;
  }

  void combined_vg::_shoot_vertex_combined (mygsl::rng & random_,
                                            geomtools::vector_3d & vertex_)
  {
    double ran_w = random_.uniform ();
    int index = -1;
    for (size_t i = 0; i < _entries_.size (); i++) {
      if (ran_w <= _entries_[i].cumulated_weight) {
        index = i;
        break;
      }
    }
    DT_THROW_IF (index < 0,std::logic_error,
                 "Cannot determine vertex location index in combined vertex generator '" << get_name() << "' !");
    genvtx::i_vertex_generator & a_vg = _entries_[index].vg_handle.grab ();
    DT_THROW_IF (! a_vg.has_next_vertex (),
                 std::logic_error,
                 "Vertex generator '" << a_vg.get_name() << "' has no more available vertex !");
    a_vg.shoot_vertex (random_, vertex_);
    return;
  }


  void combined_vg::add_generator (genvtx::vg_handle_type & a_vg,
                                   const std::string & a_name,
                                   double a_weight)
  {
    DT_THROW_IF (! a_vg,std::logic_error,
                 "Cannot add a NULL vertex generator handle in combined vertex generator '" << get_name() << "' !");
    for (size_t i = 0; i < _entries_.size (); ++i) {
      const entry_type & a_entry = _entries_[i];
      DT_THROW_IF (&a_entry.vg_handle.get () == &a_vg.get(),
                   std::logic_error,
                   "Cannot add twice a generator in combined vertex generator '" << get_name() << "' !");
    }
    entry_type dummy;
    _entries_.push_back (dummy);
    entry_type & the_entry = _entries_.back ();
    the_entry.name = a_name;
    the_entry.weight = a_weight;
    the_entry.cumulated_weight = 0.0;
    the_entry.vg_handle = a_vg;
    return;
  }


  void combined_vg::_init_ ()
  {
    DT_THROW_IF (_entries_.size () == 0,
                 std::logic_error,
                 "Missing weighted vertex generators in combined vertex generator '" << get_name() << "' !");

    // compute cumulated weights :
    for (size_t i = 0; i < _entries_.size (); i++) {
      DT_LOG_TRACE (get_logging_priority(), "Entry #" << i << " is '" << _entries_[i].name << "' ");
      double cumul = 0.0;
      if (i > 0) {
        cumul = _entries_[i - 1].cumulated_weight;
      }
      _entries_[i].cumulated_weight = cumul + _entries_[i].weight;
      DT_LOG_TRACE (get_logging_priority(), "  with cumulated weight = " << _entries_[i].cumulated_weight << "' ");
    }

    // store the total weight before normalization for alternative use :
    double the_weight_value = _entries_.back ().cumulated_weight;
    int the_weight_type = weight_info::WEIGHTING_NONE;
    weight_info the_weight_info;
    the_weight_info.set_type(the_weight_type);
    the_weight_info.set_value(the_weight_value);
    _set_total_weight (the_weight_info);
    DT_LOG_TRACE (get_logging_priority(), "Total weight = "<< the_weight_value << "' ");

    // normalize weight:
    for (size_t i = 0; i < _entries_.size (); i++) {
      _entries_[i].cumulated_weight /= _entries_.back ().cumulated_weight;
      DT_LOG_TRACE (get_logging_priority(), "Cumulated weight for generator '"
                    << _entries_[i].name << "' " << " = " << _entries_[i].cumulated_weight);
    }
    return;
  }

  void combined_vg::initialize (const ::datatools::properties & setup_,
                                ::datatools::service_manager & service_manager_,
                                ::genvtx::vg_dict_type & vgens_)
  {
    using namespace std;
    DT_THROW_IF (is_initialized (), std::logic_error, "Already initialized !");
    this->::genvtx::i_vertex_generator::_initialize_basics(setup_, service_manager_);
    this->::genvtx::i_vertex_generator::_initialize_geo_manager(setup_,service_manager_);
    // Parsing configuration parameters :
    DT_THROW_IF (! setup_.has_key ("generators"),
                 std::logic_error,
                 "Missing the 'generators' directive in combined vertex generator '" << get_name() << "' !");
    // extract information for combined generators :
    vector<string> generator_names;
    vector<double> generator_relative_weights;
    vector<double> generator_absolute_weights;
    vector<double> generator_activity_value;
    vector<string> generator_activity_label;
    bool activity_mode = false;
    setup_.fetch ("generators", generator_names);
    for (vector<string>::const_iterator i = generator_names.begin ();
         i != generator_names.end ();
         i++) {
      DT_LOG_TRACE (get_logging_priority(), "VG name = '" << *i << "'");
      const std::string & vg_name = *i;
      double activity_value = -1.0;
      std::string activity_unit_label = "";
      double relative_weight = -1.0;
      double absolute_weight = -1.0;

      // First check if we use the 'activity' mode :
      {
        std::ostringstream key_oss;
        key_oss << "generators." << vg_name << ".activity";
        if (setup_.has_key (key_oss.str ())) {
          std::string activity_str
            = setup_.fetch_string (key_oss.str ());
          DT_LOG_TRACE (get_logging_priority(), "Found activity = '" << activity_str << "' for generator '" << vg_name << "'");
          DT_THROW_IF ((! datatools::units::find_value_with_unit (activity_str,
                                                                  activity_value,
                                                                  activity_unit_label)),
                       std::logic_error,
                       "Cannot parse a value/unit directive from '" << activity_str << "' in combined vertex generator '" << get_name() << "' !");
          DT_THROW_IF ((activity_unit_label != "activity")
                       && (activity_unit_label != "volume_activity")
                       && (activity_unit_label != "surface_activity")
                       && (activity_unit_label != "mass_activity"),
                       std::logic_error,
                       "Invalid activity directive '" << activity_unit_label << "' in combined vertex generator '" << get_name() << "' !");
          activity_mode = true;
          generator_activity_value.push_back (activity_value);
          generator_activity_label.push_back (activity_unit_label);
        }
      }

      // If one doesn't use 'activity' mode, try to fetch 'weighting infos' :
      if (! activity_mode ) {
        DT_LOG_TRACE (get_logging_priority(), "One does not use activity mode...");

        // relative weight :
        {
          std::ostringstream key_oss;
          key_oss << "generators." << vg_name << ".relative_weight";
          if (setup_.has_key (key_oss.str ())) {
            DT_THROW_IF (activity_mode,
                         std::logic_error,
                         "Incompatible activity/relative_weight directive in combined vertex generator '" << get_name() << "' !");
            relative_weight = setup_.fetch_real (key_oss.str ());
            DT_THROW_IF (relative_weight < 0.0,
                         std::logic_error,
                         "Invalid negative relative weight directive in combined vertex generator '" << get_name() << "' !");
          }
        }

        // absolute weight :
        if (relative_weight < 0.0) {
          std::ostringstream key_oss;
          key_oss << "generators." << vg_name << ".absolute_weight";
          if (setup_.has_key (key_oss.str ())) {
            DT_THROW_IF (activity_mode, std::logic_error,
                         "Incompatible activity/absolute_weight directive in combined vertex generator '" << get_name() << "' !");
            DT_THROW_IF (relative_weight >= 0.0, std::logic_error,
                         "Incompatible relative/absolute weight directives in combined vertex generator '" << get_name() << "' !");
            absolute_weight = setup_.fetch_real (key_oss.str ());
            DT_THROW_IF (absolute_weight < 0.0, std::logic_error,
                         "Invalid negative absolute weight directive in combined vertex generator '" << get_name() << "' !");
          }
        }
        generator_relative_weights.push_back (relative_weight);
        generator_absolute_weights.push_back (absolute_weight);
      }
    }
    // End of parsing configuration parameters.

    int no_activity_type = weight_info::WEIGHTING_NONE;
    for (size_t i = 0; i < generator_names.size (); i++) {
      const std::string & the_vg_name = generator_names[i];
      DT_LOG_TRACE (get_logging_priority(), "i = " << i << " VG name = '" << the_vg_name << "'");
      genvtx::vg_dict_type::iterator found = vgens_.find (the_vg_name);
      DT_THROW_IF (found == vgens_.end (), std::logic_error,
                   "No vertex generator named '" << the_vg_name << "' in combined vertex generator '" << get_name() << "' !");
      double the_vg_weight = -1.0;
      genvtx::vg_handle_type vgh = found->second.grab_initialized_vg_handle ();

      // Activity mode :
      if (activity_mode) {
        DT_LOG_TRACE (get_logging_priority(), "Using 'activity' mode...");
        double the_vg_activity_value         = generator_activity_value[i];
        const std::string & the_vg_activity_label = generator_activity_label[i];
        if (the_vg_activity_label == "activity") {
          // This is the absolute activity mode :
          the_vg_weight = the_vg_activity_value;
          DT_LOG_TRACE (get_logging_priority(), "Using 'absolute activity' mode...");
        } else {
          const i_vertex_generator & test_vg = vgh.get();
          const weight_info & the_vg_weight_info = test_vg.get_total_weight ();
          if (get_logging_priority() >= datatools::logger::PRIO_TRACE) {
            DT_LOG_TRACE (get_logging_priority(), "Weight info for generator '" << the_vg_name << "' :");
            the_vg_weight_info.dump(std::cerr);
          }

          if (the_vg_activity_label == "volume_activity") {
            // This is the volume activity mode :
            DT_LOG_TRACE (get_logging_priority(), "Using 'volume activity' mode...");
            DT_THROW_IF (! the_vg_weight_info.has_volume (), std::logic_error,
                         "Vertex generator '" << the_vg_name << "' does not support 'volume_activity' mode in combined vertex generator '" << get_name() << "' !");
            DT_LOG_TRACE (get_logging_priority(), "Volume is " <<  the_vg_weight_info.get_volume () / CLHEP::cm3 << " cm3");
            the_vg_weight = the_vg_activity_value * the_vg_weight_info.get_volume ();
            double Bq_unit = 1. / CLHEP::second;
            DT_LOG_TRACE (get_logging_priority(), "Generator '" <<  the_vg_name << "' total activity is : "
                          << the_vg_weight / Bq_unit << " Bq (in volume)");
          } else if (the_vg_activity_label == "surface_activity") {
            // This is the surface activity mode :
            DT_LOG_TRACE (get_logging_priority(), "Using 'surface activity' mode...");
            DT_THROW_IF (! the_vg_weight_info.has_surface (),std::logic_error,
                         "Vertex generator '" << the_vg_name << "' does not support 'surface_activity' mode in combined vertex generator '" << get_name() << "' !");
            DT_LOG_TRACE (get_logging_priority(), "Surface is " <<  the_vg_weight_info.get_surface () / CLHEP::m2 << " m2");
            the_vg_weight = the_vg_activity_value * the_vg_weight_info.get_surface ();
            double Bq_unit = 1. / CLHEP::second;
            DT_LOG_TRACE (get_logging_priority(), "Generator '" <<  the_vg_name << "' total activity is : "
                          << the_vg_weight / Bq_unit << " Bq (in surface)");
          } else if (the_vg_activity_label == "mass_activity") {
            // This is the mass activity mode :
            DT_LOG_TRACE (get_logging_priority(), "Using 'mass activity' mode...");
            DT_THROW_IF (! the_vg_weight_info.has_mass (), std::logic_error,
                         "Vertex generator '" << the_vg_name << "' does not support 'mass_activity' mode ! Please check the access to some valid materials geometry plugin in combined vertex generator '" << get_name() << "' !");
            DT_LOG_TRACE (get_logging_priority(), "Mass is " <<  the_vg_weight_info.get_mass () / CLHEP::kg << " kg");
            the_vg_weight = the_vg_activity_value * the_vg_weight_info.get_mass ();
            double Bq_unit = 1. / CLHEP::second;
            DT_LOG_TRACE (get_logging_priority(), "Generator '" <<  the_vg_name << "' total activity is : "
                          << the_vg_weight / Bq_unit << " Bq (in mass)");
          } else {
            DT_THROW_IF(true,  std::logic_error,
                        "Invalid activity label '" << the_vg_activity_label
                        << "' for generator '" << the_vg_name << "' in combined vertex generator '" << get_name() << "' !");
          }
        }
      } else {
        // Plain mode :
        DT_LOG_TRACE (get_logging_priority(), "Using 'plain' mode...");
        double the_vg_absolute_weight = generator_absolute_weights[i];
        double the_vg_relative_weight = generator_relative_weights[i];
        DT_LOG_TRACE (get_logging_priority(), "absolute_weight = " << the_vg_absolute_weight);
        DT_LOG_TRACE (get_logging_priority(), "relative_weight = " << the_vg_relative_weight);
        if (the_vg_absolute_weight > 0.0) {
          DT_LOG_TRACE (get_logging_priority(), "Using 'absolute weight' mode...");
          the_vg_weight = the_vg_absolute_weight;
        } else if (the_vg_relative_weight > 0.0) {
          DT_LOG_TRACE (get_logging_priority(), "Using 'relative weight' mode...");
          const i_vertex_generator & test_vg = vgh.get();
          const weight_info & the_vg_weight_info = test_vg.get_total_weight ();
          DT_THROW_IF (the_vg_weight_info.get_type() == weight_info::WEIGHTING_NONE,
                       std::logic_error,
                       "Cannot support the '" << "relative_weight"
                       << "' mode for vertex generator '" << the_vg_name
                       << "' for it has no valid weight info type in combined vertex generator '" << get_name() << "' !");
          if (no_activity_type == weight_info::WEIGHTING_NONE) {
            no_activity_type = the_vg_weight_info.get_type();
          } else {
            DT_THROW_IF (the_vg_weight_info.get_type() != no_activity_type,
                         std::logic_error,
                         "Cannot use the '" << "relative_weight"
                         << "' mode for vertex generator '" << the_vg_name
                         << "' which is not compatible with the current mode in combined vertex generator '" << get_name() << "' !");
          }
          the_vg_weight = the_vg_relative_weight * the_vg_weight_info.get_value();
        } else {
          DT_THROW_IF(true, std::logic_error,
                      "Vertex generator '" << the_vg_name << "' has no weight information in combined vertex generator '" << get_name() << "' !");
        }
      }

      add_generator (vgh, the_vg_name, the_vg_weight);
    }
    _init_ ();
    _initialized_ = true;
    return;
  }

  void combined_vg::tree_dump (std::ostream & out_,
                               const std::string & title_,
                               const std::string & indent_,
                               bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    i_vertex_generator::tree_dump (out_, title_, indent_, true);

    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Entries        : " << _entries_.size () << std::endl;

    for (size_t i = 0; i < _entries_.size (); i++) {
      out_ << indent << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
      if (i == _entries_.size () - 1) {
        out_ << datatools::i_tree_dumpable::last_tag;
      } else {
        out_ << datatools::i_tree_dumpable::tag;
      }
      const entry_type & e = _entries_[i];
      out_ << e.name << ": ";
      out_ << " weight = " << e.weight << " (" << e.cumulated_weight << ")"
           << std::endl;
    }
    return;
  }

} // end of namespace genvtx

/***************
 * OCD support *
 ***************/

// OCD support' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::genvtx::combined_vg,ocd_)
{
  ocd_.set_class_name("genvtx::combined_vg");
  ocd_.set_class_description("A vertex generator that combines other weighted generators");
  ocd_.set_class_library("genvtx");
  ocd_.set_class_documentation("The combined vertex generator implements the combinaison   \n"
                               "of different vertex generators that each have a given      \n"
                               "weight, depending on their surface, volume, mass activity  \n"
                               "or arbitrary absolute activity.                            \n"
                               "This generator must be given the names of the generators   \n"
                               "to be combined and some rules to compute the weights       \n"
                               "associated to these generators.                            \n"
                               "The generators to be combined are handled by a dictionary  \n"
                               "of generator, typically managed by a ``genvtx::manager``   \n"
                               "object.                                                    \n"
                               );

  ::genvtx::i_vertex_generator::ocd_support(ocd_);

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("generators")
      .set_terse_description("The list of vertex generators' names to be combined")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(true)
      .set_long_description("This sets the names of the vertex generators that must be  \n"
                            "combined. Each name must be found in the dictionary of     \n"
                            "vertex generators handled by a manager object.             \n"
                            )
      .add_example("Combinaison of two generators: ::         \n"
                   "                                          \n"
                   " generators : string[2] = \\              \n"
                   "       \"roof_surface\" \\                \n"
                   "        \"floor_surface\"                 \n"
                   "                                          \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("generators.${generators}.activity")
      .set_terse_description("The activity values associated to each vertex generator to be combined")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_long_description("The rules to compute the weights of the vertex      \n"
                            "generators that must be combined.                   \n"
                            "Each generator is given an activity value expressed \n"
                            "in one of the following units:                      \n"
                            "                                                    \n"
                            " - absolute activity (example: Bq)                  \n"
                            " - surface activity (example: Bq/m2)                \n"
                            " - volume activity (example: uBq/m3)                \n"
                            " - mass activity (example: mBq/kg)                  \n"
                            "                                                    \n"
                            "The rule is parsed and the weight is automatically  \n"
                            "computed from available geometry and material       \n"
                            "data associated to the logical volumes associated   \n"
                            "to the vertex generators. If some of these informations\n"
                            "are missing, the initialization fails.              \n"
                            )
      .add_example("Combinaison of two generators by surface activities: ::      \n"
                   "                                          \n"
                   " generators : string[2] = \\              \n"
                   "       \"roof_surface\" \\                \n"
                   "       \"floor_surface\"                  \n"
                   " generators.roof_surface.activity  : string = \"3.2 mBq/m2\" \n"
                   " generators.floor_surface.activity : string = \"5.5 mBq/m2\" \n"
                   "                                          \n"
                   )
      .add_example("Combinaison of three generators by different activities: ::    \n"
                   "                                          \n"
                   " generators : string[3] = \\              \n"
                   "       \"roof_surface\"   \\              \n"
                   "       \"room_volume\"                    \n"
                   "       \"walls\"                          \n"
                   " generators.roof_surface.activity : string = \"3.2  mBq/m2\"  \n"
                   " generators.room_volume.activity  : string = \"97.8 uBq/m3\" \n"
                   " generators.walls.activity        : string = \"53.1 mBq\"    \n"
                   "                                                             \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("generators.${generators}.relative_weight")
      .set_terse_description("The relative weights associated to each vertex generator to be combined")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(false)
      .add_example("Combinaison of two generators by relative weights::  \n"
                   "                                                     \n"
                   "  generators : string[2] = \\                        \n"
                   "       \"east_wall\"   \\                            \n"
                   "       \"west_wall\"                                 \n"
                   "  generators.east_wall.relative_weight : real = 0.2  \n"
                   "  generators.west_wall.relative_weight : real = 0.8  \n"
                   "                                                     \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("generators.${generators}.absolute_weight")
      .set_terse_description("The absolute weights associated to each vertex generator to be combined")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(false)
      .add_example("Combinaison of two generators by absolute weights::  \n"
                   "                                                     \n"
                   "  generators : string[2] = \\                        \n"
                   "       \"east_wall\"   \\                            \n"
                   "       \"west_wall\"                                 \n"
                   "  generators.east_wall.absolute_weight : real = 0.2  \n"
                   "  generators.west_wall.absolute_weight : real = 0.8  \n"
                   "                                                     \n"
                   )
     ;
  }

  //ocd_.set_configuration_hints("Nothing special.");
  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()

DOCD_CLASS_SYSTEM_REGISTRATION(genvtx::combined_vg,"genvtx::combined_vg")

// -*- mode: c++ ; -*-
/* combined_vg.cc
 */

#include <genvtx/combined_vg.h>

#include <limits>
#include <stdexcept>

#include <datatools/ioutils.h>
#include <datatools/units.h>
#include <datatools/exception.h>

#include <mygsl/rng.h>

#include <genvtx/manager.h>

namespace genvtx {

  GENVTX_VG_REGISTRATION_IMPLEMENT(combined_vg,"genvtx::combined_vg");

  combined_vg::entry_type::entry_type ()
  {
    weight = 0.0;
    cumulated_weight = 0.0;
    vg_handle.reset ();
    return;
  }

  // Constructor :
  GENVTX_VG_CONSTRUCTOR_IMPLEMENT_HEAD(combined_vg)
  {
    _initialized_ = false;
    _set_defaults_ ();
    return;
  }

  GENVTX_VG_IS_INITIALIZED_IMPLEMENT_HEAD(combined_vg)
  {
    return _initialized_;
  }

  // Destructor :
  GENVTX_VG_DEFAULT_DESTRUCTOR_IMPLEMENT(combined_vg)

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

  GENVTX_VG_RESET_IMPLEMENT_HEAD(combined_vg)
  {
    DT_THROW_IF (! is_initialized (), std::logic_error, "Vertex generator '" << get_name() << "' is not initialized !");
    _reset_ ();
    _initialized_ = false;
    return;
  }

  GENVTX_VG_SHOOT_VERTEX_IMPLEMENT_HEAD(combined_vg,random_,vertex_)
  {
    DT_THROW_IF (! is_initialized (), std::logic_error, "Vertex generator '" << get_name() << "' is not initialized !");
    geomtools::invalidate (vertex_);
    this->_shoot_vertex_combined (random_, vertex_);
    return;
  }

  void combined_vg::_shoot_vertex_combined (mygsl::rng & random_,
                                            geomtools::vector_3d & vertex_)
  {
    bool devel = false;
    double ran_w = random_.uniform ();
    int index = -1;
    for (int i = 0; i < _entries_.size (); i++) {
      if (ran_w <= _entries_[i].cumulated_weight) {
        index = i;
        break;
      }
    }
    DT_THROW_IF (index < 0,std::logic_error,
                 "Cannot determine vertex location index in combined vertex generator '" << get_name() << "' !");
    genvtx::i_vertex_generator & a_vg = _entries_[index].vg_handle.grab ();
    // if (! a_vg.has_next_vertex ()) {
    //   // What to do here
    // }
    a_vg.shoot_vertex (random_, vertex_);
    return;
  }


  void combined_vg::add_generator (genvtx::vg_handle_type & a_vg,
                                   const std::string & a_name,
                                   double a_weight)
  {
    DT_THROW_IF (! a_vg,std::logic_error,
                 "Cannot add a NULL vertex generator handle in combined vertex generator '" << get_name() << "' !");
    for (int i = 0; i < _entries_.size (); ++i) {
      const entry_type & a_entry = _entries_[i];
      DT_THROW_IF (&a_entry.vg_handle.get () == &a_vg.get (),
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
    for (int i = 0; i < _entries_.size (); i++) {
      // if (devel) std::cerr << "DEVEL: combined_vg::_init_: Entry #" << i
      //                 << " is '" << _entries_[i].name << "' " << std::endl;
      double cumul = 0.0;
      if (i > 0) {
        cumul = _entries_[i - 1].cumulated_weight;
      }
      _entries_[i].cumulated_weight = cumul + _entries_[i].weight;
      // if (devel) std::cerr << "DEVEL: combined_vg::_init_:   with cumulated weight = "
      //                 << _entries_[i].cumulated_weight << "' " << std::endl;
    }

    // store the total weight before normalization for alternative use :
    double the_weight_value = _entries_.back ().cumulated_weight;
    int the_weight_type = weight_info::WEIGHTING_NONE;
    weight_info the_weight_info;
    the_weight_info.type = the_weight_type;
    the_weight_info.value = the_weight_value;
    _set_total_weight (the_weight_info);
    // if (devel) std::cerr << "DEVEL: combined_vg::_init_: Total weight = "
    //                 << the_weight_value << "' " << std::endl;

    // normalize weight:
    for (int i = 0; i < _entries_.size (); i++) {
      _entries_[i].cumulated_weight /= _entries_.back ().cumulated_weight;
      // if (is_debug ())
      //   {
      //     std::clog << datatools::io::debug
      //          << "genvtx::combined_vg::_init_: "
      //          << "Cumulated weight for generator '"
      //          << _entries_[i].name << "' "
      //          << " = " << _entries_[i].cumulated_weight << std::endl;
      //   }
    }
    return;
  }

  GENVTX_VG_INITIALIZE_IMPLEMENT_HEAD(combined_vg,setup_,service_manager_,vgens_)
  {
    using namespace std;
    DT_THROW_IF (is_initialized (), std::logic_error, "Already initialized !");
    GENVTX_VG_INITIALIZE_BASICS_INVOKE(setup_,service_manager_);
    GENVTX_VG_INITIALIZE_GEO_MANAGER_INVOKE(setup_,service_manager_);
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
      // if (devel)
      //   {
      //     std::cerr << "DEVEL: " << "genvtx::combined_vg::initialize: "
      //          << "VG name = '" << *i << "'" << std::endl;
      //   }
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

      // If one doesn't use 'activity' mode, try to fetch  'weighting infos' :
      if (! activity_mode ) {

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
    for (int i = 0; i < generator_names.size (); i++) {
      const std::string & the_vg_name = generator_names[i];
      // if (devel)
      //   {
      //     std::cerr << "DEVEL: " << "genvtx::combined_vg::initialize: "
      //          << "i = " << i << " VG name = '" << the_vg_name << "'"
      //          << std::endl;
      //   }
      genvtx::vg_dict_type::iterator found = vgens_.find (the_vg_name);
      DT_THROW_IF (found == vgens_.end (), std::logic_error,
                   "No vertex generator named '" << the_vg_name << "' in combined vertex generator '" << get_name() << "' !");
      double the_vg_weight = -1.0;
      genvtx::vg_handle_type vgh = found->second.grab_initialized_vg_handle ();

      // Activity mode :
      if (activity_mode) {
        // if (devel)
        //   {
        //     std::cerr << "DEVEL: " << "genvtx::combined_vg::initialize: "
        //          << "Using 'activity' mode..."
        //          << std::endl;
        //   }
        double the_vg_activity_value         = generator_activity_value[i];
        const std::string & the_vg_activity_label = generator_activity_label[i];
        if (the_vg_activity_label == "activity") {
          // This is the absolute activity mode :
          the_vg_weight = the_vg_activity_value;
          // if (devel)
          //   {
          //     std::cerr << "DEVEL: " << "genvtx::combined_vg::initialize: "
          //          << "Using 'absolute activity' mode..."
          //          << std::endl;
          //   }
        } else {
          i_vertex_generator & test_vg = vgh.grab();
          // Other activity modes are only supported by generators inherited
          // from the 'base_vg' mother class, so here we first check covariance :
          // base_vg * test_vg = dynamic_cast<base_vg *> (& vgh.get());
          // if (test_vg == 0)
          //   {
          //     std::ostringstream message;
          //     message << "genvtx::combined_vg::initialize: "
          //             << "No support for '" << the_vg_activity_label
          //             << "' mode for generator '" << the_vg_name << "' !";
          //     th row std::logic_error (message.str ());
          //   }
          // if (devel)
          //   {
          //     std::cerr << "DEVEL: " << "genvtx::combined_vg::initialize: "
          //          << "Generator '" << the_vg_name << "' is a 'base_vg'! "
          //          << std::endl;
          //   }
          const weight_info & the_vg_weight_info = test_vg.get_total_weight ();

          //the_vg_weight_info.dump(std::cerr);

          if (the_vg_activity_label == "volume_activity") {
            // This is the volume activity mode :
            // if (devel)
            //   {
            //     std::cerr << "DEVEL: " << "genvtx::combined_vg::initialize: "
            //          << "Using 'volume activity' mode..."
            //          << std::endl;
            //   }
            DT_THROW_IF (! the_vg_weight_info.has_volume (), std::logic_error,
                         "Vertex generator '" << the_vg_name << "' does not support 'volume_activity' mode in combined vertex generator '" << get_name() << "' !");
            // if (devel)
            //   {
            //     std::cerr << "DEVEL: " << "genvtx::combined_vg::initialize: "
            //          << "Volume is " <<  the_vg_weight_info.get_volume () / CLHEP::cm3
            //          << " cm3" << std::endl;
            //   }
            the_vg_weight = the_vg_activity_value * the_vg_weight_info.get_volume ();
            double Bq_unit = 1. / CLHEP::second;
            // if (devel)
            //   {
            //     std::cerr << "DEVEL: "
            //          << "genvtx::combined_vg::initialize: "
            //          << "Generator '" <<  the_vg_name << "' total activity is : "
            //          << the_vg_weight / Bq_unit << " Bq (in volume)" << std::endl;
            //   }
          } else if (the_vg_activity_label == "surface_activity") {
            // This is the surface activity mode :
            // if (devel)
            //   {
            //     std::cerr << "DEVEL: " << "genvtx::combined_vg::initialize: "
            //          << "Using 'surface activity' mode..."
            //          << std::endl;
            //   }
            DT_THROW_IF (! the_vg_weight_info.has_surface (),std::logic_error,
                         "Vertex generator '" << the_vg_name << "' does not support 'surface_activity' mode in combined vertex generator '" << get_name() << "' !");
            // if (devel)
            //   {
            //     std::cerr << "DEVEL: " << "genvtx::combined_vg::initialize: "
            //          << "Surface is " <<  the_vg_weight_info.get_surface () / CLHEP::m2
            //          << " m2" << std::endl;
            //   }
            the_vg_weight = the_vg_activity_value * the_vg_weight_info.get_surface ();
            double Bq_unit = 1. / CLHEP::second;
            // if (devel)
            //   {
            //     std::clog << "DEVEL: "
            //          << "genvtx::combined_vg::initialize: "
            //          << "Generator '" <<  the_vg_name << "' total activity is : "
            //          << the_vg_weight / Bq_unit << " Bq (in surface)" << std::endl;
            //   }
          } else if (the_vg_activity_label == "mass_activity") {
            // This is the mass activity mode :
            // if (devel)
            //   {
            //     std::cerr << "DEVEL: " << "genvtx::combined_vg::initialize: "
            //          << "Using 'mass activity' mode..."
            //          << std::endl;
            //   }
            DT_THROW_IF (! the_vg_weight_info.has_mass (), std::logic_error,
                         "Vertex generator '" << the_vg_name << "' does not support 'mass_activity' mode ! Please check the access to some valid materials geometry plugin in combined vertex generator '" << get_name() << "' !");
            // if (devel)
            //   {
            //     std::cerr << "DEVEL: " << "genvtx::combined_vg::initialize: "
            //          << "Mass is " <<  the_vg_weight_info.get_mass () / CLHEP::kg << " kg" << std::endl;
            //   }
            the_vg_weight = the_vg_activity_value * the_vg_weight_info.get_mass ();
            double Bq_unit = 1. / CLHEP::second;
            // if (devel)
            //   {
            //     std::cerr << "DEVEL: "
            //          << "genvtx::combined_vg::initialize: "
            //          << "Generator '" <<  the_vg_name << "' total activity is : "
            //          << the_vg_weight / Bq_unit << " Bq (in mass)" << std::endl;
            //   }
          } else {
            DT_THROW_IF(true,  std::logic_error,
                        "Invalid activity label '" << the_vg_activity_label
                        << "' for generator '" << the_vg_name << "' in combined vertex generator '" << get_name() << "' !");
          }
        }
      } else {
        // Plain mode :
        // if (devel)
        //   {
        //     std::cerr << "DEVEL: " << "genvtx::combined_vg::initialize: "
        //          << "Using 'plain' mode..."
        //          << std::endl;
        //   }
        double the_vg_absolute_weight = generator_absolute_weights[i];
        double the_vg_relative_weight = generator_relative_weights[i];
        // if (devel)
        //   {
        //     std::cerr << "DEVEL: " << "genvtx::combined_vg::initialize: "
        //          << "absolute_weight = " << the_vg_absolute_weight
        //          << std::endl;
        //     std::cerr << "DEVEL: " << "genvtx::combined_vg::initialize: "
        //          << "relative_weight = " << the_vg_relative_weight
        //          << std::endl;
        //   }
        if (the_vg_absolute_weight > 0.0) {
          // if (devel)
          //   {
          //     std::cerr << "DEVEL: " << "genvtx::combined_vg::initialize: "
          //          << "Using 'absolute weight' mode..."
          //          << std::endl;
          //   }
          the_vg_weight = the_vg_absolute_weight;
        } else if (the_vg_relative_weight > 0.0) {
          // if (devel)
          //   {
          //     std::cerr << "DEVEL: " << "genvtx::combined_vg::initialize: "
          //               << "Using 'relative weight' mode..."
          //               << std::endl;
          //   }
          i_vertex_generator & test_vg = vgh.grab();
          /*
            base_vg * test_vg = dynamic_cast<base_vg *> (& vgh.get());
            if (test_vg == 0)
            {
            std::ostringstream message;
            message << "genvtx::combined_vg::initialize: "
            << "No support for '" << "relative_weight"
            << "' mode for generator '" << the_vg_name << "' !";
            th row std::logic_error (message.str ());
            }
          */
          const weight_info & the_vg_weight_info = test_vg.get_total_weight ();
          DT_THROW_IF (the_vg_weight_info.type == weight_info::WEIGHTING_NONE,
                       std::logic_error,
                       "Cannot support the '" << "relative_weight"
                       << "' mode for vertex generator '" << the_vg_name
                       << "' for it has no valid weight info type in combined vertex generator '" << get_name() << "' !");
          if (no_activity_type == weight_info::WEIGHTING_NONE) {
            no_activity_type = the_vg_weight_info.type;
          } else {
            DT_THROW_IF (the_vg_weight_info.type != no_activity_type,
                         std::logic_error,
                         "Cannot use the '" << "relative_weight"
                         << "' mode for vertex generator '" << the_vg_name
                         << "' which is not compatible with the current mode in combined vertex generator '" << get_name() << "' !");
          }
          the_vg_weight = the_vg_relative_weight * the_vg_weight_info.value;
        } else {
          DT_THROW_IF(true,  std::logic_error,
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
    namespace du = datatools;
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    i_vertex_generator::tree_dump (out_, title_, indent_, true);

    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)
         << "Entries        : " << _entries_.size () << std::endl;

    for (int i = 0; i < _entries_.size (); i++) {
      out_ << indent << du::i_tree_dumpable::inherit_skip_tag (inherit_);
      if (i == _entries_.size () - 1) {
        out_ << du::i_tree_dumpable::last_tag;
      } else {
        out_ << du::i_tree_dumpable::tag;
      }
      const entry_type & e = _entries_[i];
      out_ << e.name << ": ";
      out_ << " weight = " << e.weight << " (" << e.cumulated_weight << ")"
           << std::endl;
    }
    return;
  }

} // end of namespace genvtx

// end of combined_vg.cc

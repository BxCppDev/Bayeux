/* \file genbb_inspector.cxx
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2013-04-20
 * Last modified : 2014-02-20
 *
 * Copyright (C) 2013-2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

// Standard libraries:
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <numeric>

// Third Party:
// - Boost:
#include <boost/program_options.hpp>
// - Camp:
#include <camp/camptype.hpp>
#include <camp/class.hpp>
#include <camp/enum.hpp>
#include <camp/value.hpp>
#include <camp/args.hpp>
#include <camp/userobject.hpp>

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif

#include <boost/spirit/include/qi.hpp>
#include <boost/scoped_ptr.hpp>

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/at_c.hpp>
// - Bayeux/datatools:
#include <datatools/datatools_config.h>
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/units.h>
#include <datatools/detail/reflection_macros.h>
#include <datatools/detail/reflection_export.h>
#include <datatools/library_loader.h>
#include <datatools/tracer.h>
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/kernel.h>
#include <datatools/io_factory.h>
#include <datatools/configuration/io.h>
#include <datatools/configuration/variant_service.h>
// - Bayeux/mygsl:
#include <mygsl/histogram_pool.h>
#include <mygsl/histogram_1d.h>
#include <mygsl/histogram_2d.h>
// - Bayeux/dpp:
#include <dpp/output_module.h>
#include <dpp/histogram_service.h>
// - Bayeux:
#include <bayeux/bayeux.h>

// This project:
#include <genbb_help/manager.h>
#include <genbb_help/primary_event.h>
// 2015-02-11, FM: Fix bug : add the .ipp header to use the BOOST_CLASS_VERSION macro
#include <genbb_help/primary_event.ipp>
#include <genbb_help/version.h>
#include <genbb_help/resource.h>

namespace dtc = datatools::configuration;

namespace genbb {

  /// \brief Configuration parameters for the inspector of GENBB primary event generator
  struct inspector_params
  {
    inspector_params();
    void reset();
    void dump(std::ostream & = std::clog) const;
    // Parameters:
    bool debug = false;                            //!< Debug flag
    bool no_splash = false;                        //!< No splash flag
    int trace_index;                               //!< Index associated to tracing messages
    bool interactive;                              //!< Interactive flag
    std::vector<std::string> unrecognized_options; //!< Unrecognized options
    std::string configuration;                     //!< Particle generator manager configuration file
    std::string action;                            //!< Action to be performed
    std::string list_print_mode;                   //!< Print list mode ("tree", "raw");
    std::string generator;                         //!< Particle generator name
    std::string output_mode;                       //!< Output mode
    std::string output_bank_label;                 //!< Bank label for 'bank' mode
    int         output_bank_max_records;           //!< Mas number of records per file for 'bank' mode
    std::vector<std::string> output_paths;         //!< List of output files
    int number_of_events;                          //!< Number of generated events
    int print_modulo;                              //!< Modulo print period for generated events
    int prng_seed;                                 //!< PRNG seed
    int prng_trunc;                                //!< PRNG hack
    std::string prng_tracker;                      //!< PRNG tracker output file
    double prompt_time_limit;                      //!< Prompt time limit (in ns)
    std::vector<std::string> histos_definitions;   //!< Histograms' definition files
    bool prompt;                                   //!< Flag to analyze prompt event
    bool delayed;                                  //!< Flag to analyze delayed event
    std::vector<std::string> histo_groups;         //!< Group of histograms
    std::string title_prefix;                      //!< User title prefix for histograms
    std::string name_suffix;                       //!< User name suffix for histograms
    dtc::variant_service::config variants;         //!< Variant support
  };

  void inspector_params::dump(std::ostream & out_) const
  {
    out_ << "inspector_params::dump: " << std::endl;
    out_ << "|-- debug       = " << debug << std::endl;
    out_ << "|-- trace_index = " << trace_index << std::endl;
    out_ << "|-- interactive = " << interactive << std::endl;
    out_ << "|-- unrecognized_options = " << unrecognized_options.size()
         << std::endl;
    out_ << "|-- configuration      = " << configuration << std::endl;
    out_ << "|-- action             = " << action << std::endl;
    out_ << "|-- list_print_mode    = " << list_print_mode << std::endl;
    out_ << "|-- generator          = " << generator << std::endl;
    out_ << "|-- output_mode        = " << output_mode << std::endl;
    out_ << "|-- output_bank_label  = " << output_bank_label << std::endl;
    out_ << "|-- output_bank_max_records  = " << output_bank_max_records << std::endl;
    out_ << "|-- output_paths       = " << output_paths.size() << std::endl;
    out_ << "|-- number_of_events   = " << number_of_events << std::endl;
    out_ << "|-- print_modulo       = " << print_modulo << std::endl;
    out_ << "|-- prng_seed          = " << prng_seed << std::endl;
    out_ << "|-- prng_trunc         = " << prng_trunc << std::endl;
    out_ << "|-- prng_tracker       = " << prng_tracker << std::endl;
    out_ << "|-- prompt_time_limit  = " << prompt_time_limit << std::endl;
    out_ << "|-- histos_definitions = " << histos_definitions.size() << std::endl;
    out_ << "|-- prompt             = " << prompt << std::endl;
    out_ << "|-- delayed            = " << delayed << std::endl;
    out_ << "|-- histo_groups       = " << histo_groups.size() << std::endl;
    out_ << "|-- title_prefix       = " << title_prefix << std::endl;
    out_ << "`-- name_suffix        = " << name_suffix << std::endl;
    return;
  }

  void inspector_params::reset()
  {
    debug = false;
    trace_index = 0;
    interactive = false;
    action.clear();
    list_print_mode.clear();
    unrecognized_options.clear();
    output_mode.clear();
    output_bank_label = "genbb.PE";
    output_bank_max_records = 100000;
    output_paths.clear();
    number_of_events = -1;
    print_modulo = 100;
    prng_seed = 0;
    prng_trunc = 0;
    prng_tracker.clear();
    prompt_time_limit = 1.0;
    histos_definitions.clear();
    prompt = true;
    delayed = false;
    histo_groups.clear();
    title_prefix.clear();
    name_suffix.clear();
    return;
  }

  inspector_params::inspector_params()
  {
    reset();
    return;
  }

  struct inspector_data
  {
  public:
    inspector_data();
    // Required by use of CAMP RTTI
    virtual ~inspector_data() = default;
    void reset();
    void sort();

    double get_electron_energy(int i_) const;
    double get_positron_energy(int i_) const;
    double get_gamma_energy(int i_) const;
    double get_alpha_energy(int i_) const;
    double get_neutron_energy(int i_) const;
    double get_proton_energy(int i_) const;
    double get_ion_energy(int i_) const;
    double get_muon_minus_energy(int i_) const;
    double get_muon_plus_energy(int i_) const;
    double get_other_neutral_particle_energy(int i_) const;
    double get_other_positive_particle_energy(int i_) const;
    double get_other_negative_particle_energy(int i_) const;

    double get_electrons_energy_sum() const;
    double get_positrons_energy_sum() const;
    double get_gammas_energy_sum() const;
    double get_alphas_energy_sum() const;
    double get_neutrons_energy_sum() const;
    double get_protons_energy_sum() const;
    double get_ions_energy_sum() const;
    double get_muons_minus_energy_sum() const;
    double get_muons_plus_energy_sum() const;
    double get_other_neutral_particles_energy_sum() const;
    double get_other_positive_particles_energy_sum() const;
    double get_other_negative_particles_energy_sum() const;

    double get_electron_time(int i_) const;
    double get_positron_time(int i_) const;
    double get_gamma_time(int i_) const;
    double get_alpha_time(int i_) const;
    double get_neutron_time(int i_) const;
    double get_proton_time(int i_) const;
    double get_ion_time(int i_) const;
    double get_muon_minus_time(int i_) const;
    double get_muon_plus_time(int i_) const;
    double get_other_neutral_particle_time(int i_) const;
    double get_other_positive_particle_time(int i_) const;
    double get_other_negative_particle_time(int i_) const;

    double get_electron_phi(int i_) const;
    double get_positron_phi(int i_) const;
    double get_gamma_phi(int i_) const;
    double get_alpha_phi(int i_) const;
    double get_neutron_phi(int i_) const;
    double get_proton_phi(int i_) const;
    double get_ion_phi(int i_) const;
    double get_muon_minus_phi(int i_) const;
    double get_muon_plus_phi(int i_) const;
    double get_other_neutral_particle_phi(int i_) const;
    double get_other_positive_particle_phi(int i_) const;
    double get_other_negative_particle_phi(int i_) const;

    double get_electron_cos_theta(int i_) const;
    double get_positron_cos_theta(int i_) const;
    double get_gamma_cos_theta(int i_) const;
    double get_alpha_cos_theta(int i_) const;
    double get_neutron_cos_theta(int i_) const;
    double get_proton_cos_theta(int i_) const;
    double get_ion_cos_theta(int i_) const;
    double get_muon_minus_cos_theta(int i_) const;
    double get_muon_plus_cos_theta(int i_) const;
    double get_other_neutral_particle_cos_theta(int i_) const;
    double get_other_positive_particle_cos_theta(int i_) const;
    double get_other_negative_particle_cos_theta(int i_) const;

    double get_cos_theta(const std::string & type1_, int i1_,
                         const std::string & type2_, int i2_) const;

  public:

    int    number_of_particles;
    int    number_of_charged_particles;
    double energy_sum;
    double charged_particles_energy_sum;

    int number_of_electrons;
    int number_of_positrons;
    int number_of_gammas;
    int number_of_alphas;
    int number_of_neutrons;
    int number_of_protons;
    int number_of_ions;
    int number_of_muons_minus;
    int number_of_muons_plus;
    int number_of_other_neutral_particles;
    int number_of_other_positive_particles;
    int number_of_other_negative_particles;

    std::vector<double> electrons_energies;
    std::vector<double> positrons_energies;
    std::vector<double> gammas_energies;
    std::vector<double> alphas_energies;
    std::vector<double> neutrons_energies;
    std::vector<double> protons_energies;
    std::vector<double> ions_energies;
    std::vector<double> muons_minus_energies;
    std::vector<double> muons_plus_energies;
    std::vector<double> other_neutral_particles_energies;
    std::vector<double> other_positive_particles_energies;
    std::vector<double> other_negative_particles_energies;

    std::vector<double> electrons_times;
    std::vector<double> positrons_times;
    std::vector<double> gammas_times;
    std::vector<double> alphas_times;
    std::vector<double> neutrons_times;
    std::vector<double> protons_times;
    std::vector<double> ions_times;
    std::vector<double> muons_minus_times;
    std::vector<double> muons_plus_times;
    std::vector<double> other_neutral_particles_times;
    std::vector<double> other_positive_particles_times;
    std::vector<double> other_negative_particles_times;

    std::vector<geomtools::vector_3d> electrons_momenta;
    std::vector<geomtools::vector_3d> positrons_momenta;
    std::vector<geomtools::vector_3d> gammas_momenta;
    std::vector<geomtools::vector_3d> alphas_momenta;
    std::vector<geomtools::vector_3d> neutrons_momenta;
    std::vector<geomtools::vector_3d> protons_momenta;
    std::vector<geomtools::vector_3d> ions_momenta;
    std::vector<geomtools::vector_3d> muons_minus_momenta;
    std::vector<geomtools::vector_3d> muons_plus_momenta;
    std::vector<geomtools::vector_3d> other_neutral_particles_momenta;
    std::vector<geomtools::vector_3d> other_positive_particles_momenta;
    std::vector<geomtools::vector_3d> other_negative_particles_momenta;

  private:
    geomtools::vector_3d _invalid_vector_;

    //! Reflection interface
    DR_CLASS_RTTI()

  };

  void inspector_data::reset()
  {
    number_of_particles = 0;
    number_of_charged_particles = 0;
    number_of_electrons = 0;
    number_of_positrons = 0;
    number_of_gammas = 0;
    number_of_alphas = 0;
    number_of_neutrons = 0;
    number_of_protons = 0;
    number_of_ions = 0;
    number_of_muons_minus = 0;
    number_of_muons_plus = 0;
    number_of_other_neutral_particles = 0;
    number_of_other_positive_particles = 0;
    number_of_other_negative_particles = 0;

    energy_sum = 0.0;
    charged_particles_energy_sum = 0.0;

    electrons_energies.clear();
    positrons_energies.clear();
    gammas_energies.clear();
    alphas_energies.clear();
    neutrons_energies.clear();
    protons_energies.clear();
    ions_energies.clear();
    muons_minus_energies.clear();
    muons_plus_energies.clear();
    other_neutral_particles_energies.clear();
    other_positive_particles_energies.clear();
    other_negative_particles_energies.clear();

    electrons_times.clear();
    positrons_times.clear();
    gammas_times.clear();
    alphas_times.clear();
    neutrons_times.clear();
    protons_times.clear();
    ions_times.clear();
    muons_minus_times.clear();
    muons_plus_times.clear();
    other_neutral_particles_times.clear();
    other_positive_particles_times.clear();
    other_negative_particles_times.clear();

    electrons_momenta.clear();
    positrons_momenta.clear();
    gammas_momenta.clear();
    alphas_momenta.clear();
    neutrons_momenta.clear();
    protons_momenta.clear();
    ions_momenta.clear();
    muons_minus_momenta.clear();
    muons_plus_momenta.clear();
    other_neutral_particles_momenta.clear();
    other_positive_particles_momenta.clear();
    other_negative_particles_momenta.clear();

    return;
  }

  inspector_data::inspector_data()
  {
    geomtools::invalidate(_invalid_vector_);
    reset();
    return;
  }

  double inspector_data::get_cos_theta(const std::string & type1_, int i1_,
                                       const std::string & type2_, int i2_) const
  {
    const geomtools::vector_3d * p1 = 0;
    const geomtools::vector_3d * p2 = 0;
    double cost12 = std::numeric_limits<double>::quiet_NaN();
    int i1max = -1;
    int i2max = -1;
    if (type1_ == "electron") i1max = electrons_momenta.size();
    if (type1_ == "positron") i1max = positrons_momenta.size();
    if (type1_ == "gamma") i1max = gammas_momenta.size();
    if (type1_ == "alpha") i1max = alphas_momenta.size();
    if (type1_ == "neutron") i1max = neutrons_momenta.size();
    if (type1_ == "proton") i1max = protons_momenta.size();
    if (type1_ == "ion") i1max = ions_momenta.size();
    if (type1_ == "muons_minus") i1max = muons_minus_momenta.size();
    if (type1_ == "muons_plus") i1max = muons_plus_momenta.size();
    if (type1_ == "other_neutral") i1max = other_neutral_particles_momenta.size();
    if (type1_ == "other_positive") i1max = other_positive_particles_momenta.size();
    if (type1_ == "other_negative") i1max = other_negative_particles_momenta.size();

    if (type2_ == "electron") i2max = electrons_momenta.size();
    if (type2_ == "positron") i2max = positrons_momenta.size();
    if (type2_ == "gamma") i2max = gammas_momenta.size();
    if (type2_ == "alpha") i2max = alphas_momenta.size();
    if (type2_ == "neutron") i2max = neutrons_momenta.size();
    if (type2_ == "proton") i2max = protons_momenta.size();
    if (type2_ == "ion") i2max = ions_momenta.size();
    if (type1_ == "muons_minus") i2max = muons_minus_momenta.size();
    if (type1_ == "muons_plus") i2max = muons_plus_momenta.size();
    if (type2_ == "other_neutral") i2max = other_neutral_particles_momenta.size();
    if (type2_ == "other_positive") i2max = other_positive_particles_momenta.size();
    if (type2_ == "other_negative") i2max = other_negative_particles_momenta.size();

    if (i1_ >= 0 && i2_ >= 0 && i1_ < i1max && i2_ < i2max) {

      if (type1_ == "electron") p1 = &electrons_momenta[i1_];
      if (type1_ == "positron") p1 = &positrons_momenta[i1_];
      if (type1_ == "gamma") p1 = &gammas_momenta[i1_];
      if (type1_ == "alpha") p1 = &alphas_momenta[i1_];
      if (type1_ == "neutron") p1 = &neutrons_momenta[i1_];
      if (type1_ == "proton") p1 = &protons_momenta[i1_];
      if (type1_ == "ion") p1 = &ions_momenta[i1_];
      if (type1_ == "muon_minus") p1 = &muons_minus_momenta[i1_];
      if (type1_ == "muon_plus") p1 = &muons_plus_momenta[i1_];
      if (type1_ == "other_neutral") p1 = &other_neutral_particles_momenta[i1_];
      if (type1_ == "other_positive") p1 = &other_positive_particles_momenta[i1_];
      if (type1_ == "other_negative") p1 = &other_negative_particles_momenta[i1_];

      if (type2_ == "electron") p2 = &electrons_momenta[i2_];
      if (type2_ == "positron") p2 = &positrons_momenta[i2_];
      if (type2_ == "gamma") p2 = &gammas_momenta[i2_];
      if (type2_ == "alpha") p2 = &alphas_momenta[i2_];
      if (type2_ == "neutron") p2 = &neutrons_momenta[i2_];
      if (type2_ == "proton") p2 = &protons_momenta[i2_];
      if (type2_ == "ion") p2 = &ions_momenta[i2_];
      if (type1_ == "muon_minus") p2 = &muons_minus_momenta[i2_];
      if (type1_ == "muon_plus") p2 = &muons_plus_momenta[i2_];
      if (type2_ == "other_neutral") p2 = &other_neutral_particles_momenta[i2_];
      if (type2_ == "other_positive") p2 = &other_positive_particles_momenta[i2_];
      if (type2_ == "other_negative") p2 = &other_negative_particles_momenta[i2_];
    }

    if (p1 && p2) {
      cost12 = std::cos(p1->angle(*p2));
    }
    return cost12;
  }

  /***/

  double inspector_data::get_electron_phi(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) electrons_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return electrons_momenta[i_].phi();
  }

  double inspector_data::get_positron_phi(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) positrons_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return positrons_momenta[i_].phi();
  }

  double inspector_data::get_gamma_phi(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) gammas_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return gammas_momenta[i_].phi();
  }

  double inspector_data::get_alpha_phi(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) alphas_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return alphas_momenta[i_].phi();
  }

  double inspector_data::get_neutron_phi(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) neutrons_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return neutrons_momenta[i_].phi();
  }

  double inspector_data::get_proton_phi(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) protons_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return protons_momenta[i_].phi();
  }

  double inspector_data::get_ion_phi(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) ions_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return ions_momenta[i_].phi();
  }

  double inspector_data::get_muon_minus_phi(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) muons_minus_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return muons_minus_momenta[i_].phi();
  }

  double inspector_data::get_muon_plus_phi(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) muons_plus_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return muons_plus_momenta[i_].phi();
  }

  double inspector_data::get_other_neutral_particle_phi(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) other_neutral_particles_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return other_neutral_particles_momenta[i_].phi();
  }

  double inspector_data::get_other_positive_particle_phi(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) other_positive_particles_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return other_positive_particles_momenta[i_].phi();
  }

  double inspector_data::get_other_negative_particle_phi(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) other_positive_particles_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return other_positive_particles_momenta[i_].phi();
  }

  /***/

  double inspector_data::get_electron_cos_theta(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) electrons_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return electrons_momenta[i_].cosTheta();
  }

  double inspector_data::get_positron_cos_theta(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) positrons_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return positrons_momenta[i_].cosTheta();
  }

  double inspector_data::get_gamma_cos_theta(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) gammas_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return gammas_momenta[i_].cosTheta();
  }

  double inspector_data::get_alpha_cos_theta(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) alphas_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return alphas_momenta[i_].cosTheta();
  }

  double inspector_data::get_neutron_cos_theta(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) neutrons_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return neutrons_momenta[i_].cosTheta();
  }

  double inspector_data::get_proton_cos_theta(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) protons_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return protons_momenta[i_].cosTheta();
  }

  double inspector_data::get_ion_cos_theta(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) ions_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return ions_momenta[i_].cosTheta();
  }

  double inspector_data::get_muon_minus_cos_theta(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) muons_minus_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return muons_minus_momenta[i_].cosTheta();
  }

  double inspector_data::get_muon_plus_cos_theta(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) muons_plus_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return muons_plus_momenta[i_].cosTheta();
  }

  double inspector_data::get_other_neutral_particle_cos_theta(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) other_neutral_particles_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return other_neutral_particles_momenta[i_].cosTheta();
  }

  double inspector_data::get_other_positive_particle_cos_theta(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) other_positive_particles_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return other_positive_particles_momenta[i_].cosTheta();
  }

  double inspector_data::get_other_negative_particle_cos_theta(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) other_positive_particles_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return other_positive_particles_momenta[i_].cosTheta();
  }

  /***/
  double inspector_data::get_electron_energy(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) electrons_energies.size()) return std::numeric_limits<double>::quiet_NaN();
    return electrons_energies[i_];
  }

  double inspector_data::get_positron_energy(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) positrons_energies.size()) return std::numeric_limits<double>::quiet_NaN();
    return positrons_energies[i_];
  }

  double inspector_data::get_gamma_energy(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) gammas_energies.size()) return std::numeric_limits<double>::quiet_NaN();
    return gammas_energies[i_];
  }

  double inspector_data::get_alpha_energy(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) alphas_energies.size()) return std::numeric_limits<double>::quiet_NaN();
    return alphas_energies[i_];
  }

  double inspector_data::get_neutron_energy(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) neutrons_energies.size()) return std::numeric_limits<double>::quiet_NaN();
    return neutrons_energies[i_];
  }

  double inspector_data::get_proton_energy(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) protons_energies.size()) return std::numeric_limits<double>::quiet_NaN();
    return protons_energies[i_];
  }

  double inspector_data::get_ion_energy(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) ions_energies.size()) return std::numeric_limits<double>::quiet_NaN();
    return ions_energies[i_];
  }

  double inspector_data::get_muon_minus_energy(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) muons_minus_energies.size()) return std::numeric_limits<double>::quiet_NaN();
    return muons_minus_energies[i_];
  }

  double inspector_data::get_muon_plus_energy(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) muons_plus_energies.size()) return std::numeric_limits<double>::quiet_NaN();
    return muons_plus_energies[i_];
  }

  double inspector_data::get_other_neutral_particle_energy(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) other_neutral_particles_energies.size()) return std::numeric_limits<double>::quiet_NaN();
    return other_neutral_particles_energies[i_];
  }

  double inspector_data::get_other_positive_particle_energy(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) other_positive_particles_energies.size()) return std::numeric_limits<double>::quiet_NaN();
    return other_positive_particles_energies[i_];
  }

  double inspector_data::get_other_negative_particle_energy(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) other_positive_particles_energies.size()) return std::numeric_limits<double>::quiet_NaN();
    return other_positive_particles_energies[i_];
  }

  double inspector_data::get_electron_time(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) electrons_times.size()) return std::numeric_limits<double>::quiet_NaN();
    return electrons_times[i_];
  }

  double inspector_data::get_positron_time(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) positrons_times.size()) return std::numeric_limits<double>::quiet_NaN();
    return positrons_times[i_];
  }

  double inspector_data::get_gamma_time(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) gammas_times.size()) return std::numeric_limits<double>::quiet_NaN();
    return gammas_times[i_];
  }

  double inspector_data::get_alpha_time(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) alphas_times.size()) return std::numeric_limits<double>::quiet_NaN();
    return alphas_times[i_];
  }

  double inspector_data::get_neutron_time(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) neutrons_times.size()) return std::numeric_limits<double>::quiet_NaN();
    return neutrons_times[i_];
  }

  double inspector_data::get_proton_time(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) protons_times.size()) return std::numeric_limits<double>::quiet_NaN();
    return protons_times[i_];
  }

  double inspector_data::get_ion_time(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) ions_times.size()) return std::numeric_limits<double>::quiet_NaN();
    return ions_times[i_];
  }

  double inspector_data::get_muon_minus_time(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) muons_minus_times.size()) return std::numeric_limits<double>::quiet_NaN();
    return muons_minus_times[i_];
  }

  double inspector_data::get_muon_plus_time(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) muons_plus_times.size()) return std::numeric_limits<double>::quiet_NaN();
    return muons_plus_times[i_];
  }

  double inspector_data::get_other_neutral_particle_time(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) other_neutral_particles_times.size()) return std::numeric_limits<double>::quiet_NaN();
    return other_neutral_particles_times[i_];
  }

  double inspector_data::get_other_positive_particle_time(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) other_positive_particles_times.size()) return std::numeric_limits<double>::quiet_NaN();
    return other_positive_particles_times[i_];
  }

  double inspector_data::get_other_negative_particle_time(int i_) const
  {
    if ( i_ < 0 || i_ >= (int) other_positive_particles_times.size()) return std::numeric_limits<double>::quiet_NaN();
    return other_positive_particles_times[i_];
  }

  double inspector_data::get_electrons_energy_sum() const
  {
    if (electrons_energies.size() == 0) return std::numeric_limits<double>::quiet_NaN();
    return std::accumulate(electrons_energies.begin(),
                           electrons_energies.end(),
                           0.0);
  }

  double inspector_data::get_positrons_energy_sum() const
  {
    if (positrons_energies.size() == 0) return std::numeric_limits<double>::quiet_NaN();
    return std::accumulate(positrons_energies.begin(),
                           positrons_energies.end(),
                           0.0);
  }

  double inspector_data::get_gammas_energy_sum() const
  {
    if (gammas_energies.size() == 0) return std::numeric_limits<double>::quiet_NaN();
    return std::accumulate(gammas_energies.begin(),
                           gammas_energies.end(),
                           0.0);
  }

  double inspector_data::get_alphas_energy_sum() const
  {
    if (alphas_energies.size() == 0) return std::numeric_limits<double>::quiet_NaN();
    return std::accumulate(alphas_energies.begin(),
                           alphas_energies.end(),
                           0.0);
  }

  double inspector_data::get_neutrons_energy_sum() const
  {
    if (neutrons_energies.size() == 0) return std::numeric_limits<double>::quiet_NaN();
    return std::accumulate(neutrons_energies.begin(),
                           neutrons_energies.end(),
                           0.0);
  }

  double inspector_data::get_protons_energy_sum() const
  {
    if (protons_energies.size() == 0) return std::numeric_limits<double>::quiet_NaN();
    return std::accumulate(protons_energies.begin(),
                           protons_energies.end(),
                           0.0);
  }

  double inspector_data::get_ions_energy_sum() const
  {
    if (ions_energies.size() == 0) return std::numeric_limits<double>::quiet_NaN();
    return std::accumulate(ions_energies.begin(),
                           ions_energies.end(),
                           0.0);
  }

  double inspector_data::get_muons_minus_energy_sum() const
  {
    if (muons_minus_energies.size() == 0) return std::numeric_limits<double>::quiet_NaN();
    return std::accumulate(muons_minus_energies.begin(),
                           muons_minus_energies.end(),
                           0.0);
  }

  double inspector_data::get_muons_plus_energy_sum() const
  {
    if (muons_plus_energies.size() == 0) return std::numeric_limits<double>::quiet_NaN();
    return std::accumulate(muons_plus_energies.begin(),
                           muons_plus_energies.end(),
                           0.0);
  }

  double inspector_data::get_other_neutral_particles_energy_sum() const
  {
    if (other_neutral_particles_energies.size() == 0) return std::numeric_limits<double>::quiet_NaN();
    return std::accumulate(other_neutral_particles_energies.begin(),
                           other_neutral_particles_energies.end(),
                           0.0);
  }

  double inspector_data::get_other_positive_particles_energy_sum() const
  {
    if (other_positive_particles_energies.size() == 0) return std::numeric_limits<double>::quiet_NaN();
    return std::accumulate(other_positive_particles_energies.begin(),
                           other_positive_particles_energies.end(),
                           0.0);
  }

  double inspector_data::get_other_negative_particles_energy_sum() const
  {
    if (other_negative_particles_energies.size() == 0) return std::numeric_limits<double>::quiet_NaN();
    return std::accumulate(other_negative_particles_energies.begin(),
                           other_negative_particles_energies.end(),
                           0.0);
  }


  void inspector_data::sort()
  {
    // Sort energies in descending order :
    std::sort(gammas_energies.begin(), gammas_energies.end());
    std::sort(electrons_energies.begin(), electrons_energies.end());
    std::sort(positrons_energies.begin(), positrons_energies.end());
    std::sort(alphas_energies.begin(), alphas_energies.end());
    std::sort(neutrons_energies.begin(), neutrons_energies.end());
    std::sort(protons_energies.begin(), protons_energies.end());
    std::sort(ions_energies.begin(), ions_energies.end());
    std::sort(muons_minus_energies.begin(), muons_minus_energies.end());
    std::sort(muons_plus_energies.begin(), muons_plus_energies.end());
    std::sort(other_neutral_particles_energies.begin(), other_neutral_particles_energies.end());
    std::sort(other_positive_particles_energies.begin(), other_positive_particles_energies.end());
    std::sort(other_negative_particles_energies.begin(), other_negative_particles_energies.end());

    std::reverse(gammas_energies.begin(), gammas_energies.end());
    std::reverse(electrons_energies.begin(), electrons_energies.end());
    std::reverse(positrons_energies.begin(), positrons_energies.end());
    std::reverse(alphas_energies.begin(), alphas_energies.end());
    std::reverse(neutrons_energies.begin(), neutrons_energies.end());
    std::reverse(protons_energies.begin(), protons_energies.end());
    std::reverse(ions_energies.begin(), ions_energies.end());
    std::reverse(muons_minus_energies.begin(), muons_minus_energies.end());
    std::reverse(muons_plus_energies.begin(), muons_plus_energies.end());
    std::reverse(other_neutral_particles_energies.begin(), other_neutral_particles_energies.end());
    std::reverse(other_positive_particles_energies.begin(), other_positive_particles_energies.end());
    std::reverse(other_negative_particles_energies.begin(), other_negative_particles_energies.end());

    return;
  }

  /// \brief Inspector of GENBB primary event generator
  class inspector
  {
  public:
    enum timing_type {
      PROMPT  = 0,
      DELAYED = 1
    };

    static const int DEFAULT_MAX_NUMBER_OF_EVENTS = 1000000;

    inspector();
    virtual ~inspector();
    bool is_initialized() const;
    void initialize(const genbb::inspector_params &);
    void reset();
    void run();
  protected:
    void _run_interactive();
    void _run_batch();
    void _inspect(const genbb::primary_event &);
    void _inspect_plain_writer_mode(const genbb::primary_event &);
    void _inspect_bank_writer_mode(const genbb::primary_event &);
    void _inspect_histograms_mode(const genbb::primary_event &);

  private:
    bool _initialized_;
    genbb::inspector_params _params_;

    // Working attributes :
    mygsl::rng _prng_;
    genbb::manager _manager_;
    genbb::i_genbb * _generator_;

    boost::scoped_ptr<datatools::data_writer> _plain_writer_;
    boost::scoped_ptr<dpp::output_module>     _bank_writer_;
    boost::scoped_ptr<dpp::histogram_service> _histos_service_;
    mygsl::histogram_pool * _histos_;
    std::string _title_prefix_;
    std::string _name_suffix_;

  };

} // namespace genbb


/************************************************************
 * Reflection support for the 'genbb::inspector_data' class *
 ************************************************************/

// Activate reflection layer for the 'genbb::inspector_data' class:
DR_CLASS_INIT(genbb::inspector_data)

// Export the introspection code for the 'genbb::inspector_data' class:
DR_CLASS_EXPORT (genbb::inspector_data, "genbb::inspector_data")

namespace datatools {
  namespace detail {
    namespace reflection {

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::genbb::inspector_data, tag_ __attribute__((unused)) )
      {
        // Declare and populate the introspection metaclass for the 'genbb::inspector_data' class:
        DR_CLASS_DECLARE (::genbb::inspector_data)
          .DR_CLASS_CONSTRUCTOR()

          .DR_CLASS_PROPERTY_GETTER("number_of_particles",
                                    genbb::inspector_data::number_of_particles)
          .DR_CLASS_PROPERTY_GETTER("number_of_charged_particles",
                                    genbb::inspector_data::number_of_charged_particles)

          .DR_CLASS_PROPERTY_GETTER("energy_sum",
                                    genbb::inspector_data::energy_sum)
          .DR_CLASS_PROPERTY_GETTER("charged_particles_energy_sum",
                                    genbb::inspector_data::charged_particles_energy_sum)

          .DR_CLASS_PROPERTY_GETTER("number_of_electrons",
                                    genbb::inspector_data::number_of_electrons)
          .DR_CLASS_PROPERTY_GETTER("number_of_positrons",
                                    genbb::inspector_data::number_of_positrons)
          .DR_CLASS_PROPERTY_GETTER("number_of_gammas",
                                    genbb::inspector_data::number_of_gammas)
          .DR_CLASS_PROPERTY_GETTER("number_of_alphas",
                                    genbb::inspector_data::number_of_alphas)
          .DR_CLASS_PROPERTY_GETTER("number_of_neutrons",
                                    genbb::inspector_data::number_of_neutrons)
          .DR_CLASS_PROPERTY_GETTER("number_of_protons",
                                    genbb::inspector_data::number_of_protons)
          .DR_CLASS_PROPERTY_GETTER("number_of_ions",
                                    genbb::inspector_data::number_of_ions)
          .DR_CLASS_PROPERTY_GETTER("number_of_muons_minus",
                                    genbb::inspector_data::number_of_muons_minus)
          .DR_CLASS_PROPERTY_GETTER("number_of_muons_plus",
                                    genbb::inspector_data::number_of_muons_plus)
          .DR_CLASS_PROPERTY_GETTER("number_of_other_neutral_particles",
                                    genbb::inspector_data::number_of_other_neutral_particles)
          .DR_CLASS_PROPERTY_GETTER("number_of_other_positive_particles",
                                    genbb::inspector_data::number_of_other_positive_particles)
          .DR_CLASS_PROPERTY_GETTER("number_of_other_negative_particles",
                                    genbb::inspector_data::number_of_other_negative_particles)

          .DR_CLASS_PROPERTY_GETTER("electrons_energy_sum",
                                    genbb::inspector_data::get_electrons_energy_sum)
          .DR_CLASS_PROPERTY_GETTER("positrons_energy_sum",
                                    genbb::inspector_data::get_positrons_energy_sum)
          .DR_CLASS_PROPERTY_GETTER("gammas_energy_sum",
                                    genbb::inspector_data::get_gammas_energy_sum)
          .DR_CLASS_PROPERTY_GETTER("alphas_energy_sum",
                                    genbb::inspector_data::get_alphas_energy_sum)
          .DR_CLASS_PROPERTY_GETTER("neutrons_energy_sum",
                                    genbb::inspector_data::get_neutrons_energy_sum)
          .DR_CLASS_PROPERTY_GETTER("protons_energy_sum",
                                    genbb::inspector_data::get_protons_energy_sum)
          .DR_CLASS_PROPERTY_GETTER("ions_energy_sum",
                                    genbb::inspector_data::get_ions_energy_sum)
          .DR_CLASS_PROPERTY_GETTER("muons_minus_energy_sum",
                                    genbb::inspector_data::get_muons_minus_energy_sum)
          .DR_CLASS_PROPERTY_GETTER("muons_plus_energy_sum",
                                    genbb::inspector_data::get_muons_plus_energy_sum)
          .DR_CLASS_PROPERTY_GETTER("other_neutral_particles_energy_sum",
                                    genbb::inspector_data::get_other_neutral_particles_energy_sum)
          .DR_CLASS_PROPERTY_GETTER("other_positive_particles_energy_sum",
                                    genbb::inspector_data::get_other_positive_particles_energy_sum)
          .DR_CLASS_PROPERTY_GETTER("other_negative_particles_energy_sum",
                                    genbb::inspector_data::get_other_negative_particles_energy_sum)
          .DR_CLASS_METHOD1_CONST("electron_energy",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_electron_energy,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("positron_energy",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_positron_energy,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("gamma_energy",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_gamma_energy,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("alpha_energy",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_alpha_energy,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("neutron_energy",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_neutron_energy,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("proton_energy",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_proton_energy,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("ion_energy",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_ion_energy,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("muon_minus_energy",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_muon_minus_energy,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("muon_plus_energy",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_muon_plus_energy,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("other_neutral_particle_energy",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_other_neutral_particle_energy,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("other_positive_particle_energy",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_other_positive_particle_energy,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("other_negative_particle_energy",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_other_negative_particle_energy,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("electron_time",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_electron_time,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("positron_time",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_positron_time,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("gamma_time",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_gamma_time,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("alpha_time",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_alpha_time,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("neutron_time",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_neutron_time,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("proton_time",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_proton_time,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("ion_time",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_ion_time,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("muon_minus_time",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_muon_minus_time,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("muon_plus_time",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_muon_plus_time,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("other_neutral_particle_time",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_other_neutral_particle_time,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("other_positive_particle_time",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_other_positive_particle_time,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("other_negative_particle_time",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_other_negative_particle_time,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("electron_phi",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_electron_phi,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("positron_phi",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_positron_phi,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("gamma_phi",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_gamma_phi,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("alpha_phi",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_alpha_phi,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("neutron_phi",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_neutron_phi,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("proton_phi",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_proton_phi,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("ion_phi",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_ion_phi,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("muon_minus_phi",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_muon_minus_phi,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("muon_plus_phi",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_muon_plus_phi,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("other_neutral_particle_phi",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_other_neutral_particle_phi,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("other_positive_particle_phi",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_other_positive_particle_phi,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("other_negative_particle_phi",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_other_negative_particle_phi,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("electron_cos_theta",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_electron_cos_theta,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("positron_cos_theta",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_positron_cos_theta,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("gamma_cos_theta",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_gamma_cos_theta,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("alpha_cos_theta",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_alpha_cos_theta,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("neutron_cos_theta",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_neutron_cos_theta,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("proton_cos_theta",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_proton_cos_theta,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("ion_cos_theta",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_ion_cos_theta,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("muon_minus_cos_theta",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_muon_minus_cos_theta,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("muon_plus_cos_theta",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_muon_plus_cos_theta,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("other_neutral_particle_cos_theta",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_other_neutral_particle_cos_theta,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("other_positive_particle_cos_theta",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_other_positive_particle_cos_theta,
                                  double,
                                  int)
          .DR_CLASS_METHOD1_CONST("other_negative_particle_cos_theta",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_other_negative_particle_cos_theta,
                                  double,
                                  int)
          .DR_CLASS_METHOD4_CONST("cos_theta",
                                  genbb::inspector_data,
                                  genbb::inspector_data::get_cos_theta,
                                  double,
                                  std::string,
                                  int,
                                  std::string,
                                  int)
          ;
        return;
      }
    }
  }
}

//! Return the application name
std::string app_name();

//! Print application splash screen
void app_print_splash(std::ostream & out_);

//! Print application usage (supported options and arguments)
void app_print_help(const boost::program_options::options_description & opts_,
                    std::ostream & out_ = std::clog);

// Return kernel initialization flags for this application:
uint32_t app_kernel_init_flags();

int main (int argc_, char ** argv_)
{
  bayeux::initialize(argc_, argv_, app_kernel_init_flags());

  int error_code = EXIT_SUCCESS;
  genbb::inspector_params params;
  namespace po = boost::program_options;
  po::options_description optPublic;
  po::options_description opts("Allowed options");
  po::positional_options_description args;
  try {
    std::vector<std::string> LL_dlls;
    std::string LL_config;

    opts.add_options()
      ("help,h",
       po::value<bool>()
       ->zero_tokens()
       ->default_value(false),
       "Produce help message"
       )

      ("load-dll,l",
       po::value<std::vector<std::string> > (),
       "Load a dynamic library"
       )

      ("dll-config",
       po::value<std::vector<std::string> > (),
       "Load a configuration file for dynamic library loading"
       )

      ("no-splash,S",
       po::value<bool>(&params.no_splash)
       ->zero_tokens()
       ->default_value(false),
       "Do not print the splash screen.\n"
       "Example : \n"
       "  --no-splash "
       )

      ("debug,d",
       po::value<bool>(&params.debug)
       ->zero_tokens()
       ->default_value(false),
       "Produce debug logging"
       )

      ("interactive,I",
       po::value<bool>(&params.interactive)
       ->zero_tokens()
       ->default_value (false),
       "Run in interactive mode (not implemented)"
       )

      ("number-of-events,n",
       po::value<int>(&params.number_of_events)
       ->default_value (0),
       "Set the number of generated events. \n"
       "Example :                           \n"
       " --number-of-events 10000             "
       )

      ("modulo,%",
       po::value<int>(&params.print_modulo)
       ->default_value(100),
       "Set the modulo print period of \n"
       "generated events.              \n"
       "Example :                      \n"
       " --modulo 100                    "
       )

      ("trace-index,x",
       po::value<int>(&params.trace_index)
       ->default_value (0),
       "Set the trace index. \n"
       "Example :            \n"
       " --trace-index 10000   "
       )

      ("prng-trunc,u",
       po::value<int>(&params.prng_trunc)
       ->default_value (0),
       "Set the trunc index of the random \n"
       "number generator (PRNG).          \n"
       "Example :                         \n"
       " --prng-trunc 7                     "
       )

      ("prng-seed,s",
       po::value<int>(&params.prng_seed)
       ->default_value (0),
       "Set the seed of the random number \n"
       "generator (PRNG).    \n"
       "Example :            \n"
       " --prng-seed 314159    "
       )

      ("prng-tracker,K",
       po::value<std::string> (&params.prng_tracker),
       "Set the PRNG tracker file. \n"
       "Example :    \n"
       " --prng-tracker \"genbb_inspector_prng.trk\"  "
       )

      ("configuration,c",
       po::value<std::string> (&params.configuration),
       "Set the genbb manager configuration file. \n"
       "Example :                                 \n"
       " --configuration \"genbb.conf\"             "
       )

      ("action,a",
       po::value<std::string> (&params.action),
       "Set the action.         \n"
       "Examples :              \n"
       " --action \"list\"      \n"
       " --action \"shoot\"     "
       )

      ("list-print-mode",
       po::value<std::string> (&params.list_print_mode),
       "Set the print mode for list action.          \n"
       "Examples :                                   \n"
       " --action \"list\" --list-print-mode \"tree\"\n"
       " --action \"list\" --list-print-mode \"raw\" "
       )

      ("generator,g",
       po::value<std::string> (&params.generator),
       "Set the particle generator in 'shoot'\n"
       "mode. \n"
       "Example :              \n"
       " --generator \"Bi207\"   "
       )

      ("histo-def,H",
       po::value<std::vector<std::string> > (),
       "set the name of an histogram definition input filename."
       )

      ("output-mode,m",
       po::value<std::string> (&params.output_mode),
       "Set the genbb manager output mode.      \n"
       "Example :                               \n"
       " --output-mode \"histograms\"           \n"
       "or :                                    \n"
       " --output-mode \"plain\"                \n"
       "or :                                    \n"
       " --output-mode \"bank\"                   "
       )

      ("output-bank-label,l",
       po::value<std::string> (&params.output_bank_label),
       "set the bank label in 'bank' output mode.\n"
       "Example :                                \n"
       " --output-bank-label \"PE\"                "
       )

      ("output-bank-max-records,x",
       po::value<int> (&params.output_bank_max_records),
       "Set the maximum records per file \n"
       "in 'bank' output mode.           \n"
       "Example :                        \n"
       " --output-bank-max-records 100     "
       )

      ("output-file,o",
       po::value<std::vector<std::string> > (),
       "Set the name of an output filename."
       )

      ("prompt-time-limit,t",
       po::value<double> (&params.prompt_time_limit),
       "Set the limit on prompt time in ns."
       )

      ("prompt,P",
       po::value<bool>(&params.prompt)
       ->zero_tokens()
       ->default_value (true),
       "Analyze prompt particles"
       )

      ("delayed,D",
       po::value<bool>(&params.delayed)
       ->zero_tokens()
       ->default_value (false),
       "Analyze delayed particles"
       )

      // ("histo-group,G",
      //  po::value<std::vector<std::string> > (&params.histo_groups),
      //  "activate the building of histograms a given group."
      //  )

      ("title-prefix,T",
       po::value<std::string>(&params.title_prefix),
       "Set a title prefix for exported histograms"
       )

      ("name-suffix,S",
       po::value<std::string>(&params.name_suffix),
       "Set a name suffix for exported histograms"
       )

      ; // end of options description

    // Declare options for variant support:
    po::options_description optVariant("Variant support");
    uint32_t po_init_flags = 0;
    po_init_flags |= dtc::variant_service::NO_LABEL;
    po_init_flags |= dtc::variant_service::NO_LOGGING;
    // po_init_flags |= dtc::variant_service::PROFILE_LOAD_DONT_IGNORE_UNKNOWN;
    po_init_flags |= dtc::variant_service::NO_TUI;
    dtc::variant_service::init_options(optVariant,
                                       params.variants,
                                       po_init_flags);

    po::options_description optKernel("Kernel options");
    datatools::kernel::param_type paramsKernel;
    datatools::kernel::build_opt_desc(optKernel, paramsKernel, app_kernel_init_flags());

    // Aggregate options:
    optPublic.add(opts).add(optVariant).add(optKernel);

    // Preprocessor for command line arguments:
    unsigned int vpp_flags = 0;
    // vpp_flags |= datatools::configuration::variant_preprocessor::FLAG_TRACE;
    vpp_flags |= datatools::configuration::variant_preprocessor::FLAG_REMOVE_QUOTES;
    datatools::configuration::variant_preprocessor vpp(vpp_flags);
    std::vector<std::string> preprocessed_arguments;
    vpp.preprocess_args_options(argc_, argv_, preprocessed_arguments);

    po::variables_map vm;
    po::parsed_options parsed =
      po::command_line_parser(preprocessed_arguments)
      .options(optPublic)
      .allow_unregistered()
      .run();
    params.unrecognized_options = po::collect_unrecognized(parsed.options,
                                                           po::include_positional);
    po::store(parsed, vm);
    po::notify(vm);

    // Fetch the opts/args :
    if (vm["help"].as<bool>()) {
      app_print_help(optPublic, std::cout);
      return error_code;
    }

    if (vm.count("load-dll")) {
      LL_dlls = vm["load-dll"].as<std::vector<std::string> > ();
    }

    if (vm.count("dll-config")) {
      LL_config = vm["dll-config"].as<std::string> ();
    }

    datatools::library_loader LL(LL_config);
    BOOST_FOREACH (const std::string & dll_name, LL_dlls) {
      DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Loading DLL '" << dll_name << "'...");
      DT_THROW_IF (LL.load (dll_name) != EXIT_SUCCESS,
                   std::runtime_error,
                   "Loading DLL '" << dll_name << "' failed !");
    }

    if (vm.count("output-file")) {
      params.output_paths = vm["output-file"].as<std::vector<std::string> > ();
    }

    if (vm.count("histo-def")) {
      params.histos_definitions = vm["histo-def"].as<std::vector<std::string> > ();
    }

    if (! params.no_splash) {
      app_print_splash(std::cerr);
    }

    // Variant service:
    std::unique_ptr<dtc::variant_service> vserv;
    // params.variants.print(std::cerr, "Variant configuration: ", "DEVEL: ");
    if (params.variants.is_active()) {
      // Create and start the variant service:
      vserv.reset(new dtc::variant_service);
      vserv->configure(params.variants);
      vserv->start();
    }

    if (params.debug) {
      params.dump(std::clog);
    }

    {
      genbb::inspector GI;
      GI.initialize(params);
      GI.run();
      GI.reset();
    }

    if (vserv) {
      // Stop the variant service:
      vserv->stop();
      vserv.reset();
    }

  } catch (std::exception & x) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, x.what());
    error_code = EXIT_FAILURE;
  } catch (...) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }

  bayeux::terminate();
  return error_code;
}

//! Return the application name
std::string app_name()
{
  return "bxgenbb_inspector";
}

void app_print_splash(std::ostream & out_)
{
  out_ << "                                                   \n"
       << "\tB A Y E U X  -  G E N B B    I N S P E C T O R   \n"
       << "\tVersion " << GENBB_HELP_LIB_VERSION << "         \n"
       << "                                                   \n"
       << "\tCopyright (C) 2009-2017, the BxCppDev group      \n"
       << "\tFrancois Mauger, Xavier Garrido, Arnaud Chapon   \n"
       << "                                                   \n";
  return;
}

void app_print_help(const boost::program_options::options_description & options_,
                    std::ostream & out_)
{
  out_ << "\n " << app_name() << " -- Inspector for GENBB primary event generators" << std::endl;
  out_ << std::endl;
  out_ << "Usage : " << std::endl;
  out_ << std::endl;
  out_ << "  " << app_name() << " [OPTIONS] [ARGUMENTS] " << std::endl << std::endl;
  out_ << options_ << std::endl;
  out_ << std::endl;
  out_ << "Examples : " << std::endl;

  out_ << "" << std::endl;
  out_ << "  " << app_name() << " --help\n";
  out_ << "" << std::endl;

  out_ << "  " << app_name() << " \\\n";
  out_ << "    --configuration \""
       << "@genbb_help:manager/config/pro-1.0/manager.conf"
       << "\" \\\n";
  out_ << "    --action \"list\"  \n";
  out_ << "" << std::endl;

  out_ << "  " << app_name() << " \\\n";
  out_ << "    --load-dll \"foo\" \\\n";
  out_ << "    --datatools::resource-path \"foo@${FOO_INSTALLATION_DIR}/share/foo-1.0.0/resources\" \\\n";
  out_ << "    --configuration \"@foo:config/event_generation/1.0/manager.conf\" \\\n";
  out_ << "    --action \"list\"  \n";
  out_ << "" << std::endl;

  out_ << "  " << app_name() << "  \\\n";
  out_ << "    --configuration \""
       << "@genbb_help:manager/config/pro-1.0/manager.conf"
       << "\" \\\n";
  out_ << "    --action \"shoot\"  \\\n";
  out_ << "    --generator \"Bi214_Po214\" \\\n";
  out_ << "    --prng-seed 314159 \\\n";
  out_ << "    --number-of-events 100000 \\\n";
  out_ << "    --modulo 1000 \\\n";
  out_ << "    --histo-def \""
       << "@genbb_help:inspector/config/le_nuphy-1.0/inspector_histos_prompt.conf"
       << "\" \\\n";
  out_ << "    --histo-def \""
       << "@genbb_help:inspector/config/le_nuphy-1.0/inspector_histos_delayed.conf"
       << "\" \\\n";
  out_ << "    --prompt-time-limit 1 \\\n";
  out_ << "    --prompt \\\n";
  out_ << "    --delayed \\\n";
  out_ << "    --title-prefix \"Bi214-Po214\" \\\n";
  out_ << "    --output-file \"histos_Bi214_Po214.root\" \n";
  out_ << "\n";

  return;
}

namespace genbb {

  inspector::inspector()
  {
    _initialized_ = false;
    return;
  }

  inspector::~inspector()
  {
    if (_initialized_) {
      reset();
    }
    return;
  }

  bool inspector::is_initialized() const
  {
    return _initialized_;
  }

  void inspector::initialize(const genbb::inspector_params & params_)
  {
    DT_THROW_IF (_initialized_, std::logic_error, "Already initialized !");
    _params_ = params_;

    // PRNG :
    if (_params_.prng_seed <= 0) {
      _params_.prng_seed = (int32_t) time(0);
    }
    _prng_.initialize("taus2", _params_.prng_seed);
    if (! _params_.prng_tracker.empty()) {
      _prng_.set_tracker(_params_.prng_tracker);
    }
    if (_params_.prng_trunc > 0) {
      _prng_.set_trunc(_params_.prng_trunc);
    }
    if (_title_prefix_.empty() && ! _params_.title_prefix.empty()) {
      _title_prefix_ = _params_.title_prefix;
    }
    if (_name_suffix_.empty() && ! _params_.name_suffix.empty()) {
      _name_suffix_ = _params_.name_suffix;
    }

    DT_THROW_IF(_params_.action.empty(),
                std::logic_error,
                "Missing action !");

    DT_THROW_IF(_params_.action != "list" && _params_.action != "shoot",
                std::logic_error,
                "Invalid action '" << _params_.action << "' !");

    // Event generator manager :
    _manager_.set_debug(_params_.debug);
    _manager_.set_external_prng(_prng_);
    datatools::properties config;
    DT_THROW_IF(_params_.configuration.empty(),
                std::logic_error,
                "Missing configuration file for the primary event generator manager !");

    unsigned int vpp_flags = 0;
    // vpp_flags |= datatools::configuration::variant_preprocessor::FLAG_DEVEL;
    datatools::configuration::variant_preprocessor vpp(vpp_flags);

    std::string config_filename = _params_.configuration;
    datatools::fetch_path_with_env(config_filename);
    datatools::read_config(config_filename, config);
    _manager_.initialize(config);

    if (_params_.action == "list") {
      if (_params_.list_print_mode.empty()) {
        _params_.list_print_mode = "tree";
      }
    }

    if (_params_.action == "shoot") {
      std::string effective_generator;
      if (!_params_.generator.empty()) {
        datatools::command::returned_info cri =
          vpp.preprocess(_params_.generator, effective_generator);
        DT_THROW_IF(! cri.is_success(),  std::logic_error,
                    "Failed preprocessing of generator directive '"
                    << _params_.generator << "' : "
                    << cri.get_error_message());
        // effective_generator = _params_.generator;
      } else {
        if (_manager_.has_default_generator()) {
          effective_generator = _manager_.get_default_generator();
        }
      }
      datatools::remove_quotes(effective_generator, '"');
      DT_THROW_IF(effective_generator.empty(), std::logic_error,
                  "Missing generator name !");
      DT_THROW_IF (! _manager_.has(effective_generator), std::logic_error,
                   "Generator named '" << effective_generator << "' does not exist !");
      _generator_ = &_manager_.grab(effective_generator);

      if (_params_.output_mode.empty()) {
        // Default values:
        _params_.output_mode = "histograms";
      }

      if (_params_.output_mode == "plain") {
        // Save data in plain format :
        DT_THROW_IF(_params_.output_paths.size() != 1,
                    std::logic_error,
                    "No unique output data path is provided!");
        std::string output_data_filename = _params_.output_paths[0];
        datatools::fetch_path_with_env(output_data_filename);
        datatools::data_writer * writer = new datatools::data_writer;
        writer->init(output_data_filename, datatools::using_multi_archives);
        _plain_writer_.reset(writer);
      } else if (_params_.output_mode == "bank") {
        // Save data in bank format:
        dpp::output_module * writer = new dpp::output_module;
        writer->set_list_of_output_files(_params_.output_paths);
        writer->set_limits(_params_.number_of_events,
                           _params_.output_bank_max_records);
        writer->initialize_simple();
        _bank_writer_.reset(writer);
      } else if (_params_.output_mode == "histograms") {
        // Histograms :
        std::vector<std::string> pool_histo_setups;
        if (_params_.histos_definitions.size() == 0) {
          if (_params_.prompt) {
            if (std::find(_params_.histo_groups.begin(),
                          _params_.histo_groups.end(),
                          "prompt") == _params_.histo_groups.end()) {
              _params_.histo_groups.push_back("prompt");
            }

            std::ostringstream filename;
            filename << genbb::get_resource_dir() << '/'
                     << "inspector/config/le_nuphy-1.0/inspector_histos_prompt.conf";
            _params_.histos_definitions.push_back(filename.str());
            DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                          "Using default definition file for histograms : '" << filename.str() << "'");
          }
          if (_params_.delayed) {
            if (std::find(_params_.histo_groups.begin(),
                          _params_.histo_groups.end(),
                          "delayed") == _params_.histo_groups.end()) {
              _params_.histo_groups.push_back("delayed");
            }
            std::ostringstream filename;
            filename << genbb::get_resource_dir() << '/'
                     << "inspector/config/le_nuphy-1.0/inspector_histos_delayed.conf";
            _params_.histos_definitions.push_back(filename.str());
            DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                          "Using default definition file for histograms : '" << filename.str() << "'");
          }
        }
        for (int i = 0; i < (int) _params_.histos_definitions.size(); i++) {
          std::string hconfig = _params_.histos_definitions[i];
          datatools::fetch_path_with_env(hconfig);
          pool_histo_setups.push_back(hconfig);
        }
        if (pool_histo_setups.size()) {
          datatools::properties hs_config;
          if (_params_.debug) {
            hs_config.store_string("logging.priority", "debug");
          }
          std::ostringstream hpdesc_oss;
          hpdesc_oss << "GENBB inspector histograms";
          if (!_params_.title_prefix.empty()) {
            hpdesc_oss << " -- " << _params_.title_prefix;
          }
          hs_config.store_string("pool.description", hpdesc_oss.str());
          hs_config.store("pool.histo.setups", pool_histo_setups);
          hs_config.store_flag("root_export.stats");
          hs_config.store("root_export.title_prefix", _title_prefix_);
          hs_config.store("root_export.name_suffix", _name_suffix_);
          hs_config.store("output_files", _params_.output_paths);
          std::vector<std::string> pool_export_prefixes;
          pool_export_prefixes.push_back("value.");
          hs_config.store("pool.histo.export_prefixes", pool_export_prefixes);
          //std::cerr << "DEVEL: initializing HS...\n";
          //hs_config.tree_dump(std::cerr, "Histogram service config : ", "DEVEL: ");
          //_histos_service_.tree_dump(std::cerr, "Histogram service: ", "DEVEL: ");
          //for (int i = 0; i < 10000; i++) std::cerr << std::flush;
          _histos_service_.reset(new dpp::histogram_service);
          _histos_service_->initialize_standalone(hs_config);
          _histos_ = &_histos_service_->grab_pool();
        }
      } // mode: "histograms"
    } // shoot
    _initialized_  = true;
    return;
  }

  void inspector::reset()
  {
    DT_THROW_IF (!_initialized_, std::logic_error, "Not initialized !");
    _initialized_  = false;
    _histos_ = 0;
    if (_plain_writer_) {
      if (_plain_writer_->is_initialized()) {
        _plain_writer_->reset();
      }
      _plain_writer_.reset();
    }
    if (_bank_writer_) {
      if (_bank_writer_->is_initialized()) {
        _bank_writer_->reset();
      }
      _bank_writer_.reset();
    }
    if (_histos_service_) {
      if (_histos_service_->is_initialized()) {
        _histos_service_->reset();
      }
      _histos_service_.reset();
    }
    _generator_ = 0;
    if (_manager_.is_initialized()) _manager_.reset();
    _params_.reset();
    return;
  }

  void inspector::run()
  {
    DT_THROW_IF (!_initialized_, std::logic_error, "Not initialized !");
    if (_params_.interactive) {
      _run_interactive();
    } else {
      if (_params_.debug) {
        DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "Running in batch mode...");
      }
      _run_batch();
    }
    return;
  }

  void inspector::_run_batch()
  {
    int count = 0;
    int max_count = _params_.number_of_events;
    if (max_count <= 0) {
      max_count = DEFAULT_MAX_NUMBER_OF_EVENTS;
    }

    if (_params_.action == "list") {
      std::string print_mode = _params_.list_print_mode;
      std::string title;
      if (print_mode == "tree") {
        title = "List of currently available primary particle event generators";
      }
      _manager_.print_particle_generators(std::cout, title, "", print_mode);
    }

    if (_params_.action == "shoot") {
      DT_THROW_IF(_generator_ == 0, std::logic_error, "Missing generator...");
      while (_generator_->has_next()) {
        if (_params_.trace_index > 0) {
          DT_TRACER_MESSAGE(_params_.trace_index,
                            "***************************************** Event " << count);
        }
        if (_prng_.has_tracker()) {
          _prng_.tracker_tag("Event", count);
        }
        genbb::primary_event decay_event;
        _generator_->load_next(decay_event, false);
        if (_params_.debug) decay_event.tree_dump(std::clog, "Decay event: ", "DEBUG: ");
        _inspect(decay_event);
        count++;
        const int modulo = (_params_.print_modulo > 0 ? _params_.print_modulo : 10);
        if (count == 1 || count % modulo == 0 || count == max_count) {
          DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Generated event #" << count);
        }
        if (count >= max_count) break;
      }
    }
    return;
  }

  void inspector::_inspect(const genbb::primary_event & event_)
  {
    if (_plain_writer_) {
      _inspect_plain_writer_mode(event_);
    } else if (_bank_writer_) {
      _inspect_bank_writer_mode(event_);
    } else if (_histos_service_) {
      _inspect_histograms_mode(event_);
    }
    return;
  }

  void inspector::_inspect_plain_writer_mode(const genbb::primary_event & event_)
  {
    _plain_writer_->store(event_);
    return;
  }

  void inspector::_inspect_bank_writer_mode(const genbb::primary_event & event_)
  {
    datatools::things record;
    genbb::primary_event & PE = record.add<genbb::primary_event>(_params_.output_bank_label);
    PE = event_;
    int status = _bank_writer_->process(record);
    if (status != dpp::base_module::PROCESS_OK) {
      DT_THROW_IF(true, std::logic_error,
                  "An error occured while saving the primary event !");
    }
    return;
  }

  void inspector::_inspect_histograms_mode(const genbb::primary_event & event_)
  {
    static const std::string time_range_label[2] = { "prompt", "delayed" };
    inspector_data timing_data[2];

    // Reflection meta-class :
    const camp::Class & metaClass = camp::classByName("genbb::inspector_data");

    // Extract data from the generated event :
    for (int timing = PROMPT; timing <= DELAYED; timing++) {
      double time_min = 0.0 * CLHEP::ns;
      double time_max = _params_.prompt_time_limit * CLHEP::ns;
      if (timing == PROMPT) {
        if (! _params_.prompt) continue;
      }
      if (timing == DELAYED) {
        if (! _params_.delayed) continue;
        time_min = _params_.prompt_time_limit * CLHEP::ns;
        datatools::invalidate(time_max);
      }
      // Inspector data for the current timeng :
      inspector_data & idata = timing_data[timing];

      // Loop on particles :
      double time = 0.0; //event_.get_time();
      for (genbb::primary_event::particles_col_type::const_iterator i
             = event_.get_particles().begin();
           i != event_.get_particles().end();
           i++) {
        const genbb::primary_particle & pp = *i;
        time += pp.get_time ();

        // Select the time range :
        if (datatools::is_valid(time_min)
            && (time < time_min)) continue;
        if (datatools::is_valid(time_max)
            && (time >= time_max)) continue;

        // Analyze the particles :
        idata.number_of_particles = event_.get_particles().size();
        double ke = pp.get_kinetic_energy ();
        idata.energy_sum += ke;
        if (std::abs(pp.get_charge()) > 0.9) {
          idata.number_of_charged_particles++;
          idata.charged_particles_energy_sum += ke;
        }
        if (pp.is_gamma()) {
          idata.number_of_gammas++;
          idata.gammas_energies.push_back(ke);
          idata.gammas_times.push_back(time);
          idata.gammas_momenta.push_back(pp.get_momentum ());
        } else if (pp.is_electron()) {
          idata.number_of_electrons++;
          idata.electrons_energies.push_back(ke);
          idata.electrons_times.push_back(time);
          idata.electrons_momenta.push_back(pp.get_momentum ());
        } else if (pp.is_positron()) {
          idata.number_of_positrons++;
          idata.positrons_energies.push_back(ke);
          idata.positrons_times.push_back(time);
          idata.positrons_momenta.push_back(pp.get_momentum ());
        } else if (pp.is_alpha()) {
          idata.number_of_alphas++;
          idata.alphas_energies.push_back(ke);
          idata.alphas_times.push_back(time);
          idata.alphas_momenta.push_back(pp.get_momentum ());
        } else if (pp.is_neutron()) {
          idata.number_of_neutrons++;
          idata.neutrons_energies.push_back(ke);
          idata.neutrons_times.push_back(time);
          idata.neutrons_momenta.push_back(pp.get_momentum ());
        } else if (pp.is_proton()) {
          idata.number_of_protons++;
          idata.protons_energies.push_back(ke);
          idata.protons_times.push_back(time);
          idata.protons_momenta.push_back(pp.get_momentum ());
        } else if (pp.is_ion() || pp.is_nucleus()) {
          idata.number_of_ions++;
          idata.ions_energies.push_back(ke);
          idata.ions_times.push_back(time);
          idata.ions_momenta.push_back(pp.get_momentum ());
        } else if (pp.is_muon_minus()) {
          idata.number_of_muons_minus++;
          idata.muons_minus_energies.push_back(ke);
          idata.muons_minus_times.push_back(time);
          idata.muons_minus_momenta.push_back(pp.get_momentum ());
        } else if (pp.is_muon_plus()) {
          idata.number_of_muons_plus++;
          idata.muons_plus_energies.push_back(ke);
          idata.muons_plus_times.push_back(time);
          idata.muons_plus_momenta.push_back(pp.get_momentum ());
        } else if (std::abs(pp.get_charge()) < 0.1) {
          idata.number_of_other_neutral_particles++;
          idata.other_neutral_particles_energies.push_back(ke);
          idata.other_neutral_particles_times.push_back(time);
          idata.other_neutral_particles_momenta.push_back(pp.get_momentum ());
        } else if (pp.get_charge() > 0.9) {
          idata.number_of_other_positive_particles++;
          idata.other_positive_particles_energies.push_back(ke);
          idata.other_positive_particles_times.push_back(time);
          idata.other_positive_particles_momenta.push_back(pp.get_momentum ());
        } else if (pp.get_charge() < -0.9) {
          idata.number_of_other_negative_particles++;
          idata.other_negative_particles_energies.push_back(ke);
          idata.other_negative_particles_times.push_back(time);
          idata.other_negative_particles_momenta.push_back(pp.get_momentum ());
        }
      }

      // Fetch the names of the histogram in the current 'timing' group :
      std::vector<std::string> histo_names;
      _histos_->names (histo_names, std::string("group=") + time_range_label[timing]);
      camp::UserObject idObj = idata;

      for (int ihn = 0; ihn < (int) histo_names.size(); ihn++) {
        const std::string & hn = histo_names[ihn];
        /*
          std::cerr << "DEVEL: " << "genbb::inspector::_inspect: "
          << "Histogram's name = '" << hn << "' "
          << "and TRM='" << time_range_label[timing] << "'"
          << std::endl;
        */
        if (_histos_->get_group(hn) != time_range_label[timing]) continue;

        // 1D-histogram :
        if (_histos_->has_1d(hn)) {
          mygsl::histogram_1d & h1 = _histos_->grab_1d(hn);
          const datatools::properties & h1_aux = h1.get_auxiliaries();
          //h1_aux.tree_dump(std::cerr, "H1 AUX : ", "DEVEL: ");
          if (h1_aux.has_key("value.1d.accessor")) {
            std::vector<char> value_accessor_array;
            int value_index = -1;
            int value_index2 = -1;
            std::vector<char> particle_type1_array;
            std::vector<char> particle_type2_array;
            std::string value_rule;
            value_rule = h1_aux.fetch_string("value.1d.accessor");
            // boost::fusion::vector<std::string,int> fv;
            // boost::fusion::at_c<0>(fv) = std::string("");
            // boost::fusion::at_c<1>(fv) = -1;

            std::string::iterator first = value_rule.begin();
            std::string::iterator last = value_rule.end();
            bool result
              = boost::spirit::qi::phrase_parse(first,
                                                last,
                                                //  Begin grammar
                                                (
                                                 boost::spirit::lexeme[+(boost::spirit::ascii::char_ - '(')]
                                                 [boost::phoenix::ref(value_accessor_array) = boost::spirit::qi::_1]
                                                 >> '('
                                                 >> boost::spirit::lexeme[+(boost::spirit::ascii::char_ - ',')]
                                                 [boost::phoenix::ref(particle_type1_array) = boost::spirit::qi::_1]
                                                 >> ','
                                                 >> boost::spirit::qi::int_
                                                 [boost::phoenix::ref(value_index) = boost::spirit::qi::_1]
                                                 >> ','
                                                 >> boost::spirit::lexeme[+(boost::spirit::ascii::char_ - ',')]
                                                 [boost::phoenix::ref(particle_type2_array) = boost::spirit::qi::_1]
                                                 >> ','
                                                 >> boost::spirit::qi::int_
                                                 [boost::phoenix::ref(value_index2) = boost::spirit::qi::_1]
                                                 >> ')'
                                                 |
                                                 boost::spirit::lexeme[+(boost::spirit::ascii::char_ - '[')]
                                                 [boost::phoenix::ref(value_accessor_array) = boost::spirit::qi::_1]
                                                 >> '['
                                                 >> boost::spirit::qi::int_
                                                 [boost::phoenix::ref(value_index) = boost::spirit::qi::_1]
                                                 >> ']'
                                                 |
                                                 boost::spirit::lexeme[+(boost::spirit::ascii::char_ - '[')]
                                                 [boost::phoenix::ref(value_accessor_array) = boost::spirit::qi::_1]
                                                 ),
                                                //  End grammar
                                                boost::spirit::ascii::space
                                                );
            DT_THROW_IF (!result || first != last,
                         std::logic_error,
                         "Parsing failed for '" << value_rule << "' !");
            std::string value_accessor(value_accessor_array.begin(),
                                       value_accessor_array.end());
            // value_accessor  = boost::fusion::at_c<0>(fv);
            // value_index = boost::fusion::at_c<1>(fv);
            /*
              std::cerr << "DEVEL: " << "genbb::inspector::_inspect: "
              << "Histogram's value='" << value_accessor << "'"
              << " @ " << "index=" << value_index
              << std::endl;
            */
            camp::Value val;
            if (value_index < 0) {
              if (metaClass.hasProperty(value_accessor)) {
                val = idObj.get(value_accessor);
              } else {
                DT_THROW_IF (true, std::logic_error,
                             "Cannot find property named '" << value_accessor << "' in meta class !");
              }
            } else if (value_index2 < 0) {
              if (metaClass.hasFunction(value_accessor)) {
                val = idObj.call(value_accessor, camp::Args (value_index));
              } else {
                DT_THROW_IF (true, std::logic_error,
                             "Cannot find function named '" << value_accessor << "' in meta class !");
              }
            } else {
              if (metaClass.hasFunction(value_accessor)) {
                std::string particle_type1(particle_type1_array.begin(),
                                           particle_type1_array.end());
                std::string particle_type2(particle_type2_array.begin(),
                                           particle_type2_array.end());
                val = idObj.call(value_accessor, camp::Args (particle_type1, value_index,
                                                             particle_type2, value_index2));
                //std::cerr << "DEVEL: **************** val=" << val << std::endl;
              } else {
                DT_THROW_IF (true, std::logic_error,
                             "Cannot find function named '" << value_accessor << "' in meta class !");
              }
            }
            double value;
            datatools::invalidate(value);
            if (val.type() == camp::realType) {
              value = val.to<double>();
            } else if (val.type() == camp::intType) {
              int ival = val.to<int>();
              value = ival * 1.0;
            }
            /*
              std::cerr << "DEVEL: " << "genbb::inspector::_inspect: "
              << "Value = " << value
              << std::endl;
            */
            if (datatools::is_valid(value)) {
              h1.fill(value);
            }
          }
        } // Histo 1D

          // 2D-histogram :
        if (_histos_->has_2d(hn)) {
          mygsl::histogram_2d & h2 = _histos_->grab_2d(hn);
          const datatools::properties & h2_aux = h2.get_auxiliaries();
          std::vector<std::string> value_rules;
          if (h2_aux.has_key("value.2d.accessors")) {
            h2_aux.fetch("value.2d.accessors", value_rules);
            DT_THROW_IF (value_rules.size() != 2, std::logic_error,
                         "Invalid 'value.2d.names' parameter !");
            std::vector<char> x_value_accessor_array;
            std::vector<char> y_value_accessor_array;
            int x_value_index = -1;
            int y_value_index = -1;
            std::string x_value_rule = value_rules[0];
            std::string y_value_rule = value_rules[1];

            // X-axis :
            std::string::iterator x_first = x_value_rule.begin();
            std::string::iterator x_last = x_value_rule.end();
            bool x_result
              = boost::spirit::qi::phrase_parse(x_first,
                                                x_last,
                                                //  Begin grammar
                                                (
                                                 boost::spirit::lexeme[+(boost::spirit::ascii::char_ - '[')]
                                                 [boost::phoenix::ref(x_value_accessor_array) = boost::spirit::qi::_1]
                                                 >> '['
                                                 >> boost::spirit::qi::int_
                                                 [boost::phoenix::ref(x_value_index) = boost::spirit::qi::_1]
                                                 >> ']'
                                                 | boost::spirit::lexeme[+(boost::spirit::ascii::char_ - '[')]
                                                 [boost::phoenix::ref(x_value_accessor_array) = boost::spirit::qi::_1]
                                                 ),
                                                //  End grammar
                                                boost::spirit::ascii::space
                                                );
            DT_THROW_IF (! x_result || x_first != x_last,
                         std::logic_error,
                         "Parsing failed for '" << x_value_rule << "' !");
            std::string x_value_accessor(x_value_accessor_array.begin(),
                                         x_value_accessor_array.end());

            // Y-axis :
            std::string::iterator y_first = y_value_rule.begin();
            std::string::iterator y_last = y_value_rule.end();
            bool y_result
              = boost::spirit::qi::phrase_parse(y_first,
                                                y_last,
                                                //  Begin grammar
                                                (
                                                 boost::spirit::lexeme[+(boost::spirit::ascii::char_ - '[')]
                                                 [boost::phoenix::ref(y_value_accessor_array) = boost::spirit::qi::_1]
                                                 >> '['
                                                 >> boost::spirit::qi::int_
                                                 [boost::phoenix::ref(y_value_index) = boost::spirit::qi::_1]
                                                 >> ']'
                                                 | boost::spirit::lexeme[+(boost::spirit::ascii::char_ - '[')]
                                                 [boost::phoenix::ref(y_value_accessor_array) = boost::spirit::qi::_1]
                                                 ),
                                                //  End grammar
                                                boost::spirit::ascii::space
                                                );
            DT_THROW_IF (! y_result || y_first != y_last,
                         std::logic_error,
                         "Parsing failed for '" << y_value_rule << "' !");
            std::string y_value_accessor(y_value_accessor_array.begin(),
                                         y_value_accessor_array.end());
            // Extract X and Y values :
            double x_value, y_value;
            datatools::invalidate(x_value);
            datatools::invalidate(y_value);
            camp::Value x_val;
            if (x_value_index < 0) {
              if (metaClass.hasProperty(x_value_accessor)) {
                x_val = idObj.get(x_value_accessor);
              } else {
                DT_THROW_IF (true, std::logic_error,
                             "Cannot find property named '" << x_value_accessor << "' in meta class !");
              }
            } else {
              if (metaClass.hasFunction(x_value_accessor)) {
                x_val = idObj.call(x_value_accessor, camp::Args (x_value_index));
              } else {
                DT_THROW_IF (true, std::logic_error,
                             "Cannot find function named '" << x_value_accessor << "' in meta class !");
              }
            }
            if (x_val.type() == camp::realType) {
              x_value = x_val.to<double>();
            } else if (x_val.type() == camp::intType) {
              int ival = x_val.to<int>();
              x_value = ival * 1.0;
            }
            camp::Value y_val;
            if (y_value_index < 0) {
              if (metaClass.hasProperty(y_value_accessor)) {
                y_val = idObj.get(y_value_accessor);
              } else {
                DT_THROW_IF (true, std::logic_error,
                             "Cannot find property named '" << y_value_accessor << "' in meta class !");
              }
            } else {
              if (metaClass.hasFunction(y_value_accessor)) {
                y_val = idObj.call(y_value_accessor, camp::Args (y_value_index));
              } else {
                DT_THROW_IF (true, std::logic_error,
                             "Cannot find property named '" << y_value_accessor << "' in meta class !");
              }
            }
            if (y_val.type() == camp::realType) {
              y_value = y_val.to<double>();
            } else if (y_val.type() == camp::intType) {
              int ival = y_val.to<int>();
              y_value = ival * 1.0;
            }

            if (datatools::is_valid(x_value) && datatools::is_valid(y_value)) {
              h2.fill(x_value, y_value);
            }
          }
        } // Histo 2D
      }
    }
    return;
  }

  void inspector::_run_interactive()
  {
    return;
  }

}

uint32_t app_kernel_init_flags()
{
  uint32_t kernel_init_flags = 0;
  kernel_init_flags |= ::datatools::init_no_help;
  kernel_init_flags |= ::datatools::init_no_splash;
  kernel_init_flags |= ::datatools::init_no_inhibit_libinfo;
  kernel_init_flags |= ::datatools::init_no_libinfo_logging;
  kernel_init_flags |= ::datatools::init_no_variant;
  kernel_init_flags |= ::datatools::init_no_inhibit_variant;
  kernel_init_flags |= ::datatools::init_no_locale_category;
  kernel_init_flags |= ::datatools::init_no_inhibit_qt_gui;
  kernel_init_flags |= ::datatools::init_no_inhibit_urnquery;
  return kernel_init_flags;
}

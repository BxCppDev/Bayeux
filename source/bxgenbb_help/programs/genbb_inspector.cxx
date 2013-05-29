// -*- mode: c++; -*-
// genbb_inspector.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <numeric>

#include <boost/program_options.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/at_c.hpp>

#include <datatools/datatools_config.h>
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/units.h>
#if DATATOOLS_WITH_REFLECTION == 1
#warning Found reflection support in datatools
#include <datatools/reflection_macros.h>
#include <datatools/reflection_guard.h>
#else
#error No reflection support in datatools
#endif
#include <datatools/tracer.h>

#include <mygsl/histogram_service.h>
#include <mygsl/histogram_pool.h>
#include <mygsl/histogram_1d.h>
#include <mygsl/histogram_2d.h>

#include <genbb_help/genbb_help_config.h>
#include <genbb_help/manager.h>
#include <genbb_help/primary_event.h>

// Some pre-processor guard about Boost I/O usage and linkage :
#include <datatools/bio_guard.h>
#include <geomtools/bio_guard.h>
#include <genbb_help/bio_guard.h>

namespace genbb {

  /// \brief Configuration parameters for the inspector of GENBB primary event generator
  struct inspector_params
  {
    inspector_params();
    void reset();
    void dump(std::ostream & = std::clog) const;
    bool debug; /// Debug flag
    int trace_index;
    bool interactive; /// Interactive flag
    std::vector<std::string> unrecognized_options; /// Unrecognized options
    std::string configuration; /// Particle generator manager configuration file
    std::string generator; /// Particle generator name
    std::vector<std::string> output_paths;
    int number_of_events; /// Number of generated events
    int prng_seed; /// PRNG seed
    int prng_trunc; /// PRNG hack
    std::string prng_tracker; /// PRNG tracker output file
    double prompt_time_limit; // Prompt time limit (in ns)
    std::vector<std::string> histos_definitions; /// Histograms' definition files
    bool prompt; /// Flag to analyze prompt event
    bool delayed; /// Flag to analyze delayed event
    std::string title_prefix; /// User title prefix for histograms
    std::string name_suffix; /// User name suffix for histograms
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
    out_ << "|-- generator          = " << generator << std::endl;
    out_ << "|-- output_paths       = " << output_paths.size() << std::endl;
    out_ << "|-- number_of_events   = " << number_of_events << std::endl;
    out_ << "|-- prng_seed          = " << prng_seed << std::endl;
    out_ << "|-- prng_trunc         = " << prng_trunc << std::endl;
    out_ << "|-- prng_tracker       = " << prng_tracker << std::endl;
    out_ << "|-- prompt_time_limit  = " << prompt_time_limit << std::endl;
    out_ << "|-- histos_definitions = " << histos_definitions.size() << std::endl;
    out_ << "|-- prompt             = " << prompt << std::endl;
    out_ << "|-- delayed            = " << delayed << std::endl;
    out_ << "|-- title_prefix       = " << title_prefix << std::endl;
    out_ << "`-- name_suffix        = " << name_suffix << std::endl;
    return;
  }

  void inspector_params::reset()
  {
    debug = false;
    trace_index = 0;
    interactive = false;
    unrecognized_options.clear();
    output_paths.clear();
    number_of_events = -1;
    prng_seed = 0;
    prng_trunc = 0;
    prng_tracker.clear();
    prompt_time_limit = 1.0;
    histos_definitions.clear();
    prompt = true;
    delayed = false;
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
    void reset();
    void sort();

    double get_electron_energy(int i_) const;
    double get_positron_energy(int i_) const;
    double get_gamma_energy(int i_) const;
    double get_alpha_energy(int i_) const;
    double get_neutron_energy(int i_) const;
    double get_other_neutral_particle_energy(int i_) const;
    double get_other_positive_particle_energy(int i_) const;
    double get_other_negative_particle_energy(int i_) const;

    double get_electrons_energy_sum() const;
    double get_positrons_energy_sum() const;
    double get_gammas_energy_sum() const;
    double get_alphas_energy_sum() const;
    double get_neutrons_energy_sum() const;
    double get_other_neutral_particles_energy_sum() const;
    double get_other_positive_particles_energy_sum() const;
    double get_other_negative_particles_energy_sum() const;

    double get_electron_time(int i_) const;
    double get_positron_time(int i_) const;
    double get_gamma_time(int i_) const;
    double get_alpha_time(int i_) const;
    double get_neutron_time(int i_) const;
    double get_other_neutral_particle_time(int i_) const;
    double get_other_positive_particle_time(int i_) const;
    double get_other_negative_particle_time(int i_) const;

    double get_electron_phi(int i_) const;
    double get_positron_phi(int i_) const;
    double get_gamma_phi(int i_) const;
    double get_alpha_phi(int i_) const;
    double get_neutron_phi(int i_) const;
    double get_other_neutral_particle_phi(int i_) const;
    double get_other_positive_particle_phi(int i_) const;
    double get_other_negative_particle_phi(int i_) const;

    double get_electron_cos_theta(int i_) const;
    double get_positron_cos_theta(int i_) const;
    double get_gamma_cos_theta(int i_) const;
    double get_alpha_cos_theta(int i_) const;
    double get_neutron_cos_theta(int i_) const;
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
    int number_of_other_neutral_particles;
    int number_of_other_positive_particles;
    int number_of_other_negative_particles;

    std::vector<double> electrons_energies;
    std::vector<double> positrons_energies;
    std::vector<double> gammas_energies;
    std::vector<double> alphas_energies;
    std::vector<double> neutrons_energies;
    std::vector<double> other_neutral_particles_energies;
    std::vector<double> other_positive_particles_energies;
    std::vector<double> other_negative_particles_energies;

    std::vector<double> electrons_times;
    std::vector<double> positrons_times;
    std::vector<double> gammas_times;
    std::vector<double> alphas_times;
    std::vector<double> neutrons_times;
    std::vector<double> other_neutral_particles_times;
    std::vector<double> other_positive_particles_times;
    std::vector<double> other_negative_particles_times;

    std::vector<geomtools::vector_3d> electrons_momenta;
    std::vector<geomtools::vector_3d> positrons_momenta;
    std::vector<geomtools::vector_3d> gammas_momenta;
    std::vector<geomtools::vector_3d> alphas_momenta;
    std::vector<geomtools::vector_3d> neutrons_momenta;
    std::vector<geomtools::vector_3d> other_neutral_particles_momenta;
    std::vector<geomtools::vector_3d> other_positive_particles_momenta;
    std::vector<geomtools::vector_3d> other_negative_particles_momenta;

  private:
    geomtools::vector_3d _invalid_vector_;

    //! Reflection interface
    DR_CLASS_RTTI();
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
     other_neutral_particles_energies.clear();
     other_positive_particles_energies.clear();
     other_negative_particles_energies.clear();

     electrons_times.clear();
     positrons_times.clear();
     gammas_times.clear();
     alphas_times.clear();
     neutrons_times.clear();
     other_neutral_particles_times.clear();
     other_positive_particles_times.clear();
     other_negative_particles_times.clear();

     electrons_momenta.clear();
     positrons_momenta.clear();
     gammas_momenta.clear();
     alphas_momenta.clear();
     neutrons_momenta.clear();
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

  /***/

  double inspector_data::get_cos_theta(const std::string & type1_, int i1_,
                                       const std::string & type2_, int i2_) const
  {
    const geomtools::vector_3d *p1 = 0;
    const geomtools::vector_3d *p2 = 0;
    double cost12 = std::numeric_limits<double>::quiet_NaN();
    int i1max = -1;
    int i2max = -1;
    if (type1_ == "electron") i1max = electrons_momenta.size();
    if (type1_ == "positron") i1max = positrons_momenta.size();
    if (type1_ == "gamma") i1max = gammas_momenta.size();
    if (type1_ == "alpha") i1max = alphas_momenta.size();
    if (type1_ == "neutron") i1max = neutrons_momenta.size();
    if (type1_ == "other_neutral") i1max = other_neutral_particles_momenta.size();
    if (type1_ == "other_positive") i1max = other_positive_particles_momenta.size();
    if (type1_ == "other_negative") i1max = other_negative_particles_momenta.size();

    if (type2_ == "electron") i2max = electrons_momenta.size();
    if (type2_ == "positron") i2max = positrons_momenta.size();
    if (type2_ == "gamma") i2max = gammas_momenta.size();
    if (type2_ == "alpha") i2max = alphas_momenta.size();
    if (type2_ == "neutron") i2max = neutrons_momenta.size();
    if (type2_ == "other_neutral") i2max = other_neutral_particles_momenta.size();
    if (type2_ == "other_positive") i2max = other_positive_particles_momenta.size();
    if (type2_ == "other_negative") i2max = other_negative_particles_momenta.size();

    if (i1_ >= 0 && i2_ >= 0 && i1_ < i1max && i2_ < i2max) {

      if (type1_ == "electron") p1 = &electrons_momenta[i1_];
      if (type1_ == "positron") p1 = &positrons_momenta[i1_];
      if (type1_ == "gamma") p1 = &gammas_momenta[i1_];
      if (type1_ == "alpha") p1 = &alphas_momenta[i1_];
      if (type1_ == "neutron") p1 = &neutrons_momenta[i1_];
      if (type1_ == "other_neutral") p1 = &other_neutral_particles_momenta[i1_];
      if (type1_ == "other_positive") p1 = &other_positive_particles_momenta[i1_];
      if (type1_ == "other_negative") p1 = &other_negative_particles_momenta[i1_];

      if (type2_ == "electron") p2 = &electrons_momenta[i2_];
      if (type2_ == "positron") p2 = &positrons_momenta[i2_];
      if (type2_ == "gamma") p2 = &gammas_momenta[i2_];
      if (type2_ == "alpha") p2 = &alphas_momenta[i2_];
      if (type2_ == "neutron") p2 = &neutrons_momenta[i2_];
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
    if ( i_ < 0 || i_ >= electrons_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return electrons_momenta[i_].phi();
  }

  double inspector_data::get_positron_phi(int i_) const
  {
    if ( i_ < 0 || i_ >= positrons_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return positrons_momenta[i_].phi();
  }

  double inspector_data::get_gamma_phi(int i_) const
  {
    if ( i_ < 0 || i_ >= gammas_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return gammas_momenta[i_].phi();
  }

  double inspector_data::get_alpha_phi(int i_) const
  {
    if ( i_ < 0 || i_ >= alphas_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return alphas_momenta[i_].phi();
  }

  double inspector_data::get_neutron_phi(int i_) const
  {
    if ( i_ < 0 || i_ >= neutrons_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return neutrons_momenta[i_].phi();
  }

  double inspector_data::get_other_neutral_particle_phi(int i_) const
  {
    if ( i_ < 0 || i_ >= other_neutral_particles_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return other_neutral_particles_momenta[i_].phi();
  }

  double inspector_data::get_other_positive_particle_phi(int i_) const
  {
    if ( i_ < 0 || i_ >= other_positive_particles_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return other_positive_particles_momenta[i_].phi();
  }

  double inspector_data::get_other_negative_particle_phi(int i_) const
  {
    if ( i_ < 0 || i_ >= other_positive_particles_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return other_positive_particles_momenta[i_].phi();
  }

  /***/


  double inspector_data::get_electron_cos_theta(int i_) const
  {
    if ( i_ < 0 || i_ >= electrons_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return electrons_momenta[i_].cosTheta();
  }

  double inspector_data::get_positron_cos_theta(int i_) const
  {
    if ( i_ < 0 || i_ >= positrons_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return positrons_momenta[i_].cosTheta();
  }

  double inspector_data::get_gamma_cos_theta(int i_) const
  {
    if ( i_ < 0 || i_ >= gammas_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return gammas_momenta[i_].cosTheta();
  }

  double inspector_data::get_alpha_cos_theta(int i_) const
  {
    if ( i_ < 0 || i_ >= alphas_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return alphas_momenta[i_].cosTheta();
  }

  double inspector_data::get_neutron_cos_theta(int i_) const
  {
    if ( i_ < 0 || i_ >= neutrons_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return neutrons_momenta[i_].cosTheta();
  }

  double inspector_data::get_other_neutral_particle_cos_theta(int i_) const
  {
    if ( i_ < 0 || i_ >= other_neutral_particles_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return other_neutral_particles_momenta[i_].cosTheta();
  }

  double inspector_data::get_other_positive_particle_cos_theta(int i_) const
  {
    if ( i_ < 0 || i_ >= other_positive_particles_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return other_positive_particles_momenta[i_].cosTheta();
  }

  double inspector_data::get_other_negative_particle_cos_theta(int i_) const
  {
    if ( i_ < 0 || i_ >= other_positive_particles_momenta.size()) return std::numeric_limits<double>::quiet_NaN();
    return other_positive_particles_momenta[i_].cosTheta();
  }

  /***/
  double inspector_data::get_electron_energy(int i_) const
  {
    if ( i_ < 0 || i_ >= electrons_energies.size()) return std::numeric_limits<double>::quiet_NaN();
    return electrons_energies[i_];
  }

  double inspector_data::get_positron_energy(int i_) const
  {
    if ( i_ < 0 || i_ >= positrons_energies.size()) return std::numeric_limits<double>::quiet_NaN();
    return positrons_energies[i_];
  }

  double inspector_data::get_gamma_energy(int i_) const
  {
    if ( i_ < 0 || i_ >= gammas_energies.size()) return std::numeric_limits<double>::quiet_NaN();
    return gammas_energies[i_];
  }

  double inspector_data::get_alpha_energy(int i_) const
  {
    if ( i_ < 0 || i_ >= alphas_energies.size()) return std::numeric_limits<double>::quiet_NaN();
    return alphas_energies[i_];
  }

  double inspector_data::get_neutron_energy(int i_) const
  {
    if ( i_ < 0 || i_ >= neutrons_energies.size()) return std::numeric_limits<double>::quiet_NaN();
    return neutrons_energies[i_];
  }

  double inspector_data::get_other_neutral_particle_energy(int i_) const
  {
    if ( i_ < 0 || i_ >= other_neutral_particles_energies.size()) return std::numeric_limits<double>::quiet_NaN();
    return other_neutral_particles_energies[i_];
  }

  double inspector_data::get_other_positive_particle_energy(int i_) const
  {
    if ( i_ < 0 || i_ >= other_positive_particles_energies.size()) return std::numeric_limits<double>::quiet_NaN();
    return other_positive_particles_energies[i_];
  }

  double inspector_data::get_other_negative_particle_energy(int i_) const
  {
    if ( i_ < 0 || i_ >= other_positive_particles_energies.size()) return std::numeric_limits<double>::quiet_NaN();
    return other_positive_particles_energies[i_];
  }

  double inspector_data::get_electron_time(int i_) const
  {
    if ( i_ < 0 || i_ >= electrons_times.size()) return std::numeric_limits<double>::quiet_NaN();
    return electrons_times[i_];
  }

  double inspector_data::get_positron_time(int i_) const
  {
    if ( i_ < 0 || i_ >= positrons_times.size()) return std::numeric_limits<double>::quiet_NaN();
    return positrons_times[i_];
  }

  double inspector_data::get_gamma_time(int i_) const
  {
    if ( i_ < 0 || i_ >= gammas_times.size()) return std::numeric_limits<double>::quiet_NaN();
    return gammas_times[i_];
  }

  double inspector_data::get_alpha_time(int i_) const
  {
    if ( i_ < 0 || i_ >= alphas_times.size()) return std::numeric_limits<double>::quiet_NaN();
    return alphas_times[i_];
  }

  double inspector_data::get_neutron_time(int i_) const
  {
    if ( i_ < 0 || i_ >= neutrons_times.size()) return std::numeric_limits<double>::quiet_NaN();
    return neutrons_times[i_];
  }

  double inspector_data::get_other_neutral_particle_time(int i_) const
  {
    if ( i_ < 0 || i_ >= other_neutral_particles_times.size()) return std::numeric_limits<double>::quiet_NaN();
    return other_neutral_particles_times[i_];
  }

  double inspector_data::get_other_positive_particle_time(int i_) const
  {
    if ( i_ < 0 || i_ >= other_positive_particles_times.size()) return std::numeric_limits<double>::quiet_NaN();
    return other_positive_particles_times[i_];
  }

  double inspector_data::get_other_negative_particle_time(int i_) const
  {
    if ( i_ < 0 || i_ >= other_positive_particles_times.size()) return std::numeric_limits<double>::quiet_NaN();
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
    std::sort(other_neutral_particles_energies.begin(), other_neutral_particles_energies.end());
    std::sort(other_positive_particles_energies.begin(), other_positive_particles_energies.end());
    std::sort(other_negative_particles_energies.begin(), other_negative_particles_energies.end());

    std::reverse(gammas_energies.begin(), gammas_energies.end());
    std::reverse(electrons_energies.begin(), electrons_energies.end());
    std::reverse(positrons_energies.begin(), positrons_energies.end());
    std::reverse(alphas_energies.begin(), alphas_energies.end());
    std::reverse(neutrons_energies.begin(), neutrons_energies.end());
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

  private:
    bool _initialized_;
    genbb::inspector_params _params_;

    // Working attributes :
    mygsl::rng _prng_;
    genbb::manager _manager_;
    genbb::i_genbb * _generator_;
    mygsl::histogram_service _histos_service_;
    mygsl::histogram_pool * _histos_;
    std::string _title_prefix_;
    std::string _name_suffix_;

  };

} // namespace genbb


/************************************************************
 * Reflection support for the 'genbb::inspector_data' class *
 ************************************************************/

// Activate reflection layer for the 'genbb::inspector_data' class:
DR_CLASS_INIT(genbb::inspector_data);

// Export the introspection code for the 'genbb::inspector_data' class:
DR_CLASS_EXPORT (genbb::inspector_data, "genbb::inspector_data")

namespace datatools {
  namespace detail {
    namespace reflection {

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::genbb::inspector_data,tag_)
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

void usage (const boost::program_options::options_description &,
            std::ostream & = std::clog);

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  genbb::inspector_params params;
  namespace po = boost::program_options;
  po::options_description opts ("Allowed options ");
  po::positional_options_description args;
  try {

    opts.add_options ()

      ("help,h",
       "produce help message"
       )

      ("debug,d",
       po::value<bool>(&params.debug)
       ->zero_tokens()
       ->default_value (false),
       "produce debug logging"
       )

      ("interactive,I",
       po::value<bool>(&params.interactive)
       ->zero_tokens()
       ->default_value (false),
       "run in interactive mode (not implemented)"
       )

      ("number-of-events,n",
       po::value<int>(&params.number_of_events)
       ->default_value (0),
       "set the number of generated events. \n"
       "Example :                           \n"
       " --number-of-events 10000             "
       )

      ("trace-index,x",
       po::value<int>(&params.trace_index)
       ->default_value (0),
       "set the trace index. \n"
       "Example :                           \n"
       " --trace-index 10000                  "
       )

      ("prng-trunc,u",
       po::value<int>(&params.prng_trunc)
       ->default_value (0),
       "set the trunc index of the random number generator (PRNG). \n"
       "Example :                           \n"
       " --prng-trunc 7                     "
       )

      ("prng-seed,s",
       po::value<int>(&params.prng_seed)
       ->default_value (0),
       "set the seed of the random number generator (PRNG). \n"
       "Example :                                       \n"
       " --prng-seed 314159                               "
       )

      ("prng-tracker,K",
       po::value<std::string> (&params.prng_tracker),
       "set the PRNG tracker file. \n"
       "Example :                                   \n"
       " --prng-tracker \"genbb_inspector_prng.trk\"  "
       )

      ("configuration,c",
       po::value<std::string> (&params.configuration),
       "set the genbb manager configuration file. \n"
       "Example :                                 \n"
       " --configuration \"genbb.conf\"             "
       )

      ("generator,g",
       po::value<std::string> (&params.generator),
       "set the particle generator. \n"
       "Example :                   \n"
       " --generator \"Bi207\"        "
       )

      ("histo-def,H",
       po::value<std::vector<std::string> > (),
       "set the name of an histogram definition input filename."
       )

      ("output-file,o",
       po::value<std::vector<std::string> > (),
       "set the name of an output filename."
       )

      ("prompt-time-limit,t",
       po::value<double> (&params.prompt_time_limit),
       "set the limit on prompt time in ns."
       )

      ("prompt,P",
       po::value<bool>(&params.prompt)
       ->zero_tokens()
       ->default_value (true),
       "analyze prompt particles"
       )

      ("delayed,D",
       po::value<bool>(&params.delayed)
       ->zero_tokens()
       ->default_value (false),
       "analyze delayed particles"
       )

      ("title-prefix,T",
       po::value<std::string>(&params.title_prefix),
       "set a title prefix for exported histograms"
       )

      ("name-suffix,S",
       po::value<std::string>(&params.name_suffix),
       "set a name suffix for exported histograms"
       )

      ; // end of options description

    // Describe command line arguments :
    //args.add ("generator", 1);

    po::variables_map vm;
    po::parsed_options parsed =
      po::command_line_parser (argc_, argv_)
      .options (opts)
      .positional (args)
      .allow_unregistered()
      .run ();
    params.unrecognized_options = po::collect_unrecognized(parsed.options,
                                                           po::include_positional);
    po::store (parsed, vm);
    po::notify (vm);

    // Fetch the opts/args :
    if (vm.count ("help")) {
      usage(opts, std::cout);
      return error_code;
    }

    if (vm.count ("output-file")) {
      params.output_paths = vm["output-file"].as<std::vector<std::string> > ();
    }

    if (vm.count ("histo-def")) {
      params.histos_definitions = vm["histo-def"].as<std::vector<std::string> > ();
    }

    if (params.debug) {
      params.dump(std::clog);
    }
    genbb::inspector GI;
    GI.initialize(params);
    GI.run();
    GI.reset();

  }
  catch (std::exception & x) {
    std::cerr << datatools::io::error << "genbb_inspector: "
              << x.what () << std::endl;
    usage (opts, std::cerr);
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << datatools::io::error
              << "genbb_inspector: "
              << "Unexpected error !" << std::endl;
    usage (opts,std::cerr);
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

void usage (const boost::program_options::options_description & options_,
            std::ostream & out_)
{
  out_ << "\ngenbb_inspector -- Inspector for GENBB primary event generators" << std::endl;
  out_ << std::endl;
  out_ << "Usage : " << std::endl;
  out_ << std::endl;
  out_ << "  genbb_inspector [OPTIONS] [ARGUMENTS] " << std::endl << std::endl;
  out_ << options_ << std::endl;
  out_ << "Examples : " << std::endl;
  out_ << "" << std::endl;
  out_ << "  genbb_inspector --help\n";
  out_ << "" << std::endl;
  out_ << "  genbb_inspector \\\n";
  out_ << "    --configuration \""
       << GENBB_HELP_DATA_INSTALL_DIR
       << "/resources/manager/config/pro-1.0/manager.conf"
       << "\" \\\n";
  out_ << "    --generator \"Bi214_Po214\" \\\n";
  out_ << "    --prng-seed 314159 \\\n";
  out_ << "    --prng-tracker \"genbb_inspector_prng.trk\" \\\n";
  out_ << "    --number-of-events 100000 \\\n";
  out_ << "    --histo-def \""
       << GENBB_HELP_DATA_INSTALL_DIR
       << "/resources/inspector/config/le_nuphy-1.0/inspector_histos_prompt.conf"
       << "\" \\\n";
  out_ << "    --histo-def \""
       << GENBB_HELP_DATA_INSTALL_DIR
       << "/resources/inspector/config/le_nuphy-1.0/inspector_histos_delayed.conf"
       << "\" \\\n";
  out_ << "    --prompt-time-limit 1 \\\n";
  out_ << "    --prompt \\\n";
  out_ << "    --delayed \\\n";
  out_ << "    --title-prefix \"Bi214-Po214 (decay0 C++ port)\" \\\n";
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
    if (_initialized_) {
      throw std::logic_error("genbb::inspector::initialize: Already initialized !");
    }
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

    // Event generator manager :
    _manager_.set_debug(_params_.debug);
    _manager_.set_external_prng(_prng_);
    datatools::properties config;
    std::string config_filename = _params_.configuration;
    datatools::fetch_path_with_env(config_filename);
    datatools::properties::read_config(config_filename, config);
    _manager_.initialize(config);
    if (_params_.debug) {
      _manager_.dump_particle_generators(std::clog,
                                         "List of particle generators: ",
                                         "DEBUG: ");
    }
    if (_params_.generator.empty()) {
      throw std::logic_error("genbb::inspector::initialize: Missing generator name !");
    }
    if (! _manager_.has(_params_.generator)) {
      std::ostringstream message;
      message << "genbb::inspector::initialize: Generator named '"
              << _params_.generator << "' does not exist !";
      throw std::logic_error(message.str());
    }
    _generator_ = &_manager_.grab(_params_.generator);

    // Histograms :
    std::vector<std::string> pool_histo_setups;
    if (_params_.histos_definitions.size() == 0) {
      if (_params_.prompt) {
        std::ostringstream filename;
        filename << GENBB_HELP_DATA_INSTALL_DIR << '/'
                 << "resources/inspector/config/le_nuphy-1.0/inspector_histos_prompt.conf";
        _params_.histos_definitions.push_back(filename.str());
        std::clog << datatools::io::notice
                  << "genbb::inspector::initialize: "
                  << "Using default definition file for histograms : '"
                  << filename.str() << "'" << std::endl;
      }
      if (_params_.delayed) {
        std::ostringstream filename;
        filename << GENBB_HELP_DATA_INSTALL_DIR << '/'
                 << "resources/inspector/config/le_nuphy-1.0/inspector_histos_delayed.conf";
        _params_.histos_definitions.push_back(filename.str());
        std::clog << datatools::io::notice
                  << "genbb::inspector::initialize: "
                  << "Using default definition file for histograms : '"
                  << filename.str() << "'" << std::endl;
      }
    }
    for (int i = 0; i < _params_.histos_definitions.size(); i++) {
      std::string hconfig = _params_.histos_definitions[i];
      datatools::fetch_path_with_env(hconfig);
      pool_histo_setups.push_back(hconfig);
    }

    datatools::properties hs_config;
    hs_config.store_boolean("debug", _params_.debug);
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
    _histos_service_.initialize_standalone(hs_config);
    _histos_ = &_histos_service_.grab_pool();

    _initialized_  = true;
    return;
  }

  void inspector::reset()
  {
    if (!_initialized_) {
      throw std::logic_error("genbb::inspector::reset: Not initialized !");
    }
    _initialized_  = false;
    _params_.reset();

    return;
  }

  void inspector::run()
  {
    if (!_initialized_) {
      throw std::logic_error("genbb::inspector::reset: Not initialized !");
    }

    if (_params_.interactive) {
      _run_interactive();
    } else {
      if (_params_.debug) {
        std::clog << datatools::io::debug
                  << "genbb::inspector::reset: "
                  << "Running in batch mode..." << std::endl;
      }
      _run_batch();
    }

    if (_params_.debug) {
      std::clog << datatools::io::debug
                << "genbb::inspector::reset: "
                  << "End." << std::endl;
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
    while (_generator_->has_next()) {
      if (_params_.trace_index > 0) {
        DT_TRACER_MESSAGE(_params_.trace_index,
                          "***************************************** Event " << count);
      }
      _prng_.tracker_tag("Event", count);
      genbb::primary_event decay_event;
      _generator_->load_next(decay_event, false);
      if (_params_.debug) decay_event.tree_dump(std::clog, "Decay event: ", "DEBUG: ");
      _inspect(decay_event);
      count++;
      if ((count == 1) || (count % 5000) == 0 || count == max_count) {
        std::clog << "genbb::inspector::_run_batch: Generated event #" << count << '\n';
      }

      if (count >= max_count) break;
    }

    return;
  }

  void inspector::_inspect(const genbb::primary_event & event_)
  {
    /*
    std::cerr << "DEVEL: " << "genbb::inspector::_inspect: "
              << "Entering..." << std::endl;
    */
    static const std::string time_range_label[2] = { "prompt", "delayed" };
    inspector_data timing_data[2];

    // Reflection meta-class :
    const DR_CLASS & metaClass = DR_CLASS_BY_NAME("genbb::inspector_data");

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
        /*
        std::cerr << "DEVEL: " << "genbb::inspector::_inspect: "
                  << "ke=" << ke << std::endl;
        */
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

      //idata.sort();

      // Fetch the names of the histogram in the current 'timing' group :
      std::vector<std::string> histo_names;
      _histos_->names (histo_names, std::string("group=") + time_range_label[timing]);
      DR_OBJECT idObj = idata;

      for (int ihn = 0; ihn < histo_names.size(); ihn++) {
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
            if (!result || first != last) {
              // fail if we did not get a full match
              std::ostringstream message;
              message << "genbb::inspector::_inspect: "
                      << "Parsing failed for '" << value_rule << "'!";
              throw std::logic_error(message.str());
            }
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
            DR_VALUE val;
            if (value_index < 0) {
              if (metaClass.hasProperty(value_accessor)) {
                val = idObj.DR_GET(value_accessor);
              } else {
                std::ostringstream message;
                message << "genbb::inspector::_inspect: "
                        << "Cannot find property named '" << value_accessor << "' in meta class !";
                throw std::logic_error(message.str());
              }
            } else if (value_index2 < 0) {
              if (metaClass.hasFunction(value_accessor)) {
                val = idObj.DR_CALL(value_accessor, DR_ARGS (value_index));
              } else {
                std::ostringstream message;
                message << "genbb::inspector::_inspect: "
                        << "Cannot find function named '" << value_accessor << "' in meta class !";
                throw std::logic_error(message.str());
              }
            } else {
              if (metaClass.hasFunction(value_accessor)) {
                std::string particle_type1(particle_type1_array.begin(),
                                           particle_type1_array.end());
                std::string particle_type2(particle_type2_array.begin(),
                                           particle_type2_array.end());
                val = idObj.DR_CALL(value_accessor, DR_ARGS (particle_type1, value_index,
                                                             particle_type2, value_index2));
                //std::cerr << "DEVEL: **************** val=" << val << std::endl;
              } else {
                std::ostringstream message;
                message << "genbb::inspector::_inspect: "
                        << "Cannot find function named '" << value_accessor << "' in meta class !";
                throw std::logic_error(message.str());
              }
            }
            double value;
            datatools::invalidate(value);
            if (val.type() == DR_REAL) {
              value = val.to<double>();
            } else if (val.type() == DR_INTEGER) {
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
            if (value_rules.size() != 2) {
              throw std::logic_error("genbb::inspector::_inspect: Invalid 'value.2d.names' parameter !");
            }
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
            if (!x_result || x_first != x_last) {
              // fail if we did not get a full match
              std::ostringstream message;
              message << "genbb::inspector::_inspect: "
                      << "Parsing failed for '" << x_value_rule << "'!";
              throw std::logic_error(message.str());
            }
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
            if (!y_result || y_first != y_last) {
              // fail if we did not get a full match
              std::ostringstream message;
              message << "genbb::inspector::_inspect: "
                      << "Parsing failed for '" << y_value_rule << "'!";
              throw std::logic_error(message.str());
            }
            std::string y_value_accessor(y_value_accessor_array.begin(),
                                         y_value_accessor_array.end());
            // Extract X and Y values :
            double x_value, y_value;
            datatools::invalidate(x_value);
            datatools::invalidate(y_value);
            DR_VALUE x_val;
            if (x_value_index < 0) {
              if (metaClass.hasProperty(x_value_accessor)) {
                x_val = idObj.DR_GET(x_value_accessor);
              } else {
                std::ostringstream message;
                message << "genbb::inspector::_inspect: "
                        << "Cannot find property named '" << x_value_accessor << "' in meta class !";
                throw std::logic_error(message.str());
              }
            } else {
              if (metaClass.hasFunction(x_value_accessor)) {
                x_val = idObj.DR_CALL (x_value_accessor, DR_ARGS (x_value_index));
              } else {
                std::ostringstream message;
                message << "genbb::inspector::_inspect: "
                        << "Cannot find function named '" << x_value_accessor << "' in meta class !";
                throw std::logic_error(message.str());
              }
            }
            if (x_val.type() == DR_REAL) {
              x_value = x_val.to<double>();
            } else if (x_val.type() == DR_INTEGER) {
              int ival = x_val.to<int>();
              x_value = ival * 1.0;
            }
            DR_VALUE y_val;
            if (y_value_index < 0) {
              if (metaClass.hasProperty(y_value_accessor)) {
                y_val = idObj.DR_GET(y_value_accessor);
              } else {
                std::ostringstream message;
                message << "genbb::inspector::_inspect: "
                        << "Cannot find property named '" << y_value_accessor << "' in meta class !";
                throw std::logic_error(message.str());
              }
            } else {
              if (metaClass.hasFunction(y_value_accessor)) {
                y_val = idObj.DR_CALL (y_value_accessor, DR_ARGS (y_value_index));
              } else {
                std::ostringstream message;
                message << "genbb::inspector::_inspect: "
                        << "Cannot find property named '" << y_value_accessor << "' in meta class !";
                throw std::logic_error(message.str());
              }
            }
            if (y_val.type() == DR_REAL) {
              y_value = y_val.to<double>();
            } else if (y_val.type() == DR_INTEGER) {
              int ival = y_val.to<int>();
              y_value = ival * 1.0;
            }

            if (datatools::is_valid(x_value)
                && datatools::is_valid(y_value)) {
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

// end of genbb_inspector.cxx

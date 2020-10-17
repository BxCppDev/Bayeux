/// \file genbb_help/single_particle_generator.h
/* Author(s) :  Francois Mauger <mauger@lpccaen.in2p3.fr>
 *              Arnaud Chapon <chapon@lpccaen.in2p3.fr>
 * Creation date: 2010-10-03
 * Last modified: 2014-10-20
 *
 * License:
 * Copyright 2007-2014 F. Mauger
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
 *
 * Description:
 *
 *   A generator of GENBB-like event with a single particle
 *
 * History:
 *
 */

#ifndef GENBB_HELP_SINGLE_PARTICLE_GENERATOR_H
#define GENBB_HELP_SINGLE_PARTICLE_GENERATOR_H 1

// Standard library:
#include <string>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/units.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>
#include <mygsl/tabulated_function.h>
#include <mygsl/von_neumann_method.h>
#include <mygsl/histogram.h>
// - Bayeux/geomtools:
#include <geomtools/utils.h>

// This project:
#include <genbb_help/i_genbb.h>

namespace genbb {

  /// \brief Generator for single particles with some special kinematics
  class single_particle_generator : public i_genbb
  {
  public:

    enum mode_type {
      MODE_INVALID         = -1,
      MODE_MONOKINETIC     =  0,
      MODE_GAUSSIAN_ENERGY =  1,
      MODE_ENERGY_RANGE    =  2,
      MODE_MULTI_RAYS      =  3,
      MODE_SPECTRUM        =  4,
      MODE_DEFAULT         =  MODE_MONOKINETIC
    };

    enum spectrum_mode_type {
      SPECTRUM_MODE_TABFUNC = 0,
      SPECTRUM_MODE_HISTPDF = 1
    };

    enum direction_mode_type {
      DIRECTION_Z_AXIS     = 0,
      DIRECTION_RANDOMIZED = 1,
      DIRECTION_CONE       = 2,
      DIRECTION_DEFAULT    = DIRECTION_Z_AXIS
    };

    struct ion_data_type {
      int Z;        //!< Atomic number
      int A;        //!< Number of nucleons
      double Estar; //!< Excitation energy
      int Q;        //!< Ion charge
    };

    struct neutrino_data_type {
      std::string flavour; //!< Neutrino flavour ("electron", "muon", "tau")
      bool antineutrino;   //!< Neutrino/antineutrino flag
    };

    struct multi_rays_record_type {
      double energy;            //!< Monokinetic energy of the ray
      double probability;       //!< Probability of the ray
      double cumul_probability; //!< Cumulative probability
    };

  public:

    void set_direction_mode(int);
    int get_direction_mode() const;
    bool is_randomized_direction() const;
    void set_randomized_direction(bool);
    bool is_cone_direction() const;
    void set_cone_direction(bool);
    void set_cone_max_angle(double);
    double get_cone_max_angle() const;
    void set_cone_min_angle(double);
    double get_cone_min_angle() const;
    void set_cone_axis(const geomtools::vector_3d & axis_);
    void set_z_direction(bool);
    bool is_z_direction() const;
    const geomtools::vector_3d & get_cone_axis() const;

    const std::string & get_particle_name() const;
    void set_particle_name(const std::string &);
    double get_particle_mass() const;
    void set_particle_mass(double);

    bool can_external_random() const override;
    const mygsl::rng & get_random() const;
    mygsl::rng & grab_random();

    int get_mode() const;
    void set_mode(int);
    bool is_mode_valid() const;
    bool is_mode_monokinetic() const;
    bool is_mode_gaussian_energy() const;
    bool is_mode_energy_range() const;
    bool is_mode_multi_rays() const;
    bool is_mode_spectrum() const;

    double get_mean_energy() const;
    double get_sigma_energy() const;
    void set_mean_energy(double mean_, double sigma_ = 0.0);

    double get_min_energy() const;
    double get_max_energy() const;
    void set_energy_range(double min_, double max_);

    // void set_energy_list(const std::vector<double> en_);
    // void set_probability_list(const std::vector<double> prob_);
    void add_multi_ray(double energy_, double prob_);


    void set_energy_spectrum_filename(const std::string & filename_);

    /// Constructor
    single_particle_generator();

    /// Destructor
    ~single_particle_generator() override;

    /// Main initialization interface method
    void initialize(const datatools::properties & setup_,
                             datatools::service_manager & service_manager_,
                             detail::pg_dict_type & dictionary_) override;

    /// Reset
    void reset() override;

    /// Check if a next primary event is available
    bool has_next() override;

    /// Check initialization status
    bool is_initialized() const override;

    static double get_particle_mass_from_label(const std::string & particle_name_);

    static bool particle_name_is_valid(const std::string & particle_name_);

  protected:

    /// Shoot the primary event
    void _load_next(primary_event & event_,
                            bool compute_classification_ = true) override;

    /// Insitialize the energy spectrum
    void _init_energy_spectrum();

    /// Insitialize the energy PDF from an histogram
    void _init_energy_histo_pdf();

    /// Set default attributes' values
    void _set_defaults();

  private:

    void _at_init_();

    void _at_reset_();

  private:

    bool   _initialized_;        //!< Initialization flag
    int    _particle_type_;      //!< Generated particle type
    std::string _particle_name_; //!< Generated particle name
    boost::scoped_ptr<ion_data_type> _ion_data_; //!< Ion data
    boost::scoped_ptr<neutrino_data_type> _neutrino_data_; //!< Neutrino data
    double _particle_mass_; //!< Particle mass
    int    _mode_;          //!< Mode
    double _mean_energy_;   //!< Mean energy
    double _sigma_energy_;  //!< Energy sigma
    double _min_energy_;    //!< Minimum energy
    double _max_energy_;    //!< Maximum energy

    std::vector<multi_rays_record_type> _multi_rays_records_; //!< Records for the multi-ray mode

    int         _spectrum_mode_;
    std::string _spectrum_interpolation_name_;
    std::string _energy_spectrum_filename_;
    mygsl::tabulated_function _energy_spectrum_;
    mygsl::von_neumann_method _vnm_;
    mygsl::histogram          _energy_histo_;
    mygsl::histogram::pdf     _energy_histo_pdf_;

    int           _direction_mode_; //!< Mode for the direction of emission of the generated particle
    double        _cone_max_angle_; //!< Maximum angle of the cone of emission along the main emission axis
    double        _cone_min_angle_; //!< Minimum angle of the cone of emission along the main emission axis
    geomtools::vector_3d _cone_axis_;
    unsigned long _seed_;   //!< Local PRNG's seed
    boost::scoped_ptr<mygsl::rng> _random_; //!< Local PRNG

    GENBB_PG_REGISTRATION_INTERFACE(single_particle_generator)

  };

} // end of namespace genbb

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(genbb::single_particle_generator)

#endif // GENBB_HELP_SINGLE_PARTICLE_GENERATOR_H

// Local Variables: --
// mode: c++ --
// End: --

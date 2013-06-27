// -*- mode: c++ ; -*-
/* single_particle_generator.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-10-03
 * Last modified: 2013-02-26
 *
 * License:
 * Copyright 2007-2013 F. Mauger
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

#ifndef GENBB_HELP_SINGLE_PARTICLE_GENERATOR_H_
#define GENBB_HELP_SINGLE_PARTICLE_GENERATOR_H_ 1

#include <string>

#include <datatools/properties.h>
#include <datatools/units.h>

#include <mygsl/rng.h>
#include <mygsl/tabulated_function.h>
#include <mygsl/von_neumann_method.h>
#include <mygsl/histogram.h>

#include <geomtools/utils.h>

#include <genbb_help/i_genbb.h>

namespace genbb {

  /// Generator for single particles with some special kinematics
  class single_particle_generator : public i_genbb
  {
  public:

    enum mode_type
      {
        MODE_INVALID         = -1,
        MODE_MONOKINETIC     =  0,
        MODE_GAUSSIAN_ENERGY =  1,
        MODE_ENERGY_RANGE    =  2,
        MODE_SPECTRUM        =  3,
        MODE_DEFAULT         =  MODE_MONOKINETIC
      };

    enum spectrum_mode_type
      {
        SPECTRUM_MODE_TABFUNC = 0,
        SPECTRUM_MODE_HISTPDF = 1
      };

    enum direction_mode_type
      {
        DIRECTION_Z_AXIS     =  0,
        DIRECTION_RANDOMIZED =  1,
        DIRECTION_CONE       =  2,
        DIRECTION_DEFAULT    = DIRECTION_Z_AXIS
      };

  protected:
    void _check_locked (const std::string & where_) const;

  public:
    bool is_debug () const;
    void set_debug (bool);
    void set_direction_mode (int);
    int get_direction_mode () const;
    bool is_randomized_direction () const;
    void set_randomized_direction (bool);
    bool is_cone_direction () const;
    void set_cone_direction (bool);
    void set_cone_max_angle(double);
    double get_cone_max_angle() const;
    void set_cone_min_angle(double);
    double get_cone_min_angle() const;
    void set_cone_axis (const geomtools::vector_3d & axis_);
    void set_z_direction (bool);
    bool is_z_direction () const;
    const geomtools::vector_3d & get_cone_axis() const;

    const std::string & get_particle_name () const;
    void set_particle_name (const std::string &);
    double get_particle_mass () const;
    void set_particle_mass (double);

    virtual bool can_external_random () const;
    const mygsl::rng & get_random () const;
    mygsl::rng & grab_random ();

    int get_mode () const;
    void set_mode (int);

    double get_mean_energy () const;
    double get_sigma_energy () const;
    void set_mean_energy (double mean_, double sigma_ = 0.0);

    double get_min_energy () const;
    double get_max_energy () const;
    void set_energy_range (double min_, double max_);

    void set_energy_spectrum_filename (const std::string & filename_);

    /// Constructor
    single_particle_generator ();

    /// Destructor
    virtual ~single_particle_generator ();

    /// Main initialization interface method
    virtual void initialize (const datatools::properties & setup_,
                             datatools::service_manager & service_manager_,
                             detail::pg_dict_type & dictionary_);

    virtual void reset ();

    virtual bool has_next ();

    /// Check initialization status
    virtual bool is_initialized () const;

  protected:

    virtual void _load_next (primary_event & event_,
                             bool compute_classification_ = true);

    void _init_energy_spectrum ();

    void _init_energy_histo_pdf ();

  private:

    void _at_init_ ();

    void _at_reset_ ();

  public:

    static double get_particle_mass_from_label (const std::string & particle_name_);

    static bool particle_name_is_valid (const std::string & particle_name_);

  private:

    bool   _debug_;
    bool   _initialized_;
    int    _particle_type_;
    std::string _particle_name_;
    double _particle_mass_;
    int    _mode_;
    double _mean_energy_;
    double _sigma_energy_;
    double _min_energy_;
    double _max_energy_;

    int    _spectrum_mode_;
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
    mygsl::rng    _random_; //!< Local PRNG

    GENBB_PG_REGISTRATION_INTERFACE(single_particle_generator);

  };

} // end of namespace genbb

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(genbb::single_particle_generator)

#endif // GENBB_HELP_SINGLE_PARTICLE_GENERATOR_H_

// end of single_particle_generator.h

// -*- mode: c++ ; -*-
/* single_particle_generator.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-10-03
 * Last modified: 2010-10-03
 *
 * License:
 * Copyright 2007-2011 F. Mauger
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

#ifndef __genbb_help__single_particle_generator_h
#define __genbb_help__single_particle_generator_h 1

#include <string>

#include <genbb_help/i_genbb.h>

#include <mygsl/rng.h>
#include <mygsl/tabulated_function.h>
#include <mygsl/von_neumann_method.h>
#include <mygsl/histogram.h>

#include <datatools/utils/properties.h>

namespace genbb {

  using namespace std;

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

  protected:
    void _check_locked (const string & where_) const;

  public:
    bool is_debug () const;
    void set_debug (bool);
    bool is_initialized () const;
    bool is_randomized_direction () const;
    void set_randomized_direction (bool);
    const string & get_particle_name () const;
    void set_particle_name (const string &);
    double get_particle_mass () const;
    void set_particle_mass (double);
    const mygsl::rng & get_random () const;
    mygsl::rng & get_random ();

    int get_mode () const;
    void set_mode (int);

    double get_mean_energy () const;
    double get_sigma_energy () const;
    void set_mean_energy (double mean_, double sigma_ = 0.0);

    double get_min_energy () const;
    double get_max_energy () const;
    void set_energy_range (double min_, double max_);

    void set_energy_spectrum_filename (const string & filename_);

  protected:

    void _init_energy_spectrum ();

    void _init_energy_histo_pdf ();

  public:
    // ctor:
    single_particle_generator ();

    // dtor:
    virtual ~single_particle_generator ();

    void initialize (const datatools::utils::properties & config_);

    void reset ();

    virtual bool has_next ();

  protected:

    virtual void _load_next (primary_event & event_,
			     bool compute_classification_ = true) ;
  private:

    void _at_init_ ();

    void _at_reset_ ();

  public:

    static double get_particle_mass_from_label (const string & particle_name_);

    static bool particle_name_is_valid (const string & particle_name_);

  private:

    bool   _debug_;
    bool   _initialized_;
    int    _particle_type_;
    string _particle_name_;
    double _particle_mass_;
    int    _mode_;
    double _mean_energy_;
    double _sigma_energy_;
    double _min_energy_;
    double _max_energy_;

    int    _spectrum_mode_;
    string _spectrum_interpolation_name_;
    string _energy_spectrum_filename_;
    mygsl::tabulated_function _energy_spectrum_;
    mygsl::von_neumann_method _vnm_;
    mygsl::histogram          _energy_histo_;
    mygsl::histogram::pdf     _energy_histo_pdf_;

    bool          _randomized_direction_;
    unsigned long _seed_; //!< Local PRNG's seed
    mygsl::rng    _random_; //!< Local PRNG

  };

} // end of namespace genbb

#endif // __genbb_help__single_particle_generator_h

// end of single_particle_generator.h

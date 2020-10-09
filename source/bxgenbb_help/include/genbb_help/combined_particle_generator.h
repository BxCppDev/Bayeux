/// \file genbb_help/combined_particle_generator.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 *                Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date: 2010-10-03
 * Last modified: 2014-12-12
 *
 * License:
 * Copyright 2007-2014 F. Mauger, Xavier Garrido
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
 *   A generator of GENBB-like event with a combined particle
 *
 * History:
 *
 */

#ifndef GENBB_HELP_COMBINED_PARTICLE_GENERATOR_H
#define GENBB_HELP_COMBINED_PARTICLE_GENERATOR_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/units.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>
// - Bayeux/geomtools:
#include <geomtools/utils.h>

// This project:
#include <genbb_help/i_genbb.h>

namespace genbb {

  /// Generator for combined generators
  class combined_particle_generator
    : public i_genbb
  {
  public:

    /// Time delay mode
    enum time_delay_type {
      TIME_DELAY_UNDEFINED  = 0, //!< Undefined time delay mode
      TIME_DELAY_FIXED      = 1, //!< Fixed time delay
      TIME_DELAY_DECAY      = 2, //!< Random exponential time delay
      TIME_DELAY_RANGE      = 3, //!< Random ranged time delay
      TIME_DELAY_GAUSSIAN   = 4, //!< Gaussian ranged time delay
      TIME_DELAY_DEFAULT    = TIME_DELAY_FIXED
    };

    /// Particle generator entry
    struct entry_type
    {
      /// Constructor
      entry_type();
      /// Check validity
      bool is_valid() const;
      std::string label;    //!< Label
      std::string name;     //!< Name
      double    prob;       //!< Probability
      double    cumul_prob; //!< Cumulated probability
      time_delay_type time_mode; //!< Time delay mode
      double    time;       //!< Time delay constant
      double    time_width; //!< Time delay constant (only range or gaussian)
      i_genbb * pg;         //!< Reference to a particle generator
    };

    /// Collection of particle generator entry type
    typedef std::vector<entry_type> pg_col_type;

    /// Generation mode
    enum mode_type {
      MODE_UNDEFINED         = 0, //!< Undefined mode
      MODE_PLAIN_PROBABILITY = 1, //!< Branching ratios given by probability
      MODE_ACTIVITY          = 2, //!< Branching ratios given by absolute activity
      MODE_TIME              = 3  //!< Cascade of events from a time sequence
    };

    /// Set the generation mode
    void set_mode(int mode_);

    /// Check if generation mode is based on time generation
    bool is_mode_time() const;

    /// Check if generation mode is based on plain probability
    bool is_mode_plain_probability() const;

    /// Check if generation mode is based on activity
    bool is_mode_activity() const;

    /// Check existence of external random
    bool can_external_random() const override;

    /// Return a non-mutable random generator
    const mygsl::rng & get_random() const;

    /// Return a mutable random generator
    mygsl::rng & grab_random();

    /// Constructor
    combined_particle_generator();

    /// Destructor
    ~combined_particle_generator() override;

    /// Main initialization interface method
    void initialize(const datatools::properties & setup_,
                            datatools::service_manager & service_manager_,
                            detail::pg_dict_type & dictionary_) override;

    /// Reset
    void reset() override;

    /// Check next generation event
    bool has_next() override;

    /// Check initialization status
    bool is_initialized() const override;

    /// Smart dump
    void dump(std::ostream & out_, const std::string & title_, const std::string & indent_ = "") const;

  protected:

    /// Main generation function
    void _load_next(primary_event & event_,
                            bool compute_classification_ = true) override;

  private:

    /// Internal initialization method
    void _at_init_();

    /// Internal reset method
    void _at_reset_();

  private:

    bool   _initialized_;            //!< Initialization flag
    int    _mode_;                   //!< Generation mode
    unsigned long _seed_;            //!< Local PRNG's seed
    mygsl::rng    _random_;          //!< Local PRNG
    pg_col_type   _generators_info_; //!< Particle generators
    GENBB_PG_REGISTRATION_INTERFACE(combined_particle_generator)

  };

} // end of namespace genbb

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(genbb::combined_particle_generator)

#endif // GENBB_HELP_COMBINED_PARTICLE_GENERATOR_H

// Local Variables: --
// mode: c++ --
// End: --

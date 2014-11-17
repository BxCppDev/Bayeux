/// \file genbb_help/combined_particle_generator.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
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
  class combined_particle_generator : public i_genbb
  {
  public:

    struct entry_type
    {
      std::string label;    /// Label
      std::string name;     /// Name
      double    prob;       /// Probability
      double    cumul_prob; /// Cumulated probability
      i_genbb * pg;         /// Reference to a particle generator
    };
    typedef std::vector<entry_type> pg_col_type;

    enum mode_type
      {
        MODE_PLAIN_PROBABILITY = 0,
        MODE_ACTIVITY          = 1
      };

    void set_mode(int mode_);

    bool is_mode_plain_probability () const;

    bool is_mode_activity () const;

    virtual bool can_external_random () const;

    const mygsl::rng & get_random () const;

    mygsl::rng & grab_random ();

    /// Constructor
    combined_particle_generator ();

    /// Destructor
    virtual ~combined_particle_generator ();

    /// Main initialization interface method
    virtual void initialize (const datatools::properties & setup_,
                             datatools::service_manager & service_manager_,
                             detail::pg_dict_type & dictionary_);

    virtual void reset ();

    virtual bool has_next ();

    /// Check initialization status
    virtual bool is_initialized () const;

    void dump(std::ostream & out_, const std::string & title_, const std::string & indent_ = "") const;

  protected:

    virtual void _load_next (primary_event & event_,
                             bool compute_classification_ = true);

  private:

    void _at_init_ ();

    void _at_reset_ ();

  private:

    bool   _initialized_;
    int    _mode_;
    unsigned long _seed_;   //!< Local PRNG's seed
    mygsl::rng    _random_; //!< Local PRNG
    pg_col_type   _generators_info_;
    GENBB_PG_REGISTRATION_INTERFACE(combined_particle_generator);

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

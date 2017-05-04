/// \file crazy_gun.h
/* Author(s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-02-04
 * Last modified: 2016-02-25
 *
 * License:
 * Copyright 2014-2016 F. Mauger
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
 *   A generator of GENBB-like events with crazy bunches of random particles
 *
 * History:
 *
 */

#ifndef GENBB_HELP_EX02_CRAZY_GUN_H
#define GENBB_HELP_EX02_CRAZY_GUN_H

// Third party:
// - Bayeux/genbb_help:
#include <genbb_help/i_genbb.h>

namespace genbb {

  namespace ex02 {

    /// \brief Generator for crazy particles
    /// This class fires, in the same direction, a random number of particles
    /// of various types with a flat random kinetic energy in the [0.5;10] MeV range, each
    /// particles being emitted with a random delay wrt the previous
    /// emitted particle (lifetime of 1 ns).
    class crazy_gun : public genbb::i_genbb
    {
    public:

      /// Lower bound of the number of particles range
      static const unsigned int MIN_NUMBER_OF_PARTICLES = 1;

      /// Upper bound of the number of particles range
      static const unsigned int MAX_NUMBER_OF_PARTICLES = 10;

      /// Check if the generator accepts an external PRNG
      virtual bool can_external_random() const;

      /// Set the minimum number of particles
      void set_min_number_of_particles(unsigned int);

      /// Return the minimum number of particles
      unsigned int get_min_number_of_particles() const;

      /// Set the maximum number of particles
      void set_max_number_of_particles(unsigned int);

      /// Return the maximum number of particles
      unsigned int get_max_number_of_particles() const;

      /// Constructor
      crazy_gun();

      /// Destructor
      virtual ~crazy_gun();

      /// Main initialization interface method
      virtual void initialize(const datatools::properties & setup_,
                              datatools::service_manager & service_manager_,
                              detail::pg_dict_type & dictionary_);

      /// Reset the generator
      virtual void reset();

      /// Check if another event is available
      virtual bool has_next();

      /// Check initialization status
      virtual bool is_initialized() const;

    protected:

      /// The main event generation method:
      virtual void _load_next(primary_event & event_,
                              bool compute_classification_ = true);

      /// Set default attribute values
      void _set_defaults();

    private:

      bool _initialized_; ///< Initialization flag of the generator
      unsigned int _min_number_of_particles_; ///!< Min number of generatedparticles
      unsigned int _max_number_of_particles_; ///!< Max number of generatedparticles

      /// Class registration macro
      GENBB_PG_REGISTRATION_INTERFACE(crazy_gun);

    };

  } // end of namespace ex02

} // end of namespace genbb

// OCD support:
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(genbb::ex02::crazy_gun)

#endif // GENBB_HELP_EX02_CRAZY_GUN_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

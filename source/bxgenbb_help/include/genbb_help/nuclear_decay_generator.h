/// \file genbb_help/nuclear_decay_generator.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-06-18
 * Last modified: 2014-06-18
 *
 * License:
 * Copyright 2014 F. Mauger
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
 *   A generator that read primary events from some input files
 *   using an embedded dpp::input_module. The file format is the
 *   one produced by the genbb::save_to_file_wrapper class.
 *
 * History:
 *
 */

#ifndef GENBB_HELP_NUCLEAR_DECAY_GENERATOR_H
#define GENBB_HELP_NUCLEAR_DECAY_GENERATOR_H 1

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>

// This project:
#include <genbb_help/i_genbb.h>
// #include <genbb_help/nuclear_decay_manager.h>
// #include <genbb_help/nuclear_level.h>

namespace genbb {

  // Forward declaration:
  class nuclear_level;
  class nuclear_decay_manager;

  /// \brief A generator that shoot nuclear decays
  class nuclear_decay_generator : public i_genbb
  {
  public:

    /// Default constructor
    nuclear_decay_generator();

    /// Destructor
    ~nuclear_decay_generator() override;

    /// Main initialization interface method
    void initialize(const datatools::properties & setup_,
                            datatools::service_manager & service_manager_,
                            detail::pg_dict_type & dictionary_) override;

    /// Reset
    void reset() override;

    /// Check if the generator has a next event
    bool has_next() override;

    /// Check initialization status
    bool is_initialized() const override;

    /// Return the Local PRNG's seed
    unsigned long get_seed() const;

    /// Accept external PRNG
    bool can_external_random() const override;

    /// Return the active PRNG
    const mygsl::rng & get_random() const;

    /// Return the active PRNG
    mygsl::rng & grab_random();

  protected:

    /// Generate the next event:
    void _load_next(primary_event & event_, bool compute_classification_) override;

  private:

    bool        _initialized_; //!< Initialization flag
    boost::scoped_ptr<nuclear_decay_manager> _ndm_; //!< Handle to the nuclear decay manager
    const nuclear_level * _decaying_level_; //!< Handle to the nuclear decaying level
    unsigned long _seed_;   //!< Local PRNG's seed
    boost::scoped_ptr<mygsl::rng> _random_; //!< Local PRNG

    GENBB_PG_REGISTRATION_INTERFACE(nuclear_decay_generator)

  };

} // end of namespace genbb

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(genbb::nuclear_decay_generator)

#endif // GENBB_HELP_NUCLEAR_DECAY_GENERATOR_H

// Local Variables: --
// mode: c++ --
// End: --

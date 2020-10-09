/// \file genbb_help/base_decay_driver.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-06-08
 * Last modified: 2014-06-08
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
 *   Description of a nuclear decay.
 *
 * History:
 *
 */

#ifndef GENBB_HELP_BASE_DECAY_DRIVER_H
#define GENBB_HELP_BASE_DECAY_DRIVER_H 1

// Third party:
// - Bayeux/datatools:
#include <datatools/factory_macros.h>
#include <datatools/logger.h>
#include <datatools/i_tree_dump.h>
#include <datatools/enriched_base.h>

namespace datatools {
  // Forward declaration:
  class properties;
}

namespace mygsl {
  // Forward declaration:
  class rng;
}

namespace genbb {

  // Forward declaration:
  class primary_event;
  class nuclear_level;
  class nuclear_decay;

  /// \brief Base decay driver
  class base_decay_driver : public datatools::i_tree_dumpable
  {
  public:

    /// Default constructor
    base_decay_driver();

    /// Destructor
    ~base_decay_driver() override;

    /// Return the logging priority threshold
    datatools::logger::priority get_logging() const;

    /// Set the logging priority threshold
    void set_logging(datatools::logger::priority);

    /// Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

    /// Check initialization status
    bool is_initialized() const;

    /// Initialization
    void initialize_simple();

    /// Set the transition parameters from the initial and stop levels
    virtual void set_levels(const nuclear_level & level_initial_,
                            const nuclear_level & level_final_);

    /// Initialize from configuration properties
    virtual void initialize(const datatools::properties & setup_) = 0;

    /// Reset
    virtual void reset() = 0;

    /// Shoot the kinematics of the decay and fill a primary event
    virtual int fill(mygsl::rng & prng_, primary_event & event_) = 0;

  protected:

    /// Set the default attributes
    void _set_default();

    /// Set the initialization flag
    void _set_initialized(bool);

    /// Initialization
    void _initialize(const datatools::properties & setup_);

  private:

    datatools::logger::priority _logging_;     //!< Logging priority threshold
    bool                        _initialized_; //!< Initialization flag
    const nuclear_decay *       _decay_;       //!< Handle to a nuclear decay

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(base_decay_driver)

  };


  /// \brief Decay driver factory methods
  class decay_driver_factory : public datatools::i_tree_dumpable
  {
  public:

    static base_decay_driver * create(const std::string & driver_id_);

  };

} // end of namespace genbb

#endif // GENBB_HELP_BASE_DECAY_DRIVER_H

// Local Variables: --
// mode: c++ --
// End: --

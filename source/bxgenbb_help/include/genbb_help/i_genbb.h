/// \file genbb_help/i_genbb.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-28
 * Last modified: 2014-12-18
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
 *   GENBB generator abstract class
 *
 */

#ifndef GENBB_HELP_I_GENBB_H
#define GENBB_HELP_I_GENBB_H 1

// Third party:
// - Bayeux/datatools:
#include <datatools/factory_macros.h>
#include <datatools/logger.h>
#include <datatools/i_tree_dump.h>

// This project:
#include <genbb_help/detail/pg_tools.h>

namespace datatools {
  // Forward declarations:
  class properties;
  class service_manager;
  class object_configuration_description;
}

namespace mygsl {
  // Forward declaration:
  class rng;
}

namespace genbb {

  // Forward declaration:
  class primary_event;

  /// \brief GENBB particle generator abstract base class
  class i_genbb : public datatools::i_tree_dumpable
  {
  public:

    /// Check for a non-empty name
    bool has_name() const;

    /// Return the name of the event generator
    const std::string & get_name() const;

    /// Set the name of the event generator
    void set_name(const std::string &);

    /// Reset the name of the event generator
    void reset_name();

    /// Constructor
    i_genbb();

    /// Destructor
    ~i_genbb() override;

    /// Load a new 'primary_event' object
    virtual void load_next(primary_event & event_,
                           bool compute_classification_ = true);

    /// Check if the generator accepts an external PRNG
    virtual bool can_external_random() const;

    /// Check if the generator uses an external PRNG
    virtual bool has_external_random() const;

    /// Set an external PRNG
    virtual void set_external_random(mygsl::rng &);

    /// Reset an external PRNG
    virtual void reset_external_random();

    /// Return a mutable reference to the external PRNG
    mygsl::rng & grab_external_random();

    /// Return a non-mutable reference to the external PRNG
    const mygsl::rng & get_external_random() const;

    /// Simple initialization (no external resource)
    virtual void initialize_simple();

    /// Initialization from a container of properties
    virtual void initialize_standalone(const datatools::properties & setup_);

    /// Initialization from a container of properties and a service manager
    virtual void initialize_with_service_only(const datatools::properties & setup_,
                                              datatools::service_manager & service_manager_);

    /// Initialization from a container of properties, a service manager and a dictionnary of vertex generators
    virtual void initialize_with_dictionary_only(const datatools::properties & setup_,
                                                 detail::pg_dict_type & dictionary_);

    /// Check initialization status
    virtual bool is_initialized() const = 0;

    /// Main initialization interface method
    virtual void initialize(const datatools::properties & setup_,
                            datatools::service_manager & service_manager_,
                            detail::pg_dict_type & dictionary_) = 0;

    /// Reset method
    virtual void reset() = 0;

    /// Returns true if the generator can provide one more generated event
    virtual bool has_next() = 0;

    /// Return the logging priority threshold
    datatools::logger::priority get_logging_priority() const;

    /// Set the logging priority threshold
    void set_logging_priority(datatools::logger::priority p);

    /// Check the debug logging
    bool is_debug() const;

    /// Set the flag to assign unique generation Ids to primary particles
    void set_assign_generation_ids(bool);

    /// Check the flag to assign unique generation Ids to primary particles
    bool is_assign_generation_ids() const;

    /// Smart print
    void tree_dump(std::ostream& out = std::clog,
                           const std::string& title  = "",
                           const std::string& indent = "",
                           bool inherit = false) const override;

    /// OCD interface
    static void base_initialize_ocd(datatools::object_configuration_description &);

  protected:

    /// Protected abstract interface to be invoked by the public 'load_next' method
    virtual void _load_next(primary_event & event_,
                            bool compute_classification_) = 0;

    /// Base initialization
    void _initialize_base(const datatools::properties & setup_);

  protected:

    datatools::logger::priority _logging_priority; /// Logging priority threshold

  private:

    std::string  _name_; //!< The name of the event generator
    mygsl::rng * _external_random_; //!< Handle to an external PRNG
    bool _assign_generation_ids_; //!< Flag to assign unique generation Ids to primary particles

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(i_genbb)

  };

} // end of namespace genbb

#include <genbb_help/genbb_macros.h>

#endif // GENBB_HELP_I_GENBB_H

// Local Variables: --
// mode: c++ --
// End: --

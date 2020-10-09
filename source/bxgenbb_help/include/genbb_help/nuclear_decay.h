/// \file genbb_help/nuclear_decay.h
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

#ifndef GENBB_HELP_NUCLEAR_DECAY_H
#define GENBB_HELP_NUCLEAR_DECAY_H 1

// Standard library:
#include <string>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/handle.h>
#include <datatools/logger.h>
#include <datatools/i_tree_dump.h>

namespace genbb {

  // Forward declaration
  class nuclear_level;
  class base_decay_driver;

  /// \brief Description of a nuclear decay
  class nuclear_decay : public datatools::i_tree_dumpable
  {
  public:

    /// Handle type
    typedef datatools::handle<nuclear_decay> handle_type;

    /// Default constructor
    nuclear_decay();

    /// Destructor
    ~nuclear_decay() override;

    /// Check validity
    bool is_valid() const;

    /// Check if the decay mode is compatible with parent and daughter levels
    bool check_decay(std::string &) const;

    /// Check if the decay identifier is set
    bool has_decay_id() const;

    /// Return the decay identifier
    const std::string & get_decay_id() const;

    /// Set the decay identifier
    void set_decay_id(const std::string &);

    /// Set the parent level
    void set_level_initial(const nuclear_level &);

    /// Return the parent level
    const nuclear_level & get_level_initial() const;

    /// Set the daughter level
    void set_level_final(const nuclear_level &);

    /// Return the daughter level
    const nuclear_level & get_level_final() const;

    /// Return auxiliary properties
    const datatools::properties & get_auxiliaries() const;

    /// Return auxiliary properties
    datatools::properties & grab_auxiliaries();

    /// Check initialization flag
    bool is_initialized() const;

    /// Initialize from a list of parameters and a dictionary of nuclear levels
    void initialize(const datatools::properties & config_,
                    const std::map<std::string, datatools::handle<nuclear_level> > * levels_ = 0);

      /// Standalone initialization
    void initialize_simple();

    /// Reset
    void reset();

    /// Return a descriptive string
    std::string to_string(unsigned int = 0) const;

    /// Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

    /// Check the decay driver avaibility
    bool has_decay_driver() const;

    /// Return the decay driver
    base_decay_driver & grab_decay_driver();

    /// Return the logging priority threshold
    datatools::logger::priority get_logging() const;

    /// Set the logging priority threshold
    void set_logging(datatools::logger::priority);

  protected:

    /// Initialization of the embedded decay driver
    void _init_decay_driver(const datatools::properties &);

    /// Set attributes' default values
    void _set_default();

  private:

    bool _initialized_; //!< Initialization flag
    datatools::logger::priority _logging_; //!< Logging priority threshold
    std::string           _decay_id_;      //!< Decay identifier
    const nuclear_level * _level_initial_; //!< Handle to parent level
    const nuclear_level * _level_final_;   //!< Handle to daughter level
    datatools::properties _auxiliaries_;   //!< Auxiliary properties

    boost::scoped_ptr<base_decay_driver> _decay_driver_; //!< Handle to the embedded decay driver

  };

} // end of namespace genbb

#endif // GENBB_HELP_NUCLEAR_DECAY_H

// Local Variables: --
// mode: c++ --
// End: --

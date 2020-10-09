/// \file genbb_help/nuclear_decay_manager.h
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
 *   Nuclear decay manager.
 *
 * History:
 *
 */

#ifndef GENBB_HELP_NUCLEAR_DECAY_MANAGER_H
#define GENBB_HELP_NUCLEAR_DECAY_MANAGER_H 1

// Standard library:
#include <string>
#include <map>

// Third party:
// - Bayeux/datatools:
#include <datatools/handle.h>
#include <datatools/logger.h>
#include <datatools/i_tree_dump.h>

namespace datatools {
  class properties;
  class multi_properties;
}

namespace genbb {

  // Forward declarations:
  class nuclear_level;
  class nuclear_decay;

  /// \brief A nuclear decay manager
  class nuclear_decay_manager : public datatools::i_tree_dumpable
  {
   public:

    /// Dictionary of nuclear levels
    typedef std::map<std::string, datatools::handle<nuclear_level> > level_dict_type;

    /// Dictionary of nuclear decays
    typedef std::map<std::string, datatools::handle<nuclear_decay> > decay_dict_type;

    /// Return the logging priority threshold
    datatools::logger::priority get_logging() const;

    /// Set the logging priority threshold
    void set_logging(datatools::logger::priority);

    /// Default constructor
    nuclear_decay_manager();

    /// Destructor
    ~nuclear_decay_manager() override;

    /// Load a nuclear level
    void load_level(const std::string & level_name_,
                    const datatools::properties & level_config_);
    /// Load a nuclear decay
    void load_decay(const std::string & decay_name_,
                    const datatools::properties & decay_config_);

    /// Load a decay table for an existing level
    void load_decay_table(const std::string & level_name_,
                          const datatools::properties & decay_table_config_);

    /// Load levels and decays from a configuration file
    void load(const std::string & filename_);

    /// Load levels and decays from a list of multi properties
    void load(const datatools::multi_properties & levels_and_decays_config_);

    /// Check initialization
    bool is_initialized() const;

    /// Initialization from a list of properties
    void initialize(const datatools::properties & config_);

    /// Initialization
    void initialize_simple();

    /// Reset
    void reset();

    /// Return levels
    const level_dict_type & get_levels() const;

    /// Return decays
    const decay_dict_type & get_decays() const;

    /// Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

  protected:

    /// Install decay tables associated to nuclear levels
    void _install_decay_tables();

  private:

    datatools::logger::priority _logging_; //!< Logging priority threshold
    bool            _initialized_;         //!< Initialization flag
    level_dict_type _levels_;              //!< Collection of nuclear levels
    decay_dict_type _decays_;              //!< Collection of nuclear decays
    bool            _do_decay_tables_;     //!< Flag to install decay tables

  };

} // end of namespace genbb

#endif // GENBB_HELP_NUCLEAR_DECAY_MANAGER_H

// Local Variables: --
// mode: c++ --
// End: --

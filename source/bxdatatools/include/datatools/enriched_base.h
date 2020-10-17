/// \file datatools/enriched_base.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-09-12
 * Last modified: 2015-04-18
 *
 * License:
 *
 * Copyright (C) 2013-2015 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   A base class for complex objects with several basic attributes.
 *
 */

#ifndef DATATOOLS_ENRICHED_BASE_H
#define DATATOOLS_ENRICHED_BASE_H

// Standard library:
#include <iostream>
// - Breaks with GCC4.9.2_2 in C++98 :
// #include <cstdint> // requires C++11
// Third party:
#include <boost/cstdint.hpp>

// This project:
#include <datatools/bit_mask.h>
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/logger.h>
#include <datatools/properties.h>

namespace datatools {
  class object_configuration_description;

  /// \brief A base class with useful attributes usable in many contexts
  class enriched_base
    : public datatools::i_serializable
    , public datatools::i_tree_dumpable
  {

  public:

    /// Validate a name
    static bool validate_name(const std::string & candidate_name_);

    /// Check if a name is valid
    virtual bool is_name_valid(const std::string & name_) const;

    /// Default constructor
    enriched_base();

    /// Constructor
    enriched_base(const std::string & name_,
                  const std::string & display_name_ = "",
                  const std::string & terse_description_ = "",
                  logger::priority lp_ = logger::PRIO_FATAL);

    /// Destructor
    ~enriched_base() override;

    /// Check if the name is not empty
    bool has_name() const;

    /// Set the name
    void set_name(const std::string & name_);

    /// Set the name
    enriched_base & set_name_c(const std::string & name_);

    /// Return the name
    const std::string & get_name() const;

    /// Reset the name
    void reset_name();

    /// Check if the display name is empty
    bool has_display_name() const;

    /// Set the display name
    void set_display_name(const std::string & display_name_);

    /// Set the display name
    enriched_base & set_display_name_c(const std::string & display_name_);

    /// Return the display name
    const std::string & get_display_name() const;

    /// Check if the description is empty
    bool has_terse_description() const;

    /// Set the description
    void set_terse_description(const std::string & terse_description_);

    /// Set the description
    enriched_base & set_terse_description_c(const std::string & terse_description_);

    /// Return the description
    const std::string & get_terse_description() const;

    /// Check if the container of auxiliary properties is empty
    bool has_auxiliaries() const;

    /// Return a reference to the non mutable container of auxiliary properties
    const datatools::properties & get_auxiliaries() const;

    /// Return a reference to the mutable container of auxiliary properties
    datatools::properties & grab_auxiliaries();

    /// Remove all auxiliary properties
    void reset_auxiliaries();

    /// Check the debug status
    bool is_debug() const;

    /// Set the logging priority threshold
    void set_logging_priority(logger::priority lp_);

    /// Set the logging priority threshold
    enriched_base & set_logging_priority_c(logger::priority lp_);

    /// Set the logging priority threshold from a string
    void set_logging(const std::string & label_);

    /// Return the logging priority threshold
    logger::priority get_logging_priority() const;

    /// Return the default logging priority threshold
    static logger::priority default_logging_priority();

    /// Clear
    void clear();

    /// Clear
    void reset();

    /// Smart print
    void print_tree(std::ostream & out_ = std::clog,
                    const boost::property_tree::ptree & options_ = empty_options()) const override;
    
    /// Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                   const std::string & title_  = "",
                   const std::string & indent_ = "",
                   bool inherit_ = false) const override;
    
    /// Initialization from a container of properties
    void initialize(const datatools::properties & config_, bool clear_ = false);

    /// \brief Flags for export to a container of properties
    enum export_config_flags {
      EXPORT_CONFIG_CLEAR             = bit_mask::bit00,
      EXPORT_CONFIG_NAME              = bit_mask::bit01,
      EXPORT_CONFIG_DISPLAY_NAME      = bit_mask::bit02,
      EXPORT_CONFIG_TERSE_DESCRIPTION = bit_mask::bit03,
      EXPORT_CONFIG_LOGGING_PRIORITY  = bit_mask::bit04,
      EXPORT_CONFIG_AUXILIARIES       = bit_mask::bit05,
      EXPORT_CONFIG_DEFAULT           = (EXPORT_CONFIG_CLEAR
                                         | EXPORT_CONFIG_DISPLAY_NAME
                                         | EXPORT_CONFIG_TERSE_DESCRIPTION
                                         | EXPORT_CONFIG_AUXILIARIES),
      EXPORT_CONFIG_LAST             = EXPORT_CONFIG_AUXILIARIES
    };

    /// Export to a container of properties
    void export_to_config(datatools::properties & config_,
                          uint32_t flags_ = EXPORT_CONFIG_DEFAULT,
                          const std::string & prefix_ = "") const;

    /// OCD support
    static void init_ocd(datatools::object_configuration_description &);

  private:

    std::string _name_;                  //!< Name
    std::string _display_name_;          //!< Display name
    std::string _terse_description_;     //!< Terse description
    logger::priority _logging_priority_; //!< Logging priority threshold
    datatools::properties _auxiliaries_; //!< Container of auxiliary properties

    //! Serialization interface
    DATATOOLS_SERIALIZATION_DECLARATION_ADVANCED(enriched_base)

#ifndef Q_MOC_RUN
    //! Reflection interface
    DR_CLASS_RTTI()
#endif // Q_MOC_RUN

  };

} // end of namespace datatools

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(datatools::enriched_base, "datatools::enriched_base")

#ifndef Q_MOC_RUN
// Activate reflection layer for the 'datatools::enriched_base' class:
DR_CLASS_INIT(::datatools::enriched_base)
#endif // Q_MOC_RUN

#endif // DATATOOLS_ENRICHED_BASE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

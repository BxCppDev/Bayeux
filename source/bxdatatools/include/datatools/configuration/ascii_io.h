/// \file datatools/configuration/ascii_io.h
/// \brief Read/Write of variant records, registries, and repositories
/* Copyright (C) 2014-2017 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   I/O utilities for variant registries and repositories.
 *
 */

#ifndef DATATOOLS_CONFIGURATION_ASCII_IO_H
#define DATATOOLS_CONFIGURATION_ASCII_IO_H

// Standard library
#include <iostream>
#include <string>

// This project
#include <datatools/bit_mask.h>
#include <datatools/logger.h>
#include <datatools/version_id.h>
#include <datatools/configuration/variant_record.h>
#include <datatools/configuration/variant_registry.h>
#include <datatools/configuration/variant_repository.h>

namespace datatools {
  namespace configuration {
    /// \brief ASCII I/O utility for variant repository and registries
    class ascii_io
    {
    public:
      /// \brief Special initialization flags for the ASCII I/O utility
      enum store_flags {
        IO_DESCRIPTION = datatools::bit_mask::bit00, //!< Print meta comment
        IO_TITLE       = datatools::bit_mask::bit01, //!< Print title comment
        IO_TRACE       = datatools::bit_mask::bit02, //!< Trace/debug mode
        IO_NO_HEADER   = datatools::bit_mask::bit03, //!< Do not print header
        IO_DONT_IGNORE_UNKNOWN_REGISTRY = datatools::bit_mask::bit04, //!< Do not load unknown registries from profile
        IO_DEFAULT     = 0                           //!< Default flags
      };

      /// Default constructor
      explicit ascii_io(uint32_t flags_ = IO_DEFAULT);

      /// Return unset parameter label
      static const std::string & unset_label();

      /// Return error parameter label
      static const std::string & error_label();

      /// Return format label
      static const std::string & format_label();

      /// Return the current format version identifier
      static version_id current_format_version_id();

       /// Store a variant registry record
      void store_record(std::ostream & out_, const variant_record & vrec_) const;

      /// Load a variant registry record
      int load_record(std::istream & in_, variant_record & vrec_) const;

      /// Store a variant registry
      void store_registry(std::ostream & out_, const variant_registry & vreg_) const;

      /// Load a variant registry
      int load_registry(std::istream & in_, variant_registry & vreg_) const;

      /// Store a variant repository
      void store_repository(std::ostream & out_, const variant_repository & vrep_) const;

      /// Load a variant repository
      int load_repository(std::istream & in_, variant_repository & vrep_) const;

      /// Return the logging priority
      logger::priority get_logging() const;

      /// Set the logging priority
      void set_logging(logger::priority);

      /// Print
      void print(std::ostream & out_, const std::string & title_ = "") const;

    private:
      bool _no_header_;           //!< Flag to inhibit header at begin of file
      bool _with_description_;    //!< Flag to print description meta comment
      bool _with_title_;          //!< Flag to print a title comment
      bool _dont_ignore_unknown_registries_; //!< Flag to ignore unknown registries
      logger::priority _logging_; //!< Logging priority
      datatools::version_id _format_version_; //!< Format version identifier
    };
  } // namespace configuration
} // namespace dataools

#endif // DATATOOLS_CONFIGURATION_ASCII_IO_H


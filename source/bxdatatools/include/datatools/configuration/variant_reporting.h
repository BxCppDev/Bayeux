/// \file datatools/configuration/variant_reporting.h
/// \brief Collect statistics on variant usage

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
*/

#ifndef DATATOOLS_CONFIGURATION_VARIANT_REPORTING_H
#define DATATOOLS_CONFIGURATION_VARIANT_REPORTING_H

// Standard Library
#include <iostream>
#include <map>
#include <string>

// This project
#include <datatools/logger.h>
#include <datatools/configuration/variant_repository.h>

namespace datatools {
  namespace configuration {
    /// \brief variant processing reporting
    class variant_reporting
    {
    public:
      typedef std::map<std::string, std::size_t> statistics_type;

      /// Default constructor
      variant_reporting() = default;

      /// Destructor
      ~variant_reporting() = default;

      /// Check if a variant configuration repository is used
      bool has_repository() const;

      /// Set the variant configuration repository
      void set_repository(const variant_repository & repository_);

      /// Reset the variant configuration repository
      void reset_repository();

      /// Get the variant configuration repository
      const variant_repository & get_repository() const;

      /// Reset the counters
      void reset();

      /// Print the statistic report
      void print_report(std::ostream & out_, uint32_t flags_ = 0) const;

      /// Increment the counter associated to a processed variant parameter
      void add(const std::string & path_, std::size_t increment_ = 1);

      /// Print
      void dump(std::ostream & out_ = std::cerr) const;

      /// Return the logging priority
      logger::priority get_logging() const;

      /// Set the logging priority
      void set_logging(logger::priority);

    private:
      /// Specific initialization when repository is set
      void _init_repository_();

    private:
      logger::priority _logging_ = datatools::logger::PRIO_FATAL; //!< Logging priority
      const variant_repository * _repository_ = nullptr; //!< Variant configuration repository handle
      statistics_type _parameter_stats_{}; ///< Processing counters associated to variant parameters
    };
  } // namespace configuration
} // namespace datatools

#endif // DATATOOLS_CONFIGURATION_VARIANT_REPORTING_H


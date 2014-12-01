/// \file datatools/configuration/io.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2014-09-29
 * Last modified : 2014-11-29
 *
 * Copyright (C) 2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   A registry of configuration variants and associated parameters.
 *
 * History:
 *
 */

#ifndef DATATOOLS_CONFIGURATION_IO_H
#define DATATOOLS_CONFIGURATION_IO_H

// Standard library:
#include <iostream>

// This project (Bayeux/datatools):
#include <datatools/bit_mask.h>
#include <datatools/logger.h>
#include <datatools/bit_mask.h>
#include <datatools/command_utils.h>

namespace datatools {

  namespace configuration {

    // Forward declarations:
    class variant_record;
    class variant_registry;
    class variant_repository;

    /// \brief ASCII I/O utility for variant repository and registries
    class ascii_io
    {
    public:

      /// \brief Special initialization flags for the ASCII I/O utility
      enum store_flags {
        IO_DESCRIPTION = datatools::bit_mask::bit00, //!< Print meta comment
        IO_TITLE       = datatools::bit_mask::bit01, //!< Print title comment
        IO_TRACE       = datatools::bit_mask::bit02  //!< Trace/debug mode
      };

      /// Return unset parameter label
      static const std::string & unset_label();

      /// Default constructor
      ascii_io(uint32_t flags_ = 0);

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

      bool _with_description_;    //!< Flag to print description meta comment
      bool _with_title_;          //!< Flag to print a title comment
      logger::priority _logging_; //!< Logging priority

    };

    /// \brief Configuration variant string preprocessor
    class variant_preprocessor
    {
    public:

      /// \brief Construction flags
      enum flag_type {
        FLAG_TRACE         = datatools::bit_mask::bit00, //!< Flag to print trace messages
        FLAG_REMOVE_QUOTES = datatools::bit_mask::bit01  //!< Flag to remove quotes around string parameters
      };

      /// Check the trace flag
      bool is_trace() const;

      /// Set the trace flag
      void set_trace(bool);

      /// Check the remove quotes flag
      bool is_remove_quotes() const;

      /// Set the remove quotes flag
      void set_remove_quotes(bool);

      /// Default constructor
      variant_preprocessor(unsigned int flags_ = 0);

      /// Check if a variant configuration repository is used
      bool has_repository() const;

      /// Set the variant configuration repository
      void set_repository(const variant_repository & repository_);

      /// Get the variant configuration repository
      const variant_repository & get_repository() const;

      /// Process a source string into a target string
      command::returned_info preprocess(const std::string & source_,
                                        std::string & target_) const;

      /// Process a source string into a target string
      std::string preprocess_string(const std::string & source_) const;

      /// Process a source string into a target boolean
      bool preprocess_boolean(const std::string & source_) const;

      /// Process a source string into a target integer
      int preprocess_integer(const std::string & source_) const;

      /// Process a source string into a target double
      double preprocess_real(const std::string & source_) const;

      /// Process a list of arguments and options
      /** The 'argc_' and 'argv_' parameters should be the same as passed to 'main'.
       */
      void preprocess_args_options(int argc_, char ** argv_,
                                   std::vector<std::string> & preprocessed_args_);

      /// Process a list of arguments and options
      /** The 'args_' parameter should not include program name.
       */
      void preprocess_args_options(const std::vector<std::string> & args_,
                                   std::vector<std::string> & preprocessed_args_);

      /// Process a source string into a target string as a variant parameter
      command::returned_info preprocess_parameter(const std::string & parameter_token_,
                                                  std::string & parameter_effective_token_) const;

      /*! Parse a string and check if a given configuration variant is activate

        @param variant_desc_    The string to be parsed
        @param variant_active_  The flag that checks if the variant is activated
        @param variant_reverse_ The flag that checks if the test must be reversed

        Supported syntax is:

        Case 1:
        @code
          geometry:has_detector_0/if_detector
        @endcode
        The 'variant_active_' value is set to true if the 'geometry:has_detector_0/if_detector'
        variant is found activated from the variant repository. The 'variant_reverse_' flag
        is set to false.

        Case 2:
        @code
          geometry:has_detector_0/if_detector|true
        @endcode
        The 'variant_active_' value is set to true if the 'geometry:has_detector_0/if_detector'
        variant is found activated from the variant repository. The 'variant_reverse_' flag
        is set to false. if the 'geometry:has_detector_0/if_detector'
        variant is not found from the variant repository, the 'variant_active_' is set to 'true'
        by default.

        Case 3:
        @code
          !geometry:has_detector_0/if_detector
        @endcode
        The 'variant_active_' value is set to true if the 'geometry:has_detector_0/if_detector'
        variant is found activated from the variant repository, false elsewhere.
        The 'variant_reverse_' flag is set to true to indicate that the test uses reversed logic.

      */
      command::returned_info resolve_variant(const std::string & variant_desc_,
                                             bool & variant_active_,
                                             bool & variant_reverse_) const;

    private:

      /// Install the kernel repository as the default one referenced by the preprocessor
      void _set_default_kernel_repository();

    private:

      bool _trace_; //!< Trace flag
      bool _remove_quotes_; //!< Flag to remove quotes around string parameters
      const variant_repository * _repository_; //!< Variant configuration repository handle

    };

  }  // end of namespace configuration

}  // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_IO_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

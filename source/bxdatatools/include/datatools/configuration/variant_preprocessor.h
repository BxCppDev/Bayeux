/// \file datatools/configuration/variant_preprocessor.h
/// \brief Preprocessor for variant input strings
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

#ifndef DATATOOLS_CONFIGURATION_VARIANT_PREPROCESSOR_H
#define DATATOOLS_CONFIGURATION_VARIANT_PREPROCESSOR_H

// Standard Library
#include <string>
#include <vector>

// This project
#include <datatools/bit_mask.h>
#include <datatools/logger.h>
#include <datatools/command_utils.h>
#include <datatools/configuration/variant_repository.h>

namespace datatools {
  namespace configuration {
    /// \brief Configuration variant string preprocessor
    class variant_preprocessor
    {
    public:
      /// \brief Construction flags
      enum flag_type {
        FLAG_TRACE         = datatools::bit_mask::bit00, //!< Flag to print trace messages
        FLAG_REMOVE_QUOTES = datatools::bit_mask::bit01  //!< Flag to remove quotes around string parameters
      };

      /// Default constructor
      variant_preprocessor(unsigned int flags_ = 0);

      /// Constructor taking repository and flags
      variant_preprocessor(const variant_repository& rep_, unsigned int flags_);

      /// Default destructor
      ~variant_preprocessor() = default;

      /// Check the remove quotes flag
      bool is_remove_quotes() const;

      /// Set the remove quotes flag
      void set_remove_quotes(bool);

      /// Check if a variant configuration repository is used
      bool has_repository() const;

      /// Set the variant configuration repository
      void set_repository(const variant_repository & repository_);

      /// Reset the variant configuration repository
      void reset_repository();

      /// Check if the variant configuration repository is active
      bool repository_is_active() const;

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
        @return The command error record with supported error codes:

                * command::CEC_SUCCESS : variant has been succesfully resolved or default value is used
                * command::CEC_PARSING_FAILURE : syntax error in variant description
                * command::CEC_CONTEXT_INVALID : variant repository is not activated
                * command::CEC_SCOPE_INVALID : variant registry is unknown
                * command::CEC_PARAMETER_INVALID_KEY : variant record is unknown


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

      /// Return the logging priority
      logger::priority get_logging() const;

      /// Set the logging priority
      void set_logging(logger::priority);

    private:
      logger::priority _logging_ = logger::PRIO_FATAL; //!< Logging priority
      bool _remove_quotes_ = false; //!< Flag to remove quotes around string parameters
      const variant_repository * _repository_ = nullptr; //!< Variant configuration repository handle
    };
  } // namespace configuration
} // namespace datatools

#endif // DATATOOLS_CONFIGURATION_VARIANT_PREPROCESSOR_H



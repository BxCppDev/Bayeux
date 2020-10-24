//! \file datatools/multi_properties_config.h
/* multi_properties_config.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009
 * Last modified: 2020-01-28
 *
 * License:
 *
 * Copyright (C) 2009-2020 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   I/O for the multi_properties class
 *
 */

#ifndef DATATOOLS_MULTI_PROPERTIES_CONFIG_H
#define DATATOOLS_MULTI_PROPERTIES_CONFIG_H

// Standard Library:
#include <cstdint>
#include <string>

// This Project
#include <datatools/file_include.h>
#include <datatools/multi_properties.h>

namespace datatools {
   /// \brief Reader/writer class for multi_properties objects
  class multi_properties_config
  {
  public:

    /// \brief Flags to modify the behaviour and formatting of a config object
    enum options_flag {
      SKIP_PRIVATE_SECTIONS = bit_mask::bit00, ///< Skip private sections bit
      FORBID_VARIANTS       = bit_mask::bit01, ///< Forbid variant block directives bit
      LOG_MUTE              = bit_mask::bit02, ///< Mute mode activation bit
      LOG_DEBUG             = bit_mask::bit03, ///< Debug mode activation bit
      LOG_TRACE             = bit_mask::bit04, ///< Trace mode activation bit
      SKIP_PRIVATE_PROPS    = bit_mask::bit05, ///< Skip private properties in sections bit
      HEADER_FOOTER         = bit_mask::bit06, ///< Use header/footer (write)
      DONT_CLEAR            = bit_mask::bit07, ///< Don't clear before parsing bit (read)
      REQUESTED_TOPIC       = bit_mask::bit08, ///< Requested topic (read/write)
      RESOLVE_PATH          = bit_mask::bit09, ///< Resolve path for input filename (read/write)
      START_WITHOUT_LABELS  = bit_mask::bit10, ///< Start without key/meta labels and let the reader set them
      WITHOUT_DECORATION    = bit_mask::bit11, ///< Do not use decoration
      FORBID_INCLUDE        = bit_mask::bit12  ///< Do not allow file inclusion
    };

    /// Default constructor
    multi_properties_config(uint32_t options_ = 0, const std::string & topic_ = "");

    /// Reset the reader/writer
    void reset();

    /// Read a multi_properties container from an input stream
    ///
    /// This method is the base of configuration file parsing.
    ///
    /// Example of input file to be parsed:
    /// \code
    ///
    /// # Format:
    /// # - Comment lines start with a '#'.
    /// # This is a  comment line...
    /// # ...and this is another one.
    ///
    /// # - Blank lines are ignored.
    /// # - Lines starting with '#@' are considered as optional metacomments
    /// #   with special embedded parsing options and/or actions.
    ///
    /// #@description The main configuration file for the application
    /// #  This optional metacomment provides the general description of the multi_properties container.
    /// #  This directive must be given before any *section* (see below).
    ///
    /// #@key_label "name"
    /// #  This metacomment provides the key use to identify the name of a *section*.
    /// #  (see below).
    /// #  Syntax is ``[name="foo" ... ]``.
    /// #  This directive must be given before any *section* (see below).
    ///
    /// #@meta_label "type"
    /// #  This metacomment provides the key use to identify the meta information of a *section*
    /// #  (see below).
    /// #  Syntax is ``[name="foo" type="my_model_1" ]``.
    /// #  This directive must be given before any *section* (see below).
    ///
    /// #@variant_devel
    /// #  This metacomment activates development logging from the variant mechanism
    ///
    /// #@variant_section_only "variant-expression"
    /// #  This metacomment corresponds to a directive which accept a given section only if
    ///
    /// #@forbid_include
    /// #  This metacomment inhibits the file inclusion inclusion mechanism
    ///
    /// #@include_debug
    /// #  This metacomment activates debug logging from the file inclusion mechanism
    ///
    /// #@include_dir "path"
    /// #  This metacomment specifies a directory from which files to be included are searched for.
    ///
    /// #@include_path_env "name"
    /// #  This metacomment specifies the name of an environment variable which contains
    /// #  an ordered list of priority directories from which files to be included are searched for.
    ///
    /// #@include_sections "path"
    /// #  This metacomment specifies the path of a file to be included
    ///
    /// #@include_sections_try "path"
    /// #  This metacomment specifies the path of a file to be included if possible
    ///
    // # NOT SUPPORTED"
    // #@include_path_env_strategy "label"
    // #  This metacomment specifies the strategy for resolving the include directories from the
    // #  environment variable set by the "@include_path_env" directive. Supported strategies are:
    // #  - prepend (default) : directories from the environment variable have priority on explicit
    // #    directories set through "@include_dir" directive.
    // #  - append : directories from the environment variable do not have priority on explicit
    // #    directories set through "@include_dir" directive.
    // #  - clear : directories from the environment variable are the only ones used.
    //
    ///
    /// # Sections:
    /// # Each section in a "multi_properties" container implements a single "properties" container
    /// # associated to a unique key/identifier (in the scope of the "multi_properties" container)
    /// # and eventually a meta information, usually documenting the type/category of object
    /// # or component the section refers to.
    ///
    /// [name="core"  type="algo::calibration"]
    /// #  The directive above starts a new section with a given *name* and *type* (meta information)
    ///
    /// ...
    ///
    /// [name="display"  type="algo::calibration::gui"]
    /// #  The directive above starts a new section with a given *name* and *type* (meta information)
    ///
    /// ...
    ///
    /// # Each section contains a set of properties as illustrated in the ``properties`` class.
    ///
    /// [name="debug"  type="algo::calibration::debugger"]
    /// #  This directive above starts the section named 'debug' for a component
    /// #  of which the type is ``algo::calibration::debugger``.
    ///
    /// #@description Verbosity level of the debugger
    /// verbosity : integer = 4
    ///
    /// #@description List of components with active debugging support
    /// active : string[2] = "core" "display"
    ///
    /// [name="log"  type="algo::calibration::logger"]
    /// #  The directive above starts a new section with a given *name* and *type* (meta information)
    ///
    /// # This directive include the definitions of some properties of the current section
    /// #@include "~/.config/logger/main.conf"
    ///
    /// [name="foo"  type="algo::foo"]
    /// #@variant_section_only "variant-expression"
    /// #  The directive above corresponds to a directive which accepts a given section only if
    /// #  a the variant-expression is true. It must be placed at the very beginning of a section.
    ///
    /// # The following directives include some multi-properties files and merge their contents
    /// # in the current one. Existing sections are merged and their properties are overriden
    /// # if they appear both in the current and includes files.
    /// #@include_sections "more_sections.conf"
    /// #@include_sections "overriden_sections.conf"
    ///
    /// # Same as the former directive but failure to resolve the file to be include
    /// # does not fail the parsing.
    /// #@include_sections_try "optional_sections.conf"
    ///
    /// \endcode
    ///
    void read(std::istream & in_, multi_properties & target_);

    /// Read from an input file
    void read(const std::string & filename_, multi_properties & source_);

    /// Write to an output stream
    void write(std::ostream & out_, const multi_properties & source_);

    /// Write to an output file
    void write(const std::string & filename_, const multi_properties & source_);

    /// Check if topic is set
    bool has_topic() const;

    /// Set the topic that should be matched
    void set_topic(const std::string & topic_);

    /// Return the topic
    const std::string & get_topic() const;

    /// Return the embedded file inclusion solver
    const file_include & get_fi() const;

    /// Return the mutable embedded file inclusion solver
    file_include & grab_fi();

    /// Set the embedded file inclusion solver
    void set_fi(const file_include &);

  protected:

    /// Default initialization
    void _init_defaults();

    /// Read from an input stream
    void _read(std::istream & in_, multi_properties & target_);

    /// Write to an output stream
    void _write(std::ostream & out_, const multi_properties & target_);

    /// Store the current filename
    void _set_current_filename(const std::string & filename_);

  private:

    // Configuration:
    datatools::logger::priority _logging_; ///< Logging priority threshold (read/write)
    bool _skip_private_sections_;   ///< Flag to skip private sections (read/write)
    bool _skip_private_properties_; ///< Flag to skip private properties in sections (read/write)
    bool _forbid_variants_;         ///< Flag to forbid variant directives (read)
    bool _header_footer_;           ///< Flag to print header/footer (write)
    bool _requested_topic_;         ///< Flag to activate topic matching (read/write)
    std::string _topic_;            ///< Topic to be validated
    bool _resolve_path_;            ///< Explicitely resolve path for input/output filenames (read/write)
    bool _start_without_labels_;    ///< Start without labels (read)
    bool _without_decoration_;      ///< Flag to disable decoration (write)
    bool _forbid_include_ = false;  ///< Flag to forbid file inclusion (read)

    // Working parsing data:
    std::string _current_filename_;    ///< Current filename
    int         _current_line_number_; ///< Current line number
    file_include _fi_;                 ///< File inclusion solver

  }; //----- end of class config
}

#endif // DATATOOLS_MULTI_PROPERTIES_CONFIG_H

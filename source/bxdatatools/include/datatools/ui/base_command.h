//! \file  datatools/ui/base_command.h
//! \brief Base class for command line interface command objects
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of datatools.
//
// datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools. If not, see <http://www.gnu.org/licenses/>.

#ifndef DATATOOLS_UI_BASE_COMMAND_H
#define DATATOOLS_UI_BASE_COMMAND_H

// Standard library:
#include <string>
#include <vector>
#include <memory>

// Third party:
// - Boost:
#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <boost/cstdint.hpp>

// This project:
#include <datatools/enriched_base.h>
#include <datatools/properties.h>
#include <datatools/command_utils.h>
#include <datatools/version_id.h>

namespace datatools {

  namespace ui {

    class base_command_interface;

    //! \brief Base command
    class base_command
      : public datatools::enriched_base
    {
    public:

      // Aliases:
      typedef boost::program_options::options_description            opts_type;
      typedef boost::program_options::positional_options_description args_type;
      typedef boost::program_options::variables_map                  vmap_type;

      //! Check if a name is valid
      bool is_name_valid(const std::string & candidate_name_) const override;

      //! Default constructor
      base_command();

      //! Constructor
      base_command(const std::string & name_,
                   const std::string & description_ = "",
                   const version_id & vid_ = version_id::invalid());

      //! Destructor
      ~base_command() override;

      //! Check if some generic options are described
      bool has_generic_opts() const;

      //! Return the description of generic options
      const opts_type & get_generic_opts() const;

      //! Check if generic options are used
      bool use_generic_options() const;

      //! Check if generic help are used
      bool use_generic_help() const;

      //! Check if some options are described
      bool has_opts() const;

      //! Return the description of options
      const opts_type & get_opts() const;

      //! Check if some positional options are set
      bool has_args() const;

      //! Check if short UI options are allowed
      bool has_short_ui_options() const;

      //! Check if the version is available
      bool has_version() const;

      //! Return the version of the command
      const datatools::version_id & get_version() const;

      //! Set the version of the command
      void set_version(const datatools::version_id &);

      //! Check validity
      virtual bool is_valid() const;

      //! Check is the parent interface is set
      bool has_parent_interface() const;

      //! Return the parent interface
      base_command_interface & grab_parent_interface();

      //! Set the parent interface
      void set_parent_interface(base_command_interface &);

      //! Return the parent interface
      const base_command_interface & get_parent_interface() const;

      //! Check if the command is initialized
      bool is_initialized() const;

      //! Initialize
      void initialize_simple();

      //! Initialize
      void initialize(const properties & config_);

      //! Reset
      void reset();

       //! Check if the command is disabled
      bool is_disabled() const;

      //! Main execution operator
      int operator()(const std::vector<std::string> & argv_,
                     datatools::command::returned_info & cri_,
                     uint32_t flags_ = 0);

      //! Execution operator
      datatools::command::returned_info
      operator()(const std::vector<std::string> & argv_,
                 uint32_t flags_ = 0);

      //! Call
      void call(const std::vector<std::string> & argv_,
                datatools::command::returned_info & cri_,
                uint32_t flags_ = 0);

      //! Print usage
      void print_usage(std::ostream & out_, uint32_t flags_ = 0) const;

      //! Print version
      void print_version(std::ostream & out_, uint32_t flags_ = 0) const;

      //! Smart print
      void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const override;

    protected:

      void _set_version(const datatools::version_id &);

      virtual void _init(const properties & config_);

      virtual void _fini();

      virtual void _run(datatools::command::returned_info &,
                        uint32_t flags_ = 0) = 0;

      //! Check the validity (this can be overloaded)
      virtual bool _is_valid() const;

      //! Check the disabled status (default: false)
      virtual bool _is_disabled() const;

      void _base_run(datatools::command::returned_info &,
                     uint32_t flags_ = 0);

      opts_type & _grab_generic_opts();

      void _alloc_generic_opts();

      void _free_generic_opts();

      opts_type & _grab_opts();

      void _alloc_opts(const std::string & opts_title_ = "Allowed options");

      void _free_opts();

      args_type & _grab_args();

      void _alloc_args();

      void _free_args();

      vmap_type & _grab_vmap();

      void _alloc_vmap();

      void _free_vmap();

      void _forbid_short_ui_options();

    protected:

      // Management:
      bool _initialized_ = false; //!< Initialization flag

      // Configuration:
      bool _no_short_ui_options_ = false;
      bool _no_generic_options_ = false;
      bool _no_generic_help_ = false;
      boost::optional<datatools::version_id> _version_; //!< Optional version
      boost::scoped_ptr<opts_type> _generic_opts_; //!< Description of generic options
      boost::scoped_ptr<opts_type> _opts_; //!< Description of options
      boost::scoped_ptr<args_type> _args_; //!< Description of positional options
      base_command_interface * _parent_interface_ = nullptr; //!< Reference to the parent interface (optional)

      // Working data:
      std::unique_ptr<vmap_type> _vmap_; //!< Description of variables map

    };

  } // namespace ui

} // namespace datatools

#endif // DATATOOLS_UI_BASIC_COMMAND_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

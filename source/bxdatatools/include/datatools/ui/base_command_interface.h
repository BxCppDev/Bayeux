//! \file  datatools/ui/base_command_interface.h
//! \brief Base command interface
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

#ifndef DATATOOLS_UI_BASE_COMMAND_INTERFACE_H
#define DATATOOLS_UI_BASE_COMMAND_INTERFACE_H

// Standard library:
#include <string>
#include <iostream>
#include <vector>

// Third Party:
// - Boost:
#include <boost/optional.hpp>
// - Bayeux/datatools:
#include <datatools/enriched_base.h>
#include <datatools/version_id.h>
#include <datatools/command_utils.h>
#include <datatools/factory_macros.h>
#include <datatools/exception.h>

namespace datatools {

  class properties;
  class service_manager;

  namespace ui {

    class basic_shell;
    class base_command;

    //! \brief Command interface for arbitrary objects
    //!
    //!        This object contains a dictionary of commands
    //!        associated to some arbitrary resource (object)
    class base_command_interface : public datatools::enriched_base
    {
    public:

      //! Default constructor
      base_command_interface(const std::string & name_ = "",
                             const std::string & description_ = "",
                             const version_id & vid_ = version_id::invalid());

      //! Destructor
      virtual ~base_command_interface();

      //! Check if the version is set
      bool has_version() const;

      //! Return the version
      const datatools::version_id & get_version() const;

      //! Return the number of commands
      std::size_t size() const;

      //! Check if a command with given name exists
      bool has_command(const std::string & command_name_) const;

      //! Return a reference to a mutable command
      base_command & grab_command(const std::string & command_name_);

      //! Return a reference to a command
      const base_command & get_command(const std::string & command_name_) const;

      //! Add an external command object
      base_command_interface & add_command(base_command & command_,
                                           const std::string & command_name_ = "");

      //! Add a local command object
      //! The interface is responsible of the memory management
      base_command_interface & add_command(base_command * command_,
                                           const std::string & command_name_ = "");

      //! Remove a command
      void remove_command(const std::string & command_name_);

      //! Build the list of command names
      void build_command_names(std::vector<std::string> & names_) const;

      //! Initialize the parser
      void initialize_simple();

      //! Initialize the parser using only a list of properties
      void initialize_standalone(const datatools::properties &);

      //! Check if the parser is initialized
      virtual bool is_initialized() const = 0;

      //! Initialize the device model using a list of properties with access to service manager
      virtual void initialize(const datatools::properties & config_,
                              const datatools::service_manager & services_) = 0;

      //! Reset
      virtual void reset() = 0;

      //! Check if the command interface is active
      bool is_active() const;

      //! Check if the command interface is valid
      bool is_valid() const;

      //! Smart print
      virtual void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const;

      //! Main command call operator
      int operator()(const std::vector<std::string> & argv_,
                     datatools::command::returned_info & cri_,
                     uint32_t flags_ = 0);

      //! Command call operator
      datatools::command::returned_info
      operator()(const std::vector<std::string> & argv_,
                 uint32_t flags_ = 0);

      //! Call method
      void call(const std::vector<std::string> & argv_,
                datatools::command::returned_info & cri_,
                uint32_t flags_ = 0);

    protected:

      //! Set the version
      void _set_version(const datatools::version_id &);

      //! Base initialization
      void _base_initialize(const datatools::properties & config_);

      //! Base reset
      void _base_reset();

      //! Check the validity (this can be overloaded)
      virtual bool _is_valid() const;

      //! Check the activity (this can be overloaded)
      virtual bool _is_active() const;

    private:

      // Configuration:
      boost::optional<datatools::version_id> _version_; //!< Optional version

      // Private data:
      struct pimpl_type;
      boost::scoped_ptr<pimpl_type> _pimpl_;
      pimpl_type & _grab_pimpl();
      const pimpl_type & _get_pimpl() const;

      // Factory stuff :
      DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(base_command_interface);

    };

    //! \brief Base command interface for a target object
    //!
    //! The target object (of arbitrary template type)
    //! cannot be set after the command interface object
    //! has been initialized.
    template <typename Type>
    class target_command_interface : public base_command_interface
    {
    public:

      //! Default constructor
      target_command_interface(const std::string & name_ = "",
                               const std::string & description_ = "",
                               const version_id & vid_ = version_id::invalid())
        : base_command_interface(name_, description_, vid_),
          _target_(0)
      {
        return;
      }

      //! Constructor
      target_command_interface(Type & target_,
                               const std::string & name_ = "",
                               const std::string & description_ = "",
                               const version_id & vid_ = version_id::invalid())
        : base_command_interface(name_, description_, vid_),
          _target_(0)
      {
        _set_target(target_);
        return;
      }

      //! Destructor
      virtual ~target_command_interface()
      {
        return;
      }

      //! Check if the target is set
      bool has_target() const
      {
        return _target_ != 0;
      }

      //! Set the target object
      void set_target(Type & target_)
      {
        DT_THROW_IF(is_initialized(), std::logic_error,
                    "Target command interface is already initialized!");
        _set_target(target_);
        return;
      }

      //! Return the target
      const Type & get_target() const
      {
        return *_target_;
      }

      //! Return the target
      Type & grab_target()
      {
        return _grab_target();
      }

      //! Smart print
      virtual void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const
      {
        this->base_command_interface::tree_dump(out_, title_, indent_, true);

        out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
             << "Target : [@" << _target_ << "]" << std::endl;

        return;
      }

    protected:

      //! Return the target
      Type & _grab_target()
      {
        return *_target_;
      }

      //! Set the target
      void _set_target(Type & target_)
      {
        _target_ = &target_;
        return;
      }

    private:

      Type * _target_; //!< The reference to the target object

    };

  } // namespace ui

} // namespace datatools

#endif // DATATOOLS_UI_BASE_COMMAND_INTERFACE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

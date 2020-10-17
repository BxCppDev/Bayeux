//! \file  datatools/ui/shell_command_interface.h
//! \brief Shell command interface
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

#ifndef DATATOOLS_UI_SHELL_COMMAND_INTERFACE_H
#define DATATOOLS_UI_SHELL_COMMAND_INTERFACE_H

// This project:
#include <datatools/command_utils.h>
#include <datatools/service_manager.h>
#include <datatools/properties.h>
#include <datatools/version_id.h>
#include <datatools/ui/base_command.h>
#include <datatools/ui/target_command_interface.h>

namespace datatools {

  namespace ui {

    class basic_shell;

    //! \brief Base command interface for a shell
    class shell_command_interface
      : public target_command_interface<basic_shell>
    {
    public:

      //! Default constructor
      shell_command_interface();

      //! Default constructor
      shell_command_interface(const std::string & name_ ,
                              const std::string & description_ = "",
                              const datatools::version_id & vid_ = datatools::version_id::invalid());

      //! Constructor
      shell_command_interface(basic_shell & sh_,
                              const std::string & name_,
                              const std::string & description_ = "",
                              const datatools::version_id & vid_ = datatools::version_id::invalid());

      //! Destructor
      ~shell_command_interface() override;

      //! Check initialization status
      bool is_initialized() const override;

      //! Initialization
      void initialize(const datatools::properties & config_,
                              const datatools::service_manager & services_) override;

      //! Reset
      void reset() override;

      //! Check if the load command is inhibited
      bool is_inhibit_load() const;

      //! Set the load command inhibition flag
      void set_inhibit_load(bool);

      //! Check if the cd command is inhibited
      bool is_inhibit_cd() const;

      //! Set the cd command inhibition flag
      void set_inhibit_cd(bool);

      //! Check if the shell is set
      bool has_shell() const;

      //! Return a mutable reference to the shell
      basic_shell & grab_shell();

      //! Return a non mutable reference to the shell
      const basic_shell & get_shell() const;

    protected:

      //! Add base shell commands
      virtual void _add_shell_commands();

      //! Remove base shell commands
      virtual void _remove_shell_commands();

    private:

      // Management:
      bool _initialized_ = false; //!< Initialization flag

      // Configuration:
      bool _inhibit_cd_ = false;   //!< Inhibition flag for the cd command
      bool _inhibit_load_ = false; //!< Inhibition flag for the load command

      // Factory registration:
      DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(base_command_interface,
                                                           shell_command_interface)

    };

  } // namespace ui

} // namespace datatools

#endif // DATATOOLS_UI_SHELL_COMMAND_INTERFACE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

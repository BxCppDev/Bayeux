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
#include <datatools/ui/base_command_interface.h>

namespace datatools {

  namespace ui {

    class basic_shell;

    //! \brief Command interface for a basic shell
    class shell_command_interface : public target_command_interface<basic_shell>
    {
    public:

      //! Default constructor
      shell_command_interface(basic_shell & sh_,
                              const std::string & name_ = "",
                              const std::string & description_ = "",
                              const datatools::version_id & vid_ = datatools::version_id::invalid());

      //! Destructor
      virtual ~shell_command_interface();

      //! Check initialization status
      virtual bool is_initialized() const;

      //! Initialization
      virtual void initialize(const datatools::properties & config_,
                              const datatools::service_manager & services_);

      //! Reset
      virtual void reset();

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
      void _add_base_commands();

      //! Remove base shell commands
      void _remove_base_commands();

    private:

      // Management:
      bool _initialized_; //!< Initialization flag

      // Configuration:
      bool _inhibit_cd_; //!< Inhibition flag for the cd command

    };

  } // namespace ui

} // namespace datatools

#endif // DATATOOLS_UI_SHELL_COMMAND_INTERFACE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

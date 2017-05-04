//! \file  datatools/ui/reflective_command_interface.h
//! \brief Reflective command interface
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef DATATOOLS_UI_REFLECTIVE_COMMAND_INTERFACE_H
#define DATATOOLS_UI_REFLECTIVE_COMMAND_INTERFACE_H

// Standard library:
#include <string>
#include <set>

// This project:
#include <datatools/command_utils.h>
#include <datatools/service_manager.h>
#include <datatools/properties.h>
#include <datatools/version_id.h>
#include <datatools/ui/target_command.h>
#include <datatools/ui/target_command_interface.h>

namespace datatools {

  namespace ui {

    //! \brief Reflective command interface for a target object
    template <typename Type>
    class reflective_command_interface : public target_command_interface<Type>
    {
    public:

      //! Default constructor
      reflective_command_interface();


      //! Default constructor
      reflective_command_interface(const std::string & name_ ,
                                   const std::string & description_ = "",
                                   const datatools::version_id & vid_ = datatools::version_id::invalid());

      //! Constructor
      reflective_command_interface(Type & target_,
                                   const std::string & name_,
                                   const std::string & description_ = "",
                                   const datatools::version_id & vid_ = datatools::version_id::invalid());

      //! Destructor
      virtual ~reflective_command_interface();

      //! Check initialization status
      virtual bool is_initialized() const;

      //! Initialization
      virtual void initialize(const datatools::properties & config_,
                              const datatools::service_manager & services_);

      //! Reset
      virtual void reset();

      //! Add the name of a command to be disabled
      void add_disabled_command(const std::string & name_);

    protected:

      //! Add commands
      virtual void _add_commands();

      //! Remove commands
      virtual void _remove_commands();

    private:

      // Management:
      bool _initialized_ = false; //!< Initialization flag
      std::set<std::string> _disabled_commands_; //!< List of disabled commands

    };

  } // namespace ui

} // namespace datatools

#include <datatools/ui/reflective_command_interface-inl.h>

#endif // DATATOOLS_UI_REFLECTIVE_COMMAND_INTERFACE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

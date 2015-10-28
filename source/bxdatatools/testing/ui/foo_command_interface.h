//! \file  ui/foo_command_interface.h
//! \brief Command classes for the foo class
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

#ifndef DATATOOLS_TESTING_UI_FOO_COMMAND_INTERFACE_H
#define DATATOOLS_TESTING_UI_FOO_COMMAND_INTERFACE_H

// This project:
#include <datatools/ui/base_command_interface.h>
#include "foo.h"
#include "foo_commands.h"

//! \brief Foo command interface
struct foo_command_interface : public datatools::ui::target_command_interface<foo>
{
  //! Default constructor
  foo_command_interface(foo & target_,
                        const std::string & name_ = "",
                        const std::string & description_ = "",
                        const datatools::version_id & vid_ = datatools::version_id::invalid())
    : datatools::ui::target_command_interface<foo>(target_, name_, description_, vid_)
  {
    return;
  }

  //! Destructor
  virtual ~foo_command_interface()
  {
    return;
  }

  //! Check initialization status
  virtual bool is_initialized() const
  {
    return size() > 0;
  }

  //! Initialization
  virtual void initialize(const datatools::properties & config_,
                          const datatools::service_manager & /* services_ */)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Command interface is already initialized!");

    _base_initialize(config_);

    // Populate the interface with various foo commands:

    {
      foo_test * test = new foo_test(_grab_target());
      test->initialize_simple();
      add_command(test, "test");
    }

    {
      foo_set_value * set_value = new foo_set_value(_grab_target());
      set_value->initialize_simple();
      add_command(set_value, "set_value");
    }

    {
      foo_get_value * get_value = new foo_get_value(_grab_target());
      get_value->initialize_simple();
      add_command(get_value, "get_value");
    }

    {
      foo_zero * zero = new foo_zero(_grab_target());
      zero->initialize_simple();
      add_command(zero, "zero");
    }

    {
      foo_set_sum * set_sum = new foo_set_sum(_grab_target());
      set_sum->initialize_simple();
      add_command(set_sum, "set_sum");
    }

    return;
  }

  //! Reset
  virtual void reset()
  {
    DT_THROW_IF(!is_initialized(),
                std::logic_error,
                "Command interface is not initialized!");

    remove_command("set_value");
    remove_command("get_value");
    remove_command("zero");
    remove_command("set_sum");
    remove_command("test");

    _base_reset();
    return;
  }

};

#endif // DATATOOLS_TESTING_UI_FOO_COMMAND_INTERFACE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

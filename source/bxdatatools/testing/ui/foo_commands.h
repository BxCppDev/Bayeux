//! \file  ui/foo_commands.h
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

#ifndef DATATOOLS_TESTING_UI_FOO_COMMANDS_H
#define DATATOOLS_TESTING_UI_FOO_COMMANDS_H

// This project:
#include <datatools/ui/target_command.h>
#include <datatools/detail/command_macros.h>
#include "foo.h"

//! Command
class foo_test
  : public datatools::ui::target_command<foo>
{
public:

  foo_test(foo & foo_)
    : datatools::ui::target_command<foo>(foo_, "test", "Test", datatools::version_id(1, 0))
  {
    return;
  }

  virtual ~foo_test()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

protected:

  virtual void _run(datatools::command::returned_info & cri_, uint32_t /*flags_ = 0*/)
  {
    cri_.reset();
    cri_.set_error_code(datatools::command::CEC_SUCCESS);

    DT_LOG_TRACE(get_logging_priority(), "Running command '" << get_name() << "'...");
    try {
      DT_LOG_TRACE(get_logging_priority(), "Test...");
      _grab_target().test();
    } catch (std::exception & error) {
      DT_COMMAND_RETURNED_ERROR(cri_,
                                datatools::command::CEC_FAILURE,
                                get_name() + ": " + error.what());
    }

    DT_LOG_TRACE(get_logging_priority(), "End of command '" << get_name() << "'.");
    return;
  }

};

//! Command
class foo_set_value
  : public datatools::ui::target_command<foo>
{
public:

  foo_set_value(foo & foo_)
    : datatools::ui::target_command<foo>(foo_, "set_value", "Set the value", datatools::version_id(1, 0))
  {
    return;
  }

  virtual ~foo_set_value()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

protected:

  virtual void _init(const datatools::properties & config_)
  {
    this->base_command::_init(config_);

    // Arguments description:
    _grab_opts().add_options()

      ("value",
       boost::program_options::value<int>()
       ->value_name("value"),
       "Integer value to be set\n"
       "Examples: \n"
       "  set_value --value 23 \n"
       "  set_value 23"
       )

      ; // end of options description

    // Positional options:
    _grab_args().add("value", 1);

    return;
  }

  virtual void _fini()
  {
    this->base_command::_fini();
    return;
  }

  virtual void _run(datatools::command::returned_info & cri_, uint32_t /*flags_ = 0*/)
  {
    cri_.reset();
    cri_.set_error_code(datatools::command::CEC_SUCCESS);

    DT_LOG_TRACE(get_logging_priority(), "Running command '" << get_name() << "'...");
    try {
      int value;

      DT_LOG_TRACE(get_logging_priority(), "Fetching the value...");
      if (!_grab_vmap().count("value")) {
        DT_COMMAND_RETURNED_ERROR(cri_,
                                  datatools::command::CEC_COMMAND_INVALID_NUMBER_OF_ARGUMENTS,
                                  get_name() + ": " + "Missing value argument!");
      }

      // Set the value:
      if (cri_.is_success()) {
        DT_LOG_TRACE(get_logging_priority(), "Decoding the value...");
        value = _grab_vmap()["value"].as<int>();
        DT_LOG_TRACE(get_logging_priority(), "Setting the value...");
        _grab_target().set_value(value);
      }

    } catch (std::exception & error) {
      DT_COMMAND_RETURNED_ERROR(cri_,
                                datatools::command::CEC_FAILURE,
                                get_name() + ": " + error.what());
    }

    DT_LOG_TRACE(get_logging_priority(), "End of command '" << get_name() << "'.");
    return;
  }

};

//! Command
class foo_set_sum
  : public datatools::ui::target_command<foo>
{
public:

  foo_set_sum(foo & foo_)
    : datatools::ui::target_command<foo>(foo_, "set_sum", "Set the value to the sum of values", datatools::version_id(1, 0))
  {
    return;
  }

  virtual ~foo_set_sum()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

protected:

  virtual void _init(const datatools::properties & config_)
  {
    this->base_command::_init(config_);

    // Arguments description:
    _grab_opts().add_options()

      ("values",
       boost::program_options::value<std::vector<int> >()
       ->multitoken()
       ->value_name("values"),
       "The list of integer values to be sum up"
       )

      ; // end of options description

    // Positional options:
    _grab_args().add("values", -1);

    return;
  }

  virtual void _fini()
  {
    this->base_command::_fini();
    return;
  }

  virtual void _run(datatools::command::returned_info & cri_, uint32_t /*flags_ = 0*/)
  {
    cri_.reset();
    cri_.set_error_code(datatools::command::CEC_SUCCESS);

    // DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "FORCE PRINT USAGE...");
    // print_usage(std::cerr);

    DT_LOG_TRACE(get_logging_priority(), "Running command '" << get_name() << "'...");
    try {
      boost::optional<std::vector<int> > values;

      DT_LOG_TRACE(get_logging_priority(), "Fetching the value...");
      if (!_grab_vmap().count("values")) {
        DT_COMMAND_RETURNED_ERROR(cri_,
                                  datatools::command::CEC_COMMAND_INVALID_NUMBER_OF_ARGUMENTS,
                                  get_name() + ": " + "Missing values argument!");
      }

      // Set the value:
      if (cri_.is_success()) {
        DT_LOG_TRACE(get_logging_priority(), "Decoding the values...");
        values = _grab_vmap()["values"].as<std::vector<int> >();
        DT_LOG_TRACE(get_logging_priority(), "Setting the values...");
        _grab_target().set_sum(values.get());
      }

    } catch (std::exception & error) {
      DT_COMMAND_RETURNED_ERROR(cri_,
                                datatools::command::CEC_FAILURE,
                                get_name() + ": " + error.what());
    }

    DT_LOG_TRACE(get_logging_priority(), "End of command '" << get_name() << "'.");
    return;
  }

};

//! Command
class foo_get_value
  : public datatools::ui::const_target_command<foo>
{
public:

  foo_get_value(foo & foo_)
    : datatools::ui::const_target_command<foo>(foo_,
                                               "get_value",
                                               "Get the value",
                                               datatools::version_id(1, 0))
  {
    return;
  }

  virtual ~foo_get_value()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

protected:

  virtual void _init(const datatools::properties & config_)
  {
    this->base_command::_init(config_);

    // Arguments description:
    _grab_opts().add_options()
      ; // end of options description

    return;
  }

  virtual void _run(datatools::command::returned_info & cri_, uint32_t /*flags_ = 0*/)
  {
    cri_.reset();
    cri_.set_error_code(datatools::command::CEC_SUCCESS);

    DT_LOG_TRACE(get_logging_priority(), "Running command '" << get_name() << "'...");
    try {
      DT_LOG_TRACE(get_logging_priority(), "Getting the value...");
      int the_value = _get_target().get_value();
      std::cout << the_value << std::endl;
    } catch (std::exception & error) {
      DT_COMMAND_RETURNED_ERROR(cri_,
                                datatools::command::CEC_FAILURE,
                                get_name() + ": " + error.what());
    }

    DT_LOG_TRACE(get_logging_priority(), "End of command '" << get_name() << "'.");
    return;
  }

};

//! Command
class foo_zero
  : public datatools::ui::target_command<foo>
{
public:

  foo_zero(foo & foo_)
    : datatools::ui::target_command<foo>(foo_, "zero", "Set the value to zero", datatools::version_id(1, 0))
  {
    return;
  }

  virtual ~foo_zero()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

protected:

  virtual void _run(datatools::command::returned_info & cri_, uint32_t /*flags_ = 0*/)
  {
    cri_.reset();
    cri_.set_error_code(datatools::command::CEC_SUCCESS);

    DT_LOG_TRACE(get_logging_priority(), "Running command '" << get_name() << "'...");
    try {
      DT_LOG_TRACE(get_logging_priority(), "Zero the value...");
      _grab_target().zero();
    } catch (std::exception & error) {
      DT_COMMAND_RETURNED_ERROR(cri_,
                                datatools::command::CEC_FAILURE,
                                get_name() + ": " + error.what());
    }

    DT_LOG_TRACE(get_logging_priority(), "End of command '" << get_name() << "'.");
    return;
  }

};

#endif // DATATOOLS_TESTING_UI_FOO_COMMANDS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

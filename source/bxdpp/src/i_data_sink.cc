/* i_data_sink.cc
 *
 * Copyright (C) 2011-2013 Xavier Garrido <garrido@lal.in2p3.fr>
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
 */


#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/exception.h>

#include <dpp/dpp_config.h>
#include <dpp/i_data_sink.h>

namespace dpp {

  i_data_sink::sink_record::sink_record (const std::string & a_label)
  {
    reset ();
    label = a_label;
    return;
  }

  void i_data_sink::sink_record::reset ()
  {
    label           = "";
    effective_label = "";
    status          = STATUS_CLOSED;
    return;
  }

  void i_data_sink::set_logging_priority(datatools::logger::priority a_priority)
  {
    _logging = a_priority;
    return;
  }

  datatools::logger::priority i_data_sink::get_logging_priority() const
  {
    return _logging;
  }

  void i_data_sink::set_preserve_existing_sink (bool a_value)
  {
    _preserve_existing_sink = a_value;
    return;
  }

  bool i_data_sink::is_preserve_existing_sink () const
  {
    return _preserve_existing_sink;
  }

  bool i_data_sink::is_open () const
  {
    return _sink_record.status == sink_record::STATUS_OPENED;
  }

  void i_data_sink::set (const std::string & a_sink_label)
  {
    DT_THROW_IF (! _sink_record.label.empty (),
                 std::logic_error,
                 "A sink labelled '" << _sink_record.label << "' is already in use !");
    _sink_record.label           = a_sink_label;
    std::string effective_label  = a_sink_label;
    datatools::fetch_path_with_env (effective_label);
    _sink_record.effective_label = effective_label;
    _sink_record.status          = sink_record::STATUS_CLOSED;
    return;
  }

  void i_data_sink::set_defaults_ (datatools::logger::priority a_priority)
  {
    _logging = a_priority;
    _preserve_existing_sink = false;
    return;
  }

  bool i_data_sink::is_sequential () const
  {
    return true;
  }

  bool i_data_sink::is_random () const
  {
    return false;
  }

  // ctor:
  i_data_sink::i_data_sink (datatools::logger::priority a_priority)
  {
    this->set_defaults_ (a_priority);
    return;
  }

  // ctor:
  i_data_sink::i_data_sink (const std::string & a_sink_label, datatools::logger::priority a_priority)
  {
    this->set_defaults_ (a_priority);
    this->set (a_sink_label);
    return;
  }

  // dtor:
  i_data_sink::~i_data_sink ()
  {
    return;
  }

}  // end of namespace dpp

// end of i_data_sink.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

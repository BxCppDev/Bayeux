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

// Ourselves:
#include <dpp/i_data_sink.h>

// Third party:
// - Boost:
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/exception.h>

// This project:
#include <dpp/dpp_config.h>

namespace dpp {

  // static
  const int i_data_sink::sink_record::STATUS_CLOSED;
  const int i_data_sink::sink_record::STATUS_OPENED;

  i_data_sink::sink_record::sink_record(const std::string & label_)
  {
    reset();
    label = label_;
    return;
  }

  void i_data_sink::sink_record::reset()
  {
    label           = "";
    effective_label = "";
    status          = STATUS_CLOSED;
    return;
  }

  void i_data_sink::set_logging_priority(datatools::logger::priority priority_)
  {
    _logging = priority_;
    return;
  }

  datatools::logger::priority i_data_sink::get_logging_priority() const
  {
    return _logging;
  }

  void i_data_sink::set_preserve_existing_sink(bool value_)
  {
    _preserve_existing_sink = value_;
    return;
  }

  bool i_data_sink::is_preserve_existing_sink() const
  {
    return _preserve_existing_sink;
  }

  bool i_data_sink::is_open() const
  {
    return _sink_record.status == sink_record::STATUS_OPENED;
  }

  void i_data_sink::set(const std::string & sink_label_)
  {
    DT_THROW_IF(! _sink_record.label.empty(),
                std::logic_error,
                "A sink labelled '" << _sink_record.label << "' is already in use !");
    _sink_record.label           = sink_label_;
    std::string effective_label  = sink_label_;
    datatools::fetch_path_with_env(effective_label);
    _sink_record.effective_label = effective_label;
    _sink_record.status          = sink_record::STATUS_CLOSED;
    return;
  }

  void i_data_sink::_set_defaults(datatools::logger::priority priority_)
  {
    _logging = priority_;
    _preserve_existing_sink = false;
    return;
  }

  bool i_data_sink::is_sequential() const
  {
    return true;
  }

  bool i_data_sink::is_random() const
  {
    return false;
  }

  i_data_sink::i_data_sink(datatools::logger::priority priority_)
  {
    this->_set_defaults(priority_);
    return;
  }

  i_data_sink::i_data_sink(const std::string & sink_label_, datatools::logger::priority priority_)
  {
    this->_set_defaults(priority_);
    this->set(sink_label_);
    return;
  }

  i_data_sink::~i_data_sink()
  {
    return;
  }

}  // end of namespace dpp

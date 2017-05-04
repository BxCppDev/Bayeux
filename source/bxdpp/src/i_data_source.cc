/* i_data_source.cc
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

// Third party:
// - Boost:
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/exception.h>

// Ourselves:
#include <dpp/i_data_source.h>

namespace dpp {

  // static
  const int64_t i_data_source::UNKNOWN_NUMBER_OF_ENTRIES;
  const int i_data_source::source_record::STATUS_CLOSED;
  const int i_data_source::source_record::STATUS_OPENED;

  i_data_source::source_record::source_record(const std::string & a_label)
  {
    reset();
    label = a_label;
    return;
  }

  void i_data_source::source_record::reset()
  {
    label           = "";
    effective_label = "";
    status          = STATUS_CLOSED;
    processed       = false;
    return;
  }

  void i_data_source::set_logging_priority(datatools::logger::priority a_priority)
  {
    _logging = a_priority;
    return;
  }

  datatools::logger::priority i_data_source::get_logging_priority() const
  {
    return _logging;
  }

  bool i_data_source::is_open() const
  {
    return _source_record.status == source_record::STATUS_OPENED;
  }

  void i_data_source::set(const std::string & a_source_label)
  {
    DT_THROW_IF(! _source_record.label.empty(),
                 std::logic_error,
                 "A source labelled '" << _source_record.label
                 << "' is already in use !");
    _source_record.label           = a_source_label;
    std::string effective_label         = a_source_label;
    datatools::fetch_path_with_env(effective_label);
    _source_record.effective_label = effective_label;
    _source_record.status          = source_record::STATUS_CLOSED;
    _source_record.processed       = false;
    return;
  }

  bool i_data_source::_load_record(datatools::things & /*a_event_record*/,
                                    int64_t /*a_entry*/)
  {
    DT_LOG_ERROR(get_logging_priority(),
                 "Load by entry number is not supported !");
    return false;
  }

  bool i_data_source::has_number_of_entries() const
  {
    return get_number_of_entries() > UNKNOWN_NUMBER_OF_ENTRIES;
  }

  int64_t i_data_source::_get_number_of_entries() const
  {
    return UNKNOWN_NUMBER_OF_ENTRIES;
  }

  int64_t i_data_source::get_number_of_entries() const
  {
    int noe = _get_number_of_entries();
    if (noe <= UNKNOWN_NUMBER_OF_ENTRIES) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                   "Cannot determine the number of entries !");
    }
    return noe;
  }

  bool i_data_source::can_load_record(int64_t a_entry)
  {
    if (! is_open()) {
      return false;
    }
    if (! is_random()) {
      return false;
    }
    if (! has_number_of_entries()) {
      return false;
    }
    if (a_entry < 0) return false;
    if (a_entry >= get_number_of_entries()) return false;
    return true;
  }

  bool i_data_source::load_record(datatools::things & a_event_record,
                                   int64_t a_entry)
  {
    if (! can_load_record(a_entry)) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                   "Cannot access a random entry #" << a_entry << " !");
      return false;
    }
    return _load_record(a_event_record, a_entry);
  }

  void i_data_source::_set_defaults(datatools::logger::priority a_priority)
  {
    _logging = a_priority;
    _has_next_record = false;
    return;
  }

  bool i_data_source::is_sequential() const
  {
    return true;
  }

  bool i_data_source::is_random() const
  {
    return false;
  }

  i_data_source::i_data_source(datatools::logger::priority a_priority)
  {
    this->_set_defaults(a_priority);
    return;
  }

  i_data_source::i_data_source(const std::string & a_source_label,
                                datatools::logger::priority a_priority)
  {
    this->_set_defaults(a_priority);
    this->set(a_source_label);
    return;
  }

  i_data_source::~i_data_source()
  {
    return;
  }

}  // end of namespace dpp

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

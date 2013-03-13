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
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <datatools/utils.h>
#include <datatools/ioutils.h>

#include <dpp/i_data_source.h>

namespace dpp {

  i_data_source::source_record::source_record (const std::string & a_label)
  {
    reset ();
    label = a_label;
    return;
  }

  void i_data_source::source_record::reset ()
  {
    label           = "";
    effective_label = "";
    status          = STATUS_CLOSED;
    processed       = false;
    return;
  }

  bool
  i_data_source::is_debug () const
  {
    return _debug_level > 0;
  }

  int
  i_data_source::get_debug_level () const
  {
    return _debug_level;
  }

  void
  i_data_source::set_debug_level (int a_debug_level)
  {
    _debug_level = a_debug_level;
    return;
  }

  bool i_data_source::is_open () const
  {
    return _source_record.status == source_record::STATUS_OPENED;
  }

  void i_data_source::set (const std::string & a_source_label)
  {
    if (! _source_record.label.empty ())
      {
        std::ostringstream message;
        message << "dpp::i_data_source::set:"
                << "A source labelled '" << _source_record.label
                << "' is already in use !";
        throw std::logic_error (message.str ());
      }
    _source_record.label           = a_source_label;
    std::string effective_label         = a_source_label;
    datatools::fetch_path_with_env (effective_label);
    _source_record.effective_label = effective_label;
    _source_record.status          = source_record::STATUS_CLOSED;
    _source_record.processed       = false;
    return;
  }

  bool i_data_source::_load_record (datatools::things & a_event_record, 
                                    int64_t a_entry)
  {
    std::ostringstream message;
    message << "dpp::i_data_source::_load_record: "
            << "Load by entry number is not supported !";
    std::cerr << datatools::io::error
              << message.str () << std::endl;
    return false;
  }

  bool i_data_source::has_number_of_entries () const
  {
    return get_number_of_entries () > UNKNOWN_NUMBER_OF_ENTRIES;
  }

  int64_t i_data_source::_get_number_of_entries () const
  {
    return UNKNOWN_NUMBER_OF_ENTRIES;
  }

  int64_t i_data_source::get_number_of_entries () const
  {
    int noe = _get_number_of_entries ();
    if (noe <= UNKNOWN_NUMBER_OF_ENTRIES)
      {
        std::ostringstream message;
        message << "dpp::i_data_source::get_number_of_entries: "
                << "Cannot determine the number of entries !";
        std::cerr << datatools::io::error
                  << message.str () << std::endl;
      }
    return noe;
  }

  bool i_data_source::can_load_record (int64_t a_entry)
  {
    if (! is_open ())
      {
        return false;
      }
    if (! is_random ())
      {
        return false;
      }
    if (! has_number_of_entries ())
      {
        return false;
      }
    if (a_entry < 0) return false;
    if (a_entry >= get_number_of_entries ()) return false;
    return true;
  }

  bool i_data_source::load_record (datatools::things & a_event_record, 
                                   int64_t a_entry)
  {
    if (! can_load_record (a_entry))
      {
        std::cerr << datatools::io::error
                  << "dpp::i_data_source::load_record: "
                  << "Cannot access a random entry #" << a_entry << " !" 
                  << std::endl;           
        return false;
      }
    return _load_record (a_event_record, a_entry);
  }

  void i_data_source::_set_defaults (uint32_t a_flags)
  {
    bool local_devel = false;
    if (local_devel)
      {
        std::cerr << "DEVEL: "
                  << "dpp::i_data_source::_set_defaults: "
                  << "a_flags == " << a_flags << std::endl;
      }
    _debug_level = 0;
    if (a_flags & debug)
      {
        _debug_level = 1;
        if (local_devel)
          {
            std::cerr << "DEVEL: debug "
                      << "dpp::i_data_source::_set_defaults: "
                      << "a_flags == " << a_flags << std::endl;
          }
      }
    _has_next_record = false;
    return;
  }

  bool i_data_source::is_sequential () const
  {
    return true;
  }

  bool i_data_source::is_random () const
  {
    return false;
  }

  // ctor:
  i_data_source::i_data_source (uint32_t a_flags)
  {
    this->_set_defaults (a_flags);
    return;
  }

  i_data_source::i_data_source (const std::string & a_source_label,
                                uint32_t a_flags)
  {
    this->_set_defaults (a_flags);
    this->set (a_source_label);
    return;
  }

  // dtor:
  i_data_source::~i_data_source ()
  {
    return;
  }

}  // end of namespace dpp

// end of i_data_source.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

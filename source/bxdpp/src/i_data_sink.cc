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

#include <dpp/i_data_sink.h>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <dpp/dpp_config.h>

#if DPP_DATATOOLS_LEGACY == 1
#include <datatools/utils/utils.h>
#include <datatools/utils/ioutils.h>
#else
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#endif

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

  bool
  i_data_sink::is_debug () const
  {
    return _debug_level > 0;
  }

  int
  i_data_sink::get_debug_level () const
  {
    return _debug_level;
  }

  void
  i_data_sink::set_debug_level (int a_debug_level)
  {
    _debug_level = a_debug_level;
    return;
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
    if (! _sink_record.label.empty ())
      {
        std::ostringstream message;
        message << "dpp::i_data_sink::set:"
                << "A sink labelled '" << _sink_record.label
                << "' is already in use !";
        throw std::logic_error (message.str ());
      }
    _sink_record.label           = a_sink_label;
    std::string effective_label  = a_sink_label;
    DPP_DU::fetch_path_with_env (effective_label);
    _sink_record.effective_label = effective_label;
    _sink_record.status          = sink_record::STATUS_CLOSED;
    return;
  }

  void i_data_sink::set_defaults_ (uint32_t a_flags)
  {
    bool local_devel = false;
    if (local_devel)
      {
        std::cerr << "DEVEL: "
             << "dpp::i_data_sink::set_defaults_: "
             << "a_flags == " << a_flags << std::endl;
      }
    _debug_level = 0;
    if (a_flags & debug)
      {
        _debug_level = 1;
        if (local_devel)
          {
            std::cerr << "DEVEL: "
                 << "dpp::i_data_sink::set_defaults_: "
                 << "a_flags == " << a_flags << std::endl;
          }
      }
    _preserve_existing_sink = false;
    if (a_flags & preserve_existing_sink)
      {
        _preserve_existing_sink = true;
      }
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
  i_data_sink::i_data_sink (uint32_t a_flags)
  {
    this->set_defaults_ (a_flags);
    return;
  }

  // ctor:
  i_data_sink::i_data_sink (const std::string & a_sink_label, uint32_t a_flags)
  {
    this->set_defaults_ (a_flags);
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

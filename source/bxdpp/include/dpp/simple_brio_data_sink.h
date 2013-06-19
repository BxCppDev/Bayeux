/* simple_brio_data_sink.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2013-02-16
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *
 * Description:
 *
 *   Generic data sink
 *
 * History:
 *
 */

#ifndef DPP_SIMPLE_BRIO_DATA_SINK_H_
#define DPP_SIMPLE_BRIO_DATA_SINK_H_ 1

#include <string>

#include <dpp/dpp_config.h>

#include <datatools/things.h>

#include <dpp/i_data_sink.h>

// forward declaration:
namespace brio {
  class writer;
}

namespace dpp {

  /// \brief A brio-based I/O data sink/writer
  class simple_brio_data_sink : public i_data_sink
  {

  protected:

    void _open_file_sink ();

    void _close_file_sink ();

  public:

    virtual bool is_random () const;

    virtual void open ();

    virtual bool store_next_record (const datatools::things & a_event_record);

    virtual void close ();

    virtual void reset ();

  public:

    // ctor:
    simple_brio_data_sink (datatools::logger::priority a_priority = datatools::logger::PRIO_NOTICE);


    // ctor:
    simple_brio_data_sink (const std::string & a_sink_label = "",
                           datatools::logger::priority a_priority = datatools::logger::PRIO_NOTICE);

    // dtor:
    virtual ~simple_brio_data_sink ();

  private:

    brio::writer * _brio_file_writer_;

  };

}  // end of namespace dpp

#endif // DPP_SIMPLE_BRIO_DATA_SINK_H_

// end of simple_brio_data_sink.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

/* simple_data_source.h
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   Generic sequential data source reader.
 *
 * History:
 *
 */
/**
 *
 * Standard BOOST I/O file format for SuperNEMO data is :
 *
 *   Data: event record as 'snemo:core::model::event_record' (things)
 *    - event record # 0
 *    - event record # 1
 *    - event record # 2
 *    - ...
 *    - event record # N
 *
 *
 */
#ifndef DPP_SIMPLE_DATA_SOURCE_H_
#define DPP_SIMPLE_DATA_SOURCE_H_ 1

#include <string>

#include <dpp/dpp_config.h>

#include <datatools/things.h>

#include <dpp/i_data_source.h>

// forward declaration:
namespace datatools {
  class data_reader;
}

namespace dpp {

  /// \brief A Boost/Serialization-based I/O data source/reader
  class simple_data_source : public i_data_source
  {

  public:

    virtual void open ();

    virtual bool has_next_record ();

    virtual bool load_next_record (datatools::things & a_event_record);

    virtual void close ();

    virtual void reset ();

    // ctor:
    simple_data_source (datatools::logger::priority a_priority = datatools::logger::PRIO_NOTICE);

    // ctor:
    simple_data_source (const std::string & a_source_label,
                        datatools::logger::priority a_priority = datatools::logger::PRIO_NOTICE);

    // dtor:
    virtual ~simple_data_source ();

  protected:

    virtual void _open_file_source ();

    virtual void _close_file_source ();

    virtual void _check_next_record ();

    // virtual int64_t _get_number_of_entries () const;

    // virtual bool _load_record (datatools::things & a_event_record, int64_t a_entry);

  private:

    datatools::data_reader * _boost_io_file_reader_;

  };

}  // end of namespace dpp

#endif // DPP_SIMPLE_DATA_SOURCE_H_

// end of simple_data_source.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

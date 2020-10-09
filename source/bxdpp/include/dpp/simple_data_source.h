/// \file dpp/simple_data_source.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 */

/**
 *
 * Standard BOOST I/O file format for SuperNEMO data is :
 *
 *   Metadata: medadata as 'datatools::properties'
 *    - metadata # 0
 *    - metadata # 1
 *    - ...
 *    - metadata # M
 *   Data: event record as 'datatools::things'
 *    - event record # 0
 *    - event record # 1
 *    - event record # 2
 *    - ...
 *    - event record # N
 *
 *
 */
#ifndef DPP_SIMPLE_DATA_SOURCE_H
#define DPP_SIMPLE_DATA_SOURCE_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/things.h>

// This project:
#include <dpp/dpp_config.h>
#include <dpp/i_data_source.h>

// forward declaration:
namespace datatools {
  class data_reader;
}

namespace dpp {

  /// \brief A Boost/Serialization-based I/O data source/reader
  class simple_data_source
    : public i_data_source
  {

  public:

    void open() override;

    bool has_next_record() override;

    bool load_next_record(datatools::things & a_event_record_) override;

    int64_t get_number_of_metadata() const override;

    bool load_metadata(datatools::properties & a_metadata_, int64_t a_entry_) override;

    void close() override;

    void reset() override;

    simple_data_source(datatools::logger::priority a_priority_ = datatools::logger::PRIO_NOTICE);

    simple_data_source(const std::string & a_source_label_,
                       datatools::logger::priority a_priority_ = datatools::logger::PRIO_NOTICE);

    ~simple_data_source() override;

  protected:

    virtual void _open_file_source();

    virtual void _close_file_source();

    void _check_next_record() override;

  private:

    std::vector<datatools::properties> _metadata_col_;
    datatools::data_reader * _boost_io_file_reader_;

  };

}  // end of namespace dpp

#endif // DPP_SIMPLE_DATA_SOURCE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

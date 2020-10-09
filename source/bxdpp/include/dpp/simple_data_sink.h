/// \file dpp/simple_data_sink.h
/* Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 */

#ifndef DPP_SIMPLE_DATA_SINK_H
#define DPP_SIMPLE_DATA_SINK_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/things.h>

// This project:
#include <dpp/dpp_config.h>
#include <dpp/i_data_sink.h>

// forward declaration:
namespace datatools {
  class data_writer;
}

namespace dpp {

  /// \brief A Boost/Serialization-based I/O data sink/writer
  class simple_data_sink
    : public i_data_sink
  {

  public:

    void open() override;

    bool store_next_record(const datatools::things & event_record_) override;

    bool can_store_meta_data() const override;

    bool store_metadata(const datatools::properties & meta_data_) override;

    void close() override;

    void reset() override;

    /// Constructor
    simple_data_sink(datatools::logger::priority priority_ = datatools::logger::PRIO_NOTICE);


    /// Constructor
    simple_data_sink(const std::string & sink_label_ = "",
                      datatools::logger::priority priority_ = datatools::logger::PRIO_NOTICE);

    /// Destructor
    ~simple_data_sink() override;

  protected:

    void _open_file_sink();

    void _close_file_sink();

  private:

    unsigned int _record_counter_;
    unsigned int _metadata_counter_;
    datatools::data_writer * _boost_io_file_writer_ = nullptr;

  };

}  // end of namespace snemo

#endif // DPP_SIMPLE_SIMPLE_DATA_SINK_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

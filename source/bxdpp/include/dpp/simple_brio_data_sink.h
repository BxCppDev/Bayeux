/// \file dpp/simple_brio_data_sink.h
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

#ifndef DPP_SIMPLE_BRIO_DATA_SINK_H
#define DPP_SIMPLE_BRIO_DATA_SINK_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/things.h>

// This project:
#include <dpp/dpp_config.h>
#include <dpp/i_data_sink.h>

// forward declaration:
namespace brio {
  class writer;
}

namespace dpp {

  /// \brief A brio-based I/O data sink/writer
  class simple_brio_data_sink
    : public i_data_sink
  {

  protected:

    void _open_file_sink();

    void _close_file_sink();

  public:

    bool is_random() const override;

    void open() override;

    bool store_next_record(const datatools::things & a_event_record) override;

    bool can_store_meta_data() const override;

    bool store_metadata(const datatools::properties & a_meta_data) override;

    void close() override;

    void reset() override;

  public:

    /// Constructor
    simple_brio_data_sink(datatools::logger::priority a_priority = datatools::logger::PRIO_NOTICE);


    /// Constructor
    simple_brio_data_sink(const std::string & a_sink_label = "",
                          datatools::logger::priority a_priority = datatools::logger::PRIO_NOTICE);

    /// Destructor
    ~simple_brio_data_sink() override;

  private:

    brio::writer * _brio_file_writer_; //!< handle to the brio writer

  };

}  // end of namespace dpp

#endif // DPP_SIMPLE_BRIO_DATA_SINK_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

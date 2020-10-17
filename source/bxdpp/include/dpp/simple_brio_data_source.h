/// \file dpp/simple_data_source.h
/* Author(s)     : Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date : 2011-08-05
 * Last modified : 2013-02-16
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
 *
 * Description:
 *
 *   Generic sequential data source reader for BRIO input file.
 *
 *   A special mode is available in order to read the output
 *   from the sng4_production program which does not use
 *   the 'things' (snemo::core::model::event_record) as
 *   the basis of the data model.
 *
 */

#ifndef DPP_SIMPLE_BRIO_DATA_SOURCE_H
#define DPP_SIMPLE_BRIO_DATA_SOURCE_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/things.h>

// This project:
#include <dpp/i_data_source.h>

// forward declaration:
/// \brief The brio library's main namespace
namespace brio {
  class reader;
}

namespace dpp {

  /// \brief A brio-based I/O data source/reader
  class simple_brio_data_source
    : public i_data_source
  {
  public:

    bool is_random() const override;

    void open() override;

    bool has_next_record() override;

    bool load_next_record(datatools::things & event_record_) override;

    int64_t get_number_of_metadata() const override;

    bool load_metadata(datatools::properties & metadata_, int64_t entry_) override;

    void close() override;

    void reset() override;

    simple_brio_data_source(datatools::logger::priority priority = datatools::logger::PRIO_NOTICE);

    simple_brio_data_source(const std::string & source_label_,
                            datatools::logger::priority priority_ = datatools::logger::PRIO_NOTICE);

    ~simple_brio_data_source() override;

  protected:

    virtual void _open_file_source();

    virtual void _close_file_source();

    void _check_next_record() override;

    int64_t _get_number_of_entries() const override;

    bool _load_record(datatools::things & event_record_, int64_t entry_) override;

  private:

    brio::reader * _brio_file_reader_ = nullptr;

  };

}  // end of namespace dpp

#endif // DPP_SIMPLE_BRIO_DATA_SOURCE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

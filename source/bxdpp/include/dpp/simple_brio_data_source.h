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

    virtual bool is_random() const;

    virtual void open();

    virtual bool has_next_record();

    virtual bool load_next_record(datatools::things & event_record_);

    virtual int64_t get_number_of_metadata() const;

    virtual bool load_metadata(datatools::properties & metadata_, int64_t entry_);

    virtual void close();

    virtual void reset();

    simple_brio_data_source(datatools::logger::priority priority = datatools::logger::PRIO_NOTICE);

    simple_brio_data_source(const std::string & source_label_,
                            datatools::logger::priority priority_ = datatools::logger::PRIO_NOTICE);

    virtual ~simple_brio_data_source();

  protected:

    virtual void _open_file_source();

    virtual void _close_file_source();

    virtual void _check_next_record();

    virtual int64_t _get_number_of_entries() const;

    virtual bool _load_record(datatools::things & event_record_, int64_t entry_);

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

/// \file dpp/i_data_sink.h
/* Author(s)     : Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date : 2011-08-05
 * Last modified : 2013-02-15
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
 * Description:
 *
 *   Abstract data sink reader class.
 *
 */

#ifndef DPP_I_DATA_SINK_H
#define DPP_I_DATA_SINK_H 1

// Standard library:
#include <ostream>
#include <string>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/things.h>

// This project:
#include <dpp/dpp_config.h>

namespace dpp {

  /// \brief Data sink/writer abstract interface
  class i_data_sink
  {
  public:

    /// \brief Internal class of the i_data_sink interface class
    struct sink_record
    {
      static const int STATUS_CLOSED = 0;
      static const int STATUS_OPENED = 1;
      std::string label;
      std::string effective_label;
      int         status;

      sink_record(const std::string & label_ = "");
      void reset();
    };

    /// Set logging priority
    void set_logging_priority(datatools::logger::priority p_);

    /// Returns logging priority
    datatools::logger::priority get_logging_priority() const;

    void set_preserve_existing_sink(bool value_);

    bool is_preserve_existing_sink() const;

    virtual bool is_open() const;

    virtual bool is_sequential() const;

    virtual bool is_random() const;

    virtual void set(const std::string & sink_label_);

    virtual void open() = 0;

    virtual bool store_next_record(const datatools::things & event_record_) = 0;

    virtual bool can_store_meta_data() const = 0;

    virtual bool store_metadata(const datatools::properties & meta_data_) = 0;

    virtual void close() = 0;

    virtual void reset() = 0;

    i_data_sink(datatools::logger::priority priority_ = datatools::logger::PRIO_ERROR);

    i_data_sink(const std::string & sink_label_,
                datatools::logger::priority priority_ = datatools::logger::PRIO_ERROR);

    virtual ~i_data_sink();

  protected:

    void _set_defaults(datatools::logger::priority priority_);

  protected:

    datatools::logger::priority _logging; //!< The logging priority
    bool        _preserve_existing_sink;  //!< Preserve flag
    sink_record _sink_record;             //!< Record dynamic informations about the data sink

  };

}  // end of namespace dpp

#endif // DPP_I_DATA_SINK_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

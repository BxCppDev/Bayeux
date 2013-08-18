/* simple_data_sink.cc
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
 */

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/things.h>
#include <datatools/io_factory.h>

#if DATATOOLS_WITH_BIO == 0
#include <datatools/archives_instantiation.h>
#include <datatools/properties.ipp>
#include <datatools/things.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL_IN(datatools::things)
#endif

#include <dpp/dpp_config.h>
#include <dpp/simple_data_sink.h>

namespace dpp {

  void simple_data_sink::reset ()
  {
    if (is_open ()) {
      this->simple_data_sink::close ();
    }
    _sink_record.reset ();
    return;
  }

  void simple_data_sink::close ()
  {
    if (_sink_record.status == sink_record::STATUS_CLOSED) {
      return;
    }
    if (_boost_io_file_writer_ != 0) {
      this->simple_data_sink::close_file_sink_ ();
    }
    return;
  }

  void simple_data_sink::open ()
  {
    if (_sink_record.status == sink_record::STATUS_OPENED) {
      return;
    }
    std::string label = _sink_record.effective_label;
    std::string file_name;
    bool   file_mode = false;
    bool   upload_mode = false;
    if (boost::find_first (label, "://")) {
      if (boost::starts_with (label, "file://")) {
        file_mode = true;
        file_name = label;
        boost::replace_first (file_name, "file://", "");
      } else if (boost::starts_with (label, "http://")) {
        upload_mode = true;
#if BOOST_FILESYSTEM_VERSION == 3
        file_name = boost::filesystem::path (label).filename ().string ();
#else
        file_name = boost::filesystem::path (label).filename ();
#endif
      } else if (boost::starts_with (label, "https://")) {
        upload_mode = true;
#if BOOST_FILESYSTEM_VERSION == 3
        file_name = boost::filesystem::path (label).filename ().string ();
#else
        file_name = boost::filesystem::path (label).filename ();
#endif
      } else if (boost::starts_with (label, "ftp://")) {
        upload_mode = true;
#if BOOST_FILESYSTEM_VERSION == 3
        file_name = boost::filesystem::path (label).filename ().string ();
#else
        file_name = boost::filesystem::path (label).filename ();
#endif
      } else {
        DT_THROW_IF(true,std::logic_error ,
                    "Sink labelled '" << _sink_record.effective_label << "' needs an unknown protocol !");
      }
    } else {
      file_mode = true;
      file_name = _sink_record.effective_label;
    }

    DT_THROW_IF (upload_mode, std::runtime_error,
                 "Sink file upload mode is not implemented yet !");
    if (file_mode) {
      this->simple_data_sink::open_file_sink_ ();
    }

    return;
  }

  void simple_data_sink::close_file_sink_ ()
  {
    if (_boost_io_file_writer_ != 0) {
      if (_boost_io_file_writer_->is_initialized()) {
        _boost_io_file_writer_->reset ();
      }
      delete _boost_io_file_writer_;
      _boost_io_file_writer_ = 0;
      _sink_record.status = sink_record::STATUS_CLOSED;
      _sink_record.reset ();
    }
    return;
  }

  void simple_data_sink::open_file_sink_ ()
  {
    namespace ds = datatools;
    if (boost::filesystem::exists (_sink_record.effective_label)) {
      DT_THROW_IF (is_preserve_existing_sink (),
                   std::runtime_error,
                   "File '" << _sink_record.effective_label << "' already exists !");
      DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                     "File '" << _sink_record.effective_label << "' already exists !");
    }
    int mode = 0;
    int status =
      ds::io_factory::guess_mode_from_filename (_sink_record.effective_label, mode);
    DT_THROW_IF (status == ds::io_factory::ERROR,
                 std::logic_error,
                 "File format not recognized for '" << _sink_record.effective_label << "' !");
    if (_boost_io_file_writer_ == 0) {
      _boost_io_file_writer_ = new ds::data_writer;
      _boost_io_file_writer_->init (_sink_record.effective_label,
                                    ds::using_multiple_archives);
      _sink_record.status = sink_record::STATUS_OPENED;
    }
    return;
  }

  bool simple_data_sink::store_next_record (const datatools::things & a_event_record)
  {
    bool done = false;
    if (_boost_io_file_writer_ != 0) {
      _boost_io_file_writer_->store (a_event_record);
      done = true;
    }
    return done;
  }

  simple_data_sink::simple_data_sink (datatools::logger::priority a_priority)
    : i_data_sink (a_priority)
  {
    this->simple_data_sink::set_defaults_ (a_priority);
    return;
  }

  simple_data_sink::simple_data_sink (const std::string & a_sink_label,
                                      datatools::logger::priority a_priority)
    : i_data_sink (a_sink_label, a_priority)
  {
    _boost_io_file_writer_ = 0;
    this->simple_data_sink::open ();
    return;
  }

  simple_data_sink::~simple_data_sink ()
  {
    this->simple_data_sink::reset ();
    return;
  }

}  // end of namespace dpp

// end of simple_data_sink.cc
/*
** Local Variables: --
** mode: c++ --
** indent-tabs-mode: nil --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

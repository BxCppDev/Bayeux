/* simple_data_source.cc
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
#include <datatools/exception.h>

#if DATATOOLS_WITH_BIO == 0
#include <datatools/archives_instantiation.h>
#include <datatools/things.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL_IN(datatools::things)
#endif

#include <dpp/dpp_config.h>
#include <dpp/simple_data_source.h>

namespace dpp {

  void simple_data_source::reset ()
  {
    if (is_open ()) {
      this->simple_data_source::close ();
    }
    _source_record.reset ();
    return;
  }

  void simple_data_source::close ()
  {
    // std::cerr << "DEVEL: dpp::simple_data_source::close: TEST" << std::endl;
    if (_source_record.status == source_record::STATUS_CLOSED) {
      return;
    }
    if (_boost_io_file_reader_ != 0) {
      this->simple_data_source::_close_file_source ();
    }
    _metadata_col_.clear();
    return;
  }

  void simple_data_source::open ()
  {
    //cerr << "DEVEL: dpp::simple_data_source::open: Entering..." << std::endl;
    std::string label = _source_record.effective_label;
    std::string file_name;
    bool   file_mode = false;
    bool   download_mode = false;
    if (boost::find_first (label, "://"))
      {
        if (boost::starts_with (label, "file://"))
          {
            file_mode = true;
            file_name = label;
            boost::replace_first (file_name, "file://", "");
          }
        else if (boost::starts_with (label, "http://"))
          {
            download_mode = true;
#if BOOST_FILESYSTEM_VERSION == 3
            file_name = boost::filesystem::path(label).filename ().string ();
#else
            file_name = boost::filesystem::path(label).filename ();
#endif
          }
        else if (boost::starts_with (label, "https://"))
          {
            download_mode = true;
#if BOOST_FILESYSTEM_VERSION == 3
            file_name = boost::filesystem::path(label).filename ().string ();
#else
            file_name = boost::filesystem::path(label).filename ();
#endif
          }
        else if (boost::starts_with (label, "ftp://"))
          {
            download_mode = true;
#if BOOST_FILESYSTEM_VERSION == 3
            file_name = boost::filesystem::path(label).filename ().string ();
#else
            file_name = boost::filesystem::path(label).filename ();
#endif
          }
        else
          {
            DT_THROW_IF(true,std::logic_error,
                        "Source labelled '" << _source_record.effective_label
                        << "' needs an unknown protocol !");
          }
      }
    else
      {
        // std::cerr << "DEVEL: dpp::simple_data_source::set: TEST 2: file_mode" << std::endl;
        file_mode = true;
        file_name = _source_record.effective_label;
      }

    DT_THROW_IF (download_mode,std::logic_error,
                 "Source file download mode is not implemented yet !");
    if (file_mode) {
      // std::cerr << "DEVEL: dpp::simple_data_source::set: TEST 3: invoke open_file_mode_" << std::endl;
      this->simple_data_source::_open_file_source ();
    }

    //cerr << "DEVEL: dpp::simple_data_source::set: Exiting." << std::endl;
    return;
  }

  void simple_data_source::_close_file_source ()
  {
    //cerr << "DEVEL: dpp::simple_data_source::_close_file_source: Entering..." << std::endl;
    if (_boost_io_file_reader_ != 0) {
      if (_boost_io_file_reader_->is_initialized()) {
        _boost_io_file_reader_->reset ();
      }
      delete _boost_io_file_reader_;
      _boost_io_file_reader_ = 0;
      _source_record.status = source_record::STATUS_CLOSED;
      _source_record.reset ();
      _has_next_record = false;
    }
    //cerr << "DEVEL: dpp::simple_data_source::_close_file_source: Exiting." << std::endl;
    return;
  }

  void simple_data_source::_open_file_source ()
  {
    namespace ds = datatools;
    //cerr << "DEVEL: dpp::simple_data_source::_open_file_source: Entering..." << std::endl;
    DT_THROW_IF (! boost::filesystem::exists (_source_record.effective_label),
                 std::logic_error,
                 "File '" << _source_record.effective_label << "' does not exist !");

    int mode = 0;
    int status =
      ds::io_factory::guess_mode_from_filename (_source_record.effective_label, mode);
    DT_THROW_IF (status == ds::io_factory::ERROR,
                 std::logic_error,
                  "File format not recognized for '"
                 << _source_record.effective_label << "' !");

    if (_boost_io_file_reader_ == 0) {
      _boost_io_file_reader_ = new ds::data_reader;
      DT_LOG_NOTICE (get_logging_priority (), "Label is '" << _source_record.effective_label << "'");
      _boost_io_file_reader_->init_multi (_source_record.effective_label);
      _source_record.status = source_record::STATUS_OPENED;
    }
    while (_boost_io_file_reader_->has_record_tag ()) {
      if (datatools::check_serial_tag<datatools::properties>(_boost_io_file_reader_->get_record_tag ())) {
        // Load metadata as many as possible:
        {
          datatools::properties pushed_metadata;
          _metadata_col_.push_back(pushed_metadata);
        }
        datatools::properties & new_metadata = _metadata_col_.back();
        _boost_io_file_reader_->load(new_metadata);
      } else {
        break;
      }
    }

    _has_next_record = false;
    _check_next_record ();
    return;
  }

  void simple_data_source::_check_next_record ()
  {
    _has_next_record = false;
    if (_boost_io_file_reader_ == 0) {
      return;
    }
    if (! _boost_io_file_reader_->has_record_tag ()) {
      return;
    }
    if (datatools::check_serial_tag<datatools::things>(_boost_io_file_reader_->get_record_tag ())) {
      _has_next_record = true;
      return;
    }
    return;
  }

  bool simple_data_source::has_next_record ()
  {
    DT_THROW_IF (_source_record.label.empty (),
                 std::logic_error,
                 "No source label is available !");
    if (_source_record.status == source_record::STATUS_CLOSED) {
      DT_LOG_NOTICE (get_logging_priority (), "Opening data source...");
      this->simple_data_source::open ();
    }
    return _has_next_record;
  }

  bool simple_data_source::load_next_record (datatools::things & a_event_record)
  {
    bool done = false;
    if (! _has_next_record) {
      return done;
    }
    if (_boost_io_file_reader_ != 0) {
      _boost_io_file_reader_->load (a_event_record);
      _check_next_record ();
      done = true;
    }
    return done;
  }

  int64_t simple_data_source::get_number_of_metadata() const
  {
    return _metadata_col_.size();
  }

  bool simple_data_source::load_metadata(datatools::properties & a_metadata, int64_t a_entry)
  {
    if (a_entry < 0 || a_entry >= _metadata_col_.size()) {
      return false;
    }
    a_metadata = _metadata_col_[a_entry];
    return true;
  }

  // ctor:
  simple_data_source::simple_data_source (datatools::logger::priority a_priority)
    : i_data_source (a_priority)
  {
    _boost_io_file_reader_ = 0;
    return;
  }

  // ctor:
  simple_data_source::simple_data_source (const std::string & a_source_label,
                                          datatools::logger::priority a_priority)
    : i_data_source (a_source_label, a_priority)
  {
    _boost_io_file_reader_ = 0;
    this->simple_data_source::open ();
    return;
  }

  // dtor:
  simple_data_source::~simple_data_source ()
  {
    this->simple_data_source::reset ();
    return;
  }

}  // end of namespace dpp

// end of simple_data_source.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

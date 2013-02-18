/* simple_brio_data_source.cc
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
 */

#include <dpp/dpp_config.h>
#include <dpp/simple_brio_data_source.h>
#include <dpp/brio_common.h>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#if DPP_DATATOOLS_LEGACY == 1
#include <datatools/utils/utils.h>
#include <datatools/utils/ioutils.h>
#include <datatools/utils/things.h>
#include <datatools/serialization/io_factory.h>
#else
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/things.h>
#include <datatools/io_factory.h>
#endif

#include <brio/reader.h>

#if DATATOOLS_WITH_BIO == 0
#if DPP_DATATOOLS_LEGACY == 1
#include <datatools/serialization/archives_instantiation.h>
#include <datatools/utils/things.ipp>
#else
#include <datatools/archives_instantiation.h>
#include <datatools/things.ipp>
#endif
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL_IN(DPP_DU::things)
#endif

namespace dpp {

  bool simple_brio_data_source::is_random () const
  {
    return true;
  }

  void simple_brio_data_source::reset ()
  {
    if (_source_record.status == source_record::STATUS_OPENED)
      {
        this->simple_brio_data_source::close ();
        _source_record.status = source_record::STATUS_CLOSED;
      }
    _source_record.reset ();
    return;
  }

  void simple_brio_data_source::close ()
  {
    // std::cerr << "DEVEL: simple_brio_data_source::close_source_: TEST" << std::endl;
    if (_source_record.status == source_record::STATUS_CLOSED)
      {
        return;
      }
    if (_brio_file_reader_ != 0)
      {
        this->simple_brio_data_source::_close_file_source ();
      }
    return;
  }

  void simple_brio_data_source::open ()
  {
    //cerr << "DEVEL: simple_brio_data_source::open_source_: Entering..." << std::endl;
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
            std::ostringstream message;
            message << "dpp::simple_brio_data_source::set:"
                    << "Source labelled '" << _source_record.effective_label
                    << "' needs an unknown protocol !";
            throw std::logic_error (message.str ());
          }
      }
    else
      {
        // std::cerr << "DEVEL: simple_brio_data_source::open_source_: TEST 2: file_mode" << std::endl;
        file_mode = true;
        file_name = _source_record.effective_label;
      }

    if (download_mode)
      {
        std::ostringstream message;
        message << "dpp::simple_brio_data_source::set:"
                << "Source file download mode is not implemented yet !";
        throw std::logic_error (message.str ());
      }

    if (file_mode)
      {
        // std::cerr << "DEVEL: simple_brio_data_source::open_source_: TEST 3: invoke _open_file_source" << std::endl;
        this->simple_brio_data_source::_open_file_source ();
      }

    //cerr << "DEVEL: simple_brio_data_source::open_source_: Exiting." << std::endl;
    return;
  }

  void simple_brio_data_source::_close_file_source ()
  {
    //cerr << "DEVEL: simple_brio_data_source::_close_file_source: Entering..." << std::endl;
    if (_brio_file_reader_ != 0)
      {
        _brio_file_reader_->close ();
        delete _brio_file_reader_;
        _brio_file_reader_ = 0;
        _source_record.reset ();
        _has_next_record = false;
      }
    //cerr << "DEVEL: simple_brio_data_source::_close_file_source: Exiting." << std::endl;
    return;
  }

  void simple_brio_data_source::_open_file_source ()
  {
    //cerr << "DEVEL: simple_brio_data_source::_open_file_source: Entering..." << std::endl;
    if (! boost::filesystem::exists (_source_record.effective_label))
      {
        std::ostringstream message;
        message << "dpp::simple_brio_data_source::_open_file_source: "
                << "File '" << _source_record.effective_label << "' does not exist !";
        throw std::logic_error (message.str ());
      }

    int mode;
    int status =
      brio::store_info::guess_mode_from_filename (_source_record.effective_label, mode);
    if (status == brio::store_info::ERROR)
      {
        std::ostringstream message;
        message << "dpp::simple_brio_data_source::_open_file_source:"
                << "File format not recognized for '"
                << _source_record.effective_label << "' !";
        throw std::logic_error (message.str ());
      }

    if (_brio_file_reader_ == 0)
      {
        _brio_file_reader_ = new brio::reader;
        std::clog << "NOTICE: dpp::simple_brio_data_source::_open_file_source: Label is '"
             << _source_record.effective_label << "'." << std::endl;
        _brio_file_reader_->open (_source_record.effective_label);

        // Try to find the 'general info' store :
        if (_brio_file_reader_->has_store_with_serial_tag (brio_common::GENERAL_INFO_STORE_LABEL,
                                                           datatools::utils::properties::SERIAL_TAG))
          {
            // If found, select it:
            _brio_file_reader_->select_store (brio_common::GENERAL_INFO_STORE_LABEL);

            // Note: here we could read all 'properties' record from the 'general info' store,
            // and store them in a 'multiproperties' container...
          }

        std::string checked_store = brio_common::EVENT_RECORD_STORE_LABEL;
        std::string checked_serial_tag = DPP_DU::things::SERIAL_TAG;
        if (! _brio_file_reader_->has_store_with_serial_tag (checked_store, checked_serial_tag))
          {
            _brio_file_reader_->print_info (std::cerr);
            std::ostringstream message;
            message << "dpp::simple_brio_data_source::_open_file_source: "
                    << "Cannot find a store with label '"
                    << checked_store << "' and serial tag '"
                    << checked_serial_tag
                    << "' in BRIO file '"
                    << _source_record.effective_label << "! ";
            throw std::logic_error (message.str ());
          }
        _brio_file_reader_->select_store (checked_store);
        _brio_file_reader_->print_info (std::clog);
        _source_record.status = source_record::STATUS_OPENED;
      }
    _has_next_record = false;
    _check_next_record ();
    return;
  }

  int64_t simple_brio_data_source::_get_number_of_entries () const
  {
    if (! is_open ())
      {
        return UNKNOWN_NUMBER_OF_ENTRIES;
      }
    if (_brio_file_reader_ == 0)
      {
        return UNKNOWN_NUMBER_OF_ENTRIES;
      }
    if (! _brio_file_reader_->is_opened ())
      {
        return UNKNOWN_NUMBER_OF_ENTRIES;
      }
    return _brio_file_reader_->get_number_of_entries ();
  }

  void simple_brio_data_source::_check_next_record ()
  {
    // std::cerr << "DEVEL: simple_brio_data_source::_check_next_record: Entering... " << std::endl;
    _has_next_record = false;

    std::string checked_label = brio_common::EVENT_RECORD_STORE_LABEL;
    _brio_file_reader_->select_store (checked_label);
    if (_brio_file_reader_->has_next ())
      {
        _has_next_record = true;
      }
    return;
  }

  bool simple_brio_data_source::has_next_record ()
  {
    if (_source_record.label.empty ())
      {
        std::ostringstream message;
        message << "dpp::simple_brio_data_source::has_next_record:"
                << "No source label is available !";
        throw std::logic_error (message.str ());
      }
    if (_source_record.status == source_record::STATUS_CLOSED)
      {
        std::clog << datatools::utils::io::notice
             << "dpp::simple_brio_data_source::has_next_record: "
             << "Opening data source..." << std::endl;
        this->simple_brio_data_source::open ();
      }
    return _has_next_record;
  }

  bool simple_brio_data_source::_load_record (DPP_DU::things & a_event_record,
                                              int64_t a_entry)
  {
    // XXX
    bool done = false;
    if (_brio_file_reader_ != 0)
      {
        int status = _brio_file_reader_->load (a_event_record, a_entry);
        if (status != 0)
          {
            return false;
          }
        _check_next_record ();
        done = true;
      }
    return done;
  }

  bool simple_brio_data_source::load_next_record (DPP_DU::things & a_event_record)
  {
    bool done = false;
    if (! _has_next_record)
      {
        return done;
      }
    if (_brio_file_reader_ != 0)
      {
        int status = _brio_file_reader_->load_next (a_event_record);
        if (status != 0)
          {
            return false;
          }
        _check_next_record ();
        done = true;
      }
    return done;
  }

  // ctor:
  simple_brio_data_source::simple_brio_data_source (uint32_t a_flags)
    : i_data_source (a_flags)
  {
    _brio_file_reader_ = 0;
    return;
  }

  // ctor:
  simple_brio_data_source::simple_brio_data_source (const std::string & a_source_label,
                                                    uint32_t a_flags)
    : i_data_source (a_source_label, a_flags)
  {
    _brio_file_reader_ = 0;
    this->open ();
    return;
  }

  // dtor:
  simple_brio_data_source::~simple_brio_data_source ()
  {
    this->reset ();
    return;
  }

}  // end of namespace dpp

// end of simple_brio_data_source.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

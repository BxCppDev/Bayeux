/* simple_brio_data_sink.cc
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

#include <dpp/dpp_config.h>
#include <dpp/simple_brio_data_sink.h>
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

#include <brio/writer.h>

#if DATATOOLS_WITH_BIO == 0
#if DPP_DATATOOLS_LEGACY == 1
#include <datatools/serialization/archives_instantiation.h>
#include <datatools/utils/properties.ipp>
#include <datatools/utils/things.ipp>
#else
#include <datatools/archives_instantiation.h>
#include <datatools/properties.ipp>
#include <datatools/things.ipp>
#endif
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL_IN(DPP_DU::things)
#endif

namespace dpp {

  bool simple_brio_data_sink::is_random () const
  {
    return true;
  }

  void simple_brio_data_sink::reset ()
  {
    if (is_open ())
      {
        this->simple_brio_data_sink::close ();
      }
    _sink_record.reset ();
    return;
  }

  void simple_brio_data_sink::close ()
  {
    // std::cerr << "DEVEL: dpp::simple_brio_data_sink::close: Entering..." << std::endl;
    if (_sink_record.status == sink_record::STATUS_CLOSED)
      {
        return;
      }
    if (_brio_file_writer_ != 0)
      {
        this->simple_brio_data_sink::_close_file_sink ();
      }
    return;
  }

  void simple_brio_data_sink::open ()
  {
    // std::cerr << "DEVEL: dpp::simple_brio_data_sink::open: Entering..." << std::endl;
    if (_sink_record.status == sink_record::STATUS_OPENED)
      {
        return;
      }
    std::string label = _sink_record.effective_label;
    std::string file_name;
    bool   file_mode = false;
    bool   upload_mode = false;
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
            upload_mode = true;
#if BOOST_FILESYSTEM_VERSION == 3
            file_name = boost::filesystem::path (label).filename ().string ();
#else
            file_name = boost::filesystem::path (label).filename ();
#endif
          }
        else if (boost::starts_with (label, "https://"))
          {
            upload_mode = true;
#if BOOST_FILESYSTEM_VERSION == 3
            file_name = boost::filesystem::path (label).filename ().string ();
#else
            file_name = boost::filesystem::path (label).filename ();
#endif
          }
        else if (boost::starts_with (label, "ftp://"))
          {
            upload_mode = true;
#if BOOST_FILESYSTEM_VERSION == 3
            file_name = boost::filesystem::path (label).filename ().string ();
#else
            file_name = boost::filesystem::path (label).filename ();
#endif
          }
        else
          {
            std::ostringstream message;
            message << "dpp::simple_brio_data_sink::_open_file_sink:"
                    << "Sink labelled '" << _sink_record.effective_label << "' needs an unknown protocol !";
            throw std::logic_error (message.str ());
          }
      }
    else
      {
        // std::cerr << "DEVEL: simple_brio_data_sink::_open_file_sink: TEST 2: file_mode" << std::endl;
        file_mode = true;
        file_name = _sink_record.effective_label;
      }

    if (upload_mode)
      {
        std::ostringstream message;
        message << "dpp::simple_brio_data_sink::_open_file_sink:"
                << "Sink file upload mode is not implemented yet !";
        throw std::logic_error (message.str ());
      }

    if (file_mode)
      {
        // std::cerr << "DEVEL: simple_brio_data_sink::_open_file_sink: TEST 3: invoke open_file_mode_" << std::endl;
        this->simple_brio_data_sink::_open_file_sink ();
      }

    return;
  }

  void simple_brio_data_sink::_close_file_sink ()
  {
    // std::cerr << "DEVEL: simple_brio_data_sink::_close_file_sink: TEST" << std::endl;
    if (_brio_file_writer_ != 0)
      {
        _brio_file_writer_->close ();
        delete _brio_file_writer_;
        _brio_file_writer_ = 0;
        _sink_record.status = sink_record::STATUS_CLOSED;
        _sink_record.reset ();
      }
    return;
  }

  void simple_brio_data_sink::_open_file_sink ()
  {
    // std::cerr << "DEVEL: simple_brio_data_sink::_open_file_sink: TEST" << std::endl;
    namespace ds = DPP_DSZ;
    if (boost::filesystem::exists (_sink_record.effective_label))
      {
        std::ostringstream message;
        message << "dpp::simple_brio_data_sink::_open_file_sink: "
                << "File '" << _sink_record.effective_label << "' already exists !";
        // std::cerr << "DEVEL: " << "preserve_existing_sink="<<preserve_existing_sink_<< std::endl;
        if (is_preserve_existing_sink ())
          {
            throw std::logic_error (message.str ());
          }
        else
          {
            std::clog << DPP_DU::io::warning << message.str () << std::endl;
          }
      }
    int mode = 0;
    int status =
      brio::store_info::guess_mode_from_filename (_sink_record.effective_label, mode);
    if (status == ds::io_factory::ERROR)
      {
        std::ostringstream message;
        message << "dpp::simple_brio_data_sink::_open_file_sink: "
                << "File format not recognized for '" << _sink_record.effective_label << "' !";
        throw std::logic_error (message.str ());
      }

    if (_brio_file_writer_ == 0)
      {
        _brio_file_writer_ = new brio::writer;
        _brio_file_writer_->open (_sink_record.effective_label);
        _brio_file_writer_->add_store (brio_common::GENERAL_INFO_STORE_LABEL,
                                       DPP_DU::properties::SERIAL_TAG,
                                       32000);
        _brio_file_writer_->add_store (brio_common::EVENT_RECORD_STORE_LABEL,
                                       DPP_DU::things::SERIAL_TAG,
                                       256000);
        _brio_file_writer_->select_store (brio_common::EVENT_RECORD_STORE_LABEL);
        _brio_file_writer_->print_info (std::clog);
        _sink_record.status = sink_record::STATUS_OPENED;
      }
    return;
  }

  bool simple_brio_data_sink::store_next_record (const DPP_DU::things & a_event_record)
  {
    bool done = false;
    if (_brio_file_writer_ != 0)
      {
        _brio_file_writer_->store (a_event_record);
        done = true;
      }
    else
      {
        std::ostringstream message;
        message << "dpp::simple_brio_data_sink::store_next_record: "
                << "Cannot store the event record ! This is a bug !";
        std::cerr << DPP_DU::io::error << message.str () << std::endl;
      }
    return done;
  }

  // ctor:
  simple_brio_data_sink::simple_brio_data_sink (uint32_t a_flags)
    : i_data_sink (a_flags)
  {
    _brio_file_writer_ = 0;
    return;
  }

  // ctor:
  simple_brio_data_sink::simple_brio_data_sink (const std::string & a_sink_label,
                                                uint32_t a_flags)
    : i_data_sink (a_sink_label, a_flags)
  {
    _brio_file_writer_ = 0;
    this->simple_brio_data_sink::open ();
    return;
  }

  // dtor:
  simple_brio_data_sink::~simple_brio_data_sink ()
  {
    this->simple_brio_data_sink::reset ();
    return;
  }

}  // end of namespace dpp

// end of simple_brio_data_sink.cc
/*
** Local Variables: --
** mode: c++
** indent-tabs-mode: nil --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

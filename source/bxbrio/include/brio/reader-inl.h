//! \file  brio/reader-inl.h
//! \brief Definitions of brio::reader template functions
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick
//
// This file is part of brio.
//
// brio is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// brio is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with brio.  If not, see <http://www.gnu.org/licenses/>.

#ifndef BRIO_READER_INL_H
#define BRIO_READER_INL_H

// Third Party:
// - ROOT:
#include <TTree.h>

namespace brio {
  template<typename T>
    int reader::load_next(T& data_, const std::string& label_) {
    int64_t entry = this->get_current_entry(label_);
    return this->load<T>(data_, label_, entry + 1);
  }

  template<typename T>
    int reader::load_previous(T& data_, const std::string& label_) {
    int64_t entry = this->get_current_entry(label_);
    return this->load<T>(data_, label_, entry - 1);
  }

  template<typename T>
    int reader::load(T& data_, int64_t nentry_) {
    return this->load<T>(data_, "", nentry_);
  }

  template<typename T>
    int reader::load(T& data_, const std::string& label_, int64_t nentry_) {
    DT_THROW_IF(!this->is_opened(),
		std::logic_error,
		"Operation prohibited; file is not opened !");

    store_info *ptr_si = this->_get_store_info(label_);
    if (!ptr_si) {
      DT_THROW_IF(!label_.empty (),
		  std::logic_error,
		  "No source store with label '" << label_ << "' !");
      // if we do not allow automatic store, this is a critical error:
      DT_THROW_IF(!_allow_automatic_store_,
		  std::logic_error,
		  "No source store is selected nor default store is available !");
      ptr_si = _automatic_store_;
    }
    return this->_at_load<T>(data_, ptr_si, nentry_);
  }

  template<class T>
    int reader::_at_load(T& data_, store_info *ptr_si_, int64_t nentry_) {
    DT_LOG_TRACE(this->get_logging_priority(),"Entering...");
    store_info& si = *ptr_si_;

    if (_check_serial_tag_) {
      // We check if the serialization tag from the store matches the
      // data's one:
      if (si.has_dedicated_serialization_tag()) {
	// 2013-02-19 FM : change the way we check the serial tag :
	//if (data_.get_serial_tag () != si.get_serialization_tag ())
	DT_THROW_IF(!datatools::check_serial_tag<T>(si.get_serialization_tag ()),
		    std::logic_error,
		    "Data serialization tag '" << data_.get_serial_tag()
		    << "' does not match source store's serialization tag '" << si.get_serialization_tag () << "' !");
      }
    }

    DT_THROW_IF(si.number_of_entries == 0,
		std::logic_error,
		"Source store '" << si.label << "' has no entry !");
    int64_t nentry = nentry_;
    if (nentry >= 0) {
      // check overflow:
      DT_THROW_IF(nentry_ >= si.number_of_entries,
		  std::logic_error,
		  "Source store '" << si.label << "' has "
		  << "no serialized entry at index '" << nentry_ << "' !");
    } else {
      // if nentry_ < 0: use entry index relative to the current entry
      // position
      if (si.current_entry < 0) {// at rewind position
	// start with first entry:
	nentry = 0;
      } else {
	// try next entry:
	nentry = si.current_entry + 1;
      }
    }

    // read this tree entry in the ROOT I/O system:
    si.record.reset();
    int ret = si.tree->GetEntry(nentry, 1); // -> 1 for all branches
    if (!ret) {
      DT_THROW_IF(true,
		  std::logic_error,
		  "No entry '"
		  << nentry << "' at entry # " << nentry
		  << " in source store labelled '" << si.label.c_str()
		  << "' from  file '" << _filename << "' !");
    } else if (ret == -1) {
      DT_THROW_IF(true,
		  std::logic_error,
		  "An I/O error occurs from entry '"
		  << nentry
		  << "' in source store labelled '" << si.label.c_str()
		  << "' from  file '" << _filename << "' !");
    } else {
      si.current_entry = nentry;
    }

    if (_check_serial_tag_) {
      /* We may be confused with stores without dedicated serialization tag.
       * Here we test if data and the entry's serial tags match:
       */
      if (!si.has_dedicated_serialization_tag()) {
	// check serial tag associated to the buffered binary archive:
	std::string serial_tag = si.record.fSerialTag.Data();
	// 2013-02-19 FM : change the way we check
	// if (data_.get_serial_tag () != serial_tag)
	DT_THROW_IF(!datatools::check_serial_tag<T>(serial_tag),
		    std::logic_error,
		    "Entry '" << nentry
		    << "' with serial tag '" << serial_tag
		    << "' in (mixed) source store labelled '" << si.label.c_str()

		    << "' from  file '" << _filename
		    << "' does not match the requested '"
		    << data_.get_serial_tag() << "' data type !");
      }
    }

    // Deserialize from the archive:
    boost::iostreams::stream<boost::iostreams::array_source>
      input_stream(si.record.fDataBuffer.fArray,si.record.fDataBuffer.fN);
    // 2011-06-16 FM: restored
    if (this->is_format_pba()) {
      //boost::archive::portable_binary_iarchive ia (input_stream);
      eos::portable_iarchive ia(input_stream);
      ia >> data_;
    }
    if (this->is_format_text()) {
      input_stream.imbue (*_locale);
      boost::archive::text_iarchive ia (input_stream);
      ia >> data_;
    }

    _current_store = &si;
    DT_LOG_TRACE(this->get_logging_priority(),"Exiting.");
    return 0;
  }
} // namespace brio
#endif // BRIO_READER_INL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

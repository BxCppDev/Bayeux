//! \file  writer-inl.h
//! \brief Definitions of brio::writer template functions
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

#ifndef BRIO_WRITER_INL_H
#define BRIO_WRITER_INL_H

// Third Party:
// - ROOT:
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-long-long"
// ROOT can shadow CLHEP and vice versa, appears not to be serious
#pragma clang diagnostic ignored "-Wshadow"
#endif
#include <TTree.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

namespace brio {
  template <typename T>
    int writer::store(const T& a_data, const std::string& a_label) {
    DT_THROW_IF(!this->is_opened(),
		std::logic_error,
		"Operation prohibited; file is not opened !");
    store_info *ptr_si = this->_get_store_info(a_label);
    if (!ptr_si) {
      if (a_label.empty()) {
	// if we do not allow automatic store, this is a critical error:
	DT_THROW_IF(!_allow_automatic_store_,
		    std::logic_error,
		    "No target store is selected nor target !");
	ptr_si = this->_add_store(store_info::constants::automatic_store_label(),
				  a_data.get_serial_tag(),
				  store_info::constants::default_store_buffer_size());
      } else {
	ptr_si = this->_add_store(a_label, a_data.get_serial_tag(),
				  store_info::constants::default_store_buffer_size());
      }
    }
    // Final check:
    DT_THROW_IF(ptr_si == 0,
		std::logic_error,
		"Could not determine any store to save data !");
    DT_LOG_DEBUG(this->get_logging_priority(),
		 "ptr_si = " << std::hex << ptr_si << std::dec);
    DT_LOG_DEBUG(this->get_logging_priority(),
		 "Using store with label '" << ptr_si->label << "'...");
    if (this->get_logging_priority() >= datatools::logger::PRIO_DEBUG) {
      ptr_si->tree->Print ();
    }
    return this->_at_store<T>(a_data, ptr_si);
  }

  template <typename T>
    int writer::_at_store(const T& a_data, store_info *a_store_info) {
    DT_LOG_TRACE(this->get_logging_priority(),"Entering...");
    store_info *ptr_si = a_store_info;
    // First serialized object sets the serialization tag for this store:
    if (ptr_si->serialization_tag ==
	store_info::constants::postponed_dedicated_serial_tag_label()) {
      ptr_si->serialization_tag = a_data.get_serial_tag();
    }
    // Else if the store has a dedicated serialization tag:
    else if (ptr_si->has_dedicated_serialization_tag()) {
      // Check the data serialization tag matches that requested by the store:
      DT_THROW_IF(a_data.get_serial_tag() != ptr_si->get_serialization_tag(),
		  std::logic_error,
		  "Serialization tag mismatch ! "
		  << "Attempt to store an object with `"
		  << a_data.get_serial_tag ()
		  << "' serialization tag "
		  << "in the store labelled '"
		  << ptr_si->label
		  << "' with dedicated `"
		  << ptr_si->get_serialization_tag ()
		  << "' serialization tag !");
    }

    // Prepare the (std::vector<char>) buffer to host the binary archive
    // as a sequence of bytes:
    typedef std::vector<char> buffer_type;

    /* Clear the buffer of characters for streaming but
     * intrinsic capacity is kept as is (this is a behaviour of vector
     * class from the STL library):
     */
    if (!ptr_si->buffer.empty()) {
      ptr_si->buffer.clear();
    } else if (ptr_si->buffer.capacity() == 0) {
      // Ensure minimum starting capacity of the buffer of characters for
      // streaming in order to optimize possible memory reallocation:
      ptr_si->buffer.reserve(store_info::constants::default_stream_buffer_size());
    }

    // Archiving is redirected to the buffer:
    namespace io = boost::iostreams;
    io::stream<io::back_insert_device<buffer_type> > output_stream(ptr_si->buffer);
    // 2011-06-16 FM: restored
    if (this->is_format_pba()) {
      //boost::archive::portable_binary_oarchive oa (output_stream);
      eos::portable_oarchive oa(output_stream);
      oa << a_data;
    }
    if (is_format_text()) {
      output_stream.imbue(*_locale);
      boost::archive::text_oarchive oa(output_stream);
      oa << a_data;
    }
    output_stream.flush();

    // Now the buffer contains the final sequence of bytes corresponding to
    // the serialized output binary archive:
    DT_LOG_DEBUG(this->get_logging_priority(),
		 "Buffer size = "
		 << ptr_si->buffer.size()
		 << "   buffer capacity = "
		 << ptr_si->buffer.capacity());
    // Prepare the container interface to be streamed using the ROOT I/O system:
    ptr_si->record.fSerialTag  = a_data.get_serial_tag().c_str();
    ptr_si->record.fVersionTag = 0; // not used
    ptr_si->record.fDataBuffer.fN     = ptr_si->buffer.size();
    ptr_si->record.fDataBuffer.fArray =  &(ptr_si->buffer[0]);
    // Store this tree entry in the ROOT I/O system:
    ptr_si->tree->Fill ();
    // Reset the ROOT data buffer:
    ptr_si->record.fDataBuffer.fN = 0;
    ptr_si->record.fDataBuffer.fArray = 0;
    // Update some counters:
    ptr_si->number_of_entries = ptr_si->tree->GetEntries();
    ptr_si->current_entry = ptr_si->number_of_entries - 1;
    // Update the 'current store' pointer:
    _current_store = ptr_si;
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

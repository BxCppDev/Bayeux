// -*- mode: c++ ; -*-
/* utils.cc
 */
// Ourselves
#include <brio/utils.h>

// Standard Library
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>

// Third Party
// - ROOT
#include <TTree.h>

// - Boost
#include <boost/filesystem.hpp>

// This Project

namespace brio {

  const std::string & store_info::constants::brio_file_extension()
  {
    static std::string ext;
    if (ext.empty()) ext = ".brio";
    return ext;
  }

  const std::string &
  store_info::constants::trio_file_extension()
  {
    static std::string ext;
    if (ext.empty()) ext = ".trio";
    return ext;
  }

  const std::string &
  store_info::constants::default_file_extension()
  {
    static std::string ext;
#ifdef BRIO_DEFAULT_TRIO_FORMAT
    if (ext.empty()) ext = ".trio";
#else
    if (ext.empty()) ext = ".brio";
#endif
    return ext;
  }

  const std::string &
  store_info::constants::default_store_title()
  {
    static std::string ext;
    if (ext.empty()) ext = "Serialization store";
    return ext;
  }

  const std::string &
  store_info::constants::automatic_store_label()
  {
    static std::string ext;
    if (ext.empty()) ext = "__brio::automatic_store__";
    return ext;
  }

  const size_t
  store_info::constants::default_store_buffer_size()
  {
    static size_t sz = 0;
    if (sz == 0) sz = 32000;
    return sz;
  }

  const size_t
  store_info::constants::default_stream_buffer_size()
  {
    static size_t sz = 0;
    if (sz == 0) sz = 4096;
    return sz;
  }

  const std::string &
  store_info::constants::postponed_dedicated_serial_tag_label()
  {
    static std::string ext;
    if (ext.empty()) ext = "__?__";
    return ext;
  }

  const std::string &
  store_info::constants::no_dedicated_serial_tag_label()
  {
    static std::string ext;
    if (ext.empty()) ext = "__*__";
    return ext;
  }

  store_info::store_info() : record() {
    serialization_tag = constants::postponed_dedicated_serial_tag_label();
    bufsize = constants::default_store_buffer_size();
    // pointers:
    tree = 0;
    p_record = 0;
    buffer.reserve(constants::default_stream_buffer_size());
    // reader infos:
    number_of_entries = 0;
    current_entry = -1;
  }

  store_info::~store_info() {
    p_record = 0;
    return;
  }

  const std::string& store_info::get_label() const {
    return label;
  }

  const std::string& store_info::get_serialization_tag() const {
    return serialization_tag;
  }

  bool store_info::has_dedicated_serialization_tag() const {
    if (serialization_tag.empty()) return false;
    if (serialization_tag == constants::no_dedicated_serial_tag_label()) return false;
    if (serialization_tag == constants::postponed_dedicated_serial_tag_label()) return false;
    return true;
  }

  bool store_info::has_entries() const {
    return number_of_entries > 0;
  }

  int store_info::guess_mode_from_filename(const std::string& a_filename,
                                           int& a_mode) {
    int status = store_info::SUCCESS;
    int mode = 0x0;
    if (boost::filesystem::extension(a_filename) ==
        constants::brio_file_extension()) {
      mode |= store_info::MODE_BRIO;
    } else if (boost::filesystem::extension (a_filename) ==
               constants::trio_file_extension()) {
      mode |= store_info::MODE_TRIO;
    } else {
      status = store_info::ERROR;
    }
    a_mode = mode;
    return status;
  }
} // end of namespace brio

// end of utils.cc


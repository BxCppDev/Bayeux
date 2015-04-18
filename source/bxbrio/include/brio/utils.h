/// \file brio/utils.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-11-01
 * Last modified: 2013-04-22
 *
 * License:
 *
 * Description:
 *   Utilities for BRIO classes
 *
 * History:
 *
 */

#ifndef BRIO_UTILS_H
#define BRIO_UTILS_H 1

// Standard Library:
#include <string>
#include <vector>
#include <map>

// Third Party:
// - Boost:
#include <boost/cstdint.hpp>

// This Project:
#include <brio/detail/brio_record.h>

class TTree;

namespace brio {
  /// A class that contains internal dynamic informations for a given \e store
  class store_info {
  public:
    struct constants {
      static const std::string & brio_file_extension();
      static const std::string & trio_file_extension();
      static const std::string & default_file_extension();
      static const std::string & default_store_title();
      static const std::string & automatic_store_label();
      static const std::string & postponed_dedicated_serial_tag_label();
      static const std::string & no_dedicated_serial_tag_label();
      static size_t        default_store_buffer_size();
      static size_t        default_stream_buffer_size();
    };

    enum mode_type {
      MODE_READ    = 0x0,
      MODE_WRITE   = 0x1,
      read         = MODE_READ,
      write        = MODE_WRITE,
      MODE_BRIO    = 0x0,
      MODE_TRIO    = 0x2,
      MODE_DEFAULT = MODE_READ | MODE_BRIO
    };

    static const int SUCCESS = 0;
    static const int ERROR   = 1;


  public:
    store_info();
    virtual ~store_info();

    const std::string& get_label() const;
    const std::string& get_serialization_tag() const;
    bool has_dedicated_serialization_tag() const;
    bool has_entries() const;
    static int guess_mode_from_filename (const std::string & a_filename, int & a_mode);

  public:
    // make all attributes public:
    std::string       label; /// the label (name) of the \e store
    std::string       serialization_tag; /// the serialization tag associated to the object stored in the \e store
    size_t       bufsize; /// the size of the output buffer (used only by the writer)
    TTree*      tree; /// the embedded ROOT tree
    brio_record  record; /// the current brio record to be (de)serialized
    brio_record *p_record;
    std::vector<char> buffer; /// the input buffer (used only by the writer)
    int64_t      number_of_entries; /// the number of entries in the \e store
    int64_t      current_entry; /// the current entry number in the \e store
  };

  typedef std::map<std::string, store_info> store_info_dict_type;

} // end of namespace brio

#endif // BRIO_UTILS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

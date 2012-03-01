// -*- mode: c++ ; -*-
/* utils.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-11-01
 * Last modified: 2010-11-01
 *
 * License:
 *
 * Description:
 *   Utilities for BRIO classes
 *
 * History:
 *
 */

#ifndef __brio__utils_h
#define __brio__utils_h 1

#include <vector>
#include <map>

#include <boost/cstdint.hpp>

#include <brio/detail/brio_record.h>

class TTree;

namespace brio {

  using namespace std;

  /// A class that contains internal dynamic informations for a given \e store
  class store_info
  {
  public:

    static const string BRIO_FILE_EXTENSION;
    static const string TRIO_FILE_EXTENSION;
    static const string DEFAULT_FILE_EXTENSION;
    static const string DEFAULT_STORE_TITLE;
    static const string AUTOMATIC_STORE_LABEL;
    static const size_t DEFAULT_STORE_BUFFER_SIZE;
    static const size_t DEFAULT_STREAM_BUFFER_SIZE;
    static const string POSTPONED_DEDICATED_SERIAL_TAG_LABEL;
    static const string NO_DEDICATED_SERIAL_TAG_LABEL;

  public:
    const string & get_label () const;
    const string & get_serialization_tag () const;
    bool has_dedicated_serialization_tag () const;
    bool has_entries () const;
    store_info ();
    virtual ~store_info ();

    enum mode_type
      {
        MODE_READ    = 0x0,
        MODE_WRITE   = 0x1,
        read         = MODE_READ,
        write        = MODE_WRITE,

        MODE_BRIO    = 0x0,
        MODE_TRIO    = 0x2,

        MODE_DEFAULT = MODE_READ | MODE_BRIO
      };

    static const int SUCCESS;
    static const int ERROR;

    static int guess_mode_from_filename (const std::string & a_filename, int & a_mode);

  public:
    // make all attributes public:
    string       label; /// the label (name) of the \e store
    string       serialization_tag; /// the serialization tag associated to the object stored in the \e store
    size_t       bufsize; /// the size of the output buffer (used only by the writer)
    TTree *      tree; /// the embedded ROOT tree
    brio_record  record; /// the current brio record ro be (de)serialized
    brio_record * p_record;
    vector<char> buffer; /// the input buffer (used only by the writer)
    int64_t      number_of_entries; /// the number of entries in the \e store
    int64_t      current_entry; /// the current entry number in the \e store

  };

  typedef map<string, store_info> store_info_dict_t;

} // end of namespace brio

#endif // __brio__utils_h

// end of utils.h


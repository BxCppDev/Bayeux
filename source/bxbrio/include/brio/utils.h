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

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include <boost/cstdint.hpp>

#include <TTree.h>

#include <brio/detail/brio_record.h>

namespace brio {

  using namespace std;

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

    string       label;
    string       serialization_tag;
    size_t       bufsize; // used only by the writer
    TTree *      tree;
    brio_record  record;
    brio_record * p_record;
    vector<char> buffer; // used only by the writer
    int64_t      number_of_entries;
    int64_t      current_entry;

    const string & get_label () const;
    const string & get_serialization_tag () const;
    bool has_dedicated_serialization_tag () const;
    bool has_entries () const;    
    store_info ();
    virtual ~store_info ();

  };

  typedef map<string, store_info> store_info_dict_t;
 
} // end of namespace brio

#endif // __brio__utils_h

// end of utils.h


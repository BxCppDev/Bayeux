// -*- mode: c++ ; -*-
/* utils.cc
 */

#include <brio/utils.h>

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>

#include <TTree.h>

#include <boost/filesystem.hpp>

namespace brio {

  using namespace std;

  const int store_info::SUCCESS = 0;
  const int store_info::ERROR   = 1;

  //const string store_info::DEFAULT_FILE_EXTENSION = ".root";
  const string store_info::BRIO_FILE_EXTENSION = ".brio";
  const string store_info::TRIO_FILE_EXTENSION = ".trio";

#ifdef BRIO_DEFAULT_TRIO_FORMAT
  const string store_info::DEFAULT_FILE_EXTENSION = store_info::TRIO_FILE_EXTENSION;
#else
  const string store_info::DEFAULT_FILE_EXTENSION = store_info::BRIO_FILE_EXTENSION;
#endif

  const string store_info::DEFAULT_STORE_TITLE = "Serialization store";
  const string store_info::AUTOMATIC_STORE_LABEL = "__brio::automatic_store__";

  const size_t store_info::DEFAULT_STORE_BUFFER_SIZE = 32000;
  const size_t store_info::DEFAULT_STREAM_BUFFER_SIZE = 4096;

  const string store_info::POSTPONED_DEDICATED_SERIAL_TAG_LABEL =  "__?__";
  const string store_info::NO_DEDICATED_SERIAL_TAG_LABEL =  "__*__";

  const string & store_info::get_label () const
  {
    return label;
  }

  const string & store_info::get_serialization_tag () const
  {
    return serialization_tag;
  }

  bool store_info::has_dedicated_serialization_tag () const
  {
    if (serialization_tag.empty ()) return false;
    if (serialization_tag == store_info::NO_DEDICATED_SERIAL_TAG_LABEL) return false;
    if (serialization_tag == store_info::POSTPONED_DEDICATED_SERIAL_TAG_LABEL) return false;
    return true;
  }

  bool store_info::has_entries () const
  {
    return number_of_entries > 0;
  }

  store_info::store_info () : record ()
  {
    serialization_tag = POSTPONED_DEDICATED_SERIAL_TAG_LABEL;
    bufsize = DEFAULT_STORE_BUFFER_SIZE;
    // pointers:
    tree = 0;
    p_record = 0;
    buffer.reserve (DEFAULT_STREAM_BUFFER_SIZE);
    // reader infos:
    number_of_entries = 0;
    current_entry = -1;
    return;
  }

  store_info::~store_info ()
  {
    p_record = 0;
    return;
  }

  int
  store_info::guess_mode_from_filename (const std::string & a_filename,
                                        int & a_mode)
  {
    int status = store_info::SUCCESS;
    int mode = 0x0;
    if (boost::filesystem::extension (a_filename) == store_info::BRIO_FILE_EXTENSION)
      {
        mode |= store_info::MODE_BRIO;
      }
    else if (boost::filesystem::extension (a_filename) == store_info::TRIO_FILE_EXTENSION)
      {
        mode |= store_info::MODE_TRIO;
      }
    else
      {
        status = store_info::ERROR;
      }
    a_mode = mode;
    return status;
  }

} // end of namespace brio

// end of utils.cc

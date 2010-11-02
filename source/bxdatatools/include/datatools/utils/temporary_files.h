// -*- mode: c++; -*- 
// temporary_files.h

#ifndef __datatools__utils__temporary_files_h
#define __datatools__utils__temporary_files_h 1

#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>

#include <datatools/utils/utils.h>
#include <boost/filesystem.hpp>

namespace datatools {
  
  namespace utils {

    using namespace std;

    class temp_file
    {
    public:
      static bool g_devel;
      static const string DEFAULT_PATTERN;

    private:
      bool   __remove_at_destroy;
      string __path_dir;
      string __pattern;
      string __full_pattern;
      string __filename;
      bool   __read_open;
      bool   __write_open;
      char * __template;
      ofstream __out;
      ifstream __in;

    protected:

      void _set_defaults ();

    public:

      const string & get_filename () const;

      bool is_valid () const;

      bool is_read_open () const;

      bool is_write_open () const;

      void set_remove_at_destroy (bool);

      ofstream & out ();

      ifstream & in ();

      temp_file (bool remove_at_destroy_ = true);

      temp_file (string pattern_, bool remove_at_destroy_ = true);

      temp_file (string path_dir_, string pattern_, bool remove_at_destroy_ = true);

      ~temp_file ();

      void create (string path_dir_, string pattern_);
      
      void close ();
      
      void remove ();

    };

    
  } // namespace utils

} // namespace datatools 

#endif // __datatools__utils__temporary_files_h

// end of utils.h

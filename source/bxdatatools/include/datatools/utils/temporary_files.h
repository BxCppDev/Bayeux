// -*- mode: c++; -*- 
// temporary_files.h

#ifndef __datatools__utils__temporary_files_h
#define __datatools__utils__temporary_files_h 1

#include <iostream>
#include <fstream>
#include <string>

namespace datatools {
  
  namespace utils {

    using namespace std;

    class temp_file
    {
    public:
      static bool g_devel;
      static const string DEFAULT_PATTERN;

    private:
      bool   remove_at_destroy_;
      string path_dir_;
      string pattern_;
      string full_pattern_;
      string filename_;
      bool   read_open_;
      bool   write_open_;
      char * template_;
      ofstream out_;
      ifstream in_;

    protected:

      void set_defaults_ ();

    public:

      const string & get_filename () const;

      bool is_valid () const;

      bool is_read_open () const;

      bool is_write_open () const;

      void set_remove_at_destroy (bool);

      ofstream & out ();

      ifstream & in ();

      temp_file ();

      temp_file (const char * a_pattern, bool a_remove_at_destroy = true);

      temp_file (string a_pattern, bool a_remove_at_destroy = true);

      temp_file (const char * a_path_dir, const char * a_pattern, bool a_remove_at_destroy = true);

      temp_file (string a_path_dir, const char * a_pattern, bool a_remove_at_destroy = true);

      temp_file (string a_path_dir, string a_pattern, bool a_remove_at_destroy = true);

      ~temp_file ();

      void create (string a_path_dir, string a_pattern);
      
      void close ();
      
      void remove ();

    };

    
  } // namespace utils

} // namespace datatools 

#endif // __datatools__utils__temporary_files_h

// end of utils.h

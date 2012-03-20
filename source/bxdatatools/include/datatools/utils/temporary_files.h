// -*- mode: c++; -*- 
// temporary_files.h

#ifndef __datatools__utils__temporary_files_h
#define __datatools__utils__temporary_files_h 1

#include <iostream>
#include <fstream>
#include <string>

namespace datatools {
  
  namespace utils {

    // using   namespace std;

    class temp_file
    {
    public:

      static bool g_devel;
      static const std::string DEFAULT_PATTERN;

    private:
      bool   _remove_at_destroy_;
      std::string _path_dir_;
      std::string _pattern_;
      std::string _full_pattern_;
      std::string _filename_;
      bool   _read_open_;
      bool   _write_open_;
      char * _template_;
      std::ofstream _out_;
      std::ifstream _in_;

    protected:

      void _set_defaults ();

    public:

      const std::string & get_filename () const;

      bool is_valid () const;

      bool is_read_open () const;

      bool is_write_open () const;

      void set_remove_at_destroy (bool);

      std::ofstream & out ();

      std::ifstream & in ();

      temp_file ();

      temp_file (const char * a_pattern, bool a_remove_at_destroy = true);

      temp_file (std::string a_pattern, bool a_remove_at_destroy = true);

      temp_file (const char * a_path_dir, const char * a_pattern, bool a_remove_at_destroy = true);

      temp_file (std::string a_path_dir, const char * a_pattern, bool a_remove_at_destroy = true);

      temp_file (std::string a_path_dir, std::string a_pattern, bool a_remove_at_destroy = true);

      ~temp_file ();

      void create (std::string a_path_dir, std::string a_pattern);
      
      void close ();
      
      void remove ();

    };

    
  } // namespace utils

} // namespace datatools 

#endif // __datatools__utils__temporary_files_h

// end of utils.h

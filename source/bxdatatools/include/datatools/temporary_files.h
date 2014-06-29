// -*- mode: c++; -*-
/// \file datatools/temporary_files.h

#ifndef DATATOOLS_TEMPORARY_FILES_H
#define DATATOOLS_TEMPORARY_FILES_H

// Standard Library:
#include <iostream>
#include <fstream>
#include <string>

namespace datatools {

/*! \brief A class for the management of temporary unique file stored in a given directory
 */
class temp_file {
 public:
  static const std::string & default_pattern();

 public:

  /// Default constructor
  temp_file();

  /// Constructor
  temp_file(const char* a_pattern, bool a_remove_at_destroy = true);

  /// Constructor
  temp_file(std::string a_pattern, bool a_remove_at_destroy = true);

  /// Constructor
  temp_file(const char* a_path_dir, const char* a_pattern,
            bool a_remove_at_destroy = true);

  /// Constructor
  temp_file(std::string a_path_dir, const char* a_pattern,
            bool a_remove_at_destroy = true);

  /// Constructor
  temp_file(std::string a_path_dir, std::string a_pattern,
            bool a_remove_at_destroy = true);

  /// Destructor
  ~temp_file();

  /// Return the filename of the temporary file
  const std::string& get_filename() const;

  /// Check the validity of the temporary file
  bool is_valid() const;

  /// Check if the temporary file is open for reading
  bool is_read_open() const;

  /// Check if the temporary file is open for writing
  bool is_write_open() const;

  /// Set the 'remove at destroy' flag
  void set_remove_at_destroy(bool);

  /// Activate verbosity
  void set_verbose(bool);

  /// Return the associated output stream
  std::ofstream& out();

  /// Return the associated input stream
  std::ifstream& in();

  /// Create the temporary file from path and filename pattern
  void create(std::string a_path_dir, std::string a_pattern);

  /// Close the temporary file
  void close();

  /// Remove the temporary file from the filesystem
  void remove();

 protected:

  /// Set the default values of attributes
  void set_defaults();

 private:
  bool   verbose_; /// Verbosity flag
  bool   remove_at_destroy_; /// Remove at destroy flag
  std::string path_dir_; /// Directory path for the temporary file
  std::string pattern_; /// Filename pattern
  std::string full_pattern_; /// Filename pattern
  std::string filename_; /// Filename
  bool   read_open_; /// Flag for temporary file opened for reading
  bool   write_open_; /// Flag for temporary file opened for writing
  char  *template_; /// Filename pattern template
  std::ofstream out_; /// Associated output stream
  std::ifstream in_; /// Associated input stream
};

} // namespace datatools

#endif // DATATOOLS_TEMPORARY_FILES_H

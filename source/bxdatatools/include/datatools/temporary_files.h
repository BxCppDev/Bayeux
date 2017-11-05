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
  class temp_file
  {
  public:
    static const std::string & default_pattern();

  public:

    /// Default constructor
    temp_file();

    /// Constructor
    temp_file(const char* pattern_, bool remove_at_destroy_ = true);

    /// Constructor
    temp_file(std::string pattern_, bool remove_at_destroy_ = true);

    /// Constructor
    temp_file(const char* path_dir_, const char* pattern_,
              bool remove_at_destroy_ = true);

    /// Constructor
    temp_file(std::string path_dir_, const char* pattern_,
              bool remove_at_destroy_ = true);

    /// Constructor
    temp_file(std::string path_dir_, std::string pattern_,
              bool remove_at_destroy_ = true);

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
    void create(std::string path_dir_, std::string pattern_);

    /// Close the temporary file
    void close();

    /// Remove the temporary file from the filesystem
    void remove();

  protected:

    /// Set the default values of attributes
    void set_defaults();

  private:
    bool   _verbose_; ///< Verbosity flag
    bool   _remove_at_destroy_; ///< Remove at destroy flag
    std::string _path_dir_; ///< Directory path for the temporary file
    std::string _pattern_; ///< Filename pattern
    std::string _full_pattern_; ///< Filename pattern
    std::string _filename_; ///< Filename
    bool   _read_open_; ///< Flag for temporary file opened for reading
    bool   _write_open_; ///< Flag for temporary file opened for writing
    char  * _template_; ///< Filename pattern template
    std::ofstream _out_; ///< Associated output stream
    std::ifstream _in_; ///< Associated input stream
  };

} // namespace datatools

#endif // DATATOOLS_TEMPORARY_FILES_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

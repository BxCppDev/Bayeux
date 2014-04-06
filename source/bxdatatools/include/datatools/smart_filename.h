/* smart_filename.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-09-12
 * Last modified : 2011-09-12
 *
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *
 *
 * Description:
 *
 *   Automated incremental filenames
 *
 * History:
 *
 */
#ifndef DATATOOLS_SMART_FILENAME_H_
#define DATATOOLS_SMART_FILENAME_H_
// Standard Library
#include <iostream>
#include <string>
#include <vector>

// Third Party
// - Boost
#include <boost/cstdint.hpp>

// This Project
#include <datatools/properties.h>

namespace datatools {

/*! \brief A class that builds an arbitrary list of filenames on user request.
 *  Supported modes are:
 *  \li "single" : single filename
 *  \li "list" : explicit list of filenames
 *  \li "incremental" : list of filenames built from an incremental approach
 */
class smart_filename {
 public:
  static const int MODE_INCREMENTAL_UNRANGED          = -1;
  static const int MODE_INCREMENTAL_DEFAULT_START     =  0;
  static const int MODE_INCREMENTAL_DEFAULT_INCREMENT =  1;

  enum mode_t {
    MODE_INVALID     =  0,
    MODE_SINGLE      =  0x1,
    MODE_LIST        =  0x2,
    MODE_INCREMENTAL =  0x4
  };

  struct labels {
    static const std::string & mode_single();
    static const std::string & mode_list();
    static const std::string & mode_incremental();
  };

 public:
  typedef std::vector<std::string>  list_type;
  typedef list_type::const_iterator const_iterator;

 public:

  /// Default constructor:
  smart_filename();

  /// Destructor
  virtual ~smart_filename();

  /// Check debug flag
  bool is_debug() const;

  /// Set debug flag
  void set_debug(bool);

  /// Return the mode
  int get_mode() const;

  /// Check the initialization flag
  bool is_initialized() const;

  /// Check the single mode
  bool is_single() const;

  /// Check the list mode
  bool is_list() const;

  /// Check the incremental mode
  bool is_incremental() const;

  /// Check if the object is valid
  bool is_valid() const;

  /// Check is the list of filenames is ranged
  bool is_ranged() const;

  /// Check the expand path flag
  bool is_expand_path() const;

  /// Return the number of filenames
  size_t size() const;

  /// Return the current number of filenames
  size_t current_size() const;

  /// Return the first const iterator from the embedded list of filenames
  const_iterator begin() const;

  /// Return the fpast-the-end const iterator from the embedded list of filenames
  const_iterator end() const;

  /// Access to a filename by index
  const std::string& operator[](int index) const;

  /// Access to a filename by index
  const std::string& get_filename(int index) const;

  /// Check if a given filename exists from the embedded list of filenames
  bool has_filename(const std::string& name, bool expand = true) const;

  /// Set the filename in single mode
  void set(const std::string& filename);

  /// Set the single mode and associated filename
  void set_single(const std::string& filename);

  /// Add a filename in list mode
  void add(const std::string& filename);

  /// Add a filename in list mode
  void add_to_list(const std::string& filename);

  /// Reset the object
  void reset();

  /// Build the filename from the increment index in incremental mode
  void build_incremental_filename(int increment_index,
                                  std::string& filename) const;

  /// Raw print
  void dump(std::ostream& out = std::clog) const;

  /// Initialize from a list of configuration parameters
  void initialize(const properties& config);

  /// Print the list of filenames
  void print_list_of_filenames(std::ostream& out = std::clog) const;

  /// Store the list of filenames in a file
  void store_list_of_filenames(const std::string& a_list_filename,
                               bool append = true) const;

  /// Make a smart_filename object in single mode
  static void make_single(smart_filename& a_smart_filename,
                          const std::string& filename,
                          bool expand_path = true);

  /// Make a smart_filename object in list mode
  static void make_list(smart_filename& a_smart_filename,
                        bool allow_duplication = false,
                        bool expand_path = true);

  /// Make a smart_filename object in list mode
  static void make_list(smart_filename& a_smart_filename,
                         const std::string& a_list_file,
                         bool allow_duplication = false,
                         bool expand_path = true);

  /// Make a smart_filename object in incremental mode
  static void make_incremental(smart_filename& a_smart_filename,
                               const std::string& path,
                               const std::string& prefix,
                               const std::string& extension,
                               int stopping_index,
                               int starting_index = 0,
                               int increment_index = 1,
                               const std::string& suffix = "",
                               int incremental_index_ndigit = 0,
                               bool expand_path = true);

  /// Make a smart_filename object in unranged incremental mode
  static void make_unranged_incremental(smart_filename& smart_filename,
                                        const std::string& path,
                                        const std::string& prefix,
                                        const std::string & a_extension,
                                        int starting_index = 0,
                                        int increment_index = 1,
                                        const std::string& suffix = "",
                                        int incremental_index_ndigit = 0,
                                        bool expand_path = true);

 protected:

  /// Set the mode
  void set_mode(int);

  /// Add a file name in the embedded list of filenames
  void add_list(const std::string& filename);

  /// Set the flag to allow duplicated filenames
  void set_list_allow_duplication(bool);

  /// Set the current in incremental/list mode
  void set_current_index(int);

 private:

  bool        debug_; //! Debug flag
  uint32_t    mode_; //! Mode
  bool        expand_path_; //! Flag to expand the file path name
  list_type   list_; //! The list of filenames
  bool        list_allow_duplication_; //! Flag to allow duplication of filenames in the list
  bool        ranged_; //! Flag to indicate if the list is ranged
  std::string incremental_path_; //! Path of the incremented filenames
  std::string incremental_prefix_; //! Prefix of the incremented filenames
  std::string incremental_suffix_; //! Suffix of the incremented filenames
  std::string incremental_extension_; //! Extension of the incremented filenames
  int32_t     incremental_starting_index_; //! Starting index of the incremented filenames
  int32_t     incremental_stopping_index_; //! Stopping index of the incremented filenames
  int32_t     incremental_increment_; //! Index increment of the incremented filenames
  uint32_t    incremental_index_ndigit_; //! Number of digits used for the increment part of the filenames
};

}  // end of namespace datatools

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(datatools::smart_filename)

#endif // DATATOOLS_SMART_FILENAME_H_

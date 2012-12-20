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
 *   Automated incremental filename
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
  static const int         MODE_INCREMENTAL_UNRANGED          = -1;
  static const int         MODE_INCREMENTAL_DEFAULT_START     =  0;
  static const int         MODE_INCREMENTAL_DEFAULT_INCREMENT =  1;
  static const std::string MODE_SINGLE_LABEL;
  static const std::string MODE_LIST_LABEL;
  static const std::string MODE_INCREMENTAL_LABEL;

  enum mode_t {
    MODE_INVALID     =  0,
    MODE_SINGLE      =  0x1,
    MODE_LIST        =  0x2,
    MODE_INCREMENTAL =  0x4
  };

 public: 
  typedef std::vector<std::string>  list_type;
  typedef list_type::const_iterator const_iterator;

 public:
  // ctor:
  smart_filename();

  // dtor:
  virtual ~smart_filename();

  bool is_debug() const;

  void set_debug(bool);

  int get_mode() const;

  bool is_initialized() const;

  bool is_single() const;

  bool is_list() const;

  bool is_incremental() const;

  bool is_valid() const;

  bool is_ranged() const;

  bool is_expand_path() const;

  size_t size() const;

  size_t current_size() const;

  const_iterator begin() const;

  const_iterator end() const;

  const std::string& operator[](int index) const;

  const std::string& get_filename(int index) const;

  bool has_filename(const std::string& name, bool expand = true) const;

  // single mode :
  void set(const std::string& filename);

  // single mode :
  void set_single(const std::string& filename);

  // list mode :
  void add(const std::string& filename);

  // list mode :
  void add_to_list(const std::string& filename);

  void reset();  

  void build_incremental_filename(int increment_index, 
                                  std::string& filename) const;

  void dump(std::ostream& out = std::clog) const;

  void initialize(const properties& config);

  void print_list_of_filenames(std::ostream& out = std::clog) const;

  void store_list_of_filenames(const std::string& a_list_filename, 
                               bool append = true) const;

  static void make_single(smart_filename& a_smart_filename,
                          const std::string& filename,
                          bool expand_path = true);

  static void make_list(smart_filename& a_smart_filename,
                        bool allow_duplication = false,
                        bool expand_path = true);

  static void make_list(smart_filename& a_smart_filename,
                         const std::string& a_list_file, 
                         bool allow_duplication = false,
                         bool expand_path = true);

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
  void set_mode(int);

  // list:
  void add_list(const std::string& filename);

  void set_list_allow_duplication(bool);

  // incremental/list:
  void set_current_index(int);


 private: 
  bool        debug_; //! Debug flag
  uint32_t    mode_; //! Mode
  bool        expand_path_; //! Flag to expand the file path name
  list_type   list_; //! The list of filenames
  bool        list_allow_duplication_; //! Flag to allow duplication of filenames in the list
  bool        ranged_; //! Flag to indicate if the list is ranged
  std::string incremental_path_;
  std::string incremental_prefix_;
  std::string incremental_suffix_;
  std::string incremental_extension_;
  int32_t     incremental_starting_index_;
  int32_t     incremental_stopping_index_;
  int32_t     incremental_increment_;
  uint32_t    incremental_index_ndigit_;
};

}  // end of namespace datatools

#endif // DATATOOLS_SMART_FILENAME_H_


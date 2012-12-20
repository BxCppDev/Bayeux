/* datatools/library_loader.h */
/* Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-30
 * Last modified : 2011-06-30
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
 * Description:
 *
 *  A shared library loader facility (base on kwsys).
 *
 */
#ifndef DATATOOLS_UTILS_LIBRARY_LOADER_H_
#define DATATOOLS_UTILS_LIBRARY_LOADER_H_
// Standard Library
#include <iostream>
#include <list>
#include <map>
#include <string>

// Third Party
// - A
#include <boost/cstdint.hpp>
#include <boost/utility.hpp>
#include <boost/scoped_ptr.hpp>

// This Project
#include <datatools/datatools_config.h>
#include <datatools/handle.h>
#include <datatools/bit_mask.h>
#include <datatools/detail/DynamicLoader.h>

namespace datatools {


struct library_entry_type;

typedef datatools::handle<library_entry_type> handle_library_entry_type;
typedef std::map<std::string, handle_library_entry_type> handle_library_entry_dict_type;
typedef std::list<handle_library_entry_type> handle_library_entry_stack_type;

typedef datatools::detail::DynamicLoader::SymbolPointer symbol_ptr;

//! \brief A class used internally by the library_loader class
struct library_entry_type {
  std::string name;
  std::string directory;
  std::string filename;
  std::string full_path;
  std::string version;
  bool   autoload;
  int    status;
  datatools::detail::DynamicLoader::LibraryHandle handle;

  library_entry_type(const std::string& lib_name      = "",
                     const std::string& lib_directory = "",
                     const std::string& lib_filename  = "",
                     const std::string& lib_full_path = "",
                     const std::string& lib_version   = "",
                     bool lib_autoload                = true);

  virtual ~library_entry_type();

  void print(std::ostream& out = std::clog, const std::string& indent = "") const;

};


//! \brief A DLL loader class
class library_loader : boost::noncopyable {
 public:
  static bool g_devel;
  static bool g_test;

  enum flag_type {
    debug              = bit_mask::bit00,
    allow_unregistered = bit_mask::bit01,
    auto_all           = bit_mask::bit02,
    auto_none          = bit_mask::bit03,
    test               = bit_mask::bit10
  };

 public:
  library_loader(uint32_t flags = 0, const std::string& config_file = "");
  virtual ~library_loader();

  bool is_debug() const;

  void set_debug(bool);

  void set_allow_unregistered(bool);

  bool allowing_unregistered() const;

  bool is_test() const;
  bool has(const std::string& name) const;

  bool is_loaded(const std::string& name) const;

  int registration(const std::string& name,
                   const std::string& directory = "",
                   const std::string& filename  = "",
                   const std::string& full_path = "",
                   const std::string& version   = "",
                   bool autoload = false);

  int load(const std::string& name,
           const std::string& directory = "",
           const std::string& filename  = "",
           const std::string& full_path = "",
           const std::string& version   = "");

  int close(const std::string& name);

  int close_all();

  void print(std::ostream& out = std::clog) const;

  symbol_ptr get_symbol_address(const std::string& name,
                                const std::string& symbol);

 protected:
  void init();

 private:
  uint32_t    flags_; //!< Flags
  std::string config_filename_;
  handle_library_entry_stack_type stacked_libraries_;
  handle_library_entry_dict_type  libraries_;
};


} // end of namespace datatools

#endif // DATATOOLS_UTILS_LIBRARY_LOADER_H_


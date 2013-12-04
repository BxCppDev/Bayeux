/* datatools/library_loader.h */
/* Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-30
 * Last modified : 2013-04-19
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#ifndef DATATOOLS_LIBRARY_LOADER_H_
#define DATATOOLS_LIBRARY_LOADER_H_

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
#include <datatools/exception.h>
#include <datatools/multi_properties.h>

namespace datatools {
struct library_entry_type;
typedef datatools::handle<library_entry_type> handle_library_entry_type;
typedef std::map<std::string, handle_library_entry_type> handle_library_entry_dict_type;
typedef std::list<handle_library_entry_type> handle_library_entry_stack_type;
typedef datatools::detail::DynamicLoader::SymbolPointer symbol_ptr;

//! \brief A class used internally by the library_loader class
struct library_entry_type {
  library_entry_type(const std::string& lib_name      = "",
                     const std::string& lib_directory = "",
                     const std::string& lib_filename  = "",
                     const std::string& lib_full_path = "",
                     const std::string& lib_version   = "",
                     bool lib_autoload                = true);
  virtual ~library_entry_type();

  void print(std::ostream& out = std::clog, const std::string& indent = "") const;

 public:
  std::string name;
  std::string directory;
  std::string filename;
  std::string full_path;
  std::string version;
  bool   autoload;
  int    status;
  datatools::detail::DynamicLoader::LibraryHandle handle;
};


//! \brief A DLL loader class
class library_loader : boost::noncopyable {
 public:
  enum flag_type {
    debug              = bit_mask::bit00,
    allow_unregistered = bit_mask::bit01,
    auto_all           = bit_mask::bit02,
    auto_none          = bit_mask::bit03,
    test               = bit_mask::bit10
  };

 public:
  //! Default constructor
  explicit library_loader(uint32_t flags=0);

  //! Construct an instance with default flags and a config file
  library_loader(uint32_t flags, const std::string& config_file);

  //! Construct and instance with default flags and multi_properties
  library_loader(uint32_t flags, const datatools::multi_properties& config);

  /// Destructor
  virtual ~library_loader();

  //! Return true if the instance is in debug mode
  bool is_debug() const;

  //! Change debug mode
  void set_debug(bool);

  //! Set the allow unregistered flag
  void set_allow_unregistered(bool);

  //! Check the allow unregistered flag
  bool allowing_unregistered() const;

  //! Return true if the instance is in test mode
  bool is_test() const;

  //! Return true if the DLL with name is setup
  bool has(const std::string& name) const;

  //! Return true if the DLL with name is loaded
  bool is_loaded(const std::string& name) const;

  //! Register a DLL
  int registration(const std::string& name,
                   const std::string& directory = "",
                   const std::string& filename  = "",
                   const std::string& full_path = "",
                   const std::string& version   = "",
                   bool autoload = false);

  //! Load a DLL
  int load(const std::string& name,
           const std::string& directory = "",
           const std::string& filename  = "",
           const std::string& full_path = "",
           const std::string& version   = "");

  //! Close a DLL
  int close(const std::string& name);

  //! Close all DLLs
  int close_all();

  //! Smart print libraries configured/loaded to supplied stream
  void print(std::ostream& out = std::clog) const;

  //! Return pointer to symbol in named library
  symbol_ptr get_symbol_address(const std::string& name,
                                const std::string& symbol);

 protected:
  /// Protected initialization
  void init();

 private:
  uint32_t    flags_; //!< Flags
  datatools::multi_properties config_; //!< Configuration
  handle_library_entry_stack_type stacked_libraries_;
  handle_library_entry_dict_type  libraries_;
};

} // end of namespace datatools

#define DATATOOLS_DLL_FORCE_LOAD(DllName,FromWhere)                     \
  {                                                                     \
    ::datatools::library_loader dt_ll_(::datatools::library_loader::allow_unregistered); \
    int load_status = dt_ll_.load(DllName);                             \
    DT_THROW_IF (load_status != EXIT_SUCCESS,                           \
                 std::logic_error,                                      \
                 "Automatic loading of library '" << DllName << "' failed !"); \
  }
/**/

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(datatools::library_loader)

#endif // DATATOOLS_LIBRARY_LOADER_H_

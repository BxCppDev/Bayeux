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
//#include <list>
//#include <map>
#include <string>

// Third Party
// - A
#include <boost/cstdint.hpp>
#include <boost/utility.hpp>
#include <boost/scoped_ptr.hpp>

// This Project
#include <datatools/datatools_config.h>
//#include <datatools/handle.h>
#include <datatools/bit_mask.h>
#include <datatools/detail/DynamicLoader.h>
//#include <datatools/exception.h>
#include <datatools/multi_properties.h>

namespace datatools {
//! \brief A DLL loader class
class library_loader : boost::noncopyable {
 public:
  typedef datatools::detail::DynamicLoader::SymbolPointer symbol_ptr;

  enum flag_type {
    allow_unregistered = bit_mask::bit01,
    auto_all           = bit_mask::bit02,
    auto_none          = bit_mask::bit03,
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

  //! Set the allow unregistered flag
  void set_allow_unregistered(bool);

  //! Check the allow unregistered flag
  bool allowing_unregistered() const;

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
  class LibraryCollection;
  boost::scoped_ptr<LibraryCollection> libEntries_;
};

} // end of namespace datatools


#endif // DATATOOLS_LIBRARY_LOADER_H_

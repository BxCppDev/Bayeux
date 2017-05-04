/// \file datatools/library_loader.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-30
 * Last modified : 2013-04-19
 *
 * Copyright (C) 2014 Ben Morgan <Ben.Morgan@warwick.ac.uk>
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
 *  A shared library loader facility (based on kwsys).
 *
 */
#ifndef DATATOOLS_LIBRARY_LOADER_H
#define DATATOOLS_LIBRARY_LOADER_H

// Standard Library:
#include <iostream>
#include <string>

// Third Party:
// - Boost:
#include <boost/cstdint.hpp>
#include <boost/utility.hpp>
#include <boost/scoped_ptr.hpp>

// This Project:
#include <datatools/datatools_config.h>
#include <datatools/bit_mask.h>
#include <datatools/detail/DynamicLoader.h>
#include <datatools/multi_properties.h>

namespace datatools {
//! \brief A DLL loader class
class library_loader
  : boost::noncopyable
{
 public:
  typedef datatools::detail::DynamicLoader::SymbolPointer symbol_ptr;

 public:
  //! Default constructor
  library_loader();

  //! Construct with path to config file
  explicit library_loader(const std::string& config_file);

  //! Construct and instance with default flags and multi_properties
  library_loader(const datatools::multi_properties& config);

  /// Destructor
  virtual ~library_loader();

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
  /** @arg name the name of the library
   *       if the directory is empty, then the name
   *       can use the following format:
   *         name\@directory
   *  @arg directory the directory where the shared library file is stored
   *  @arg filename the name of the shared library file
   *  @arg full_path the full path of the shared library file
   *  @arg version the version of the shared library
   */
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
  datatools::multi_properties config_; //!< Configuration
  class LibraryCollection;
  boost::scoped_ptr<LibraryCollection> libEntries_;
};

} // end of namespace datatools

#endif // DATATOOLS_LIBRARY_LOADER_H

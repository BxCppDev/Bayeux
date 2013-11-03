// -*- mode: c++; -*-
//! \file    bayeux/reloc.h
//! \brief   Utilities for accessing Bayeux relocatable directories
//! \details The Bayeux library makes use of several resource files
//!          containing isotopic and radioactive decays data.
//!
//!          As these files are not compiled into the Bayeux library,
//!          Bayeux provides a simple API to get a path to known
//!          resource files.
//
// Copyright (c) 2013 by François Mauger <mauger@lpccaeb.in2p3.fr>
//
// This file is part of Bayeux.
//
// Bayeux is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux.  If not, see <http://www.gnu.org/licenses/>.

#ifndef BAYEUX_RELOC_HPP
#define BAYEUX_RELOC_HPP
// Standard Library
#include <string>

// Third Party
// - A

// This Project

namespace bayeux {

  //! Init directories.
  int init_directories();

  //! Return URL, i.e. a path, to the installation directory where Bayeux is installed
  std::string get_prefix_dir();

  //! Return URL, i.e. a path, to the directory where Bayeux's executable are installed
  std::string get_binary_dir();

  //! Return URL, i.e. a path, to the directory where Bayeux's libraries are installed
  std::string get_library_dir();

  //! Return URL, i.e. a path, to the base directory where Bayeux's data are installed
  std::string get_data_dir();

  //! Return URL, i.e. a path, to the base directory where Bayeux's resource files are installed
  std::string get_resource_dir();

  //! Return URL, i.e. a path, to the base directory where Bayeux's documentation is installed
  std::string get_doc_dir();

} // end of namespace bayeux

#endif // BAYEUX_RELOC_HPP

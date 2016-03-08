//! \file    bayeux/bayeux.h
//! \brief   Provide system init for Bayeux internals
//! \details
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick
// Copyright (c) 2013 by Francois Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2013 by Université de Caen
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
//

#ifndef BAYEUX_BAYEUX_H
#define BAYEUX_BAYEUX_H

// Standard Library:
#include <string>

// Third Party:
// - Boost:
#include <boost/noncopyable.hpp>

// This project:
#include <datatools/logger.h>

/// Top-level namespace of the Bayeux library
namespace bayeux {

  /// Initialize the Bayeux library core functionnalities
  void initialize(int argc_ = 0, char * argv_[] = 0);

  /// Terminate the Bayeux library core functionnalities
  void terminate();

  void _special_initialize_impl();

  void _special_terminate_impl();

  /// \brief Nested namespace of the Bayeux library (detail)
  namespace detail {

    /// \brief System class for developpers:
    struct sys : private boost::noncopyable
    {
      /// Default constructor
      sys();

      /// Destructor
      virtual ~sys();

      /// Return the logging priority
      datatools::logger::priority get_logging() const;

      /// Access to the 'sys' singleton (read-only)
      static const sys & const_instance();

    private:

      datatools::logger::priority _logging_; //!< Logging priority threshold

    };
  } // end of namespace detail

}

/// bayeux initialization macro using main function arguments
#define BAYEUX_INIT_MAIN(Argc,Argv)   \
  ::bayeux::initialize( Argc , Argv );    \
  /**/

/// bayeux initialization macro
#define BAYEUX_INIT()       \
  ::bayeux::initialize(0, 0);     \
  /**/

/// bayeux termination macro
#define BAYEUX_FINI()       \
  ::bayeux::terminate();      \
  /**/

#endif // BAYEUX_BAYEUX_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

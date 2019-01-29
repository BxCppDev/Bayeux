// bayeux/bayeux.cc - Implementation of bayeux init function
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick
// Copyright (c) 2013-2016 by Francois Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2013-2016 by Université de Caen Normandie
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

// Standard library
#include <cstdlib>

// This project
#include <datatools/datatools.h>
#include <datatools/logger.h>

// Ourselves
#include <bayeux/bayeux_config.h> // IWYU pragma: keep
#include <bayeux/bayeux.h>
#include <bayeux/detail/bayeux_library.h>

namespace {

  static bool _bxinit = false;

  void _special_initialize_impl()
  {
    ::datatools::logger::priority logging = ::bayeux::detail::bayeux_library::process_logging_env();
    DT_LOG_TRACE_ENTERING(logging);
    // bayeux::init_directories();

    if (!::bayeux::detail::bayeux_library::is_instantiated()) {
      DT_LOG_TRACE(logging, "Instantiating Bayeux library specific internals...");
      ::bayeux::detail::bayeux_library & bxLib = ::bayeux::detail::bayeux_library::instantiate();
      DT_LOG_TRACE(logging, "Initializing Bayeux library specific internals...");
      bxLib.initialize();
    }

    DT_LOG_TRACE_EXITING(logging);
    return;
  }

  void _special_terminate_impl()
  {
   ::datatools::logger::priority logging = ::bayeux::detail::bayeux_library::process_logging_env();
    DT_LOG_TRACE_ENTERING(logging);

    if (::bayeux::detail::bayeux_library::is_instantiated()) {
      DT_LOG_TRACE(logging, "Shutdown Bayeux library specific internals...");
      ::bayeux::detail::bayeux_library & bxLib = ::bayeux::detail::bayeux_library::instance();
      if (bxLib.is_initialized()) {
        bxLib.shutdown();
      }
      DT_LOG_TRACE(logging, "Bayeux library specific internals have been terminated.");
    }

    DT_LOG_TRACE_EXITING(logging);
    return;
  }

}

namespace bayeux {

  bool is_initialized()
  {
    return _bxinit;
  }

  void initialize(int argc_, char * argv_[], uint32_t flags_)
  {
    if (! ::_bxinit) {

      // Wrap datatools kernel initialization:
      uint32_t dt_init_flags = flags_;
      ::datatools::initialize(argc_, argv_, dt_init_flags);

      // Special initialization code:
      ::_special_initialize_impl();

      _bxinit = true;
    }
    return;
  }

  void terminate()
  {
    if (_bxinit) {

      // Special termination code:
      ::_special_terminate_impl();

      // Wrap datatools kernel termination:
      ::datatools::terminate();

      _bxinit = false;
    }
    return;
  }

} // namespace bayeux

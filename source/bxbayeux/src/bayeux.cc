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

// Ourselves
#include <bayeux/bayeux_config.h> // IWYU pragma: keep
#include <bayeux/bayeux.h>

// This project
#include <datatools/datatools.h>
#include <datatools/logger.h>

// Third party
#include <boost/smart_ptr/scoped_ptr.hpp>

namespace {
  static bool _bxinit = false;
}

namespace bayeux {

  bool is_initialized()
  {
    return _bxinit;
  }

  void initialize(int argc_, char * argv_[], uint32_t flags_)
  {
    DT_LOG_TRACE_ENTERING(detail::sys::const_instance().get_logging());
    //static bool _init = false;
    // if (! _init) {
    if (! ::_bxinit) {

      // Wrap datatools kernel initialization:
      uint32_t dt_init_flags = flags_;
      ::datatools::initialize(argc_, argv_, dt_init_flags);

      // Special initialization code:
      ::bayeux::_special_initialize_impl();

      // _init = true;
      _bxinit = true;
    }
    DT_LOG_TRACE_EXITING(detail::sys::const_instance().get_logging());
    return;
  }

  void terminate()
  {
    DT_LOG_TRACE_ENTERING(detail::sys::const_instance().get_logging());
    // static bool _terminate = false;
    // if (!_terminate) {
    if (_bxinit) {

      // Special termination code:
      ::bayeux::_special_terminate_impl();

      // Wrap datatools kernel termination:
      ::datatools::terminate();
      // _terminate = true;
      _bxinit = false;
    }
    DT_LOG_TRACE_EXITING(detail::sys::const_instance().get_logging());
    return;
  }

  namespace detail {

    sys::sys()
    {
      _logging_ = datatools::logger::PRIO_FATAL;
      {
        char * e = getenv("BAYEUX_SYS_LOGGING");
        if (e) {
          std::string level_label(e);
          ::datatools::logger::priority prio = ::datatools::logger::get_priority(level_label);
          if (prio != ::datatools::logger::PRIO_UNDEFINED) {
            _logging_ = prio;
          } else {
            DT_LOG_WARNING(::datatools::logger::PRIO_WARNING,
                           "Ignoring invalid BAYEUX_SYS_LOGGING=\"" << level_label << "\" environment!");
          }
        }
      }
      return;
    }

    sys::~sys()
    {
      _logging_ = datatools::logger::PRIO_FATAL;
      return;
    }

    datatools::logger::priority sys::get_logging() const
    {
      return _logging_;
    }

    // static
    const sys & sys::const_instance()
    {
      static boost::scoped_ptr<sys> _sys(new sys);
#ifdef BAYEUX_SYS_DEVEL
      _sys->_logging_ = datatools::logger::PRIO_TRACE;
#endif // BAYEUX_SYS_DEVEL
      return *_sys;
    }

  } // end of namespace detail

} // namespace bayeux

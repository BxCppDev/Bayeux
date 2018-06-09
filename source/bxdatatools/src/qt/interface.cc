/// \file datatools/qt/interface.cc
/*
 * Copyright (C) 2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 */

// Ourselves:
#include <datatools/qt/interface.h>

// Standard library:
#include <cstdlib>
#include <cstring>
#include <clocale>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Qt:
// #include <QStyleFactory>
#include <QApplication>

// This project:
#include <datatools/logger.h>

namespace datatools {

  namespace qt {

    /*
    /// \brief Not used yet
    class interface_impl
    {
      int _argc_;            //!< Command line argument count
      char ** _argv_;        //!< Command line arguments
      bool _arg_owned_;      //!< Flag for command line arguments ownership
      bool _external_app_;   //!< Flag for external Qt application
      bool _qt_initialized_; //!< Flag for Qt initialization
    };
    */

    interface::interface(int argc_, char ** argv_, const char * name_)
    {
      _argc_ = 0;
      _argv_ = NULL;
      _arg_owned_ = false;
      _external_app_ = false;
      _qt_initialized_ = false;
      _qt_app_ = 0;
      if (QApplication::instance()) {
        _external_app_ = true;
        _qt_initialized_ = true;
      } else {
        if (! _qt_initialized_) {
          if (argc_ != 0) {
            _argc_ = argc_;
            _argv_ = argv_;
          } else {
            _argc_ = 1;
            _argv_ = (char **) malloc( 1 * sizeof(char *));
            _argv_[0] = (char *) malloc((std::strlen(name_) + 2) * sizeof(char));
            _arg_owned_ = true;
           std::memcpy(_argv_[0], name_, std::strlen(name_) + 1);
          }
          // QApplication::setStyle(QStyleFactory::create(QString::fromStdString(_gui_style_)));
          _qt_app_ = new QApplication(_argc_, _argv_);
          // Force numeric to basic locale:
          if (! QApplication::instance()) {
            DT_LOG_ERROR(datatools::logger::PRIO_ALWAYS, "Unable to initialize Qt!");
          } else {
            _qt_initialized_ = true;
          }
          std::setlocale(LC_NUMERIC, "C");
        }
      }
      return;
    }

    interface::~interface()
    {
      // std::cerr << "DEVEL: qt::interface::~interface: Entering." << std::endl;
      if (_qt_initialized_) {
        if (_qt_app_) {
          // QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
          // delete _qt_app_; // Core dump !!!
          _qt_app_ = 0;
        }
      }
      if (_arg_owned_) {
        for (int i = 0; i < _argc_; i++) {
          if (_argv_[i]) {
            free(_argv_[i]);
          }
        }
        if (_argv_) {
          free(_argv_);
        }
      }
      // std::cerr << "DEVEL: qt::interface::~interface: Exiting." << std::endl;
      return;
    }

    // static
    interface & interface::instance()
    {
      return instance(0, 0, (const char *) "Bayeux");
    }

    // static
    interface & interface::instance(int argc_, char ** argv_, const char * name_)
    {
      static boost::scoped_ptr<interface> _i(new interface(argc_, argv_, name_));
      return *_i;
    }

    bool interface::is_initialized() const
    {
      return _qt_initialized_;
    }

    bool interface::is_external_app() const
    {
      return _external_app_;
    }

  } // end of namespace qt

} // end of namespace datatools

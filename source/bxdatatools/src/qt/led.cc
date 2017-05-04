//! \file    datatools/qt/led.cc
//! \brief   A simple LED Qt-based widget
//! \details
//
// Copyright (c) 2014 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2014 by Université de Caen
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
/***************************************************************************
 *   Copyright (C) 2010 by P. Sereno                                       *
 *   http://www.sereno-online.com                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License           *
 *   version 2.1 as published by the Free Software Foundation              *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Lesser General Public License for more details.                   *
 *   http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.               *
 ***************************************************************************/

// Ourselves:
#include <datatools/qt/led.h>

// Standard library:
#include <string>
#include <sstream>
#include <iostream>

// Third party:
// - Qt:
#include <QColor>
#include <QString>
#include <QtSvg/QSvgRenderer>
#include <QtGui/QPainter>

/*
#include <QtGlobal>
#include <QtGui>
#include <QPolygon>
#include <QSvgRenderer>
*/

// This project:
#include <datatools/utils.h>

namespace datatools {

  namespace qt {

    void led::_set_default()
    {
      _value = false;
      _shapes << "circle_"
              << "square_"
              << "triang_"
              << "round_";
      _colors << "red"
              << "green"
              << "yellow"
              << "grey"
              << "orange"
              << "purple"
              << "blue";
      _on_color = Red;
      _off_color = Grey;
      _shape = Circle;
      _on_renderer_ = 0;
      _off_renderer_ = 0;
      // resize(32,32);
      setMinimumSize(15, 15);
      setMaximumSize(50, 50);
      return;
    }

    /*!
      \brief led: this is the led constructor.
      \param parent_: The Parent Widget
    */
    led::led(led_shape_type shape_,
             led_color_type on_color_,
             led_color_type off_color_,
             QWidget * parent_) : QWidget(parent_)
    {
      _set_default();
      _on_renderer_ = new QSvgRenderer();
      _off_renderer_ = new QSvgRenderer();
      set_shape(shape_);
      set_on_color(on_color_);
      set_off_color(off_color_);
      _reload_svg_();
      return;
    }

    /*!
      \brief led: this is the led constructor.
      \param parent_: The Parent Widget
    */
    led::led(QWidget * parent_) : QWidget(parent_)
    {
      _set_default();
      _on_renderer_ = new QSvgRenderer();
      _off_renderer_ = new QSvgRenderer();
      _reload_svg_();
      return;
    }

    led::~led()
    {
      if (_on_renderer_) {
        delete _on_renderer_;
      }
      if (_off_renderer_) {
        delete _off_renderer_;
      }
      return;
    }


    void led::_reload_svg_()
    {
      // std::cerr << "DEVEL: datatools::qt::led::_reload_svg_: Entering...\n";
      QString ledShapeAndColor[2];
      for (int i = 0; i < 2; i++) {
        std::ostringstream svg_filename_ss;
        svg_filename_ss << "@datatools:" << "qt/led/";
        svg_filename_ss << _shapes[_shape].toStdString();
        if (i == 0) {
          svg_filename_ss << _colors[_on_color].toStdString();
        } else {
          svg_filename_ss << _colors[_off_color].toStdString();
        }
        svg_filename_ss << ".svg";
        std::string svg_filename = svg_filename_ss.str();
        // std::cerr << "DEVEL: datatools::qt::led::_reload_svg_: svg_filename='" << svg_filename << "'\n";
        datatools::fetch_path_with_env(svg_filename);
        // std::cerr << "DEVEL: datatools::qt::led::_reload_svg_: svg_filename='" << svg_filename << "'\n";
        ledShapeAndColor[i] = QString::fromStdString(svg_filename);
        // std::cerr << "DEVEL: datatools::qt::led::_reload_svg_: ledShapeAndColor[" << i << "]='" << ledShapeAndColor[i].toStdString()  << "'\n";
      }
      _on_renderer_->load(ledShapeAndColor[0]);
      _off_renderer_->load(ledShapeAndColor[1]);
      // std::cerr << "DEVEL: datatools::qt::led::_reload_svg_: Exiting.\n";
      return;
    }

    /*!
      \brief paintEvent: painting method
      \param QPaintEvent *
      \return void
    */
    void led::paintEvent(QPaintEvent *)
    {
      resize(20, 20);
      QPainter painter(this);
      painter.setRenderHint(QPainter::Antialiasing, true);
      if (_value) {
        _on_renderer_->render(&painter);
      } else {
        _off_renderer_->render(&painter);
      }
      return;
    }

    QSize led::sizeHint () const
    {
      QSize sz(20, 20);
      return sz;
    }

    /*!
      \brief set_on_color: this method allows to change the On color {Red,Green,Yellow,Grey,Orange,Purple,blue}
      \param led_color_type new_color_
      \return void
    */
    void led::set_on_color(led_color_type new_color_)
    {
      _on_color = new_color_;
      _reload_svg_();
      update();
      return;
    }


    /*!
      \brief set_off_color: this method allows to change the Off color {Red,Green,Yellow,Grey,Orange,Purple,blue}
      \param led_color_type new_color_
      \return void
    */
    void led::set_off_color(led_color_type new_color_)
    {
      _off_color = new_color_;
      _reload_svg_();
      update();
      return;
    }


    /*!
      \brief set_shape: this method allows to change the led shape {Circle,Square,Triangle,Rounded rectangle}
      \param led_shape_type new_shape_
      \return void
    */
    void led::set_shape(led_shape_type new_shape_)
    {
      _shape = new_shape_;
      _reload_svg_();
      update();
      return;
    }


    /*!
      \brief set_value: this method allows to set the led value {true,false}
      \param bool value_
      \return void
    */
    void led::set_value(bool value_)
    {
      _value = value_;
      //_reload_svg_();
      update();
      return;
    }


    /*!
      \brief toggle_value: this method toggles the led value
      \return void
    */
    void led::toggle_value()
    {
      _value = ! _value;
      update();
      return;
    }

  } // end of namespace qt

} // end of namespace datatools

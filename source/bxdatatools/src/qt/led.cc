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
      _three_states_ = false;
      _value = ValueOff;
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
      _on_color = Green;
      _off_color = Grey;
      _indeterminate_color = Orange;
      _shape = Circle;
      _on_renderer_ = nullptr;
      _off_renderer_ = nullptr;
      _indeterminate_renderer_ = nullptr;
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
    led::led(led_shape_type shape_,
             led_color_type on_color_,
             led_color_type off_color_,
             led_color_type indeterminate_color_,
             QWidget * parent_) : QWidget(parent_)
    {
      _set_default();
      _three_states_ = true;
      _on_renderer_ = new QSvgRenderer();
      _off_renderer_ = new QSvgRenderer();
      _indeterminate_renderer_ = new QSvgRenderer();
      set_shape(shape_);
      set_on_color(on_color_);
      set_off_color(off_color_);
      set_indeterminate_color(indeterminate_color_);
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
      if (_indeterminate_renderer_) {
        delete _indeterminate_renderer_;
      }
      return;
    }


    void led::_reload_svg_()
    {
      QString ledShapeAndColor[3];
      int maxColors = 2;
      if (_three_states_) maxColors = 3;
      for (int i = 0; i < maxColors; i++) {
        std::ostringstream svg_filename_ss;
        svg_filename_ss << "@datatools:" << "qt/led/";
        svg_filename_ss << _shapes[_shape].toStdString();
        if (i == 0) {
          svg_filename_ss << _colors[_on_color].toStdString();
        } else if (i == 1) {
          svg_filename_ss << _colors[_off_color].toStdString();
        } else if (i == 2) {
          svg_filename_ss << _colors[_indeterminate_color].toStdString();
        }
        svg_filename_ss << ".svg";
        std::string svg_filename = svg_filename_ss.str();
        datatools::fetch_path_with_env(svg_filename);
        ledShapeAndColor[i] = QString::fromStdString(svg_filename);
      }
      _on_renderer_->load(ledShapeAndColor[0]);
      _off_renderer_->load(ledShapeAndColor[1]);
      if (_three_states_) {
        _indeterminate_renderer_->load(ledShapeAndColor[2]);
      }
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
      if (_three_states_ && _value == ValueIndeterminate) {
         _indeterminate_renderer_->render(&painter);
         return;
      }
      if (_value == ValueOn) {
        _on_renderer_->render(&painter);
      } else if (_value == ValueOff){
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
      \brief set_indeterminate_color: this method allows to change the Indeterminate color {Red,Green,Yellow,Grey,Orange,Purple,blue}
      \param led_color_type new_color_
      \return void
    */
    void led::set_indeterminate_color(led_color_type new_color_)
    {
      _indeterminate_color = new_color_;
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
      if (value_) {
        _value = ValueOn;
      } else {
        _value = ValueOff;
      } 
      update();
      return;
    }

    void led::set_indeterminate()
    {
      if (_three_states_) {
        _value = ValueIndeterminate;
      }
      update();
      return;
    }
    
    /*!
      \brief toggle_value: this method toggles the led value
      \return void
    */
    void led::toggle_value()
    {
      if (_three_states_ && _value == ValueIndeterminate) {
        return;
      }
      if (_value == ValueOn) {
        _value = ValueOff;
      } else if (_value == ValueOff) {
        _value = ValueOn;
      }
      update();
      return;
    }

    void led::set(const boost::logic::tribool t_)
    {
      if (_three_states_ && boost::logic::indeterminate(t_)) {
        _value = ValueIndeterminate;
        update();
        return;
      }
      if (t_) {
        _value = ValueOn;
      } else if (!t_) {
        _value = ValueOff;
      }
      update();
      return;
    }

  } // end of namespace qt

} // end of namespace datatools

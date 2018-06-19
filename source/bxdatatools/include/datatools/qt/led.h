//! \file    datatools/qt/led.h
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

#ifndef DATATOOLS_QT_LED_H
#define DATATOOLS_QT_LED_H

// Third party:
// - Qt:
//#include <Qt>
#include <QObject>
#include <QWidget>
#include <QStringList>
// - Boost:
#include <boost/logic/tribool.hpp>

// Forward declarations:
class QColor;
class QSvgRenderer;

namespace datatools {

  namespace qt {

    /// \brief A LED widget with ON/OFF(/INDETERMINATE) status
    class led : public QWidget
    {
      Q_OBJECT
      Q_ENUMS (led_color_type)
      Q_ENUMS (led_shape_type)
      Q_PROPERTY(bool value READ get_value WRITE set_value)
      Q_PROPERTY(led_color_type on_color READ get_on_color WRITE set_on_color)
      Q_PROPERTY(led_color_type off_color READ get_off_color WRITE set_off_color)
      Q_PROPERTY(led_color_type indeterminate_color READ get_indeterminate_color WRITE set_indeterminate_color)
      Q_PROPERTY(led_shape_type shape READ get_shape WRITE set_shape)

    public:

      /// Supported colors
      enum led_color_type {
        Red = 0,
        Green,
        Yellow,
        Grey,
        Orange,
        Purple,
        Blue
      };

      /// Supported shapes
      enum led_shape_type {
        Circle = 0,
        Square,
        Triangle,
        Rounded
      };

      enum value_type {
        ValueOff = 0,
        ValueOn  = 1,
        ValueIndeterminate = 2
      };

      /// Default constructor
      led(QWidget * parent = 0);

      /// Default constructor
      led(led_shape_type shape_,
          led_color_type on_color_,
          led_color_type off_color_,
          QWidget * parent = 0);

      /// Default constructor
      led(led_shape_type shape_,
          led_color_type on_color_,
          led_color_type off_color_,
          led_color_type indeterminate_color_,
          QWidget * parent = 0);

      /// Destructor
      virtual ~led();

      /// Return the value
      bool get_value() const { return _value; }

      /// Return the ON color
      led_color_type get_on_color() const { return _on_color; }

      /// Return the OFF color
      led_color_type get_off_color() const { return _off_color; }

      /// Return the indeterminate color
      led_color_type get_indeterminate_color() const { return _indeterminate_color; }

      /// Return the shape
      led_shape_type get_shape() const { return _shape; }

    public slots:

      /// Set the value
      void set_value(bool);

      /// Set the inderminate value
      void set_indeterminate();

      /// Set the value
      void set(const boost::logic::tribool t_);

      /// Set the ON color
      void set_on_color(led_color_type);

      /// Set the OFF color
      void set_off_color(led_color_type);

      /// Set the indeterminate color
      void set_indeterminate_color(led_color_type);

      /// Set the shape
      void set_shape(led_shape_type);

      /// Toggle value
      void toggle_value();

    protected:

      /// Qt widget interface
      virtual void paintEvent(QPaintEvent *event);

      /// Qt widget interface
      virtual QSize sizeHint() const;

    protected:

      /// Set default attributes
      void _set_default();

    private:

      /// Reload the SVG file in the SVG renderer
      void _reload_svg_();

    protected:

      bool           _three_states_ = false;
      value_type     _value;    //!< Current value (On/Off)
      led_color_type _on_color; //!< On color
      led_color_type _off_color; //!< Off color
      led_color_type _indeterminate_color; //!< Indeterminate color
      led_shape_type _shape;  //!< Current shape of the LED
      QStringList    _shapes; //!< List of supported shapes
      QStringList    _colors; //!< List of supported colors

    private:

      QSvgRenderer * _on_renderer_ = nullptr;  //!< The On value SVG renderer
      QSvgRenderer * _off_renderer_ = nullptr; //!< The Off value SVG renderer
      QSvgRenderer * _indeterminate_renderer_ = nullptr; //!< The Indeterminate value SVG renderer

    };

  } // end of namespace qt

} // end of namespace datatools

#endif // DATATOOLS_QT_LED_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

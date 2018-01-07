//! \file  datatools/ui/ansi_colors.h
//! \brief ANSI colors support
//
// Copyright (c) 2017 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of datatools.
//
// datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools. If not, see <http://www.gnu.org/licenses/>.

#ifndef DATATOOLS_UI_ANSI_COLORS_H
#define DATATOOLS_UI_ANSI_COLORS_H

// Standard libraries:
#include <string>
#include <iostream>

namespace datatools {

  namespace ui {

    namespace ansi_colors {

      struct ansi_code
      {
        ansi_code(const int code_);
        friend std::ostream & operator<<(std::ostream & out_, const ansi_code &);
        int _code_ = 0;
      };

      struct color_8bits
      {
        static const unsigned int BLACK        = 0;
        static const unsigned int DARK_RED     = 1;
        static const unsigned int DARK_GREEN   = 2;
        static const unsigned int DARK_YELLOW  = 3;
        static const unsigned int DARK_BLUE    = 4;
        static const unsigned int DARK_MAGENTA = 5;
        static const unsigned int DARK_CYAN    = 6;
        static const unsigned int LIGHT_GRAY   = 7;
        static const unsigned int DARK_GRAY    = 8;
        static const unsigned int RED          = 9;
        static const unsigned int GREEN        = 10;
        static const unsigned int YELLOW       = 11;
        static const unsigned int BLUE         = 12;
        static const unsigned int MAGENTA      = 13;
        static const unsigned int CYAN         = 14;
        static const unsigned int WHITE        = 15;
        static const unsigned int GRAY00       = 232;
        static const unsigned int GRAY01       = 233;
        static const unsigned int GRAY02       = 234;
        static const unsigned int GRAY03       = 235;
        static const unsigned int GRAY04       = 236;
        static const unsigned int GRAY05       = 237;
        static const unsigned int GRAY06       = 238;
        static const unsigned int GRAY07       = 239;
        static const unsigned int GRAY08       = 240;
        static const unsigned int GRAY09       = 241;
        static const unsigned int GRAY10       = 242;
        static const unsigned int GRAY11       = 243;
        static const unsigned int GRAY12       = 244;
        static const unsigned int GRAY13       = 245;
        static const unsigned int GRAY14       = 246;
        static const unsigned int GRAY15       = 247;
        static const unsigned int GRAY16       = 248;
        static const unsigned int GRAY17       = 249;
        static const unsigned int GRAY18       = 250;
        static const unsigned int GRAY19       = 251;
        static const unsigned int GRAY20       = 252;
        static const unsigned int GRAY21       = 253;
        static const unsigned int GRAY22       = 254;
        static const unsigned int GRAY23       = 255;

        enum mode_type {
          MODE_STANDARD  = 0,
          MODE_RGB666    = 1,
          MODE_GRAYSCALE = 2
        };

        color_8bits(const mode_type mode_,
                    const unsigned int n_,
                    const unsigned int r_ = 0,
                    const unsigned int g_ = 0,
                    const unsigned int b_ = 0);

        color_8bits(const unsigned int n_);

        color_8bits(const unsigned int r_, const unsigned int g_, const unsigned int b_);

        friend std::ostream & operator<<(std::ostream & out_, const color_8bits & c8_);

        unsigned int _n_ = 0;
        unsigned int _r_ = 0;
        unsigned int _g_ = 0;
        unsigned int _b_ = 0;
      };

      typedef color_8bits color256;

      struct reset
        : public ansi_code
      {
        reset() : ansi_code(0) {}
      };

      struct high_intensity_on
        : public ansi_code
      {
        high_intensity_on() : ansi_code(1) {}
      };

      struct low_intensity_on
        : public ansi_code
      {
        low_intensity_on() : ansi_code(2) {}
      };

      struct italics_on
        : public ansi_code
      {
        italics_on() : ansi_code(3) {}
      };

      struct underline_on
        : public ansi_code
      {
        underline_on() : ansi_code(4) {}
      };

      struct blink_slow
        : public ansi_code
      {
        blink_slow() : ansi_code(5) {}
      };

      struct blink_fast
        : public ansi_code
      {
        blink_fast() : ansi_code(6) {}
      };

      struct inverse_on
        : public ansi_code
      {
        inverse_on() : ansi_code(7) {}
      };

      struct concealed_on
        : public ansi_code
      {
        concealed_on() : ansi_code(8) {}
      };

      struct strikethrough_on
        : public ansi_code
      {
        strikethrough_on() : ansi_code(9) {}
      };

      struct normal_intensity
        : public ansi_code
      {
        normal_intensity() : ansi_code(22) {}
      };

      struct italic_off
        : public ansi_code
      {
        italic_off() : ansi_code(23) {}
      };

      struct underline_off
        : public ansi_code
      {
        underline_off() : ansi_code(24) {}
      };

      struct blink_off
        : public ansi_code
      {
        blink_off() : ansi_code(25) {}
      };

      struct inverse_off
        : public ansi_code
      {
        inverse_off() : ansi_code(27) {}
      };

      struct concealed_off
        : public ansi_code
      {
        concealed_off() : ansi_code(28) {}
      };

      struct strikethrough_off
        : public ansi_code
      {
        strikethrough_off() : ansi_code(29) {}
      };

      struct foreground_black
        : public ansi_code
      {
        foreground_black() : ansi_code(30) {}
      };

      struct foreground_bright_black
        : public ansi_code
      {
        foreground_bright_black() : ansi_code(90) {}
      };

      typedef foreground_bright_black foreground_gray;

      struct foreground_red
        : public ansi_code
      {
        foreground_red() : ansi_code(31) {}
      };

      struct foreground_bright_red
        : public ansi_code
      {
        foreground_bright_red() : ansi_code(91) {}
      };

      struct foreground_green
        : public ansi_code
      {
        foreground_green() : ansi_code(32) {}
      };

      struct foreground_bright_green
        : public ansi_code
      {
        foreground_bright_green() : ansi_code(92) {}
      };

      struct foreground_yellow
        : public ansi_code
      {
        foreground_yellow() : ansi_code(33) {}
      };

      struct foreground_bright_yellow
        : public ansi_code
      {
        foreground_bright_yellow() : ansi_code(93) {}
      };

      struct foreground_blue
        : public ansi_code
      {
        foreground_blue() : ansi_code(34) {}
      };

      struct foreground_bright_blue
        : public ansi_code
      {
        foreground_bright_blue() : ansi_code(94) {}
      };

      struct foreground_magenta
        : public ansi_code
      {
        foreground_magenta() : ansi_code(35) {}
      };

      struct foreground_bright_magenta
        : public ansi_code
      {
        foreground_bright_magenta() : ansi_code(95) {}
      };

      struct foreground_cyan
        : public ansi_code
      {
        foreground_cyan() : ansi_code(36) {}
      };

      struct foreground_bright_cyan
        : public ansi_code
      {
        foreground_bright_cyan() : ansi_code(96) {}
      };

      struct foreground_white
        : public ansi_code
      {
        foreground_white() : ansi_code(37) {}
      };

      struct foreground_bright_white
        : public ansi_code
      {
        foreground_bright_white() : ansi_code(97) {}
      };

      struct restore_default
        : public ansi_code
      {
        restore_default() : ansi_code(39) {}
      };

      struct background_black
        : public ansi_code
      {
        background_black() : ansi_code(40) {}
      };

      struct background_bright_black
        : public ansi_code
      {
        background_bright_black() : ansi_code(100) {}
      };

      struct background_red
        : public ansi_code
      {
        background_red() : ansi_code(41) {}
      };

      struct background_bright_red
        : public ansi_code
      {
        background_bright_red() : ansi_code(101) {}
      };

      struct background_green
        : public ansi_code
      {
        background_green() : ansi_code(42) {}
      };

      struct background_bright_green
        : public ansi_code
      {
        background_bright_green() : ansi_code(102) {}
      };

      struct background_yellow
        : public ansi_code
      {
        background_yellow() : ansi_code(43) {}
      };

      struct background_bright_yellow
        : public ansi_code
      {
        background_bright_yellow() : ansi_code(103) {}
      };

      struct background_blue
        : public ansi_code
      {
        background_blue() : ansi_code(44) {}
      };

      struct background_bright_blue
        : public ansi_code
      {
        background_bright_blue() : ansi_code(104) {}
      };

      struct background_magenta
        : public ansi_code
      {
        background_magenta() : ansi_code(45) {}
      };

      struct background_bright_magenta
        : public ansi_code
      {
        background_bright_magenta() : ansi_code(105) {}
      };

      struct background_cyan
        : public ansi_code
      {
        background_cyan() : ansi_code(46) {}
      };

      struct background_bright_cyan
        : public ansi_code
      {
        background_bright_cyan() : ansi_code(106) {}
      };

      struct background_white
        : public ansi_code
      {
        background_white() : ansi_code(47) {}
      };

      struct background_bright_white
        : public ansi_code
      {
        background_bright_white() : ansi_code(107) {}
      };

      struct default_background
        : public ansi_code
      {
        default_background() : ansi_code(49) {}
      };

      struct framed_on
        : public ansi_code
      {
        framed_on() : ansi_code(51) {}
      };

      struct encircled_on
        : public ansi_code
      {
        encircled_on() : ansi_code(52) {}
      };

      struct overlined_on
        : public ansi_code
      {
        overlined_on() : ansi_code(53) {}
      };

      struct framed_off
        : public ansi_code
      {
        framed_off() : ansi_code(54) {}
      };

      struct overlined_off
        : public ansi_code
      {
        overlined_off() : ansi_code(55) {}
      };

    } // namespace ansi_colors

  } // namespace ui

} // namespace datatools

#endif // DATATOOLS_UI_UTILS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

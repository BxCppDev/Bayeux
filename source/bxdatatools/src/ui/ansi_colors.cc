// datatools/ui/ansi_colors.cc
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

// Ourselves:
#include <datatools/ui/ansi_colors.h>

// This project:
#include <datatools/datatools_config.h>

namespace datatools {

  namespace ui {

    namespace ansi_colors {

      ansi_code::ansi_code(const int code_)
        : _code_(code_)
      {
        return;
      }

      // friend
      std::ostream & operator<<(std::ostream & out_, const ansi_code & code_)
      {
#if DATATOOLS_WITH_ANSI_COLORS == 1
        out_ << "\x1B[";
        out_ << code_._code_ << "m";
#endif
        return out_;
      }

      color_8bits::color_8bits(const mode_type mode_,
                               const unsigned int n_,
                               const unsigned int r_,
                               const unsigned int g_,
                               const unsigned int b_)
        : _n_(0)
        , _r_(0)
        , _g_(0)
        , _b_(0)
      {
        if (mode_ == MODE_STANDARD) {
          _n_ = n_;
        } else if (mode_ == MODE_RGB666) {
          _r_ = r_> 5 ? 5 : r_;
          _g_ = g_> 5 ? 5 : g_;
          _b_ = b_> 5 ? 5 : b_;
        } else {
         _n_ = 232 + n_;
        }
        return;
      }

      color_8bits::color_8bits(const unsigned int n_)
        : _n_(n_)
        , _r_(0)
        , _g_(0)
        , _b_(0)
      {
        return;
      }

      color_8bits::color_8bits(const unsigned int r_, const unsigned int g_, const unsigned int b_)
        : _n_(0)
        , _r_(r_> 5 ? 5 : r_)
        , _g_(g_> 5 ? 5 : g_)
        , _b_(b_> 5 ? 5 : b_)
      {
        return;
      }

      // friend
      std::ostream & operator<<(std::ostream & out_, const color_8bits & c8_)
      {
#if DATATOOLS_WITH_ANSI_COLORS == 1
        out_ << "\x1B[38;5;";
        if (c8_._n_) {
          out_ << c8_._n_;
        } else {
          int rgb666 = 16 + 36 * c8_._r_ + 6 * c8_._g_ + c8_._b_;
          out_ << rgb666;
        }
        out_ << "m";
#endif
        return out_;
      }

    } // namespace ansi_colors

  } // namespace ui

} // namespace datatools

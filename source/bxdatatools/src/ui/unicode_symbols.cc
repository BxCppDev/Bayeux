// datatools/ui/unicode_symbols.cc
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

// Ourselves
#include <datatools/ui/unicode_symbols.h>

namespace datatools {

  namespace ui {

    namespace unicode {

      const std::string & check_mark()
      {
        static const std::string _check_mark = "\u2714";
        return _check_mark;
      }

      const std::string & error_mark()
      {
        static const std::string _error_mark = "\u2718";
        return _error_mark;
      }

    } // namespace unicode

  } // namespace ui

} // namespace datatools

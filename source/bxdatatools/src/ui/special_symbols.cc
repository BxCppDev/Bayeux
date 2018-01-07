// datatools/ui/special_symbols.cc
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
#include <datatools/ui/special_symbols.h>
#include <datatools/datatools_config.h>
#if DATATOOLS_WITH_UNICODE_SYMBOLS == 1
#include <datatools/ui/unicode_symbols.h>
#endif // DATATOOLS_WITH_UNICODE_SYMBOLS == 1

namespace datatools {

  namespace ui {

    bool has_unicode_symbols()
    {
#if DATATOOLS_WITH_UNICODE_SYMBOLS == 1
      return true;
#else
      return false;
#endif // DATATOOLS_WITH_UNICODE_SYMBOLS == 1
    }

    const std::string & check_mark()
    {
#if DATATOOLS_WITH_UNICODE_SYMBOLS == 1
      return unicode::check_mark();
#else
      static const std::string _check_mark = "v";
      return _check_mark;
#endif // DATATOOLS_WITH_UNICODE_SYMBOLS == 1
    }

    const std::string & error_mark()
    {
#if DATATOOLS_WITH_UNICODE_SYMBOLS == 1
      return unicode::error_mark();
#else
      static const std::string _error_mark = "x";
      return _error_mark;
#endif // DATATOOLS_WITH_UNICODE_SYMBOLS == 1
    }

  } // namespace ui

} // namespace datatools

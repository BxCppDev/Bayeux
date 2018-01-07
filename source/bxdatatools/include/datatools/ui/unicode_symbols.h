//! \file  datatools/ui/unicode_symbols.h
//! \brief UNICODE symbols support
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

#ifndef DATATOOLS_UI_UNICODE_SYMBOLS_H
#define DATATOOLS_UI_UNICODE_SYMBOLS_H

// Standard libraries:
#include <string>

namespace datatools {

  namespace ui {

    namespace unicode {

      const std::string & check_mark();

      const std::string & error_mark();

    } // namespace unicode

  } // namespace ui

} // namespace datatools

#endif // DATATOOLS_UI_UNICODE_SYMBOLS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

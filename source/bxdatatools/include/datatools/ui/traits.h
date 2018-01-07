//! \file  datatools/ui/traits.h
//! \brief Definition of traits used in the interface hierachical system
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

#ifndef DATATOOLS_UI_TRAITS_H
#define DATATOOLS_UI_TRAITS_H

// Standard library:
#include <string>

namespace datatools {

  namespace ui {

    struct traits {

      //! Prefix for all trait's keys
      static const std::string & key_prefix();

      //! Build the full key for a trait
      std::string build_key(const std::string & topic_);

      //! Busy command label
      static const std::string & busy_label();

      //! Disabled command label
      static const std::string & disabled_label();

      //! Broken command label
      static const std::string & broken_label();

      //! Hidden command label
      static const std::string & hidden_label();

    };

  } // namespace ui

} // namespace datatools

#endif //  DATATOOLS_UI_TRAITS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

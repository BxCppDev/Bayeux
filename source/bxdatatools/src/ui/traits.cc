// datatools/ui/traits.cc
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
#include <datatools/ui/traits.h>

namespace datatools {

  namespace ui {

    // static
    const std::string & traits::key_prefix()
    {
      static const std::string _p("__datatools__ui__traits.");
      return _p;
    }

    std::string traits::build_key(const std::string & topic_)
    {
      std::string full_key = key_prefix() + topic_;
      return full_key;
    }

    // static
    const std::string & traits::busy_label()
    {
      static const std::string _k("busy");
      return _k;
    }

    // static
    const std::string & traits::disabled_label()
    {
      static const std::string _k("disabled");
      return _k;
    }

    // static
    const std::string & traits::broken_label()
    {
      static const std::string _k("broken");
      return _k;
    }

    // static
    const std::string & traits::hidden_label()
    {
      static const std::string _k("hidden");
      return _k;
    }

  } // namespace ui

} // namespace datatools

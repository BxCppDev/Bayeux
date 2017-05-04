// datatools/introspection/access_type.cc
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Datatools.
//
// Datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Datatools. If not, see <http://www.gnu.org/licenses/>.

// Ourselves:
#include <datatools/introspection/access_type.h>

// Third party:
// - Camp:
#include <camp/enum.hpp>

namespace datatools {

  namespace introspection {

    const std::string & to_string(access_type t_)
    {
      const camp::Enum & metaAccessTypeEnum = camp::enumByType<access_type>();
      return metaAccessTypeEnum.name(t_);
    }

    bool from_string(const std::string & label_, access_type & type_)
    {
      type_ = ACCESS_INVALID;
      const camp::Enum & metaAccessTypeEnum = camp::enumByType<access_type>();
      if (metaAccessTypeEnum.hasName(label_)) {
        type_= static_cast<access_type>(metaAccessTypeEnum.value(label_));
        return true;
      }
      return false;
    }

    bool is_valid(access_type t_)
    {
      return t_ != ACCESS_INVALID;
    }

    bool is_input(access_type t_)
    {
      return t_ == ACCESS_INPUT;
    }

    bool is_output(access_type t_)
    {
      return t_ == ACCESS_OUTPUT;
    }

  } // namespace introspection

} // namespace datatools

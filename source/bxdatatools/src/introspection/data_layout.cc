// datatools/introspection/data_layout.cc
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
#include <datatools/introspection/data_layout.h>

// Third party:
// - Camp:
#include <camp/enum.hpp>

namespace datatools {

  namespace introspection {

    const std::string & to_string(data_layout t_)
    {
      const camp::Enum & metaDataLayoutEnum = camp::enumByType<data_layout>();
      return metaDataLayoutEnum.name(t_);
    }

    bool from_string(const std::string & label_, data_layout & layout_)
    {
      layout_ = DATA_LAYOUT_INVALID;
      const camp::Enum & metaDataLayoutEnum = camp::enumByType<data_layout>();
      if (metaDataLayoutEnum.hasName(label_)) {
        layout_= static_cast<data_layout>(metaDataLayoutEnum.value(label_));
        return true;
      }
      return false;
    }

    bool is_valid(data_layout t_)
    {
      return t_ != DATA_LAYOUT_INVALID;
    }

    bool is_scalar(data_layout t_)
    {
      return t_ == DATA_LAYOUT_SCALAR;
    }

    bool is_vector(data_layout t_)
    {
      return t_ == DATA_LAYOUT_VECTOR_WITH_FIXED_SIZE
        ||  t_ == DATA_LAYOUT_VECTOR_WITH_FREE_SIZE
        ;
    }

  } // namespace introspection

} // namespace datatools

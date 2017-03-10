// datatools/urn_to_path.cc
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

// Standard library:
#include <sstream>

// Ourselves:
#include <datatools/urn_to_path.h>
#include <datatools/urn.h>

namespace datatools {

  DATATOOLS_SERIALIZATION_IMPLEMENTATION_ADVANCED(urn_to_path, "datatools::urn_to_path")

  urn_to_path::urn_to_path()
  {
  }

  urn_to_path::urn_to_path(const std::string & urn_,
                           const std::string & path_,
                           const std::string & category_,
                           const std::string & mime_)
  {
    set_urn(urn_);
    set_path(path_);
    set_category(category_);
    set_mime(mime_);
    return;
  }

  urn_to_path::~urn_to_path()
  {
  }

  void urn_to_path::reset()
  {
    _urn_.clear();
    _path_.clear();
    _category_.clear();
    _mime_.clear();
    return;
  }

  bool urn_to_path::is_valid() const
  {
    if (!has_urn()) return false;
    if (!has_path()) return false;
    return true;
  }

  bool urn_to_path::has_urn() const
  {
    return ! _urn_.empty();
  }

  void urn_to_path::set_urn(const std::string & u_)
  {
    {
      urn u;
      DT_THROW_IF(!u.from_string(u_),
                  std::logic_error,
                  "Invalid URN format '" << u_ << "'!");
    }
    _urn_ = u_;
    return;
  }

  const std::string & urn_to_path::get_urn() const
  {
    return _urn_;
  }

  bool urn_to_path::has_path() const
  {
    return !_path_.empty();
  }

  void urn_to_path::set_path(const std::string & p_)
  {
    {
      urn u;
      DT_THROW_IF(u.from_string(p_),
                  std::logic_error,
                  "Invalid path format '" << p_ << "'!");
    }
    _path_ = p_;
    return;
  }

  const std::string & urn_to_path::get_path() const
  {
    return _path_;
  }

  bool urn_to_path::has_category() const
  {
    return !_category_.empty();
  }

  void urn_to_path::set_category(const std::string & cat_)
  {
    _category_ = cat_;
    return;
  }

  const std::string & urn_to_path::get_category() const
  {
    return _category_;
  }

  bool urn_to_path::has_mime() const
  {
    return !_mime_.empty();
  }

  void urn_to_path::set_mime(const std::string & m_)
  {
    _mime_ = m_;
    return;
  }

  const std::string & urn_to_path::get_mime() const
  {
    return _mime_;
  }

  void urn_to_path::tree_dump(std::ostream& out_,
                              const std::string& title_,
                              const std::string& indent_,
                              bool inherit_) const
  {
    if (! title_.empty()) out_ << indent_ << title_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "URN : '" << _urn_ << "'" << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Path : '" << _path_ << "'" << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Category : ";
    if (has_category()) {
      out_ << "'" << _category_ << "'";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "MIME type : ";
    if (has_mime()) {
      out_ << "'" << _mime_ << "'";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    return;
  }

} // namespace datatools

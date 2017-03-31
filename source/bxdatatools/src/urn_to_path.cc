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
    set_category(category_);
    set_mime(mime_);
    set_path(path_);
    return;
  }

  urn_to_path::urn_to_path(const std::string & urn_,
                           const std::string & category_,
                           const std::string & mime_,
                           const std::vector<std::string> & paths_)
  {
    set_urn(urn_);
    set_category(category_);
    set_mime(mime_);
    for (auto p : paths_) {
      add_path(p);
    }
    return;
  }

  urn_to_path::~urn_to_path()
  {
  }

  void urn_to_path::reset()
  {
    _urn_.clear();
    clear_paths();
    reset_category();
    reset_mime();
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

  bool urn_to_path::has_single_path() const
  {
    return get_number_of_paths() == 1;
  }

  std::size_t urn_to_path::get_number_of_paths() const
  {
    return _paths_.size();
  }

  const std::string & urn_to_path::get_path_by_index(const std::size_t index_) const
  {
    DT_THROW_IF(index_ >= _paths_.size(), std::range_error,
                "Invalid path index [" << index_ << "] for URN='" << _urn_ << "'!");
    return _paths_[index_];
  }


  bool urn_to_path::has_path_by_value(const std::string & path_) const
  {
    return std::find(_paths_.begin(), _paths_.end(), path_) != _paths_.end();
  }

  void urn_to_path::add_path(const std::string & path_, bool preprend_)
  {
    {
      urn u;
      DT_THROW_IF(u.from_string(path_),
                  std::logic_error,
                  "Invalid path format '" << path_ << "'!");
    }
    DT_THROW_IF(has_path_by_value(path_), std::logic_error,
                "Path '" << path_ << "' is already set in the list of paths for URN='" << _urn_ << "'!");
    if (!preprend_) {
      _paths_.push_back(path_);
    } else {
      _paths_.insert(_paths_.begin(), path_);
    }
    return;
  }

  bool urn_to_path::has_path() const
  {
    return _paths_.size() > 0;
  }

  void urn_to_path::set_path(const std::string & path_)
  {
    add_path(path_, true);
    return;
  }

  const std::string & urn_to_path::get_path() const
  {
    return get_path_by_index(0);
  }

  void urn_to_path::clear_paths()
  {
    _paths_.clear();
    return;
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

  void urn_to_path::reset_category()
  {
    _category_.clear();
    return;
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

  void urn_to_path::reset_mime()
  {
    _mime_.clear();
    return;
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
         << "Category : ";
    if (has_category()) {
      out_ << "'" << _category_ << "'";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "MIME type : ";
    if (has_mime()) {
      out_ << "'" << _mime_ << "'";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Paths : [" << _paths_.size() << "]" << std::endl;
    for (std::size_t i = 0; i < _paths_.size(); i++) {
      out_ << indent_ << i_tree_dumpable::inherit_skip_tag(inherit_);
      if (i + 1 < _paths_.size()) {
        out_ << i_tree_dumpable::tag;
      } else {
        out_ << i_tree_dumpable::last_tag;
      }
      out_ << "Path[" << i << "] : " << _paths_[i] << std::endl;
    }

    return;
  }

} // namespace datatools

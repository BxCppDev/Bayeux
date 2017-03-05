// datatools/urn.cc
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
#include <datatools/urn.h>

// Third party:
#include <boost/algorithm/string.hpp>

// This project:
#include <datatools/exception.h>
#include <datatools/utils.h>

namespace datatools {

  DATATOOLS_SERIALIZATION_IMPLEMENTATION_ADVANCED(urn, "datatools::urn")

  // static
  const std::string & urn::urn_scheme()
  {
    static const std::string _s("urn");
    return _s;
  }

  // static
  char urn::urn_separator()
  {
    return ':';
  }

  urn::urn()
  {
  }

  urn::urn(const std::vector<std::string> & segments_)
  {
    set_segments(segments_);
  }

  urn::~urn()
  {
  }

  bool urn::is_valid() const
  {
    return _segments_.size() > 0;
  }

  std::size_t urn::get_depth() const
  {
    return _segments_.size();
  }

  void urn::set_segments(const std::vector<std::string> & segments_)
  {
    for (const std::string & s : segments_) {
      append_segment(s);
    }
  }

  void urn::append_segment(const std::string & segment_)
  {
    static const uint32_t nv_flags = NV_NO_COLON | NV_LEADING_DIGIT;
    DT_THROW_IF(!name_validation(segment_, nv_flags),
                std::logic_error,
                "Invalid URN path segment '" << segment_ << "'!");
    _segments_.push_back(segment_);
  }

  void urn::reset()
  {
    _segments_.clear();
  }

  const std::vector<std::string> & urn::get_segments() const
  {
    return _segments_;
  }

  // friend
  std::ostream & operator<<(std::ostream & out_, const urn & u_)
  {
    if (!u_.is_valid()) {
      out_ << '!';
    } else {
      out_ << urn::urn_scheme();
      for (const std::string & s : u_._segments_) {
        out_ << urn::urn_separator() << s;
      }
    }
    return out_;
  }

  void urn::to_string(std::string & s_) const
  {
    std::ostringstream out;
    out << *this;
    s_ = out.str();
  }

  std::string urn::to_string() const
  {
    std::string s;
    to_string(s);
    return s;
  }

  bool urn::from_string(const std::string & repr_)
  {
    std::vector<std::string> backup_segs = _segments_;
    if (repr_ == "!") {
      reset();
    } else {
      std::vector<std::string> segs;
      boost::split(segs, repr_, boost::is_any_of(":"));
      if (segs.size() < 2) {
        return false;
      }
      if (segs.front() != urn::urn_scheme()) {
        return false;
      }
      segs.erase(segs.begin());
      try {
        set_segments(segs);
      } catch (std::exception &) {
        _segments_ = backup_segs;
        return false;
      }
    }
    return true;
  }

} // namespace datatools

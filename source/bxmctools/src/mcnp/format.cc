//! \file  mctools/mcnp/format.cc
//! \brief MNCP input file formating
//

// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Bayeux.
//
// Bayeux is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux. If not, see <http://www.gnu.org/licenses/>.

// Ourselves:
#include <mctools/mcnp/format.h>

// Standard library:
#include <vector>

namespace mctools {

  namespace mcnp {

    // static
    const uint32_t format::MAX_LINE_WIDTH;
    const uint32_t format::HEAD_LINE_WIDTH;

    // static
    void format::add_comment_line(std::ostream & out_, const std::string & line_)
    {
      out_ << "c " << line_;
      if (line_.empty()) {
        out_ << std::endl;
      } else {
        if (line_[line_.length() - 1] != '\n') {
          out_ << std::endl;
        }
      }
      return;
    }

    // static
    void format::add_comment_endl(std::ostream & out_, const std::string & comment_)
    {
      out_ << " $ " << comment_;
      if (comment_.empty()) {
        out_ << std::endl;
      } else {
        if (comment_[comment_.length() - 1] != '\n') {
          out_ << std::endl;
        }
      }
      return;
    }

    // static
    void format::format_line(std::ostream & out_, const std::string & line_)
    {
      std::vector<std::string> lines;
      static const std::string _head("     ");
      std::string l = line_;
      uint32_t sz = MAX_LINE_WIDTH;
      while(l.length()) {
        std::string h = l.substr(0, sz);
        // std::cerr << "DEVEL: h=[" << h << "]" << std::endl;
        if (h.length() == sz) {
          std::string t = l.substr(sz);
          // std::cerr << "DEVEL: t=[" << t << "]" << std::endl;
          l = t;
        } else {
          l.clear();
        }
        // std::cerr << "DEVEL: l=[" << l << "]" << std::endl;
        sz = MAX_LINE_WIDTH - HEAD_LINE_WIDTH;
        if (!h.empty()) {
          lines.push_back(h);
        }
      }

      if (lines.size()) {
        out_ << lines[0] << std::endl;
        if (lines.size() > 1) {
          for (int iline = 1; iline < (int) lines.size(); iline++) {
            out_ << _head << lines[iline] << std::endl;
          }
        }
      }
      return;
    }

  } // namespace mcnp

} // namespace mctools

// datatools/configuration/occurrence_factory.cc
/*
 * Copyright (C) 2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

// Ourselves:
#include <datatools/configuration/occurrence_factory.h>

// Standard library:
#include <string>
#include <vector>

// Third party:
// - Boost:
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

// This project (Bayeux/datatools):
#include <datatools/exception.h>
#include <datatools/configuration/single_occurrence.h>
#include <datatools/configuration/array_occurrence.h>

namespace datatools {

  namespace configuration {

    i_occurrence * occurrence_factory::create(const std::string & def_)
    {
      i_occurrence * occ = 0;
      single_occurrence * socc = 0;
      array_occurrence  * aocc = 0;
      std::string type;
      unsigned int array_size = 0;
      int array_start_id = -1;

      std::vector<std::string> tokens;
      boost::split(tokens, def_, boost::is_any_of("\t "));
      for (size_t i = 0; i < tokens.size(); i++) {
        const std::string & token = tokens[i];
        if (token.empty()) {
          continue;
        }

        if (type.empty()) {
          type = token;
          if (type == "single") {
            socc = new single_occurrence;
            occ = socc;
          } else if (type == "array") {
            aocc = new array_occurrence;
            occ = aocc;
          } else {
            DT_THROW(std::logic_error, "Invalid occurrence type '" << type << "'!");
          }
          continue;
        }

        try {

          if (socc) {
            // Nothing.
          } else if (aocc) {
            if (! array_size) {
              if (boost::starts_with(token, "size=")) {
                array_size = boost::lexical_cast<unsigned int>(token.substr(5));
              }
            }
            if (array_start_id < 0) {
              if (boost::starts_with(token, "start_id=")) {
                array_start_id = boost::lexical_cast<int>(token.substr(9));
              }
            }
          }
        } catch (boost::bad_lexical_cast) {
          DT_THROW(std::logic_error, "Bad parameter in occurrence definition '" << def_ << "'");
        }

      } // for

      if (aocc) {
        DT_THROW_IF(! array_size, std::logic_error, "Missing array occurrence size !");
        aocc->set_size(array_size);
        if (array_start_id < 0) {
          aocc->set_start_id(0);
        } else {
          aocc->set_start_id(array_start_id);
        }
      }

      return occ;
    }

  }  // end of namespace configuration

}  // end of namespace datatools

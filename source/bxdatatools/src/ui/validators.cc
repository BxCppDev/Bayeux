// datatools/ui/validators.cc
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
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
#include <datatools/ui/validators.h>

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

// This project:
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/units.h>
#include <datatools/logger.h>

namespace datatools {

  namespace ui {

    void validate(boost::any & v,
                  const std::vector<std::string>& values,
                  datatools::ui::wrapped_boolean *, int)
    {
      using namespace boost::program_options;
      // Make sure no previous assignment to 'v' was made.
      validators::check_first_occurrence(v);

      const std::string & value_token = validators::get_single_string(values);
      uint32_t read_flags = 0;
      bool value = false;
      if (!datatools::io::read_boolean(value_token, value, read_flags)) {
        throw validation_error(validation_error::invalid_option_value);
      }
      datatools::ui::wrapped_boolean wb;
      wb.value = value;
      v = boost::any(wb);
      return;
    }

    void validate(boost::any & v,
                  const std::vector<std::string> & values,
                  datatools::ui::wrapped_integer *, int)
    {
      using namespace boost::program_options;
      // Make sure no previous assignment to 'v' was made.
      validators::check_first_occurrence(v);

      const std::string & value_token = validators::get_single_string(values);
      uint32_t read_flags = 0;
      int value = 0;
      if (!datatools::io::read_integer(value_token, value, read_flags)) {
        throw validation_error(validation_error::invalid_option_value);
      }
      datatools::ui::wrapped_integer wi;
      wi.value = value;
      v = boost::any(wi);
      return;
    }

    void validate(boost::any & v,
                  const std::vector<std::string> & values,
                  datatools::ui::wrapped_real_without_unit *, int)
    {
      using namespace boost::program_options;
      // Make sure no previous assignment to 'v' was made.
      validators::check_first_occurrence(v);

      const std::string & value_token = validators::get_single_string(values);
      uint32_t read_flags = 0;
      double value = datatools::invalid_real();
      bool normal = false;
      if (!datatools::io::read_real_number(value_token, value, normal, read_flags)) {
        throw validation_error(validation_error::invalid_option_value);
      }
      datatools::ui::wrapped_real_without_unit rwou;
      rwou.value = value;
      rwou.normal = normal;
      v = boost::any(rwou);
      return;
    }

    void validate(boost::any & v,
                  const std::vector<std::string>& values,
                  datatools::ui::wrapped_real_with_unit *, int)
    {
      datatools::logger::priority logging = datatools::logger::PRIO_DEBUG;
      using namespace boost::program_options;
      // for (auto token : values) {
      //   DT_LOG_DEBUG(logging, "Token is '" << token << "'");
      // }
      // Make sure no previous assignment to 'v' was made.
      validators::check_first_occurrence(v);
      // Extract the token to be parsed:
      const std::string & unique_token = validators::get_single_string(values);
      // Split the value and unit fields:
      typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
      boost::char_separator<char> sep(" ~_");
      tokenizer tokens(unique_token, sep);
      std::string value_token;
      std::string unit_token;
      int count = 0;
      for (tokenizer::iterator tok_iter = tokens.begin();
           tok_iter != tokens.end(); ++tok_iter, ++count) {
        std::string tok = *tok_iter;
        if (value_token.empty()) {
          value_token = *tok_iter;
        } else if (unit_token.empty()) {
          unit_token = *tok_iter;
        } else {
          throw validation_error(validation_error::invalid_option_value);
        }
      }

      // parsing...
      if (count == 1) {
        // We expect only +/-inf or NaN:
        uint32_t read_flags = 0;
        double value = datatools::invalid_real();
        bool normal = false;
        if (!datatools::io::read_real_number(value_token, value, normal, read_flags)) {
          throw validation_error(validation_error::invalid_option_value);
        }
        // Normal should be false here:
        if (normal) {
          throw validation_error(validation_error::invalid_option_value);
        }
        datatools::ui::wrapped_real_with_unit rwu;
        rwu.value = value;
        rwu.normal = false;
        rwu.unit_symbol.clear();
        rwu.unit_label.clear();
        v = boost::any(rwu);
      } else if (count == 2) {
        std::string token = value_token + " " + unit_token;
        double value = datatools::invalid_real();
        std::string unit_symbol;
        std::string unit_label;
        uint32_t read_flags = 0;
        if (!datatools::units::parse_value_with_unit(token, value, unit_symbol, unit_label)) {
          throw validation_error(validation_error::invalid_option_value);
        }
        datatools::ui::wrapped_real_with_unit rwu;
        rwu.value = value;
        rwu.normal = true;
        rwu.unit_symbol = unit_symbol;
        rwu.unit_label = unit_label;
        v = boost::any(rwu);
      } else {
        throw validation_error(validation_error::invalid_option_value);
      }
      return;
    }

  } // namespace ui

} // namespace datatools

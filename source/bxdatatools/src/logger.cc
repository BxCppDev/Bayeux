// logger.cc - Implementation of datatools logger
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick
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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools.  If not, see <http://www.gnu.org/licenses/>.

// Ourselves
#include "datatools/logger.h"

// Standard Library

// Third Party
// - Boost
#include "boost/assign.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/bimap.hpp"
#include "boost/bimap/set_of.hpp"
#include "boost/bimap/multiset_of.hpp"

// This Project
#include "datatools/exception.h"
#include "datatools/properties.h"
#include <datatools/object_configuration_description.h>

namespace {
//! Hide awkward boost bimap declaration in a typedef
struct PriorityLookup {
  typedef boost::bimap<
      boost::bimaps::set_of<std::string>,
      boost::bimaps::multiset_of<datatools::logger::priority>
      > LookupTable;
};

//! Construct the lookup table.
PriorityLookup::LookupTable ConstructLookupTable() {
  PriorityLookup::LookupTable a;
  boost::assign::insert(a)
      ("PRIO_FATAL", datatools::logger::PRIO_FATAL)
      ("FATAL", datatools::logger::PRIO_FATAL)
      ("fatal", datatools::logger::PRIO_FATAL)
      ("PRIO_CRITICAL", datatools::logger::PRIO_CRITICAL)
      ("CRITICAL", datatools::logger::PRIO_CRITICAL)
      ("critical", datatools::logger::PRIO_CRITICAL)
      ("PRIO_ERROR", datatools::logger::PRIO_ERROR)
      ("ERROR", datatools::logger::PRIO_ERROR)
      ("error", datatools::logger::PRIO_ERROR)
      ("PRIO_WARNING", datatools::logger::PRIO_WARNING)
      ("WARNING", datatools::logger::PRIO_WARNING)
      ("warning", datatools::logger::PRIO_WARNING)
      ("PRIO_NOTICE", datatools::logger::PRIO_NOTICE)
      ("NOTICE", datatools::logger::PRIO_NOTICE)
      ("notice", datatools::logger::PRIO_NOTICE)
      ("PRIO_INFORMATION", datatools::logger::PRIO_INFORMATION)
      ("INFORMATION", datatools::logger::PRIO_INFORMATION)
      ("information", datatools::logger::PRIO_INFORMATION)
      ("PRIO_DEBUG", datatools::logger::PRIO_DEBUG)
      ("DEBUG", datatools::logger::PRIO_DEBUG)
      ("debug", datatools::logger::PRIO_DEBUG)
      ("PRIO_TRACE", datatools::logger::PRIO_TRACE)
      ("TRACE", datatools::logger::PRIO_TRACE)
      ("trace", datatools::logger::PRIO_TRACE);

  return a;
}

//! Return priority label stripped of "PRIO_" prefix and lowercased
std::string GetCanonicalLabel(const std::string& raw) {
  return boost::to_lower_copy(boost::ireplace_first_copy(raw, "PRIO_", ""));
}

} // namespace

namespace datatools {
logger::priority logger::get_priority(const std::string& name) {
  static PriorityLookup::LookupTable a;
  if (a.empty()) a = ConstructLookupTable();

  PriorityLookup::LookupTable::left_const_iterator p = a.left.find(name);
  return p != a.left.end() ? p->second : PRIO_UNDEFINED;
}

std::string logger::get_priority_label(logger::priority p)
{
  static PriorityLookup::LookupTable a;
  if (a.empty()) a = ConstructLookupTable();

  PriorityLookup::LookupTable::right_const_iterator n = a.right.find(p);
  return n != a.right.end() ? GetCanonicalLabel(n->second) : "";
}

logger::priority logger::extract_logging_configuration(
  const datatools::properties & config,
  logger::priority default_prio,
  bool throw_on_error)
{
  datatools::logger::priority p = default_prio;
  if (config.has_key("logging.priority")) {
    std::string ps = config.fetch_string("logging.priority");
    p = datatools::logger::get_priority(ps);
    DT_THROW_IF(p == datatools::logger::PRIO_UNDEFINED && throw_on_error,
                std::logic_error,
                "Invalid logging priority label '" << ps << "' !");
  } else if (config.has_flag("debug") || config.has_flag("logging.debug")) {
    p = datatools::logger::PRIO_DEBUG;
  } else if (config.has_flag("verbose") || config.has_flag("logging.verbose")) {
    p = datatools::logger::PRIO_NOTICE;
  }
  return p;
}

void logger::declare_ocd_logging_configuration(datatools::object_configuration_description & ocd_,
                                               const std::string & default_value_,
                                               const std::string & prefix_,
                                               const std::string & from_)
{
  {
    std::ostringstream desc;
    desc <<  "Allowed values are:                      \n"
      "                                                \n"
      "  * ``\"trace\"`` : Heavy development messages  \n"
      "  * ``\"debug\"`` : Debug messages              \n"
      "  * ``\"information\"`` :                       \n"
      "  * ``\"notice\"`` :                            \n"
      "  * ``\"warning\"`` :                           \n"
      "  * ``\"error\"`` :                             \n"
      "  * ``\"critical\"`` :                          \n"
      "  * ``\"fatal\"`` : Only fatal error messages   \n"
      "                                                \n";
    std::ostringstream example;
    example <<
      "Set the logging priority to allow the printing  \n"
      "of informational messages: ::                   \n"
      "                                                \n";
    example << "  " << prefix_;
    example << "logging.priority : string = \"notice\" \n"
      "                                                \n";

    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern(prefix_ + "logging.priority")
      .set_terse_description("Set the logging priority threshold")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_default_value_string(default_value_.empty() ? "fatal": default_value_)
      .set_long_description(desc.str())
      .add_example(example.str())
      ;
    if (! from_.empty()) {
      cpd.set_from(from_);
    }
  }

  return;
}

} // namespace datatools

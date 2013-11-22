// component_utils.cc
//
// Copyright (c) 2013 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Bayeux/electronics.
//
// Bayeux/electronics is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux/electronics is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux/electronics.  If not, see <http://www.gnu.org/licenses/>.

// Ourselves
#include "electronics/component_types.h"

// Third Party
// - Boost
#include "boost/assign.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/bimap.hpp"
#include "boost/bimap/set_of.hpp"
#include "boost/bimap/multiset_of.hpp"

namespace {

  //! Hide Boost bimap declaration in a typedef
  struct component_type_lookup {
    typedef boost::bimap<
      boost::bimaps::set_of<std::string>,
      boost::bimaps::multiset_of<electronics::component_type>
      > lookup_table;
  };

  //! Construct the lookup table.
  component_type_lookup::lookup_table construct_lookup_table() {
    component_type_lookup::lookup_table lt;
    boost::assign::insert(lt)
      ("undefined",              electronics::TYPE_UNDEFINED)
      ("generic",                electronics::TYPE_GENERIC)
      ("setup",                  electronics::TYPE_SETUP)
      ("device",                 electronics::TYPE_DEVICE)
      ("rack",                   electronics::TYPE_RACK)
      ("crate",                  electronics::TYPE_CRATE)
      ("module",                 electronics::TYPE_MODULE)
      ("module_board",           electronics::TYPE_MODULE_BOARD)
      ("module_mezzanine_board", electronics::TYPE_MODULE_MEZZANINE_BOARD)
      ("chip",                   electronics::TYPE_CHIP)
      ("chip_pld",               electronics::TYPE_CHIP_PLD)
      ("chip_asic",              electronics::TYPE_CHIP_ASIC)
      ("chip_eprom",             electronics::TYPE_CHIP_EPROM)
      ("connector",              electronics::TYPE_CONNECTOR)
      ("cable",                  electronics::TYPE_CABLE)
      ;
    return lt;
  }

  //! Return priority label stripped of "TYPE_" prefix and lowercased
  std::string get_canonical_label(const std::string& raw) {
    return raw;
    // return boost::to_lower_copy(boost::ireplace_first_copy(raw, "PRIO_", ""));
  }

} // namespace

namespace electronics {

  component_type get_component_type(const std::string& name) {
    static component_type_lookup::lookup_table a;
    if (a.empty()) a = construct_lookup_table();

    component_type_lookup::lookup_table::left_const_iterator p = a.left.find(name);
    return p != a.left.end() ? p->second : TYPE_UNDEFINED;
  }

  std::string get_component_type_label(component_type p)
  {
    static component_type_lookup::lookup_table a;
    if (a.empty()) a = construct_lookup_table();

    component_type_lookup::lookup_table::right_const_iterator n = a.right.find(p);
    return n != a.right.end() ? get_canonical_label(n->second) : "";
  }

} // end of namespace electronics

// end of component_types.cc

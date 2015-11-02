//! \file  datatools/units/units-reflect.h
//! \brief Units introspection support
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef DATATOOLS_UNITS_REFLECT_H
#define DATATOOLS_UNITS_REFLECT_H

// Ourselves:
#include <datatools/units.h>

// Registration:
DR_TYPE_EXPORT(::datatools::units::power_of_ten, "datatools::units::power_of_ten");

namespace datatools {
  namespace detail {
    namespace reflection {

      DR_TYPE_IMPLEMENT_REFLECTION_HEAD(::datatools::units::power_of_ten, /*tag_*/)
      {
        DR_ENUM_DECLARE(::datatools::units::power_of_ten)
          .DR_ENUM_PAIR("yocto", ::datatools::units::YOCTO)
          .DR_ENUM_PAIR("zepto", ::datatools::units::ZEPTO)
          .DR_ENUM_PAIR("atto",  ::datatools::units::ATTO)
          .DR_ENUM_PAIR("femto", ::datatools::units::FEMTO)
          .DR_ENUM_PAIR("pico",  ::datatools::units::PICO)
          .DR_ENUM_PAIR("nano",  ::datatools::units::NANO)
          .DR_ENUM_PAIR("micro", ::datatools::units::MICRO)
          .DR_ENUM_PAIR("milli", ::datatools::units::MILLI)
          .DR_ENUM_PAIR("centi", ::datatools::units::CENTI)
          .DR_ENUM_PAIR("deci",  ::datatools::units::DECI)
          .DR_ENUM_PAIR("",      ::datatools::units::UNIT_POWER_OF_TEN)
          .DR_ENUM_PAIR("deca",  ::datatools::units::DECA)
          .DR_ENUM_PAIR("hecto", ::datatools::units::HECTO)
          .DR_ENUM_PAIR("kilo",  ::datatools::units::KILO)
          .DR_ENUM_PAIR("mega",  ::datatools::units::MEGA)
          .DR_ENUM_PAIR("giga",  ::datatools::units::GIGA)
	  .DR_ENUM_PAIR("tera",  ::datatools::units::TERA)
	  .DR_ENUM_PAIR("peta",  ::datatools::units::PETA)
	  .DR_ENUM_PAIR("exa",   ::datatools::units::EXA)
          .DR_ENUM_PAIR("zetta", ::datatools::units::ZETTA)
	  .DR_ENUM_PAIR("yotta", ::datatools::units::YOTTA)
	  ;
        return;
      }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // DATATOOLS_UNITS_REFLECT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

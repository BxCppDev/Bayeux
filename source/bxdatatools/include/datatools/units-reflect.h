//! \file  datatools/units-reflect.h
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

// This project:
#include <datatools/detail/reflection_macros.h>
#include <datatools/detail/reflection_export.h>
#include <datatools/compiler_macros.h>

// Registration:
DR_TYPE_EXPORT(::datatools::units::power_of_ten, "datatools::units::power_of_ten")

namespace datatools {
  namespace detail {
    namespace reflection {

      DR_TYPE_IMPLEMENT_REFLECTION_HEAD(::datatools::units::power_of_ten, DT_UNUSED(tag_))
      {
        DR_ENUM_DECLARE(::datatools::units::power_of_ten)
          .value("yocto", ::datatools::units::YOCTO)
          .value("zepto", ::datatools::units::ZEPTO)
          .value("atto",  ::datatools::units::ATTO)
          .value("femto", ::datatools::units::FEMTO)
          .value("pico",  ::datatools::units::PICO)
          .value("nano",  ::datatools::units::NANO)
          .value("micro", ::datatools::units::MICRO)
          .value("milli", ::datatools::units::MILLI)
          .value("centi", ::datatools::units::CENTI)
          .value("deci",  ::datatools::units::DECI)
          .value("",      ::datatools::units::UNIT_POWER_OF_TEN)
          .value("deca",  ::datatools::units::DECA)
          .value("hecto", ::datatools::units::HECTO)
          .value("kilo",  ::datatools::units::KILO)
          .value("mega",  ::datatools::units::MEGA)
          .value("giga",  ::datatools::units::GIGA)
          .value("tera",  ::datatools::units::TERA)
          .value("peta",  ::datatools::units::PETA)
          .value("exa",   ::datatools::units::EXA)
          .value("zetta", ::datatools::units::ZETTA)
          .value("yotta", ::datatools::units::YOTTA)
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

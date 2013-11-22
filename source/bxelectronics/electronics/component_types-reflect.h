// -*- mode: c++; -*-
#ifndef ELECTRONICS_COMPONENT_TYPES_REFLECT_H
#define ELECTRONICS_COMPONENT_TYPES_REFLECT_H

// Ourselves
#include "electronics/component_types.h"

// Standard Library
#include <iostream>

// Export the introspection code for the 'electronics::component_type' enum:
DR_TYPE_EXPORT(::electronics::component_type, "electronics::component_type")

namespace datatools {
  namespace detail {
    namespace reflection {

      DR_TYPE_IMPLEMENT_REFLECTION_HEAD(::electronics::component_type, tag_)
        {
          DR_ENUM_DECLARE(::electronics::component_type)
            .DR_ENUM_PAIR("undefined",    ::electronics::TYPE_UNDEFINED)
            .DR_ENUM_PAIR("generic",      ::electronics::TYPE_GENERIC)
            .DR_ENUM_PAIR("setup",        ::electronics::TYPE_SETUP)
            .DR_ENUM_PAIR("rack",         ::electronics::TYPE_RACK)
            .DR_ENUM_PAIR("crate",        ::electronics::TYPE_CRATE)
            .DR_ENUM_PAIR("module",       ::electronics::TYPE_MODULE)
            .DR_ENUM_PAIR("module_board", ::electronics::TYPE_MODULE_BOARD)
            .DR_ENUM_PAIR("module_mezzanine_board",
                          ::electronics::TYPE_MODULE_MEZZANINE_BOARD)
            .DR_ENUM_PAIR("chip",         ::electronics::TYPE_CHIP)
            .DR_ENUM_PAIR("chip_pld",     ::electronics::TYPE_CHIP_PLD)
            .DR_ENUM_PAIR("chip_asic",    ::electronics::TYPE_CHIP_ASIC)
            .DR_ENUM_PAIR("chip_eprom",   ::electronics::TYPE_CHIP_EPROM)
            .DR_ENUM_PAIR("connector",    ::electronics::TYPE_CONNECTOR)
            .DR_ENUM_PAIR("cable",        ::electronics::TYPE_CABLE)
            ;
          return;
        }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // ELECTRONICS_COMPONENT_TYPES_REFLECT_H

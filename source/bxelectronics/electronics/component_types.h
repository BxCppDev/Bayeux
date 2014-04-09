// -*- mode: c++; -*-
#ifndef ELECTRONICS_COMPONENT_TYPES_H
#define ELECTRONICS_COMPONENT_TYPES_H

// Standard library
#include <string>

// Third party
// - Bayeux/datatools
#include <datatools/reflection_macros.h>

namespace electronics {

  enum component_type {
    TYPE_UNDEFINED              =   -1, /// Undefined component type
    TYPE_GENERIC                =    0, /// Generic component
    TYPE_SETUP                  =   10, /// Generic setup
    TYPE_DEVICE                 = 1000, /// Generic device
    TYPE_RACK                   = 1100, /// Generic rack
    TYPE_CRATE                  = 1200, /// Generic crate
    TYPE_MODULE                 = 1300, /// Generic module
    TYPE_MODULE_BOARD           = 1310, /// Generic board
    TYPE_MODULE_MEZZANINE_BOARD = 1320, /// Generic mezzanine board
    TYPE_CHIP                   = 1400, /// Generic chip
    TYPE_CHIP_PLD               = 1410, /// Generic Programmable Logic Device (Example: FPGA)
    TYPE_CHIP_ASIC              = 1420, /// Generic Application-Specific Integrated Circuit (ASIC)
    TYPE_CHIP_EPROM             = 1430, /// Generic e-PROM device
    TYPE_IO                     = 2000, /// Generic I/O
    TYPE_IO_CLOCK               = 2100, /// Generic clock I/O
    TYPE_CONNECTOR              = 3000, /// Generic connector
    TYPE_CABLE                  = 4000, /// Generic cable
    TYPE_REGISTER               = 5000, /// Generic register
  };

  component_type get_component_type(const std::string & label_);

  std::string get_component_type_label(component_type);

} // end of namespace electronics

// Activate reflection layer for the 'electronics::component_type' enum:
DR_TYPE_INIT(::electronics::component_type);

#endif // ELECTRONICS_COMPONENT_TYPES_H

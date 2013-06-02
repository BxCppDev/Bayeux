// -*- mode: c++; -*-
//! \file geomtools/utils-reflect.h
#ifndef GEOMTOOLS_UTILS_REFLECT_H_
#define GEOMTOOLS_UTILS_REFLECT_H_

// Standard Library

// CAMP :
#include <camp/enum.hpp>

// Datatools
#include <datatools/reflection_macros.h>

// Geomtools
#include <geomtools/utils.h>

camp::Enum::declare<geomtools::orientation_type>("geomtools::orientation_type")
.value("vertical",    geomtools::VERTICAL)
.value("horizontal",  geomtools::HORIZONTAL);

camp::Enum::declare<geomtools::direction_type>("geomtools::direction_type")
.value("back",    geomtools::BACK)
.value("front",    geomtools::FRONT)
.value("left",    geomtools::LEFT)
.value("right",    geomtools::RIGHT)
.value("bottom",    geomtools::BOTTOM)
.value("top",    geomtools::TOP);

camp::Enum::declare<geomtools::axis_type>("geomtools::axis_type")
.value("invalid", geomtools::AXIS_INVALID)
.value("x", geomtools::AXIS_X)
.value("y", geomtools::AXIS_Y)
.value("z", geomtools::AXIS_Z);

camp::Enum::declare<geomtools::rotation_axis_type>("geomtools::rotation_axis_type")
.value("invalid", geomtools::ROTATION_AXIS_INVALID)
.value("x", geomtools::ROTATION_AXIS_X)
.value("y", geomtools::ROTATION_AXIS_Y)
.value("z", geomtools::ROTATION_AXIS_Z);

#endif // GEOMTOOLS_UTILS_REFLECT_H_

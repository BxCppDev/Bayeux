/// \file geomtools/clhep.h

#ifndef GEOMTOOLS_CLHEP_H
#define GEOMTOOLS_CLHEP_H 1

// Third party:

// - CLHEP:
#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <CLHEP/Vector/TwoVector.h>
#include <CLHEP/Vector/ThreeVector.h>

// In C++11 with CLHEP 2.1.3.1, can no longer use register
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif
#include <CLHEP/Vector/Rotation.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include <CLHEP/Random/RandFlat.h>

// This project:
#include <geomtools/geomtools_config.h>

namespace geomtools {

  /// Alias for the CLHEP 2D-vector class
  typedef CLHEP::Hep2Vector  vector_2d;

  /// Alias for the CLHEP 3D-vector class
  typedef CLHEP::Hep3Vector  vector_3d;

  /// Alias for the CLHEP 3D-rotation class
  typedef CLHEP::HepRotation rotation_3d;

  /// Alias for the 3D-rotation class
  typedef rotation_3d        rotation;

}

// Activate reflection layer for some classes :
#include <datatools/reflection_interface.h>
DR_CLASS_INIT(::geomtools::vector_2d)
DR_CLASS_INIT(::geomtools::vector_3d)
DR_CLASS_INIT(::geomtools::rotation_3d)

namespace geomtools {

  /// \brief Basic uniform deviates generator from CLHEP used for special geometry algorithms
  class random_tools
  {
  public:

    static double random_flat()
    {
      return CLHEP::RandFlat::shoot(0.0, 1.0);
    }

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_CLHEP_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

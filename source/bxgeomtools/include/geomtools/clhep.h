// -*- mode: c++; -*- 
/* clhep.h
 * 
 */

#ifndef __geomtools__clhep_h
#define __geomtools__clhep_h 1

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

#include <CLHEP/Vector/TwoVector.h>
#include <CLHEP/Vector/ThreeVector.h>
#include <CLHEP/Vector/Rotation.h>

namespace geomtools {

  typedef CLHEP::Hep2Vector  vector_2d;
  typedef CLHEP::Hep3Vector  vector_3d;
  typedef CLHEP::HepRotation rotation;

} // end of namespace geomtools

#endif // __geomtools__clhep_h

// end of clhep.h

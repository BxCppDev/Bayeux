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

#include <CLHEP/Random/RandFlat.h>
//#include <CLHEP/Random/RandomEngine.h>

namespace geomtools {

  typedef CLHEP::Hep2Vector  vector_2d;
  typedef CLHEP::Hep3Vector  vector_3d;
  typedef CLHEP::HepRotation rotation;

  /* basic uniform deviates generator from CLHEP
   * used for special geometry algorithms
   */
  class random_tools
  {
  public:

    static double random_flat ()
    {
      return CLHEP::RandFlat::shoot (0.0, 1.0);
    }

  };



} // end of namespace geomtools

#endif // __geomtools__clhep_h

// end of clhep.h

// -*- mode: c++; -*- 
/* genbb_commons.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-28
 * Last modified: 
 * 
 * Description: 
 *   GENBB generator
 */

#ifndef __genbb_help__genbb_commons_h
#define __genbb_help__genbb_commons_h 1

#include <iostream>
#include <string>

#include <boost/cstdint.hpp>

struct ENRANGE_t
{
  //	common/enrange/ebb1,ebb2,toallevents,levelE,chdspin
public:

  float ebb1;
  float ebb2;
  float toallevents;
  int   levelE;

  void reset()
  {
    ebb1 = 0;
    ebb2 = 4.3; // MeV
    toallevents = 1.0;
    levelE = 0;
  }

};

struct GENEVENT_t 
{ 
public:

  static const size_t GENBB_MAX_NB_PARTICLES = 100;
  
public:

  float     tevst;  //!< Time of event's start (sec);
  int32_t   npfull; //!< Full number of emitted particles in event;

  /** array of GEANT numbers for particle identification
   *  1 for gamma, 
   *  2 for e+, 
   *  3 for e-,
   *  5 for muon+, 
   *  6 for muon-, 
   *  47 for alpha;
   */
  int32_t   npgeant[GENBB_MAX_NB_PARTICLES];

  /** array of x,y,z components of particle momentum (MeV/c)
   */
  float     pmoment[GENBB_MAX_NB_PARTICLES][3];
  /** array of time shift from previous time to calculate when 
   *  particle was emitted (sec);
   */
  float     ptime[GENBB_MAX_NB_PARTICLES];

  GENEVENT_t ()
  {
    reset ();
  }

  void reset ()
  {
    // reset Fortran common block:
    this->npfull = 0;
    this->tevst = 0.0F;
    for (int i = 0; i < GENBB_MAX_NB_PARTICLES; i++)
      {
	this->npgeant[i] = 0;
	for (int j = 0; j < 3; j++)
	  {
	    this->pmoment[i][j] = 0.0F;
	  }
	this->ptime[i] = 0.0F;
      }

  }

};

#endif // __genbb_help__genbb_commons_h

// end of genbb_commons.h

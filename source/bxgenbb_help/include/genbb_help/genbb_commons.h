// -*- mode: c++; -*- 
/* genbb_commons.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-28
 * Last modified: 2012-06-21
 * 
 * License: 
 * Copyright 2007-2012 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Description: 
 *
 *   Interface classes with Fortran GENBB generator
 *
 */

#ifndef __genbb_help__genbb_commons_h
#define __genbb_help__genbb_commons_h 1

#include <iostream>
#include <string>

#include <boost/cstdint.hpp>

struct ENRANGE_t
{
  //    common/enrange/ebb1,ebb2,toallevents,levelE,chdspin
public:

  float   ebb1; // MeV
  float   ebb2; // MeV
  float   toallevents;
  int32_t levelE;

  ENRANGE_t ();

  void reset ();

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

  GENEVENT_t ();

  void reset ();

};


struct GENBBPAR_t 
{ 
public:
  int32_t nevents;
  int32_t ievstart;
  int32_t irndmst;
  int32_t iwrfile;
  char    chfile[256];

  GENBBPAR_t ();

  void reset ();
};

#endif // __genbb_help__genbb_commons_h

// end of genbb_commons.h

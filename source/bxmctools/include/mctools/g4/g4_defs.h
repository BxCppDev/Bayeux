/// \file mctools/g4/g4_defs.h
/* Author(s) :  François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-04-22
 * Last modified: 2022-04-22
 *
 * License: GPL 3.0
 *
 * Description:
 *
 *   GEANT4 definitions
 *
 */

#ifndef MCTOOLS_G4_DEFS_H
#define MCTOOLS_G4_DEFS_H 1

#include <G4Version.hh>

#if G4VERSION_NUMBER < 1000

#pragma message ( "Geant4 version 9.X" )

#else // G4VERSION_NUMBER >= 1000

// The G4MULTITHREADED variable is possibly defined in the 'G4GlobalConfig.hh' header
#include <G4GlobalConfig.hh>
#if G4VERSION_NUMBER < 1100

#pragma message ( "Geant4 version >= 10.0" )

#else // G4VERSION_NUMBER >= 1100

#pragma message ( "Geant4 version >= 11.0" )

#endif // G4VERSION_NUMBER >= 1100

#ifdef G4MULTITHREADED
#pragma message ( "Detected Geant4 MT mode" )
#pragma message ( "Disabling Geant4 MT mode (undefine G4MULTITHREADED)" )
#undef G4MULTITHREADED
#endif

#endif // G4VERSION_NUMBER >= 1000

#ifdef G4MULTITHREADED
#define BXGEANT4MT
#endif

#endif // MCTOOLS_G4_DEFS_H 

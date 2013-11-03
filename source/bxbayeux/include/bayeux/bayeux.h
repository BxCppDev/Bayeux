//! \file    bayeux/bayeux.h
//! \brief   Provide system init for Bayeux internals
//! \details
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick
// Copyright (c) 2013 by Francois Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2013 by Université de Caen
//
// This file is part of Bayeux.
//
// Bayeux is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux.  If not, see <http://www.gnu.org/licenses/>.

/*!
 * \mainpage Bayeux C++ library documentation
 *
 * \section overview_section Overview
 *
 * Bayeux provides a collection of C++ classes and functions designed
 * for the simulation, recording and analysis of data for experimental
 * particle and nuclear physics projects. This functionality is split
 * into several specialized submodules:
 *
 * \li datatools :  Serializable data structures based on Boost.
 *
 * \li cuts :       Generic tools for making arbitrary data selections.
 *
 * \li materials :  Description of isotopes, elements and materials plus tools
 * 	        for input to simulation applications (i.e. GDML/Geant4).
 *
 * \li mygsl :      C++ wrapper and extensions to the GNU Scientific Library.
 *
 * \li geomtools :  Generic tools for working with experiment geometries and
 *              provide input to simulation tools (i.e. GDML/Geant4).
 *
 * \li emfield :    Electromagnetic field modelling and management.
 *
 * \li brio :       A Boost over ROOT I/O serialization system (extends datatools).
 *
 * \li dpp :        A basic data processing pipeline API.
 *
 * \li genbb_help : C++ wrapper and extensions to the Decay0/GENBB program
 *              by Vladimir Tretyak, for input to simulation applications.
 *
 * \li genvtx :     Vertex random generator tools for input to
 *              simulation applications.
 *
 * \li mctools :    Utilities for particle and nuclear physics simulation with
 *              a Geant4 interface.
 *
 *
 * \section authors Authors
 *
 *  \li François Mauger (LPC Caen, CNRS/IN2P3, ENSICAEN, Université de Caen Basse-Normandie, Caen, France)
 *  \li Xavier Garrido (LAL, CNRS/IN2P3, Université Paris-Sud, Orsay, France)
 *  \li Ben Morgan (The University of Warwick, Coventry, United Kingdom)
 *
 * Bayeux also makes use of some code fragments or contributions written by other people and/or companies/institutions.
 *
 *  \li Hongli Lai, for the BinReloc tools (WTFPL licence)
 *  \li Christian Pfligersdorffer, for the portable binary archive (version 5.0, Boost software licence)
 *  \li Kitware, Inc., Insight Software Consortium, for dynamic loader class (OSI-approved BSD License)
 *  \li Vladimir Tretyak, for the original genbb/decay0 event generator
 *  \li Nicolas Devillard, for the Gnuplot pipe utility (GNU GPL v2)
 *
 * \section license_section License
 *
 * Bayeux is distributed under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *\verbatim
 Copyright (C) 2008-2013 François Mauger, LPC Caen CNRS/IN2P3 and Université de Caen Basse-Normandie
 Contact: mauger@lpccaen.in2p3.fr

 This file is part of the Bayeux library.

 Bayeux is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Bayeux is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Bayeux.  If not, see <http://www.gnu.org/licenses/>.
\endverbatim
 */

#ifndef BAYEUX_BAYEUX_H
#define BAYEUX_BAYEUX_H
// Standard Library
#include <string>

// This project
//#include <bayeux/bayeux_config.h>

/// Top-level namespace of the Bayeux library
namespace bayeux {

  /// Initialize the Bayeux library core functionnalities
  void initialize(int argc_ = 0, char * argv_[] = 0);

  /// Terminate the Bayeux library core functionnalities
  void terminate();

  void _special_initialize_impl();

  void _special_terminate_impl();

}

/// bayeux initialization macro using main function arguments
#define BAYEUX_INIT_MAIN(Argc,Argv)		\
  ::bayeux::initialize( Argc , Argv );		\
  /**/

/// bayeux initialization macro
#define BAYEUX_INIT()				\
  ::bayeux::initialize(0, 0);			\
  /**/

/// bayeux termination macro
#define BAYEUX_FINI()				\
  ::bayeux::terminate();			\
  /**/

#endif // BAYEUX_BAYEUX_H

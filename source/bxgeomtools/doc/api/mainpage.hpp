/*! \mainpage geomtools C++ library documentation
 *
 * \htmlonly
 * <!--div class="logo">
 * \endhtmlonly
 * \image html logo_geomtools.png
 * \htmlonly
 * </div-->
 * \endhtmlonly
 *
 * \section overview_section Overview
 *
 * \t geomtools is a C++ library which provides general purpose utility classes
 *  for geometry. It is based on the CLHEP 2D and 3D vectors and rotation 
 *  matrices. It provides some serialization facilities for vector and rotation classes
 *  based on Boost/Serialization (through the \b datatools library).
 *  Some basic 1D, 2D and 3D shapes are implemented with useful features. 
 *
 *  Most parts of the library are compatible with CLHEP and GEANT4.
 *
 *  More, it is possible to build a geometry model in the way the GEANT4
 *  library does with GDLM exporting capability. Some tools are available to:
 *  - dynamically build a tree of geometry models that describes a full static 3D-setup from
 *    pure ASCII files
 *  - automate the placement of solid with respect to others thanks to useful automated
 *    placement algorithms
 *  - identify some or all 3D-objects in a geometry, assigning to some of them a
 *    unique autocomputed \e geometry \e ID (GID)
 *  - enable the automated \e mapping between any 3D-object (placement, shape)
 *    and the associated geometry ID
 *  - associate some arbitrary properties (visibility, material, mapping...) to any virtual solid
 *  - display part of the geometry setup using a simple graphics renderer based on gnuplot
 *  - export a geometry setup to GEANT4 using the GDML language
 *  - extends the library by providing new geometry model classes with auto-registration 
 *    in a \e models \e factory
 *
 *  This package depends on:
 *  - the \b datatools library (https://nemo.lpc-caen.in2p3.fr/wiki/datatools/). This implies
 *  the following dependencies (see the \b datatools requirements):
 *    - the \b Boost C++ library 
 *    - the \b CLHEP C++ library (version 2.0 and above)
 *  - the \b mygsl library. This implies the following dependency (see the \b mygsl requirements)::
 *    - the \b GSL (GNU Scientific Library):
 *  - the \b gnuplot program (for online graphics display)
 *
 * \section contents_section Contents 
 *
 * To be done.
 *
 * \section quick_start_section Quick start 
 *
 * To be done.
 *
 * \section license_section License
 *
 * geomtools is distributed under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * \verbatim
 * Copyright (C) 2012 François Mauger, LPC Caen CNRS/IN2P3 and Université de Caen Basse-Normandie
 * Contact: mauger@lpccaen.in2p3.fr
 * 
 * This file is part of the geomtools library.
 * 
 * geomtools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * geomtools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with geomtools.  If not, see <http://www.gnu.org/licenses/>.
 * \endverbatim
 *
 */

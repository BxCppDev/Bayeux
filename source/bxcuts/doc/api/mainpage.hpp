/*! \mainpage cuts C++ library documentation
 *
 * \htmlonly
 * <!--div class="logo">
 * \endhtmlonly
 * \image html logo_cuts.png
 * \htmlonly
 * </div-->
 * \endhtmlonly
 *
 * \section overview_section Overview
 *The \t cuts C++ library provides some classes to create, 
 * compose and apply selection algorithms (cut functors) on arbitrary data.
 *
 * Given an arbitrary data model, the user can derived his/her 
 * own cut algorithms using the \c i_cut class interface 
 * (include/cuts/i_cut.h). Such 
 * cuts may be combined using boolean logic in order to apply 
 * more complex criteria :
 *
 * Example:
 *    "\t(CUT1 \t or \t CUT2) \t and \t (not \t CUT3) \t and \t (CUT6 \t nor \t CUT7)"
 * where each \t CUTX instances implement the 
 * \t protected virtual \c _accept() method
 * that applies a specific selection criterion on the data model.
 *
 * Basic boolean cut-functors are provided by the library :
 *  \li unary cut:   \c not_cut, 
 *  \li binary cuts: \c and_cut, \c or_cut, \c not_cut, \c xor_cut... 
 *  \li more:        \c multi_cut (general and for many cuts)
 *  
 * A mechanism is implemented within a \e factory class in order
 * to auto-register new cut classes in the library and make
 * possible to allocate and setup cut instances through the factory.
 * 
 * A \e cut \e manager is provided. It enables:
 *  \li the description of collection of cuts (and combinaison of them) 
 *    using ASCII files (through the 'datatools::utils::properties' class), 
 *  \li the use of any of such cut instances to be applied to user 
 *    data (selection).
 *
 *
 * \section contents_section Contents 
 *
 * The \c cuts::cut_manager class is the most useful resource to be used by
 * the user. It provides a mechanism to dynamically allocate cut objects
 * from an ASCII  configuration file, pick up one single cut among a 
 * dictionnary of available cut objects, and use it as the selector on a 
 * set of data.
 *
 * \section quick_start_section Quick start 
 *
 * Empty
 *
 * \section license_section License
 *
 * cuts is distributed under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * \verbatim
 * Copyright (C) 2012 François Mauger, Université de Caen Basse-Normandie, LPC Caen (CNRS/IN2P3)
 * Contact: mauger@lpccaen.in2p3.fr
 * 
 * This file is part of the cuts library.
 * 
 * cuts is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * cuts is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with cuts.  If not, see <http://www.gnu.org/licenses/>.
 * \endverbatim
 *
 */

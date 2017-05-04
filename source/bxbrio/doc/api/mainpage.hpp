/*! \mainpage brio C++ library documentation
 *
 * \htmlonly
 * <!--div class="logo">
 * \endhtmlonly
 * \image html logo_brio.png
 * \htmlonly
 * </div-->
 * \endhtmlonly
 *
 * \section overview_section Overview
 *
 *  brio is a C++ library which provides a serialization mechanism
 * based on the Boost/Serialization library and the ROOT I/O mechanism.
 * It enables the serialization of arbitrary  Boost-serializable objects.

 *  The  brio package implements some  writer and  reader classes that enable
 *   to store/load arbitrary  serializable objects using  portable 
 *    binary  archives from the Boost/Serialization system coupled with 
 *   ROOT files (TFile) and trees (TTree) 
 *   as storage media. 
 *   
 *   While loading back the data, serialized objects can then be accessed sequentially or 
 *   non-sequentially while serialized Boost archives (ASCII, XML or binary) using standard
 *   I/O filestreams can only be accessed sequentially (see the  
 *   datatools:::data_reader
 *   and  datatools::data_writer classes from the  datatools 
 *   C++ program library).
 *
 * \section contents_section Contents 
 *
 * The  brio library implements the  brio::writer and the  brio::reader classes.
 *
 * \section quick_start_section Quick start 
 *
 * Here is a sample code that shows how to create a simple serializable class 
 * (using the techniques proposed by the  datatools program library) and use 
 * a  brio::writer and  brio::reader objects to store and load a set data
 * of this class.
 *
 * \code
 * #include <brio/writer.h>
 * #include <brio/reader.h>
 * #include <boost/cstdint.hpp>
 * #include <datatools/i_serializable.h>
 *
 * // A simple serializable class :
 * // here we use macros provided by the datatools library.
 * struct dummy : DATATOOLS_SERIALIZABLE_CLASS 
 * {
 *   int value;
 *   DATATOOLS_SERIALIZATION_DECLARATION(); 
 * };
 *
 * // Implementation of the serialization interface :
 * // the 'dummy' class is associated to its  serialization  tag :
 * // the "dummy" string
 * DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION (dummy, "dummy")
 * 
 * // Instantiation of the serialization method :
 * #include <boost/serialization/base_object.hpp>
 * template<class Archive>
 * void dummy::serialize (Archive & ar, const unsigned int version)
 * {
 *   ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
 *   ar & boost::make_nvp ("val", val);
 *   return;
 * }
 *
 * int main ()
 * {
 *   { // Store :
 *     brio::writer my_writer ("dummies.brio");
 *     for (int i = 0; i < 10; i++) 
 *     {
 *       dummy my_data;
 *       my_data.val = i;
 *       my_writer.store (my_data);
 *     }
 *     my_writer.close ();
 *   }
 *   { // Load :
 *     brio::reader my_reader ("dummies.brio");
 *     while (my_reader.has_next ()) 
 *     {
 *       dummy my_data;
 *       my_reader.load_next (my_data);
 *     }
 *     my_reader.close ();
 *   }
 *   return 0;
 * }
 * \endcode
 *
 *
 * \section authors Authors
 *
 *  \li François Mauger (LPC Caen, CNRS/IN2P3, ENSICAEN, Université de Caen Basse-Normandie, Caen, France)
 *  \li Ben Morgan (The University of Warwick, Coventry, United Kingdom) 
 *  
 * 
 * \section license_section License
 *
 * brio is distributed under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *\verbatim
Copyright (C) 2012 François Mauger, LPC Caen CNRS/IN2P3 and Université de Caen Basse-Normandie
Contact: mauger@lpccaen.in2p3.fr

brio is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
 
brio is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with brio.  If not, see <http://www.gnu.org/licenses/>.
\endverbatim
 *
 */

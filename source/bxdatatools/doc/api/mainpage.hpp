/*! \mainpage datatools C++ library documentation
 *
 * \htmlonly
 * <!--div class="logo">
 * \endhtmlonly
 * \image html logo_datatools.png
 * \htmlonly
 * </div-->
 * \endhtmlonly
 *
 * \section overview Overview
 *
 * datatools is a C++ library which provides...
 *
 * \li General purpose containers
 * \li Serialization support
 * \li General purpose utilities
 *
 * \section example Quick example
 * 
 * Here is a simple example of how to use datatool:
 *
 * \code
 * #include <datatools/utils/properties.h>
 * #include <iostream>
 *
 * using namespace std;
 *
 * int main ()
 * {
 *   {
 *     datatools::utils::properties parameters;
 *     parameters.set_description ("A list of configuration parameters");
 *     parameters.store_flag ("debug", "A flag that activates debugging functionalities");
 *     parameters.store ("author", "M. Python", "The name of the authors");
 *     parameters.store ("pi", 3.14159, "The approximated value of Pi");
 *     parameters.store ("max_users", 7, "The maximum number of users");
 *
 *     datatools::utils::properties::write_config ("params.conf", parameters);
 *   }
 *
 *   {
 *     datatools::utils::properties parameters;
 *     datatools::utils::properties::read_config ("params.conf", parameters);
 *     if ( parameters.has_flag (debug) )
 *       {
 *          cout << "Debug mode is activated." << endl;
 *       }
 *     if ( parameters.has_key ("author") )
 *       {
 *          cout << "Author is : " << parameters.fetch_string ("author") << endl;
 *       }
 *     if ( parameters.has_key ("pi") )
 *       {
 *          cout << "Pi value is : " << parameters.fetch_real ("pi") << endl;
 *       }
 *     if ( parameters.has_key ("max_users") )
 *       {
 *          cout << "Max number of users is : " << parameters.fetch_integer ("max_users") << endl;
 *       }
 *   }
 *   return 0;
 * }
 * \endcode
 *
 * \section license License
 *
 * datatools is distributed under the terms of the GNU General Public License as published by the Free Software Foundation, either version of the License, or (at your option) any later version.
 *
 * \verbatim
 * Copyright (C) 2011 François Mauger, LPC Caen CNRS/IN2P3 and Université de Caen Basse-Normandie
 * Contact: mauger@lpccaen.in2p3.fr
 * 
 * This file is part of the datatools library.
 * 
 * datatools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * datatools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with datatools.  If not, see <http://www.gnu.org/licenses/>.
 * \endverbatim
 *
 */

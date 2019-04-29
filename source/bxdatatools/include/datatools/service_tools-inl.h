/// \file datatools/service_tools-inl.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2019-02-01
 *
 * Copyright (C) 2011-2019 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *   Service tools, template defs.
 *
 */

#ifndef DATATOOLS_SERVICE_TOOLS_INL_H
#define DATATOOLS_SERVICE_TOOLS_INL_H

// Standard Library:
#include <typeinfo>

namespace datatools {

  /** Check if a typed service object with given name exist in a dictionary of services
   *   @param services_ The dictionary of service entries
   *   @param service_name_ The name of the service to be checked
   *   @return a mutable reference to the service instance requested by name and type
   */
  template<class T>
  bool has(const service_dict_type & services_, const std::string & service_name_)
  {
    if (!service_exists(services_, service_name_)) {
      return false;
    }
    const base_service & srvc = get_service(services_, service_name_);
    const std::type_info & ti = typeid(T);
    const std::type_info & tf = typeid(srvc);
    return ti == tf;
  }

  /** Return a mutable reference to a typed service object with given name from a dictionary of services
   *   @param services_ The dictionary of service entries
   *   @param service_name_ The name of the service to be checked
   *   @return a mutable reference to the service instance requested by name and type
   */
  template<class T>
  T & grab(service_dict_type & services_, const std::string & service_name_)
  {
    base_service & srvc = grab_service(services_, service_name_);
    const std::type_info & ti = typeid(T);
    const std::type_info & tf = typeid(srvc);
    DT_THROW_IF(ti != tf, std::logic_error, "Service '" << service_name_ << "' is not of requested type!");
    return dynamic_cast<T &>(srvc);
  }

  /** Return a non mutable reference to a typed service object with given name from a dictionary of services
   *   @param services_ The dictionary of service entries
   *   @param service_name_ The name of the service to be checked
   *   @return a const reference to the service instance requested by name and type
   */
  template<class T>
  const T & get(const service_dict_type & services_, const std::string & service_name_)
  {
    const base_service & srvc = get_service(services_, service_name_);
    const std::type_info & ti = typeid(T);
    const std::type_info & tf = typeid(srvc);
    DT_THROW_IF(ti != tf, std::logic_error, "Service '" << service_name_ << "' is not of requested type!");
    return dynamic_cast<const T &>(srvc);
  }

}  // end of namespace datatools

#endif // DATATOOLS_SERVICE_TOOLS_INL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

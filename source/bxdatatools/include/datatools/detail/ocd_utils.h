/* -*- mode: c++; -*- */
/* datatools::ocd_utils.h */
/* 
 * Description :
 *
 *  Some useful classes related to OCD features.
 *
 * Copyright (C) 2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 */ 
#ifndef DATATOOLS_DETAIL_OCD_UTILS_H_
#define DATATOOLS_DETAIL_OCD_UTILS_H_

// Standard Library
#include <string>

namespace datatools {
  class object_configuration_description;
}

namespace datatools {                                                   
  namespace detail {                                                   
    namespace ocd {                                                   
      template <typename T>
      struct loader {};
      template <typename T>
      void implement_load(::datatools::object_configuration_description & ocd_);
}}}

#include <boost/utility/enable_if.hpp>
#include <boost/mpl/has_xxx.hpp>

/// Template support for configurable type 
BOOST_MPL_HAS_XXX_TRAIT_DEF(load);

namespace datatools {                                                   

template <typename ConfigurableType>
bool load_ocd(::datatools::object_configuration_description & ocd_,
              typename boost::disable_if< has_load< ::datatools::detail::ocd::loader<ConfigurableType> > >::type* dummy = 0)
{
  //std::cerr << "DEVEL: disable_if ! \n";
  return false;
}


template <typename ConfigurableType>
bool load_ocd(::datatools::object_configuration_description & ocd_,
              typename boost::enable_if< has_load< ::datatools::detail::ocd::loader<ConfigurableType> > >::type* dummy = 0)
{
  //std::cerr << "DEVEL: enable_if ! \n";
  typename ::datatools::detail::ocd::loader<ConfigurableType>::load l;
  l(ocd_);
  return true;
}

}

#endif // DATATOOLS_DETAIL_OCD_UTILS_H_


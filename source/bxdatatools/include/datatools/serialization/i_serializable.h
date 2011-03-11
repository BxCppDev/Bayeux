/* i_serializable.h */
/* 
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef __datatools__serialization__i_serializable_h
#define __datatools__serialization__i_serializable_h 1

#include <string>

#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/assume_abstract.hpp>

#include <datatools/serialization/archives_list.h>

namespace datatools {
  
  namespace serialization {
    
    class i_serializable
    {
    public:

      virtual const std::string & get_serial_tag () const = 0;
  
    private:
      
      friend class boost::serialization::access;
      
      template<class Archive>
	void serialize (Archive & ar_, const unsigned int file_version_)
	{
	  return;
	}
      
    };
    
  } // end of namespace serialization 

} // end of namespace datatools 

#define DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP				\
  boost::serialization::make_nvp(							\
    "datatools__serialization__i_serializable",                                         \
    boost::serialization::base_object<datatools::serialization::i_serializable >(*this) \
  )
/**/

BOOST_SERIALIZATION_ASSUME_ABSTRACT(datatools::serialization::i_serializable)

#endif // __datatools__serialization__i_serializable_h

/* end of i_serializable.h */

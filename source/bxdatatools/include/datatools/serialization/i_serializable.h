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

#ifndef DATATOOLS_SERIALIZATION_I_SERIALIZABLE_H_
#define DATATOOLS_SERIALIZATION_I_SERIALIZABLE_H_

// Standard Library
#include <string>

// Third Party
// - Boost
#include <boost/serialization/access.hpp>

// Datatools
#include <datatools/serialization/utils.h>


namespace datatools {
namespace serialization {
    
class i_serializable {
 public:
  i_serializable() {};
  virtual ~i_serializable() {}

  virtual const std::string& get_serial_tag() const = 0;

 private:
  friend class boost::serialization::access;
  BOOST_SERIALIZATION_SERIALIZE_DECLARATION()
};
    
} // end of namespace serialization 
} // end of namespace datatools 

#define DATATOOLS_SERIALIZATION_SERIAL_TAG_DECLARATION()		\
 public:								\
  static const std::string SERIAL_TAG;					\
  virtual const std::string& get_serial_tag() const;			\
  /**/

#define DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(ClassName,ClassSerialTag) \
  const std::string ClassName::SERIAL_TAG = ClassSerialTag;		\
  const std::string& ClassName::get_serial_tag() const {		\
    return ClassName::SERIAL_TAG;					\
  }									\
  /**/

#define DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP		\
  boost::serialization::make_nvp("datatools__serialization__i_serializable", \
				 boost::serialization::base_object<datatools::serialization::i_serializable>(*this)) \
  									\
  /**/

#define DATATOOLS_SERIALIZABLE_CLASS		  \
 public datatools::serialization::i_serializable \
  /**/

#define DATATOOLS_SERIALIZATION_DECLARATION()	   \
 public:					   \
  DATATOOLS_SERIALIZATION_SERIAL_TAG_DECLARATION() \
 private:					   \
  friend class boost::serialization::access;	   \
  BOOST_SERIALIZATION_SERIALIZE_DECLARATION()	   \
  /**/

#endif // DATATOOLS_SERIALIZATION_I_SERIALIZABLE_H_


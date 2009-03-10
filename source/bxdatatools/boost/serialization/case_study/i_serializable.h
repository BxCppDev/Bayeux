// -*- mode: C++; -*- 
/* i_serializable.h */
/* 
 * Author: F. Mauger <mauger@lpccaen.in2p3.fr>
 * Date:   2009-03-10
 *
 * Copyright (c) 2008 François Mauger
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy 
 * at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

/*
 * This is a pure virtual interface that provides the 
 * `get_serial_tag' method.
 *
 * The `serial tag' string is used as a flag associated to
 * the class of some serialized object within a boost 
 * archive. It play the role of a user-friendly class id.
 *
 * Note you can use this library without explicit usage of 
 * `i_serializable' inherited objects.
 *
 */

#ifndef __boost_case_study__serialization__i_serializable_h
#define __boost_case_study__serialization__i_serializable_h 1

namespace boost_case_study {

  namespace serialization {
    
    class i_serializable
      {
      public:
	virtual const std::string & get_serial_tag () const = 0;
      };
    
  } // end of namespace serialization 

} // end of namespace boost_case_study 

#endif // __boost_case_study__serialization__i_serializable_h

/* end of i_serializable.h */

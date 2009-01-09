// -*- mode: c++; -*- 
/* geom_id.h
 * Author(s):     F.Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2006-11-22
 * Last modified: 2006-11-22
 * 
 * License: 
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * 
 * Description: 
 *   General geometry ID
 * 
 * History: 
 * 
 */

#ifndef __GeomUtils__geom_id_h
#define __GeomUtils__geom_id_h 1

// standard includes:
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>
// <here you may add some other standard '#include' directives>

// miscellaneous includes:
#include <SNutils/string_tools.h>
// <here you may add some '#include' directives>

namespace geomutils {

  class geom_id
  {

  // typedefs declarations:
  public: 
    typedef std::list<geom_id> id_coll_t;

  // static constants declarations:
  public: 
    static const char   TOKEN_CAT_INVALID;
    static const char   TOKEN_ADD_INVALID;
    static const char   TOKEN_ADD_SEP;

    static const char   TOKEN_ID_OPEN;
    static const char   TOKEN_ID_SEP;
    static const char   TOKEN_ID_CLOSE;

    static const size_t MIN_ADDRESS_DEPTH=1;
    static const size_t MAX_ADDRESS_DEPTH=10;
    static const int    CAT_INVALID;
    static const int    ADD_INVALID;
  
  // static declarations:
  
  // non static fields declarations:
  protected: 
    int    _category;
    // don't want to use vector/list for better performance:
    size_t _address_depth;
    int    _address[MAX_ADDRESS_DEPTH];
  
  // getters/setters declarations:
  private:
    void __assert_address_value_makes_sense( size_t pos_ , int val_ ) const;
  public: 
    int  get_category() const;
    void set_category( int );
    size_t get_address_depth() const;
    const int * get_address() const;
    int  get_address_value( size_t ) const;
    void set_address_value( size_t , int val_ );
    void set_address_values( int val0_ , 
		      int val1_ = ADD_INVALID, 
		      int val2_ = ADD_INVALID, 
		      int val3_ = ADD_INVALID , 
		      int val4_ = ADD_INVALID ,
		      int val5_ = ADD_INVALID , 
		      int val6_ = ADD_INVALID , 
		      int val7_ = ADD_INVALID , 
		      int val8_ = ADD_INVALID ,
		      int val9_ = ADD_INVALID );
  
  // ctor/dtor:
  public: 
    geom_id();
    geom_id( int , size_t );
    virtual ~geom_id();
  
  // methods:
  public: 
    void init( int , size_t );
    void init_address( size_t );
    void reset_address();
    void reset();
    bool is_initialized() const;
    bool operator==( const geom_id & ) const;
    bool is_valid() const;
    bool is_category( int ) const;

    static void extract( const geom_id & from_id_ , geom_id & to_id_ );
    static void encode_address( std::string & , const int * , size_t );
    static bool decode_address( const std::string & , int * , size_t = 0 );
    friend std::ostream & operator<<( std::ostream & out_ , const geom_id & id_ );
    // <here you may define more class methods>
    
  };

} // end of namespace geomutils

#endif // __GeomUtils__geom_id_h

// end of geom_id.h

// -*- mode: c++; -*- 
/* geom_locator.h
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
 *   Geometry locator
 * 
 * History: 
 * 
 */

#ifndef __GeomUtils__geom_locator_h
#define __GeomUtils__geom_locator_h 1

// standard includes:
#include <cmath>
#include <stdexcept>
#include <iostream>
// <here you may add some other standard '#include' directives>

// miscellaneous includes:
#include <CLHEP/Vector/ThreeVector.h>
#include <CLHEP/Vector/Rotation.h>
#include <CLHEP/Units/SystemOfUnits.h>

#include <GeomUtils/geom_id.h>
#include <GeomUtils/cat_dictionary.h>
#include <GeomUtils/geom_data.h>
// <here you may add some other '#include' directives>

namespace geomutils {

  class geom_locator
  {
  
  // typedefs declarations:
  public:
    typedef std::list<geom_id> geom_id_coll_t;
  
  // static constants declarations:
  
  // static declarations:
  
  // non static fields declarations:
  protected: 
    bool                _debug;
    bool                _initialized;  
    cat_dictionary      _categories_dict;

  // getters/setters declarations:
  protected:
    void _assert_lock( const std::string & what_ );
  public: 
    bool is_debug() const;
    void set_debug( bool );
    bool is_initialized() const;
  
  // ctor/dtor:
  public: 
    geom_locator();
    virtual ~geom_locator();
  
  // methods:
  protected:
    virtual void _at_init_do();
    virtual void _at_reset_do();
  public: 
    void load_cat_dictionary( const std::string & );
    void dump_cat_dictionary( std::ostream & ) const;
    void init();
    void reset();
    virtual bool category_is_valid( int ) const;
    virtual bool category_is_valid( const std::string & ) const;
    std::string get_category_label( int ) const;

    /************************************************************/
    // the following pure abstract methods should be implemented!

    // check if 'id_' is valid:
    virtual bool validate_id( const geom_id & id_ ) const = 0;

    // find the 'id_' of the object from its category where position 'position_' lies in;
    // return invalid 'id_' if not found:
    virtual void find_id( geom_id & id_ , const CLHEP::Hep3Vector & position_ ) const = 0;

    // get the geometry data related to object with id 'id_':
    virtual void locate_id( const geom_id & id_ , geom_data & gdata_ ) const = 0;
    /************************************************************/

    // ID manipulation methods:
    bool is_id( const geom_id & , int ) const;
    bool is_id( const geom_id & , const std::string & ) const;
    void create_id( geom_id & , int ) const;
    void create_id( geom_id & , const std::string & ) const;
    /*** may be overloaded but standard on covers most usage ***/
    virtual void extract_id( const geom_id & , geom_id & ) const;
    int get_address_id( const geom_id & , int ) const;
    int get_address_id( const geom_id & , const std::string & ) const;
    void set_address_id( geom_id & id_ , 
			 int add_depth_ ,
			 int val_ ) const;
    void set_address_id( geom_id & id_ , 
			 const std::string & add_label_ ,
			 int val_ ) const;
    void set_full_address_id( geom_id & , 
			      int , int = -1 , int = -1 , int = -1 , int = -1 , 
			      int = -1 , int = -1 , int = -1 , int = -1 , int = -1 ) const;
    void encode_id( const geom_id & , std::string & ) const;
    bool decode_id( geom_id & , const std::string & ) const;
    std::string to_string_id( const geom_id & ) const;
    virtual void dump( std::ostream & , 
		       const std::string & title_ = "geomutils::geom_locator" , 
		       const std::string & indent_ = "" , 
		       bool inherit_ = false ) const;

    // <here you may define more class methods>

  };

} // end of namespace geomutils

#endif // __GeomUtils__geom_locator_h

// end of geom_locator.h

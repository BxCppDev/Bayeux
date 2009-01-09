// -*- mode: c++; -*- 
/* geom_data.h
 * Author(s):     F.Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2006-11-26
 * Last modified: 2006-11-26
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
 *   Geometry data concerning an arbitrary object
 * 
 * History: 
 * 
 */

#ifndef __GeomUtils__geom_data_h
#define __GeomUtils__geom_data_h 1

// standard includes:
#include <cmath>
#include <stdexcept>
#include <iostream>
// <here you may add some other standard '#include' directives>

// miscellaneous includes:
#include <CLHEP/Vector/ThreeVector.h>
#include <CLHEP/Vector/Rotation.h>
#include <GeomUtils/geom_tools.h>
#include <GeomUtils/shape_data.h>
// <here you may add some other '#include' directives>

namespace geomutils {

  class geom_data : public shape_data
  {
  
  // typedefs declarations:
  
  // static constants declarations:
  
  // static declarations:
  
  // non static fields declarations:
  protected: 
    CLHEP::Hep3Vector  _position;
    CLHEP::HepRotation _rotation;         // world->local coordinates systems
    CLHEP::HepRotation _inverse_rotation; // local->world coordinates systems
  
  // getters/setters declarations:
  public: 
    const CLHEP::Hep3Vector & get_position() const;
    CLHEP::Hep3Vector & get_position();
    void set_position( double , double , double );
    void set_position( const CLHEP::Hep3Vector & );
    const CLHEP::HepRotation & get_rotation() const;
    const CLHEP::HepRotation & get_inverse_rotation() const;
    void set_orientation( const CLHEP::HepRotation & );
    void set_orientation( double , double , double );

  // ctor/dtor:
  public: 
    geom_data();
    virtual ~geom_data();
    virtual void reset();
  
  // methods:
    void world_to_local_pos( const CLHEP::Hep3Vector & , CLHEP::Hep3Vector & ) const;
    void local_to_world_pos( const CLHEP::Hep3Vector & , CLHEP::Hep3Vector & ) const;
    void dump( std::ostream & out_ , 
	       const std::string & title_  = "geomutils::geom_data", 
	       const std::string & indent_ = "" ,
	       bool inherit_               =  false ) const;
     // <here you may define more class methods>

  };

} // end of namespace geomutils

#endif // __GeomUtils__geom_data_h

// end of geom_data.h

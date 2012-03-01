// -*- mode: c++; -*- 
/* geom_data.cc
 */

#include <GeomUtils/geom_data.h>

namespace geomutils {

  // static constants definitions:
  
  // static definitions:
  
  // getters/setters definitions:
  const CLHEP::Hep3Vector & geom_data::get_position() const
  {
    return _position;
  }

  CLHEP::Hep3Vector & geom_data::get_position()
  {
    return _position;
  }
  
  void geom_data::set_position( double x_ , double y_ , double z_ )
  {
    set_position(CLHEP::Hep3Vector(x_,y_,z_));
  }

  void geom_data::set_position( const CLHEP::Hep3Vector & new_value_ )
  {
    _position=new_value_;
  }
  
  const CLHEP::HepRotation & geom_data::get_rotation() const
  {
    return _rotation;
  }
  
  void geom_data::set_orientation( const CLHEP::HepRotation & new_value_ )
  {
    _rotation=new_value_;
    _inverse_rotation=_rotation.inverse();
  }

  void geom_data::set_orientation( double phi_ , double theta_ , double delta_ )
  {
    CLHEP::HepRotation r;
    geom_tools::create_rotation(r,phi_,theta_,delta_);
    set_orientation(r);
  }
  
  const CLHEP::HepRotation & geom_data::get_inverse_rotation() const
  {
    return _inverse_rotation;
  }
  
  // ctor/dtor:
  geom_data::geom_data()
  {
  }
  
  geom_data::~geom_data()
  {
  }

  void geom_data::reset()
  {
    shape_data::reset();
    set_position(CLHEP::Hep3Vector(0.,0.,0.));
    set_orientation(0.,0.,0.);
  }

  void geom_data::world_to_local_pos( 
				     const CLHEP::Hep3Vector & world_pos_ , 
				     CLHEP::Hep3Vector &       local_pos_ ) const
  {
    CLHEP::Hep3Vector v = world_pos_ - _position;
    //local_pos_=v.transform(_inverse_rotation);
    local_pos_=v.transform(_rotation);
  }

  void geom_data::local_to_world_pos( 
				     const CLHEP::Hep3Vector & local_pos_ , 
				     CLHEP::Hep3Vector & world_pos_ ) const
  {
    CLHEP::Hep3Vector v(local_pos_);
    //world_pos_ = v.transform(_rotation) + _position;
    world_pos_ = v.transform(_inverse_rotation) + _position;
  }

  void geom_data::dump( std::ostream & out_ , 
			const std::string & title_ , 
			const std::string & indent_ ,
			bool inherit_ ) const
  {
    shape_data::dump(out_,title_,indent_,true);
    std::string indent=indent_;
    /*
    std::string title=title_;
    std::string indent=indent_;
    if ( title.empty() ) title=title_;
    out_ << title << std::endl;
    */
    std::string tag="|-- ";
    std::string tagc="|   ";
    std::string last_tag="`-- ";
    std::string last_tagc="    ";
    std::string & atag=tag;
    std::string & atagc=tagc;
    out_ << indent << atag << "Position :            " 
	 << get_position() << std::endl;
    geom_tools::dump(get_rotation(),
		     out_,
		     indent+atag+"Rotation",
		     indent+atagc);
    if ( !inherit_ ) {
      atag=last_tag;
      atagc=last_tagc;
    }
    geom_tools::dump(get_inverse_rotation(),
		     out_,
		     indent+atag+"Inverse rotation",
		     indent+atagc);
  }
  
} // end of namespace geomutils

// end of geom_data.cc

// -*- mode: c++; -*- 
/* cylinder.cc
 */

#include <geomtools/cylinder.h>

namespace geomtools {

  const std::string cylinder::CYLINDER_LABEL = "cylinder";

  double 
  cylinder::get_r () const
  {
    return __r;
  }
  
  void 
  cylinder::set_r (double new_value_)
  {
    if (new_value_ < 0.0 )
      {
	std::ostringstream message;
	message << "cylinder::set_r: Invalid '" << new_value_ << "' R value!";
	throw std::logic_error (message.str ());
      }
    __r = new_value_;
  }
  
  double 
  cylinder::get_radius () const
  {
    return get_r ();
  }

  double 
  cylinder::get_diameter () const
  {
    return (__r + __r);
  }
  
  void 
  cylinder::set_diameter (double new_value_)
  {
    set_r (new_value_ * 0.5);
  }

  double 
  cylinder::get_z () const
  {
    return __z;
  }
  
  void 
  cylinder::set_z (double new_value_)
  {
    if (new_value_ < 0.0) 
      {
	std::ostringstream message;
	message << "cylinder::set_z: Invalid '" << new_value_ << "' Z value!";
	throw std::logic_error (message.str ());
      }
    __z = new_value_;
  }
  
  double 
  cylinder::get_half_z () const
  {
    return __z * 0.5;
  }
  
  void
  cylinder::set_half_z (double new_value_)
  {
    set_z (new_value_ + new_value_);
  }

  void 
  cylinder::set (double r_, double z_)
  {
    set_r (r_);
    set_z (z_);
  }
  
  void 
  cylinder::set_alternative (double d_, double hz_)
  {
    set_diameter (d_);
    set_half_z (hz_);
  }

  // ctor/dtor:
  cylinder::cylinder ()
  {
    reset ();
  }

  cylinder::cylinder (double r_, double z_)
  {
    set (r_,z_);
  }
  
  cylinder::~cylinder ()
  {
  }

  std::string 
  cylinder::get_shape_name () const
  {
    return CYLINDER_LABEL;
  }
 
  double 
  cylinder::get_surface (int mask_) const
  {
    double s = 0.0;
    int mask = mask_;
    if (mask_ == ALL_SURFACES) mask = FACE_ALL;

    if (mask & FACE_SIDE) 
      {
	s += 2 * M_PI * __r * __z;
      }
    if (mask & FACE_BOTTOM) 
      {
	s += M_PI * __r * __r;
      }
    if (mask & FACE_TOP) 
      {
	s += M_PI * __r * __r;
      }
    return s;
  }

  double 
  cylinder::get_volume () const 
  {
    return M_PI * __r * __r * __z;
  }

  double 
  cylinder::get_parameter ( const std::string & flag_ ) const
  {
    if ( flag_ == "r" ) return get_r ();
    if ( flag_ == "radius" ) return get_r ();
    if ( flag_ == "diameter" ) return get_diameter ();
    if ( flag_ == "z" ) return get_z ();
    if ( flag_ == "volume" ) return get_volume ();
    if ( flag_ == "surface.top" ) return get_surface (FACE_TOP);
    if ( flag_ == "surface.bottom" ) return get_surface (FACE_BOTTOM);
    if ( flag_ == "surface.side" ) return get_surface (FACE_SIDE);
    if ( flag_ == "surface" ) return get_surface (FACE_ALL);

    throw std::runtime_error ("cylinder::get_parameter: Unknown flag!");
  }


  bool 
  cylinder::is_valid () const
  {
    return (__r > 0.0 && __z > 0.0);
  }


  void 
  cylinder::init ()
  {
  }

  void 
  cylinder::reset ()
  {
    __r = -1.0;
    __z = -1.0;
  }

  bool 
  cylinder::is_inside (const vector_3d & point_, double skin_) const
  {
    double skin = get_skin ();
    if (skin_ > USING_PROPER_SKIN) skin = skin_;

    double r = hypot (point_.x (), point_.y ());
    if ( r > __r + 0.5 * skin ) return false;
    if ( std::abs (point_.z ()) > 0.5 *__z + 0.5 * skin ) return false;
    return true;
  }

  bool 
  cylinder::is_on_surface (const vector_3d & point_ , 
			   int    mask_ ,
			   double skin_) const
  {
    double skin = get_skin ();
    if (skin_ > USING_PROPER_SKIN) skin = skin_;

    int mask = mask_;
    if (mask_ == ALL_SURFACES) mask = FACE_ALL;

    double r = hypot (point_.x (), point_.y ());
    if (mask & FACE_BOTTOM) 
      {
	if ( (std::abs (point_.z () + 0.5 * __z) < 0.5 * skin) 
	     && (r < __r)) return true;
      }
    if (mask & FACE_TOP) 
      {
	if ( ( std::abs (point_.z () - 0.5 * __z) < 0.5 * skin) 
	     && (r < __r)) return true;
      }
    if (mask & FACE_SIDE) 
      {
	if ( (std::abs (point_.z ()) < 0.5 * __z)   
	     && (std::abs ( r - __r ) < 0.5 * skin)) return true;
      }
    return false;
  }
  
  bool 
  cylinder::find_intercept (const vector_3d & from_, 
		       const vector_3d & direction_,
		       vector_3d & intercept_,
		       int & face_,
		       double skin_) const
  {
    face_ = 0;
    invalidate (intercept_);
    return false;
  }

  std::ostream & operator<< (std::ostream & out_, const cylinder & c_)
  {
    out_ << '{' << cylinder::CYLINDER_LABEL << ' ' 
	 << c_.__r << ' ' 
	 << c_.__z << '}';
    return out_;
  }

  std::istream & operator>> ( std::istream & in_, cylinder & c_)
  {
    c_.reset ();
    char c = 0;
    in_.get (c);
    if (c != '{') 
      {
	in_.clear (std::ios_base::failbit);
	return in_;
      } 
    std::string name;
    in_ >> name;
    if (name != cylinder::CYLINDER_LABEL) 
      {
	in_.clear (std::ios_base::failbit);
	return in_;
      } 
    double r, z;
    in_ >> r >> z;
    if (! in_) 
      {
	in_.clear (std::ios_base::failbit);
	return in_;
      } 
    c = 0;
    in_.get (c);
    if (c != '}') 
      {
	in_.clear (std::ios_base::failbit);
	return in_;
      } 
    try 
      {
	c_.set (r,z);
      }
    catch (...)
      {
	c_.reset ();
	in_.clear (std::ios_base::failbit);
      }
    return in_;
  }

} // end of namespace geomtools

// end of cylinder.cc

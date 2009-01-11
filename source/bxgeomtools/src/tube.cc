// -*- mode: c++; -*- 
/* tube.cc
 */

#include <geomtools/tube.h>

namespace geomtools {

  const std::string tube::TUBE_LABEL = "tube";
  
  double 
  tube::get_z () const
  {
    return __z;
  }
  
  void 
  tube::set_z (double new_value_)
  {
    if (new_value_ < 0.0) 
      {
	std::ostringstream message;
	message << "tube::set_z: Invalid '" 
		<< new_value_ << "' Z value!";
	throw std::logic_error (message.str ());
      }
    __z = new_value_;
  }
  
  double 
  tube::get_half_z () const
  {
    return 0.5 * __z;
  }
  
  void 
  tube::set_half_z (double new_value_)
  {
    set_z (new_value_ + new_value_);
  }
  
  double 
  tube::get_inner_r () const
  {
    return __inner_r;
  }
  
  double 
  tube::get_outer_r () const
  {
    return __outer_r;
  }
  
  void 
  tube::set_radii (double inner_r_, double outer_r_)
  {
    if (inner_r_ < 0.0) 
      {
	std::ostringstream message;
	message << "tube::set_radii: Invalid inner radius '" 
		<< inner_r_ << "'!";
	throw std::logic_error (message.str ());
      }
    __inner_r = inner_r_;
    if (outer_r_ < 0.0) 
      {
	std::ostringstream message;
	message << "tube::set_radii: Invalid outer radius '" 
		<< outer_r_ << "'!";
	throw std::logic_error (message.str ());
      }
    __outer_r = outer_r_;
    if (__outer_r <= __inner_r) 
      {
	std::ostringstream message;
	message << "tube::set_radii: Outer radius '" 
		<< __outer_r << "' <= inner radius '"
		<< __inner_r << "'!";
	throw std::logic_error (message.str ());
      }
  }
  
  void 
  tube::set (double ir_, double or_, double z_)
  {
    set_radii (ir_ ,or_);
    set_z (z_);
  }
  
  void 
  tube::set_half (double ir_, double or_, double hz_)
  {
    set_radii (ir_, or_);
    set_half_z (hz_);
  }

  
  // ctor/dtor:
  tube::tube ()
  {
    reset ();
  }

  tube::tube (double ir_, double or_, double z_)
  {
    set (ir_, or_, z_);
  }
    
  tube::~tube ()
  {
  }

  double
  tube::get_surface ( int mask_ ) const
  {
    double s = 0.0;
    if (mask_ & FACE_INNER_SIDE) 
      {
	s += 2.0 * M_PI * __inner_r * __z;
      }
    if ( mask_ & FACE_OUTER_SIDE ) 
      {
	s += 2.0 * M_PI * __outer_r * __z;
      }
    if ( mask_ & FACE_BOTTOM ) 
      {
	s += M_PI * (__outer_r * __outer_r - __inner_r * __inner_r);
      }
    if ( mask_ & FACE_TOP ) 
      {
	s += M_PI * (__outer_r * __outer_r - __inner_r * __inner_r);
      }
    return s;
  }

  double 
  tube::get_volume () const 
  {
    return M_PI * (__outer_r * __outer_r - __inner_r * __inner_r ) * __z;
  }

  bool 
  tube::is_valid () const
  {
    return (__inner_r > 0.0 && __outer_r > __inner_r && __z > 0.0);
  }

  void 
  tube::reset ()
  {
    __inner_r = -1.0;
    __outer_r = -1.0;
    __z       = -1.0;
  }

  std::string 
  tube::get_shape_name () const
  {
    return tube::TUBE_LABEL;
  }

  bool 
  tube::is_inside ( const vector_3d & point_ , double skin_ ) const
  {
    double skin = get_skin ();
    if (skin_ > USING_PROPER_SKIN) skin = skin_;

    double r = hypot (point_.x (), point_.y ());
    if (r > __outer_r + 0.5 * skin 
	|| r < __inner_r - 0.5 * skin) return false;
    if (std::abs (point_.z ()) > 0.5 * __z + 0.5 * skin ) return false;
    return true;
  }

  double 
  tube::get_parameter ( const std::string & flag_ ) const
  {
    if ( flag_ == "inner.r" ) return get_inner_r ();
    if ( flag_ == "outer.r" ) return get_outer_r ();
    if ( flag_ == "z" ) return get_z ();
    if ( flag_ == "volume" ) return get_volume ();
    if ( flag_ == "surface.top" ) return get_surface (FACE_TOP);
    if ( flag_ == "surface.bottom" ) return get_surface (FACE_BOTTOM);
    if ( flag_ == "surface.inner.side" ) return get_surface (FACE_INNER_SIDE);
    if ( flag_ == "surface.outer.side" ) return get_surface (FACE_OUTER_SIDE);
    if ( flag_ == "surface" ) return get_surface (FACE_ALL);

    throw std::runtime_error ("tube::get_parameter: Unknown flag!");
  }

  vector_3d 
  tube::get_normal_on_surface (const vector_3d & position_) const
  {
    vector_3d normal;
    invalidate (normal);
    if (is_on_surface (position_, FACE_OUTER_SIDE)) 
      {
	double phi = position_.phi ();
	normal.set (std::cos (phi), std::sin (phi), 0.0);
      }
    else if (is_on_surface (position_, FACE_INNER_SIDE)) 
      {
	double phi = position_.phi ();
	normal.set (std::cos (phi), std::sin (phi), 0.0);
	normal *= -1.;
      }
    else if (is_on_surface (position_, FACE_BOTTOM)) normal.set (0.0, 0.0, -1.0);
    else if (is_on_surface (position_, FACE_TOP)) normal.set (0.0, 0.0, +1.0); 
    return (normal);
  }

  bool 
  tube::is_on_surface (const vector_3d & point_, 
		      int mask_, 
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
	    && (r < __outer_r && r > __inner_r)) return true;
      }
    if (mask & FACE_TOP) 
      {
	if ( ( std::abs (point_.z () - 0.5 * __z) < 0.5 * skin) 
	     && (r < __outer_r && r > __inner_r)) return true;
      }
    if (mask & FACE_OUTER_SIDE) 
      {
	if ( (std::abs (point_.z ()) < 0.5 * __z)   
	    && (std::abs (r - __outer_r) < 0.5 * skin)) return true;
      }
    if (mask & FACE_INNER_SIDE) 
      {
	if ( (std::abs (point_.z ()) < 0.5 * __z)   
	    && (std::abs (r - __inner_r) < 0.5 * skin)) return true;
      }
    return false;
  }
  
  bool 
  tube::find_intercept (const vector_3d & from_, 
			const vector_3d & direction_,
			vector_3d & intercept_,
			int & face_,
			double skin_) const
  {
    face_ = 0;
    invalidate (intercept_);
    return false;
  }

  std::ostream & 
  operator<< (std::ostream & out_, const tube & t_)
  {
    out_ << '{' << tube::TUBE_LABEL << ' ' 
	 << t_.__inner_r << ' ' 
	 << t_.__outer_r << ' ' 
	 << t_.__z << '}';
    return out_;
  }

  std::istream & 
  operator>> (std::istream & in_, tube & t_)
  {
    t_.reset ();
    char c = 0;
    in_.get (c);
    if (c != '{') 
      {
	in_.clear (std::ios_base::failbit);
	return in_;
      } 
    std::string name;
    in_ >> name;
    if (name != tube::TUBE_LABEL) 
      {
	in_.clear (std::ios_base::failbit);
	return in_;
      } 
    double in_r, out_r, z;
    in_ >> in_r >> out_r >> z;
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
	t_.set (in_r, out_r, z);
      }
    catch (...)
      {
	t_.reset ();
	in_.clear (std::ios_base::failbit);
      }
    return in_;
  }

  
} // end of namespace geomtools

// end of tube.cc

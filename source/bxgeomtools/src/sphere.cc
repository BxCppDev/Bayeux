// -*- mode: c++; -*- 
/* sphere.cc
 */

#include <geomtools/sphere.h>

namespace geomtools {

  const std::string sphere::SPHERE_LABEL = "sphere";
  
  double 
  sphere::get_r () const
  {
    return __r;
  }
  
  double 
  sphere::get_radius () const
  {
    return get_r ();
  }
  
  void 
  sphere::set_r (double new_value_)
  {
    if  (new_value_ < 0.0) 
      {
	std::ostringstream message;
	message << "sphere::set_r: Invalid '" 
		<< new_value_ << "' R value!";
	throw std::logic_error (message.str ());
      }
    __r =new_value_;
  }

  void 
  sphere::set_radius (double new_value_)
  {
    set_r (new_value_);
  }
  
  void 
  sphere::set (double r_)
  {
    set_r (r_);
  }
  
  // ctor/dtor:
  sphere::sphere ()
  {
    reset ();
  }

  sphere::sphere (double r_)
  {
    set (r_);
  }
    
  sphere::~sphere ()
  {
  }

  double 
  sphere::get_surface (int mask_) const
  {
    double s = 0.0;
    if  (mask_ & FACE_SIDE) 
      {
	s += 4.0 * M_PI * __r * __r;
      }
    return s;
  }

  double 
  sphere::get_volume () const 
  {
    return 4.0 * M_PI * __r * __r * __r / 3.0;
  }

  bool 
  sphere::is_valid () const
  {
    return  (__r > 0.0);
  }

  void 
  sphere::reset ()
  {
    __r = -1.0;
  }

  std::string 
  sphere::get_shape_name () const
  {
    return SPHERE_LABEL;
  }

  bool 
  sphere::is_inside (const vector_3d & point_, double skin_) const
  {
    double skin = get_skin ();
    if  (skin_ > USING_PROPER_SKIN) skin = skin_;

    double r = point_.mag ();
    if  (r > __r + 0.5 * skin) return false;
    return true;
  }

  double 
  sphere::get_parameter (const std::string & flag_) const
  {
    if  (flag_ == "r") return get_r ();
    if  (flag_ == "radius") return get_r ();
    if  (flag_ == "volume") return get_volume ();
    if  (flag_ == "surface.side") return get_surface (FACE_SIDE);
    if  (flag_ == "surface") return get_surface (FACE_ALL);

    throw std::runtime_error ("sphere::get_parameter: Unknown flag!");
  }

  bool 
  sphere::is_on_surface (const vector_3d & point_, 
			int mask_    , 
			double skin_) const
  {
    double skin = 0.0;
    if  (skin_ > 0.0) skin = skin_;
    double r = point_.mag ();
    if  (mask_ & FACE_SIDE) 
      {
	if  (std::abs (r - __r) < skin) return true;
      }
    return false;
  }
  
  bool 
  sphere::find_intercept (const vector_3d & from_, 
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
  operator<< (std::ostream & out_, const sphere & s_)
  {
    out_ << '{' << sphere::SPHERE_LABEL << ' ' 
	 << s_.__r << '}'; 
    return out_;
  }

  std::istream & 
  operator>> (std::istream & in_, sphere & s_)
  {
    s_.reset ();
    char c = 0;
    in_.get (c);
    if  (c != '{') 
      {
	in_.clear (std::ios_base::failbit);
	return in_;
      } 
    std::string name;
    in_ >> name;
    if  (name != sphere::SPHERE_LABEL) 
      {
	in_.clear (std::ios_base::failbit);
	return in_;
      } 
    double r;
    in_ >> r;
    if  (! in_) 
      {
	in_.clear (std::ios_base::failbit);
	return in_;
      } 
    c = 0;
    in_.get (c);
    if  (c != '}') 
      {
	in_.clear (std::ios_base::failbit);
	return in_;
      } 
    try 
      {
	s_.set (r);
      }
    catch  (...) 
      {
	s_.reset ();
	in_.clear (std::ios_base::failbit);
      }
    return in_;
  }

  
} // end of namespace geomtools

// end of sphere.cc

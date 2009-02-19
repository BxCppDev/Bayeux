// -*- mode: c++; -*- 
/* box.cc 
 */

#include <geomtools/box.h>

namespace geomtools {

  const std::string box::BOX_LABEL = "box";

  double 
  box::get_x () const
  {
    return __x;
  }
  
  void 
  box::set_x (double new_value_)
  {
    if (new_value_ < 0.0 )
      {
	std::ostringstream message;
	message << "box::set_x: Invalid '" << new_value_ << "' X value!";
	throw std::logic_error (message.str ());
      }
    __x = new_value_;
  }
  
  double 
  box::get_half_x () const
  {
    return __x * 0.5;
  }
  
  void 
  box::set_half_x (double new_value_)
  {
    set_x (new_value_ + new_value_);
  }
  
  double 
  box::get_y () const
  {
    return __y;
  }

  double 
  box::get_half_y () const
  {
    return __y * 0.5;
  }
  
  void 
  box::set_half_y (double new_value_)
  {
    set_y (new_value_ + new_value_);
  }
  
  void 
  box::set_y ( double new_value_ )
  {
    if (new_value_ < 0.0) 
      {
	std::ostringstream message;
	message << "box::set_y: Invalid '" << new_value_ << "' Y value!";
	throw std::logic_error (message.str ());
      }
    __y = new_value_;
  }
  
  double 
  box::get_z () const
  {
    return __z;
  }
  
  void 
  box::set_z (double new_value_)
  {
    if (new_value_ < 0.0) 
      {
	std::ostringstream message;
	message << "box::set_z: Invalid '" << new_value_ << "' Z value!";
	throw std::logic_error (message.str ());
      }
    __z = new_value_;
  }
  
  double 
  box::get_half_z () const
  {
    return __z * 0.5;
  }
  
  void
  box::set_half_z (double new_value_)
  {
    set_z (new_value_ + new_value_);
  }

  void 
  box::set (double x_, double y_, double z_)
  {
    set_x (x_);
    set_y (y_);
    set_z (z_);
  }
  
  void 
  box::set_half (double x_,double y_, double z_)
  {
    set_half_x (x_);
    set_half_y (y_);
    set_half_z (z_);
  }

  // ctor/dtor:
  box::box ()
  {
    reset ();
  }

  box::box (double x_ ,double y_ ,double z_ )
  {
    set_x (x_);
    set_y (y_);
    set_z (z_);
  }
  
  box::~box ()
  {
  }

  std::string 
  box::get_shape_name () const
  {
    return BOX_LABEL;
  }
 
  double 
  box::get_surface (int mask_) const
  {
    double s = 0.0;
    int mask = mask_;
    if (mask_ == (int) ALL_SURFACES) mask = FACE_ALL;

    if (mask & FACE_BACK) 
      {
	s += __y * __z;
      }
    if (mask & FACE_FRONT) 
      {
	s += __y * __z;
      }
    if (mask & FACE_LEFT) 
      {
	s += __x * __z;
      }
    if (mask & FACE_RIGHT)
      {
	s += __x * __z;
      }
    if (mask & FACE_BOTTOM) 
      {
	s += __x * __y;
      }
    if (mask & FACE_TOP) 
      {
	s += __x * __y;
      }
    return s;
  }

  double 
  box::get_parameter (const std::string & flag_) const
  {
    if (flag_ == "x") return get_x ();
    if (flag_ == "y") return get_y ();
    if (flag_ == "z") return get_z ();
    if (flag_ == "volume")         return get_volume ();
    if (flag_ == "surface.top")    return get_surface (FACE_TOP);
    if (flag_ == "surface.bottom") return get_surface (FACE_BOTTOM);
    if (flag_ == "surface.back")   return get_surface (FACE_BACK);
    if (flag_ == "surface.front")  return get_surface (FACE_FRONT);
    if (flag_ == "surface.left")   return get_surface (FACE_LEFT);
    if (flag_ == "surface.right")  return get_surface (FACE_RIGHT);
    if (flag_ == "surface")        return get_surface (FACE_ALL);
    
    throw std::runtime_error ("box::get_parameter: Unknown flag!");
  }

  double 
  box::get_volume () const 
  {
    return __x * __y * __z;
  }

  bool 
  box::is_valid () const
  {
    return (__x > 0.0 && __y > 0.0 && __z > 0.0);
  }

  void 
  box::init ()
  {
  }

  void 
  box::reset ()
  {
    __x = -1.0;
    __y = -1.0;
    __z = -1.0;
  }

  bool 
  box::is_inside (const vector_3d & point_, double skin_) const
  {
    double skin = get_skin ();
    if (skin_ > USING_PROPER_SKIN) skin = skin_;

    if (std::abs (point_.x ()) > 0.5 * __x + 0.5 * skin) return false;
    if (std::abs (point_.y ()) > 0.5 * __y + 0.5 * skin) return false;
    if (std::abs (point_.z ()) > 0.5 * __z + 0.5 * skin) return false;
    return true;
  }

  vector_3d 
  box::get_normal_on_surface (const vector_3d & position_) const
  {
    vector_3d normal;
    invalidate (normal);
    if (is_on_surface (position_, FACE_BACK)) normal.set (-1.0, 0.0, 0.0);
    else if (is_on_surface (position_, FACE_FRONT)) normal.set (+1.0, 0.0, 0.0);
    else if (is_on_surface (position_, FACE_LEFT)) normal.set (0.0, -1.0, 0.0);
    else if (is_on_surface (position_, FACE_RIGHT)) normal.set (0.0, +1.0, 0.0);
    else if (is_on_surface (position_, FACE_BOTTOM)) normal.set (0.0, 0.0, -1.0);
    else if (is_on_surface (position_, FACE_TOP)) normal.set (0.0, 0.0, +1.0); 
    return (normal);
  }

  bool 
  box::is_on_surface (const vector_3d & point_ , 
		      int    mask_ ,
		      double skin_) const
  {
    bool debug = false;
    double skin = get_skin ();
    if (skin_ > USING_PROPER_SKIN) skin = 2 * skin_;

    int mask = mask_;
    if (mask_ == (int) ALL_SURFACES) mask = FACE_ALL;

    double hskin = 0.5 * skin;
    if (mask & FACE_BACK) 
      {
	if ((std::abs (point_.x () + 0.5 * __x) < hskin) 
	    && (std::abs (point_.y ()) < 0.5 * __y)
	    && (std::abs (point_.z ()) < 0.5 * __z)) return true;
      }
    if (mask & FACE_FRONT) 
      {
	if ((std::abs (point_.x () - 0.5 * __x) < hskin)
	     && (std::abs (point_.y ()) < 0.5 * __y)
	    && (std::abs (point_.z ()) < 0.5 * __z)) return true;
      }
    if (mask & FACE_LEFT) 
      {
	if ((std::abs (point_.y () + 0.5 * __y) < hskin) 
	    && (std::abs (point_.x ()) < 0.5 * __x)
	    && (std::abs (point_.z ()) < 0.5 * __z)) return true;
      }
    if (mask & FACE_RIGHT) 
      {
	if (debug)
	  {
	    std::clog << "DEVEL: box::is_on_surface: FACE_RIGHT" << std::endl;
	    std::clog << "DEVEL: box::is_on_surface: hskin=" << hskin << std::endl;
	    std::clog << "DEVEL: box::is_on_surface: point=" << point_ << std::endl;
	    std::clog << "DEVEL: box::is_on_surface: dim x=" << 0.5 * __x << std::endl;
	    std::clog << "DEVEL: box::is_on_surface: dim y=" << 0.5 * __y << std::endl;
	    std::clog << "DEVEL: box::is_on_surface: dim z=" << 0.5 * __z << std::endl;
	  }
	if ((std::abs (point_.y () - 0.5 * __y) < hskin) 
	    && (std::abs (point_.x ()) < 0.5 * __x)
	    && (std::abs (point_.z ()) < 0.5 * __z)) return true;
      }
    if (mask & FACE_BOTTOM) 
      {
	if ((std::abs (point_.z () + 0.5 * __z) < hskin) 
	    && (std::abs (point_.x ()) < 0.5 * __x)
	    && (std::abs (point_.y ()) < 0.5 * __y)) return true;
      }
    if (mask & FACE_TOP) 
      {
	if ((std::abs (point_.z () - 0.5 * __z) < hskin) 
	    && (std::abs (point_.x ()) < 0.5 * __x)
	    && (std::abs (point_.y ()) < 0.5 * __y)) return true;
      }
    return false;
  }

  bool 
  box::find_intercept (const vector_3d & from_, 
		       const vector_3d & direction_,
		       intercept_t & intercept_,
		       double skin_) const
  {
    bool debug = false;
    const unsigned int NFACES = 6;
    double t[NFACES];
    t[BACK]   = -(get_half_x () + from_[vector_3d::X]) 
      / direction_[vector_3d::X];
    t[FRONT]  = +(get_half_x () - from_[vector_3d::X]) 
      / direction_[vector_3d::X];
    t[LEFT]   = -(get_half_y () + from_[vector_3d::Y]) 
      / direction_[vector_3d::Y];
    t[RIGHT]  = +(get_half_y () - from_[vector_3d::Y]) 
      / direction_[vector_3d::Y];
    t[BOTTOM] = -(get_half_z () + from_[vector_3d::Z]) 
      / direction_[vector_3d::Z];
    t[TOP]    = +(get_half_z () - from_[vector_3d::Z])
      / direction_[vector_3d::Z];

    double t_min = -1.0;
    int face_min = 0;
    for (int i = 0; i < (int) NFACES; i++)
      {
	double ti = t[i];
	if (debug)
	  {
	    std::clog << "DEVEL: box::find_intercept: t[" << i << "]= "
		      << ti << " t_min=" << t_min 
		      << " face_min=" << face_min 
		      << std::endl;
	  }
	if (std::isnormal (ti) && (ti > 0.0))
	  {
	    int face_bit = (0x1 << i); // face mask
	    vector_3d intercept = from_ + direction_ * ti;
	    if (is_on_surface (intercept, face_bit, skin_))
	      {
		if ((t_min < 0.0) || (ti < t_min))
		  {
		    t_min = ti;
		    face_min = face_bit;
		  }
	      }
	  }
      }
    intercept_.reset ();
    if (face_min > 0) 
      {
	intercept_.set (0, face_min, from_ + direction_ * t_min);
      }
    return intercept_.is_ok ();
  }
  
  std::ostream & 
  operator<< (std::ostream & out_, const box & b_)
  {
    out_ << '{' << box::BOX_LABEL << ' ' 
	 << b_.__x << ' ' 
	 << b_.__y << ' ' 
	 << b_.__z << '}';
    return out_;
  }

  std::istream & 
  operator>> (std::istream & in_, box & b_)
  {
    b_.reset ();
    char c = 0;
    in_.get (c);
    if (c != '{') 
      {
	in_.clear (std::ios_base::failbit);
	return in_;
      } 
    std::string name;
    in_ >> name;
    if (name != box::BOX_LABEL) 
      {
	in_.clear (std::ios_base::failbit);
	return in_;
      } 
    double x, y, z;
    in_ >> x >> y >> z;
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
	b_.set (x,y,z);
      }
    catch (...)
      {
	b_.reset ();
	in_.clear (std::ios_base::failbit);
      }
    return in_;
  }

} // end of namespace geomtools

// end of box.cc

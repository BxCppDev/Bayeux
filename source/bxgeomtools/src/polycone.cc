// -*- mode: c++ ; -*- 
/* polycone.cc
 */

#include <geomtools/polycone.h>

namespace geomtools {
 
  using namespace std;
  
  const string polycone::POLYCONE_LABEL = "polycone";

  string polycone::get_shape_name () const
  {
    return POLYCONE_LABEL;
  }

  double polycone::get_z_min () const
  {
    return __z_min;
  }

  double polycone::get_z_max () const
  {
    return __z_max;
  }

  double polycone::get_r_max () const
  {
    return __r_max;
  }

  const polycone::rz_col_t & polycone::points () const
  {
    return __points;
  }
 
  // ctor:
  polycone::polycone ()
  {
    reset ();
  }
  
  // dtor:
  polycone::~polycone ()
  {
  }

  void polycone::__compute_all ()
  {
   __compute_surfaces ();
   __compute_volume ();
   __compute_limits ();
  }

  void polycone::add (double z_, double r_)
  {
   if (r_ < 0.0)
      {
	throw runtime_error ("polycone::add: Invalid negative 'r' !");
      }
   __points[z_] = r_;
   __compute_all ();
  }

  bool polycone::is_valid () const
  {
    return __points.size () > 1;
  }
  
  void polycone::reset ()
  {
    __points.clear ();
    __top_surface = numeric_limits<double>::quiet_NaN();
    __bottom_surface = numeric_limits<double>::quiet_NaN();
    __side_surface = numeric_limits<double>::quiet_NaN();
    __volume = numeric_limits<double>::quiet_NaN();
    __z_min = __z_max = __r_max = numeric_limits<double>::quiet_NaN();
  }

  void polycone::__compute_limits ()
  {
    if (! is_valid ()) return;
    __z_min = __z_max = __r_max = numeric_limits<double>::quiet_NaN();    
    for (rz_col_t::const_iterator i = __points.begin ();
	 i != __points.end ();
	 i++)
      {
	double z = i->first;
	double r = i->second;
	if (__z_min != __z_min)
	  {
	    __z_min = z;
	  }
	else if (z < __z_min)
	  {
	    __z_min = z;
	  }
	if (__z_max != __z_max)
	  {
	    __z_max = z;
	  }
	else if (z > __z_max)
	  {
	    __z_max = z;
	  }
	if (__r_max != __r_max)
	  {
	    __r_max = r;
	  }
	else if (r > __r_max)
	  {
	    __r_max = r;
	  }
      }
    return;
  }

  void polycone::__compute_surfaces ()
  {
    if (! is_valid ()) return;
    rz_col_t::const_iterator i = __points.begin ();
    double z0 = i->first;
    double r0 = i->second;
    __bottom_surface = M_PI * r0 * r0;
    double s = 0.0;
    while (i != __points.end ())
      {
	rz_col_t::const_iterator j = i;
	j++;
	double z1 = j->first;
	double r1 = j->second;
	double za, zb, ra, rb;
	ra = r0;
	za = z0;
	rb = r1;
	zb = z1;
	if (ra < rb)
	  {
	    swap (ra, rb);
	    swap (za, zb);
	  }
	double zh = (ra *zb - rb * za) / (ra - rb);
	double ha = zh - za;
	double hb = zh - zb;
	double sa = sqrt (ra * ra + ha * ha);
	double sb = sqrt (rb * rb + hb * hb);
	double Sa = M_PI * ra * sa;
	double Sb = M_PI * rb * sb;
	s += (Sa - Sb);
	i++;
	__top_surface = M_PI * r1 * r1;
      }
    __side_surface = s;
   }
  
  void polycone::__compute_volume ()
  {
    if (! is_valid ()) return;
    rz_col_t::const_iterator i = __points.begin ();
    double z0 = i->first;
    double r0 = i->second;
    double v = 0.0;
    while (i != __points.end ())
      {
	rz_col_t::const_iterator j = i;
	j++;
	double z1 = j->first;
	double r1 = j->second;
	double za, zb, ra, rb;
	ra = r0;
	za = z0;
	rb = r1;
	zb = z1;
	if (ra < rb)
	  {
	    swap (ra, rb);
	    swap (za, zb);
	  }
	double zh = (ra *zb - rb * za) / (ra - rb);
	double ha = zh - za;
	double hb = zh - zb;
	double Va = M_PI * ra * ra * ha / 3.0;
	double Vb = M_PI * rb * rb * hb / 3.0;
	v += (Va - Vb);
	i++;
      }
    __volume = v;
  }
 
  double polycone::get_surface (int mask_) const
  {
    double s = 0.0;
    int mask = mask_;
    if (mask_ == (int) ALL_SURFACES) mask = FACE_ALL;
    if (mask & FACE_SIDE) 
      {
	s += __side_surface;
      }
    if (mask & FACE_BOTTOM) 
      {
	s += __bottom_surface;
      }
    if (mask & FACE_TOP) 
      {
	s += __top_surface;
      }
    return s;
   }

  double polycone::get_volume () const
  {
    return __volume;
  }

  double polycone::get_parameter ( const string & flag_ ) const
  {
    if ( flag_ == "r_max" ) return get_r_max ();
    if ( flag_ == "z_min" ) return get_z_min ();
    if ( flag_ == "z_max" ) return get_z_max ();
    if ( flag_ == "volume" ) return get_volume ();
    if ( flag_ == "surface.top" ) return get_surface (FACE_TOP);
    if ( flag_ == "surface.bottom" ) return get_surface (FACE_BOTTOM);
    if ( flag_ == "surface.side" ) return get_surface (FACE_SIDE);
    if ( flag_ == "surface" ) return get_surface (FACE_ALL);

    throw runtime_error ("polycone::get_parameter: Unknown flag!");
  }

  bool polycone::is_inside (const vector_3d & point_, 
			    double skin_) const
  {
    double skin = get_skin ();
    if (skin_ > USING_PROPER_SKIN) skin = skin_;
    
    double z = point_.z ();
    if (z > get_z_max () + 0.5 * skin) return false;
    if (z < get_z_min () - 0.5 * skin) return false;
    double r = hypot (point_.x (), point_.y ());
    if (r > get_r_max () + 0.5 * skin) return false;
    for (rz_col_t::const_iterator i = __points.begin ();
	 i != __points.end ();
	 i++)
      {
	double z1 = i->first;
	rz_col_t::const_iterator j = i;
	j++;
	if (j == __points.end ())
	  {
	    break;
	  }
	double z2 = j->first;
	if ((z >= z1) && (z <= z2))
	  {
	    double r1 = i->second;
	    double r2 = j->second;
	    double alpha = atan2 (r2 - r1, z2 - z1);
	    double epsilon = skin / cos (alpha);
	    double rs = r1 + (r2 - r1) * (z2 - z1) / ( z - z1);
	    if (r < (rs + 0.5 * epsilon))
	      {
		return true;
	      }
	    break;
	  } 
      }
    return false;
  }

  vector_3d polycone::get_normal_on_surface (const vector_3d & position_) const
  {
    vector_3d normal;
    invalidate (normal);
    double x = position_.x ();
    double y = position_.y ();
    double z = position_.z ();
    if (is_on_surface (position_, FACE_BOTTOM)) normal.set (0.0, 0.0, -1.0);
    else if (is_on_surface (position_, FACE_TOP)) normal.set (0.0, 0.0, +1.0); 
    else if (is_on_surface (position_, FACE_SIDE)) 
      {
	for (rz_col_t::const_iterator i = __points.begin ();
	     i != __points.end ();
	     i++)
	  {
	    double z1 = i->first;
	    rz_col_t::const_iterator j = i;
	    j++;
	    if (j == __points.end ())
	      {
		break;
	      }
	    double z2 = j->first;
	    if ((z >= z1) && (z <= z2))
	      {
		double r1 = i->second;
		double r2 = j->second;
		double alpha = atan2 (r2 - r1, z2 - z1);
		double phi = atan2 (y, x);
		double theta = alpha + 0.5 * M_PI;
		make_phi_theta (normal, phi, theta);
		break;
	      }
	  } // for
      }
    return normal;
  }

  bool polycone::is_on_surface (const vector_3d & point_ , 
				int    mask_ ,
				double skin_) const
  {
    double skin = get_skin ();
    if (skin_ > USING_PROPER_SKIN) skin = skin_;
    double z = point_.z ();
    double r = hypot (point_.x (), point_.y ());
    double hskin = 0.5 * skin;

    int mask = mask_;
    if (mask_ == (int) ALL_SURFACES) mask = FACE_ALL;

    if (mask & FACE_BOTTOM) 
      {
	double zbottom = __points.begin ()->first;
	double rbottom = __points.begin ()->second;
	if ((abs(z - zbottom) < hskin) 
	    && (r < (rbottom + hskin))) return true;
      } 

    if (mask & FACE_TOP) 
      {
	double ztop = __points.rbegin ()->first;
	double rtop = __points.rbegin ()->second;
	if ((abs(z - ztop) < hskin) 
	    && (r < (rtop + hskin))) return true;
      } 
    
    if (mask & FACE_SIDE) 
      {
	for (rz_col_t::const_iterator i = __points.begin ();
	     i != __points.end ();
	     i++)
	  {
	    double z1 = i->first;
	    rz_col_t::const_iterator j = i;
	    j++;
	    if (j == __points.end ())
	      {
		break;
	      }
	    double z2 = j->first;

	    if ((z >= z1) && (z <= z2))
	      {
		double r1 = i->second;
		double r2 = j->second;
		double alpha = atan2 (r2 - r1, z2 - z1);
		double epsilon = skin / cos (alpha);
		double rs = r1 + (r2 - r1) * (z2 - z1) / ( z - z1);
		if (abs(r - rs) < 0.5 * epsilon)
		  {
		    return true;
		  }
		break;
	      } 
	  }
      }
    return false;
  }
      
  bool polycone::find_intercept (const vector_3d & from_, 
				 const vector_3d & direction_,
				 intercept_t & intercept_,
				 double skin_) const
  {
    throw runtime_error ("polycone::find_intercept: Not implemented yet !");
    return false;
  }

  ostream & operator<< (ostream & out_, const polycone & p_)
  {
    out_ << '{' << polycone::POLYCONE_LABEL;
    out_ << ' ' << p_.__points.size ();
    for (polycone::rz_col_t::const_iterator i = p_.__points.begin ();
	 i != p_.__points.end ();
	 i++)
      {
	double z = i->first;
	double r = i->second;
	out_ << ' ' << z << ' '<< r;
      }
    out_ << '}';
    return out_;
  }

  istream & operator>> (istream & in_, polycone & p_)
  {
    p_.reset ();
    char c = 0;
    in_.get (c);
    if (c != '{') 
      {
	in_.clear (ios_base::failbit);
	return in_;
      } 
    string name;
    in_ >> name;
    if (name != polycone::POLYCONE_LABEL) 
      {
	in_.clear (ios_base::failbit);
	return in_;
      } 
    size_t n;
    in_ >> n;
    for (int i = 0; i < n; i++)
      {
	double r, z;
	in_ >> r >> z;
	if (! in_) 
	  {
	    in_.clear (ios_base::failbit);
	    return in_;
	  } 
	try 
	  {
	    p_.add (r, z);
	  }
	catch (...)
	  {
	    p_.reset ();
	    in_.clear (ios_base::failbit);
	  }	
      }
    c = 0;
    in_.get (c);
    if (c != '}') 
      {
	in_.clear (ios_base::failbit);
	return in_;
      } 
    return in_;
  }

  void polycone::tree_dump (ostream & out_, 
			    const string & title_, 
			    const string & indent_, 
			    bool inherit_) const
  {
    namespace du = datatools::utils;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    i_object_3d::tree_dump (out_, title_, indent_, true);

    out_ << indent << du::i_tree_dumpable::tag 
	 << "Z(min) : " << get_z_min () / CLHEP::mm << " mm" << endl;
    out_ << indent << du::i_tree_dumpable::tag 
	 << "Z(max) : " << get_z_max () / CLHEP::mm << " mm" << endl;
    out_ << indent << du::i_tree_dumpable::tag 
	 << "R(max) : " << get_r_max () / CLHEP::mm << " mm" << endl;
    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_) 
	 << "Number of points : " << __points.size () << endl;
    return;
  }
  
} // end of namespace geomtools

// end of polycone.cc

// -*- mode: c++ ; -*- 
/* polyhedra.cc
 */

#include <geomtools/polyhedra.h>

namespace geomtools {
 
  using namespace std;
  
  const string polyhedra::POLYHEDRA_LABEL = "polyhedra";

  string polyhedra::get_shape_name () const
  {
    return POLYHEDRA_LABEL;
  }

  void polyhedra::set_n_sides (size_t n_sides_)
  {
    if (n_sides_ < 3)
      {
	ostringstream message;
	message << "polyhedra::set_n_sides: " 
		<< "Invalid number of sides '" << n_sides_ << "'" << endl;
	throw runtime_error (message.str ());
      }
    __n_sides = n_sides_;
    return;
  }

  size_t polyhedra::get_n_sides () const
  {
    return __n_sides;
  }

  double polyhedra::get_z_min () const
  {
    return __z_min;
  }

  double polyhedra::get_z_max () const
  {
    return __z_max;
  }

  double polyhedra::get_r_max () const
  {
    return __r_max;
  }

  const polyhedra::rz_col_t & polyhedra::points () const
  {
    return __points;
  }
 
  // ctor:
  polyhedra::polyhedra ()
  {
    reset ();
  }
  
  // dtor:
  polyhedra::~polyhedra ()
  {
  }

  void polyhedra::initialize (const string & filename_)
  {
    ifstream ifs;
    string filename = filename_;
    ifs.open (filename.c_str ());
    if (! ifs)
      {
	ostringstream message;
	message << "polyhedra::initialize: " 
		<< "Cannot open data file '"
		<< filename << "' !";
	throw runtime_error (message.str ()); 
      }
    size_t count = 0;
    double length_unit = CLHEP::mm;
    double z_factor = 1.0;
    double r_factor = 1.0;
    size_t n_sides = 0;
    while (! ifs.eof ())
      {
	string line;
	getline (ifs, line);
	/*
        cerr << "DEVEL: polyhedra::initialize: " 
	     << "line = '" << line << "'" << endl;
	*/
	count++;
	// Special lines:
	{
	  istringstream iss (line);
	  string  word;
	  iss >> word;
	  if (word.empty ()) 
	    {
	      /*
	      cerr << "DEVEL: " <<  "polyhedra::initialize: " 
		   << "Blank line !" << endl;
	      */
	      continue;
	    }
	  if (word[0] == '#') 
	    {
	      /*
	      cerr << "DEVEL: " <<  "polyhedra::initialize: " 
		   << "Comment line !" << endl;
	      */
	      if (word.size () >= 2)
		{
		  if (word == "#@length_unit")
		    {
		      string unit_str;
		      iss >> unit_str;
		      if (! iss)
			{
			  ostringstream message;
			  message << "polyhedra::initialize: " 
				  << "Invalid format for the length unit directive in data file '"
				  << filename << "' at line " << count << " !";
			  throw runtime_error (message.str ()); 
			}
		      length_unit = datatools::utils::units::get_length_unit_from (unit_str);
		    }
		  else if (word == "#@z_factor")
		    {
		      double z_factor;
		      iss >> z_factor;
		      if (! iss)
			{
			  ostringstream message;
			  message << "polyhedra::initialize: " 
				  << "Invalid format for the Z-factor directive in data file '"
				  << filename << "' at line " << count << " !";
			  throw runtime_error (message.str ()); 
			}
		    }
		  else if (word == "#@r_factor")
		    {
		      double r_factor;
		      iss >> r_factor;
		      if (! iss)
			{
			  ostringstream message;
			  message << "polyhedra::initialize: " 
				  << "Invalid format for the R-factor directive in data file '"
				  << filename << "' at line " << count << " !";
			  throw runtime_error (message.str ()); 
			}
		    }
		}
	      continue;
	    }
	}
	// Data lines:
	{
	  istringstream iss (line);
	  if (n_sides < 3)
	    {
	      iss >> n_sides;
	      this->set_n_sides (n_sides);
	      /*
	      cerr << "DEVEL: " <<  "polyhedra::initialize: " 
		   << "n_sides= " << __n_sides << endl;
	      */
	    }
	  else
	    {
	      double z, r;
	      iss >> z >> r;
	      if (! iss)
		{
		  ostringstream message;
		  message << "polyhedra::initialize: " 
			  << "Format error in data file '"
			  << filename << "' at line " << count << " !";
		  throw runtime_error (message.str ()); 
		  
		}
	      /*
	      cerr << "DEVEL: polyhedra::initialize: " 
		   << "Adding z = " << z << " " 
		   << "r = " << r << " " << endl;
	      */
	      this->add (z * z_factor * length_unit, r * r_factor * length_unit);
	      
	    }
	}
      }
    return;
  }

  void polyhedra::__compute_all ()
  {
   __compute_surfaces ();
   __compute_volume ();
   __compute_limits ();
  }

  void polyhedra::add (double z_, double r_)
  {
   if (r_ < 0.0)
      {
	throw runtime_error ("polyhedra::add: Invalid negative 'r' !");
      }
   __points[z_] = r_;
   __compute_all ();
  }

  bool polyhedra::is_valid () const
  {
    return __points.size () > 1;
  }
  
  void polyhedra::reset ()
  {
    __n_sides = 0;
    __points.clear ();
    __top_surface = numeric_limits<double>::quiet_NaN();
    __bottom_surface = numeric_limits<double>::quiet_NaN();
    __side_surface = numeric_limits<double>::quiet_NaN();
    __volume = numeric_limits<double>::quiet_NaN();
    __z_min = __z_max = __r_max = numeric_limits<double>::quiet_NaN ();
  }

  void polyhedra::__compute_limits ()
  {
    if (! is_valid ()) return;
    __z_min = __z_max = __r_max = numeric_limits<double>::quiet_NaN ();    
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

  void polyhedra::__compute_surfaces ()
  {
    if (! is_valid ()) return;
    rz_col_t::const_iterator i = __points.begin ();
    double z0 = i->first;
    double r0 = i->second;
    regular_polygon bottom_polygon (__n_sides, r0);
    __bottom_surface = bottom_polygon.get_surface ();
    double s = 0.0;
    rz_col_t::const_iterator j = i;
    j++;
    while (j != __points.end ())
      {
	double z1 = j->first;
	double r1 = j->second;
	// See: http://en.wikipedia.org/wiki/Frustum#Surface_Area
	size_t n = __n_sides;
	double angle = M_PI / n;
	double a1 = 2 * r0 * sin (angle); 
	double a2 = 2 * r1 * sin (angle); 
	double a1_2 = a1 * a1;
	double a2_2 = a2 * a2;
	double h = abs (z1 - z0);
	double A = 0.25 
	  * M_PI 
	  * (
	     (a1_2 + a2_2) / tan (angle) 
	     + 
	     sqrt ((a1_2 - a2_2) * (a1_2 - a2_2) / (cos (angle) * cos (angle))
		   + 4 * h * h * (a1_2 + a2_2) * (a1_2 + a2_2)));
	s += A;
	// increment:
	j++;
	z0 = z1;
	r0 = r1;
      }
    regular_polygon top_polygon (__n_sides, r0);
    __top_surface = top_polygon.get_surface ();
    __side_surface = s;
  }
  
  void polyhedra::__compute_volume ()
  {
    if (! is_valid ()) return;
    rz_col_t::const_iterator i = __points.begin ();
    double z0 = i->first;
    double r0 = i->second;
    double v = 0.0;
    rz_col_t::const_iterator j = i;
    j++;
    while (j != __points.end ())
      {
	double z1 = j->first;
	double r1 = j->second;
	// See: http://en.wikipedia.org/wiki/Frustum# Volume
	size_t n = __n_sides;
	double angle = M_PI / n;
	double a1 = 2 * r0 * sin (angle); 
	double a2 = 2 * r1 * sin (angle); 
	double h = abs (z1 - z0);
	double V = n * h * (a1 * a1 + a2 * a2 + a1 * a2) / tan (angle) / 12;
	v += V;
	// increment:
	j++;
	z0 = z1;
	r0 = r1;
      }
    __volume = v;
    return;
  }
  /*

    if (! is_valid ()) return;
    datatools::utils::invalidate (__volume);
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
	size_t n = __n_sides;
	double angle = M_PI / n;
	double a1 = 2 * ra * sin (angle); 
	double a2 = 2 * rb * sin (angle); 
	double h = abs (zb - za);
	double V = n * h * (a1 * a1 + a2 * a2 + a1 * a2) / tan (angle) / 12;
	v += V;
	if (j == __points.end ()) 
	  {
	    break;
	  }
	// increment:
	i++;
	z0 = z1;
	r0 = r1;
      }
    __volume = v;
  }
  */
 
  double polyhedra::get_surface (int mask_) const
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

  double polyhedra::get_volume () const
  {
    return __volume;
  }

  double polyhedra::get_parameter ( const string & flag_ ) const
  {
    if ( flag_ == "n_sides" ) return get_n_sides ();
    if ( flag_ == "r_max" ) return get_r_max ();
    if ( flag_ == "z_min" ) return get_z_min ();
    if ( flag_ == "z_max" ) return get_z_max ();
    if ( flag_ == "volume" ) return get_volume ();
    if ( flag_ == "surface.top" ) return get_surface (FACE_TOP);
    if ( flag_ == "surface.bottom" ) return get_surface (FACE_BOTTOM);
    if ( flag_ == "surface.side" ) return get_surface (FACE_SIDE);
    if ( flag_ == "surface" ) return get_surface (FACE_ALL);

    throw runtime_error ("polyhedra::get_parameter: Unknown flag!");
  }

  bool polyhedra::is_inside (const vector_3d & point_, 
			    double skin_) const
  {
    double skin = get_skin ();
    if (skin_ > USING_PROPER_SKIN) skin = skin_;
    
    double z = point_.z ();
    if (z > get_z_max () + 0.5 * skin) return false;
    if (z < get_z_min () - 0.5 * skin) return false;
    double r = hypot (point_.x (), point_.y ());
    if (r > get_r_max () + 0.5 * skin) return false;
    /*
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
    */
    return false;
  }

  vector_3d polyhedra::get_normal_on_surface (const vector_3d & position_) const
  {
    vector_3d normal;
    invalidate (normal);
    /*
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
    */
    return normal;
  }

  bool polyhedra::is_on_surface (const vector_3d & point_ , 
				int    mask_ ,
				double skin_) const
  {
    double skin = get_skin ();
    if (skin_ > USING_PROPER_SKIN) skin = skin_;

    /*
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
    */
    return false;
  }
      
  bool polyhedra::find_intercept (const vector_3d & from_, 
				 const vector_3d & direction_,
				 intercept_t & intercept_,
				 double skin_) const
  {
    throw runtime_error ("polyhedra::find_intercept: Not implemented yet !");
    return false;
  }

  ostream & operator<< (ostream & out_, const polyhedra & p_)
  {
    out_ << '{' << polyhedra::POLYHEDRA_LABEL;
    out_ << ' ' << p_.__n_sides;
    out_ << ' ' << p_.__points.size ();
    for (polyhedra::rz_col_t::const_iterator i = p_.__points.begin ();
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

  istream & operator>> (istream & in_, polyhedra & p_)
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
    if (name != polyhedra::POLYHEDRA_LABEL) 
      {
	in_.clear (ios_base::failbit);
	return in_;
      } 
    size_t n_sides;
    in_ >> n_sides;
    p_.set_n_sides (n_sides);
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

  void polyhedra::tree_dump (ostream & out_, 
			    const string & title_, 
			    const string & indent_, 
			    bool inherit_) const
  {
    namespace du = datatools::utils;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    i_object_3d::tree_dump (out_, title_, indent_, true);

    out_ << indent << du::i_tree_dumpable::tag 
	 << "N(sides) : " << get_n_sides () << endl;
    out_ << indent << du::i_tree_dumpable::tag 
	 << "Z(min) : " << get_z_min () / CLHEP::mm << " mm" << endl;
    out_ << indent << du::i_tree_dumpable::tag 
	 << "Z(max) : " << get_z_max () / CLHEP::mm << " mm" << endl;
    out_ << indent << du::i_tree_dumpable::tag 
	 << "R(max) : " << get_r_max () / CLHEP::mm << " mm" << endl;
    out_ << indent << du::i_tree_dumpable::tag 
	 << "Number of points : " << __points.size () << endl;
    out_ << indent << du::i_tree_dumpable::tag 
	 << "Volume : " << get_volume () / CLHEP::cm3 << " cm3" << endl;
    out_ << indent << du::i_tree_dumpable::tag 
	 << "Bottom surface : " << get_surface (FACE_BOTTOM) / CLHEP::cm2 << " cm2" << endl;
    out_ << indent << du::i_tree_dumpable::tag 
	 << "Top surface : " << get_surface (FACE_TOP) / CLHEP::cm2 << " cm2" << endl;
    out_ << indent << du::i_tree_dumpable::tag 
	 << "Side surface : " << get_surface (FACE_SIDE) / CLHEP::cm2 << " cm2" << endl;
    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_) 
	 << "Total surface : " << get_surface (FACE_ALL) / CLHEP::cm2 << " cm2" << endl;
    return;
  }
  
} // end of namespace geomtools

// end of polyhedra.cc

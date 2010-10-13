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

  void polycone::__build_from_envelope_and_skin (double skin_thickness_, 
						 double skin_step_)
  {
    bool devel = false;
    devel = true;
    if (devel)
      {
	clog << "DEVEL: " << "polycone::__build_from_envelope_and_skin: "
	     << "Entering..." << endl;
      }
    if (skin_thickness_ < 0.0)
      {
	throw runtime_error ("polycone::__build_from_envelope_and_skin: Invalid skin thickness !");
      }

    if (__points.size () < 2)
      {
	throw runtime_error ("polycone::__build_from_envelope_and_skin: Not enough points !");
      }
    if (devel)
      {
	clog << "DEVEL: " << "polycone::__build_from_envelope_and_skin: "
	     << "Building interpolated primary outer envelope..." << endl;
      }
    string interpolation_mode = "akima";
    mygsl::tabulated_function tf (interpolation_mode);
    for (rz_col_t::const_iterator i = __points.begin ();
	 i != __points.end ();
	 i++)
      {
	double z = i->first;
	double rmax = i->second.rmax;
	tf.add_point (z, rmax);
      }
    tf.lock_table (); 

    // thickness of the polycone's skin:
    double skin = skin_thickness_;

    // compute the recommended sampling step along the envelope:
    double dx = 5 * skin; // default
    if (skin_step_ > 0.0)
      {
	dx = skin_step_;
      }
    if (devel)
      {
	clog << "DEVEL: " << "polycone::__build_from_envelope_and_skin: "
	     << "Building interpolated inner envelope..." << endl;
      }
    //mygsl::tabulated_function tf2 (interpolation_mode); 
    mygsl::tabulated_function tf3 (interpolation_mode); 
    for (mygsl::tabulated_function::points_map_t::const_iterator 
	   i = tf.points ().begin ();
	 i != tf.points ().end ();
	 i++)
      {
	mygsl::tabulated_function::points_map_t::const_iterator j = i;
	double xi = i->first;
	double yi = i->second;
	j++;
	mygsl::interval domain (tf.x_min (), tf.x_max (), 0.05 * skin);
	double drdz = mygsl::derivative (tf, xi, domain);
	double ux = 1.0;
	double uy = drdz;
	double dyij = skin / sqrt (1 + uy * uy);
	double dxij = - dyij * uy;
	double xj = xi + dxij;
	double yj = yi + dyij;
	double xk = xi - dxij;
	double yk = yi - dyij;
	//tf2.add_point (xj, yj, false);
	tf3.add_point (xk, yk, false);
      }
    //tf2.lock_table ();
    tf3.lock_table ();
    
    // Manage bounds:
    if (devel)
      {
	clog << "DEVEL: " << "polycone::__build_from_envelope_and_skin: "
	     << "Building properly bounded interpolated inner envelope..." << endl;
      }
    mygsl::tabulated_function tf3bis (interpolation_mode); 
    for (mygsl::tabulated_function::points_map_t::const_iterator 
	   i = tf.points ().begin ();
	 i != tf.points ().end ();
	 i++)
      {
	double x = i->first;
	double y;
	if (tf3.is_valid (x)) 
	  {
	    y = tf3 (x);
	  }
	else
	  {
	    if (x < tf3.x_min ())
	      {
		double df3dx = mygsl::derivative_forward (tf3, tf3.x_min (), 0.05 * dx); 
		y = tf3 (tf3.x_min ()) + (x - tf3.x_min ()) * df3dx;
	      }
	    else //if (x > tf3.x_max ())
	      {
		double df3dx = mygsl::derivative_backward (tf3, tf3.x_max (), 0.05 * dx); 
		y = tf3 (tf3.x_max ()) + (x - tf3.x_max ()) * df3dx;
	      }
	  }
	tf3bis.add_point (x, y, false);
      }
    tf3bis.lock_table ();
    
    if (devel)
      {
	clog << "DEVEL: " << "polycone::__build_from_envelope_and_skin: "
	     << "Building final interpolated outer and inner envelopes with requested Z sampling..." << endl;
      }
    mygsl::tabulated_function tf_outer ("linear"); 
    mygsl::tabulated_function tf_inner ("linear"); 
    double z = tf.x_min ();
    double dz = dx;
    double za, ra;
    datatools::utils::invalidate (za);
    while (z < tf.x_max () + 0.1 * dz)
      {
	if (z > tf.x_max ()) z = tf.x_max ();
	mygsl::interval domain (tf.x_min (), tf.x_max (), 0.05 * skin);
	double drdz = mygsl::derivative (tf, z, domain);
	dz = dx  / sqrt ( 1 + drdz * drdz);
	if (dz > dx) 
	  {
	    dz = dx;
	  }
	else if (dz < 0.25* skin)
	  {
	    dz = 0.25 * skin;
	  }
	double r_outer = tf (z);
	if (r_outer < 0.0) // should not occur
	  {
	    r_outer = 0.0;
	  }
	double r_inner = tf3bis (z);
	if (datatools::utils::is_valid (za))
	  {
	    double zb = z;
	    double rb = r_inner;
	    if ((ra * rb) < 0.0)
	      {
		double a = (rb - ra)  / (zb - za);
		double b = rb - a * zb;
		double z_r = -b / a;
		double rmin_r = 0.0;
		double rmax_r = tf (z_r);
		tf_outer.add_point (z_r, rmax_r, false);
		tf_inner.add_point (z_r, rmin_r, false);
	      }
	  }
	za = z;
	ra = r_inner;
	if (r_inner < 0.0)
	  {
	    r_inner = 0.0;
	  }
	tf_outer.add_point (z, r_outer, false);
	tf_inner.add_point (z, r_inner, false);
	z += dz;
      }
    // lock interpolators:
    tf_outer.lock_table ();
    tf_inner.lock_table (); 

    __points.clear ();
    mygsl::tabulated_function::points_map_t::const_iterator i = tf_outer.points ().begin ();
    mygsl::tabulated_function::points_map_t::const_iterator j = tf_inner.points ().begin ();
    for (int k = 0; k < tf_outer.points ().size (); k++)
      {
	double z = i->first;
	double rmin = j->second;
	if (rmin < 0.0) // should not occur
	  {
	    rmin = 0.0;
	  }
	double rmax = i->second;
	this->add (z, rmin, rmax);
	i++;
	j++;
      }
    return;
  }

  void polycone::initialize (const string & filename_)
  {
    ifstream ifs;
    string filename = filename_;
    ifs.open (filename.c_str ());
    if (! ifs)
      {
	ostringstream message;
	message << "polycone::initialize: " 
		<< "Cannot open data file '"
		<< filename << "' !";
	throw runtime_error (message.str ()); 
      }
    size_t count = 0;
    double length_unit = CLHEP::mm;
    double z_factor = 1.0;
    double r_factor = 1.0;
    double skin_thickness;
    datatools::utils::invalidate (skin_thickness);
    double skin_step = 0.0;
    while (! ifs.eof ())
      {
	string line;
	getline (ifs, line);
	//cerr << "DEVEL: polycone::initialize: " << "line = '" << line << "'" << endl;
	count++;
	{
	  istringstream iss (line);
	  string  word;
	  iss >> word;
	  if (word.empty ()) continue;
	  if (word[0] == '#') 
	    {
	      if (word.size () >= 2)
		{
		  if (word == "#@length_unit")
		    {
		      string unit_str;
		      iss >> unit_str;
		      if (! iss)
			{
			  ostringstream message;
			  message << "polycone::initialize: " 
				  << "Invalid format for the length unit directive in data file '"
				  << filename << "' at line " << count << " !";
			  throw runtime_error (message.str ()); 
			}
		      length_unit = datatools::utils::units::get_length_unit_from (unit_str);
		    }
		  else if (word == "#@z_factor")
		    {
		      iss >> z_factor;
		      if (! iss)
			{
			  ostringstream message;
			  message << "polycone::initialize: " 
				  << "Invalid format for the Z-factor directive in data file '"
				  << filename << "' at line " << count << " !";
			  throw runtime_error (message.str ()); 
			}
		    }
		  else if (word == "#@r_factor")
		    {
		      iss >> r_factor;
		      if (! iss)
			{
			  ostringstream message;
			  message << "polycone::initialize: " 
				  << "Invalid format for the R-factor directive in data file '"
				  << filename << "' at line " << count << " !";
			  throw runtime_error (message.str ()); 
			}
		    }
		  else if (word == "#@skin_thickness")
		    {
		      iss >> skin_thickness;
		      if (! iss)
			{
			  ostringstream message;
			  message << "polycone::initialize: " 
				  << "Invalid format for the skin_thickness directive in data file '"
				  << filename << "' at line " << count << " !";
			  throw runtime_error (message.str ()); 
			}
		    }
		  else if (word == "#@skin_step")
		    {
		      iss >> skin_step;
		      if (! iss)
			{
			  ostringstream message;
			  message << "polycone::initialize: " 
				  << "Invalid format for the skin_thickness directive in data file '"
				  << filename << "' at line " << count << " !";
			  throw runtime_error (message.str ()); 
			}
		    }
		}
	      continue;
	    }
	}
	{
	  istringstream iss (line);
	  double z, r, r2;
	  datatools::utils::invalidate (r2);
	  iss >> z;
	  if (! iss)
	    {
	      ostringstream message;
	      message << "polycone::initialize: " 
		      << "Format error for Z in data file '"
		      << filename << "' at line " << count << " !";
	      throw runtime_error (message.str ()); 
	    }
	  iss >> r;
	  if (! iss)
	    {
	      ostringstream message;
	      message << "polycone::initialize: " 
		      << "Format error for 'r' in data file '"
		      << filename << "' at line " << count << " !";
	      throw runtime_error (message.str ()); 
	    }
	  if (r < 0.0)
	    {
	      ostringstream message;
	      message << "polycone::initialize: " 
		      << "Invalid value '" << r << "' for 'r' in data file '"
		      << filename << "' at line " << count << " !";
	      throw runtime_error (message.str ()); 
	    }
	  string token;
	  iss >> token;
	  if (datatools::utils::is_valid (skin_thickness))
	    {
	      if (token.length () > 0)
		{
		  ostringstream message;
		  message << "polycone::initialize: " 
			  << "Invalid format for 'z r' pair "
			  << "in 'skin_thickness' mode from data file '"
			  << filename << "' at line " << count << " !";
		  throw runtime_error (message.str ()); 
		}
	    }
	  else
	    {
	      if (token.length () > 0)
		{
		  istringstream iss2 (token);
		  iss2 >> r2;
		  if (! iss2)
		    {
		      ostringstream message;
		      message << "polycone::initialize: " 
			      << "Format error for 'r2' in data file '"
			      << filename << "' at line " << count << " !";
		      throw runtime_error (message.str ()); 
		    }	    
		  if (r2 < 0.0)
		    {
		      ostringstream message;
		      message << "polycone::initialize: " 
			      << "Invalid value '" << r2 << "' for 'r2' in data file '"
			      << filename << "' at line " << count << " !";
		      throw runtime_error (message.str ()); 
		    }
		}
	    }
	  
	  cerr << "DEVEL: polycone::initialize: " 
	     << "z = " << z << " " 
	     << "r = " << r << " "
	     << "r2 = " << r2 << " " << endl;
	  
	  if (datatools::utils::is_valid (r2))
	    {
	      this->add (z * z_factor * length_unit, 
			 r * r_factor * length_unit, 
			 r2 * r_factor * length_unit);
	    }
	  else
	    {
	      this->add (z * z_factor * length_unit, 
			 r * r_factor * length_unit);
	    }
	}
      }
    if (datatools::utils::is_valid (skin_thickness))
      {
	__build_from_envelope_and_skin (skin_thickness, skin_step);
      }
   return;
  }

  void polycone::__compute_all ()
  {
   __compute_surfaces ();
   __compute_volume ();
   __compute_limits ();
  }

  void polycone::add (double z_, double rmax_)
  {
   if (rmax_ < 0.0)
      {
	throw runtime_error ("polycone::add: Invalid negative 'rmax' !");
      }
   r_min_max RMM;
   RMM.rmin = 0.0;
   RMM.rmax = rmax_;
   __points[z_] = RMM;
   if (__points.size () > 1)
     {
       __compute_all ();
     }
  }

  void polycone::add (double z_, double rmin_,  double rmax_)
  {
   if (rmin_ < 0.0)
      {
	throw runtime_error ("polycone::add: Invalid negative 'rmin' !");
      }
   if (rmax_ < rmin_)
      {
	throw runtime_error ("polycone::add: Invalid value for 'rmax' !");
      }
   r_min_max RMM;
   RMM.rmin = rmin_;
   RMM.rmax = rmax_;
   __points[z_] = RMM;
   if (__points.size () > 1)
     {
       __compute_all ();
     }
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
    __inner_side_surface = numeric_limits<double>::quiet_NaN();
    __outer_side_surface = numeric_limits<double>::quiet_NaN();
    __inner_volume = numeric_limits<double>::quiet_NaN();
    __outer_volume = numeric_limits<double>::quiet_NaN();
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
	double rmax = i->second.rmax;
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
	    __r_max = rmax;
	  }
	else if (rmax > __r_max)
	  {
	    __r_max = rmax;
	  }
      }
    return;
  }

  void polycone::__compute_surfaces ()
  {
    if (! is_valid ()) return;
    rz_col_t::const_iterator i = __points.begin ();
    double z0 = i->first;
    double rmin0 = i->second.rmin;
    double rmax0 = i->second.rmax;
    __bottom_surface = M_PI * (rmax0 * rmax0 - rmin0 * rmin0);
    double s = 0.0;
    rz_col_t::const_iterator j = i;
    j++;
    while (j != __points.end ())
      {
	double z1 = j->first;
	double rmin1 = j->second.rmin;
	double rmax1 = j->second.rmax;
	// See: http://en.wikipedia.org/wiki/Frustum#Surface_Area
	double R1 = rmax0;
	double R2 = rmax1;
	double R1_2 = R1 * R1;
	double R2_2 = R2 * R2;
	double h = abs (z1 - z0);
	double A = M_PI * (R1_2 + R2_2 + 
			   sqrt ((R1_2 - R2_2) * (R1_2 - R2_2) 
				 + h * h * (R1_2 + R2_2) * (R1_2 + R2_2)));
	s += A;
	// increment:
	j++;
	z0 = z1;
	rmin0 = rmin1;
	rmax0 = rmin1;
      }
    __outer_side_surface = s;
    __top_surface = M_PI * (rmax0 * rmax0 - rmin0 * rmin0);
    while (j != __points.end ())
      {
	double z1 = j->first;
	double rmin1 = j->second.rmin;
	double rmax1 = j->second.rmax;
	// See: http://en.wikipedia.org/wiki/Frustum#Surface_Area
	double R1 = rmin0;
	double R2 = rmin1;
	double R1_2 = R1 * R1;
	double R2_2 = R2 * R2;
	double h = abs (z1 - z0);
	double A = M_PI * (R1_2 + R2_2 + 
			   sqrt ((R1_2 - R2_2) * (R1_2 - R2_2) 
				 + h * h * (R1_2 + R2_2) * (R1_2 + R2_2)));
	s += A;
	// increment:
	j++;
	z0 = z1;
	rmin0 = rmin1;
	rmax0 = rmin1;
      }
    __inner_side_surface = s;
   }
  
  void polycone::__compute_volume ()
  {
    if (! is_valid ()) return;
    double vext = 0.0;
    double vint = 0.0;
    // Outer envelope:
    {
      rz_col_t::const_iterator i = __points.begin ();
      double z0 = i->first;
      double rmax0 = i->second.rmax;
      rz_col_t::const_iterator j = i;
      j++;
      while (j != __points.end ())
	{
	  double z1 = j->first;
	  double r1 = j->second.rmax;
	  // See: http://en.wikipedia.org/wiki/Frustum# Volume
	  double R1 = rmax0;
	  double R2 = r1;
	  double R1_2 = R1 * R1;
	  double R2_2 = R2 * R2;
	  double h = abs (z1 - z0);
	  double V = M_PI * h * (R1_2 + R2_2 + R1 * R2) / 3;
	  vext += V;
	  // increment:
	  j++;
	  z0 = z1;
	  rmax0 = r1;
	}
    }
    // Inner envelope:
    {
      rz_col_t::const_iterator i = __points.begin ();
      double z0 = i->first;
      double rmin0 = i->second.rmin;
      rz_col_t::const_iterator j = i;
      j++;
      while (j != __points.end ())
	{
	  double z1 = j->first;
	  double r1 = j->second.rmax;
	  // See: http://en.wikipedia.org/wiki/Frustum# Volume
	  double R1 = rmin0;
	  double R2 = r1;
	  double R1_2 = R1 * R1;
	  double R2_2 = R2 * R2;
	  double h = abs (z1 - z0);
	  double V = M_PI * h * (R1_2 + R2_2 + R1 * R2) / 3;
	  vint += V;
	  // increment:
	  j++;
	  z0 = z1;
	  rmin0 = r1;
	}
    }
    __outer_volume = vext;
    __inner_volume = vint;
    __volume = __outer_volume - __inner_volume;
    return;
  }
 
  double polycone::get_surface (int mask_) const
  {
    double s = 0.0;
    int mask = mask_;
    if (mask_ == (int) ALL_SURFACES) mask = FACE_ALL;
    if (mask & FACE_INNER_SIDE) 
      {
	s += __inner_side_surface;
      }
    if (mask & FACE_OUTER_SIDE) 
      {
	s += __outer_side_surface;
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
    if ( flag_ == "surface.inner_side" ) return get_surface (FACE_INNER_SIDE);
    if ( flag_ == "surface.outer_side" ) return get_surface (FACE_OUTER_SIDE);
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
	    double r1 = i->second.rmax;
	    double r2 = j->second.rmax;
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
    throw runtime_error ("polycone::get_normal_on_surface: Not implemented yet !");
    vector_3d normal;
    invalidate (normal);
    double x = position_.x ();
    double y = position_.y ();
    double z = position_.z ();
    if (is_on_surface (position_, FACE_BOTTOM)) normal.set (0.0, 0.0, -1.0);
    else if (is_on_surface (position_, FACE_TOP)) normal.set (0.0, 0.0, +1.0); 
    else if (is_on_surface (position_, FACE_OUTER_SIDE)) 
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
		double r1 = i->second.rmax;
		double r2 = j->second.rmax;
		double alpha = atan2 (r2 - r1, z2 - z1);
		double phi = atan2 (y, x);
		double theta = alpha + 0.5 * M_PI;
		make_phi_theta (normal, phi, theta);
		break;
	      }
	  } // for
      }
    else if (is_on_surface (position_, FACE_INNER_SIDE)) 
      {
	//...
      }
    return normal;
  }

  bool polycone::is_on_surface (const vector_3d & point_ , 
				int    mask_ ,
				double skin_) const
  {
    throw runtime_error ("polycone::is_on_surface: Not implemented yet !");
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
	double rbottom = __points.begin ()->second.rmax;
	if ((abs(z - zbottom) < hskin) 
	    && (r < (rbottom + hskin))) return true;
      } 

    if (mask & FACE_TOP) 
      {
	double ztop = __points.rbegin ()->first;
	double rtop = __points.rbegin ()->second.rmax;
	if ((abs(z - ztop) < hskin) 
	    && (r < (rtop + hskin))) return true;
      } 

    if (mask & FACE_INNER_SIDE) 
      {
	throw runtime_error ("polycone::is_on_surface: Not implemented yet !");
      }
    
    if (mask & FACE_OUTER_SIDE) 
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
		double r1 = i->second.rmax;
		double r2 = j->second.rmax;
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
	double rmin = i->second.rmin;
	double rmax = i->second.rmax;
	out_ << ' ' << z << ' ' << rmin << ' ' << rmax;
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
	double rmin, rmax, z;
	in_ >> z >> rmin >> rmax;
	if (! in_) 
	  {
	    in_.clear (ios_base::failbit);
	    return in_;
	  } 
	try 
	  {
	    p_.add (z, rmin, rmax);
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
    out_ << indent << du::i_tree_dumpable::tag 
	 << "Number of points : " << __points.size () << endl;
    out_ << indent << du::i_tree_dumpable::tag 
	 << "Volume : " << get_volume () / CLHEP::cm3 << " cm3" << endl;
    out_ << indent << du::i_tree_dumpable::tag 
	 << "Bottom surface : " << get_surface (FACE_BOTTOM) / CLHEP::cm2 << " cm2" << endl;
    out_ << indent << du::i_tree_dumpable::tag 
	 << "Top surface : " << get_surface (FACE_TOP) / CLHEP::cm2 << " cm2" << endl;
    out_ << indent << du::i_tree_dumpable::tag 
	 << "Outer side surface : " << get_surface (FACE_OUTER_SIDE) / CLHEP::cm2 << " cm2" << endl;
    out_ << indent << du::i_tree_dumpable::tag 
	 << "Inner side surface : " << get_surface (FACE_INNER_SIDE) / CLHEP::cm2 << " cm2" << endl;
    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)
	 << "Total surface : " << get_surface (FACE_ALL) / CLHEP::cm2 << " cm2" << endl;
     return;
  }
  
} // end of namespace geomtools

// end of polycone.cc

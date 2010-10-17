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
    if (n_sides_ < MIN_NUMBER_OF_SIDES)
      {
	ostringstream message;
	message << "polyhedra::set_n_sides: " 
		<< "Invalid number of sides '" << n_sides_ << "'" << endl;
	throw runtime_error (message.str ());
      }
    __n_sides = n_sides_;
    return;
  }

  bool polyhedra::is_extruded () const
  {
    return __extruded;
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

  void polyhedra::initialize (const datatools::utils::properties & setup_)
  {
    string build_mode_label;
    string datafile;
    double lunit = CLHEP::mm;
 
    if (setup_.has_key ("length_unit"))
      {
	string lunit_str = setup_.fetch_string ("length_unit");
	lunit = datatools::utils::units::get_length_unit_from (lunit_str);
      }

    if (setup_.has_key ("build_mode"))
      {
	build_mode_label = setup_.fetch_string ("build_mode");
      }

    if (build_mode_label == "points")
      {
	vector<double> zs;
	vector<double> rmins;
	vector<double> rmaxs;
	double rmin = 0.0;
	size_t n_sides;

	if (setup_.has_key ("sides"))
	  {
	    int ns = setup_.fetch_integer ("sides");
	    if (ns < MIN_NUMBER_OF_SIDES)
	      {
		ostringstream message;
		message << "polyhedra::initialize: "
			<< "'sides' is not large enough !";
		throw runtime_error (message.str ());
	      }
	    n_sides = ns;
	  }

	if (setup_.has_key ("list_of_z"))
	  {
	    setup_.fetch ("list_of_z", zs);
	    if (zs.size () < 2)
	      {
		ostringstream message;
		message << "polyhedra::initialize: "
			<< "'list_of_z' has not enough points !";
		throw runtime_error (message.str ());
	      }
	  }
	else
	  {
	    ostringstream message;
	    message << "polyhedra::initialize: "
		    << "Missing 'list_of_z' property !";
	    throw runtime_error (message.str ());
	  }

	if (setup_.has_key ("list_of_rmax"))
	  {
	    setup_.fetch ("list_of_rmax", rmaxs);
	    if (rmaxs.size () != zs.size ())
	      {
		ostringstream message;
		message << "polyhedra::initialize: "
			<< "'list_of_z' and 'list_of_rmax' have not the same size !";
		throw runtime_error (message.str ());
		    
	      }
	  }
	else
	  {
	    ostringstream message;
	    message << "polyhedra::initialize: "
		    << "Missing 'list_of_rmax' property !";
	    throw runtime_error (message.str ());
	  }

	if (setup_.has_key ("list_of_rmin"))
	  {
	    setup_.fetch ("list_of_rmin", rmins);
	    if (rmins.size () != zs.size ())
	      {
		ostringstream message;
		message << "polyhedra::initialize: "
			<< "'list_of_rmin' and 'list_of_rmax' have not the same size !";
		throw runtime_error (message.str ());
		    
	      }
	  }
	else if (setup_.has_key ("rmin"))
	  {
	    rmin = setup_.fetch_real ("rmin", rmin);
	    rmin *= lunit;
	  }
	    
	for (int i = 0; i < zs.size (); i++)
	  {
	    double a_z = zs[i];
	    double a_rmin;
	    if (datatools::utils::is_valid (rmin))
	      {
		a_rmin = rmin;
	      }
	    else
	      {
		a_rmin = rmins[i];
	      }
	    double a_rmax = rmaxs[i];
	    this->add (a_z, a_rmin, a_rmax, false);
	  }
	this->__compute_all ();	  
      }
    else if (build_mode_label == "datafile")
      {
	string datafile;
	if (setup_.has_key ("datafile"))
	  {
	    datafile = setup_.fetch_string ("datafile");
	  }  
	else
	  {
	    ostringstream message;
	    message << "polyhedra::initialize: "
		    << "Missing 'datafile' property !";
	    throw runtime_error (message.str ());
	  }
	datatools::utils::fetch_path_with_env (datafile);
	this->initialize (datafile);
      }
    else 
      {
	ostringstream message;
	message << "polyhedra::initialize: "
		<< "Invalid build mode '" << build_mode_label << "' !";
	throw runtime_error (message.str ());
      }

    return;
  }


  void polyhedra::initialize (const string & filename_)
  {
    bool devel = false;
    //devel = true;
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
    bool   ignore_rmin = false;
    size_t nsides;

    while (! ifs.eof ())
      {
	string line;
	getline (ifs, line);
	//cerr << "DEVEL: polyhedra::initialize: " << "line = '" << line << "'" << endl;
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
		  if (word == "#@sides")
		    {
		      int ns;
		      iss >> ns;
		      if (! iss)
			{
			  ostringstream message;
			  message << "polyhedra::initialize: " 
				  << "Invalid format for the number of sides directive in data file '"
				  << filename << "' at line " << count << " !";
			  throw runtime_error (message.str ()); 
			}
		      if (ns < MIN_NUMBER_OF_SIDES)
			{
			  ostringstream message;
			  message << "polyhedra::initialize: " 
				  << "Number of sides is not large enough in data file '"
				  << filename << "' at line " << count << " !";
			  throw runtime_error (message.str ()); 
			}
		      nsides = (size_t) ns;
		      set_n_sides (nsides);
		    }
		  else if (word == "#@length_unit")
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
		  else if (word == "#@ignore_rmin")
		    {
		      ignore_rmin = true;
		    }
		  else if (word == "#@z_factor")
		    {
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
	// parse (z,r1) or (z,r1,r2) formated lines:
	{
	  istringstream iss (line);
	  double z, r1, r2;
	  datatools::utils::invalidate (z);
	  datatools::utils::invalidate (r1);
	  datatools::utils::invalidate (r2);
	  iss >> z;
	  if (! iss)
	    {
	      ostringstream message;
	      message << "polyhedra::initialize: " 
		      << "Format error for 'z' in data file '"
		      << filename << "' at line " << count << " !";
	      throw runtime_error (message.str ()); 
	    }
	  iss >> r1;
	  if (! iss)
	    {
	      ostringstream message;
	      message << "polyhedra::initialize: " 
		      << "Format error for 'r1' in data file '"
		      << filename << "' at line " << count << " !";
	      throw runtime_error (message.str ()); 
	    }
	  // try to read a third column:
	  string token;
	  iss >> token;
	  if (token.length () == 0)
	    {
	      // two columns format:
	      r2 = r1;
	      datatools::utils::invalidate (r1);
	    }
	  else
	    {
	      if (token[0] == '#')
		{
		  // if line ends with a comment: this is two columns format !
		  r2 = r1;
		  datatools::utils::invalidate (r1);
		}
	      else 
		{
		  // try three columns format (z, r1, r2):
		  istringstream iss2 (token);
		  iss2 >> r2;
		  if (! iss2)
		    {
		      ostringstream message;
		      message << "polyhedra::initialize: " 
			      << "Format error for 'r2' in data file '"
			      << filename << "' at line " << count << " !";
		      throw runtime_error (message.str ()); 
		    }	    
		  if (ignore_rmin)
		    {
		      datatools::utils::invalidate (r1);
		    }
		}
	    }
	  if (datatools::utils::is_valid (r2) && (r2 < 0.0))
	    {
	      ostringstream message;
	      message << "polyhedra::initialize: " 
		      << "Invalid value '" << r2 << "' for '2' in data file '"
		      << filename << "' at line " << count << " !";
	      throw runtime_error (message.str ()); 
	    }
	  double tz, tr1, tr2;
	  tz  = z  * z_factor * length_unit;
	  tr1 = r1 * r_factor * length_unit;
	  tr2 = r2 * r_factor * length_unit;
	  if (datatools::utils::is_valid (r1))
	    {
	      this->add (tz, tr1, tr2, false);
	    }
	  else
	    {
	      this->add (tz, tr2, false);
	    }
	}
      }
    this->__compute_all ();	    
    return;
  }

  void polyhedra::__compute_all ()
  {
    __compute_surfaces ();
    __compute_volume ();
    __compute_limits ();
    return;
  }

  void polyhedra::add (double z_, double rmax_, bool compute_)
  {
    if (rmax_ < 0.0)
      {
	throw runtime_error ("polyhedra::add: Invalid negative 'rmax' !");
      }
    r_min_max RMM;
    RMM.rmin = 0.0;
    RMM.rmax = rmax_;
    __points[z_] = RMM;
    if (__points.size () > 1)
      {
	if (compute_) __compute_all ();
      }
    return;
  }
  
  void polyhedra::add (double z_, double rmin_,  double rmax_, bool compute_)
  {
    if (rmin_ < 0.0)
      {
	throw runtime_error ("polyhedra::add: Invalid negative 'rmin' !");
      }
    if (rmax_ < rmin_)
      {
	throw runtime_error ("polyhedra::add: Invalid value for 'rmax' !");
      }
    r_min_max RMM;
    if (rmin_ > 0.0) __extruded = true;
    RMM.rmin = rmin_;
    RMM.rmax = rmax_;
    __points[z_] = RMM;
    if (__points.size () > 1)
      {
	if (compute_) __compute_all ();
      }
    return;
  }

  /*
    void polyhedra::add (double z_, double r_)
    {
    if (r_ < 0.0)
    {
    throw runtime_error ("polyhedra::add: Invalid negative 'r' !");
    }
    __points[z_] = r_;
    __compute_all ();
    }
  */

  bool polyhedra::is_valid () const
  {
    return (__n_sides > 3) && (__points.size () > 1);
  }
  
  void polyhedra::reset ()
  {
    __n_sides = 0;
    __points.clear ();
    __top_surface = numeric_limits<double>::quiet_NaN();
    __bottom_surface = numeric_limits<double>::quiet_NaN();
    __inner_side_surface = numeric_limits<double>::quiet_NaN();
    __outer_side_surface = numeric_limits<double>::quiet_NaN();
    __inner_volume = numeric_limits<double>::quiet_NaN();
    __outer_volume = numeric_limits<double>::quiet_NaN();
    __volume = numeric_limits<double>::quiet_NaN();
    __z_min = __z_max = __r_max = numeric_limits<double>::quiet_NaN();
    __extruded = false;
    return;
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

  void polyhedra::__compute_surfaces ()
  {
    if (! is_valid ()) return;
    
    // bottom surface:
    {
      rz_col_t::const_iterator i = __points.begin ();
      double z0 = i->first;
      double rmin0 = i->second.rmin;
      double rmax0 = i->second.rmax;
      regular_polygon bottom_inner_polygon (__n_sides, rmin0);
      regular_polygon bottom_outer_polygon (__n_sides, rmax0);
      __bottom_surface = bottom_outer_polygon.get_surface ()
	- bottom_inner_polygon.get_surface ();
    }

    // outer side surface:
    {
      rz_col_t::const_iterator i = __points.begin ();
      double z0 = i->first;
      double rmin0 = i->second.rmin;
      double rmax0 = i->second.rmax;
      double s = 0.0;
      rz_col_t::const_iterator j = __points.begin ();
      j++;
      while (j != __points.end ())
	{
	  double z1 = j->first;
	  double rmin1 = j->second.rmin;
	  double rmax1 = j->second.rmax;
	  // See: http://en.wikipedia.org/wiki/Frustum#Surface_Area
	  size_t n = __n_sides;
	  double angle = M_PI / n;
	  double a1 = 2 * rmax0 * sin (angle); 
	  double a2 = 2 * rmax1 * sin (angle); 
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
	  rmin0 = rmin1;
	  rmax0 = rmax1;
	}
      __outer_side_surface = s;
    
      // top surface:
      regular_polygon top_inner_polygon (__n_sides, rmin0);
      regular_polygon top_outer_polygon (__n_sides, rmax0);
      __top_surface = top_outer_polygon.get_surface ()
	- top_inner_polygon.get_surface ();
    }
 
    {
      // inner side surface:
      rz_col_t::const_iterator i = __points.begin ();
      double z0 = i->first;
      double rmin0 = i->second.rmin;
      double rmax0 = i->second.rmax;
      double s = 0.0;
      rz_col_t::const_iterator j = i;
      j++;
      while (j != __points.end ())
	{
	  double z1 = j->first;
	  double rmin1 = j->second.rmin;
	  double rmax1 = j->second.rmax;
	  // See: http://en.wikipedia.org/wiki/Frustum#Surface_Area
	  size_t n = __n_sides;
	  double angle = M_PI / n;
	  double a1 = 2 * rmin0 * sin (angle); 
	  double a2 = 2 * rmin1 * sin (angle); 
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
	  rmin0 = rmin1;
	  rmax0 = rmax1;
	}
      __inner_side_surface = s;
    }
     
    return;
  }
  
  void polyhedra::__compute_volume ()
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
	  double rmax1 = j->second.rmax;
	  // See: http://en.wikipedia.org/wiki/Frustum# Volume
	  size_t n = __n_sides;
	  double angle = M_PI / n;
	  double a1 = 2 * rmax0 * sin (angle); 
	  double a2 = 2 * rmax1 * sin (angle); 
	  double h = abs (z1 - z0);
	  double V = n * h * (a1 * a1 + a2 * a2 + a1 * a2) / tan (angle) / 12;
	  vext += V;
	  // increment:
	  j++;
	  z0 = z1;
	  rmax0 = rmax1;
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
	  double rmin1 = j->second.rmin;
	  // See: http://en.wikipedia.org/wiki/Frustum# Volume
	  size_t n = __n_sides;
	  double angle = M_PI / n;
	  double a1 = 2 * rmin0 * sin (angle); 
	  double a2 = 2 * rmin1 * sin (angle); 
	  double h = abs (z1 - z0);
	  double V = n * h * (a1 * a1 + a2 * a2 + a1 * a2) / tan (angle) / 12;
	  vint += V;
	  // increment:
	  j++;
	  z0 = z1;
	  rmin0 = rmin1;
	}
    }

    __outer_volume = vext;
    __inner_volume = vint;
    __volume = __outer_volume - __inner_volume;
    return;
  }
 
  double polyhedra::get_surface (int mask_) const
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
    if ( flag_ == "surface.inner_side" ) return get_surface (FACE_INNER_SIDE);
    if ( flag_ == "surface.outer_side" ) return get_surface (FACE_OUTER_SIDE);
    if ( flag_ == "surface" ) return get_surface (FACE_ALL);

    throw runtime_error ("polyhedra::get_parameter: Unknown flag!");
  }

  bool polyhedra::is_inside (const vector_3d & point_, 
			     double skin_) const
  {
    throw runtime_error ("polyhedra::is_inside: Not implemented yet !");
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
    throw runtime_error ("polyhedra::get_normal_on_surface: Not implemented yet !");
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
    throw runtime_error ("polyhedra::is_on_surface: Not implemented yet !");
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
	double rmin = i->second.rmin;
	double rmax = i->second.rmax;
	out_ << ' ' << z << ' ' << rmin << ' ' << rmax;
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
	 << "Outer side surface : " << get_surface (FACE_OUTER_SIDE) / CLHEP::cm2 << " cm2" << endl;
    out_ << indent << du::i_tree_dumpable::tag 
	 << "Inner side surface : " << get_surface (FACE_INNER_SIDE) / CLHEP::cm2 << " cm2" << endl;
    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)
	 << "Total surface : " << get_surface (FACE_ALL) / CLHEP::cm2 << " cm2" << endl;
    return;
  }
  
} // end of namespace geomtools

// end of polyhedra.cc

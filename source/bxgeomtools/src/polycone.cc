// -*- mode: c++ ; -*- 
/* polycone.cc
 */

#include <geomtools/polycone.h>

#include <cmath>
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <vector>
#include <fstream>

#include <datatools/utils/utils.h>
#include <datatools/utils/units.h>

#include <mygsl/tabfunc.h>
#include <mygsl/numerical_differentiation.h>
#include <mygsl/interval.h>

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

  void polycone::compute_inner_polycone (polycone & ip_)
  {
    ip_.reset ();
    for (polycone::rz_col_t::const_iterator i = __points.begin ();
	 i != __points.end ();
	 i++)
      {
	double z = i->first;
	double rmax = i->second.rmin;
	bool add_it = true;
	if (add_it)
	  {
	    ip_.add (z, rmax, false);
	  }
      }
    ip_.__compute_all ();
    return;
  }

  void polycone::compute_outer_polycone (polycone & op_)
  {
    op_.reset ();
    for (polycone::rz_col_t::const_iterator i = __points.begin ();
	 i != __points.end ();
	 i++)
      {
	double z = i->first;
	double rmax = i->second.rmax;
	op_.add (z, 0.0, rmax, false);
      }
    op_.__compute_all ();
    return;
  }

  void polycone::__build_from_envelope_and_skin (double skin_thickness_, 
						 double skin_step_)
  {
    __build_from_envelope_and_skin (skin_thickness_,
				    skin_step_,
				    std::numeric_limits<double>::quiet_NaN (),
				    std::numeric_limits<double>::quiet_NaN ());
    return;
  }

  void polycone::__build_from_envelope_and_skin (double skin_thickness_, 
						 double skin_step_,
						 double zmin_,
						 double zmax_)
  {
    bool devel = false;
    devel = true;
    double zmin = zmin_;
    double zmax = zmax_;
    
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
    string interpolation_mode = "linear";
    //interpolation_mode = "akima";
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
    // ofstream f1 ("f1.data");
    // tf.print_points (f1);

    // thickness of the polycone's skin:
    double skin = skin_thickness_;

    // compute the recommended sampling step along the envelope:
    double dx0 = 5 * skin; // default
    double dx = dx0;
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
	mygsl::interval domain (tf.x_min (), tf.x_max (), 0.01 * skin);
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
    //ofstream f3 ("f3.data");
    //tf3.print_points (f3);
    
    // Manage bounds:
    if (devel)
      {
	clog << "DEVEL: " << "polycone::__build_from_envelope_and_skin: "
	     << "Building properly bounded interpolated inner envelope..."
	     << endl;
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
    double z1 = tf.x_min ();
    double z2 = tf.x_max ();
    if (devel) cerr << "DEVEL: " << "polycone::__build_from_envelope_and_skin: "
		    << "z1=" << z1 << endl;
    if (devel) cerr << "DEVEL: " << "polycone::__build_from_envelope_and_skin: "
		    << "z2=" << z2 << endl;
    if (datatools::utils::is_valid (zmin))
      {
	if (devel) cerr << "DEVEL: " << "polycone::__build_from_envelope_and_skin: "
			<< "Z(min)=" << zmin << endl;
	if (zmin > tf.x_min ())
	  {
	    z1 = zmin;
	  }
      }
    double zlim = tf.x_max ();
    if (datatools::utils::is_valid (zmax))
      {
	if (devel) cerr << "DEVEL: "  << "polycone::__build_from_envelope_and_skin: "
			<< "Z(max)=" << zmax << endl;
	if (zmax < tf.x_max ())
	  {
	    z2 = zmax;
	  }
      }
    double dz = dx;
    double za, ra;
    datatools::utils::invalidate (za);
    double z = z1;
    bool stop = false;
    //while (z < tf.x_max () + 0.1 * dz)
    do
      {
	/*
	cerr << "DEVEL: "  << "polycone::__build_from_envelope_and_skin: "
	     << "Loop: z=" << z << endl;
	*/
	if (z >= z2) 
	  {
	    if (devel) cerr << "DEVEL: "  << "polycone::__build_from_envelope_and_skin: "
			    << "z2 stop" << endl;
	    z = z2;
	    stop = true;
	  }
	double safe = 0.05 * skin;
	safe = 0.1 * dz;
	mygsl::interval domain (tf.x_min (), tf.x_max (), safe);
	double drdz = mygsl::derivative (tf, z, domain);
	dz = dx / sqrt (1.0 + drdz * drdz);
	if (dz > dx) 
	  {
	    dz = dx;
	  }
	/*
	else if (dz < 0.1 * skin)
	  {
	    dz = 0.1 * skin;
	  }
	*/
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
		double a = (rb - ra) / (zb - za);
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
	if (stop) break;
      }
      while (! stop);
     // lock interpolators:
    tf_outer.lock_table ();
    tf_inner.lock_table (); 
    if (devel) 
      {
	cerr << "DEVEL: " << "polycone::__build_from_envelope_and_skin: "
	     << "Locked !" << endl;
      }
    /*
    ofstream fo ("fo.data");
    ofstream fi ("fi.data");
    tf_outer.print_points (fo);
    tf_inner.print_points (fi);
    */
    __points.clear ();

    //ofstream fp ("fp.data");
    mygsl::tabulated_function::points_map_t::const_iterator i = tf_outer.points ().begin ();
    mygsl::tabulated_function::points_map_t::const_iterator j = tf_inner.points ().begin ();
    for (int k = 0; k < tf_outer.points ().size (); k++)
      {
	double z = i->first;
	double rmin = j->second;
	double rmax = i->second;
	if (rmin > rmax) rmin = rmax;
	if (rmin < 0.0) // should not occur
	  {
	    rmin = 0.0;
	  }
	this->add (z, rmin, rmax, false);
	i++;
	j++;
	//fp << z << ' ' << rmin << ' ' << rmax << endl;
      }

    __compute_all ();
    return;
  }

  void polycone::initialize (const datatools::utils::properties & setup_)
  {
    string build_mode_label;
    string datafile;
    double zmin, zmax;
    datatools::utils::invalidate (zmin);
    datatools::utils::invalidate (zmax);
    double lunit = CLHEP::mm;

    if (setup_.has_key ("build_mode"))
      {
	build_mode_label = setup_.fetch_string ("build_mode");
      }

    if (setup_.has_key ("length_unit"))
      {
	string lunit_str = setup_.fetch_string ("length_unit");
	lunit = datatools::utils::units::get_length_unit_from (lunit_str);
      }

    if (build_mode_label == "points")
      {
	vector<double> zs;
	vector<double> rmins;
	vector<double> rmaxs;
	double rmin;
	datatools::utils::invalidate (rmin);

	if (setup_.has_key ("list_of_z"))
	  {
	    setup_.fetch ("list_of_z", zs);
	    if (zs.size () < 2)
	      {
		ostringstream message;
		message << "polycone::initialize: "
			<< "'list_of_z' has not enough points !";
		throw runtime_error (message.str ());
	      }
	  }
	else
	  {
	    ostringstream message;
	    message << "polycone::initialize: "
		    << "Missing 'list_of_z' property !";
	    throw runtime_error (message.str ());
	  }

	if (setup_.has_key ("list_of_rmax"))
	  {
	    setup_.fetch ("list_of_rmax", rmaxs);
	    if (rmaxs.size () != zs.size ())
	      {
		ostringstream message;
		message << "polycone::initialize: "
			<< "'list_of_z' and 'list_of_rmax' have not the same size !";
		throw runtime_error (message.str ());
		    
	      }
	  }
	else
	  {
	    ostringstream message;
	    message << "polycone::initialize: "
		    << "Missing 'list_of_rmax' property !";
	    throw runtime_error (message.str ());
	  }

	if (setup_.has_key ("list_of_rmin"))
	  {
	    setup_.fetch ("list_of_rmin", rmins);
	    if (rmins.size () != zs.size ())
	      {
		ostringstream message;
		message << "polycone::initialize: "
			<< "'list_of_rmin' and 'list_of_rmax' have not the same size !";
		throw runtime_error (message.str ());
		    
	      }
	  }
	else if (setup_.has_key ("rmin"))
	  {
	    rmin = setup_.fetch_real ("rmin", rmin);
	    rmin *= lunit;
	  }
	else
	  {
	    rmin = 0.0 * lunit;
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
	double zmin, zmax;
	datatools::utils::invalidate (zmin);
	datatools::utils::invalidate (zmax);

	if (setup_.has_key ("datafile"))
	  {
	    datafile = setup_.fetch_string ("datafile");
	  }  
	else
	  {
	    ostringstream message;
	    message << "polycone::initialize: "
		    << "Missing 'datafile' property !";
	    throw runtime_error (message.str ());
	  }

	if (setup_.has_key ("zmin"))
	  {
	    zmin = setup_.fetch_real ("zmin");
	    zmin *= lunit;
	  }  

	if (setup_.has_key ("zmax"))
	  {
	    zmax = setup_.fetch_real ("zmax");
	    zmax *= lunit;
	  }  

	datatools::utils::fetch_path_with_env (datafile);
	this->initialize (datafile, zmin, zmax);
      }
    else 
      {
	ostringstream message;
	message << "polycone::initialize: "
		<< "Invalid build mode '" << build_mode_label << "' !";
	throw runtime_error (message.str ());
      }

    return;
  }

  void polycone::initialize (const string & filename_)
  {
    polycone::initialize (filename_,
			  std::numeric_limits<double>::quiet_NaN (),
			  std::numeric_limits<double>::quiet_NaN ());
    return;
  }

  void polycone::initialize (const string & filename_, 
			     double zmin_, double zmax_)
  {
    bool devel = false;
    //devel = true;
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
    double skin_step;
    datatools::utils::invalidate (skin_step);
    bool ignore_rmin = false;

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
	      message << "polycone::initialize: " 
		      << "Format error for 'z' in data file '"
		      << filename << "' at line " << count << " !";
	      throw runtime_error (message.str ()); 
	    }
	  iss >> r1;
	  if (! iss)
	    {
	      ostringstream message;
	      message << "polycone::initialize: " 
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
		  // try three columns format:
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
		  if (ignore_rmin)
		    {
		      datatools::utils::invalidate (r1);
		    }
		  else if (datatools::utils::is_valid (skin_thickness))
		    {
		      ostringstream message;
		      message << "polycone::initialize: " 
			      << "Invalid format for 'z r2' pair "
			      << "in 'skin_thickness' mode from data file '"
			      << filename << "' at line " << count << " !";
		      throw runtime_error (message.str ()); 
		    }
		}
	    }
	  if (datatools::utils::is_valid (r2) && (r2 < 0.0))
	    {
	      ostringstream message;
	      message << "polycone::initialize: " 
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

    if (datatools::utils::is_valid (skin_thickness))
      {
	if (! datatools::utils::is_valid (skin_step))
	  {
	    skin_step = abs (zmax_ - zmin_) / 20.0;
	  }
	__build_from_envelope_and_skin (skin_thickness, skin_step, zmin_, zmax_);
      }
    return;
  }

  void polycone::__compute_all ()
  {
    __compute_surfaces ();
    __compute_volume ();
    __compute_limits ();
    return;
  }

  void polycone::add (double z_, double rmax_, bool compute_)
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
	if (compute_) __compute_all ();
      }
    return;
  }

  bool polycone::is_extruded () const
  {
    return __extruded;
  }

  void polycone::add (double z_, double rmin_,  double rmax_, bool compute_)
  {
    if (rmin_ < 0.0)
      {
	throw runtime_error ("polycone::add: Invalid negative 'rmin' !");
      }
    if (rmax_ < rmin_)
      {
	ostringstream message;
	message <<  "polycone::add: " 
		<< "Invalid value for 'rmax==" << rmax_ << "' ! ('rmin==" << rmin_ << "')";
	throw runtime_error (message.str ());
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
    __extruded = false;
    return;
  }

  void polycone::initialize ()
  {
    if (! is_valid ())
      {
	throw runtime_error ("polycone::initialize: Polycone is not valid !");
      }
    __compute_all ();
    return;
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
	if (! datatools::utils::is_valid (__z_min))
	  {
	    __z_min = z;
	  }
	else if (z < __z_min)
	  {
	    __z_min = z;
	  }
	if (! datatools::utils::is_valid (__z_max))
	  {
	    __z_max = z;
	  }
	else if (z > __z_max)
	  {
	    __z_max = z;
	  }
	if (! datatools::utils::is_valid (__r_max))
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
    //cerr << "DEBUG: polycone::__compute_surfaces: Entering..." << endl;
    
    // bottom surface:
    {
      rz_col_t::const_iterator i = __points.begin ();
      double z0 = i->first;
      double rmin0 = i->second.rmin;
      double rmax0 = i->second.rmax;
      __bottom_surface = M_PI * (rmax0 * rmax0 - rmin0 * rmin0);
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
	  rmax0 = rmax1;
	}
      __outer_side_surface = s;

      // top surface:
      __top_surface = M_PI * (rmax0 * rmax0 - rmin0 * rmin0);
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
	  rmax0 = rmax1;
	}
      __inner_side_surface = s;
      /*
      cerr << "DEBUG: polycone::__compute_surfaces: " 
	   << "__inner_side_surface==" << __inner_side_surface << endl;
      */
    }
    return;
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
	  double rmin1 = j->second.rmin;
	  // See: http://en.wikipedia.org/wiki/Frustum# Volume
	  double R1 = rmin0;
	  double R2 = rmin1;
	  double R1_2 = R1 * R1;
	  double R2_2 = R2 * R2;
	  double h = abs (z1 - z0);
	  double V = M_PI * h * (R1_2 + R2_2 + R1 * R2) / 3;
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

    throw runtime_error ("polycone::get_parameter: Unknown parameter flag !");
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
	    p_.add (z, rmin, rmax, false);
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
    p_.__compute_all ();
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

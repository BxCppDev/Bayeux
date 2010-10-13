// -*- mode: c++ ; -*- 
/* disk.cc
 */

#include <geomtools/disk.h>

namespace geomtools {

  using namespace std;

  const string disk::DISK_LABEL = "disk";

  string disk::get_shape_name () const
  {
    return DISK_LABEL;
  }
  
  double disk::get_r () const
  {
    return __r;
  }
  
  double disk::get_radius () const
  {
    return get_r ();
  }
 
  void disk::set_diameter (double new_value_)
  {
    set_r (new_value_ * 0.5);
  }

  double disk::get_diameter () const
  {
    return (__r + __r);
  }
  
  void disk::set_r (double new_value_)
  {
    if (new_value_ < 0.0 )
      {
	ostringstream message;
	message << "disk::set_r: Invalid '" << new_value_ << "' R value!";
	throw logic_error (message.str ());
      }
    __r = new_value_;
  }
  
   
  double disk::get_surface () const
  {
    return M_PI * __r * __r;
  }

  double disk::get_circumference () const
  {
    return 2 * M_PI * __r;
  }

  bool disk::is_valid () const
  {
    return (__r > 0.0);
  }
  
  // ctor:
  disk::disk ()
  {
    __r = -1.0;
  }
  
  // ctor:
  disk::disk (double r_)
  {
    set_r (r_);
  }
  
  // dtor:
  disk::~disk ()
  {
  }

  bool disk::is_on_surface (const vector_3d & position_,
			    double tolerance_) const
  {
    double tolerance = get_tolerance ();
    if (tolerance_ > USING_PROPER_TOLERANCE) tolerance = tolerance_;
    double z = position_.z ();
    if (std::abs (z) > 0.5 * tolerance) return false;

    double x = position_.x ();
    double y = position_.y ();
    double r2 = (__r + 0.5 * tolerance) * (__r + 0.5 * tolerance);
    double rho2 = x * x + y * y;
    if (rho2 > r2) 
      {
	return false;
      }
    return true;
  }

  vector_3d disk::get_normal_on_surface (const vector_3d & position_,
					 bool up_) const
  {
    vector_3d normal;
    geomtools::invalidate (normal);
    if (is_on_surface (position_))
      {
	normal.set (0.0, 0.0, 1.0);
	if (! up_) normal.setZ (-1.0);
      }
    return normal;
  }

  bool disk::find_intercept (const vector_3d & from_, 
			     const vector_3d & direction_,
			     intercept_t & intercept_,
			     double tolerance_) const
  {
     double ux = direction_.x ();
    double uy = direction_.y ();
    double uz = direction_.z ();
    double xf = from_.x ();
    double yf = from_.y ();
    double zf = from_.z ();
    if (uz == 0.0)
      {
	intercept_.reset ();
	if (zf != 0.0)
	  {
	    return intercept_.is_ok ();	    
	  }
	double p0;
	double p1;
	double p2;
	double lambda1;
	double lambda2;
	p2 = (ux * ux + uy * uy);
	p1 = -2 * (ux * xf + uy * yf);
	double r = pow (__r + 0.5 * tolerance_, 2);
	p0 = xf * xf + yf * yf - r * r;
	size_t nsol = 0;
	nsol = gsl_poly_solve_quadratic (p2, p1, p0, &lambda1, &lambda2);
	if (nsol == 1)
	  {
	    double xi = xf + lambda1 * ux; 
	    double yi = yf + lambda1 * uy;
	    double zi = zf;
	    intercept_.set (0, 0, vector_3d (xi, yi, zi));
	  }
	else if (nsol == 2)
	  {
	    double lambda = -1.0;
	    if (lambda1 >= 0.0)
	      {
		lambda = lambda1;
	      }
	    if ((lambda2 >= 0.0) && (lambda2 < lambda1))
	      {
		lambda = lambda2;
	      }
	    double xi = xf + lambda * ux; 
	    double yi = yf + lambda * uy;
	    double zi = zf;
	    intercept_.set (0, 0, vector_3d (xi, yi, zi));
	  }
	return intercept_.is_ok ();	    	    
      }

    intercept_.reset ();
    double lambda = - zf / uz;
    if (lambda < 0.0)
      {
	return intercept_.is_ok ();
      }
    double xi, yi, zi;
    zi = 0.0;
    xi = xf + lambda * ux;
    yi = yf + lambda * uy;
    vector_3d impact (xi, yi, zi);
    if (! is_on_surface (impact, tolerance_))
      {
	intercept_.is_ok ();
      }
    else
      {
	int face = FACE_UP;
	if (from_.z ()  < 0.0)
	  {
	    face = FACE_DOWN;
	  }
	intercept_.set (0, face, vector_3d (xi, yi, zi));
      }
    return intercept_.is_ok ();
  }

  void disk::tree_dump (ostream & out_, 
			const string & title_, 
			const string & indent_, 
			bool inherit_) const
  {
    namespace du = datatools::utils;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    i_object_3d::tree_dump (out_, title_, indent_, true);

    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)  
	 << "R : " << get_r () / CLHEP::mm << " mm" << endl;
    return;
  }
  
} // end of namespace geomtools

// end of disk.cc

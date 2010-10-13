// -*- mode: c++ ; -*- 
/* rectangle.cc
 */

#include <geomtools/rectangle.h>

namespace geomtools {

  using namespace std;

  const string rectangle::RECTANGLE_LABEL = "rectangle";

  string rectangle::get_shape_name () const
  {
    return RECTANGLE_LABEL;
  }
  
  double rectangle::get_x () const
  {
    return __x;
  }
  
  double rectangle::get_y () const
  {
    return __y;
  }
  
  void rectangle::set_x (double new_value_)
  {
    if (new_value_ < 0.0 )
      {
	ostringstream message;
	message << "rectangle::set_x: Invalid '" << new_value_ << "' X value!";
	throw logic_error (message.str ());
      }
    __x = new_value_;
  }
  
  void rectangle::set_y (double new_value_)
  {
    if (new_value_ < 0.0 )
      {
	ostringstream message;
	message << "rectangle::set_y: Invalid '" << new_value_ << "' Y value!";
	throw logic_error (message.str ());
      }
    __y = new_value_;
  }
  
  double rectangle::get_diagonal () const
  {
    return  hypot (__x, __y);
  }
   
  double rectangle::get_surface () const
  {
    return  __x * __y;
  }

  double rectangle::get_circumference () const
  {
    return 2 * (__x + __y);
  }

  bool rectangle::is_valid () const
  {
    return (__x > 0.0) && (__y > 0.0);
  }
  
  // ctor:
  rectangle::rectangle ()
  {
    __x = -1.0;
    __y = -1.0;
  }

  // ctor:
  rectangle::rectangle (double x_, double y_)
  {
    set_x (x_);
    set_y (y_);
  }
  
  // dtor:
  rectangle::~rectangle ()
  {
  }

  bool rectangle::is_on_surface (const vector_3d & position_,
				 double tolerance_) const
  {
    double tolerance = get_tolerance ();
    if (tolerance_ > USING_PROPER_TOLERANCE) tolerance = tolerance_;
    double z = position_.z ();
    if (std::abs (z) > (0.5 * tolerance))
      {
	return false;
      }
    double x = position_.x ();
    double y = position_.y ();
    if (std::abs (x) > (0.5 * (__x + tolerance)))
      {
	return false;
      }
    if (std::abs (y) > (0.5 * (__y + tolerance)))
      {
	return false;
      }
    return true;
  }

  vector_3d rectangle::get_normal_on_surface (const vector_3d & position_,
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

  bool rectangle::find_intercept (const vector_3d & from_, 
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
	/*
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
	*/
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

  void rectangle::tree_dump (ostream & out_, 
			const string & title_, 
			const string & indent_, 
			bool inherit_) const
  {
    namespace du = datatools::utils;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    i_object_3d::tree_dump (out_, title_, indent_, true);
    out_ << indent << du::i_tree_dumpable::tag
	 << "X : " << get_x () / CLHEP::mm << " mm" << endl;
    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)  
	 << "Y : " << get_y () / CLHEP::mm << " mm" << endl;
    return;
  }
  
} // end of namespace geomtools

// end of rectangle.cc

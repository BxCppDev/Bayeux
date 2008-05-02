// test_ode.cxx
/*
 * compute movement of a simple oscillator:
 *
 * shell$ ./test_ode > a.data
 *
 * gnuplot> plot 'a.data' using 1:2  title "" with dots 
 *
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

#include <mygsl/ode.h>

class test_ode_system : public mygsl::ode_system
{
  size_t __dimension;
  double __x, __vx;
  double __omega_square;
  bool   __use_jacobian;

public:

  virtual bool 
  has_jacobian() const
  {
    return __use_jacobian;
  }
  
  void 
  set_omega_square(double o2_) 
  {
    if (o2_ <= 0.0) 
      {
	throw std::runtime_error(
				 "test_ode_system::set_omega_square: Invalid value!");
      }
    __omega_square = o2_;
  }

  void 
  set_xvx(double x_ , double vx_)
  {
    __x  = x_;
    __vx = vx_;
  }

  test_ode_system(double o2_ = 1.0, bool uj_ = true)
  {
    __use_jacobian = uj_ ;
    __dimension = 2;
    set_omega_square(o2_);
    set_xvx(1.0,0.0);
  }

  virtual size_t 
  get_dimension() const
  {
    return __dimension;
  }
    
  virtual int 
  compute_derivatives(double         t_ , 
		      const double * y_ ,
		      double  *      f_)
  {
    double x  = y_[0];
    double vx = y_[1];
    f_[0] = vx;
    f_[1] = -__omega_square*x;
    return 0;
  }

  virtual int 
  compute_jacobian(double t_ , 
		   const double * y_ ,
		   double  * dfdy_  ,
		   double  * dfdt_)
  {
    double o2 = __omega_square;
    gsl_matrix_view dfdy_mat = gsl_matrix_view_array(dfdy_,2,2);
    gsl_matrix * m = &dfdy_mat.matrix;
    gsl_matrix_set(m,0,0,0.0);
    gsl_matrix_set(m,0,1,1.0);
    gsl_matrix_set(m,1,0,-o2);
    gsl_matrix_set(m,1,1,0.0);
    dfdt_[0] = 0.0;
    dfdt_[1] = 0.0;
    return 0;
  }

  virtual void 
  to_double_star(double * y_ , 
		 size_t dimension_ ) const
  {
    if (dimension_ != __dimension) 
      {
	throw std::range_error(
			       "test_ode_system::to_double_star: Invalid dimension!");
      }
    y_[0] = __x;
    y_[1] = __vx;
  }
  
  virtual void 
  from_double_star(const double * y_ , 
		   size_t dimension_) 
  {
    if (dimension_ != __dimension) 
      {
	throw std::range_error(
			       "test_ode_system::from_double_star: Invalid dimension!");
      }
    __x  = y_[0];
    __vx = y_[1];
  }

};

int 
main (int argc_ , char ** argv_)
{
  try 
    {
      mygsl::ode_driver::g_debug = false;
      mygsl::ode_system::g_debug = false;

      std::string type     = "rk8pd"; // RK8 Prince-Dormand
      std::string arg_type = ""; 

      bool debug        = false;
      bool use_jacobian = false;

      int iarg=1;
      while (iarg < argc_) 
	{

	  std::string arg = argv_[iarg];

	  if (arg[0] == '-') 
	    {
	      if (arg == "-d" || arg == "--debug") 
		{
		  debug=true;
		}
	      if (arg == "-j" || arg == "--use-jacobian") 
		{
		  use_jacobian=true;
		}
	    }
	  else 
	    {
	      if (arg_type.empty()) 
		{
		  arg_type=arg;
		}
	    }

	  iarg++;
	}

      if ( debug ) 
	{
	  mygsl::ode_driver::g_debug = true;
	  std::cerr << "available ODE steppers:" << std::endl;
	  mygsl::ode_driver::print_types(std::cerr);
	  std::cerr << std::endl;
	}

      if (!arg_type.empty()) 
	{
	  type = arg_type;
	} 

      double epsabs  = 1.e-6;   // step relative error
      double epsrel  = 1.e-6;   // step maximum error
      bool   regular = true;    // set regular steps

      double omega_square = 1.0;  // oscillator's squared pulsation

      test_ode_system sys(omega_square,use_jacobian);

      mygsl::ode_driver od(sys,type,epsabs,epsrel,regular);
      od.set_default_step_action(); // optional;

      double t_min =  0.0;
      double t_max = 20.0;
      double h     =  0.02;

      if (od.run(t_min,t_max,h) != GSL_SUCCESS) 
	{
	  throw std::runtime_error("ode_driver::run: failed!");
	}

    }
  catch (std::exception & x) 
    { 
      std::cerr << "ERROR: " << x.what() << std::endl;
      exit(EXIT_FAILURE);
    }
  return(EXIT_SUCCESS);
}

// end of test_ode.cxx

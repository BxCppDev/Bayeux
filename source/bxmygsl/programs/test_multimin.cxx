// test_multimin.cxx
/*
 * compute movement of a simple oscillator:
 *
 * shell$ ./test_multimin > a.data
 *
 * gnuplot> plot 'a.data' using 1:2  title "" with dots 
 *
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include <mygsl/rng.h>
#include <mygsl/histogram.h>
#include <mygsl/multimin.h>

double sigma_resol( double e_ , double r_ )
{
  double f = 2.*std::sqrt(2.*std::log(2.));
  return r_*std::sqrt(e_/1000.)*1000./f;
}

class experiment
{
public:

  struct params_t
  {
    double a,b,r,p;
  };

  double __e1;
  double __e2;
  double __sig_e1;
  double __sig_e2;
  size_t __nevents;

  double __r;
  double __a;
  double __b;
  double __p;

  void set( const params_t & params_ , size_t nevent_ )
  {
    __r = params_.r;
    __a = params_.a;
    __b = params_.b;
    __p = params_.p;
    __sig_e1=sigma_resol(__e1,__r);
    __sig_e2=sigma_resol(__e2,__r);
    __nevents = nevent_;
    std::cerr << "experiment::set " 
	      << __r << ' ' 
	      << __a << ' ' 
	      << __b << ' ' 
	      << __p << ' ' 
	      << __nevents << ' ' 
	      << __sig_e1 << ' ' 
	      << __sig_e2 << ' ' 
	      << std::endl;
  }

  experiment( const params_t & params_ , size_t nevent_ )
  {
    __e1=540.;
    __e2=980.;
    set(params_,nevent_);
  }

  void shoot( mygsl::histogram & h_ )
  {
    h_.init(1024,0.,1024.);

    mygsl::rng ran;

    for ( int i=0; i<__nevents; i++ ) {

      double energy;

      double r=ran.uniform();

      if ( r<__p ) {
	energy = ran.gaussian(__e1,__sig_e1);
      }
      else {
	energy = ran.gaussian(__e2,__sig_e2);
      }

      int ch=(int) ((energy-__b)/__a);
      h_.fill(ch);

    }
  }
  
};


class test_multimin_system : public mygsl::multimin_system
{
  bool debug;
  //experiment::params_t     __params;
  const mygsl::histogram * __hdata;

public:
  
  void set_debug( bool d_ )
  {
    debug=d_;
  }

  test_multimin_system ( const mygsl::histogram & h_ )
  {
    debug=false;
    __hdata=&h_;
  }

  double f_nth( double energy_ )
  {
    double res = 0.0;
    double e   = energy_;
    double p1  = get_param_value(3);  // p
    double p2  = 1.-p1;
    double t   = 0.0;
    double e1  = 540.;
    double e2  = 980.;
    double sum = __hdata->sum();
    double sig_e1 = sigma_resol(e1,get_param_value(2)); // r
    double sig_e2 = sigma_resol(e2,get_param_value(2)); // r
    t += p1*gsl_ran_gaussian_pdf(e-e1,sig_e1);
    t += p2*gsl_ran_gaussian_pdf(e-e2,sig_e2);
    res = t;
    res *= sum; 
    return res;
  }

  void print_solution( std::ostream & out_ )
  {
    double a   = get_param_value(0);
    double b   = get_param_value(1);
    for ( int ich=0; ich<__hdata->bins(); ich++ ) {
      double energy     = a*(ich+0.5)+b;
      if ( debug ) std::cerr << "DEBUG: test_multimin_system::compute_chi_square: energy=" << energy << std::endl;
      double nth        = f_nth( energy ); 
      out_ << ich << ' ' << nth << std::endl;
    }
  }

  static double sqr( double x_ )
  {
    return x_*x_;
  } 

  double compute_chi_square()
  {
    if ( debug ) std::cerr << "DEBUG: test_multimin_system::compute_chi_square: entering... " << std::endl;
    double chi2=0.0;
    size_t ch_min=0;
    size_t ch_max=__hdata->bins();
    ch_min = 100;
    ch_max = 500;
    double a   = get_param_value(0);
    double b   = get_param_value(1);
    for ( int ich=ch_min; ich<ch_max; ich++) {
      if ( debug ) std::cerr << "DEBUG: test_multimin_system::compute_chi_square: LOOP ich=" << ich << std::endl;
      double nexp       = __hdata->at(ich);
      if ( debug ) std::cerr << "DEBUG: test_multimin_system::compute_chi_square: nexp=" << nexp << std::endl;
      double sigma_nexp = std::sqrt(nexp);
      if ( nexp < 2 ) sigma_nexp = 1.0;
      if ( debug ) std::cerr << "DEBUG: test_multimin_system::compute_chi_square: sigma_nexp=" << sigma_nexp << std::endl;
      double energy     = a*(ich+0.5)+b;
      if ( debug ) std::cerr << "DEBUG: test_multimin_system::compute_chi_square: energy=" << energy << std::endl;
      double nth        = f_nth( energy ); 
      if ( debug ) std::cerr << "DEBUG: test_multimin_system::compute_chi_square: nth=" << nth << std::endl;
      chi2 += sqr((nexp-nth)/sigma_nexp);
    }
    if ( debug ) std::cerr << "DEBUG: test_multimin_system::compute_chi_square: chi2=" << chi2 << std::endl;
    return chi2;
  }
    
  virtual int eval_f( const double * x_ , double & f_ )
  {
    if ( debug ) std::cerr << "DEBUG: test_multimin_system::eval_f: entering... " << std::endl;
    from_double_star(x_,get_dimension());
    f_ = compute_chi_square();
    return 0;
  }

  virtual int eval_df( const double * x_ , double * gradient_ )
  {
    if ( debug ) std::cerr << "DEBUG: test_multimin_system::eval_df: entering... " << std::endl;
    from_double_star(x_,get_dimension());
    return 0;
  }

};

int main ( int argc_ , char ** argv_ )
{
  try {

    mygsl::histogram h;

    experiment::params_t params;
    params.a = 3.0;
    params.b = 50.0;
    params.r = 0.15;
    params.p = 0.35;

    size_t nevents = 10000;   

    experiment e(params,nevents);
    e.shoot(h);

    std::ofstream fhist("test_multimin.hist");
    h.print(fhist);
    fhist.close();

    experiment::params_t start_params;

    start_params = params;
    /*
    start_params.a = 3.5;
    start_params.b = 100.0;
    start_params.r = 0.20;
    start_params.p = 0.40;
    */

    test_multimin_system sys(h);
    sys.add_param(mygsl::multimin_system::param_entry::make_param_entry(
      "a", start_params.a, 2., 4., 0.01,
      mygsl::multimin_system::param_entry::FREE_PARAM));

    sys.add_param(mygsl::multimin_system::param_entry::make_param_entry(
      "b", start_params.b, 0., 200., 10.,
      mygsl::multimin_system::param_entry::FREE_PARAM));

    sys.add_param(mygsl::multimin_system::param_entry::make_param_entry(
      "r", start_params.r, 0.13, 0.22, 0.005,
      mygsl::multimin_system::param_entry::FREE_PARAM));

    sys.add_param(mygsl::multimin_system::param_entry::make_param_entry(
      "p", start_params.p, 0., 1., 0.01,
      mygsl::multimin_system::param_entry::FREE_PARAM));

    mygsl::multimin::print_types(std::cerr);
    std::cerr << std::endl;

    mygsl::multimin mm;
    mm.set_default_step_action();
    mm.init("nmsimplex",sys);

    double epsabs=1.0;
    if ( mm.minimize(epsabs) == 0 ) {
      std::clog << "Found minimum!" << std::endl;
      std::ofstream fsol("test_multimin.sol");
      sys.print_params(std::cout);
      sys.print_solution(fsol);
    }
    else {
      std::clog << "Cannot find minimum!" << std::endl;
    }

  }
  catch (std::exception & x) { 
    std::cerr << "ERROR: " << x.what() << std::endl;
    exit(EXIT_FAILURE);
  }
  return(EXIT_SUCCESS);
}

// end of test_multimin.cxx

// test_multimin.cxx
/*
 * compute movement of a simple oscillator:
 *
 * shell$ ./test_multimin > a.data
 *
 * gnuplot> plot 'a.data' using 1:2  title "" with dots 
 *
 * test for memory management pbs:
 * shell$ valgrind --tool=memcheck 
 *                 --leak-check=yes 
 *                 --show-reachable=yes 
 *                 --num-callers=20 
 *                 --track-fds=yes 
 *                 ./Linux-i686/bin/test_multimin  > v.log 2>&1
 *
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <ctime>

#include <gsl/gsl_sf.h>
#include <mygsl/rng.h>
#include <mygsl/histogram.h>
#include <mygsl/multimin.h>


double sqr( double x_ )
{
  return x_*x_;
} 

double log_fact( unsigned long k_ )
{
  return gsl_sf_lnfact(k_);
}

double log_poisson( unsigned long k_ , double mu_ )
{
  return -mu_ + k_*std::log(mu_) - log_fact(k_);
}

struct data
{
  static const double E1;
  static const double E2;
  static const double E3;
  static const double E4;
};
const double data::E1=540.;
const double data::E2=980.;
const double data::E3=720.;
const double data::E4=1150.;

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
    double a,b,r,p1,p2,p3,p4;
    void dump( std::ostream & out_ ) const
    {
      std::string tag  = "|-- ";
      std::string ltag = "`-- ";
      out_ << "experiment::params_t" << std::endl;
      out_ << tag << "a  = " << a << std::endl;
      out_ << tag << "b  = " << b << std::endl;
      out_ << tag << "r  = " << r << std::endl;
      out_ << tag << "p1 = " << p1 << std::endl;
      out_ << tag << "p2 = " << p2 << std::endl;
      out_ << tag << "p3 = " << p3 << std::endl;
      out_ <<ltag << "p4 = " << p4 << std::endl;
    }
  };

  double __e1;
  double __e2;
  double __e3;
  double __e4;
  double __sig_e1;
  double __sig_e2;
  double __sig_e3;
  double __sig_e4;
  size_t __nevents;
  unsigned long int __seed;

  double __r;
  double __a;
  double __b;
  double __p1;
  double __p2;
  double __p3;
  double __p4;

  void set( const params_t & params_ , 
	    size_t nevents_ , 
	    unsigned long int seed_ )
  {
    __r = params_.r;
    __a = params_.a;
    __b = params_.b;
    __p1 = params_.p1;
    __p2 = params_.p2;
    __p3 = params_.p3;
    __p4 = params_.p4;
    __sig_e1=sigma_resol(__e1,__r);
    __sig_e2=sigma_resol(__e2,__r);
    __sig_e3=sigma_resol(__e3,__r);
    __sig_e4=sigma_resol(__e4,__r);
    __nevents = nevents_;
    __seed    = seed_ ;
    /*
    std::cerr << "experiment::set " 
	      << __r << ' ' 
	      << __a << ' ' 
	      << __b << ' ' 
	      << __p << ' ' 
	      << __nevents << ' ' 
	      << __sig_e1 << ' ' 
	      << __sig_e2 << ' ' 
	      << std::endl;
    */
  }

  experiment( const params_t & params_ , 
	      size_t nevents_          = 5000 , 
	      unsigned long int seed_ = 0 )
  {
    __e1=data::E1;
    __e2=data::E2;
    __e3=data::E3;
    __e4=data::E4;
    set(params_,nevents_,seed_);
  }

  void shoot( mygsl::histogram & h_ )
  {
    h_.init(1024,0.,1024.);

    mygsl::rng ran("mt19937",__seed);

    for ( int i=0; i<__nevents; i++ ) {

      double energy;

      double r=ran.uniform();

      if ( r<__p1 ) {
	energy = ran.gaussian(__e1,__sig_e1);
      }
      else if ( r<__p2+__p1 ) {
	energy = ran.gaussian(__e2,__sig_e2);
      }
      else if ( r<__p3+__p2+__p1 ) {
	energy = ran.gaussian(__e3,__sig_e3);
      }
      else {
	energy = ran.gaussian(__e4,__sig_e4);
      }

      int ch=(int) ((energy-__b)/__a);
      h_.fill(ch);

    }
  }
  
};

class test_multimin_system : public mygsl::multimin_system
{

public:
  enum mode_t
    {
      MODE_CHISQUARE  = 0,
      MODE_LIKELIHOOD = 1
    };

private:
  bool                       debug;
  int                      __mode;
  experiment::params_t     __exp_params;
  const mygsl::histogram * __hdata;

public:
  
  bool is_chisquare() const
  {
    return __mode==MODE_CHISQUARE;
  }

  bool is_likelihood() const
  {
    return __mode==MODE_LIKELIHOOD;
  }

  void set_debug( bool d_ )
  {
    debug=d_;
  }

  test_multimin_system ( const mygsl::histogram & h_ , 
			 int mode_ = MODE_CHISQUARE ,
			 bool debug_ = false )
  {
    debug   = debug_;
    __mode  = mode_;
    __hdata = &h_;
  }

  double f_nth( double energy_ )
  {
    double e   = energy_;
    double a   = __exp_params.a;
    double b   = __exp_params.b;
    double r   = __exp_params.r;
    double p1  = __exp_params.p1;  
    double p2  = __exp_params.p2;
    double p3  = __exp_params.p3;  
    double p4  = __exp_params.p4;  

    double t   = 0.0;
    double e1  = data::E1;
    double e2  = data::E2;
    double e3  = data::E3;
    double e4  = data::E4;
    double sum = __hdata->sum();

    double sig_e1 = sigma_resol(e1,r); 
    double sig_e2 = sigma_resol(e2,r);
    double sig_e3 = sigma_resol(e3,r);
    double sig_e4 = sigma_resol(e4,r);
    t += p1*gsl_ran_gaussian_pdf(e-e1,sig_e1);
    t += p2*gsl_ran_gaussian_pdf(e-e2,sig_e2);
    t += p3*gsl_ran_gaussian_pdf(e-e3,sig_e3);
    t += p4*gsl_ran_gaussian_pdf(e-e4,sig_e4);
    double res = t;
    res *= sum; 
    res *= a; 
    return res;
  }

  void print_solution( std::ostream & out_ )
  {
    double a = get_param_value(0);
    double b = get_param_value(1);
    for ( int ich=0; ich<__hdata->bins(); ich++ ) {
      double energy = a*(ich+0.5)+b;
      double nth = f_nth( energy ); 
      out_ << ich << ' ' << nth << std::endl;
    }
  }

  double compute_likelihood()
  {
    bool local_debug=false;
    //if ( debug ) std::cerr << "DEBUG: test_multimin_system::compute_likelihood: entering... " << std::endl;
    double likelihood = 0.0;
    size_t ch_min = 0;
    size_t ch_max = __hdata->bins();
    double a      = get_param_value(0);
    double b      = get_param_value(1);
    for ( int ich=ch_min; ich<ch_max; ich++) {
      double nexp       = __hdata->at(ich);
      double energy     = a*(ich+0.5)+b;
      double nth        = f_nth( energy ); 
      unsigned int sz_exp = (unsigned int) nexp;

      double lp=log_poisson(sz_exp,nth);
      if ( local_debug ) std::cerr << "DEBUG: test_multimin_system::compute_likelihood: " 
			     << " ich="  << ich 
			     << " nth="  << nth 
			     << " sz_exp=" << sz_exp 
			     << " lp=" << lp<< std::endl;

      likelihood -= lp;
    }

    bool more=true;
    //more=false;

    if ( more ) { 

      double slope=1e15;
      for ( int i=0; i<get_dimension(); i++ ) {
	if ( is_param_free(i) ) {
	  if ( !get_param(i).is_in_range() ) {
	    double dist=get_param(i).get_dist_to_limit();
	    double dist_term=slope*(dist/get_param(i).get_step());
	    likelihood+=dist_term;
	    //std::cerr << "DEBUG: test_multimin_system::compute_likelihood: dist_term="  << dist_term << " for i=" << i << std::endl;
	  }
	}
      }
      
      double p2   = get_param_value(4);
      double p4   = get_param_value(6);
      
      if ( p4 > 0.35*p2 ) {
	double dist=p4-0.35*p2;
	double dist_term=slope*(dist/get_param(4).get_step());
	likelihood+=dist_term;
	//std::cerr << "DEBUG: test_multimin_system::compute_likelihood: dist_term="  << dist_term << " for p4 > 0.5*p2" << std::endl;
      }
    }

    if ( local_debug ) std::cerr << "DEBUG: test_multimin_system::compute_likelihood: likelihood="  << likelihood<< std::endl;

    return likelihood;
  }

  double compute_chi_square()
  {
    bool local_debug=false;
    if ( local_debug ) std::cerr << "DEBUG: test_multimin_system::compute_chi_square: entering... " << std::endl;
    double chi2=0.0;
    size_t ch_min=0;
    size_t ch_max=__hdata->bins();
    //ch_min = 100;
    //ch_max = 500;
    double a   = get_param_value(0);
    double b   = get_param_value(1);
    for ( int ich=ch_min; ich<ch_max; ich++) {
      if ( local_debug ) std::cerr << "DEBUG: test_multimin_system::compute_chi_square: LOOP ich=" << ich << std::endl;
      double nexp       = __hdata->at(ich);
      if ( local_debug ) std::cerr << "DEBUG: test_multimin_system::compute_chi_square: nexp=" << nexp << std::endl;
      double sigma_nexp = std::sqrt(nexp);
      if ( nexp < 2 ) sigma_nexp = 1.0;
      if ( local_debug ) std::cerr << "DEBUG: test_multimin_system::compute_chi_square: sigma_nexp=" << sigma_nexp << std::endl;
      double energy    = a*(ich+0.5)+b;
      if ( local_debug ) std::cerr << "DEBUG: test_multimin_system::compute_chi_square: energy=" << energy << std::endl;
      double nth       = f_nth( energy ); 
      if ( local_debug ) std::cerr << "DEBUG: test_multimin_system::compute_chi_square: nth=" << nth << std::endl;
      chi2 += sqr((nexp-nth)/sigma_nexp);
    }
    if ( local_debug ) std::cerr << "DEBUG: test_multimin_system::compute_chi_square: chi2=" << chi2 << std::endl;

    double slope=1e7;

    for ( int i=0; i<get_dimension(); i++ ) {
      if ( is_param_free(i) ) {
	if ( !get_param(i).is_in_range() ) {
	  double dist=get_param(i).get_dist_to_limit();
	  double dist_term=slope*(dist/get_param(i).get_step());
	  chi2+=dist_term;
	}
      }
    }

    double p2   = get_param_value(4);
    double p4   = get_param_value(6);

    if ( p4 > 0.35*p2 ) {
      double dist=p4-0.35*p2;
      double dist_term=slope*(dist/get_param(4).get_step());
      chi2+=dist_term;
    }

    return chi2;
  }
    
protected:

  virtual int _auto_values()
  {
    //std::cerr << "DEBUG: test_multimin_system::_auto_values: entering... " << std::endl;
    __exp_params.p2  = 1.-__exp_params.p1-__exp_params.p3-__exp_params.p4;
    /*
    std::cerr << "DEBUG: test_multimin_system::_auto_values: entering... " 
	      << __exp_params.p1 << ' '
	      << __exp_params.p2 << ' '
	      << __exp_params.p3 << ' '
	      << __exp_params.p4 << ' '
	      << std::endl;
    */
    set_param_value_no_check(4,__exp_params.p2);
    return 0;
  }

  virtual int _prepare_values()
  {
    //std::cerr << "DEBUG: test_multimin_system::_prepare_values: entering... " << std::endl;
    __exp_params.a   = get_param_value(0);
    __exp_params.b   = get_param_value(1);
    __exp_params.r   = get_param_value(2);
    __exp_params.p1  = get_param_value(3);  
    __exp_params.p2  = get_param_value(4);
    __exp_params.p3  = get_param_value(5);  
    __exp_params.p4  = get_param_value(6); 
    /*    std::cerr << "DEBUG: test_multimin_system::_prepare_values: entering... " 
	      << __exp_params.p1 << ' '
	      << __exp_params.p2 << ' '
	      << __exp_params.p3 << ' '
	      << __exp_params.p4 << ' '
	      << std::endl;
    */
    return 0;
  }

  virtual int _eval_f( double & f_ )
  {
    //if ( debug ) std::cerr << "DEBUG: test_multimin_system::_eval_f: entering... " << std::endl;
    
    if ( is_chisquare() ) {
      f_ = compute_chi_square();
      return 0;
    }

    if ( is_likelihood() ) {
      f_ = compute_likelihood();
      return 0;
    }

    return 1;
  }

  virtual int _eval_df( double * gradient_ )
  {
    //if ( debug ) std::cerr << "DEBUG: test_multimin_system::_eval_df: entering... " << std::endl;
    std::cerr << "DEBUG: test_multimin_system::_eval_df: Not implemented!" << std::endl;
    return 1;
  }

};

int main ( int argc_ , char ** argv_ )
{
  try {


    int mode = test_multimin_system::MODE_CHISQUARE;
    bool debug = false;
    bool fixed_seed = false;

    int iarg=1;
    while ( iarg < argc_ ) {
      
      std::string arg = argv_[iarg];
      std::clog << "main: arg='" << arg << "'"<< std::endl;
      if ( arg[0] == '-' ) {
	
	if ( arg == "--debug" || arg == "-d" ) {
	  debug=true;
	}

	if ( arg == "--seed" || arg == "-s" ) {
	  fixed_seed=true;
	}

	if ( arg == "--likelihood" || arg == "-l" ) {
	  mode = test_multimin_system::MODE_LIKELIHOOD;
	}

	if ( arg == "--chisquare" || arg == "-c" ) {
	  mode = test_multimin_system::MODE_CHISQUARE;
	}

      }
      else {

      }

      iarg++;
    }


    mygsl::histogram h;

    experiment::params_t params;
    params.a  = 3.0;
    params.b  = 50.0;
    params.r  = 0.15;
    params.p1 = 0.30;
    params.p2 = 0.45;
    params.p3 = 0.10;
    params.p4 = 0.15;

    params.dump(std::clog);

    size_t nevents = 5000;   
    //nevents = 5000000;   

    unsigned long int seed = 1;

    if ( ! fixed_seed ) {
      time_t tt;
      tt = time(NULL);
      srand((unsigned int)tt);
      seed = rand();
    }

    std::cerr << "seed=" << seed << std::endl;

    experiment e(params,nevents,seed);

    e.shoot(h);

    std::ofstream fhist("test_multimin.hist");
    h.print(fhist);
    fhist.close();

    experiment::params_t start_params;

    start_params = params;
    start_params.a  = 2.9;
    start_params.b  = 40.0;
    start_params.r  = 0.16;  
    start_params.p1 = 0.30;
    start_params.p2 = 0.50;
    start_params.p3 = 0.10;
    start_params.p4 = 0.10;

    test_multimin_system sys(h,mode,debug);

    sys.add_param(mygsl::multimin_system::param_entry::make_param_entry_ranged(
      "a", start_params.a, 2.8, 3.2, 0.005));

    // 10.
    sys.add_param(mygsl::multimin_system::param_entry::make_param_entry_ranged(
      "b", start_params.b, 0., 200., 1.));

    // 0.005
    sys.add_param(mygsl::multimin_system::param_entry::make_param_entry_ranged(
      "r", start_params.r, 0.13, 0.20, 0.001));

    sys.add_param(mygsl::multimin_system::param_entry::make_param_entry_ranged(
      "p1", start_params.p1, 0., 1., 0.005));

    sys.add_param(mygsl::multimin_system::param_entry::make_param_entry_auto(
      "p2"));

    //sys.add_param(mygsl::multimin_system::param_entry::make_param_entry_fixed("p3", start_params.p3));

    sys.add_param(mygsl::multimin_system::param_entry::make_param_entry_ranged(
      "p3", start_params.p3, 0., 1., 0.005));

    //sys.add_param(mygsl::multimin_system::param_entry::make_param_entry_fixed("p4", start_params.p4));

    sys.add_param(mygsl::multimin_system::param_entry::make_param_entry_ranged(
      "p4", start_params.p4, 0., 1., 0.005));
   
    sys.dump(std::clog);
    sys.lock_params();

    //return 1;

    mygsl::multimin::print_types(std::cerr);
    std::cerr << std::endl;

    mygsl::multimin mm;
    //mm.set_default_step_action();
    mm.init("nmsimplex",sys);

    double epsabs=1.0;
    //epsabs=1.0e-2;
    if ( mm.minimize(epsabs) == 0 ) {
      std::clog << "Found minimum!" << std::endl;
      std::ofstream fsol("test_multimin.sol");
      sys.print_params(std::cout);
      sys.print_solution(fsol);
      double fval = mm.get_fval();
      std::clog << "Fval=" << fval << std::endl;
      std::ofstream fpar("test_multimin.pars",std::ios::app);
      sys.print_params2(fpar);
      fpar << fval << std::endl;
    }
    else {
      std::clog << "Cannot find minimum!" << std::endl;
      double fval = mm.get_fval();
      std::clog << "Last fval=" << fval << std::endl;
    }
  }
  catch (std::exception & x) { 
    std::cerr << "ERROR: " << x.what() << std::endl;
    exit(EXIT_FAILURE);
  }
  return(EXIT_SUCCESS);
}

// end of test_multimin.cxx

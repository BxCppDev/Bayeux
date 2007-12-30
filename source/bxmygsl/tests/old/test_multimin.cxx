// -*- mode: c++; -*- 
// test_multimin.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <cmath>
#include <vector>

#include <mygsl/dummy_mygsl.h>
#include <gsl/gsl_multimin.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_integration.h>

namespace utils {

  struct gsl_quad_defaults
  {
    static const int    DEFAULT_KEY     = GSL_INTEG_GAUSS21;
    //static const int    DEFAULT_KEY     = GSL_INTEG_GAUSS15;
    //static const int    DEFAULT_KEY     = GSL_INTEG_GAUSS61;
    static const size_t DEFAULT_WS_SIZE = 20;
    static const double DEFAULT_EPSREL  = 1.e-5;
  };

  struct dumpable
  {
    typedef std::ostream & (*manip)( std::ostream & out_ );

    static std::ostream & item( std::ostream & out_ )
    {
      out_ << "|-- ";
      return out_;
    }

    static std::ostream & last_item( std::ostream & out_ )
    {
      out_ << "`-- ";
      return out_;
    }

    static std::ostream & skip( std::ostream & out_ )
    {
      out_ << "|   ";
      return out_;
    }

    static std::ostream & no_skip( std::ostream & out_ )
    {
      out_ << "    ";
      return out_;
    }

    virtual void dump( std::ostream & out_ , 
		       const std::string & title_ = "" , 
		       const std::string & indent_ = "" , 
		       bool inherit_ = false ) const = 0;
  };

  struct unary_eval : 
    public std::unary_function<double,double>
  {
    virtual double eval( double x_ ) const = 0;
    double operator()( double x_ ) const
    {
      return eval(x_);
    }
  };



  double gauss( double x_ , double mu_ , double sigma_ )
  {
    return gsl_ran_gaussian_pdf(x_-mu_,sigma_);
  }

  double counts_approx_err( double counts_ )
  {
    double res;
    if ( counts_ >= 10. ) {
      res=sqrt(counts_);
    }
    else {
      res=2.3+((std::sqrt(10.)-2.3)/10.)*counts_;
    }
    return res;
  }

  double fwhm_2_sigma( double fwhm_ )
  {
    return fwhm_/(2.*sqrt(2.*log(2.)));
  }

  double energy_fwhm( double e_ , double r_ )
  {
    double e_keV = e_;
    double e_MeV = e_keV/1000.;
    return r_*sqrt(e_MeV)*1000.;
  }

  double energy_sigma( double e_ , double r_ )
  {
    return fwhm_2_sigma(energy_fwhm(e_,r_));
  }

  struct energy_resolution : public unary_eval
  {
    double __r;
    double __bound;
    double r() const { return __r; }

    void r( double r_ ) { 
      if ( r_ < 0.0 ) {
	throw std::runtime_error(
          "energy_resolution::r: Invalid 'r' value!");
      }
      __r=r_; 
    }

    void init( double r_ )
    {
      r(r_);
    }

    energy_resolution( double r_= 0.0 )
    {
      __bound=100.; // keV
      init(r_);
    }

    double fwhm( double energy_ ) const
    {
      if ( energy_ < __bound ) return energy_fwhm(__bound,__r);
      return energy_fwhm(energy_,__r);
    }

    double sigma( double energy_ ) const
    {
      if ( energy_ < __bound ) return energy_sigma(__bound,__r);
      return energy_sigma(energy_,__r);
    }

    virtual double eval( double energy_ ) const
    {
      return sigma( energy_ );
    }

  };

  struct energy_calibration
  {
    double __alpha, __beta;

    double alpha() const { return __alpha; }

    void alpha( double alpha_ ) {
      if ( alpha_ < 0.0 ) {
	throw std::runtime_error(
          "energy_calibration::alpha: Invalid 'alpha' value!");
      }
      __alpha=alpha_; 
    }

    double beta() const { return __beta; }

    void beta( double beta_ ) { 
      __beta=beta_; 
    }

    double to_energy( int channel_ ) const
    {
      return __alpha*channel_+__beta;
    }

    int to_channel( double energy_ ) const
    {
      return (int) ((energy_-__beta)/__alpha);
    }
    
  };

  struct resol_peak : public unary_eval
  {
    double __peak_energy;
    double __sigma;

    void init( double peak_energy_ , double r_ )
    {
      __peak_energy=peak_energy_;
      __sigma=energy_resolution(r_).sigma(__peak_energy);
    }

    resol_peak( double peak_energy_ =1.0, double r_=0.01 )
    {
      init(peak_energy_,r_);
    }

    virtual double eval( double energy_ ) const
    {
      return gauss(energy_,__peak_energy,__sigma);
    }

  };

  /*********************************************/

  struct range_sampling
  {
    double __min, __max;
    size_t __bins;
    double __step;

    double min() const { return __min; }
    double max() const { return __max; }
    double step() const { return __step; }
    size_t bins() const { return __bins; }

    double init( double min_ , double max_ , size_t bins_ )
    {
      if ( min_ >= max_ ) {
	throw std::runtime_error(
          "range_sampling::init: Invalid range!");
      }      
      if ( bins_ < 1 ) {
	throw std::runtime_error(
          "range_sampling::init: Invalid number of bins!");
      }      
      __min=min_;
      __max=max_;
      __bins=bins_;
      __step=(__max-__min)/__bins;
    }

    range_sampling( double min_ = 0.0 , 
		    double max_ = 1.0, 
		    size_t bins_=1024 )
    {
      init(min_,max_,bins_);
    }

    enum value_mode_t
      {
	BIN_MIDDLE=0,
	BIN_LOWER=1,
	BIN_UPPER=2
      };

    double to_value( size_t bin_ , int mode_ = BIN_LOWER ) const
    {
      double val=__min + __step*bin_; // BIN_LOWER
      if ( mode_ == BIN_MIDDLE ) val += 0.5*bin_;
      if ( mode_ == BIN_UPPER ) val += bin_;
      return val;
    }

    int to_bin( double val_ ) const
    {
      return (int) ((val_ - __min)/__step);
    }

  };

  /*********************************************/

  struct tabulated_function : public unary_eval 
  {
    range_sampling __sampling;
    std::vector<double> __values;

    tabulated_function()
    {
    }

    void scale( double s_ ) 
    {
      for ( int i=0; i<__values.size(); i++ ) {
	__values[i]*=s_;
      }
    }

    size_t size() const 
    {
      return __values.size();
    }

    void init( double min_ , double max_ , double step_ )
    {
      __values.clear();
      size_t bins= (size_t) ((max_-min_)/step_);
      __sampling.init(min_,max_,bins);
      __values.assign(bins+1,0.0);     
    }

    tabulated_function( double min_ , double max_ , double step_ )
    {
      init(min_,max_,step_);
    }

    void set( int ibin_ , double value_ )
    {
      if ( ibin_ < 0 || ibin_ >= size() ) {;
	throw std::runtime_error(
          "tabulated_function::set: invalid bin index!");
      }
      __values[ibin_]=value_;
    }

    double get( int ibin_ ) const
    {
      if ( ibin_ < 0 || ibin_ >= size() ) {
	throw std::runtime_error(
          "tabulated_function::get: invalid bin index!");
      }
      return __values[ibin_];
    }

    virtual double eval( double x_ ) const {
      int ibin1=__sampling.to_bin(x_);
      int ibin2=ibin1+1;
      // no interpolation scheme:
      return get(ibin1);
    }

    void load( std::istream & in_ )
    {
      double min, max, step;
      in_ >> min >> std::ws;
      in_ >> max >> std::ws;
      in_ >> step >> std::ws;
      init(min,max,step);
      size_t n=0;
      in_ >> n >> std::ws;
      if ( n != size() ) {
	throw std::runtime_error("tabulated_function::load: Corrupted input format!");
      }
      for ( int i=0; i<n ; i++ ) {
	in_ >> __values[i] >> std::ws;
      }
    }

    void store( std::ostream & out_ ) const
    {
      out_ << __sampling.min() << std::endl;
      out_ << __sampling.max() << std::endl;
      out_ << __sampling.step() << std::endl;
      size_t n=__sampling.bins()+1;
      out_ << n << std::endl;
      for ( int i=0; i<n ; i++ ) {
	double y = get(i);
	out_ << y << ' ';
      }
    }

    void dump( std::ostream & out_ ) const
    {
      for ( int i=0; i<size(); i++ ) {
	double x,y;
	if ( i==size()-1 ) {
	  x=__sampling.max();
	}
	else {
	  x=__sampling.to_value(i,range_sampling::BIN_LOWER);
	}
	y=get(i);
	out_ << x << ' ' << y << std::endl;
      }
    }
  };

  /**************************************************/

  double compton_edge( double hnu_ ) 
  {
    double m0,gamma,e,hnu;
    m0  = 511.;
    hnu = hnu_;
    gamma=hnu/m0;
    return 2*hnu*gamma/(1.+2.*gamma);
  }
  
  double compton_spectrum( double e_ , double hnu_ )
  {
    double m0,gamma,e,hnu,the_compton_edge,t,s;
    m0  = 511.;
    e   = e_;
    hnu = hnu_;
    gamma=hnu/m0;
    the_compton_edge=compton_edge(hnu);
    if ( e < 0.0 ) {
      return 0.0;
    }
    // XXXXX
    /*
    if ( e < 0.5*the_compton_edge  ) {
      return 0.0;
    }
    */
    if ( e > the_compton_edge  ) {
      return 0.0;
    }
    s = e/hnu;
    t = ( 2. 
	  + std::pow( s/ ( gamma*(1.-s) ),2) 
	  + ( s/(1.-s) )*( s - 2/gamma ) 
         ) / ( gamma*gamma );
    // XXXXX
    //t=1.0;
    return t;
  }

  double quad_compton_spectrum( double e_ , void * params_ )
  {
    double * hnu = static_cast<double*>(params_);
    return compton_spectrum(e_,*hnu);
  }

  double sum_compton_spectrum( double hnu_ , double & error_ )
  {
    double min=0.;
    min=20.;
    double max=compton_edge(hnu_);
    //max=50.;
    size_t ws_size=gsl_quad_defaults::DEFAULT_WS_SIZE*10;
    gsl_integration_workspace * ws = gsl_integration_workspace_alloc(ws_size);
    double result,error;

    gsl_function F;
    F.function = &quad_compton_spectrum;
    F.params   = &hnu_;

    int    key=gsl_quad_defaults::DEFAULT_KEY;

    size_t limit = ws_size;
    double epsabs = 1.e-4;
    double epsrel = gsl_quad_defaults::DEFAULT_EPSREL;
    int ret=gsl_integration_qag(&F,
				min,max,
				epsabs,epsrel,
				limit,key,
				ws,&result,&error);

    gsl_integration_workspace_free(ws);
    error_=error;
    return result;
  }

  struct norm_compton_spectrum : public unary_eval
  {
    double __hnu;
    double __sum;
    norm_compton_spectrum ( double hnu_ )
    {
      __hnu=hnu_;
      double error;
      __sum=sum_compton_spectrum(__hnu,error);
    }
    virtual double eval( double x_ ) const
    {
      return compton_spectrum(x_,__hnu)/__sum;
    }
  };

  double quad_norm_compton_spectrum( double e_ , void * params_ )
  {
    norm_compton_spectrum * ncs = 
      static_cast<norm_compton_spectrum*>(params_);
    return ncs->eval(e_);
  }

  double sum_norm_compton_spectrum( double hnu_ , double & error_ )
  {
    norm_compton_spectrum ncs(hnu_);
    double min = 0.;
    min=20.;
    double max = compton_edge(hnu_);
    //max=50.;
    size_t ws_size = gsl_quad_defaults::DEFAULT_WS_SIZE*10;
    gsl_integration_workspace * ws =
      gsl_integration_workspace_alloc(ws_size);
    double result,error;

    gsl_function F;
    F.function = &quad_norm_compton_spectrum;
    F.params   = &ncs;

    int    key = gsl_quad_defaults::DEFAULT_KEY;

    size_t limit = ws_size;
    double epsabs = 1.e-4;
    double epsrel = gsl_quad_defaults::DEFAULT_EPSREL;

    int ret=gsl_integration_qag(&F,
				min,max,
				epsabs,epsrel,
				limit,key,
				ws,&result,&error);
    error_=error;
    gsl_integration_workspace_free(ws);
    return result;
  }

  /**************************************************/

  struct convoluted_compton_spectrum : public unary_eval, dumpable
  {

    struct quad_params
    {
      double                              energy;
      const convoluted_compton_spectrum * ccs;
    };

    static double quad_func( double x_ , void * params_ );

    double            __hnu;
    double            __compton_edge;
    energy_resolution __er;
    size_t            __ws_size;
    gsl_integration_workspace * __ws;

    virtual void dump( std::ostream & out_ , 
		       const std::string & title_ = "" , 
		       const std::string & indent_ = "" , 
		       bool inherit_ = false ) const
    {

      dumpable::manip last=dumpable::last_item;
      if ( inherit_ ) last=dumpable::item;
      std::string indent=indent_;
      std::string title=title_;
      if ( !title.empty() ) {
	out_ << indent << title << std::endl;
      }
      out_ << indent << dumpable::item << "hnu = " 
           << __hnu << std::endl;
      out_ << indent << dumpable::item << "compton edge = " 
           << __compton_edge << std::endl;
      out_ << indent << dumpable::item << "energy resolution = " 
           << __er.r() << std::endl;
      out_ << indent << dumpable::skip
	   << dumpable::item 
	   << "sigma @ 1 MeV= " 
           << __er.sigma(1000.) << std::endl;
      out_ << indent << dumpable::skip 
	   << dumpable::last_item 
	   << "fwhm @ 1 MeV= " 
           << __er.fwhm(1000.) << std::endl;
      out_ << indent << last << "WS size = " 
           << __ws_size << std::endl;
    }

    void init( double hnu_ , 
	       double r_ )
    {
      __hnu=hnu_;
      __compton_edge=compton_edge(__hnu);
      __er.init(r_);
    }

    const energy_resolution & er() const { return __er; }

    double max_significative_energy() const
    {
      double sigma=__er.sigma(__compton_edge);
      std::cerr << "DEBUG: max_significative_energy: compton edge=" << __compton_edge << std::endl;
      std::cerr << "DEBUG: max_significative_energy: sigma=" << sigma << std::endl;
      double factor=3.; 
      return __compton_edge+factor*sigma;
    }

    convoluted_compton_spectrum( double hnu_     = 1000., 
				 double r_       = 0.01 , 
				 size_t ws_size_ = gsl_quad_defaults::DEFAULT_WS_SIZE )
    {
      __ws_size=ws_size_;
      __ws=0;
      __ws=gsl_integration_workspace_alloc(__ws_size);
      init(hnu_,r_);
    }

    virtual ~convoluted_compton_spectrum()
    {
      if ( __ws != 0 ) gsl_integration_workspace_free(__ws);
    }
    
    virtual double eval( double energy_ ) const
    {
      //std::cerr << "eval: Entering..." << std::endl;
      double sigma=__er.sigma(energy_);

      // protect at low energy:
      double bound = __hnu/10.;
      if ( energy_ < bound ) {
	return compton_spectrum(energy_,__hnu);
      }

      double factor=5.;

      double lower=std::max(-factor*sigma,energy_-__compton_edge);
      lower=energy_-__compton_edge;
      double upper=std::min(+factor*sigma,energy_);
      upper=energy_;
      size_t limit = __ws_size;

      double epsabs=1.e-4;

      double epsrel=gsl_quad_defaults::DEFAULT_EPSREL;

      int    key=gsl_quad_defaults::DEFAULT_KEY;

      if ( energy_ < 5. ) {
	std::cerr << "eval: energy=" << energy_ << std::endl;
	std::cerr << "eval: sigma=" << sigma << std::endl;
	std::cerr << "eval: lower=" << lower << std::endl;
	std::cerr << "eval: upper=" << upper << std::endl;
      }
      double result;

      double abserr;

      quad_params qp;
      qp.energy=energy_;
      qp.ccs=this;

      gsl_function func;
      func.function = quad_func;
      func.params   = &qp;

      int ret=gsl_integration_qag(
	&func,
	lower,
	upper,
	epsabs,
	epsrel,
	limit,
	key,
	__ws,
	&result,
	&abserr);

      if ( ret != GSL_SUCCESS) {
	throw std::runtime_error(
          "convoluted_compton_spectrum::eval: QAG failed!");
      }

      //std::cerr << "eval: Exiting." << std::endl;
      return result;
    }

  };

  double convoluted_compton_spectrum::quad_func( double x_ ,
						 void * params_ )
  {
    quad_params * qp = static_cast<quad_params *>(params_);
    
    double energy = qp->energy;
    const convoluted_compton_spectrum * ccs = qp->ccs;
    double sigma = ccs->__er.sigma(energy);
    return gauss(x_,0.0,sigma) 
      * compton_spectrum(energy-x_,ccs->__hnu);
  }

  /******************************************************************/

  struct quad_convoluted_compton_spectrum_params
  {
    double hnu;
    double r;
    convoluted_compton_spectrum ccs;
  };

  double quad_convoluted_compton_spectrum( double e_ , void * params_ )
  {
    quad_convoluted_compton_spectrum_params * qccsp=
      static_cast<quad_convoluted_compton_spectrum_params *>(params_);
    double hnu = qccsp->hnu;
    double r   = qccsp->r;
    convoluted_compton_spectrum & ccs = qccsp->ccs;
    return ccs(e_);
  }

  double sum_convoluted_compton_spectrum( double hnu_ , 
					  double r_ ,
					  double & error_ )
  {
    size_t ws_size=gsl_quad_defaults::DEFAULT_WS_SIZE*10;
    gsl_integration_workspace * ws = gsl_integration_workspace_alloc(ws_size);
    double result,error;

    quad_convoluted_compton_spectrum_params qccsp;
    qccsp.hnu = hnu_;
    qccsp.r   = r_;
    qccsp.ccs.init(hnu_,r_);

    double min=0.; // keV
    min=20.;
    double max=qccsp.ccs.max_significative_energy();
    //max=50.;
    double ce=compton_edge(hnu_);
    std::cerr << "hnu          = " << hnu_ << " keV" << std::endl;
    std::cerr << "compton_edge = " << ce << " keV" << std::endl;
    std::cerr << "min energy   = " << min << " keV" << std::endl;
    std::cerr << "max energy   = " << max << " keV" << std::endl;
    qccsp.ccs.dump(std::cerr,"Convoluted Compton spectrum:","");

    gsl_function F;
    F.function = &quad_convoluted_compton_spectrum;
    F.params   = &qccsp;

    int    key=gsl_quad_defaults::DEFAULT_KEY;

    size_t limit = ws_size;
    double epsabs = 1.e-4;
    double epsrel = gsl_quad_defaults::DEFAULT_EPSREL;

    int ret=gsl_integration_qag(&F,
				min,max,
				epsabs,epsrel,
				limit,key,ws,
				&result,&error);
    error_=error;
    gsl_integration_workspace_free(ws);
    return result;
  }

  /******************************************************************/


struct tabulated_compton_spectrum
{
  double hnu;
  double r;
  tabulated_function tf;

  tabulated_compton_spectrum()
  {
    hnu=0.0;
    r=0.0;
  }

  void store( std::ostream & out_ ) const
  {
    out_ << hnu << std::endl;
    out_ << r   << std::endl;
    tf.store(out_);
    out_ << std::endl;
  }

  void load( std::istream & in_ )
  {
    in_ >> hnu >> std::ws;
    in_ >> r >> std::ws;
    tf.load(in_); 
    in_ >> std::ws;
  }

  void process()
  {
    double sccs_err;
    double sccs;
    sccs=sum_convoluted_compton_spectrum(hnu,r,sccs_err);
    tf.scale(1./sccs);
  }
};


void tabulate_compton_spectra()
{
  bool print_tcs=true;
  std::ofstream oftables("tabulated_compton_spectra.data");
  std::ofstream oftcs("tcs.data");
  
  std::vector<double> compton_hnus;
  compton_hnus.push_back(570.);
  compton_hnus.push_back(1063.);
  compton_hnus.push_back(1770.);
  
  std::vector<double> energy_resolutions;
  double energy_r_min=0.14;
  double energy_r_max=0.23;
  double energy_r_step=0.01;
  for ( double r = energy_r_min; 
	r < energy_r_max+0.5*energy_r_step ;
	r += energy_r_step ) 
    {
      energy_resolutions.push_back(r);
    }
  
  double energy_min =    0.0;
  double energy_max = 1500.0;
  double energy_step=    1.0;

  for ( std::vector<double>::const_iterator i=compton_hnus.begin();
	i !=compton_hnus.end();
	i++ ) {
    double hnu=*i;
    oftables << hnu << " # hnu== gamma energy in keV" << std::endl;
    oftables << energy_resolutions.size() << " # number of tabulated energy resolution" << std::endl;

    for ( std::vector<double>::const_iterator j=energy_resolutions.begin();
	  j !=energy_resolutions.end();
	  j++ ) {
      double r=*j;

      tabulated_compton_spectrum tcs;
      tcs.hnu=hnu;
      tcs.r=r;
      tcs.tf.init(energy_min,energy_max,energy_step); 

      convoluted_compton_spectrum ccs;
      ccs.init(hnu,r);
      ccs.dump(std::cerr,"Convoluted Compton spectrum:","");

      int ibin=0;
      for ( double e=energy_min; 
	    e < energy_max+0.5*energy_step ; 
	    e += energy_step ) {
	double y=ccs(e);
	tcs.tf.set(ibin,y);
	ibin++;
      }
      tcs.process();
      tcs.store(oftables);
      if ( print_tcs ) {
	oftcs << "# hnu=" << hnu << " r=" << r << std::endl;
	tcs.tf.dump(oftcs);
	oftcs << std::endl << std::endl;
	print_tcs=false;
      }

    }

  }
}

} // namespace


using namespace utils;

int main( int argc_ , char ** argv_ )
{
  int error_code=EXIT_SUCCESS;
  try {

    std::cerr << "Hello, this is a sample program for class 'multimin'!" << std::endl; 
  
    bool debug=false;

    int iarg=1;
    while ( iarg<argc_ ) {
    
      std::string arg=argv_[iarg];

      if ( arg == "-d" || arg == "--debug" ) debug=true;

      iarg++;
    }

    double hnu=975.;
    //hnu=475.;

    for ( double x=0; x<10.; x+=0.1 ) {
      std::cout << x << ' ' << gauss(x,4.0,1.5) << std::endl;
    }
    std::cout << std::endl << std::endl;

    for ( double x=0; x<1200.; x+=1.0 ) {
      std::cout << x << ' ' << compton_spectrum(x,hnu) << std::endl;
    }
    std::cout << std::endl << std::endl;

    double r=0.15;

    resol_peak peak1;
    peak1.init(hnu,r);
    for ( double x=0; x<1200.; x+=1.0 ) {
      std::cout << x << ' ' << peak1(x) << std::endl;
    }
    std::cout << std::endl << std::endl;

    /******************************/

    convoluted_compton_spectrum ccs1;
    ccs1.init(hnu,r);
    ccs1.dump(std::cerr,"Convoluted Compton spectrum:","");

    std::ofstream fout("compton.gnuplot");
    fout << "set grid" << std::endl;
    fout << "set xrange [-500:+500]" << std::endl;
    fout << "set yrange [0:+0.02]" << std::endl;
    for ( double e = 0.; e < 1100. ; e += 20. ) {
      fout << "# e=" << e << std::endl;
      convoluted_compton_spectrum::quad_params params;
      params.energy = e;
      params.ccs    = &ccs1;
      
      double ce    = compton_edge(hnu);
      double sigma = ccs1.er().sigma(e);
      double c     = 5.*sigma;
      double lower = std::max(-c,e-ce);
      double upper = std::min(+c,e);

      double x1 = -10.*sigma;
      double x2 = +10.*sigma;
      double h  = (x2-x1)/1000;
      //fout <<;
      fout << "set arrow 1 from " << lower << ',' << 0.02
	   << " to " << lower << ',' << 0 << " head"
	   << std::endl;
      fout << "set arrow 2 from " << upper << ',' << 0.02 
	   << " to " << upper << ',' << 0 << " head"
	   << std::endl;
      fout << "plot '-' title \"E=" << e << " keV\" with lines" << std::endl;
      for ( double x=x1; x<x2; x+=h ) {
	double y = convoluted_compton_spectrum::quad_func(x,&params);
	fout << x << ' ' << y << std::endl;
      }
      fout << std::endl << 'e' << std::endl;
      fout << "unset arrow 1" << std::endl;
      fout << "unset arrow 2" << std::endl;
      
      fout << "pause -1 \"Hit return...\"" << std::endl;
    }

    double x =500.;
    std::cerr << x << ' ' << ccs1(x) << std::endl;
    
    std::cout << "# Convoluted Compton spectrum: " << std::endl;
    for ( double x=0.0; x<1200.; x+=1.0 ) {
      std::cout << x << ' ' << ccs1(x) << std::endl;
    }
    std::cout << std::endl << std::endl;

    double scs_err;
    double scs=sum_compton_spectrum(hnu,scs_err);
    std::cerr << "Integral of Compton spectrum at " 
	      << hnu << " keV =" << scs << "+/-" << scs_err << std::endl;

    double sncs_err;
    double sncs=sum_norm_compton_spectrum(hnu,sncs_err);
    std::cerr << "Integral of normalized Compton spectrum at " 
	      << hnu << " keV =" << sncs << "+/-" << sncs_err << std::endl;

    double sccs_err;
    double sccs;
    
    std::ofstream fsccs("sccs.data");
    fsccs << 0 << ' ' << scs << ' ' << scs_err << std::endl;
    for ( r=0.05; r<0.25; r+=0.01 ) { 
      sccs=sum_convoluted_compton_spectrum(hnu,r,sccs_err);
      std::cerr << "Integral of convoluted Compton spectrum at " 
		<< hnu << " keV and r=" << r << " =" << sccs << "+/-" << sccs_err << std::endl;
      fsccs << r << ' ' << sccs << ' ' << sccs_err << std::endl;
    }

    tabulate_compton_spectra();

  }
  catch(std::exception & x){
    std::cerr << "error: " << x.what() << std::endl; 
    error_code=EXIT_FAILURE;
  }
  catch(...){
    std::cerr << "error: " << "unexpected error!" << std::endl; 
    error_code=EXIT_FAILURE;
  }
  return error_code;
}

// end of test_multimin.cxx

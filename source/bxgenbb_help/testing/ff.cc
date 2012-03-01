// FermiFunction.cc

#include "NemoBB/FermiFunction.h"

FermiFunction::FermiFunction()
    {
      setZ(2);
      setA(6.);
    }

FermiFunction::FermiFunction( int z_ , double a_ )
    {
      setZ(z_);
      setA(a_);
      //cerr << "debug: FermiFunction.ctor: z=" << z_ << endl;
      //cerr << "debug: FermiFunction.ctor: a=" << a_ << endl;
    }

FermiFunction::FermiFunction(  const FermiFunction & ff_ )
    {
      setZ(ff_.getZ());
      setA(ff_.getA());
    }

double FermiFunction::approxNR( double e_ ) const
    {
      double alpha = PhysicalConstants::ALPHA;
      double me    = PhysicalConstants::ME_MEV;
      double beta  = Kinematics::w2b(e_+me,me);
      double t     = 2.*M_PI*alpha*z/beta;
      return(t/(1.-::exp(-t)));
    }

double FermiFunction::operator()( double t_ ) const 
    {
      return(eval(t_,BDBuhring::F0L0_VALUE));
    }

double FermiFunction::eval( double t_ , int code_value_ ) const 
    {
      double hbarc  = PhysicalConstants::HBARC_MEVFM; //MeV.fm
      double alpha  = PhysicalConstants::ALPHA;
      double r0     = 1.2; // fm
      double me     = PhysicalConstants::ME_MEV; // MeV
      // unit: me(c2)
      double we     = (t_+me)/me;
      double pe     = Kinematics::w2p(we,1.);
      double aZ     = alpha*z;
      //cerr << "debug: FermiFunction.eval: alpha = " << alpha << endl;
      //cerr << "debug: FermiFunction.eval: z = " << z << endl;
      //cerr << "debug: FermiFunction.eval: aZ = " << aZ << endl;
      double gamma1 = ::sqrt(1.-aZ*aZ);
      //cerr << "debug: FermiFunction.eval: gamma1 = " << gamma1 << endl;
      double y      = aZ*we/pe;
      double R      = r0*::exp(::log(a)/3.)*me/hbarc;
      double F0 = 4.*::exp(2.*(gamma1-1.)*::log(2.*pe*R))*::exp(M_PI*y);
      double g2 = gsl_sf_gamma(2.*gamma1+1.);  
      /* BUG
      gsl_sf_result res_lnr1;
      double lnr1;
      double x = 2.*gamma1+1.;
      //cerr << "debug: FermiFunction.eval: x = " << x << endl;
      int status1 = gsl_sf_lngamma_e(x,&res_lnr1);
      if ( status1 != GSL_SUCCESS ) {
	cerr << "error: FermiFunction.eval: GSL error: " << gsl_strerror(status1) << endl;
        throw runtime_error("FermiFunction.eval: GSL error at 'gsl_sf_lngamma_e' invocation!");
      }
      lnr1 = res_lnr1.val;
      double g2 = exp(2.*lnr1);  
      */
      double lnr;
      gsl_sf_result res_lnr, res_arg;
      int status = gsl_sf_lngamma_complex_e(gamma1,y,&res_lnr,&res_arg);
      if ( status != GSL_SUCCESS ) {
	cerr << "error: FermiFunction.eval: GSL error: " << gsl_strerror(status) << endl;
        throw runtime_error("FermiFunction.eval: GSL error at 'gsl_sf_lngamma_complex_e' invocation!");
      }
      lnr = res_lnr.val;
      /*
      double relErr = res_lnr.err/lnr;
      if ( ::fabs(relErr) > 1.e-6 ) {
	cerr << "FermiFunction.eval: GSL error: too large relative error!" << endl;	
      }
      */  
      double g1 = ::exp(lnr);
      F0 *= (g1*g1);
      F0 /= (g2*g2);
      double term = 1.-aZ*(we*R-7.*aZ/15.) - 0.5*gamma1*aZ*R/we;
      double L0 = 0.5*(1.+gamma1)*term;
      L0=1.0;
      //cerr << we << ' ' << L0 << endl;
      if ( code_value_ == BDBuhring::L0_VALUE ) return(L0);
      if ( code_value_ == BDBuhring::F0_VALUE ) return(F0);
      if ( code_value_ == BDBuhring::F0L0_VALUE ) return(F0*L0);
      return(0.0);
    }

int FermiFunction::main( int argc_ , char ** argv_ )
  {
    int    Z;
    cout << "Enter Z (daughter): " << endl;
    cin >> Z;
    if (!cin || Z<0 ) {
      cerr << "error: invalid Z value!" << endl;
      return(1);
    }
    double A;
    cout << "Enter A: " << endl;
    cin >> A;
    if (!cin || A<0 ) {
      cerr << "error: invalid A value!" << endl;
      return(1);
    }
    FermiFunction ff(Z,A);
    double prMin = 0.1; // MeV
    double prMax = 50.; // MeV
    int    nsample = 100;
    double dpr = (prMax-prMin)/nsample;
    for ( double pr=prMin ; pr<prMax; ) {
      double wr = ::sqrt(pr*pr+1.);
      double t = (wr-1.)*PhysicalConstants::ME_MEV;
      cout << pr << ' ' << ff(t) << ' ' << ff.approxNR(t) << endl;
      if (pr<1.0) pr+=0.1;
      else if (pr<4.0) pr+=0.2;
      else if (pr<8.0) pr+=0.5;
      else pr+=1;
    }
    /*
    for ( double pr=prMin; pr<prMax; pr+=dpr ) {
      double wr = ::sqrt(pr*pr+1.);
      double t = (wr-1.)*PhysicalConstants::ME_MEV;
      cout << pr << ' ' << ff(t) << ' ' << ff.approxNR(t) << endl;
      }
    */
    return(0);
  }

// end of  FermiFunction.cc

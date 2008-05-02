// mygsl::fft_real.h

#ifndef __mygsl__fft_real_h 
#define __mygsl__fft_real_h 1

#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <stdexcept>
#include <vector>
#include <complex>
#include <algorithm>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>

namespace mygsl {

  class fft_real
    {
      bool   __initialized;
      size_t _n;
      double _start;
      double _stop;
      double _step;       // grid interval/scanning period
      double _F0;         // scanning frequency
      double * _data;     // sampled data
      double * _data_rec; 
      double * _freq;
      double * _tmp;
      double _Nyquist_freq;
      double _min_freq_cutoff;
      double _max_freq_cutoff;

      // gsl stuff:
      gsl_fft_real_wavetable *        __the_real;
      gsl_fft_halfcomplex_wavetable * __hc;
      gsl_fft_real_workspace *        __work;

    public:

      fft_real()
	{
	  __initialized=false;
	  _n=0;
	  _start=0.0;
	  _step=1.0;
	  _F0=0.0;
	  _Nyquist_freq=0.0;
	  _stop=0.0;
	  _data=0;
	  _data_rec=0;
	  _freq=0;
	  _tmp=0;
	  // gsl stuff:
	  __the_real=0;
	  __hc=0;
	  __work=0;
	  _min_freq_cutoff=0.0;
	  _max_freq_cutoff=_Nyquist_freq;
	}

      ~fft_real()
	{
	  if ( __initialized ) reset();
	}

      void 
	reset()
	{
	  if (!__initialized) 
	    throw std::runtime_error("fft_real::reset: Not initialized!");
	  __initialized=false;
	  gsl_fft_real_workspace_free(__work);
	  gsl_fft_real_wavetable_free(__the_real);
	  gsl_fft_halfcomplex_wavetable_free(__hc);
	  if ( _data ) delete[] _data;
	  if ( _freq ) delete[] _freq;
	  if ( _tmp )  delete[] _tmp;
	  if ( _data_rec )  delete[] _data_rec;
	  _n=0;
	  _start=0.0;
	  _step=1.0;
	  _stop=0.0;
	  _data=0;
	  _data_rec=0;
	  _freq=0;
	  _tmp=0;
	  _F0=0.0;
	  _Nyquist_freq=0.0;
	  _min_freq_cutoff=0.0;
	  _max_freq_cutoff=_Nyquist_freq;
	}

      void 
	init(const std::vector<double> & sample_ , 
	     double start_ , 
	     double step_ ,
	     double min_freq_cutoff_ ,
	     double max_freq_cutoff_)
	{
	  if (__initialized) 
	    throw std::runtime_error("fft_real::init: Already initialized!");
	  _n = sample_.size();
	  _start=start_;
	  _step=step_;
	  _stop=_start+_n*_step;
	  _data = new double[_n];
	  _freq = new double[_n];
	  _tmp = new double[_n];
	  _data_rec = new double[_n];
	  for (size_t i = 0; i < _n; i++) 
	    {
	      _data[i]=0.0;
	      _data_rec[i]=0.0;
	      _freq[i]=0.0;
	      _tmp[i]=0.0; 
	    }
	  _F0=1.0/_step;
	  std::copy(sample_.begin(),sample_.end(),_data);
	  _Nyquist_freq=0.5*_F0;
	  _min_freq_cutoff=min_freq_cutoff_;
	  _max_freq_cutoff=_Nyquist_freq;
	  if (_max_freq_cutoff > 0.0) 
	    {
	      _max_freq_cutoff=max_freq_cutoff_;
	    }
	  __work = gsl_fft_real_workspace_alloc(_n);
	  __the_real = gsl_fft_real_wavetable_alloc(_n);
	  __hc   = gsl_fft_halfcomplex_wavetable_alloc(_n);
	  __initialized=true;
	}


      void 
	init(const std::vector<double> & sample_ , 
	     double start_ , 
	     double step_ ,
	     double max_freq_cutoff_)
	{
	  init(sample_, start_, step_, 0.0, max_freq_cutoff_);
	}

      /*
	double compute( double t_ ) const
	{
	std::complex<double> r=0.0;
	std::complex<double> j(0,1);

	for ( size_t i=0; i<_n; i++ ) {
	r = r + _freq[i]*(
	std::cos(2.*M_PI*(i/_n)*(t_/_step))
	+ j*std::sin(2.*M_PI*(i/_n)*(t_/_step)));
	}
	return (r.real()/_n);
	}
      */

      double 
	get_frequency_step() const
	{
	  return _F0/_n;
	}

      void 
	dump( 
	     std::ostream & out_ , bool dump_arrays_ = true 
	     ) const
	{
	  out_ << "# fft_real: " << std::endl;
	  out_ << "# |-- sample size  = " << _n << std::endl;
	  out_ << "# |-- start        = " << _start << std::endl;
	  out_ << "# |-- stop         = " << _stop << std::endl;
	  out_ << "# |-- step         = " << _step << std::endl;
	  out_ << "# |-- min f cutoff = " << _min_freq_cutoff << std::endl;
	  out_ << "# |-- max f cutoff = " << _max_freq_cutoff << std::endl;
	  out_ << "# |-- F0           = " << _F0 << std::endl;
	  out_ << "# |-- Freq. step   = " << get_frequency_step() << std::endl;
	  out_ << "# `-- Nyquist freq = " << _Nyquist_freq << std::endl;
	  if (! dump_arrays_) return; 
	  double t=_start;
	  for (size_t i = 0; i < _n; i++) 
	    {
	      out_ << t << ' ' << _data[i] << std::endl;
	      t+=_step;
	    }
	  out_ << std::endl;
	  out_ << std::endl;
    
	  double freq=0.0;   
	  for (size_t i = 0; i < _n; i++) 
	    {
	      out_ << freq << ' ' << _freq[i] << std::endl;
	      freq+=_F0/_n;
	    }
	  out_ << std::endl;
	  out_ << std::endl;    

	  t=_start;
	  for (size_t i = 0; i < _n; i++) 
	    {
	      out_ << t << ' ' << _data_rec[i] << std::endl;
	      t+=_step;
	    }
	  out_ << std::endl;
	  out_ << std::endl;

	  /*
	  // this part is not sure... 
	  t=_start;
	  for ( double time=0.0; time<= 50; time+=0.01 ) {
	  out_ << time << ' ' << compute(time) << std::endl;
	  }
	  out_ << std::endl;
	  out_ << std::endl;
	  */

	}

      void 
	compute_fourier_spectrum(std::vector<double> & freq_spectrum_) const 
	{
	  freq_spectrum_.clear();
	  freq_spectrum_.assign(_n,0.0);
	  for (size_t i = 0; i < _n; i++) 
	    {
	      freq_spectrum_[i] = _freq[i];
	    }
	}

      void 
	compute_filtered_data(std::vector<double> & filtered_data_) const
	{
	  filtered_data_.clear();
	  filtered_data_.assign(_n,0.0);
	  for (size_t i = 0; i < _n; i++) 
	    {
	      filtered_data_[i]=_data_rec[i];
	    }
	}

    private:
      void 
	__process_filter()
	{
	  double freq=0.0;   
	  for (size_t i = 0; i < _n; i++) 
	    {

	      if (freq <  _min_freq_cutoff) _freq[i] = 0.0;
	      if (freq >= _max_freq_cutoff) _freq[i] = 0.0;
	      freq += (1. / _step / _n);
	    }
	}

    public:
      void 
	process()
	{
	  if (!__initialized) 
	    throw std::runtime_error("fft_real::process: Not initialized!");
	  for (size_t i = 0; i < _n; i++) _tmp[i] = _data[i];

	  gsl_fft_real_transform(_tmp,1,_n,__the_real,__work);
	  for (size_t i = 0; i < _n; i++) _freq[i] = _tmp[i];

	  // apply filter:
	  __process_filter();

	  for (size_t i = 0; i < _n; i++) _tmp[i] = _freq[i];
	  gsl_fft_halfcomplex_inverse(_tmp,1,_n,__hc,__work);

	  for (size_t i = 0; i < _n; i++) _data_rec[i] = _tmp[i];
	  //__processed=true;
	}


      static int main(int argc_ , char ** argv_) 
	{
	  bool debug=false;
	  double t_step=0.5;

	  int iarg=1;
	  while (iarg < argc_) 
	    {
	      std::string arg=argv_[iarg];
	      if (arg == "-d") 
		{
		  debug=true;
		}
	      if (arg == "-step=0.5") t_step=0.5;
	      if (arg == "-step=0.25") t_step=0.25;
	      if (arg == "-step=0.1") t_step=0.1;
	      if (arg == "-step=0.01") t_step=0.01;
	    }

	  std::vector<double> data;
	  double t_min=0.0;
	  double t_max=100.0;

	  size_t n_steps=(size_t) ((t_max-t_min+0.1*t_step)/t_step);
	  if (debug) 
	    {
	      std::cerr << "DEBUG: " << "t_min   = " << t_min << std::endl;
	      std::cerr << "DEBUG: " << "t_max   = " << t_max << std::endl;
	      std::cerr << "DEBUG: " << "t_step  = " << t_step << std::endl;
	      std::cerr << "DEBUG: " << "n_steps = " << n_steps << std::endl;
	    }
	  double Nyquist_frequency = 0.5/t_step;
	  if (debug) std::cerr << "DEBUG: " 
			       << "Nyquist_frequency = " 
			       << Nyquist_frequency << std::endl;
	  double min_freq_cut=0.0;
	  double max_freq_cut=Nyquist_frequency;
	  if (debug) std::cerr << "DEBUG: " 
			       << "fft_real::main: argc=" 
			       << argc_ << std::endl;

	  /*
	    if ( argc_ >= 2 ) {
      
	    if ( debug ) std::cerr << "DEBUG: " 
	    << "fft_real::main: argv[1]='" 
	    << argv_[1] << "'" << std::endl;
	    std::string arg(argv_[1]);
	    if ( debug ) std::cerr << "DEBUG: " 
	    << "fft_real::main: arg='" 
	    << arg << "'" << std::endl;
	    std::istringstream iss(arg);
	    iss >> max_freq_cut;
	    if ( !iss ) {
	    throw std::runtime_error("fft_real::main: Format error!");
	    }
	    }
	  */

	  if (debug) std::cerr << "DEBUG: " 
			       << "Maximum frequency cut= " 
			       << max_freq_cut << std::endl;
	  data.assign(n_steps,0.0);
	  for (size_t i = data.size()/5; i < 2 *data.size() / 4; i++) 
	    {
	      data[i] = 1.0;
	    }

	  fft_real fft;
	  fft.init(data, t_min, t_step, min_freq_cut, max_freq_cut);
	  fft.process();
	  fft.dump(std::cout);
	  if (debug) fft.dump(std::cerr,false);
	}

    };

}

#endif //  __mygsl__fft_real_h

// end of mygsl::fft_real.h

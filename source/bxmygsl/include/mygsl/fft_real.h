// -*-mode: c++; -*-
// mygsl::fft_real.h

#ifndef __mygsl__fft_real_h 
#define __mygsl__fft_real_h 1

#include <iostream>
#include <vector>

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

    fft_real ();

    ~fft_real ();

    void reset ();

    void init (const std::vector<double> & sample_ , 
	       double start_ , 
	       double step_ ,
	       double min_freq_cutoff_ ,
	       double max_freq_cutoff_);

    void init (const std::vector<double> & sample_ , 
	       double start_ , 
	       double step_ ,
	       double max_freq_cutoff_);

    double get_frequency_step () const;

    void dump (std::ostream & out_ , bool dump_arrays_ = true) const;

    void compute_fourier_spectrum (std::vector<double> & freq_spectrum_) const;

    void compute_filtered_data (std::vector<double> & filtered_data_) const;

  private:

    void __process_filter ();

  public:

    void process ();

    static int main (int argc_ , char ** argv_);

  };

}

#endif //  __mygsl__fft_real_h

// end of mygsl::fft_real.h

/// \file mygsl/fft_real.h

#ifndef MYGSL_FFT_REAL_H
#define MYGSL_FFT_REAL_H 1

// Standard library:
#include <iostream>
#include <vector>

// Third party:
// - GSL:
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>

namespace mygsl {

  /// \brief Fast Fourier transform algorithm
  class fft_real
  {
  public:

    fft_real();

    ~fft_real();

    void reset();

    void init(const std::vector<double> & sample_,
               double start_,
               double step_,
               double min_freq_cutoff_,
               double max_freq_cutoff_);

    void init(const std::vector<double> & sample_,
               double start ,
               double step_,
               double max_freq_cutoff_);

    double get_frequency_step() const;

    void dump(std::ostream & out_, bool dump_arrays_ = true) const;

    void compute_fourier_spectrum(std::vector<double> & freq_spectrum_) const;

    void compute_filtered_data(std::vector<double> & filtered_data_) const;

    void process();

  private:

    void _process_filter_();

  private:

    bool   _initialized_  = false;
    size_t _n_;
    double _start_;
    double _stop_;
    double _step_;       ///< Grid interval/scanning period
    double _F0_;         ///< Scanning frequency
    double * _data_ = nullptr;  ///< Sampled data
    double * _data_rec_ = nullptr; ///< Reconstructed data
    double * _freq_ = nullptr;
    double * _tmp_ = nullptr;
    double _Nyquist_freq_;
    double _min_freq_cutoff_;
    double _max_freq_cutoff_;

    // gsl stuff:
    gsl_fft_real_wavetable *        _the_real_ = nullptr;
    gsl_fft_halfcomplex_wavetable * _hc_ = nullptr;
    gsl_fft_real_workspace *        _work_ = nullptr;

  };

} // namespace mygsl 

#endif // MYGSL_FFT_REAL_H

// Local Variables:
// mode: c++
// coding: utf-8
// End:

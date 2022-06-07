// mygsl::fft_real.cc

// Ourselved:
#include <mygsl/fft_real.h>

// Standard library:
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <string>
#include <cmath>
#include <complex>
#include <algorithm>

// This project:
#include <datatools/exception.h>
#include <gsl/gsl_errno.h>
//#include <gsl/gsl_fft_real.h>
//#include <gsl/gsl_fft_halfcomplex.h>

namespace mygsl {

  using namespace std;

  fft_real::fft_real()
  {
    _initialized_ = false;
    _n_ = 0;
    _start_ = 0.0;
    _step_ = 1.0;
    _F0_ = 0.0;
    _Nyquist_freq_ = 0.0;
    _stop_ = 0.0;
    _data_ = nullptr;
    _data_rec_ = nullptr;
    _freq_ = nullptr;
    _tmp_ = nullptr;
    // gsl stuff:
    _the_real_ = nullptr;
    _hc_ = nullptr;
    _work_ = nullptr;
    _min_freq_cutoff_ = 0.0;
    _max_freq_cutoff_ = _Nyquist_freq_;
    return;
  }

  fft_real::~fft_real()
  {
    if (_initialized_) reset();
    return;
  }

  void fft_real::reset()
  {
    DT_THROW_IF(! _initialized_, std::logic_error, "Not initialized!");
    _initialized_=false;
    gsl_fft_real_workspace_free(_work_);
    gsl_fft_real_wavetable_free(_the_real_);
    gsl_fft_halfcomplex_wavetable_free(_hc_);
    if (_data_) delete[] _data_;
    if (_freq_) delete[] _freq_;
    if (_tmp_)  delete[] _tmp_;
    if (_data_rec_) delete[] _data_rec_;
    _n_ = 0;
    _start_ = 0.0;
    _step_ = 1.0;
    _stop_ = 0.0;
    _data_ = nullptr;
    _data_rec_ = nullptr;
    _freq_ = nullptr;
    _tmp_ = nullptr;
    _F0_ = 0.0;
    _Nyquist_freq_ = 0.0;
    _min_freq_cutoff_ = 0.0;
    _max_freq_cutoff_ = _Nyquist_freq_;
    return;
  }

  void fft_real::init(const std::vector<double> & sample_,
                      double start_,
                      double step_,
                      double min_freq_cutoff_,
                      double max_freq_cutoff_)
  {
    DT_THROW_IF(_initialized_, std::runtime_error, "Already initialized!");
    _n_ = sample_.size();
    _start_ = start_;
    _step_ = step_;
    _stop_ = _start_ + _n_ * _step_;
    _data_ = new double[_n_];
    _freq_ = new double[_n_];
    _tmp_ = new double[_n_];
    _data_rec_ = new double[_n_];
    for (size_t i = 0; i < _n_; i++) {
      _data_[i] = 0.0;
      _data_rec_[i] = 0.0;
      _freq_[i] = 0.0;
      _tmp_[i] = 0.0;
    }
    _F0_ = 1.0 / _step_;
    std::copy(sample_.begin(), sample_.end(), _data_);
    _Nyquist_freq_ = 0.5 * _F0_;
    _min_freq_cutoff_ = min_freq_cutoff_;
    _max_freq_cutoff_ = _Nyquist_freq_;
    if (_max_freq_cutoff_ > 0.0) {
      _max_freq_cutoff_ = max_freq_cutoff_;
    }
    _work_ = gsl_fft_real_workspace_alloc(_n_);
    _the_real_ = gsl_fft_real_wavetable_alloc(_n_);
    _hc_ = gsl_fft_halfcomplex_wavetable_alloc(_n_);
    _initialized_ = true;
    return;
  }

  void fft_real::init(const std::vector<double> & sample_,
                      double start_,
                      double step_,
                      double max_freq_cutoff_)
  {
    init(sample_, start_, step_, 0.0, max_freq_cutoff_);
    return;
  }

  double fft_real::get_frequency_step() const
  {
    return _F0_ / _n_;
  }
   
  double fft_real::get_nyquist_frequency() const
  {
    return _Nyquist_freq_;
  }

  void fft_real::dump(std::ostream & out_,  bool dump_arrays_) const
  {
    out_ << "# fft_real: " << std::endl;
    out_ << "# |-- sample size  = " << _n_ << std::endl;
    out_ << "# |-- start        = " << _start_ << std::endl;
    out_ << "# |-- stop         = " << _stop_ << std::endl;
    out_ << "# |-- step         = " << _step_ << std::endl;
    out_ << "# |-- min f cutoff = " << _min_freq_cutoff_ << std::endl;
    out_ << "# |-- max f cutoff = " << _max_freq_cutoff_ << std::endl;
    out_ << "# |-- F0           = " << _F0_ << std::endl;
    out_ << "# |-- Freq. step   = " << get_frequency_step() << std::endl;
    out_ << "# `-- Nyquist freq = " << _Nyquist_freq_ << std::endl;
    if (! dump_arrays_) return;
    double t = _start_;
    for (size_t i = 0; i < _n_; i++) {
      out_ << t << ' ' << _data_[i] << std::endl;
      t += _step_;
    }
    out_ << std::endl;
    out_ << std::endl;

    double freq = 0.0;
    for (size_t i = 0; i < _n_; i++) {
      out_ << freq << ' ' << _freq_[i] << std::endl;
      freq += _F0_ / _n_;
    }
    out_ << std::endl;
    out_ << std::endl;

    t = _start_;
    for (size_t i = 0; i < _n_; i++) {
      out_ << t << ' ' << _data_rec_[i] << std::endl;
      t += _step_;
    }
    out_ << std::endl;
    out_ << std::endl;

    /*
    // this part is not sure...
    t=_start_;
    for (double time=0.0; time<= 50; time+=0.01) {
    out_ << time << ' ' << compute (time) << std::endl;
    }
    out_ << std::endl;
    out_ << std::endl;
    */

  }

  void fft_real::compute_fourier_spectrum(std::vector<double> & freq_spectrum_) const
  {
    freq_spectrum_.clear();
    freq_spectrum_.assign(_n_, 0.0);
    for (size_t i = 0; i < _n_; i++) {
      freq_spectrum_[i] = _freq_[i];
    }
    return;
  }

  void fft_real::compute_filtered_data(std::vector<double> & filtered_data_) const
  {
    filtered_data_.clear();
    filtered_data_.assign(_n_, 0.0);
    for (size_t i = 0; i < _n_; i++) {
      filtered_data_[i] = _data_rec_[i];
    }
    return;
  }

  void fft_real::_process_filter_()
  {
    double freq = 0.0;
    for (size_t i = 0; i < _n_; i++) {
      if (freq <  _min_freq_cutoff_) _freq_[i] = 0.0;
      if (freq >= _max_freq_cutoff_) _freq_[i] = 0.0;
      freq += (1. / _step_ / _n_);
    }
    return;
  }

  void fft_real::process()
  {
    DT_THROW_IF(!_initialized_, std::runtime_error, "Not initialized!");
    for (size_t i = 0; i < _n_; i++) _tmp_[i] = _data_[i];

    gsl_fft_real_transform(_tmp_, 1, _n_, _the_real_, _work_);
    for (size_t i = 0; i < _n_; i++) _freq_[i] = _tmp_[i];

    // apply filter:
    _process_filter_();

    for (size_t i = 0; i < _n_; i++) _tmp_[i] = _freq_[i];
    gsl_fft_halfcomplex_inverse(_tmp_, 1, _n_, _hc_, _work_);

    for (size_t i = 0; i < _n_; i++) _data_rec_[i] = _tmp_[i];
    return;
  }

  /*
    double compute(double t_) const
    {
    std::complex<double> r=0.0;
    std::complex<double> j(0,1);

    for (size_t i=0; i<_n_; i++) {
    r = r + _freq_[i]* (
    std::cos (2.*M_PI* (i/_n_)* (t_/_step_))
    + j*std::sin (2.*M_PI* (i/_n_)* (t_/_step_)));
    }
    return (r.real()/_n_);
    }
  */

}

// end of mygsl::fft_real.cc


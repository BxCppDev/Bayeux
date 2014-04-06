// \file mygsl/linear_regression.cc

// Ourselves:
#include <mygsl/linear_regression.h>

// Standard library:
#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

// Third party:
// - GSL:
#include <gsl/gsl_errno.h>
#include <gsl/gsl_fit.h>
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/logger.h>

namespace mygsl {

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(linear_regression::fit_data,
                                                    "mygsl::linear_regression::fit_data")

  linear_regression::fit_data::fit_data ()
  {
    reset ();
    return;
  }

  linear_regression::fit_data::~fit_data ()
  {
    return;
  }

  void linear_regression::fit_data::reset ()
  {
    n = 0;
    status = GSL_CONTINUE;
    weighted = false;
    with_constant = false;
    c0 = 0.0;
    datatools::invalidate(c1);
    cov00 = 0.0;
    cov01 = 0.0;
    datatools::invalidate(cov11);
    datatools::invalidate(sumsq);
    datatools::invalidate(chisq);
    return;
  }

  bool linear_regression::fit_data::is_valid () const
  {
    return n > 1 && status == GSL_SUCCESS;
  }

  bool linear_regression::fit_data::is_weighted () const
  {
    return weighted;
  }

  bool linear_regression::fit_data::has_constant () const
  {
    return with_constant;
  }

  double linear_regression::fit_data::get_constant () const
  {
    return c0;
  }

  double linear_regression::fit_data::get_slope () const
  {
    return c1;
  }

  size_t linear_regression::fit_data::get_ndof () const
  {
    return n - 2;
  }

  double linear_regression::fit_data::get_chisq () const
  {
    DT_THROW_IF (! is_weighted(), std::logic_error, "Not computed! Use 'get_sumsq' !");
    return chisq;
  }

  double linear_regression::fit_data::get_sumsq () const
  {
    DT_THROW_IF (is_weighted(), std::logic_error, "Not computed! Use 'get_chisq'!");
    return sumsq;
  }

  double linear_regression::fit_data::get_slope_err () const
  {
    return std::sqrt(cov11);
  }

  double linear_regression::fit_data::get_constant_err () const
  {
    return std::sqrt(cov00);
  }

  /* ********************** */

  linear_regression::function::function(const fit_data & fd_)
  {
    set_fit_data(fd_);
    return;
  }

  void linear_regression::function::set_fit_data(const fit_data & fd_)
  {
    _fit_data_ = fd_;
    return;
  }

  const linear_regression::fit_data & linear_regression::function::get_fit_data() const
  {
    return _fit_data_;
  }

  void linear_regression::function::eval_err (double x_,
                                              double & y_,
                                              double & y_err_) const
  {
    DT_THROW_IF (! _fit_data_.is_valid(), std::logic_error , "Fit data are not valid !");
    int gsl_err;
    if (_fit_data_.has_constant ()) {
      gsl_err = gsl_fit_linear_est (x_,
                                    _fit_data_.c0,
                                    _fit_data_.c1,
                                    _fit_data_.cov00,
                                    _fit_data_.cov01,
                                    _fit_data_.cov11,
                                    &y_,
                                    &y_err_);
    } else {
      gsl_err = gsl_fit_mul_est (x_,
                                 _fit_data_.c1,
                                 _fit_data_.cov11,
                                 &y_,
                                 &y_err_);
    }
    DT_THROW_IF (gsl_err != GSL_SUCCESS,std::logic_error, "Cannot evaluate value !");
    return;
  }

  double linear_regression::function::_eval (double x_) const
  {
    DT_THROW_IF (! _fit_data_.is_valid(),std::logic_error , "Fit data are not valid !");
    double y, y_err;
    eval_err (x_, y, y_err);
    return y;
  }

  /* ********************** */

  linear_regression::linear_regression ()
  {
    _initialized_ = false;
    _can_weighted_ = false;
    _fit_data_.reset();
    _delete_ = false;
    _x_ = 0;
    _y_ = 0;
    _w_ = 0;
    return;
  }

  linear_regression::linear_regression (const std::vector<datapoint> & p_)
  {
    _initialized_ = false;
    _can_weighted_ = false;
    _fit_data_.reset ();
    _delete_ = false;
    _x_ = 0;
    _y_ = 0;
    _w_ = 0;
    init (p_);
    return;
  }

  linear_regression::linear_regression (size_t npoints_,
                                        const double * x_,
                                        const double * y_,
                                        const double * w_)
  {
    _initialized_ = false;
    _can_weighted_ = false;
    _fit_data_.reset ();
    _delete_ = false;
    _x_ = 0;
    _y_ = 0;
    _w_ = 0;
    init (npoints_, x_, y_, w_);
    return;
  }

  linear_regression::linear_regression (const std::vector<double> & x_,
                                        const std::vector<double> & y_,
                                        const std::vector<double> & w_)
  {
    _initialized_ = false;
    _can_weighted_ = false;
    _fit_data_.reset ();
    _delete_ = false;
    _x_ = 0;
    _y_ = 0;
    _w_ = 0;
    init (x_, y_, w_);
    return;
  }

  linear_regression::linear_regression (const std::vector<double> & x_,
                                        const std::vector<double> & y_)
  {
    _initialized_ = false;
    _can_weighted_ = false;
    _fit_data_.reset ();
    _delete_ = false;
    _x_ = 0;
    _y_ = 0;
    _w_ = 0;
    init (x_, y_);
    return;
  }

  linear_regression::~linear_regression ()
  {
    reset ();
    return;
  }

  bool linear_regression::is_initialized () const
  {
    return _initialized_;
  }

  bool linear_regression::can_weighted () const
  {
    return _can_weighted_;
  }

  void linear_regression::init (const std::vector<datapoint> & p_)
  {
    DT_THROW_IF (p_.size() < MINIMUM_NUMBER_OF_DATA_POINTS,
                 std::logic_error, "Not enough data points !");
    const size_t n = p_.size ();
    _can_weighted_ = true;
    _delete_ = true;
    _x_ = new double [n];
    _y_ = new double [n];
    _w_ = new double [n];
    _can_weighted_ = true;
    for (size_t i = 0; i < n; i++) {
      _x_[i] = p_[i].x ();
      _y_[i] = p_[i].y ();
      if (! p_[i].is_weighted ()) {
        DT_LOG_WARNING(datatools::logger::PRIO_WARNING, "Datapoint #" << i << " is not weighted !");
        _can_weighted_ = false;
      }
      _w_[i] = p_[i].sigma_y ();
    }
    _fit_data_.n = n;
    _initialized_ = true;
    return;
  }


  void linear_regression::init (size_t npoints_,
                                const double * x_,
                                const double * y_,
                                const double * w_)
  {
    DT_THROW_IF (npoints_ < MINIMUM_NUMBER_OF_DATA_POINTS,
                 std::logic_error, "Not enough datapoints !");
    const size_t n = npoints_;
    _can_weighted_ = true;
    _delete_ = false;
    _x_ = const_cast<double *> (x_);
    _y_ = const_cast<double *> (y_);
    _w_ = 0;
    _can_weighted_ = false;
    if (w_ != 0) {
      _w_ = const_cast<double *> (w_);
      _can_weighted_ = true;
      for (size_t i = 0; i < n; i++) {
        if (isnan(_w_[i])) {
          DT_LOG_WARNING(datatools::logger::PRIO_WARNING, "Datapoint #" << i << " is not weighted !");
          _can_weighted_ = false;
        }
      }
    }
    _fit_data_.n = n;
    _initialized_ = true;
    return;
  }

  void linear_regression::init (const std::vector<double> & x_,
                                const std::vector<double> & y_)
  {
    std::vector<double> w_no;
    init (x_, y_, w_no);
    return;
  }

  void linear_regression::init (const std::vector<double> & x_,
                                const std::vector<double> & y_,
                                const std::vector<double> & w_)
  {
    DT_THROW_IF (x_.size () < MINIMUM_NUMBER_OF_DATA_POINTS,
                 std::logic_error, "Not enough datapoints !");
    DT_THROW_IF (x_.size () != y_.size (), std::logic_error, "Data vectors' dimensions do not match !");
    if (w_.size ()) {
      DT_THROW_IF (y_.size () != w_.size (), std::logic_error, "Error vector's dimension and data vactor do not match !");
      _can_weighted_ = true;
    } else {
      _can_weighted_ = false;
    }
    const size_t n = x_.size ();

    // 2009-10-28 FM: std::vector::data() method is not available:
    /*
      _delete_ = false;
      _x_ = const_cast<double *> (x_.data ());
      _y_ = const_cast<double *> (y_.data ());
    */
    _delete_ = false;
    _x_ = const_cast<double *> (&x_[0]);
    _y_ = const_cast<double *> (&y_[0]);
    /*
    _delete_ = true;
    _x_ = new double [n];
    _y_ = new double [n];
    for (size_t i = 0; i < n; i++) {
      _x_[i] = x_[i];
      _y_[i] = y_[i];
    }
    */

    _w_ = 0;
    if (w_.size () != 0) {
      // 2009-10-28 FM: missing std::vector::data() method:
      //_w_ = const_cast<double *>(w_.data ());
      _w_ = const_cast<double *> (&w_[0]);
      /*
      _w_ = new double [n];
      */
      _can_weighted_ = true;
      for (size_t i = 0; i < n; i++) {
        _w_[i] = w_[i];
        if (isnan (_w_[i])) {
          DT_LOG_WARNING(datatools::logger::PRIO_WARNING, "Datapoint #" << i << " is not weighted !");
          _can_weighted_ = false;
        }
      }
    }
    _fit_data_.n = n;
    _initialized_ = true;
  }

  void linear_regression::reset ()
  {
    _initialized_ = false;
    _can_weighted_ = false;
    _fit_data_.reset ();
    if (_delete_) {
      if (_x_ != 0) {
        delete [] _x_;
      }
      if (_y_ != 0) {
        delete [] _y_;
        _y_ = 0;
      }
      if (_w_ != 0) {
        delete [] _w_;
      }
    }
    _delete_ = false;
    _x_ = 0;
    _y_ = 0;
    _w_ = 0;
    return;
  }

  const linear_regression::fit_data & linear_regression::get_fit_data () const
  {
    return _fit_data_;
  }

  bool linear_regression::fit_linear ()
  {
    DT_THROW_IF (! is_initialized (),std::logic_error , "Not initialized !");
    int gsl_err =  gsl_fit_linear (_x_, 1,
                                   _y_, 1,
                                   _fit_data_.n,
                                   &_fit_data_.c0,
                                   &_fit_data_.c1,
                                   &_fit_data_.cov00,
                                   &_fit_data_.cov01,
                                   &_fit_data_.cov11,
                                   &_fit_data_.sumsq);
    _fit_data_.status = gsl_err;
    _fit_data_.weighted = false;
    _fit_data_.with_constant = true;
    return gsl_err == GSL_SUCCESS;
  }

  bool linear_regression::fit_weighted_linear ()
  {
    DT_THROW_IF (! is_initialized (),std::logic_error , "Not initialized !");
    DT_THROW_IF (! _can_weighted_,std::logic_error ,"Cannot perform weighted fit !");
    int gsl_err =  gsl_fit_wlinear (_x_, 1,
                                    _w_, 1,
                                    _y_, 1,
                                    _fit_data_.n,
                                    &_fit_data_.c0,
                                    &_fit_data_.c1,
                                    &_fit_data_.cov00,
                                    &_fit_data_.cov01,
                                    &_fit_data_.cov11,
                                    &_fit_data_.chisq);
    _fit_data_.status = gsl_err;
    _fit_data_.weighted = true;
    _fit_data_.with_constant = true;
    return gsl_err == GSL_SUCCESS;
  }

  bool linear_regression::fit_linear_no_constant ()
  {
    DT_THROW_IF (! is_initialized (),std::logic_error , "Not initialized !");
    int gsl_err =  gsl_fit_mul (_x_, 1,
                                _y_, 1,
                                _fit_data_.n,
                                &_fit_data_.c1,
                                &_fit_data_.cov11,
                                &_fit_data_.sumsq);
    _fit_data_.status = gsl_err;
    _fit_data_.weighted = false;
    _fit_data_.with_constant = false;
    _fit_data_.c0 = 0.0;
    _fit_data_.cov00 = 0.0;
    _fit_data_.cov01 = 0.0;
    return gsl_err == GSL_SUCCESS;
  }

  bool linear_regression::fit_weighted_linear_no_constant ()
  {
    DT_THROW_IF (! is_initialized (),std::logic_error , "Not initialized !");
    DT_THROW_IF (! _can_weighted_,std::logic_error ,"Cannot perform weighted fit !");
    int gsl_err =  gsl_fit_wmul (_x_, 1,
                                 _w_, 1,
                                 _y_, 1,
                                 _fit_data_.n,
                                 &_fit_data_.c1,
                                 &_fit_data_.cov11,
                                 &_fit_data_.chisq);
    _fit_data_.status = gsl_err;
    _fit_data_.weighted = true;
    _fit_data_.with_constant = false;
    _fit_data_.c0 = 0.0;
    _fit_data_.cov00 = 0.0;
    _fit_data_.cov01 = 0.0;
    return gsl_err == GSL_SUCCESS;
  }

} // namespace mygsl

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

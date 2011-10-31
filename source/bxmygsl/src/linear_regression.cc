// mygsl::linear_regression.cc

#include <mygsl/linear_regression.h>

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_fit.h>

namespace mygsl {

  linear_regression::fit_data::fit_data ()
  {
    reset ();
    return;
  }

  void linear_regression::fit_data::reset ()
  {
    status = GSL_CONTINUE;
    weighted = false;
    with_constant = false;
    n = 0;
    sumsq = -1.0;
    chisq = -1.0;
    return;
  }

  bool linear_regression::fit_data::is_valid () const
  {
    return status == GSL_SUCCESS;
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
    if (! is_weighted ()) 
      {
        throw runtime_error ("linear_regression::fit_data::get_sumsq: Not computed! Use 'get_sumsq'!");
      }
    return chisq;
  }

  double linear_regression::fit_data::get_sumsq () const
  {
    if (is_weighted ()) 
      {
        throw runtime_error ("linear_regression::fit_data::get_sumsq: Not computed! Use 'get_chisq'!");
      }
    return sumsq;
  }

  double linear_regression::fit_data::get_slope_err () const
  {
    return sqrt (cov11);
  }

  double linear_regression::fit_data::get_constant_err () const
  {
    return sqrt (cov00);
  }

  /************************/

  linear_regression::linear_regression ()
  {
    _initialized_ = false;
    _can_weighted_ = false;
    _fit_data_.reset ();
    _delete_ = false;
    _x_ = 0;
    _y_ = 0;
    _w_ = 0;
    return;
  }

  linear_regression::linear_regression (const vector<datapoint> & p_)
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

  linear_regression::linear_regression (const vector<double> & x_, 
                                        const vector<double> & y_, 
                                        const vector<double> & w_)
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

  linear_regression::linear_regression (const vector<double> & x_, 
                                        const vector<double> & y_)
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

  void linear_regression::init (const vector<datapoint> & p_)
  {
    if (p_.size () < 3)
      {
        ostringstream message;
        message << "linear_regression::init: Not enough datapoints!";
        throw runtime_error (message.str ());
      }
    size_t n = p_.size ();
    _can_weighted_ = true;    
    _delete_ = true;
    _x_ = new double [n];
    _y_ = new double [n];
    _w_ = new double [n];
    _can_weighted_ = true;
    int count = 0;
    for (int i = 0; i < n; i++)
      {
        _x_[i] = p_[i].x ();
        _y_[i] = p_[i].y ();
        if (! p_[i].is_weighted ())
          {
            clog << "WARNING: linear_regression::init: Datapoint #" << count << " is not weighted!" << endl;
            _can_weighted_ = false;
          }
        _w_[i] = p_[i].sigma ();
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
    if (npoints_ < 3)
      {
        ostringstream message;
        message << "linear_regression::init: Not enough datapoints!";
        throw runtime_error (message.str ());
      }
    size_t n = npoints_;
    _can_weighted_ = true;    
    _delete_ = false;
    _x_ = const_cast<double *> (x_);
    _y_ = const_cast<double *> (y_);
    _w_ = 0;
    _can_weighted_ = false;
    int count = 0;
    if (w_ != 0)
      {
        _w_ = const_cast<double *> (w_);
        _can_weighted_ = true;
        for (int i = 0; i < n; i++)
          {
            if (isnan (_w_[i]))
              {
                clog << "WARNING: linear_regression::init: Datapoint #" << count << " is not weighted!" << endl;
                _can_weighted_ = false;
              }
          }
      }
    _fit_data_.n = n;
    _initialized_ = true;
    return;
  }

  void linear_regression::init (const vector<double> & x_, 
                                const vector<double> & y_)
  {
    vector<double> w_no;
    init (x_, y_, w_no);
    return;
  }

  void linear_regression::init (const vector<double> & x_, 
                                const vector<double> & y_, 
                                const vector<double> & w_)
  {
    if (x_.size () < 3)
      {
        ostringstream message;
        message << "linear_regression::init: Not enough datapoints!";
        throw runtime_error (message.str ());
      }
    if (x_.size () != y_.size ())
      {
        ostringstream message;
        message << "linear_regression::init: Data vectors' dimensions do not match!";
        throw runtime_error (message.str ());
      }
    if (w_.size ())
      {
        if (y_.size () != w_.size ())
          {
            ostringstream message;
            message << "linear_regression::init: Error vector's dimension and data vactor do not match!";
            throw runtime_error (message.str ());
          }
        _can_weighted_ = true;    
      }
    else
      {
        _can_weighted_ = false;    
      }
    size_t n = x_.size ();

    // 2009-10-28 FM: std::vector::data() method is not available:
    /*
      _delete_ = false;
      _x_ = const_cast<double *> (x_.data ());
      _y_ = const_cast<double *> (y_.data ());
    */
    _delete_ = true;
    _x_ = new double [n];
    _y_ = new double [n];
    for (int i = 0; i < n; i++)
      {
        _x_[i] = x_[i];
        _y_[i] = y_[i];
      }

    _w_ = 0;
    int count = 0;
    if (w_.size () != 0)
      {
        // 2009-10-28 FM: missing std::vector::data() method:
        //_w_ = const_cast<double *>(w_.data ());
        _w_ = new double [n];
        _can_weighted_ = true;
        for (int i = 0; i < n; i++)
          {
            _w_[i] = w_[i];
            if (isnan (_w_[i]))
              {
                clog << "WARNING: linear_regression::init: Datapoint #" << count << " is not weighted!" << endl;
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
    if (_delete_)
      {
        if (_x_ != 0) 
          {
            delete [] _x_;
          }
        if (_y_ != 0) 
          {
            delete [] _y_;
            _y_ = 0;
          }
        if (_w_ != 0) 
          {
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
    if (! is_initialized ())
      {
        ostringstream message;
        message << "linear_regression::fit_linear: Not initialized!";
        throw runtime_error (message.str ());
      }

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
    if (! is_initialized ())
      {
        ostringstream message;
        message << "linear_regression::fit_linear: Not initialized!";
        throw runtime_error (message.str ());
      }
    if (! _can_weighted_)
      {
        ostringstream message;
        message << "linear_regression::fit_linear: Cannot perform weighted fit!";
        throw runtime_error (message.str ());
      }
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
    if (! is_initialized ())
      {
        ostringstream message;
        message << "linear_regression::fit_linear_no_constant: Not initialized!";
        throw runtime_error (message.str ());
      }

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
    if (! is_initialized ())
      {
        ostringstream message;
        message << "linear_regression::fit_weighted_linear_no_constant: Not initialized!";
        throw runtime_error (message.str ());
      }
    if (! _can_weighted_)
      {
        ostringstream message;
        message << "linear_regression::fit_weighted_linear_no_constant: Cannot perform weighted fit!";
        throw runtime_error (message.str ());
      }
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
   
  void linear_regression::eval_err (double x_, 
                                    double & y_, 
                                    double & y_err_) const
  {
    if (! is_initialized ())
      {
        ostringstream message;
        message << "linear_regression::eval: Not initialized!";
        throw runtime_error (message.str ());
      }
    int gsl_err;
    if (_fit_data_.has_constant ())
      {
        gsl_err = gsl_fit_linear_est (x_,
                                      _fit_data_.c0, 
                                      _fit_data_.c1, 
                                      _fit_data_.cov00, 
                                      _fit_data_.cov01, 
                                      _fit_data_.cov11, 
                                      &y_,
                                      &y_err_);
      }
    else
      {
        gsl_err = gsl_fit_mul_est (x_,
                                   _fit_data_.c1, 
                                   _fit_data_.cov11, 
                                   &y_,
                                   &y_err_);
      }
    if (gsl_err != GSL_SUCCESS)
      {
        ostringstream message;
        message << "linear_regression::eval_err: Cannot evaluate value!";
        throw runtime_error (message.str ());
      }
    return;
  }

  double linear_regression::eval (double x_) const
  {
    if (! is_initialized ())
      {
        ostringstream message;
        message << "linear_regression::eval: Not initialized!";
        throw runtime_error (message.str ());
      }
    
    double y, y_err;
    eval_err (x_, y, y_err);
    return y;
  }
  
}

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

// end of mygsl::linear_regression.cc

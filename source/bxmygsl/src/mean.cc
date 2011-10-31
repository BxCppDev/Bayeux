// mygsl::mean.cc

#include <mygsl/mean.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <functional>
#include <limits>
#include <cmath>

namespace mygsl {
  
  size_t arithmetic_mean::get_n () const
  {
    return _n_;
  }

  double arithmetic_mean::get_sum () const
  {
    return _sum_;
  }

  double arithmetic_mean::get_sum_of_squared () const
  {
    return _sum_of_squared_;
  }

  bool arithmetic_mean::is_valid () const
  {
    return _n_ > 0;
  }

  double arithmetic_mean::get_mean () const
  {
    if (_n_ == 0)
      {
	throw runtime_error ("arithmetic_mean::get_mean: Not enough value !");
      }
    return _sum_ / _n_;
  }

  double arithmetic_mean::get_mean_of_squared () const
  {
    if (_n_ == 0)
      {
	throw runtime_error ("arithmetic_mean::get_mean_of_squared: Not enough value !");
      }
    return _sum_of_squared_ / _n_;
  }

  double arithmetic_mean::get_variance (bool bessel_correction_) const
  {
    double v = 1.0;
    size_t nlim = 1;
    if (bessel_correction_)
      {
	nlim = 2;
	v *= (_n_ / (_n_ - 1));
      }
    if (_n_ < nlim)
      {
	throw runtime_error ("arithmetic_mean::get_variance: Not enough value !");
      }
    double m = get_mean ();
    v *= (get_mean_of_squared () - m * m);
    return v;
  }
  
  arithmetic_mean::arithmetic_mean ()
  {
    reset ();
    return;
  }

  void arithmetic_mean::reset ()
  {
    _n_ = 0;
    _sum_ = 0.0;
    _sum_of_squared_ = 0.0;
    return;
  }

  void arithmetic_mean::add (double value_)
  {
    _n_++;
    _sum_ += value_;
    _sum_of_squared_ += (value_ * value_);
    return;
  }

  void arithmetic_mean::remove (double value_)
  {
    if (_n_ == 0)
      {
	throw runtime_error ("arithmetic_mean::remove: Cannot remove any values !");
      }
    _n_--;
    _sum_ -= value_;
    _sum_of_squared_ -= (value_ * value_);
  }
  
  /***************************************/

  double weighted_mean::get_weight () const
  {
    return _weight_;
  }

  double weighted_mean::get_weighted_sum () const
  {
    return _weighted_sum_;
  }

  bool weighted_mean::is_valid () const
  {
    return _weight_ != 0.0;
  }

  double weighted_mean::get_mean () const
  {
    return _weighted_sum_ / _weight_;
  }
  
  weighted_mean::weighted_mean ()
  {
    reset ();
    return;
  }

  void weighted_mean::reset ()
  {
    _weight_ = 0;
    _weighted_sum_ = 0.0;
    return;
  }

  void weighted_mean::add (double value_, double weight_)
  {
    _weight_ += weight_ ;
    _weighted_sum_ += value_ * weight_;
    return;
  }

}

// end of mygsl::mean.cc


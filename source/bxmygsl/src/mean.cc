// mygsl::mean.cc

#include <mygsl/mean.h>

namespace mygsl {
  
  size_t arithmetic_mean::get_n () const
  {
    return __n;
  }

  double arithmetic_mean::get_sum () const
  {
    return __sum;
  }

  double arithmetic_mean::get_sum_of_squared () const
  {
    return __sum_of_squared;
  }

  bool arithmetic_mean::is_valid () const
  {
    return __n > 0;
  }

  double arithmetic_mean::get_mean () const
  {
    if (__n == 0)
      {
	throw runtime_error ("arithmetic_mean::get_mean: Not enough value !");
      }
    return __sum / __n;
  }

  double arithmetic_mean::get_mean_of_squared () const
  {
    if (__n == 0)
      {
	throw runtime_error ("arithmetic_mean::get_mean_of_squared: Not enough value !");
      }
    return __sum_of_squared / __n;
  }

  double arithmetic_mean::get_variance (bool bessel_correction_) const
  {
    double v = 1.0;
    size_t nlim = 1;
    if (bessel_correction_)
      {
	nlim = 2;
	v *= (__n / (__n - 1));
      }
    if (__n < nlim)
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
  }

  void arithmetic_mean::reset ()
  {
    __n = 0;
    __sum = 0.0;
    __sum_of_squared = 0.0;
  }

  void arithmetic_mean::add (double value_)
  {
    __n++;
    __sum += value_;
    __sum_of_squared += (value_ * value_);
  }

  void arithmetic_mean::remove (double value_)
  {
    if (__n == 0)
      {
	throw runtime_error ("arithmetic_mean::remove: Cannot remove any values !");
      }
    __n--;
    __sum -= value_;
    __sum_of_squared -= (value_ * value_);
  }
  
  /***************************************/

  double weighted_mean::get_weight () const
  {
    return __weight;
  }

  double weighted_mean::get_weighted_sum () const
  {
    return __weighted_sum;
  }

  bool weighted_mean::is_valid () const
  {
    return __weight != 0.0;
  }

  double weighted_mean::get_mean () const
  {
    return __weighted_sum / __weight;
  }
  
  weighted_mean::weighted_mean ()
  {
    reset ();
  }

  void weighted_mean::reset ()
  {
    __weight = 0;
    __weighted_sum = 0.0;
  }

  void weighted_mean::add (double value_, double weight_)
  {
    __weight += weight_ ;
    __weighted_sum += value_ * weight_;
  }

}

// end of mygsl::mean.cc


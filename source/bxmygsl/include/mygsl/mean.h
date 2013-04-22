// -*- mode: c++; -*-
/* mean.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009-10-02
 * Last modified: 2013-04-23
 *
 * License:
 *
 * Description:
 *
 *   A collection of algorithm to compute the mean of
 *   a list of numbers
 *
 */

#ifndef MYGSL_MEAN_H_
#define MYGSL_MEAN_H_ 1

#include <cstdlib>
#include <boost/cstdint.hpp>

namespace mygsl {

  class arithmetic_mean
    {
    public:
      bool is_valid () const;
      size_t get_n () const;
      double get_sum () const;
      double get_sum_of_squared () const;
      double get_mean () const;
      double get_mean_of_squared () const;
      double get_variance (bool bessel_correction_ = false) const;
      arithmetic_mean ();
      void reset ();
      void add (double value_);
      void remove (double value_);
     private:
      uint32_t _n_;
      double  _sum_;
      double  _sum_of_squared_;
   };

  class weighted_mean
    {
    public:
      bool is_valid () const;
      double get_weight () const;
      double get_weighted_sum () const;
      double get_mean () const;
      weighted_mean ();
      void reset ();
      void add (double value_, double weight_);
    private:
      double _weight_;
      double _weighted_sum_;
    };

} // end of namespace mygsl

#endif // MYGSL_MEAN_H_

// end ofmean.h

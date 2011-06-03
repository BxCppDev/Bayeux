// -*- mode: c++; -*- 
/* mygsl::mean.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009-10-02
 * Last modified: 2009-10-02
 * 
 * License: 
 * 
 * Description: 
 *
 *   A collection of algorithm to compute the mean of
 *   a list of numbers
 *
 */

#ifndef __mygsl__mean_h
#define __mygsl__mean_h 1

#include <cstdlib>

using namespace std;

namespace mygsl {

  class arithmetic_mean 
    {
    private:
      size_t __n;
      double __sum;
      double __sum_of_squared;
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
    };

  class weighted_mean 
    {
    private:
      double __weight;
      double __weighted_sum;
    public:
      bool is_valid () const;
      double get_weight () const;
      double get_weighted_sum () const;
      double get_mean () const;
      weighted_mean ();
      void reset ();
      void add (double value_, double weight_);
    };

} // end of namespace mygsl

#endif // __mygsl__mean_h

// end of mygsl::mean.h

// -*- mode: c++; -*- 
/* mygsl::von_neumann_method.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009-09-23
 * Last modified: 2009-09-23
 * 
 * License: 
 * 
 * 
 * Description: 
 *   Abstract interface for R to R functor
 *
 * History: 
 * 
 */

#ifndef __mygsl__von_neumann_method_h
#define __mygsl__von_neumann_method_h 1

#include <cmath>
#include <iostream>
#include <stdexcept>

#include <mygsl/rng.h>
#include <mygsl/unary_eval.h>

using namespace std;

namespace mygsl {

  class von_neumann_method
    {
    public:
      static bool g_debug;
      static const double AUTO_FMAX;
      static const size_t DEFAULT_NSAMPLES   = 1000;
      static const size_t DEFAULT_MAX_COUNTS = 10000;
      static const size_t NO_MAX_COUNTS      = 0;

    private:
      double __xmin;
      double __xmax;
      double __fmax;
      unary_eval * __func;
      size_t       __max_counts;

    public:

      double get_xmin () const
      {
	return __xmin;
      }

      double get_xmax () const
      {
	return __xmax;
      }

      double get_fmax () const
      {
	return __fmax;
      }

      size_t get_max_counts () const
      {
	return __max_counts;
      }

      von_neumann_method (double xmin_, 
			  double xmax_, 
			  unary_eval & func_, 
			  double fmax_ = AUTO_FMAX,
			  size_t nsamples_ = DEFAULT_NSAMPLES,
			  size_t max_counts_ = DEFAULT_MAX_COUNTS);

      void init (double xmin_, 
		 double xmax_, 
		 unary_eval & func_, 
		 double fmax_,
		 size_t nsamples_,
		 size_t max_counts_);

      double shoot (rng &);

      void dump (std::ostream & out_ = std::clog) const;

    };

} // end of namespace mygsl

#endif // __mygsl__von_neumann_method_h

// end of von_neumann_method.h

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

    private:
      double __xmin;
      double __xmax;
      double __fmax;
      unary_eval * __func;
      size_t       __max_counts;

    public:

      von_neumann_method (double xmin_, 
			  double xmax_, 
			  unary_eval & func_, 
			  double fmax_ = 0.0,
			  size_t nsamples_ = 1000,
			  size_t max_counts_ = 10000);

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

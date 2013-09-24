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
 *
 *  A Von Neumann Pseudo Random Number generator
 *  for an arbitrary  R to R functor used as the density function.
 *
 * History:
 *
 */

#ifndef MYGSL_VON_NEUMANN_METHOD_H_
#define MYGSL_VON_NEUMANN_METHOD_H_ 1

#include <iostream>

#include <mygsl/rng.h>
#include <mygsl/i_unary_function.h>

namespace mygsl {

  class von_neumann_method
  {
  public:

    static const double AUTO_FMAX;
    static const size_t DEFAULT_NSAMPLES   = 1000;
    static const size_t DEFAULT_MAX_COUNTS = 10000;
    static const size_t NO_MAX_COUNTS      = 0;

    bool is_initialized () const;

    double get_xmin () const;

    double get_xmax () const;

    double get_fmax () const;

    size_t get_max_counts () const;

    von_neumann_method ();

    von_neumann_method (double xmin_,
                        double xmax_,
                        const i_unary_function & func_,
                        double fmax_ = 0.0,
                        size_t nsamples_ = DEFAULT_NSAMPLES,
                        size_t max_counts_ = DEFAULT_MAX_COUNTS);

    void init (double xmin_,
               double xmax_,
               const i_unary_function & func_,
               double fmax_,
               size_t nsamples_ = 0,
               size_t max_counts_ = DEFAULT_MAX_COUNTS);

    void reset ();

    double shoot (rng &);

    void dump (std::ostream & out_ = std::clog) const;

  private:

    double _xmin_;
    double _xmax_;
    double _fmax_;
    const i_unary_function * _func_;
    size_t _max_counts_;

  };

} // end of namespace mygsl

#endif // MYGSL_VON_NEUMANN_METHOD_H_

// end of von_neumann_method.h

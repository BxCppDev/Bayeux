// mygsl::polynomial.h

#ifndef __mygsl__polynomial_h 
#define __mygsl__polynomial_h 1

#include <gsl/gsl_errno.h>
#include <gsl/gsl_fit.h>

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <vector>

#include <gsl/gsl_poly.h>

#include <mygsl/unary_eval.h>

using namespace std;

namespace mygsl {

  class polynomial : public unary_eval
    {
    private:

      size_t         __size;

      vector<double> __c;

    public:

      size_t get_degree () const;

      double get_coeff (size_t i_) const;

      void set_coeff (size_t i_, double c_);

      polynomial (size_t sz_);

      polynomial (double c0_);

      polynomial (double c0_, double c1_);

      polynomial (double c0_, double c1_, double c2_);

      polynomial (const vector<double> & c_);

      polynomial (const polynomial & p_);

      virtual ~polynomial ();

      double eval (double x_) const;

      void print (ostream & out_) const;

    };

}

#endif // __mygsl__polynomial_h

// end of mygsl::polynomial.h

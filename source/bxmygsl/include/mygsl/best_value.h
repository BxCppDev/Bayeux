// mygsl::best_value.h
// -*- mode: C++; -*-

#ifndef __mygsl__best_value_h
#define __mygsl__best_value_h 1

#include <limits>
#include <iostream>
#include <stdexcept>
#include <cmath>

using namespace std;

namespace mygsl {
 
  class best_value
    {
    private:
      double __value;
      double __error_low;
      double __error_high;
      double __confidence_level;

    public:

      operator double ();
      void set_value (double value_);
      void set_error_low (double error_low_);
      void set_error_high (double error_lhigh_);
      void set_confidence_level (double confidence_level_);

      bool has_errors () const;
      bool is_symmetric_error () const;
      bool has_value () const;
      bool has_confidence_level () const;

      void reset ();
      void unset_errors ();
      void unset_value ();
      void unset_confidence_level ();

      double get_value () const;
      double get_min_value () const;
      double get_max_value () const;
      double get_error_low () const;
      double get_error_high () const;
      double get_confidence_level () const;

      best_value ();

      best_value (double value_);

      best_value (double value_, 
		  double error_);

      best_value (double value_, 
		  double error_, 
		  double CL_);

      best_value (double value_, 
		  double error_low_, 
		  double error_high_,
		  double CL_);
    };

  ostream & operator<< (ostream & out_, const best_value & bv_);

} // namespace mygsl

#endif // __mygsl__best_value_h

// end of mygsl::best_value.h

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

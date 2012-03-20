// mygsl::best_value.cc

#include <mygsl/best_value.h>

#include <limits>
#include <stdexcept>
#include <cmath>

namespace mygsl {
  
  using namespace std;

  best_value::operator double ()
  {
    return __value;
  }

  void best_value::set_value (double value_)
  {
    __value = value_;
    return;
  }

  void best_value::set_error_low (double error_low_)
  {
    if (error_low_ < 0.0)
      {
        throw runtime_error ("best_value::set_error_low: Invalid value for error low!");
      }
    __error_low = error_low_;
    return;
  }

  void best_value::set_error_high (double error_high_)
  {
    if (error_high_ < 0.0)
      {
        throw runtime_error ("best_value::set_error_high: Invalid value for error high!");
      }
    __error_high = error_high_;
    return;
  }

  void best_value::reset ()
  {
    unset_value ();
    unset_errors ();
    unset_confidence_level ();
    return;
  }

  void best_value::unset_errors ()
  {
    __error_low = 0.0;
    __error_high = 0.0;
    return;
  }

  bool best_value::has_value () const
  { 
    //cerr << "TEST: best_value::has_value >>> value == " << __value << endl;
    return isnormal (__value);
  }

  void best_value::unset_value ()
  {
    __value = numeric_limits<double>::quiet_NaN ();
    return;
  }

  void best_value::unset_confidence_level ()
  {
    __confidence_level = numeric_limits<double>::quiet_NaN ();
    return;
  }

  void best_value::set_confidence_level (double confidence_level_)
  {
    if ((confidence_level_ <= 0.0)
        || (confidence_level_ > 1.0))
      {
        throw runtime_error ("best_value::set_confidence_level: Invalid value for confidence level!");
      }
    __confidence_level = confidence_level_;
    return;
  }
  
  double best_value::get_min_value () const
  {
    return __value - __error_low;
  }
  
  double best_value::get_max_value () const
  {
    return __value + __error_high;
  }

  double best_value::get_value () const
  {
    return __value;
  }

  bool best_value::has_errors () const
  {
    return (__error_low != 0.0) && (__error_high != 0.0);
  }

  bool best_value::is_symmetric_error () const
  {
    return __error_low == __error_high;
  }

  double best_value::get_error_low () const
  {
    return __error_low;
  }

  double best_value::get_error_high () const
  {
    return __error_high;
  }

  double best_value::get_confidence_level () const
  {
    return __confidence_level;
  }
  
  bool best_value::has_confidence_level () const
  {
    return isnormal (__confidence_level);
  }
  
  best_value::best_value ()
  {
    reset ();
    return;
  }
  
  best_value::best_value (double value_)
  {
    reset ();
    set_value (value_);
    set_error_low (0.0);
    set_error_high (0.0);
    unset_confidence_level ();
    return;
  }
  
  best_value::best_value (double value_, 
                          double error_)
  {
    reset ();
    set_value (value_);
    set_error_low (error_);
    set_error_high (error_);
    unset_confidence_level ();
    return;
  }
  
  best_value::best_value (double value_, 
                          double error_, 
                          double CL_)
  {
    reset ();
    set_value (value_);
    set_error_low (error_);
    set_error_high (error_);
    set_confidence_level (CL_);
    return;
  }
  
  best_value::best_value (double value_, 
                          double error_low_, 
                          double error_high_,
                          double CL_)
  {
    reset ();
    set_value (value_);
    set_error_low (error_low_);
    set_error_high (error_high_);
    set_confidence_level (CL_);
    return;
  }

  ostream & operator<< (ostream & out_, const best_value & bv_)
  {
    out_ << bv_.get_value ();
    if (bv_.has_errors ())
      {
        if (bv_.is_symmetric_error ()) 
          {
            out_ << " +/- " << bv_.get_error_low ();
          }
        else
          {
            out_ << " [" << bv_.get_min_value ()
                 << ':' << bv_.get_max_value ()
                 << ']';
          }
        if (bv_.has_confidence_level ())
          {
            double cl = 100 * bv_.get_confidence_level ();
            out_ << " (";
            int nprec = out_.precision ();
            out_.precision (4);
            out_ << cl;
            out_.precision (nprec);
            out_ << "% C.L.) ";
          }
      }
    return out_;
  }
}

// end of mygsl::best_value.cc

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

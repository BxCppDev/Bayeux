// mygsl/best_value.cc

// Ourselves:
#include <mygsl/best_value.h>

// Standard library;
#include <limits>
#include <stdexcept>
#include <cmath>

// Datatools:
#include <datatools/exception.h>

namespace mygsl {

  using namespace std;

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(best_value,"mygsl::best_value")

  best_value::operator double ()
  {
    return _value_;
  }

  void best_value::set_value (double value_)
  {
    _value_ = value_;
    return;
  }

  void best_value::set_error_low (double error_low_)
  {
    DT_THROW_IF (error_low_ < 0.0,
                 std::logic_error,
                 "Invalid value for error low!");
    _error_low_ = error_low_;
    return;
  }

  void best_value::set_error_high (double error_high_)
  {
    DT_THROW_IF (error_high_ < 0.0,
                 std::logic_error,
                 "Invalid value for error high!");
    _error_high_ = error_high_;
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
    _error_low_ = 0.0;
    _error_high_ = 0.0;
    return;
  }

  bool best_value::has_value () const
  {
    //cerr << "TEST: best_value::has_value >>> value == " << _value_ << endl;
    return isnormal (_value_);
  }

  void best_value::unset_value ()
  {
    _value_ = numeric_limits<double>::quiet_NaN ();
    return;
  }

  void best_value::unset_confidence_level ()
  {
    _confidence_level_ = numeric_limits<double>::quiet_NaN ();
    return;
  }

  void best_value::set_confidence_level (double confidence_level_)
  {
    DT_THROW_IF ((confidence_level_ <= 0.0) || (confidence_level_ > 1.0),
                 std::domain_error,
                 "Invalid value for confidence level!");
    _confidence_level_ = confidence_level_;
    return;
  }

  double best_value::get_min_value () const
  {
    return _value_ - _error_low_;
  }

  double best_value::get_max_value () const
  {
    return _value_ + _error_high_;
  }

  double best_value::get_value () const
  {
    return _value_;
  }

  bool best_value::has_errors () const
  {
    return (_error_low_ != 0.0) && (_error_high_ != 0.0);
  }

  bool best_value::is_symmetric_error () const
  {
    return _error_low_ == _error_high_;
  }

  double best_value::get_error_low () const
  {
    return _error_low_;
  }

  double best_value::get_error_high () const
  {
    return _error_high_;
  }

  double best_value::get_confidence_level () const
  {
    return _confidence_level_;
  }

  bool best_value::has_confidence_level () const
  {
    return isnormal (_confidence_level_);
  }

  best_value::~best_value ()
  {
    reset ();
    return;
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

} // namespace mygsl

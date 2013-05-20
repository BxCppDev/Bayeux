// mygsl::interval.cc

#include <mygsl/interval.h>

#include <sstream>
#include <stdexcept>
#include <functional>
#include <limits>
#include <cmath>

#include <datatools/exception.h>

namespace mygsl {

  using namespace std;

  const double interval::NO_MIN_VALUE = -numeric_limits<double>::infinity ();
  const double interval::NO_MAX_VALUE = numeric_limits<double>::infinity ();
  const double interval::NO_VALUE     = numeric_limits<double>::quiet_NaN ();
  const double interval::DEFAULT_EPS  = 1.0e-7;
  const double interval::AUTO_EPS     = 0.0;
  const double interval::auto_eps     = interval::AUTO_EPS;

  const char interval::IO_SEP        = ':';
  const char interval::IO_EMPTY      = '!';
  const char interval::IO_OPEN_INCL  = '[';
  const char interval::IO_OPEN_EXCL  = '(';
  const char interval::IO_CLOSE_INCL = ']';
  const char interval::IO_CLOSE_EXCL = ')';

  const bool interval::included = true;
  const bool interval::excluded = false;

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(interval,"mygsl::interval")

  // ctor:
  interval::interval ()
  {
    reset ();
    return;
  }

  // ctor:
  interval::interval (double min_, double max_, double eps_)
  {
    reset ();
    set (min_, max_, eps_);
    return;
  }

  // ctor:
  interval::interval (double min_, bool min_include_,
                      double max_, bool max_include_,
                      double eps_)
  {
    reset ();
    set (min_, max_, eps_);
    set_min_included (min_include_);
    set_max_included (max_include_);
    return;
  }

  // dtor:
  interval::~interval ()
  {
    reset ();
    return;
  }

  void interval::remove_min ()
  {
    _min_included_ = interval::excluded;
    _min_ = NO_MIN_VALUE;
    return;
  }

  void interval::remove_max ()
  {
    _max_included_ = interval::excluded;
    _min_ = NO_MAX_VALUE;
    return;
  }

  void interval::close ()
  {
    if (has_min ())
      {
        set_min_included (true);
      }
    if (has_max ())
      {
        set_max_included (true);
      }
    return;
  }

  void interval::open ()
  {
    if (has_min ())
      {
        set_min_included (false);
      }
    if (has_max ())
      {
        set_max_included (false);
      }
    return;
  }

  bool interval::intersection_with (const interval & i_) const
  {
    if (! is_valid ()) return false;
    if (! i_.is_valid ()) return false;
    // XXXX
    return false;
  }

  void interval::reset ()
  {
    remove_min ();
    remove_max ();
    _eps_ = NO_VALUE;
    return;
  }

  bool interval::is_empty () const
  {
    return (_eps_ != _eps_);
  }

  bool interval::is_valid () const
  {
    return ! is_empty(); //(_min_ == _min_) || (_max_ == _max_);
  }

  void interval::set_min_included (bool inc_)
  {
    if (has_min ())
      {
        _min_included_ = inc_;
      }
    else
      {
        _min_included_ = false;
      }
    return;
  }

  void interval::set_max_included (bool inc_)
  {
    if (has_max ())
      {
        _max_included_ = inc_;
      }
    else
      {
        _max_included_ = false;
      }
    return;
  }

  bool interval::is_min_included () const
  {
    return _min_included_;
  }

  bool interval::is_max_included () const
  {
    return _max_included_;
  }

  bool interval::has_min () const
  {
    return isfinite(_min_);
  }

  bool interval::has_max () const
  {
    return isfinite(_max_);
  }

  double interval::get_min () const
  {
    return _min_;
  }

  double interval::get_max () const
  {
    return _max_;
  }

  double interval::get_eps () const
  {
    return _eps_;
  }

  bool interval::is_in (double x_) const
  {
    if (x_ != x_) return false;
    if (has_min ())
      {
        if (x_ < _min_) return false;
        if (! _min_included_ && x_ == _min_) return false;
      }
    if (has_max ())
      {
        if (x_ > _max_) return false;
        if (! _max_included_ && x_ == _max_) return false;
      }
    return true;
  }

  bool interval::is_in_safe (double x_) const
  {
    return get_safe_interval ().is_in (x_);
  }

  bool interval::is_in_min_unsafe (double x_) const
  {
    if (is_in (x_)) return false;
    if (! has_min ()) return false;
    return x_ < (_min_ + _eps_);
  }

  bool interval::is_in_max_unsafe (double x_) const
  {
    if (is_in (x_)) return false;
    if (! has_max ()) return false;
    return x_ > (_max_ - _eps_);
  }

  interval interval::get_safe_interval (double eps_) const
  {
    interval i;
    double min = NO_MIN_VALUE;
    double max = NO_MAX_VALUE;
    double eps = eps_;
    if (eps <= 0.0)
      {
        eps = get_eps ();
      }
    if (has_min ())
      {
        min = get_min () + eps;
      }
    if (has_max ())
      {
        max = get_max () - eps;
      }
    i.set (min, max, eps);
    return i;
  }

  void interval::set (double min_, double max_, double eps_)
  {
    if ((min_ == min_) && (max_ == max_))
      {
        DT_THROW_IF (min_ >= max_,std::logic_error,"Invalid bounds (min == '" << min_ << "') >= (max == '" << max_ << "') !");
      }
    _min_ = min_;
    _max_ = max_;
    set_eps (eps_);
    return;
  }

  void interval::set_eps (double eps_)
  {
    if (eps_ <= 0.0)
      {
        if (has_min () && has_max ())
          {
            _eps_ = (_max_ - _min_) / 100.0;
          }
        else
          {
            _eps_ = DEFAULT_EPS;
          }
      }
    else
      {
        if (has_min () && has_max ())
          {
            DT_THROW_IF (eps_ > (_max_ - _min_),std::logic_error, "Epsilon value '" << eps_ << "' is too large for interval bounds (min == '" << _min_ << "') and (max == '" << _max_ << "') !");
          }
        _eps_ = eps_;
      }
    return;
  }

  interval interval::make_empty ()
  {
    interval i;
    i._eps_ = NO_VALUE;
    return i;
  }

  bool interval::is_no_limit () const
  {
    return _min_ == -numeric_limits<double>::infinity ()
      && _max_ == +numeric_limits<double>::infinity ();
  }

  interval interval::make_no_limit (double eps_)
  {
    interval i;
    i.set (NO_MIN_VALUE, NO_MAX_VALUE, eps_);
    return i;
  }

  interval interval::make_neighbourhood (double value_, double delta_, bool included_)
  {
    interval i;
    i.set (value_ - delta_, value_ + delta_, AUTO_EPS);
    i.set_min_included (included_);
    i.set_max_included (included_);
    return i;
  }

  interval interval::make_min (double min_, bool min_include_, double eps_)
  {
    interval i;
    i.set (min_, NO_MAX_VALUE, eps_);
    i.set_min_included (min_include_);
    return i;
  }

  interval interval::make_max (double max_, bool max_include_, double eps_)
  {
    interval i;
    i.set (NO_MIN_VALUE, max_, eps_);
    i.set_max_included (max_include_);
    return i;
  }

  interval interval::make_min_max_included (double min_,
                                           double max_,
                                           double eps_)
  {
    interval i;
    i.set (min_, max_, eps_);
    i.set_min_included (interval::included);
    i.set_max_included (interval::included);
    return i;
  }

  interval interval::make_min_max_excluded (double min_,
                                           double max_,
                                           double eps_)
  {
    interval i;
    i.set (min_, max_, eps_);
    i.set_min_included (interval::excluded);
    i.set_max_included (interval::excluded);
    return i;
  }


  interval interval::make_min_max (double min_, bool min_include_,
                                   double max_, bool max_include_,
                                   double eps_)
  {
    interval i;
    i.set (min_, max_, eps_);
    i.set_min_included (min_include_);
    i.set_max_included (max_include_);
    return i;
  }

  ostream & operator<< (ostream & out_, const interval & i_)
  {
    int prec = out_.precision ();
    if (! i_.is_valid ())
      {
        out_ << interval::IO_OPEN_INCL
             << interval::IO_EMPTY
             << interval::IO_CLOSE_INCL;
        return out_;
      }
    if (i_.has_min ())
      {
        if (i_.is_min_included ())
          {
            out_ << interval::IO_OPEN_INCL;
          }
        else
          {
            out_ << interval::IO_OPEN_EXCL;
          }
        out_.precision (prec);
        out_ << i_.get_min ();
      }
    else
      {
        out_ << interval::IO_OPEN_EXCL;
      }

    out_ << interval::IO_SEP;

    if (i_.has_max ())
      {
        out_.precision (prec);
        out_ << i_.get_max ();
        if (i_.is_max_included ())
          {
            out_ << interval::IO_CLOSE_INCL;
          }
        else
          {
            out_ << interval::IO_CLOSE_EXCL;
          }
      }
    else
      {
        out_ << interval::IO_CLOSE_EXCL;
      }
    out_.precision (prec);
    return out_;
  }

  istream & operator>> (istream & in_, interval & i_)
  {
    in_ >> ws;
    char c = 0;
    in_.get (c);
    if (! in_) return in_;
    bool   min_included = false;
    bool   max_included = false;
    double min = interval::NO_MIN_VALUE;
    double max = interval::NO_MAX_VALUE;

    // open char + min:
    if (c == interval::IO_OPEN_INCL)
      {
        c = 0;
        in_.get (c);
        if (! in_) return in_;
        if (c != interval::IO_EMPTY)
          {
            in_.putback (c);
            // included min:
            in_ >> min;
            if (! in_) return in_;
            min_included = true;
          }
        else
          {
            c = 0;
            in_.get (c);
            if (! in_) return in_;
            if (c == interval::IO_CLOSE_INCL)
              {
                i_ = interval::make_empty ();
              }
            else
              {
                in_.putback (c);
                in_.setstate (ios::failbit);
              }
            return in_;
          }
      }
    else if (c == interval::IO_OPEN_EXCL)
      {
        c = 0;
        in_.get (c);
        if (! in_) return in_;
        if (c != interval::IO_SEP)
          {
            in_.putback (c);
            in_ >> min;
            if (! in_) return in_;
          }
        else
          {
            in_.putback (c);
          }
      }
    else
      {
        in_.putback (c);
        in_.setstate (ios::failbit);
        return in_;
      }

    // separator char:
    in_ >> ws;
    c = 0;
    in_.get (c);
    if (! in_) return in_;
    if (c != interval::IO_SEP)
      {
        in_.setstate (ios::failbit);
        return in_;
      }

    // max + close char:
    in_ >> ws;
    c = 0;
    in_.get (c);
    if (! in_) return in_;
    if (c != interval::IO_CLOSE_EXCL)
      {
        in_.putback (c);
        // max:
        in_ >> max;
        if (! in_) return in_;
        c = 0;
        in_.get (c);
        if (! in_) return in_;
        if (c == interval::IO_CLOSE_INCL)
          {
            max_included = true;

          }
        else if (c != interval::IO_CLOSE_EXCL)
          {
            return in_;
          }
      }
    i_.set (min, max);
    i_.set_min_included (min_included);
    i_.set_max_included (max_included);
    return in_;
  }

}

// end of mygsl::interval.cc

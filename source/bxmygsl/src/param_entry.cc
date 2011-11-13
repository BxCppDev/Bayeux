// mygsl::param_entry.cc

#include <mygsl/param_entry.h>

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <limits>

namespace mygsl {

  const double param_entry::NO_MIN_VALUE = -numeric_limits<double>::infinity ();
  const double param_entry::NO_MAX_VALUE = numeric_limits<double>::infinity ();
  const double param_entry::NO_VALUE     = numeric_limits<double>::quiet_NaN ();
  const double param_entry::DEFAULT_STEP = 1.0;
  const double param_entry::AUTO_STEP = 0.0;
  const string param_entry::AUTO_LABEL = "auto";
  const string param_entry::FREE_LABEL = "free";
  const string param_entry::CONST_LABEL = "const";

  bool param_entry::has_step () const
  {
    return _step_ == _step_;
  }

  bool param_entry::has_value () const
  {
    return _value_ == _value_;
  }

  const string & param_entry::get_name () const
  {
    return _name_;
  }

  const string & param_entry::get_comment () const
  {
    return _comment_;
  }

  void param_entry::set_name (const string & name_)
  {
    _name_ = name_;
    return;
  }

  void param_entry::set_comment (const string & comment_)
  {
    _comment_ = comment_;
    return;
  }


  const best_value & param_entry::get_best_value () const
  {
    return _best_value_;
  }

  best_value & param_entry::get_best_value ()
  {
    return _best_value_;
  }

  bool param_entry::has_limit () const
  {
    return _limit_ != LIMIT_NO;
  }

  bool param_entry::has_no_limit () const
  {
    return _limit_ == LIMIT_NO;
  }

  bool param_entry::has_min () const
  {
    return _limit_ & LIMIT_MIN;
  }

  bool param_entry::has_max () const
  {
    return _limit_ & LIMIT_MAX;
  }

  bool param_entry::is_const () const
  {
    return _type_ & TYPE_CONST;
  }

  bool param_entry::is_free () const
  {
    return _type_ & TYPE_FREE;
  }

  bool param_entry::is_auto () const
  {
    return _type_ & TYPE_AUTO;
  }

  void param_entry::set_best_value (const best_value & best_value_)
  {
    /*
      if (is_const ())
      {
      ostringstream message;
      message << "param_entry::set_best_value: "
      << "Cannot force best value for 'const' parameter '"
      << get_name () << "'!";
      throw logic_error (message.str ());
      }
    */
    _best_value_ = best_value_;
    return;
  }

  void param_entry::reset_best_value ()
  {
    _best_value_.reset ();
    return;
  }

  bool param_entry::is_auto_computed () const
  {
    return _type_ & TYPE_AUTO_COMPUTED;
  }

  void param_entry::set_auto_computed (bool auto_)
  {
    if (is_auto ())
      {
        if (auto_)
          {
            _type_ |= TYPE_AUTO_COMPUTED;
          }
        else
          {
            _type_ &= TYPE_AUTO_COMPUTED;
          }
      }
    return;
  }

  bool param_entry::has_best_value () const
  {
    return _best_value_.has_value ();
  }

  double param_entry::get_dist_to_min () const
  {
    double dist = 0.0;
    if (has_min ())
      {
        dist = get_value () - _min_;
      }
    return dist;
  }

  double param_entry::get_overflow () const
  {
    double overflow = 0.0;
    if (has_max ())
      {
        if (get_value () > _max_)
          {
            overflow = get_value () - _max_;
          }
      }
    return overflow;
  }

  double param_entry::get_underflow () const
  {
    double underflow = 0.0;
    if (has_min ())
      {
        if (get_value () < _min_)
          {
            underflow = _min_ - get_value ();
          }
      }
    return underflow;
  }

  double param_entry::get_dist_to_max () const
  {
    double dist = 0.0;
    if (has_max ())
      {
        dist = get_value () - _max_;
      }
    return dist;
  }

  double param_entry::get_sign_limit () const
  {
    if (has_min ())
      {
        if (get_value () < _min_)
          {
            return -1.0;
          }
      }
    if (has_max ())
      {
        if (get_value () > _max_)
          {
            return +1.0;
          }
      }
    return 0.0;
  }

  double param_entry::get_dist_to_limit () const
  {
    double dist = 0.0;
    if (! is_in_range ())
      {
        double u = get_underflow ();
        double o = get_overflow ();
        dist = std::max (u, o);
      }
    return dist;
  }

  bool param_entry::is_in_range_but_close_to_min () const
  {
    if (has_min ())
      {
        if ((get_value () >= _min_)
            && (get_value () < (_min_ + get_step ()))
            )
          {
            return true;
          }
      }
    return false;
  }

  bool param_entry::is_in_range_but_close_to_max () const
  {
    if (has_max ())
      {
        if ((get_value () <= _max_)
            && (get_value () > (_max_ - get_step ()))
            )
          {
            return true;
          }
      }
    return false;
  }

  bool param_entry::is_in_safe_range () const
  {
    bool ok = true;
    if (has_limit ())
      {
        if (ok && has_min () && (get_value () < (_min_ + get_step ()))) ok = false;
        if (ok && has_max () && (get_value () > (_max_ - get_step ()))) ok = false;
      }
    return ok;
  }

  bool param_entry::is_in_range () const
  {
    bool ok = true;
    if (has_limit ())
      {
        if (ok && has_min () && (get_value () < _min_)) ok = false;
        if (ok && has_max () && (get_value () > _max_)) ok = false;
      }
    return ok;
  }

  double param_entry::get_value_safe () const
  {
    double dist;
    return get_value_safe (dist);
  }

  double param_entry::get_value_safe (double & dist_) const
  {
    double val = get_value ();
    dist_ = 0.0;
    if (! is_const ())
      {
        if (has_min () && (get_value () < _min_))
          {
            val = _min_;
            dist_ = _min_ - get_value ();
          }
        if (has_max () && (get_value () > _max_))
          {
            val= _max_;
            dist_ = get_value () - _max_;
          }
      }
    return val;
  }

  double param_entry::get_value () const
  {
    return _value_;
  }

  double param_entry::get_step () const
  {
    return _step_;
  }

  double param_entry::get_min () const
  {
    return _min_;
  }

  double param_entry::get_max () const
  {
    return _max_;
  }

  void param_entry::set_value_no_check (double value_)
  {
    if (is_const ())
      {
        throw logic_error ("mygsl::param_entry::set_value_no_check: Cannot change value for this parameter is 'const'!");
      }
    _value_ = value_;
    if (is_auto ())
      {
        set_auto_computed (true);
      }
    return;
  }

  void param_entry::set_no_max ()
  {
    _limit_ ^= param_entry::LIMIT_MAX;
    _max_ = param_entry::NO_MAX_VALUE;
    return;
  }

  void param_entry::set_no_min ()
  {
    _limit_ ^= param_entry::LIMIT_MIN;
    _min_ = param_entry::NO_MIN_VALUE;
    return;
  }

  void param_entry::set_max (double max_)
  {
    _limit_ |= param_entry::LIMIT_MAX;
    _max_ = max_;
    return;
  }

  void param_entry::set_min_max (double min_, double max_)
  {
    if (min_ >= max_)
      {
        ostringstream message;
        message << "mygsl::param_entry::set_min_max: "
                << "Invalid min/max range"
                << " [" << min_ << ":" << max_ << "] "
                << "for parameter '" << get_name () << "'!";
        throw logic_error (message.str ());
      }
    _limit_ |= param_entry::LIMIT_MIN;
    _min_ = min_;
    _limit_ |= param_entry::LIMIT_MAX;
    _max_ = max_;
    return;
  }

  void param_entry::set_min (double min_)
  {
    _limit_ |= param_entry::LIMIT_MIN;
    _min_ = min_;
    return;
  }

  void param_entry::set_step (double step_)
  {
    if (step_ <= 0.0)
      {
        if (has_min () && has_max ())
          {
            _step_ = (_max_ - _min_) / 100.0;
          }
        else
          {
            _step_ = DEFAULT_STEP;
          }
      }
    else
      {
        if (has_min () && has_max ())
          {
            if (step_ > (_max_ - _min_))
              {
                std::ostringstream message;
                message << "param_entry::set_step: ";
                message << "step to large for parameter '" << get_name () << "'!";
                throw std::logic_error (message.str ());
              }
          }
        _step_ = step_;
      }
     return;
 }

  bool param_entry::check_value () const
  {
    return is_in_range ();
  }

  void param_entry::set_value (double value_)
  {
    double old_val = _value_;
    set_value_no_check (value_);
    if (! is_in_range ())
      {
        _value_ = old_val;
        std::ostringstream message;
        message << "mygsl::param_entry::set_value: ";
        message << "value " << value_ << " outside of allowed range for parameter '" << get_name () << "'!";
        throw std::logic_error (message.str ());
      }
    return;
  }

  /***********************************************************/

  param_entry::param_entry (const string & name_)
  {
    _name_  = name_;
    _type_  = param_entry::TYPE_FREE;
    _limit_ = param_entry::LIMIT_NO;
    _min_   = param_entry::NO_MIN_VALUE;
    _max_   = param_entry::NO_MAX_VALUE;
    _value_ = param_entry::NO_VALUE;
    _step_  = DEFAULT_STEP;
    reset_best_value ();
    return;
  }

  param_entry
  param_entry::make_auto (const std::string & name_, double step_)
  {
    param_entry pe (name_);
    pe._type_  = param_entry::TYPE_AUTO;
    pe.set_step (step_);
    return pe;
  }

  param_entry
  param_entry::make_auto_min (const std::string & name_,
                              double min_,
                              double step_)
  {
    param_entry pe (name_);
    pe._type_ = param_entry::TYPE_AUTO;
    pe.set_min (min_);
    pe.set_step (step_);
    return pe;
  }

  param_entry
  param_entry::make_auto_max (const std::string & name_,
                              double max_,
                              double step_)
  {
    param_entry pe (name_);
    pe._type_ = param_entry::TYPE_AUTO;
    pe.set_max (max_);
    pe.set_step (step_);
    return pe;
  }

  param_entry
  param_entry::make_auto_range (const std::string & name_,
                                double min_,
                                double max_,
                                double step_)
  {
    param_entry pe (name_);
    pe._type_ = param_entry::TYPE_AUTO;
    pe.set_min_max (min_, max_);
    pe.set_step (step_);
    return pe;
  }

  param_entry
  param_entry::make_const (const std::string & name_ ,
                           double value_)
  {
    param_entry pe (name_);
    pe.set_value_no_check (value_);
    pe._type_ = param_entry::TYPE_CONST;
    return pe;
  }

  param_entry
  param_entry::make_free (const std::string & name_,
                          double value_,
                          double step_)
  {
    param_entry pe (name_);
    pe._type_ = param_entry::TYPE_FREE;
    pe.set_value (value_);
    pe.set_step (step_);
    return pe;
  }

  param_entry
  param_entry::make_free_min (const std::string & name_,
                              double min_,
                              double value_,
                              double step_)
  {
    param_entry pe (name_);
    pe._type_ = TYPE_FREE;
    pe.set_min (min_);
    pe.set_value (value_);
    pe.set_step (step_);
    return pe;
  }

  param_entry
  param_entry::make_free_max (const std::string & name_,
                              double max_,
                              double value_,
                              double step_)
  {
    param_entry pe (name_);
    pe._type_ = TYPE_FREE;
    pe.set_max (max_);
    pe.set_value (value_);
    pe.set_step (step_);
    return pe;
  }

  param_entry
  param_entry::make_free_range (const std::string & name_,
                                double min_,
                                double max_,
                                double value_,
                                double step_)
  {
    param_entry pe (name_);
    pe._type_ = TYPE_FREE;
    pe.set_min_max (min_, max_);
    pe.set_value (value_);
    pe.set_step (step_);
    return pe;
  }

  void param_entry::print (ostream & out_,
                           const string & title_,
                           const string & indent_) const
  {
    string indent = indent_;
    string tag  = "|-- ";
    string ltag = "`-- ";
    if (! title_.empty ())
      {
        out_ << indent << title_ << ":" << endl;
      }
    out_ << indent << tag << "Name :      '" << get_name () << "'" << endl;
    out_ << indent << tag << "Type:       '"
         << (is_free () ? FREE_LABEL : (is_const () ? CONST_LABEL : AUTO_LABEL))
         << "'"  << endl;
    out_ << indent << tag << "Minimum:    ";
    if (! has_min ())
      {
        out_ << "<no minimum value>";
      }
    else
      {
        out_ << get_min ();
      }
    out_ << endl;
    out_ << indent << tag << "Maximum:    ";
    if (! has_max ())
      {
        out_ << "<no maximum value>";
      }
    else
      {
        out_ << get_max ();
      }
    out_ << endl;
    out_ << indent;
    if (is_free ())
      {
        out_ << tag;
      }
    else
      {
        out_ << tag;
      }
    out_ << "Value:      ";
    if (! is_auto () || is_auto_computed ())
      {
        out_ << get_value ();
        if (is_auto ())
          {
            out_ << " <automatically computed>";
          }
        else if (! is_in_range ())
          {
            out_ << " <out of range>";
          }
      }
    else
      {
        out_ << "<not computed yet>";
      }
    out_ << endl;
    if (! is_const ())
      {
        out_ << indent << tag << "Step:       " << get_step () << endl;
        if (has_best_value ())
          {
            out_ << indent << tag << "Best value: " << get_best_value () << endl;
          }
        else
          {
            out_ << indent << tag << "Best value: " << "<no best value>" << endl;
          }
      }
    out_ << indent << ltag << "Comment:   "
         << (get_comment ().empty ()? "<no comment>" : get_comment ()) << endl;
    return;
  }

  void param_entry::print_status (ostream & out_) const
  {
    out_.width (10);
    out_ << get_name () << " (";
    out_.width (5);
    out_.setf (ios::left);
    if (is_free ())
      {
        out_ << FREE_LABEL;
      }
    if (is_auto ())
      {
        out_ << AUTO_LABEL;
      }
    if (is_const ())
      {
        out_ << CONST_LABEL;
      }
    out_.width (1);
    //out_.unsetf(ios::left);
    out_ << ")";
    out_ << ": ";
    if (get_underflow () > 0)
      {
        out_ << "--*--";
      }
    else
      {
        out_ << "-----";
      }
    if (has_min ())
      {
        out_ << "[";
      }
    else
      {
        out_ << "-";
      }
    if (is_in_range ())
      {
        out_ << "--*--";
      }
    else
      {
        out_ << "-----";
      }
    if (has_max ())
      {
        out_ << "]";
      }
    else
      {
        out_ << "-";
      }
    if (get_overflow () > 0)
      {
        out_ << "--*--";
      }
    else
      {
        out_ << "-----";
      }
    out_ << "  value == " << get_value ();
    out_ << endl;
    return;
  }

}

// end of mygsl::param_entry.cc

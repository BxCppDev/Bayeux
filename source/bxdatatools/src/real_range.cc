// -*- mode: c++; -*-
// real_range.cc
 
#include <datatools/utils/real_range.h>

#include <sstream>
#include <stdexcept>
#include <limits>
#include <cmath>

using namespace std;

namespace datatools {
  
  namespace utils {

    const double real_range::DEFAULT_EPSILON = 5.e-15;

    bool real_range::is_valid () const
    {
      return (_lower_flag_ != range::undefined)
	&& (_upper_flag_ != range::undefined);
    }

    void real_range::invalidate ()
    {
      reset ();
      return;
    }
      
    void real_range::reset_lower ()
    {
      _lower_      = -numeric_limits<value_type>::max ();
      _lower_flag_ = range::undefined;
      return;
    }

    void real_range::reset_upper ()
    {
      _upper_      = numeric_limits<value_type>::max ();
      _upper_flag_ = range::undefined;
      return;
    }
    
    void real_range::reset ()
    {
      _abs_epsilon_ = numeric_limits<double>::quiet_NaN ();
      reset_lower ();
      reset_upper ();
      return;
    }

    void real_range::set_lower (value_type a_lower_value, 
				   int a_lower_flag)
    {
      if (a_lower_flag == range::unbound)
	{
	  // "];...."
	  _lower_      = -numeric_limits<value_type>::infinity ();
	  _lower_flag_ = a_lower_flag;
	}
      else if (a_lower_flag == range::included)
	{
	  // "[value;...."
	  _lower_      = a_lower_value;
	  _lower_flag_ = a_lower_flag;
	}
      else if (a_lower_flag == range::excluded)
	{
	  // "]value;...."
	  _lower_      = a_lower_value;
	  _lower_flag_ = a_lower_flag;
	}
      else
	{
	  throw logic_error ("datatools::utils::real_range::set_lower: Invalid lower bound flag !");
	}
      return;
    }

    void real_range::set_upper (value_type a_upper_value, 
				int a_upper_flag)
    {
     if (a_upper_flag == range::unbound)
	{
	  // "...;...["
	  _upper_      = +numeric_limits<value_type>::infinity ();
	  _upper_flag_ = a_upper_flag;
	}
      else if (a_upper_flag == range::included)
	{
	  // "...;value]"
	  _upper_      = a_upper_value;
	  _upper_flag_ = a_upper_flag;
	}
      else if (a_upper_flag == range::excluded)
	{
	  // "...;value["
	  _upper_      = a_upper_value;
	  _upper_flag_ = a_upper_flag;
	}
      else
	{
	  throw logic_error ("datatools::utils::real_range::set_upper: Invalid upper bound flag !");
	}
      return;
    }

    void real_range::set (value_type a_lower_value, 
			     value_type a_upper_value, 
			     int a_lower_flag, 
			     int a_upper_flag)
    {
      set_lower (a_lower_value, a_lower_flag);
      set_upper (a_upper_value, a_upper_flag);
      return;
    }
     
    // Ctor:
    real_range::real_range ()
    {
      _abs_epsilon_ = numeric_limits<double>::quiet_NaN ();
      reset ();
      return;
    }
     
    // Ctor:
    real_range::real_range (value_type a_lower_value, 
				  value_type a_upper_value, 
				  int a_lower_flag, 
				  int a_upper_flag)
    {
      _abs_epsilon_ = numeric_limits<double>::quiet_NaN ();
      set (a_lower_value, a_upper_value, a_lower_flag, a_upper_flag);
      return;
    }

    bool real_range::is_lower_bounded () const
    {
      return _lower_flag_ > 0 ;
    }

    bool real_range::is_upper_bounded () const
    {
      return _upper_flag_ > 0 ;
    }

    bool real_range::is_bounded () const
    {
      return is_lower_bounded () && is_upper_bounded ();
    }

    real_range::value_type real_range::get_lower_bound () const
    {
      if (! is_lower_bounded ())
	{
	  throw logic_error ("datatools::utils::real_range::get_lower_bound: No lower bound !");
	}
      return _lower_;
    }

    real_range::value_type real_range::get_upper_bound () const
    {
      if (! is_upper_bounded ())
	{
	  throw logic_error ("datatools::utils::real_range::get_upper_bound: No upper bound !");
	}
      return _upper_;
    }

    bool real_range::is_lower_included () const
    {
      if (! is_lower_bounded ())
	{
	  throw logic_error ("datatools::utils::real_range::is_lower_included: No lower bound !");
	}
      return (_lower_flag_ == range::included);
    }

    bool real_range::is_upper_included () const
    {
      if (! is_upper_bounded ())
	{
	  throw logic_error ("datatools::utils::real_range::is_upper_included: No upper bound !");
	}
      return (_upper_flag_ == range::included);
    }
    
    /*
    bool real_range::is_empty () const
    {
      return false;//count () == 0;
    }
    */

    void real_range::make_empty ()
    {
      set (0, 0, range::excluded, range::excluded);
      return;
    }

    void real_range::make_full ()
    {
     set (-numeric_limits<value_type>::max (), 
	  numeric_limits<value_type>::max (), 
	  range::included, 
	  range::included);
    }
 
    void real_range::make_full_positive ()
    {
      set (0, 
	   numeric_limits<value_type>::max (), 
	   range::included, 
	   range::included);
      return;
    }

    void real_range::make_full_negative ()
    {
      set (-numeric_limits<value_type>::max (), 
	   0, 
	   range::included, 
	   range::included);
     return;
    }

    void real_range::make_upper_unbounded (value_type a_lower_value, 
					      bool a_lower_included)
    {
      set (a_lower_value, 
	   +numeric_limits<value_type>::infinity (), 
	   a_lower_included ? range::included : range::excluded , 
	   range::unbound);
     return;
    }

    void real_range::make_lower_unbounded (value_type a_upper_value, 
					      bool a_upper_included)
    {
      set (-numeric_limits<value_type>::infinity (),
	   a_upper_value,
	   range::unbound,
	   a_upper_included ? range::included : range::excluded);
      return;
    }

    void real_range::make_bounded (value_type a_lower_value, 
				      value_type a_upper_value,
				      bool a_lower_included, 
				      bool a_upper_included)
    {
      set (a_lower_value,
	   a_upper_value,
	   a_lower_included ? range::included : range::excluded,
	   a_upper_included ? range::included : range::excluded);
      return;
    }

    double real_range::compute_epsilon (double a_lower, double a_upper)
    {
      double eps;
      double l_eps = numeric_limits<double>::quiet_NaN ();
      double u_eps = numeric_limits<double>::quiet_NaN ();
      if (a_lower == a_lower)
	{
	  double scale = std::abs (a_lower);
	  l_eps = DEFAULT_EPSILON * scale;
	}
      if (a_upper == a_upper)
	{
	  double scale = std::abs (a_upper);
	  u_eps = DEFAULT_EPSILON * scale;
	} 
      if ((l_eps == l_eps) && (u_eps == u_eps))
	{
	  eps = max (l_eps, u_eps);
	}
      else if (l_eps == l_eps)
	{
	  eps = l_eps;
	}
      else 
	{
	  eps = u_eps;
	}      
      return eps;
    }

    bool real_range::has (value_type a_value) const
    {
      static bool ok = true;
      double eps = _abs_epsilon_;
      if (eps != eps)
	{
	  eps = compute_epsilon ((_lower_ == _lower_) ? _lower_ : numeric_limits<double>::quiet_NaN (),
				 (_upper_ == _upper_) ? _upper_ : numeric_limits<double>::quiet_NaN ());
	  {
	    real_range & rr = const_cast<real_range &> (*this);
	    rr._abs_epsilon_ = eps;
	  }
	  if (ok) 
	    {
	      clog << "real_range::has: _abs_epsilon_ = " << _abs_epsilon_ << endl;
	      ok = false;
	    }
	}
      if (is_lower_bounded ())
	{
	  if (is_lower_included ())
	    {
	      if (a_value < (_lower_ - eps)) return false;
	    }
	  else
	    {
	      if (a_value < (_lower_ + eps)) return false;
	    }
	}
      if (is_upper_bounded ())
	{
	  if (is_upper_included ())
	    {
	      if (a_value > (_upper_ + eps)) return false;
	    }
	  else
	    {
	      if (a_value > (_upper_ - eps)) return false;
	    }
	}
      return true;
    }

    void real_range::dump (ostream & a_out) const
    {
      a_out << "real_range: " << endl;
      if (is_valid ())
	{
	  a_out << "|-- " << "Lower bounded  = " << is_lower_bounded () << endl;
	  if (is_lower_bounded ())
	    {
	      a_out << "|-- " << "Lower bound    = " << _lower_ << endl;
	      a_out << "|-- " << "Lower included = " << is_lower_included () << endl;
	    }
	  a_out << "|-- " << "Upper bounded  = " << is_upper_bounded () << endl;
	  if (is_upper_bounded ())
	    {
	      a_out << "|-- " << "Upper bound    = " << _upper_ << endl;
	      a_out << "|-- " << "Upper included = " << is_upper_included () << endl;
	    }
	  a_out << "|-- " << "Bounded        = " << is_bounded () << endl;
	  /*
	  if (is_bounded ())
	    {
	      a_out << "|-- " << "Empty          = " << is_empty () << endl;
	    }
	  */
	}
      else
	{
	  a_out << "|-- " << "[invalid]" << endl;
	}
      a_out << "`-- " << "Rendering      = '" << *this << "'" << endl; 
      return;
    }
  
    ostream & operator<< (ostream & a_out, const real_range & a_range)
    {
      if (! a_range.is_valid ())
	{
	  a_out << "]?[";
	}
      else
	{
	  if (a_range.is_lower_bounded ())
	    {
	      if (a_range.is_lower_included ()) a_out << range::token_open;
	      else a_out << range::token_close;
	      a_out << a_range.get_lower_bound ();
	    }
	  else
	    {
	      a_out << range::token_close;
	    }
	  a_out << ';';
	  if (a_range.is_upper_bounded ())
	    {
	      a_out << a_range.get_upper_bound ();
	      if (a_range.is_upper_included ()) a_out << range::token_close;
	      else a_out << range::token_open;
	    }
	  else 
	    {
	      a_out << range::token_open;
	    }
	}
      return a_out;
    }
   
  } // namespace utils
  
} // namespace datatools

// end of real_range.cc

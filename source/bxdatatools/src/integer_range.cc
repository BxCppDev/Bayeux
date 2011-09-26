// -*- mode: c++; -*-
// integer_range.cc
 
#include <datatools/utils/integer_range.h>

#include <sstream>
#include <stdexcept>
#include <limits>

using namespace std;

namespace datatools {
  
  namespace utils {
    
    bool integer_range::is_valid () const
    {
      return (_lower_flag_ != range::undefined)
	&& (_upper_flag_ != range::undefined);
    }

    void integer_range::invalidate ()
    {
      reset ();
      return;
    }
      
    void integer_range::reset_lower ()
    {
      _lower_      = numeric_limits<value_type>::min ();
      _lower_flag_ = range::undefined;
      return;
    }

    void integer_range::reset_upper ()
    {
      _upper_      = numeric_limits<value_type>::max ();
      _upper_flag_ = range::undefined;
      return;
    }
    
    void integer_range::reset ()
    {
      reset_lower ();
      reset_upper ();
      return;
    }

    void integer_range::set_lower (value_type a_lower_value, 
				   int a_lower_flag)
    {
      if (a_lower_flag == range::unbound)
	{
	  // "];...."
	  _lower_      = numeric_limits<value_type>::min ();
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
	  throw logic_error ("datatools::utils::integer_range::set_lower: Invalid lower bound flag !");
	}
      return;
    }

    void integer_range::set_upper (value_type a_upper_value, 
				   int a_upper_flag)
    {
     if (a_upper_flag == range::unbound)
	{
	  // "...;...["
	  _upper_      = numeric_limits<value_type>::max ();
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
	  throw logic_error ("datatools::utils::integer_range::set_upper: Invalid upper bound flag !");
	}
      return;
    }

    void integer_range::set (value_type a_lower_value, 
			     value_type a_upper_value, 
			     int a_lower_flag, 
			     int a_upper_flag)
    {
      set_lower (a_lower_value, a_lower_flag);
      set_upper (a_upper_value, a_upper_flag);
      return;
    }
     
    // Ctor:
    integer_range::integer_range ()
    {
      reset ();
      return;
    }
     
    // Ctor:
    integer_range::integer_range (value_type a_lower_value, 
				  value_type a_upper_value, 
				  int a_lower_flag, 
				  int a_upper_flag)
    {
      set (a_lower_value, a_upper_value, a_lower_flag, a_upper_flag);
      return;
    }

    bool integer_range::is_lower_bounded () const
    {
      return _lower_flag_ > 0 ;
    }

    bool integer_range::is_upper_bounded () const
    {
      return _upper_flag_ > 0 ;
    }

    bool integer_range::is_bounded () const
    {
      return is_lower_bounded () && is_upper_bounded ();
    }

    integer_range::value_type integer_range::get_lower_bound () const
    {
      if (! is_lower_bounded ())
	{
	  throw logic_error ("datatools::utils::integer_range::get_lower_bound: No lower bound !");
	}
      return _lower_;
    }

    integer_range::value_type integer_range::get_upper_bound () const
    {
      if (! is_upper_bounded ())
	{
	  throw logic_error ("datatools::utils::integer_range::get_upper_bound: No upper bound !");
	}
      return _upper_;
    }

    bool integer_range::is_lower_included () const
    {
      if (! is_lower_bounded ())
	{
	  throw logic_error ("datatools::utils::integer_range::is_lower_included: No lower bound !");
	}
      return (_lower_flag_ == range::included);
    }

    bool integer_range::is_upper_included () const
    {
      if (! is_upper_bounded ())
	{
	  throw logic_error ("datatools::utils::integer_range::is_upper_included: No upper bound !");
	}
      return (_upper_flag_ == range::included);
    }
    
    uint64_t integer_range::count () const
    {
      if (! is_bounded ())
	{
	  throw logic_error ("datatools::utils::integer_range::count: (semi-)infinite range !");
	}
      int64_t cnt64 = _upper_;
      cnt64 -= _lower_;
      cnt64 += 1;
      if (! is_upper_included ())
	{
	  cnt64--;
	}
      if (! is_lower_included ())
	{
	  cnt64--;
	}
      uint64_t ucnt64 = cnt64;
      return ucnt64;
    }
    
    bool integer_range::is_empty () const
    {
      return count () == 0;
    }

    void integer_range::make_empty ()
    {
      set (0, 0, range::excluded, range::excluded);
      return;
    }

    void integer_range::make_full ()
    {
     set (numeric_limits<value_type>::min (), 
	  numeric_limits<value_type>::max (), 
	  range::included, 
	  range::included);
    }
 
    void integer_range::make_full_positive ()
    {
      set (0, 
	   numeric_limits<value_type>::max (), 
	   range::included, 
	   range::included);
      return;
    }

    void integer_range::make_full_negative ()
    {
      set (numeric_limits<value_type>::min (), 
	   0, 
	   range::included, 
	   range::included);
     return;
    }

    void integer_range::make_upper_unbounded (value_type a_lower_value, 
					      bool a_lower_included)
    {
      set (a_lower_value, 
	   0, 
	   a_lower_included ? range::included : range::excluded , 
	   range::unbound);
     return;
    }

    void integer_range::make_lower_unbounded (value_type a_upper_value, 
					      bool a_upper_included)
    {
      set (0,
	   a_upper_value,
	   range::unbound,
	   a_upper_included ? range::included : range::excluded);
      return;
    }

    void integer_range::make_bounded (value_type a_lower_value, 
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

    bool integer_range::has (value_type a_value) const
    {
      if (is_lower_bounded ())
	{
	  if (is_lower_included ())
	    {
	      if (a_value < _lower_) return false;
	    }
	  else
	    {
	      if (a_value <= _lower_) return false;
	    }
	}
      if (is_upper_bounded ())
	{
	  if (is_upper_included ())
	    {
	      if (a_value > _upper_) return false;
	    }
	  else
	    {
	      if (a_value >= _upper_) return false;
	    }
	}
      return true;
    }

    void integer_range::dump (ostream & a_out) const
    {
      a_out << "integer_range: " << endl;
      if (is_valid ())
	{
	  a_out << "|-- " << "Lower bounded  = " << is_lower_bounded () << endl;
	  if (is_lower_bounded ())
	    {
	      a_out << "|-- " << "Lower bound    = " << _lower_ << endl;
	      a_out << "|-- " << "Lower included = " << is_lower_included () << endl;
	      a_out << "|-- " << "First          = " << first () << endl;
	    }
	  a_out << "|-- " << "Upper bounded  = " << is_upper_bounded () << endl;
	  if (is_upper_bounded ())
	    {
	      a_out << "|-- " << "Upper bound    = " << _upper_ << endl;
	      a_out << "|-- " << "Upper included = " << is_upper_included () << endl;
	      a_out << "|-- " << "Last           = " << last () << endl;
	    }
	  a_out << "|-- " << "Bounded        = " << is_bounded () << endl;
	  if (is_bounded ())
	    {
	      a_out << "|-- " << "Count          = " << count () << endl;
	      a_out << "|-- " << "Empty          = " << is_empty () << endl;
	      a_out << "|-- " << "Begin          = " << begin () << endl;
	      a_out << "|-- " << "End            = " << end () << endl;
	    }
	}
      else
	{
	  a_out << "|-- " << "[invalid]" << endl;
	}
      a_out << "`-- " << "Rendering      = '" << *this << "'" << endl; 
      return;
    }
    

    integer_range::value_type integer_range::begin () const
    {
      if (! is_bounded ()) 
	{
	  throw logic_error ("datatools::utils::integer_range::begin: Not bounded !");
	}
      value_type the_beg = _lower_;
      if (! is_lower_included ()) 
	{
	  the_beg++;
	}
      return the_beg;
    }
    
    integer_range::value_type integer_range::end () const
    {
      if (! is_bounded ()) 
	{
	  throw logic_error ("datatools::utils::integer_range::end: Not bounded !");
	}
      value_type the_end = _upper_; 
      // case : the_end != numeric_limits<value_type>::max () ???
      if (is_upper_included ()) 
	{
	  the_end++;
	}
      return the_end;
    }

    integer_range::value_type integer_range::first () const
    {
      if (! is_lower_bounded ()) 
	{
	  throw logic_error ("datatools::utils::integer_range::first: Not lower bounded !");
	}
      value_type the_first = _lower_;
      if (! is_lower_included ()) 
	{
	  the_first++;
	}
      return the_first;
    }
    
    integer_range::value_type integer_range::last () const
    {
      if (! is_upper_bounded ()) 
	{
	  throw logic_error ("datatools::utils::integer_range::last: Not upper bounded !");
	}
      value_type the_last = _upper_; 
      if (! is_upper_included ()) 
	{
	  the_last--;
	}
      return the_last;
    }
     
    ostream & operator<< (ostream & a_out, const integer_range & a_range)
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

// end of integer_range.cc

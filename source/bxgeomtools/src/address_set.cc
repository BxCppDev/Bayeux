// -*- mode: c++; -*- 
/* address_set.cc 
 */

#include <geomtools/address_set.h>

namespace geomtools {
  
  bool address_set::is_valid () const
  {
    return __mode != MODE_INVALID;
  }

  void address_set::invalidate ()
  {
    reset ();
    __mode = MODE_INVALID;
  }

  void address_set::__reset_list ()
  {
    __addrs.clear ();
  }

  void address_set::__reset_range ()
  {
    __range_min = 1;
    __range_max = 0;
  }
  
  bool address_set::is_reverse () const
  {
    return __reverse;
  }
 
  void address_set::set_reverse (bool r_)
  {
    __reverse = r_;
  }
 
  bool address_set::is_mode_none () const
  {
    return __mode == MODE_NONE;
  }
  
  bool address_set::is_mode_all () const
  {
    return __mode == MODE_ALL;
  }
  
  bool address_set::is_mode_range () const
  {
    return __mode == MODE_RANGE;
  }
  
  bool address_set::is_mode_list () const
  {
    return __mode == MODE_LIST;
  }
  
  void address_set::set_mode_none ()
  {
    __reset_range ();
    __reset_list ();
    __mode = MODE_NONE;
  }
  
  void address_set::set_mode_all ()
  {
    __reset_range ();
    __reset_list ();
    __mode = MODE_ALL;
  }
  
  void address_set::set_mode_range ()
  {
    __reset_list ();
    __reset_range ();
    __mode = MODE_RANGE;
  }
  
  void address_set::set_range (uint32_t min_, uint32_t max_)
  {
    if (! is_mode_range ())
      {
	set_mode_range ();
	//throw runtime_error ("address_set::set_range: Range mode is not active !");
      }
    if (min_ >  max_)
      {
	__reset_range ();
      }
    else
     {
       __range_min = min_;
       __range_max = max_;     
     }
    return;
  }
  
  void address_set::set_mode_range (uint32_t min_, uint32_t max_)
  {
    __mode = MODE_RANGE;
    set_range (min_, max_);
  }

  void address_set::set_mode_list ()
  {
    __reset_list ();
    __reset_range ();
    __mode = MODE_LIST;
  }

  void address_set::add_to_list (uint32_t val_)
  {
    if (! is_mode_list ())
      {
	set_mode_list ();
      }
    __addrs.insert (val_);
  }

  bool address_set::match (uint32_t val_) const
  {
    bool matched_value = false;

    if (is_mode_all ())
      {
	matched_value = true;
      }
    else if (is_mode_range ())
      {
	if (val_ < __range_min) matched_value = false;
	else if (val_ > __range_max) matched_value = false;
	else matched_value = true;
      }
    else if (is_mode_list ())
      {
	matched_value = __addrs.find (val_) != __addrs.end ();
      }
    else
      {
	matched_value = false;
      }
    if (is_reverse ())
      {
	matched_value = ! matched_value;
      }
    return matched_value;
  }

  void address_set::reset ()
  {
    __reverse = false; 
    __mode = MODE_DEFAULT;
    __reset_range ();
    __reset_list ();
  }

  // ctor:
  address_set::address_set ()
  {
    reset ();
  }

  ostream & operator<< (ostream & out_, const address_set & as_)
  {
    if (! as_.is_valid ())
      {
	out_ << "?";
	return out_;
      }
    if (as_.is_reverse ())
      {
	out_ << '!';
      }
    if (as_.is_mode_none ())
      {
	out_ << "{!}";
      }
    else if (as_.is_mode_all ())
      {
	out_ << "{*}";
      }
    else if (as_.is_mode_range ())
      {
	out_ << '[';
	out_ << as_.__range_min;
	out_ << ';';
	out_ << as_.__range_max;
	out_ << ']';
      }
    else
      {
	out_ << '{';
	for (set<uint32_t>::const_iterator i = as_.__addrs.begin ();
	      i != as_.__addrs.end ();
	     i++)
	  {
	    set<uint32_t>::const_iterator j = i;
	    j++;
	    out_ << *i;
	    if (j != as_.__addrs.end ())
	      {
		out_ << ';';
	      }
	  }
	out_ << '}';
      }

    return out_;
  }

  istream & operator>> (istream & in_, address_set & as_)
  {
    //bool devel = true;
    as_.invalidate ();
    bool reverse = false;
    bool mode_none = false;
    bool mode_all = false;
    bool mode_range = false;
    bool mode_list = false;
    char c = in_.peek ();
    if (c == '!')
      {
	in_.get ();
	reverse = true;
	c = in_.peek ();
      }
    if (c == '{')
      {
	in_.get ();
	c = in_.peek ();
	if (c == '!')
	  {
	    in_.get ();
	    c = in_.peek ();
	    if (c != '}')
	      {
		as_.invalidate ();
		in_.setstate (ios::failbit);
		return in_;
	      }
	    in_.get ();
	    as_.set_mode_none ();
	  }
	else if (c == '*')
	  {
	    in_.get ();
	    c = in_.peek ();
	    if (c != '}')
	      {
		as_.invalidate ();
		in_.setstate (ios::failbit);
		return in_;
	      }
	    in_.get ();
	    as_.set_mode_all ();
	  }
	else
	  {
	    //if (devel) cerr << "mode==LIST" << endl;
	    // mode list:
	    while (true)
	      {
		uint32_t v;
		in_ >> v >> ws;
		if (! in_)
		  {
		    return in_;
		  }
		//if (devel) cerr << "mode==LIST v=" << v << endl;
		as_.add_to_list (v);
		c = in_.peek ();
		if (c == '}') 
		  {
		    in_.get ();	
		    break;
		  }
		if (c != ';') 
		  {
		    as_.invalidate ();
		    in_.setstate (ios::failbit);
		    return in_;
		  }
		in_.get ();	
		//if (devel) cerr << "mode==LIST loop a new value" << endl;
	      }
	  }
      }
    else if (c == '[')
      {
	// mode range:
	in_.get ();
	uint32_t min, max;
	in_ >> min;
	if (! in_)
	  {
	    as_.invalidate ();
	    in_.setstate (ios::failbit);
	    return in_;
	  }
	c = in_.peek ();
	if (c != ';')
	  {
	    as_.invalidate ();
	    in_.setstate (ios::failbit);
	    return in_;
	  }
	in_.get ();
	in_ >> max;
	if (! in_)
	  {
	    as_.invalidate ();
	    in_.setstate (ios::failbit);
	    return in_;
	  }
	c = in_.peek ();
	if (c != ']')
	  {
	    as_.invalidate ();
	    in_.setstate (ios::failbit);
	    return in_;
	  }
	in_.get ();
	as_.set_range (min, max);
      }
    else
      {
	as_.invalidate ();
	in_.setstate (ios::failbit);
	return in_;
      }
    as_.set_reverse (reverse);
    return in_;
  }

} // end of namespace geomtools

// end of address_set.cc

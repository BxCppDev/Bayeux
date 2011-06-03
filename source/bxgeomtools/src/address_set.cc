// -*- mode: c++; -*- 
/* address_set.cc 
 */

#include <geomtools/address_set.h>
#include <cstdlib>
#include <stdexcept>
#include <sstream>
#include <string>
#include <algorithm>

namespace geomtools {
  
  bool address_set::is_valid () const
  {
    return mode_ != MODE_INVALID;
  }

  void address_set::invalidate ()
  {
    reset ();
    mode_ = MODE_INVALID;
    return;
  }

  void address_set::reset_list_ ()
  {
    addrs_.clear ();
    return;
  }

  void address_set::reset_range_ ()
  {
    range_min_ = 1;
    range_max_ = 0;
    return;
  }
  
  bool address_set::is_reverse () const
  {
    return reverse_;
  }
 
  void address_set::set_reverse (bool a_reverse)
  {
    reverse_ = a_reverse;
    return;
  }
 
  bool address_set::is_mode_none () const
  {
    return mode_ == MODE_NONE;
  }
  
  bool address_set::is_mode_all () const
  {
    return mode_ == MODE_ALL;
  }
  
  bool address_set::is_mode_range () const
  {
    return mode_ == MODE_RANGE;
  }
  
  bool address_set::is_mode_list () const
  {
    return mode_ == MODE_LIST;
  }
  
  void address_set::set_mode_none ()
  {
    reset_range_ ();
    reset_list_ ();
    mode_ = MODE_NONE;
    return;
  }
  
  void address_set::set_mode_all ()
  {
    reset_range_ ();
    reset_list_ ();
    mode_ = MODE_ALL;
    return;
  }
  
  void address_set::set_mode_range ()
  {
    reset_list_ ();
    reset_range_ ();
    mode_ = MODE_RANGE;
    return;
  }
  
  void address_set::set_range (uint32_t a_min, uint32_t a_max)
  {
    if (! is_mode_range ())
      {
	set_mode_range ();
	//throw runtime_error ("address_set::set_range: Range mode is not active !");
      }
    if (a_min >  a_max)
      {
	reset_range_ ();
      }
    else
     {
       range_min_ = a_min;
       range_max_ = a_max;     
     }
    return;
  }
  
  void address_set::set_mode_range (uint32_t a_min, uint32_t a_max)
  {
    mode_ = MODE_RANGE;
    set_range (a_min, a_max);
    return;
  }

  void address_set::set_mode_list ()
  {
    reset_list_ ();
    reset_range_ ();
    mode_ = MODE_LIST;
    return;
  }

  void address_set::add_to_list (uint32_t a_value)
  {
    if (! is_mode_list ())
      {
	set_mode_list ();
      }
    addrs_.insert (a_value);
    return;
  }

  bool address_set::match (uint32_t a_value) const
  {
    bool matched_value = false;

    if (is_mode_all ())
      {
	matched_value = true;
      }
    else if (is_mode_range ())
      {
	if (a_value < range_min_) matched_value = false;
	else if (a_value > range_max_) matched_value = false;
	else matched_value = true;
      }
    else if (is_mode_list ())
      {
	matched_value = addrs_.find (a_value) != addrs_.end ();
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
    reverse_ = false; 
    mode_ = MODE_DEFAULT;
    reset_range_ ();
    reset_list_ ();
    return;
  }

  // ctor:
  address_set::address_set ()
  {
    reset ();
    return;
  }

  ostream & operator<< (ostream & a_out, const address_set & a_addset)
  {
    if (! a_addset.is_valid ())
      {
	a_out << "?";
	return a_out;
      }
    if (a_addset.is_reverse ())
      {
	a_out << '!';
      }
    if (a_addset.is_mode_none ())
      {
	a_out << "{!}";
      }
    else if (a_addset.is_mode_all ())
      {
	a_out << "{*}";
      }
    else if (a_addset.is_mode_range ())
      {
	a_out << '[';
	a_out << a_addset.range_min_;
	a_out << ';';
	a_out << a_addset.range_max_;
	a_out << ']';
      }
    else
      {
	a_out << '{';
	for (set<uint32_t>::const_iterator i = a_addset.addrs_.begin ();
	      i != a_addset.addrs_.end ();
	     i++)
	  {
	    set<uint32_t>::const_iterator j = i;
	    j++;
	    a_out << *i;
	    if (j != a_addset.addrs_.end ())
	      {
		a_out << ';';
	      }
	  }
	a_out << '}';
      }

    return a_out;
  }

  istream & operator>> (istream & a_in, address_set & a_addset)
  {
    //bool devel = true;
    a_addset.invalidate ();
    bool reverse = false;
    bool mode_none = false;
    bool mode_all = false;
    bool mode_range = false;
    bool mode_list = false;
    char c = a_in.peek ();
    if (c == '!')
      {
	a_in.get ();
	reverse = true;
	c = a_in.peek ();
      }
    if (c == '{')
      {
	a_in.get ();
	c = a_in.peek ();
	if (c == '!')
	  {
	    a_in.get ();
	    c = a_in.peek ();
	    if (c != '}')
	      {
		a_addset.invalidate ();
		a_in.setstate (ios::failbit);
		return a_in;
	      }
	    a_in.get ();
	    a_addset.set_mode_none ();
	  }
	else if (c == '*')
	  {
	    a_in.get ();
	    c = a_in.peek ();
	    if (c != '}')
	      {
		a_addset.invalidate ();
		a_in.setstate (ios::failbit);
		return a_in;
	      }
	    a_in.get ();
	    a_addset.set_mode_all ();
	  }
	else
	  {
	    //if (devel) cerr << "mode==LIST" << endl;
	    // mode list:
	    while (true)
	      {
		uint32_t v;
		a_in >> v >> ws;
		if (! a_in)
		  {
		    return a_in;
		  }
		//if (devel) cerr << "mode==LIST v=" << v << endl;
		a_addset.add_to_list (v);
		c = a_in.peek ();
		if (c == '}') 
		  {
		    a_in.get ();	
		    break;
		  }
		if (c != ';') 
		  {
		    a_addset.invalidate ();
		    a_in.setstate (ios::failbit);
		    return a_in;
		  }
		a_in.get ();	
		//if (devel) cerr << "mode==LIST loop a new value" << endl;
	      }
	  }
      }
    else if (c == '[')
      {
	// mode range:
	a_in.get ();
	uint32_t min, max;
	a_in >> min;
	if (! a_in)
	  {
	    a_addset.invalidate ();
	    a_in.setstate (ios::failbit);
	    return a_in;
	  }
	c = a_in.peek ();
	if (c != ';')
	  {
	    a_addset.invalidate ();
	    a_in.setstate (ios::failbit);
	    return a_in;
	  }
	a_in.get ();
	a_in >> max;
	if (! a_in)
	  {
	    a_addset.invalidate ();
	    a_in.setstate (ios::failbit);
	    return a_in;
	  }
	c = a_in.peek ();
	if (c != ']')
	  {
	    a_addset.invalidate ();
	    a_in.setstate (ios::failbit);
	    return a_in;
	  }
	a_in.get ();
	a_addset.set_range (min, max);
      }
    else
      {
	a_addset.invalidate ();
	a_in.setstate (ios::failbit);
	return a_in;
      }
    a_addset.set_reverse (reverse);
    return a_in;
  }

} // end of namespace geomtools

// end of address_set.cc

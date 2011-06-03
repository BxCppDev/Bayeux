// mygsl::min_max.cc 

#include <mygsl/min_max.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>

namespace mygsl {

  double min_max::get_min () const
  {
    return __min;
  }

  double min_max::get_max () const
  {
    return __max;
  }

  bool min_max::is_valid () const
  {
    return __min <= __max;
  }

  double min_max::get_median () const
  {
    return 0.5 * (__min + __max);
  }
  
  min_max::min_max ()
  {
    reset ();
    return;
  }

  void min_max::reset ()
  {
    __min = 0.0;
    __max = -1.0;
    return;
  }

  void min_max::add (double value_)
  {
    if (__min > __max)
      {
	__min = value_;
	__max = value_;
      }
    else
      {
	if (value_ < __min)
	  {
	    __min = value_;
	  }
	if (value_ > __max)
	  {
	    __max = value_;
	  }
      }    
    return;
  }

}

// end of mygsl::min_max.cc


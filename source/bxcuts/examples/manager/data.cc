// -*- mode: c++ ; -*-
/* data.cc */

// Ourselves:
#include <data.h>

// Standard library:
#include <limits>

data::data () {
  x = y = z = std::numeric_limits<double>::quiet_NaN();
  color = COLOR_INVALID;
}

void data::print (std::ostream & out_) const {
  out_ << x << ' ' << y << ' ' << z << ' ' << color << std::endl;
}

void data::reset()
{
  x = y = z = std::numeric_limits<double>::quiet_NaN();
  color = COLOR_INVALID;
}

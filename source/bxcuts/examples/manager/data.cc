// -*- mode: c++ ; -*-
/* data.cc */

#include <data.h>

data::data () {
  x = y = z = 0.0;
  color = BLACK;
}

void data::print (std::ostream & out_) const {
  out_ << x << ' ' << y << ' ' << z << ' ' << color << std::endl;
}

// end of data.cc

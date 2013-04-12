// -*- mode: c++; -*-
// histogram_utils.h

#ifndef MYGSL_HISTOGRAM_UTILS_H_
#define MYGSL_HISTOGRAM_UTILS_H_ 1

namespace mygsl {

  enum bin_mode_type
  {
    BIN_MODE_INVALID = 0,
    BIN_MODE_LINEAR  = 1,
    BIN_MODE_LOG     = 2
  };

  enum bin_axis_type
  {
    BIN_AXIS_INVALID = -1,
    BIN_AXIS_X = 0,
    BIN_AXIS_Y = 1
  };

} // end of namespace mygsl

#endif // MYGSL_HISTOGRAM_UTILS_H_

// end of histogram_utils.h

// -*- mode: c++; -*-
// histogram_utils.h

#ifndef __mygsl__histogram_utils_h
#define __mygsl__histogram_utils_h 1

namespace mygsl {

  enum bin_mode_t
  {
    BIN_MODE_INVALID = 0,
    BIN_MODE_LINEAR  = 1,
    BIN_MODE_LOG     = 2
  };

  enum bin_axis_t
  {
    BIN_AXIS_INVALID = -1,
    BIN_AXIS_X = 0,
    BIN_AXIS_Y = 1
  };

} // end of namespace mygsl

#endif // __mygsl__histogram_utils_h

// end of histogram_utils.h

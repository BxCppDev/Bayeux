/// \file mygsl/histogram_utils.h

#ifndef MYGSL_HISTOGRAM_UTILS_H
#define MYGSL_HISTOGRAM_UTILS_H 1

namespace mygsl {

  /// \brief Histogram binning mode
  enum bin_mode_type {
    BIN_MODE_INVALID = 0, /// Invalid mode
    BIN_MODE_LINEAR  = 1, /// Linear
    BIN_MODE_LOG     = 2  /// Logarithmic
  };

  /// \brief Histogram axis type
  enum bin_axis_type {
    BIN_AXIS_INVALID = -1, /// invalid axis
    BIN_AXIS_X = 0, /// X axis
    BIN_AXIS_Y = 1  /// Y axis
  };

} // end of namespace mygsl

#endif // MYGSL_HISTOGRAM_UTILS_H

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

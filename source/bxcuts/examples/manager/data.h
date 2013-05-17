// -*- mode: c++ ; -*-
/* data.h
 * Author (s) : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-05-10
 * Last modified: 2013-05-10
 */

#ifndef DATA_H_
#define DATA_H_ 1

#include <iostream>

/// \brief A simple data structure with (x,y,z) position
///        in 3D-space and associated color.
class data
{
public:

  /// \brief Color types
  enum color_type {
    COLOR_INVALID = -1,
    COLOR_BLACK   =  0,
    COLOR_RED     =  1,
    COLOR_GREEN   =  2,
    COLOR_BLUE    =  3
  };

  /// Constructor
  data();

  /// Smart print
  void print(std::ostream & out_) const;

  /// Reset
  void reset();

public:

  double x, y, z; //!< Cartesian 3D-coordinates of the point
  int    color;   //!< Color ID associated to the point

};

#endif // DATA_H_

// end of data.h

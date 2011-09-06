// -*- mode: c++; -*- 
/* polyline_3d.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009-03-31
 * Last modified: 2009-03-31
 */

#ifndef __geomtools__polyline_3d_h
#define __geomtools__polyline_3d_h 1

#include <iostream>
#include <stdexcept>
#include <list>

#include <datatools/serialization/i_serializable.h>

#include <geomtools/i_shape_1d.h>
#include <geomtools/utils.h>

namespace geomtools {

  using namespace std;

  class polyline_3d : 
    public i_shape_1d,
    DATATOOLS_SERIALIZABLE_CLASS
    {
    public:
      static const string POLYLINE_3D_LABEL;

      static const bool CLOSED = true;
      static const bool closed = CLOSED;
      static const bool OPEN   = false;
      static const bool open   = OPEN;
      static const bool DEFAULT_CLOSED = OPEN;

      typedef basic_polyline_3d point_col;

    public:

      virtual string get_shape_name () const;

      bool is_closed () const;

      void set_closed (bool);

      bool is_empty () const;

      polyline_3d ();

      polyline_3d (bool closed_);

      virtual ~polyline_3d ();

      void clear ();

      void add (const vector_3d &);

      int get_number_of_points () const;
      
      const vector_3d & get_point (int i_) const;

      int get_number_of_vertex () const;
      
      const vector_3d & get_vertex (int i_) const;

      // inefficient algorithm:
      void make_vertex_collection (basic_polyline_3d &) const;

      // inefficient algorithm:
      basic_polyline_3d make_vertex_collection () const;
 
    public:

      virtual bool is_on_curve (const vector_3d & position_, 
				double tolerance_ = USING_PROPER_TOLERANCE) const;
      
      virtual vector_3d get_direction_on_curve (const vector_3d & position_) const;

    private:

      bool      __closed;
      point_col __points;

      /* interface i_serializable */
      DATATOOLS_SERIALIZATION_DECLARATION();
      
    }; 
 
} // end of namespace geomtools

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(geomtools::polyline_3d, "geomtools::polyline_3d")

#endif // __geomtools__polyline_3d_h

// end of box.h


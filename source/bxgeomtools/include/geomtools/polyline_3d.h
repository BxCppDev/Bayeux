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

#include <geomtools/utils.h>

namespace geomtools {

  class polyline_3d : 
    public datatools::serialization::i_serializable
    {
    public:
      static const std::string SERIAL_TAG;

      static const bool CLOSED = true;
      static const bool closed = CLOSED;
      static const bool OPEN   = false;
      static const bool open   = OPEN;
      static const bool DEFAULT_CLOSED = OPEN;

      typedef basic_polyline_3d point_col;

    private:
      bool      __closed;
      point_col __points;

    public:

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

      virtual const std::string & get_serial_tag () const;

      // inefficient algorithm:
      void make_vertex_collection (basic_polyline_3d &) const;

      // inefficient algorithm:
      basic_polyline_3d make_vertex_collection () const;

    private:
      friend class boost::serialization::access; 
      template<class Archive>
	void serialize (Archive            & ar_, 
			const unsigned int   version_)
	{
	  ar_ & boost::serialization::make_nvp ("closed", __closed);
	  ar_ & boost::serialization::make_nvp ("points", __points);
	}

 
    }; 
 
} // end of namespace geomtools

#endif // __geomtools__polyline_3d_h

// end of box.h


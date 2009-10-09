// -*- mode: c++; -*- 
/* line_3d.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-12-18
 * Last modified: 2008-12-18
 * 
 * License: 
 * 
 * Description: 
 *   Line 3D model
 *
 *
 * History: 
 * 
 */

#ifndef __geomtools__line_3d_h
#define __geomtools__line_3d_h 1

#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>

#include <datatools/serialization/serialization.h>

#include <datatools/utils/i_tree_dump.h>

#include <geomtools/utils.h>

namespace geomtools {

  class line_3d :
    public datatools::utils::i_tree_dumpable,
    public datatools::serialization::i_serializable
  {
  private: 
    vector_3d __first;
    vector_3d __last;

  public: 

    bool is_valid () const;

    void invalidate ();

    const vector_3d & get_first () const;

    void set_first (const vector_3d &);

    const vector_3d & get_last () const;

    void set_last (const vector_3d &);

    vector_3d get_point (double t_) const;
  
    double get_length () const;

  public: 

    // ctor/dtor:
    line_3d ();

    line_3d (const vector_3d & first_, const vector_3d & last_);

    virtual ~line_3d ();

    /* interface i_tree_dumpable */
    virtual void tree_dump (std::ostream & out_         = std::cerr, 
			    const std::string & title_  = "",
			    const std::string & indent_ = "",
			    bool inherit_               = false) const;

    void dump () const
    {
      tree_dump (std::clog);
    }

    void make_vertex_collection (basic_polyline_3d &) const;
    
    basic_polyline_3d make_vertex_collection () const;

    /* interface i_serializable */
    static const std::string SERIAL_TAG;
    
    virtual const std::string & get_serial_tag () const;
    
  private:
      
    friend class boost::serialization::access; 
    
    template<class Archive>
    void serialize (Archive            & ar_, 
		    const unsigned int version_) 
    {
      ar_ & boost::serialization::make_nvp ("first", __first);
      ar_ & boost::serialization::make_nvp ("last", __last);
    }

    /*
  public:
    
    static void print_xyz (std::ostream & out_, 
			   const line_3d & line_, 
			   double step_ = 0.0);
    */

  };

} // end of namespace geomtools

#endif // __geomtools__line_3d_h

// end of line_3d.h

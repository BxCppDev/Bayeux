// -*- mode: c++; -*- 
/* i_shape_3d.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2010-02-14
 * 
 * License: 
 * 
 * Description: 
 *  Interface for 3D shaped volumes
 *
 * History: 
 * 
 */

#ifndef __geomtools__i_shape_3d_h
#define __geomtools__i_shape_3d_h 1

#include <string>

#include <geomtools/utils.h>
#include <geomtools/i_object_3d.h>
#include <geomtools/i_stackable.h>

namespace geomtools {

  class i_shape_3d : public i_object_3d
  {
  public:
    static const double DEFAULT_SKIN;
    static const double USING_PROPER_SKIN;

  public:

    /** Check if a 3D-shape can be stacked using some
     *  stacking algorithms. There are 2 checks:
     *  \i check #1: checks if the instance owns a valid 
     *  embedded 'stackable_data' instance.
     *  \i check #2: if check #1 fails, check if the instance inherits 
     *  the 'i_stackable' interface.
     */
    static bool is_stackable (const i_shape_3d &);

    static bool is_xmin_stackable (const i_shape_3d &);

    static bool is_xmax_stackable (const i_shape_3d &);

    static bool is_ymin_stackable (const i_shape_3d &);

    static bool is_ymax_stackable (const i_shape_3d &);

    static bool is_zmin_stackable (const i_shape_3d &);

    static bool is_zmax_stackable (const i_shape_3d &);

    /** Initialize a 'stackable_data' instance
     *  from stackable data attached to the 3D-shape.
     */
    static bool pickup_stackable (const i_shape_3d &, stackable_data &);

    static bool pickup_stackable_with_properties (const i_shape_3d & a_shape, 
                                                  stackable_data & a_stackable_data);

    const stackable_data & get_stackable_data () const;

    bool has_stackable_data () const;

    void set_stackable_data (const stackable_data & a_stackable_data);

    void set_stackable_data (const stackable_data * a_stackable_data);

    virtual int get_dimensional () const;
      
    double get_skin () const;

    void set_skin (double a_skin);

    // ctor:
    i_shape_3d ();

    // ctor:
    i_shape_3d (double a_skin);

    // dtor:
    virtual ~i_shape_3d ();

    virtual bool is_composite () const;

    // bool has_bounding_box () const;

    // const box & get_bounding_box () const;

    //virtual double 
    //  get_parameter(const std::string &) const = 0;

    virtual bool is_inside (const vector_3d &, 
                            double a_skin = USING_PROPER_TOLERANCE) const = 0;
      
    virtual bool is_on_surface (const vector_3d &,
                                int a_surface_mask = ALL_SURFACES, 
                                double a_skin = USING_PROPER_TOLERANCE) const = 0; 
      
    virtual vector_3d get_normal_on_surface (const vector_3d & a_position) const = 0;
      
    virtual bool is_outside (const vector_3d &,
                             double a_skin = USING_PROPER_TOLERANCE) const; 
      

    virtual bool find_intercept (const vector_3d & a_from, 
                                 const vector_3d & a_direction,
                                 intercept_t & a_intercept,
                                 double a_skin = USING_PROPER_TOLERANCE) const;

    virtual void tree_dump (ostream & a_out         = clog, 
                            const string & a_title  = "", 
                            const string & a_indent = "", 
                            bool a_inherit          = false) const;

  private:

    bool                   _owns_stackable_data_; //!< Ownership flag for stackable data
    const stackable_data * _stackable_data_; //!< Handle to stackable data
 
  };
    
} // end of namespace geomtools

#endif // __geomtools__i_shape_3d_h

// end of i_shape_3d.h

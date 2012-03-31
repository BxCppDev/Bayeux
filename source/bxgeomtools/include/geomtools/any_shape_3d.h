// -*- mode: c++; -*- 
/* any_shape_3d.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2008-05-23
 * 
 * License: 
 * 
 * Description: 
 *  Interface:
 *
 * History: 
 * 
 */

#ifndef __geomtools__any_shape_3d_h
#define __geomtools__any_shape_3d_h 1

#include <sstream>
#include <stdexcept>

#include <geomtools/i_shape_3d.h>
#include <geomtools/box.h>

namespace geomtools {

  class any_shape_3d : public i_shape_3d 
  {

  public:
    static const std::string ANY_SHAPE_3D_LABEL;
    enum faces_mask_type
      {
        FACE_NONE = FACE_NONE_BIT,
        FACE_ALL  = 0xFFFFFFFF
      };

  public: 

    void set_shape_name (const std::string & shape_name_);

    virtual void compute_bounding_box (box & bb_);

    void set_bounding_box (const box & bounding_box_);

    const box & get_bounding_box () const;

    // ctor/dtor:
  public: 
    any_shape_3d ();

    any_shape_3d (const std::string & name_);

    virtual ~any_shape_3d ();
    
    // methods:
      
    virtual std::string get_shape_name () const;

    bool is_valid () const;

    void reset ();

    virtual bool 
    is_inside (const vector_3d &, 
               double skin_ = USING_PROPER_SKIN) const;

    // if 'skin' < 0 no skin is taken into account:
    virtual bool 
    is_on_surface (const vector_3d & , 
                   int mask_    = FACE_ALL , 
                   double skin_ = USING_PROPER_SKIN) const;

    virtual vector_3d get_normal_on_surface (const vector_3d & position_) const;
      
    virtual bool find_intercept (const vector_3d & from_, 
                                 const vector_3d & direction_,
                                 intercept_t & intercept_,
                                 double skin_ = USING_PROPER_SKIN) const;

  private: 

    std::string _shape_name_;
    box    _bounding_box_;
       
  };
    
} // end of namespace geomtools

#endif // __geomtools__any_shape_3d_h

// end of any_shape_3d.h


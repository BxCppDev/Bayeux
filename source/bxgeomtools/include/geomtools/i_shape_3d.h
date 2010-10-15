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

#include <datatools/utils/properties.h>

#include <geomtools/utils.h>
#include <geomtools/i_object_3d.h>
#include <geomtools/i_stackable.h>

namespace geomtools {

  //class box;

  class i_shape_3d : public i_object_3d
    {
    public:
      static const double DEFAULT_SKIN;
      static const double USING_PROPER_SKIN;

    private:

      bool                   __owns_stackable_data;
      const stackable_data * __stackable_data;

    public:

      /** Check if a 3D-shape can be stacked using some
       *  stacking algorithms. There are 2 checks:
       *  \i check #1: checks if the instance owns a valid 
       *  embedded 'stackable_data' instance.
       *  \i check #2: if check #1 fails, check if the instance inherits 
       *  the 'i_stackable' interface.
       */
      static bool is_stackable (const i_shape_3d &);

      /** Initialize a 'stackable_data' instance
       *  from stackable data attached to the 3D-shape.
       */
      static bool pickup_stackable (const i_shape_3d &, stackable_data &);

      const stackable_data & get_stackable_data () const;

      bool has_stackable_data () const;

      void set_stackable_data (const stackable_data & sd_);

      void set_stackable_data (const stackable_data * sd_);

      virtual int get_dimensional () const;
      
      double get_skin () const;

      void set_skin (double skin_);

      // ctor:
      i_shape_3d ();

      // ctor:
      i_shape_3d (double skin_);

      // dtor:
      virtual ~i_shape_3d ();

      virtual bool is_composite () const
      {
	return false;
      }

      // bool has_bounding_box () const;

      // const box & get_bounding_box () const;

      //virtual double 
      //	get_parameter(const std::string &) const = 0;

      virtual bool is_inside (const vector_3d &, 
			      double skin_ = USING_PROPER_TOLERANCE) const = 0;
      
      virtual bool is_on_surface (const vector_3d &,
				  int surface_mask_ = ALL_SURFACES, 
				  double skin_ = USING_PROPER_TOLERANCE) const = 0; 
      
      virtual vector_3d get_normal_on_surface (const vector_3d & position_) const = 0;
      
      virtual bool is_outside (const vector_3d &,
			       double skin_ = USING_PROPER_TOLERANCE) const; 
      

      virtual bool find_intercept (const vector_3d & from_, 
				   const vector_3d & direction_,
				   intercept_t & intercept_,
				   double skin_ = USING_PROPER_TOLERANCE) const
      {
	// temporary:
	// NOT IMPLEMENTED !
	// default: no intercept on any face of the 3D shape...
	intercept_.reset ();
	return intercept_.is_ok ();
      }

      virtual void tree_dump (ostream & out_         = clog, 
			      const string & title_  = "", 
			      const string & indent_ = "", 
			      bool inherit_          = false) const;
 
  };
    
} // end of namespace geomtools

#endif // __geomtools__i_shape_3d_h

// end of i_shape_3d.h

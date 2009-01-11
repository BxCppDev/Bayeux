// -*- mode: c++; -*- 
/* i_shape_3d.h
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

#ifndef __geomtools__i_shape_3d_h
#define __geomtools__i_shape_3d_h 1

#include <string>

#include <datatools/utils/properties.h>

#include <geomtools/utils.h>

namespace geomtools {

  class i_shape_3d 
    {
    public:
      static const double DEFAULT_SKIN;
      static const unsigned int ALL_SURFACES;
      static const unsigned int NO_SURFACES;
      static const double USING_PROPER_SKIN;

    private:
      double __skin; // proper tolerance to check surface belonging
      datatools::utils::properties __properties;

    public:

      double get_skin () const;

      void set_skin (double skin_);

      datatools::utils::properties & properties ();

      const datatools::utils::properties & properties () const;

      i_shape_3d (double skin_ = DEFAULT_SKIN);

      virtual ~i_shape_3d ();

      virtual std::string get_shape_name () const = 0;

      virtual bool is_composite () const
      {
	return false;
      }

      //virtual double 
      //	get_parameter(const std::string &) const = 0;

      virtual bool is_inside (const vector_3d &, 
			      double skin_ = USING_PROPER_SKIN) const = 0;
      
      virtual bool is_on_surface (const vector_3d &,
				  int surface_mask_ = ALL_SURFACES, 
				  double skin_ = USING_PROPER_SKIN) const = 0; 
      
      virtual vector_3d get_normal_on_surface (const vector_3d & position_) const = 0;
      
      virtual bool is_outside (const vector_3d &,
			       double skin_ = USING_PROPER_SKIN) const; 
      
      virtual bool find_intercept (const vector_3d & from_, 
				   const vector_3d & direction_,
				   vector_3d & intercept_,
				   int & face_,
				   double skin_ = USING_PROPER_SKIN) const
      {
	// temporary:
	// default: no intercept on any face of the 3D shape...
	face_ = face_3d::FACE_NONE_BIT;
	return false; // there is no solution
      }

  };
    
} // end of namespace geomtools

#endif // __geomtools__i_shape_3d_h

// end of i_shape_3d.h

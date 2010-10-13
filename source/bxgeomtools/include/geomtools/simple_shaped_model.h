// -*- mode: c++ ; -*- 
/* simple_shaped_model.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-10-13
 * Last modified: 2010-10-13
 * 
 * License: 
 * 
 * Description: 
 *   Model implementing a logical volume with a simple shape
 *   box, cylinder, sphere, tube
 * 
 * History: 
 * 
 */

#ifndef __geomtools__simple_shaped_model_h
#define __geomtools__simple_shaped_model_h 1

#include <iostream>
#include <string> 
#include <exception>
#include <limits> 

#include <datatools/utils/units.h>

#include <geomtools/i_model.h>

#include <geomtools/box.h>
#include <geomtools/cylinder.h>
#include <geomtools/tube.h>
#include <geomtools/sphere.h>

#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/logical_volume.h>

#include <geomtools/visibility.h>
#include <geomtools/material.h>
#include <geomtools/sensitive.h>

namespace geomtools {

  using namespace std;

  // define a geometry model made of a unique simple 3D shape: 
  class simple_shaped_model : public i_model
    {
    private:

      string                __shape_name;

      geomtools::box *      __box;
      geomtools::cylinder * __cylinder;
      geomtools::tube *     __tube;
      geomtools::sphere *   __sphere;

      geomtools::i_shape_3d * __solid;
      geomtools::i_shape_3d * __inner_shape; //!> for tube only

      placement               __inner_placement;
      logical_volume          __inner_logical;
      physical_volume         __inner_phys;

    public: 

      string get_material_name () const;
      const string & get_shape_name () const;

      const geomtools::box & get_box () const;
      const geomtools::cylinder & get_cylinder () const;
      const geomtools::tube & get_tube () const;
      const geomtools::sphere & get_sphere () const;

      const geomtools::i_shape_3d & get_solid () const;

    public:
  
      simple_shaped_model ();
  
      virtual ~simple_shaped_model ();

    public:

      virtual string get_model_id () const;

    protected:
  
      virtual void _at_construct (const string & name_,
				  const datatools::utils::properties & config_,
				  models_col_t * models_ = 0);

    private:

      static creator_registration<simple_shaped_model> __CR;
      
    public: 
      virtual void tree_dump (ostream & out_         = clog, 
                              const string & title_  = "", 
                              const string & indent_ = "", 
                              bool inherit_          = false) const;
  
    };

} // end of namespace geomtools

#endif // __geomtools__simple_shaped_model_h

// end of simple_shaped_model.h

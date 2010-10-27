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
 *   box, cylinder, sphere, tube, polycone, polyhedra
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
#include <geomtools/polycone.h>
#include <geomtools/polyhedra.h>

#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/logical_volume.h>

#include <geomtools/visibility.h>
#include <geomtools/material.h>
#include <geomtools/sensitive.h>
#include <geomtools/color.h>
#include <geomtools/i_stackable.h>
#include <geomtools/utils.h>
//#include <geomtools/mapping.h>

#include <geomtools/model_with_internal_items_tools.h>

namespace geomtools {

  using namespace std;

  // define a geometry model made of a unique simple 3D shape: 
  class simple_shaped_model : public i_model
    {
    private:

      string                 __shape_name;
      int                    __filled_mode;
      string                 __material_name;
      string                 __filled_material_name;

      geomtools::box *       __box;
      geomtools::cylinder *  __cylinder;
      geomtools::tube *      __tube;
      geomtools::sphere *    __sphere;
      geomtools::polycone *  __polycone;
      geomtools::polyhedra * __polyhedra;

      geomtools::i_shape_3d * __solid;
      geomtools::i_shape_3d * __inner_shape; //!> for filled tube or polycone or polyhedra
      geomtools::i_shape_3d * __outer_shape; //!> for mother polycone or polyhedra

      placement               __inner_placement;
      logical_volume          __inner_logical;
      physical_volume         __inner_phys;

      logical_volume        * __daughter_owner_logical;
      logical_volume        * __visibility_logical;

      // internal items:
      MWIM                    __internals;

    public: 

      MWIM & get_internals ();
      const MWIM & get_internals () const;

      const string & get_material_name () const;
      const string & get_filled_material_name () const;
      const string & get_shape_name () const;

      const geomtools::box & get_box () const;
      const geomtools::cylinder & get_cylinder () const;
      const geomtools::tube & get_tube () const;
      const geomtools::sphere & get_sphere () const;
      const geomtools::polycone & get_polycone () const;
      const geomtools::polyhedra & get_polyhedra () const;

      const geomtools::i_shape_3d & get_solid () const;

    public:
 
      bool is_filled () const;

      bool is_filled_by_envelope () const;

      bool is_filled_by_extrusion () const;

      simple_shaped_model ();
  
      virtual ~simple_shaped_model ();

    public:

      virtual string get_model_id () const;

    protected:

      virtual void _post_construct (datatools::utils::properties & setup_);
  
      virtual void _at_construct (const string & name_,
				  const datatools::utils::properties & config_,
				  models_col_t * models_ = 0);

    protected:

      virtual void _construct_box (const string & name_,
				   const datatools::utils::properties & config_,
				   models_col_t * models_);

      virtual void _construct_cylinder (const string & name_,
					const datatools::utils::properties & config_,
					models_col_t * models_);
      
      virtual void _construct_sphere (const string & name_,
				      const datatools::utils::properties & config_,
				      models_col_t * models_);
      
      virtual void _construct_tube (const string & name_,
				    const datatools::utils::properties & config_,
				    models_col_t * models_);
      
      virtual void _construct_polycone (const string & name_,
					const datatools::utils::properties & config_,
					models_col_t * models_);

      virtual void _construct_polyhedra (const string & name_,
					 const datatools::utils::properties & config_,
					 models_col_t * models_);
      
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

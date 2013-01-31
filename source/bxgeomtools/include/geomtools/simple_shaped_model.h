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

#ifndef GEOMTOOLS_SIMPLE_SHAPED_MODEL_H_
#define GEOMTOOLS_SIMPLE_SHAPED_MODEL_H_ 1

#include <string> 

#include <geomtools/i_model.h>
#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/logical_volume.h>
#include <geomtools/model_with_internal_items_tools.h>

namespace geomtools {

  class box;
  class cylinder;
  class tube;
  class sphere;
  class polycone;
  class polyhedra;
  class i_shape_3d;
  class logical_volume;

  // define a geometry model made of a unique simple 3D shape: 
  GEOMTOOLS_MODEL_CLASS_DECLARE(simple_shaped_model)
  {

  public: 

    MWIM & get_internals ();
    const MWIM & get_internals () const;

    const std::string & get_material_name () const;
    const std::string & get_filled_material_name () const;
    const std::string & get_shape_name () const;

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

    virtual std::string get_model_id () const;

  protected:

    virtual void _post_construct (datatools::properties & setup_);
  
    virtual void _at_construct (const std::string & name_,
                                const datatools::properties & config_,
                                models_col_type * models_ = 0);

  protected:

    virtual void _construct_box (const std::string & name_,
                                 const datatools::properties & config_,
                                 models_col_type * models_);

    virtual void _construct_cylinder (const std::string & name_,
                                      const datatools::properties & config_,
                                      models_col_type * models_);
      
    virtual void _construct_sphere (const std::string & name_,
                                    const datatools::properties & config_,
                                    models_col_type * models_);
      
    virtual void _construct_tube (const std::string & name_,
                                  const datatools::properties & config_,
                                  models_col_type * models_);
      
    virtual void _construct_polycone (const std::string & name_,
                                      const datatools::properties & config_,
                                      models_col_type * models_);

    virtual void _construct_polyhedra (const std::string & name_,
                                       const datatools::properties & config_,
                                       models_col_type * models_);
      
  public: 

    virtual void tree_dump (std::ostream & out_         = std::clog, 
                            const std::string & title_  = "", 
                            const std::string & indent_ = "", 
                            bool inherit_          = false) const;
  private:

    std::string                 _shape_name_;
    int                    _filled_mode_;
    std::string                 _material_name_;
    std::string                 _filled_material_name_;

    geomtools::box *       _box_;
    geomtools::cylinder *  _cylinder_;
    geomtools::tube *      _tube_;
    geomtools::sphere *    _sphere_;
    geomtools::polycone *  _polycone_;
    geomtools::polyhedra * _polyhedra_;

    geomtools::i_shape_3d * _solid_;
    geomtools::i_shape_3d * _inner_shape_; //!< for filled tube or polycone or polyhedra
    geomtools::i_shape_3d * _outer_shape_; //!< for mother polycone or polyhedra

    placement               _inner_placement_;
    logical_volume          _inner_logical_;
    physical_volume         _inner_phys_;

    logical_volume        * _daughter_owner_logical_;
    logical_volume        * _visibility_logical_;

    // internal items :
    MWIM                    _internals_;
 
    // registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(simple_shaped_model);
  
  };

} // end of namespace geomtools

#endif // GEOMTOOLS_SIMPLE_SHAPED_MODEL_H_

// end of simple_shaped_model.h

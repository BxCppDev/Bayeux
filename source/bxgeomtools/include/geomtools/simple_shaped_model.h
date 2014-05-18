// -*- mode: c++ ; -*-
/// \file geomtools/simple_shaped_model.h
/* Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
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

// Standard library:
#include <string>

// This project:
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

    MWIM & grab_internals ();

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

    bool is_filled () const;

    bool is_filled_by_envelope () const;

    bool is_filled_by_extrusion () const;

    simple_shaped_model ();

    virtual ~simple_shaped_model ();

    virtual std::string get_model_id () const;

    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const;

  protected:

    virtual void _pre_construct (datatools::properties & setup_,
                                 models_col_type * models_);

    virtual void _post_construct (datatools::properties & setup_,
                                  models_col_type * models_);

    virtual void _at_construct (const std::string & name_,
                                const datatools::properties & config_,
                                models_col_type * models_ = 0);

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

  private:

    std::string _shape_name_;                //!< The name of the shape
    std::string _material_name_;             //!< The name of the material the shape is made of
    filled_utils::filled_type _filled_mode_; //!< The filled mode (for tube, polycone or polyhedra)
    std::string _filled_material_name_;      //!< The name of the material of the shape cavity (for tube, polycone or polyhedra)
    std::string _filled_label_;              //!< The label of the shape in enveloppe filled mode

    // Effective solids:
    geomtools::box *       _box_;
    geomtools::cylinder *  _cylinder_;
    geomtools::tube *      _tube_;
    geomtools::sphere *    _sphere_;
    geomtools::polycone *  _polycone_;
    geomtools::polyhedra * _polyhedra_;

    geomtools::i_shape_3d * _solid_;       //!< Solid handle
    geomtools::i_shape_3d * _inner_shape_; //!< For filled tube or polycone or polyhedra
    geomtools::i_shape_3d * _outer_shape_; //!< For mother polycone or polyhedra

    placement               _inner_placement_;
    logical_volume          _inner_logical_;
    physical_volume         _inner_phys_;

    //logical_volume        * _daughter_owner_logical_; //!< The logical volume that is the top level mother of all daughter volumes implied by this model

    MWIM                    _internals_;        //!< internal items within the shape
    MWIM                    _filled_internals_; //!< internal items within the cavity (type, polycone, polyhedra)

    // registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(simple_shaped_model);

  };

} // end of namespace geomtools

#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(geomtools::simple_shaped_model)

#endif // GEOMTOOLS_SIMPLE_SHAPED_MODEL_H_

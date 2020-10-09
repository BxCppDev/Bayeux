/// \file geomtools/simple_shaped_model.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-10-13
 * Last modified: 2014-05-19
 *
 * License:
 *
 * Description:
 *
 *   Model implementing a logical volume with a simple shape
 *   box, cylinder, sphere, tube, polycone, polyhedra or whatever solid (3D-shape)
 *   registerable in a shape factory.
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_SIMPLE_SHAPED_MODEL_H
#define GEOMTOOLS_SIMPLE_SHAPED_MODEL_H 1

// Standard library:
#include <string>

// This project:
#include <geomtools/i_model.h>
#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/logical_volume.h>
#include <geomtools/model_with_internal_items_tools.h>

namespace geomtools {

  // Forward declarations:
  class box;
  class cylinder;
  class tube;
  class sphere;
  class polycone;
  class polyhedra;
  class i_shape_3d;
  class logical_volume;

  /// \brief A geometry model implementing an unique simple 3D shape, possibly with internal/daughter volumes
  class simple_shaped_model : public i_model
  {
  public:

    /// \brief Shape build mode
    enum shape_build_mode_type {
      SBM_INVALID = 0, //!< Invalid
      SBM_LEGACY  = 1, //!< Build shape from hardcoded (legacy)
      SBM_FACTORY = 2, //!< Build shape from a factory (needs a reference shape factory)
      SBM_DEFAULT = SBM_LEGACY
    };

    MWIM & grab_internals ();

    const MWIM & get_internals () const;

    const std::string & get_material_name () const;

    const std::string & get_filled_material_name () const;

    shape_build_mode_type get_shape_build_mode() const;

    const std::string & get_shape_type_id () const;

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

    ~simple_shaped_model () override;

    std::string get_model_id () const override;

    void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const override;

  protected:

    void _pre_construct (datatools::properties & setup_,
                                 models_col_type * models_) override;

    void _post_construct (datatools::properties & setup_,
                                  models_col_type * models_) override;

    void _at_construct (const std::string & name_,
                                const datatools::properties & config_,
                                models_col_type * models_ = 0) override;

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

    shape_build_mode_type _sbm_;             //!< The shape build mode
    std::string _shape_type_id_;             //!< The type identifier of the shape
    std::string _material_name_;             //!< The name of the material the shape is made of
    filled_utils::filled_type _filled_mode_; //!< The filled mode for tube, polycone or polyhedra (legacy)
    std::string _filled_material_name_;      //!< The name of the material of the shape cavity for tube, polycone or polyhedra (legacy)
    std::string _filled_label_;              //!< The label of the shape in enveloppe filled mode (legacy)

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

    placement               _inner_placement_;  //!< Inner daughter volume placement
    logical_volume          _inner_logical_;    //!< Inner logical volume
    physical_volume         _inner_phys_;       //!< Inner physical volume

    //logical_volume        * _daughter_owner_logical_; //!< The logical volume that is the top level mother of all daughter volumes implied by this model

    MWIM                    _internals_;        //!< Internal items within the shape
    MWIM                    _filled_internals_; //!< Internal items within the cavity (tube, polycone, polyhedra)

    // registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(simple_shaped_model)

  };

} // end of namespace geomtools

#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(geomtools::simple_shaped_model)

#endif // GEOMTOOLS_SIMPLE_SHAPED_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

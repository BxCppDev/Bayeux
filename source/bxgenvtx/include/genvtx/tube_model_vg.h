// -*- mode: c++; -*-
/* tube_model_vg.h
 * Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-06-14
 * Last modified: 2013-06-14
 *
 * License:
 *
 * Description:
 *
 *   Vertex generator from a tube model addressed through some
 *   collection of geometry IDs extracted from a mapping object.
 *
 * History:
 *
 */

#ifndef GENVTX_TUBE_MODEL_VG_H_
#define GENVTX_TUBE_MODEL_VG_H_ 1

#include <string>

#include <geomtools/id_selector.h>

#include <genvtx/tube_vg.h>
#include <genvtx/vg_macros.h>
#include <genvtx/utils.h>

namespace genvtx {

  GENVTX_VG_CLASS_DECLARE(tube_model_vg)
  {
  public:

    bool is_mode_valid () const;

    bool is_mode_bulk () const;

    bool is_mode_surface () const;

    bool is_surface_inner_side () const;

    bool is_surface_outer_side () const;

    bool is_surface_bottom () const;

    bool is_surface_top () const;

    void set_surface_inner_side (bool);

    void set_surface_outer_side (bool);

    void set_surface_bottom (bool);

    void set_surface_top (bool);

    void set_skin_skip (double skin_skip_);

    void set_skin_thickness (double skin_thickness_);

    const std::string & get_origin_rules () const;

    void set_origin_rules (const std::string &);

    bool has_mapping_plugin_name() const;

    const std::string & get_mapping_plugin_name() const;

    void set_mapping_plugin_name(const std::string & mpn_);

    bool has_materials_plugin_name() const;

    const std::string & get_materials_plugin_name() const;

    void set_materials_plugin_name(const std::string & mpn_);

    int get_mode () const;

    void set_mode (int);

    virtual void tree_dump (std::ostream & out_ = std::clog,
                            const std::string & title_ = "",
                            const std::string & indent_ = "",
                            bool inherit_ = false) const;

    GENVTX_VG_INTERFACE_CTOR_DTOR (tube_model_vg);

  private:

    void _init_ ();

    void _reset_ ();

    void _set_defaults_ ();

  protected:

    void _shoot_vertex_tubes (mygsl::rng & random_,
                              geomtools::vector_3d & vertex_);

  private:

    bool                    _initialized_;           //!< Initialization flag
    int                     _mode_;                  //!< Mode : "bulk" of "surface"
    bool                    _surface_inner_side_;    //!< Flag for inner side surface generation mode
    bool                    _surface_outer_side_;    //!< Flag for outer side surface generation mode
    bool                    _surface_bottom_;        //!< Flag for bottom surface generation mode
    bool                    _surface_top_;           //!< Flag for top surface generation mode
    genvtx::tube_vg         _tube_vg_;               //!< Embeded vertex generator from a tube
    double                  _skin_skip_;             //!< Skip (normal to the surface) to an effective position of the skin relative to the surface of the box
    double                  _skin_thickness_;        //!< Intrinsic thickness of the surface
    std::string             _origin_rules_;          //!< Rules to select the physical volumes from where to generate vertexes
    std::string             _mapping_plugin_name_;   //!< The name of the geometry 'mapping' plugin
    std::string             _materials_plugin_name_; //!< The name of the geometry 'materials' plugin
    geomtools::id_selector  _src_selector_;          //!< A selector of GIDs
    std::vector<weight_entry_type> _entries_;        //!< Information about the weights

    GENVTX_VG_REGISTRATION_INTERFACE(tube_model_vg);

  };

} // end of namespace genvtx

#endif // GENVTX_TUBE_MODEL_VG_H_

// end of tube_model_vg.h

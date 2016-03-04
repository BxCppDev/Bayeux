/// \file genvtx/sphere_model_vg.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-10-10
 * Last modified: 2014-04-09
 *
 * License:
 *
 * Description:
 *
 *   Vertex generator from a sphere model addressed through some
 *   collection of geometry IDs extracted from a mapping object.
 *
 * History:
 *
 */

#ifndef GENVTX_SPHERE_MODEL_VG_H
#define GENVTX_SPHERE_MODEL_VG_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/geomtools
#include <geomtools/id_selector.h>

// This project:
#include <genvtx/sphere_vg.h>
#include <genvtx/vg_macros.h>
#include <genvtx/utils.h>

namespace genvtx {

  /// \brief A vertex generator based on a spherical geometry model
  class sphere_model_vg : public i_vertex_generator
  {
  public:

    bool is_mode_valid () const;

    bool is_mode_bulk () const;

    bool is_mode_surface () const;

    bool is_surface_inner_side () const;

    bool is_surface_outer_side () const;

    bool is_surface_start_phi_side () const;

    bool is_surface_stop_phi_side () const;

    bool is_surface_start_theta_side () const;

    bool is_surface_stop_theta_side () const;

    void set_surface_inner_side (bool);

    void set_surface_outer_side (bool);

    void set_surface_start_phi_side (bool);

    void set_surface_stop_phi_side (bool);

    void set_surface_start_theta_side (bool);

    void set_surface_stop_theta_side (bool);

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

    /// Constructor
    sphere_model_vg();

    /// Destructor
    virtual ~sphere_model_vg();

    /// Initialization
    virtual void initialize(const ::datatools::properties &,
                             ::datatools::service_manager &,
                             ::genvtx::vg_dict_type &);

    /// Reset
    virtual void reset();

    /// Check initialization status
    virtual bool is_initialized() const;

  protected :

    /// Randomize vertex
    virtual void _shoot_vertex(::mygsl::rng & random_, ::geomtools::vector_3d & vertex_);

  private:

    void _init_ ();

    void _reset_ ();

    void _set_defaults_ ();

  protected:

    void _shoot_vertex_spheres(mygsl::rng & random_,
                               geomtools::vector_3d & vertex_);

  private:

    bool                    _initialized_;    //!< Initialization flag
    int                     _mode_;           //!< Mode : "bulk" of "surface"
    bool                    _surface_inner_side_;    //!< Flag for inner side surface generation mode
    bool                    _surface_outer_side_;    //!< Flag for outer side surface generation mode
    bool                    _surface_start_phi_side_;   //!< Flag for start phi side surface generation mode
    bool                    _surface_stop_phi_side_;    //!< Flag for stop phi side surface generation mode
    bool                    _surface_start_theta_side_;   //!< Flag for start theta side surface generation mode
    bool                    _surface_stop_theta_side_;    //!< Flag for stop theta side surface generation mode
    double                  _skin_skip_;      //!< Skip (normal to the surface) to an effective position of the skin relative to the surface of the sphere
    double                  _skin_thickness_; //!< Intrinsic thickness of the surface
    genvtx::sphere_vg       _sphere_vg_;         //!< Embeded vertex generator from a sphere
    std::string             _origin_rules_;   //!< Rules to select the physical volumes from where to generate vertexes
    std::string             _mapping_plugin_name_;   //!< The name of the geometry 'mapping' plugin
    std::string             _materials_plugin_name_; //!< The name of the geometry 'materials' plugin
    geomtools::id_selector  _src_selector_;   //!< A selector of GIDs
    std::vector<weight_entry_type> _entries_; //!< Information about the weights

    /// Registration macro
    /// @arg sphere_model_vg the class to be registered
    GENVTX_VG_REGISTRATION_INTERFACE(sphere_model_vg)

  };

} // end of namespace genvtx

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(genvtx::sphere_model_vg)

#endif // GENVTX_SPHERE_MODEL_VG_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

/// \file genvtx/box_model_vg.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-13
 * Last modified: 2014-04-09
 *
 * License:
 *
 * Description:
 *
 *   Vertex generator from a box model addressed through some
 *   collection of geometry IDs extracted from a mapping object.
 *
 * History:
 *
 */

#ifndef GENVTX_BOX_MODEL_VG_H
#define GENVTX_BOX_MODEL_VG_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/geomtools:
#include <geomtools/id_selector.h>

// This project:
#include <genvtx/box_vg.h>
#include <genvtx/vg_macros.h>
#include <genvtx/utils.h>

namespace genvtx {

  /// \brief A vertex generator based on a boxed geometry model
  class box_model_vg : public i_vertex_generator
  {
  public:

    /// Check the validity of the mode
    bool is_mode_valid() const;

    /// Check if 'bulk' mode is activated
    bool is_mode_bulk() const;

    /// Check if 'surface' mode is activated
    bool is_mode_surface() const;

    /// Check if 'back' surface is activated
    bool is_surface_back() const;

    /// Check if 'front' surface is activated
    bool is_surface_front() const;

    /// Check if 'bottom' surface is activated
    bool is_surface_bottom() const;

    /// Check if 'top' surface is activated
    bool is_surface_top() const;

    /// Check if 'left' surface is activated
    bool is_surface_left() const;

    /// Check if 'right' surface is activated
    bool is_surface_right() const;

    /// Set 'left' surface activation
    void set_surface_left(bool);

    /// Set 'right' surface activation
    void set_surface_right(bool);

    /// Set 'bottom' surface activation
    void set_surface_bottom(bool);

    /// Set 'top' surface activation
    void set_surface_top(bool);

    /// Set 'back' surface activation
    void set_surface_back(bool);

    /// Set 'front' surface activation
    void set_surface_front(bool);

    /// Set the skin skip distance
    void set_skin_skip(double skin_skip_);

    /// Set the skin thickness
    void set_skin_thickness(double skin_thickness_);

    /// Return the origin rules
    const std::string & get_origin_rules() const;

    /// Set the origin rules
    void set_origin_rules(const std::string &);

    /// Check if a mapping plugin name is defined
    bool has_mapping_plugin_name() const;

    /// Return the mapping plugin name
    const std::string & get_mapping_plugin_name() const;

    /// Set the mapping plugin name
    void set_mapping_plugin_name(const std::string & mpn_);

    /// Check if a materials plugin name is defined
    bool has_materials_plugin_name() const;

    /// Return the materials plugin name is defined
    const std::string & get_materials_plugin_name() const;

    /// Set the materials plugin name is defined
    void set_materials_plugin_name(const std::string & mpn_);

    /// Return the mode
    int get_mode() const;

    /// Set the mode
    void set_mode(int);

    /// Smart print
    virtual void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const;

    /// Constructor
    box_model_vg();

    /// Destructor
    virtual ~box_model_vg();

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

    /// Main vertex randomization algorithm
    void _shoot_vertex_boxes(mygsl::rng & random_,
                             geomtools::vector_3d & vertex_);

  private:

    /// Private initialization
    void _init_();

    /// Private reset
    void _reset_();

    /// Set default attibutes' values
    void _set_defaults_();

  private:

    bool                    _initialized_;    //!< Initialization flag
    int                     _mode_;           //!< Mode : "bulk" of "surface"
    bool                    _surface_back_;   //!< Flag for back surface generation mode
    bool                    _surface_front_;  //!< Flag for front surface generation mode
    bool                    _surface_bottom_; //!< Flag for bottom surface generation mode
    bool                    _surface_top_;    //!< Flag for top surface generation mode
    bool                    _surface_left_;   //!< Flag for left surface generation mode
    bool                    _surface_right_;  //!< Flag for right surface generation mode
    double                  _skin_skip_;      //!< Skip (normal to the surface) to an effective position of the skin relative to the surface of the box
    double                  _skin_thickness_; //!< Intrinsic thickness of the surface
    genvtx::box_vg          _box_vg_;         //!< Embeded vertex generator from a box
    std::string             _origin_rules_;   //!< Rules to select the physical volumes from where to generate vertexes
    std::string             _mapping_plugin_name_;   //!< The name of the geometry 'mapping' plugin
    std::string             _materials_plugin_name_; //!< The name of the geometry 'materials' plugin
    geomtools::id_selector  _src_selector_;   //!< A selector of GIDs
    std::vector<weight_entry_type> _entries_; //!< Information about the weights

    /// Registration macro
    /// @arg box_model_vg the class to be registered
    GENVTX_VG_REGISTRATION_INTERFACE(box_model_vg)

  };

} // end of namespace genvtx

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(genvtx::box_model_vg)

#endif // GENVTX_BOX_MODEL_VG_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

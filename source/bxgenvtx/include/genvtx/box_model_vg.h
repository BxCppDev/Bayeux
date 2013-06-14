// -*- mode: c++; -*-
/* box_model_vg.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-13
 * Last modified: 2013-03-10
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

#ifndef GENVTX_BOX_MODEL_VG_H_
#define GENVTX_BOX_MODEL_VG_H_ 1

#include <string>

#include <geomtools/id_selector.h>

#include <genvtx/box_vg.h>
#include <genvtx/vg_macros.h>
#include <genvtx/utils.h>

namespace genvtx {

  GENVTX_VG_CLASS_DECLARE(box_model_vg)
  {
  public:

    bool is_mode_valid () const;

    bool is_mode_bulk () const;

    bool is_mode_surface () const;

    bool is_surface_back () const;

    bool is_surface_front () const;

    bool is_surface_bottom () const;

    bool is_surface_top () const;

    bool is_surface_left () const;

    bool is_surface_right () const;

    void set_surface_left (bool);

    void set_surface_right (bool);

    void set_surface_bottom (bool);

    void set_surface_top (bool);

    void set_surface_back (bool);

    void set_surface_front (bool);

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

    GENVTX_VG_INTERFACE_CTOR_DTOR (box_model_vg);

  private:

    void _init_ ();

    void _reset_ ();

    void _set_defaults_ ();

  protected:

    void _shoot_vertex_boxes (mygsl::rng & random_,
                              geomtools::vector_3d & vertex_);

  private:

    bool                    _initialized_;    //!< Initialization flag
    int                     _mode_;           //!< Mode : "bulk" of "surface"
    bool                    _surface_back_;   //!< Flag for back surface generation mode
    bool                    _surface_front_;  //!< Flag for front surface generation mode
    bool                    _surface_bottom_; //!< Flag for bottom surface generation mode
    bool                    _surface_top_;    //!< Flag for top surface generation mode
    bool                    _surface_left_;   //!< Flag for left surface generation mode
    bool                    _surface_right_;  //!< Flag for right surface generation mode
    genvtx::box_vg          _box_vg_;         //!< Embeded vertex generator from a box
    std::string             _origin_rules_;   //!< Rules to select the physical volumes from where to generate vertexes
    std::string             _mapping_plugin_name_;   //!< The name of the geometry 'mapping' plugin
    std::string             _materials_plugin_name_; //!< The name of the geometry 'materials' plugin
    geomtools::id_selector  _src_selector_;   //!< A selector of GIDs
    std::vector<weight_entry_type> _entries_; //!< Information about the weights

    GENVTX_VG_REGISTRATION_INTERFACE(box_model_vg);

  };

} // end of namespace genvtx

#endif // GENVTX_BOX_MODEL_VG_H_

// end of box_model_vg.h

// -*- mode: c++ ; -*-
/// \file genvtx/detail/vg_tools.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-12
 * Last modified: 2012-05-28
 *
 * License:
 *
 * Description:
 *
 * History:
 *
 */

#ifndef GENVTX_DETAIL_VG_TOOLS_H
#define GENVTX_DETAIL_VG_TOOLS_H 1

// Standard library:
#include <iostream>
#include <string>
#include <map>

// Third party:
// - Bayeux/datatools:
#include <datatools/handle.h>
#include <datatools/bit_mask.h>
#include <datatools/properties.h>

namespace genvtx {

  class i_vertex_generator;

  typedef datatools::handle<i_vertex_generator> vg_handle_type;

  class vg_tools
  {
  public:
    static const std::string SHAPE_REF_NONE;
    static const std::string SHAPE_REF_PLAIN;
    static const std::string SHAPE_REF_GETTER;
  };

  class manager;

  class vg_entry_type
  {
  public:

    enum status_type
      {
        STATUS_BLANK             = 0,
        STATUS_CREATED           = datatools::bit_mask::bit00,
        STATUS_INITIALIZED       = datatools::bit_mask::bit01
      };

  public:

    const datatools::properties & get_vg_config () const;

    datatools::properties & grab_vg_config ();

    void set_vg_config (const datatools::properties &);

    const std::string & get_vg_name () const;

    void set_vg_name (const std::string &);

    const std::string & get_vg_id () const;

    void set_ptr (i_vertex_generator *);

    void set_vg_id (const std::string &);

    bool has_manager () const;

    void set_manager (genvtx::manager &);

    void reset_manager ();

    void set_blank ();

    void set_created ();

    void set_initialized ();

    void set_uninitialized ();

    bool is_created () const;

    bool is_initialized () const;

    /// Constructor
    vg_entry_type ();

    /// Check is the vg is referenced in the embedded handle
    bool has_vg () const;

    const i_vertex_generator & get_vg () const;

    i_vertex_generator & grab_vg ();

    const vg_handle_type & get_vg_handle () const;

    vg_handle_type & grab_vg_handle ();

    vg_handle_type & grab_initialized_vg_handle ();

    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_               = false) const;


  private:


    std::string           _vg_name_;    //!< The name of the vg
    std::string           _vg_id_;      //!< The ID (type) of the vg
    datatools::properties _vg_config_;  //!< The configuration of the vg
    uint32_t              _vg_status_;  //!< The status of the vg
    vg_handle_type        _vg_handle_;  //!< The handle for the allocated vg
    ::genvtx::manager   * _manager_;    //!< Vertex generator manager
  };

  typedef std::map<std::string, vg_entry_type> vg_dict_type;

} // end of namespace genvtx

#endif // GENVTX_DETAIL_VG_TOOLS_H

// -*- mode: c++ ; -*-
/* manager.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-17
 * Last modified: 2013-03-10
 *
 * License:
 *
 * Copyright 2007-2013 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *   Vertex generator factory and manager
 *
 * History:
 *
 */

#ifndef GENVTX_MANAGER_H_
#define GENVTX_MANAGER_H_ 1

#include <string>
#include <map>
#include <vector>

#include <datatools/multi_properties.h>
#include <datatools/i_tree_dump.h>
#include <datatools/logger.h>

#include <geomtools/utils.h>

#include <mygsl/rng.h>
#include <mygsl/random_utils.h>

#include <genvtx/detail/vg_tools.h>
#include <genvtx/i_vertex_generator.h>
#include <genvtx/utils.h>


namespace datatools {
  class service_manager;
}

namespace genvtx {

  /** The main vertex generator manager drives the generation of
   *  random vertexes within a given geometry setup.
   *  The geometry setup is described through a dedicated
   *  external geometry manager, eventually passed through a service
   *  manager.
   *  The manager handles its own pseudo-random number generator.
   *  It hosts a special factory that enables to create
   *  specific random vertex generators on the fly (with possibility
   *  to combine them given special vertex generators).
   *  The main configuration of the manager and the vertex generators
   *  it manages is done through containers of properties.
   *
   */
  class manager :
    public datatools::i_tree_dumpable
  {

  public:

    bool is_initialized () const;

    bool is_debug () const;

    void set_debug (bool);

    bool is_debug_vg () const;

    void set_debug_vg (bool);

    bool is_factory_debug () const;

    void set_factory_debug (bool);

    void set_service_manager (datatools::service_manager &);

    bool has_service_manager () const;

    const datatools::service_manager & get_service_manager () const;

    datatools::service_manager & grab_service_manager ();

    bool has_geometry_manager () const;

    void set_geometry_manager (const geomtools::manager &);

    const geomtools::manager & get_geometry_manager () const;

    bool has_external_random () const;

    void set_external_random (mygsl::rng &);

    bool has_random_seed () const;

    void set_random_seed (int);

    int  get_random_seed () const;

    void set_rng_id (const std::string & rng_id_);

    const std::string & get_rng_id () const;

    const mygsl::rng & get_random () const;

    mygsl::rng & grab_random ();

    bool has_generator_name () const;

    void set_generator_name (const std::string & vg_name_);

    const std::string & get_generator_name () const;

    bool has_generator (const std::string &) const;

    genvtx::vg_handle_type grab_generator (const std::string & vg_name_);

    /// Constructor
    manager (datatools::logger::priority p_ = datatools::logger::PRIO_WARNING,
             bool debug_vg_ = false,
             bool verbose_factory_ = false);

    /// Destructor
    virtual ~manager ();

    /// Reset
    void reset ();

    /// Main initialization method
    void init (const datatools::properties & config_);

    /// Main initialization method
    void initialize (const datatools::properties & config_);

    void load_vg (const std::string & vg_name_,
                  const std::string & vg_id_,
                  const datatools::properties & vg_config_);

    virtual void create_vg (genvtx::vg_entry_type & vg_entry_);

    virtual void initialize_vg (genvtx::vg_entry_type & vg_entry_);

    void load_vgs (const datatools::multi_properties & vgs_config_);

    bool has_current_vg () const;

    void activate_current_vg (const std::string & vg_name_ = "");

    void desactivate_current_vg ();

    /// Check if manager can fire vertex :
    bool can_shoot_vertex () const;

    /// Main public method to randomize a 3D vertex point
    void shoot_vertex (geomtools::vector_3d & vertex_);

    /// Smart print method
    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_               = false) const;

    const genvtx::i_vertex_generator & get (const std::string & vg_name_) const;

    genvtx::i_vertex_generator & grab (const std::string & vg_name_);

    datatools::logger::priority get_logging_priority() const;

    void set_logging_priority(datatools::logger::priority);

    void vg_names(std::vector<std::string> & names_,
                  std::vector<std::string> & descriptions_,
                  std::string & current_) const;

  protected:

    void _load_vg (const std::string & vg_name_,
                   const std::string & vg_id_,
                   const datatools::properties & vg_config_);

    void _load_vgs (const datatools::multi_properties & vgs_config_);

    void _create_vg (genvtx::vg_entry_type & vg_entry_);

    void _initialize_vg (genvtx::vg_entry_type & vg_entry_);

    void _reset_vg (genvtx::vg_entry_type & vg_entry_);

  protected:

    datatools::logger::priority  _logging_priority; //!< Logging priority threshold

  private:

    bool _initialized_; //!< Initialization flag
    bool _debug_vg_;    //!< Vertex generator dedicated debug flag

    genvtx::i_vertex_generator::factory_register_type _factory_register_; //!< The vertex generator factory register
    bool _factory_preload_;         //!< Factory dictionary preload flag
    bool _factory_debug_;           //!< Factory debug flag
    int         _random_seed_;      //!< The seed of the embeded pseudo random number generator (PRNG)
    std::string _rng_id_;           //!< The ID of the embeded pseudo random number generator (PRNG)
    mygsl::rng  _random_;           //!< The embeded pseudo random number generator (PRNG)
    mygsl::rng * _external_random_; //!< The external pseudo random number generator (PRNG)

    datatools::service_manager * _service_manager_;  //!< The service manager
    const geomtools::manager   * _geometry_manager_; //!< The geometry manager
    genvtx::vg_dict_type         _vg_store_;         //!< Collection of handles to vertex generators
    std::string                  _current_vg_name_;  //!< The name of the current vertex generator
    genvtx::vg_handle_type       _current_vg_;       //!< Handle to the current vertex generator

  };

} // end of namespace genvtx


/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(genvtx::manager)

#endif // GENVTX_MANAGER_H_

// end of manager.h

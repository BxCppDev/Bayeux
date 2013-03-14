// -*- mode: c++ ; -*- 
/* i_vertex_generator.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-12
 * Last modified: 2013-03-10
 * 
 * License: 
 * 
 * Description: 
 * 
 * History: 
 * 
 */

#ifndef GENVTX_I_VERTEX_GENERATOR_H_
#define GENVTX_I_VERTEX_GENERATOR_H_ 1

#include <string>
#include <iostream>

#include <boost/scoped_ptr.hpp>

#include <geomtools/utils.h>
#include <geomtools/detail/model_tools.h>
#include <datatools/factory_macros.h>
#include <datatools/handle.h>
#include <datatools/i_tree_dump.h>

#include <mygsl/rng.h>

#include <genvtx/detail/vg_tools.h>
#include <genvtx/utils.h>

namespace datatools {
  class properties;
  class service_manager;
}

namespace geomtools {
  class manager;
}

namespace genvtx {
  
  class i_vertex_generator :
    public datatools::i_tree_dumpable
  {
  
  public: 

    /// Check the debug flag
    bool is_debug () const;

    /// Set the debug flag
    void set_debug (bool debug_);

    bool has_geo_label () const;
    
    void set_geo_label (const std::string & geo_label_);
    
    const std::string & get_geo_label () const;
    
    bool has_geom_setup_requirement () const;
    
    void set_geom_setup_requirement (const std::string & geom_setup_requirement_);
    
    std::string get_geom_setup_requirement () const;
    
    void check_geom_setup_requirement (const geomtools::manager * gmgr_) const;
    

    /// Check if a geometry manager is attached to the vertex generator
    bool has_geom_manager () const;

    /// Set a geometry manager
    void set_geom_manager (const geomtools::manager & gmgr_);
    
    /// Get the geometry manager attached to the vertex generator
    const geomtools::manager & get_geom_manager () const;

    /// Check if some weight info is attached to the vertex generator
    bool has_total_weight () const;

    /// Get the weight info attached to the vertex generator
    const weight_info & get_total_weight () const;

    /// Check if another vertex is available
    virtual bool has_next_vertex () const;

    /// Wrapper method for vertex randomization
    void shoot_vertex (mygsl::rng & random_, geomtools::vector_3d & vertex_);

    /// Wrapper method for vertex randomization
    geomtools::vector_3d shoot_vertex (mygsl::rng & random_);
     
    /// Simple initialization (no external resource)
    virtual void initialize_simple ();

    /// Initialization from a container of properties
    virtual void initialize_standalone (const datatools::properties & setup_);

    /// Initialization from a container of properties and a service manager
    virtual void initialize_with_service_only (const datatools::properties & setup_,
                                               datatools::service_manager & service_manager_);

    /// Initialization from a container of properties, a service manager and a dictionnary of vertex generators
    virtual void initialize_with_dictionary_only (const datatools::properties & setup_,
                                                  vg_dict_type & dictionary_);
 
    /// Constructor
    i_vertex_generator ();

    /// Destructor
    virtual ~i_vertex_generator ();

  public: 

    /*** Main interface ***/

    /// Check initialization status
    virtual bool is_initialized () const = 0;

    /// Main initialization interface method
    virtual void initialize (const datatools::properties & setup_,
                             datatools::service_manager & service_manager_,
                             vg_dict_type & dictionary_) = 0;

    /// Reset method
    virtual void reset () = 0;

    /// Smart print
    virtual void tree_dump (std::ostream & out_         = std::clog, 
                            const std::string & title_  = "", 
                            const std::string & indent_ = "", 
                            bool inherit_               = false) const;
 
  protected:

    void _reset();

    weight_info & _grab_total_weight ();
    
    void _set_total_weight (const weight_info & a_info);

    void _initialize_basics (const datatools::properties & setup_,
                             datatools::service_manager & service_manager_);
    
    void _initialize_geo_manager (const datatools::properties & setup_,
                                  datatools::service_manager & service_manager_);

    /// Guard method
    void _assert_lock (const std::string & where_, const std::string & what_ = "") const;
    
    /// Main vertex randomization interface method
    virtual void _shoot_vertex (mygsl::rng & random_, geomtools::vector_3d & vertex_) = 0;

  private:

    bool                         _debug_; /// Debug flag
    std::string                  _geo_label_;    /// The label of the Geometry Service
    std::string                  _geom_setup_requirement_; /// The requirement ont the requested geometry setup (label and version)
    const ::geomtools::manager * _geom_manager_; /// Handle to the geometry manager
    weight_info                  _total_weight_; /// Weighting data

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(i_vertex_generator);

  };

} // end of namespace genvtx

#include <genvtx/vg_macros.h>

#endif // GENVTX_I_VERTEX_GENERATOR_H_

// end of i_vertex_generator.h

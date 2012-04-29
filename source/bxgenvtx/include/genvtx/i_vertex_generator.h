// -*- mode: c++ ; -*- 
/* i_vertex_generator.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-12
 * Last modified: 2010-02-12
 * 
 * License: 
 * 
 * Description: 
 * 
 * History: 
 * 
 */

#ifndef __genvtx__i_vertex_generator_h
#define __genvtx__i_vertex_generator_h 1

#include <string>
#include <iostream>

#include <boost/scoped_ptr.hpp>

#include <geomtools/utils.h>
#include <geomtools/detail/model_tools.h>
#include <datatools/factory/factory_macros.h>
#include <datatools/utils/handle.h>
#include <datatools/utils/i_tree_dump.h>

#include <mygsl/rng.h>

#include <genvtx/detail/vg_tools.h>

namespace datatools {
  namespace utils {
    class properties;
  }}

namespace datatools {
  namespace service {
    class service_manager;
  }}

namespace genvtx {

  class i_vertex_generator :
    public datatools::utils::i_tree_dumpable
  {
  
  public: 

    /// Check the debug flag
    bool is_debug () const;

    /// Set the debug flag
    void set_debug (bool debug_);

    /// Check if another vertex is available
    virtual bool has_next_vertex () const;

    /// Wrapper method for vertex randomization
    void shoot_vertex (mygsl::rng & random_, geomtools::vector_3d & vertex_);

    /// Wrapper method for vertex randomization
    geomtools::vector_3d shoot_vertex (mygsl::rng & random_);
     
    /// Simple initialization (no external resource)
    virtual void initialize_simple ();

    /// Initialization from a container of properties
    virtual void initialize_standalone (const datatools::utils::properties & setup_);

    /// Initialization from a container of properties and a service manager
    virtual void initialize_with_service_only (const datatools::utils::properties & setup_,
                                               datatools::service::service_manager & service_manager_);

    /// Initialization from a container of properties, a service manager and a dictionnary of vertex generators
    virtual void initialize_with_dictionary_only (const datatools::utils::properties & setup_,
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
    virtual void initialize (const datatools::utils::properties & setup_,
                             datatools::service::service_manager & service_manager_,
                             vg_dict_type & dictionary_) = 0;

    /// Reset method
    virtual void reset () = 0;

    /// Smart print
    virtual void tree_dump (std::ostream & out_         = std::clog, 
                            const std::string & title_  = "", 
                            const std::string & indent_ = "", 
                            bool inherit_               = false) const;
 
  protected:

    /// Main vertex randomization interface method
    virtual void _shoot_vertex (mygsl::rng & random_, geomtools::vector_3d & vertex_) = 0;

    /// Guard method
    void _assert_lock (const std::string & where_, const std::string & what_ = "") const;

  private:

    bool _debug_; /// Debug flag

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(i_vertex_generator);

  };

} // end of namespace genvtx

#include <genvtx/vg_macros.h>

#endif // __genvtx__i_vertex_generator_h

// end of i_vertex_generator.h

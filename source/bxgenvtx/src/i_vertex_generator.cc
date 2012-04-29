// -*- mode: c++ ; -*- 
/* i_vertex_generator.cc
 */

#include <genvtx/i_vertex_generator.h>

#include <stdexcept>
#include <sstream>

#include <datatools/utils/properties.h>
#include <datatools/services/service_manager.h>

namespace genvtx {

  using namespace std;

  // Factory stuff :
  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(i_vertex_generator,"genvtx::i_vertex_generator/__system__");


  bool i_vertex_generator::is_debug () const
  {
    return _debug_;
  }

  void i_vertex_generator::set_debug (bool debug_)
  {
    _debug_ = debug_;
    return;
  }

  void i_vertex_generator::_assert_lock (const std::string & where_, 
                                         const std::string & what_) const
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        if (where_.empty ())
          {
            message << "genvtx::box_vg::_assert_lock: ";
          }
        else
          {
            message << where_ << ": ";
          }
        if (what_.empty ())
          {
            message << "Object is initialized/locked !";
          }
        else 
          {
            message << what_;
          }
        throw logic_error (message.str());
     }
    return;
  }

  i_vertex_generator::i_vertex_generator ()
  {
    _debug_ = false;
    return;
  }

  i_vertex_generator::~i_vertex_generator ()
  {
    return;
  }

  bool i_vertex_generator::has_next_vertex () const
  {
    return true;
  }

  void i_vertex_generator::shoot_vertex (mygsl::rng & random_, 
                                         geomtools::vector_3d & vertex_)
  {
    _shoot_vertex (random_, vertex_);
    return;
  }

  geomtools::vector_3d i_vertex_generator::shoot_vertex (mygsl::rng & random_)
  {
    geomtools::vector_3d vertex;
    shoot_vertex (random_, vertex);
    return vertex;
  }

  void i_vertex_generator::initialize_simple ()
  {
    if (is_debug ()) std::cerr << "DEBUG: genvtx::i_vertex_generator::initialize_simple: Entering..." << std::endl;
    datatools::utils::properties dummy_setup;
    initialize_standalone (dummy_setup);
    return;
  }

  void i_vertex_generator::initialize_standalone (const datatools::utils::properties & setup_)
  {
    if (is_debug ()) std::cerr << "DEBUG: genvtx::i_vertex_generator::initialize_standalone: Entering..." << std::endl;
    datatools::service::service_manager dummy_srvcmgr;
    vg_dict_type dummy_dict;
    initialize (setup_, dummy_srvcmgr, dummy_dict);
    return;
  }
   
  void i_vertex_generator::initialize_with_dictionary_only (const datatools::utils::properties & setup_,
                                                            vg_dict_type & dictionary_)
  {
    if (is_debug ()) std::cerr << "DEBUG: genvtx::i_vertex_generator::initialize_with_dictionary_only: Entering..." << std::endl;
    datatools::service::service_manager dummy_srvcmgr;
    initialize (setup_, dummy_srvcmgr, dictionary_);
    return;
  }

  void i_vertex_generator::initialize_with_service_only (const datatools::utils::properties & setup_,
                                                         datatools::service::service_manager & service_manager_)
  {
    if (is_debug ()) std::cerr << "DEBUG: genvtx::i_vertex_generator::initialize_with_service_only: Entering..." << std::endl;
    vg_dict_type dummy_dict;
    initialize (setup_, service_manager_, dummy_dict);
    return;
  }

  void i_vertex_generator::tree_dump (std::ostream & out_, 
                                      const std::string & title_, 
                                      const std::string & indent_, 
                                      bool inherit_) const
  {
    namespace du = datatools::utils;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ()) 
      {
        out_ << indent << title_ << endl;
      }

    {
      out_ << indent << du::i_tree_dumpable::tag  
           << "Initialized : " << (is_initialized() ? "Yes": "No") << endl;
    }

    {
      out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)  
           << "Debug : " << (is_debug() ? "Yes": "No") << endl;
      
    }

    return;
  }
 
} // end of namespace genvtx

// end of i_vertex_generator.cc

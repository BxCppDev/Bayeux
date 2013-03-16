// -*- mode: c++ ; -*- 
/* i_vertex_generator.cc
 */

#include <genvtx/i_vertex_generator.h>

#include <string>
#include <stdexcept>
#include <sstream>

#include <datatools/properties.h>
#include <datatools/service_manager.h>
#include <datatools/version_id.h>
#include <datatools/version_check.h>
#include <datatools/utils.h>

#include <mygsl/rng.h>

#include <geomtools/manager.h>
#include <geomtools/geometry_service.h>

#include <genvtx/utils.h>

namespace genvtx {

  using namespace std;

  // Factory stuff :
  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(i_vertex_generator,"genvtx::i_vertex_generator/_system_");

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

  bool i_vertex_generator::has_geom_setup_requirement () const
  {
    return ! _geom_setup_requirement_.empty ();
  }
    
  void i_vertex_generator::set_geom_setup_requirement (const std::string & geom_setup_requirement_)
  {
    _assert_lock ("snemo::genvertex::i_vertex_generator::set_geom_setup_requirement");
    _geom_setup_requirement_ = geom_setup_requirement_;
    return;
  }
    
  std::string i_vertex_generator::get_geom_setup_requirement () const
  {
    return _geom_setup_requirement_;
  }
  
  bool i_vertex_generator::has_geo_label () const
  {
    return ! _geo_label_.empty ();
  }

  void i_vertex_generator::set_geo_label (const std::string & geo_label_)
  {
    _geo_label_ = geo_label_;
    return;
  }

  const std::string & i_vertex_generator::get_geo_label () const
  {
    return _geo_label_;
  }

  void i_vertex_generator::_set_total_weight (const weight_info & a_info)
  {
    _total_weight_ = a_info;
    return;
  }

  bool i_vertex_generator::has_total_weight () const
  {
    return _total_weight_.is_valid ();
  }

  const weight_info & i_vertex_generator::get_total_weight () const
  {
    return _total_weight_;
  }

  weight_info & i_vertex_generator::_grab_total_weight ()
  {
    return _total_weight_;
  }
 
  bool i_vertex_generator::has_geom_manager () const
  {
    return _geom_manager_ != 0;
  }
 
  void i_vertex_generator::check_geom_setup_requirement (const geomtools::manager * gmgr_) const
  {
    // Default check the embedded geometry manager if any :
    const geomtools::manager * gmgr = _geom_manager_;
    if (gmgr_ != 0)
      {
        // Or check the argument geometry manager:
        gmgr = gmgr_;
      }
    if (gmgr != 0 && has_geom_setup_requirement ())
      {
        // Example : "snemo"
        const string & geom_mgr_setup_label = gmgr->get_setup_label ();
        // Example : "2.0"
        const string & geom_mgr_setup_version = gmgr->get_setup_version ();
        datatools::version_id geom_mgr_setup_vid;
        {
          std::istringstream iss (geom_mgr_setup_version);
          iss >> geom_mgr_setup_vid;
        }
        if (! datatools::validate_version (geom_mgr_setup_label, 
                                           geom_mgr_setup_vid, 
                                           get_geom_setup_requirement ()))
          {
            std::ostringstream message;
            message << "genvtx::i_vertex_generator::check_geom_setup_requirement: "
                    << "Geometry manager setup label '" << gmgr->get_setup_label ()  << "' with version '" << geom_mgr_setup_vid << "' does not match the requested setup requirement '" << get_geom_setup_requirement () << "' !";
            throw std::logic_error (message.str ());
          }
        else
          {
            std::clog << "NOTICE: " << "genvtx::i_vertex_generator::check_geom_setup_requirement: "  
                      << "Geometry manager setup label '" << gmgr->get_setup_label ()  
                      << "' with version '" << geom_mgr_setup_vid 
                      << "' matches the requested setup requirement '" 
                      << get_geom_setup_requirement () << "'."
                      << std::endl;
          }
      }      
    return;
  }

  void i_vertex_generator::set_geom_manager (const geomtools::manager & gmgr_)
  {
    _assert_lock ("genvtx::i_vertex_generator::set_manager");
    check_geom_setup_requirement (&gmgr_);
    _geom_manager_ = &gmgr_;
    return;
  }
  
  const geomtools::manager & i_vertex_generator::get_geom_manager () const
  {
    return *_geom_manager_;
  }

  bool i_vertex_generator::has_external_prng() const
  {
    return _external_prng_ != 0;
  }

  void i_vertex_generator::set_external_prng(mygsl::rng & prng_)
  {
    _external_prng_ = &prng_;
    return;
  }

  bool i_vertex_generator::has_prng() const
  {
    return has_external_prng();
  }

  mygsl::rng & i_vertex_generator::grab_prng()
  {
    if (! has_external_prng())
      {
        throw std::logic_error ("mygsl::i_vertex_generator::shoot_vertex: No available PRNG !");
     }
    return *_external_prng_;
  }

  i_vertex_generator::i_vertex_generator ()
  {
    _debug_ = false;
    _geom_manager_ = 0;
    _total_weight_.invalidate ();
    _external_prng_ = 0;
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

  void i_vertex_generator::shoot_vertex (geomtools::vector_3d & vertex_)
  {
    if (_external_prng_ == 0)
      {
        throw std::logic_error ("mygsl::i_vertex_generator::shoot_vertex: Missing external PRNG handle !");
      }
    shoot_vertex (*_external_prng_, vertex_);
    return;
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

  void i_vertex_generator::_initialize_basics (const datatools::properties & setup_,
                                               datatools::service_manager & service_manager_)
  {
    // Fetch configuration parameters :
    if (setup_.has_flag ("debug"))
      {
        set_debug (true);
      }

    // Required geometry setup label :
    if (setup_.has_key ("geometry.setup_requirement"))
      {
        std::clog << "NOTICE: " << "genvtx::i_vertex_generator::_initialize_basics: "
                  << "Loading 'geometry.setup_requirement' rules..." << std::endl;
        string geom_setup_requirement = setup_.fetch_string ("geometry.setup_requirement");
        set_geom_setup_requirement (geom_setup_requirement);
      }

    // Do we need support for embeded PRNG ???

    check_geom_setup_requirement (0);
      
    return;
  }
  
  void i_vertex_generator::_initialize_geo_manager (const datatools::properties & setup_,
                                                    datatools::service_manager & service_manager_)
  {
    // Geometry manager :
    if (_geom_manager_ == 0)
      {
        // Only if geometry manager is not already set :
        if (_geo_label_.empty ())
          {
            // Service labels :
            if (setup_.has_key ("Geo_label"))
              {
                _geo_label_ = setup_.fetch_string ("Geo_label");
              }
            else
              { 
                std::ostringstream message;
                message << "genvtx::i_vertex_generator::_initialize_geo_manager: "
                        << "Missing  '" << "Geo_label" << "' property !";
                throw std::logic_error (message.str ());
              }
          }

        if (_geo_label_.empty ())
          {
            std::ostringstream message;
            message << "genvtx::i_vertex_generator::_initialize_geo_manager: "
                    << "Invalid '" << "Geo_label" << "' property !";
            throw std::logic_error (message.str ());
          }

        if (service_manager_.has (_geo_label_)
            && service_manager_.is_a<geomtools::geometry_service> (_geo_label_))
          {
            geomtools::geometry_service & Geo
              = service_manager_.get<geomtools::geometry_service> (_geo_label_);
            set_geom_manager (Geo.get_geom_manager ());
          }
        else
          {
            std::ostringstream message;
            message << "genvtx::i_vertex_generator::_initialize_geo_manager: "
                    << "Cannot find '" << _geo_label_ << "' service !";
            throw std::logic_error (message.str ());
          }
      }
        
    return;
  }

  void i_vertex_generator::initialize_simple ()
  {
    if (is_debug ()) std::cerr << "DEBUG: genvtx::i_vertex_generator::initialize_simple: Entering..." << std::endl;
    datatools::properties dummy_setup;
    initialize_standalone (dummy_setup);
    return;
  }

  void i_vertex_generator::initialize_standalone (const datatools::properties & setup_)
  {
    if (is_debug ()) std::cerr << "DEBUG: genvtx::i_vertex_generator::initialize_standalone: Entering..." << std::endl;
    datatools::service_manager dummy_srvcmgr;
    vg_dict_type dummy_dict;
    initialize (setup_, dummy_srvcmgr, dummy_dict);
    return;
  }
   
  void i_vertex_generator::initialize_with_dictionary_only (const datatools::properties & setup_,
                                                            vg_dict_type & dictionary_)
  {
    if (is_debug ()) std::cerr << "DEBUG: genvtx::i_vertex_generator::initialize_with_dictionary_only: Entering..." << std::endl;
    datatools::service_manager dummy_srvcmgr;
    initialize (setup_, dummy_srvcmgr, dictionary_);
    return;
  }

  void i_vertex_generator::initialize_with_service_only (const datatools::properties & setup_,
                                                         datatools::service_manager & service_manager_)
  {
    if (is_debug ()) std::cerr << "DEBUG: genvtx::i_vertex_generator::initialize_with_service_only: Entering..." << std::endl;
    vg_dict_type dummy_dict;
    initialize (setup_, service_manager_, dummy_dict);
    return;
  }
  
  
  void i_vertex_generator::_reset ()
  {
    _geo_label_.clear ();
    _geom_setup_requirement_.clear ();
    _geom_manager_ = 0;
    _total_weight_.invalidate ();
    return;
  }

  void i_vertex_generator::tree_dump (std::ostream & out_, 
                                      const std::string & title_, 
                                      const std::string & indent_, 
                                      bool inherit_) const
  {
    namespace du = datatools;
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ()) 
      {
        out_ << indent << title_ << endl;
      }

    out_ << indent << du::i_tree_dumpable::tag  
         << "Initialized : " << (is_initialized() ? "Yes": "No") << endl;

    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)  
         << "Debug : " << (is_debug() ? "Yes": "No") << endl;

    out_ << indent << du::i_tree_dumpable::tag
         << "Geo label        : '" << _geo_label_ << "'" << std::endl;

    out_ << indent << du::i_tree_dumpable::tag
         << "Geometry setup requirement : '" << _geom_setup_requirement_ << "'" << std::endl;

    out_ << indent << du::i_tree_dumpable::tag
         << "Geometry manager : " << _geom_manager_ << std::endl;

    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)
         << "Weight info " << std::endl;
 
    return;
  }
 
} // end of namespace genvtx

// end of i_vertex_generator.cc

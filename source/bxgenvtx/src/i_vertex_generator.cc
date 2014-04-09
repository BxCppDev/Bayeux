/* i_vertex_generator.cc
 */

// Ourselves:
#include <genvtx/i_vertex_generator.h>

// Standard library:
#include <string>
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools
#include <datatools/properties.h>
#include <datatools/service_manager.h>
#include <datatools/version_id.h>
#include <datatools/version_check.h>
#include <datatools/utils.h>
#include <datatools/logger.h>
#include <datatools/exception.h>
// - Bayeux/mygsl
#include <mygsl/rng.h>
// - Bayeux/geomtools
#include <geomtools/manager.h>
#include <geomtools/geometry_service.h>

// This project:
#include <genvtx/utils.h>

namespace genvtx {

  using namespace std;

  // Factory stuff :
  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(i_vertex_generator,"genvtx::i_vertex_generator/_system_");

  const std::string & i_vertex_generator::get_name() const
  {
    return _name_;
  }

  bool i_vertex_generator::has_name() const
  {
    return ! _name_.empty();
  }

  void i_vertex_generator::set_name(const std::string & name_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Vertex generator '" << get_name() << "' is initialized/locked !");
    _name_ = name_;
  }

  datatools::logger::priority
  i_vertex_generator::get_logging_priority() const
  {
    return _logging_priority;
  }

  void i_vertex_generator::set_logging_priority(datatools::logger::priority p_)
  {
    _logging_priority = p_;
  }


  bool i_vertex_generator::is_debug () const
  {
    return _logging_priority >= datatools::logger::PRIO_DEBUG;
  }

  void i_vertex_generator::set_debug (bool debug_)
  {
    if (debug_) {
      _logging_priority = datatools::logger::PRIO_DEBUG;
    } else {
      _logging_priority = datatools::logger::PRIO_WARNING;
    }
    return;
  }

  bool i_vertex_generator::has_geom_setup_requirement () const
  {
    return ! _geom_setup_requirement_.empty ();
  }

  void i_vertex_generator::set_geom_setup_requirement (const std::string & geom_setup_requirement_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Vertex generator '" << get_name() << "' is initialized/locked !");
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
    if (gmgr_ != 0) {
        // Or check the argument geometry manager:
        gmgr = gmgr_;
      }
    if (gmgr != 0 && has_geom_setup_requirement ()) {
        // Example : "snemo"
        const string & geom_mgr_setup_label = gmgr->get_setup_label ();
        // Example : "2.0"
        const string & geom_mgr_setup_version = gmgr->get_setup_version ();
        datatools::version_id geom_mgr_setup_vid;
        {
          std::istringstream iss (geom_mgr_setup_version);
          iss >> geom_mgr_setup_vid;
        }
        DT_THROW_IF ((! datatools::validate_version (geom_mgr_setup_label,
                                                     geom_mgr_setup_vid,
                                                     get_geom_setup_requirement ())),
                     std::logic_error,
                     "Geometry manager setup label '" << gmgr->get_setup_label ()
                     << "' with version '" << geom_mgr_setup_vid
                     << "' does not match the requested setup requirement '"
                     << get_geom_setup_requirement () << "' !");
        DT_LOG_NOTICE(get_logging_priority(),
                      "Geometry manager setup label '" << gmgr->get_setup_label ()
                      << "' with version '" << geom_mgr_setup_vid
                      << "' matches the requested setup requirement '"
                      << get_geom_setup_requirement () << "'.");
      }
    return;
  }

  void i_vertex_generator::set_geom_manager (const geomtools::manager & gmgr_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Vertex generator '" << get_name() << "' is initialized/locked !");
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
    DT_THROW_IF (! has_external_prng(), std::logic_error,
                 "No available PRNG in vertex generator '" << get_name() << "' !");
    return *_external_prng_;
  }

  i_vertex_generator::i_vertex_generator ()
  {
    _logging_priority = datatools::logger::PRIO_WARNING;
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
    DT_THROW_IF (_external_prng_ == 0, std::logic_error,
                 "Missing external PRNG handle in vertex generator '" << get_name() << "' !");
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
                                               datatools::service_manager & /*service_manager_*/)
  {
    // Fetch configuration parameters :

    // Logging priority:
    datatools::logger::priority lp
      = datatools::logger::extract_logging_configuration(setup_,
                                                         datatools::logger::PRIO_WARNING);
    DT_THROW_IF(lp ==  datatools::logger::PRIO_UNDEFINED,
                std::logic_error,
                "Invalid logging priority !");
    set_logging_priority(lp);

    // Name of the generator (only if not already set) :
    if (_name_.empty()) {
      if (setup_.has_key ("name")) {
        set_name(setup_.fetch_string("name"));
      }
    }

    // Required geometry setup label :
    if (setup_.has_key ("geometry.setup_requirement")) {
      DT_LOG_NOTICE(get_logging_priority(),
                    "Loading 'geometry.setup_requirement' rules...");
      std::string geom_setup_requirement = setup_.fetch_string ("geometry.setup_requirement");
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
    if (_geom_manager_ == 0) {
        // Only if geometry manager is not already set :
        if (_geo_label_.empty ()) {
            // Service labels :
            DT_THROW_IF (!setup_.has_key ("Geo_label"),
                         std::logic_error,
                         "Missing  '" << "Geo_label" << "' property in vertex generator '" << get_name() << "' !");
            _geo_label_ = setup_.fetch_string ("Geo_label");
          }

        DT_THROW_IF (_geo_label_.empty (),
                     std::logic_error,
                     "Invalid '" << "Geo_label" << "' property in vertex generator '" << get_name() << "' !");
        DT_THROW_IF (! service_manager_.has (_geo_label_)
                     && service_manager_.is_a<geomtools::geometry_service> (_geo_label_),
                     std::logic_error,
                     "Cannot find '" << _geo_label_ << "' service for vertex generator '" << get_name() << "' !");
        geomtools::geometry_service & Geo
          = service_manager_.get<geomtools::geometry_service> (_geo_label_);
        set_geom_manager (Geo.get_geom_manager ());
      }

    return;
  }

  void i_vertex_generator::initialize_simple ()
  {
    datatools::properties dummy_setup;
    initialize_standalone (dummy_setup);
    return;
  }

  void i_vertex_generator::initialize_standalone (const datatools::properties & setup_)
  {
    datatools::service_manager dummy_srvcmgr;
    vg_dict_type dummy_dict;
    initialize (setup_, dummy_srvcmgr, dummy_dict);
    return;
  }

  void i_vertex_generator::initialize_with_dictionary_only (const datatools::properties & setup_,
                                                            vg_dict_type & dictionary_)
  {
    datatools::service_manager dummy_srvcmgr;
    initialize (setup_, dummy_srvcmgr, dictionary_);
    return;
  }

  void i_vertex_generator::initialize_with_service_only (const datatools::properties & setup_,
                                                         datatools::service_manager & service_manager_)
  {
    vg_dict_type dummy_dict;
    initialize (setup_, service_manager_, dummy_dict);
    return;
  }


  void i_vertex_generator::_reset ()
  {
    _name_.clear ();
    _geo_label_.clear ();
    _geom_setup_requirement_.clear ();
    _geom_manager_ = 0;
    _total_weight_.invalidate ();
    _logging_priority = datatools::logger::PRIO_WARNING;
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
    if (! title_.empty ()) {
      out_ << indent << title_ << endl;
    }

    out_ << indent << du::i_tree_dumpable::tag
         << "Name        : '" << _name_ << "'" << std::endl;

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

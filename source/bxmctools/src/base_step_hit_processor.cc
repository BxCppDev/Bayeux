/** mctools/base_step_hit_processor.cc */

// Ourselves:
#include <mctools/base_step_hit_processor.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/ioutils.h>
#include <datatools/service_manager.h>
#include <datatools/exception.h>
// - Bayeux/geomtools:
#include <geomtools/geometry_service.h>

// This project:
#include <mctools/utils.h>
#include <mctools/step_hit_processor_factory.h>

namespace mctools {

  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(base_step_hit_processor,
                                                   "mctools::base_step_hit_processor/__system__")

  // static
  const size_t base_step_hit_processor::DEFAULT_POOL_CAPACITY;

  datatools::logger::priority base_step_hit_processor::get_logging_priority() const
  {
    return _logging_priority;
  }

  void base_step_hit_processor::set_logging_priority(datatools::logger::priority p)
  {
    _logging_priority = p;
    return;
  }

  bool base_step_hit_processor::is_debug() const
  {
    return _logging_priority >= datatools::logger::PRIO_DEBUG;
  }

  void base_step_hit_processor::set_debug(bool d_)
  {
    if (d_) {
      _logging_priority = datatools::logger::PRIO_DEBUG;
    } else {
      _logging_priority = datatools::logger::PRIO_FATAL;
    }
    return;
  }

  const std::string & base_step_hit_processor::get_name() const
  {
    return _name;
  }

  void base_step_hit_processor::set_name(const std::string & n_)
  {
    _name = n_;
    return;
  }

  const datatools::properties & base_step_hit_processor::get_auxiliaries() const
  {
    return _auxiliaries;
  }

  datatools::properties & base_step_hit_processor::grab_auxiliaries()
  {
    return _auxiliaries;
  }

  bool base_step_hit_processor::accept_external_rng() const
  {
    return false;
  }

  void base_step_hit_processor::set_external_rng(mygsl::rng & /*rng_*/)
  {
    DT_LOG_WARNING(_logging_priority,
                   "Processor '" << get_name() << "' does not use any external PRNG. "
                   << "Ignoring this request !");
    return;
  }

  void base_step_hit_processor::setup_private_pool(size_t capacity_)
  {
    if (_private_pool != 0) {
      DT_THROW_IF (_private_pool->get_number_of_used_item() > 0,
                   std::logic_error,
                   "Cannot resize private pool for processor '" << get_name() << "' !");
      if (capacity_ < _private_pool->get_capacity()) {
        return;
      } else {
        _private_pool->resize(capacity_);
      }
    }
    else {
      DT_LOG_NOTICE(_logging_priority,
                     "Setup a private pool of hits for processor '" << get_name()
                     << "' with capacity = " << capacity_);
      _private_pool = new pool_type(capacity_);
      _pool_owner = false;
    }
    _pool = _private_pool;
    return;
  }

  void base_step_hit_processor::set_pool(pool_type & a_pool)
  {
    _pool = &a_pool;
    return;
  }

  base_step_hit_processor::pool_type & base_step_hit_processor::get_pool() const
  {
    DT_THROW_IF (_pool == 0,
                 std::logic_error,
                 "Processor '" << get_name() << "' does not use any pool of hits !");
    return *_pool;
  }

  bool base_step_hit_processor::has_pool() const
  {
    return _pool != 0;
  }

  void base_step_hit_processor::add_new_hit(simulated_data::hit_handle_collection_type & the_hits)
  {
    if (has_pool()) {
      the_hits.push_back(_pool->create());
      /*
//////////// 2013-05-13 FM : remove this test for now, may be unuseful...
// There is 2 references here:
// - one from the pool itself
// - the other fresh one from this collection of hits
if (the_hits.back().count() > 2)
{
ostringstream message;
message << "mctools::base_step_hit_processor::add_new_hit: "
<< "This hit is already referenced by another handle !";
th row logic_error(message.str());
}
      */
    } else {
      the_hits.push_back(simulated_data::hit_handle_type(new base_step_hit()));
    }
    return;
  }

  bool base_step_hit_processor::using_private_pool() const
  {
    return _private_pool != 0 && _private_pool == _pool;
  }

  base_step_hit_processor::base_step_hit_processor()
  {
    _logging_priority = datatools::logger::PRIO_ERROR;
    _geom_manager = 0;
    _private_pool = 0;
    _pool_owner = false;
    _pool = 0;
    return;
  }

  base_step_hit_processor::~base_step_hit_processor()
  {
    // std::cerr << "DEVEL: " << "base_step_hit_processor::DTOR: Entering..." << std::endl;
    if (_private_pool != 0) {
      delete _private_pool;
    }
    // std::cerr << "DEVEL: " << "base_step_hit_processor::DTOR: Exiting." << std::endl;
    return;
  }

  const std::string & base_step_hit_processor::get_hit_category() const
  {
    return _hit_category;
  }

  void base_step_hit_processor::set_hit_category(const std::string & hc_)
  {
    _hit_category = hc_;
    return;
  }

  const std::string & base_step_hit_processor::get_sensitive_category() const
  {
    return _sensitive_category;
  }

  void base_step_hit_processor::set_sensitive_category(const std::string & sc_)
  {
    _sensitive_category = sc_;
    return;
  }

  bool base_step_hit_processor::has_geom_manager() const
  {
    return _geom_manager != 0;
  }

  const geomtools::manager &
  base_step_hit_processor::get_geom_manager() const
  {
    return *_geom_manager;
  }

  void base_step_hit_processor::set_geom_manager(const geomtools::manager & gmgr_)
  {
    _geom_manager = &gmgr_;
    return;
  }

  void base_step_hit_processor::initialize(const datatools::properties & config_)
  {
    datatools::service_manager dummy_service_mgr;
    initialize(config_,dummy_service_mgr);
    return;
  }

  void base_step_hit_processor::initialize(const datatools::properties & config_,
                                           datatools::service_manager & service_mgr_)
  {
    _initialize(config_, service_mgr_);
    return;
  }

  void base_step_hit_processor::_initialize(const datatools::properties & config_,
                                            datatools::service_manager & service_mgr_)
  {

    // Logging priority:
    datatools::logger::priority lp
      = datatools::logger::extract_logging_configuration(config_,
                                                         datatools::logger::PRIO_ERROR);
    DT_THROW_IF(lp ==  datatools::logger::PRIO_UNDEFINED,
                std::logic_error,
                "Invalid logging priority !");
    set_logging_priority(lp);

    if (! has_geom_manager())  {
      if (config_.has_key("geometry_service.name")) {
        const std::string geo_service_name = config_.fetch_string("geometry_service.name");
        DT_THROW_IF (! service_mgr_.has(geo_service_name),
                     std::logic_error,
                     "No service named '" << geo_service_name << "' found in the service manager !");
        DT_THROW_IF (! service_mgr_.is_a<geomtools::geometry_service>(geo_service_name),
                     std::logic_error,
                     "Service named '" << geo_service_name << "' is not a geometry service !");
        const geomtools::geometry_service & geo_srv = service_mgr_.get<geomtools::geometry_service>(geo_service_name);
        DT_LOG_NOTICE(_logging_priority,
                      "Set the geometry manager for '" << geo_service_name
                      << "' service in the '" <<  get_name() << "' step hit processor.");
        set_geom_manager(geo_srv.get_geom_manager());
      }
    }

    if (config_.has_key("hit.category")) {
      const std::string hit_cat = config_.fetch_string("hit.category");
      set_hit_category(hit_cat);
    }

    if (config_.has_key("sensitive.category")) {
      const std::string sens_cat = config_.fetch_string("sensitive.category");
      set_sensitive_category(sens_cat);
    }

    // Export others 'sensitive' properties :
    config_.export_starting_with(_auxiliaries, "sensitive.");

    // Export others 'geometry' properties :
    config_.export_starting_with(_auxiliaries, "geometry.");

    // Export others 'output_profiles' properties :
    config_.export_starting_with(_auxiliaries, "output_profiles.");

    // Check:
    DT_THROW_IF (_hit_category.empty(),
                 std::logic_error,
                 "Missing hit category !");
    // Check:
    DT_THROW_IF (_sensitive_category.empty(),
                 std::logic_error,
                 "Missing sensitive category !");

    // Check if we should install a private hit pool in this processor :
    if (! has_pool()) {
      if (config_.has_flag("use_private_pool")) {
        size_t capacity = DEFAULT_POOL_CAPACITY;

        // Use a dedicated capacity for the private pool :
        if (config_.has_key("private_pool_capacity")) {
          int cap = config_.fetch_integer("private_pool_capacity");
          DT_THROW_IF (cap < 1, std::logic_error, "Invalid hit pool capacity(" << cap << ") !");
          capacity =(size_t) cap;
        }
        setup_private_pool(capacity);
        if (is_debug()) _private_pool->dump(std::clog, "Private hit pool: ", "[debug]: ");
      }
    }

    return;
  }

  void base_step_hit_processor::process(const base_step_hit_processor::step_hit_ptr_collection_type & the_base_step_hits,
                                        simulated_data & a_sim_data)
  {
    if (a_sim_data.use_handle_hit_collection()) {
      // Check if the event has a collection of hits
      // within the requested hit category:
      simulated_data::step_hits_dict_type::iterator found
        = a_sim_data.grab_step_hits_dict().find(_hit_category);
      if (found == a_sim_data.grab_step_hits_dict().end()) {
        DT_LOG_TRACE(_logging_priority, "Add the category '" << _hit_category
                     << "' list of hits in the current event...");
        {
          simulated_data::hit_handle_collection_type dummy;
          a_sim_data.grab_step_hits_dict()[_hit_category] = dummy;
        }
        found = a_sim_data.grab_step_hits_dict().find(_hit_category);
      }

      // Pickup a reference to the proper list of handles of hits:
      simulated_data::hit_handle_collection_type & hits = found->second;

      // Invoke the virtual processing method:
      process(the_base_step_hits, hits);
    }

    if (a_sim_data.use_plain_hit_collection()) {
      // Check if the event has a collection of plain hits
      // within the requested hit category:
      simulated_data::plain_step_hits_dict_type::iterator found
        = a_sim_data.grab_plain_step_hits_dict().find(_hit_category);
      if (found == a_sim_data.grab_plain_step_hits_dict().end()) {
        DT_LOG_TRACE(_logging_priority,"Add the category '" << _hit_category
                     << "' list of hits in the current event...");
        simulated_data::hit_collection_type dummy;
        a_sim_data.grab_plain_step_hits_dict()[_hit_category] = dummy;
        found = a_sim_data.grab_plain_step_hits_dict().find(_hit_category);
      }

      // Pickup a reference to the proper list of hits:
      simulated_data::hit_collection_type & hits = found->second;

      // Invoke the virtual processing method:
      process(the_base_step_hits, hits);
    }

    return;
  }


  void base_step_hit_processor::process(const base_step_hit_processor::step_hit_ptr_collection_type & /*the_base_step_hits*/,
                                        simulated_data::hit_collection_type & /*the_plain_hits*/)
  {
    DT_THROW_IF(true, std::runtime_error, "Not implemented !");
    return;
  }

  void base_step_hit_processor::tree_dump(std::ostream & a_out,
                                          const std::string & a_title,
                                          const std::string & a_indent,
                                          bool a_inherit) const
  {
    std::string indent;
    if (! a_indent.empty()) {
      indent = a_indent;
    }
    if ( ! a_title.empty()) {
      a_out << indent << a_title << std::endl;
    }

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Logging priority   : " << datatools::logger::get_priority_label(_logging_priority) << std::endl;
    a_out << indent << datatools::i_tree_dumpable::tag
          << "Name               : '" << _name << "'" << std::endl;
    a_out << indent << datatools::i_tree_dumpable::tag
          << "Hit category       : '" << _hit_category << "'" << std::endl;
    a_out << indent << datatools::i_tree_dumpable::tag
          << "Sensitive category : '" << _sensitive_category << "'" << std::endl;
    a_out << indent << datatools::i_tree_dumpable::tag
          << "Geometry manager   : " << _geom_manager << std::endl;
    {
      a_out << indent << datatools::i_tree_dumpable::tag
            << "Auxiliary properties : " << std::endl;
      std::ostringstream indent_ss;
      indent_ss << indent << datatools::i_tree_dumpable::skip_tag ;
      _auxiliaries.tree_dump(a_out, "", indent_ss.str());
    }
    a_out << indent << datatools::i_tree_dumpable::tag
          << "Private pool : " << _private_pool << std::endl;
    a_out << indent << datatools::i_tree_dumpable::tag
          << "Pool owner   : " << _pool_owner << std::endl;
    a_out << indent << datatools::i_tree_dumpable::inherit_tag(a_inherit)
          << "Pool         : " << _pool << std::endl;

    return;
  }

  // static
  void base_step_hit_processor::init_ocd(datatools::object_configuration_description & ocd_)
  {

    datatools::logger::declare_ocd_logging_configuration(ocd_, "error");

    {
      datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
      cpd.set_name_pattern("geometry_service.name")
        .set_terse_description("Set the name of the geometry service")
        .set_from("mctools::base_step_hit_processor")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .add_example("Use a specific geometry service::          \n"
                     "                                           \n"
                     "  geometry_service.name : string = \"Geo\" \n"
                     "                                           \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
      cpd.set_name_pattern("hit.category")
        .set_terse_description("Set the category of output hits to be produced")
        .set_from("mctools::base_step_hit_processor")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(true)
        .add_example("Use a specific output hit category::       \n"
                     "                                           \n"
                     "  hit.category : string = \"calo\"         \n"
                     "                                           \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
      cpd.set_name_pattern("sensitive.category")
        .set_terse_description("Set the category of input sensitive hits to be processed")
        .set_from("mctools::base_step_hit_processor")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(true)
        .add_example("Use a specific source of input sensitive hits:: \n"
                     "                                            \n"
                     "  sensitive.category : string = \"calo_SD\" \n"
                     "                                            \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
      cpd.set_name_pattern("use_private_pool")
        .set_terse_description("Activate the use of a private pool of preallocated hits")
        .set_from("mctools::base_step_hit_processor")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(false)
        .add_example("Use a private pool of preallocated hits::   \n"
                     "                                            \n"
                     "  use_private_pool : boolean = 1            \n"
                     "                                            \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
      cpd.set_name_pattern("private_pool_capacity")
        .set_terse_description("Set the capacity of the private pool of preallocated hits")
        .set_from("mctools::base_step_hit_processor")
        .set_traits(datatools::TYPE_INTEGER)
        .set_mandatory(false)
        .set_default_value_integer(DEFAULT_POOL_CAPACITY)
        .set_triggered_by_flag("use_private_pool")
        .add_example("Set the capacity of the pool of preallocated hits:: \n"
                     "                                            \n"
                     "  private_pool_capacity : integer = 10      \n"
                     "                                            \n"
                     )
        ;
    }


    return;
  }

  void push_all_step_hit_processor::set_visu_highlighted_hits(bool h_)
  {
    _visu_highlighted_hits_ = h_;
    return;
  }

  bool push_all_step_hit_processor::are_visu_highlighted_hits() const
  {
    return _visu_highlighted_hits_;
  }

  void push_all_step_hit_processor::set_record_mc_step_hit_processor(bool r_)
  {
    _record_mc_step_hit_processor_ = r_;
    return;
  }

  bool push_all_step_hit_processor::is_record_mc_step_hit_processor() const
  {
    return _record_mc_step_hit_processor_;
  }

  push_all_step_hit_processor::push_all_step_hit_processor()
    : base_step_hit_processor()
  {
    _visu_highlighted_hits_ = false;
    _record_mc_step_hit_processor_ = false;

    return;
  }

  push_all_step_hit_processor::~push_all_step_hit_processor()
  {
    return;
  }

  void push_all_step_hit_processor::initialize(const ::datatools::properties & config_,
                                               ::datatools::service_manager & service_mgr_)
  {
    if (get_hit_category().empty()) {
      set_hit_category(step_hit_processor_factory::default_detailed_hit_collection());
    }

    this->base_step_hit_processor::initialize(config_,
                                              service_mgr_);

    if (config_.has_flag("visu.highlighted_hits")) {
      set_visu_highlighted_hits(true);
    }

    if (config_.has_flag("record_mc_step_hit_processor")) {
      set_record_mc_step_hit_processor(true);
    }

    return;
  }

  void push_all_step_hit_processor::process(
    const ::mctools::base_step_hit_processor::step_hit_ptr_collection_type & the_base_step_hits,
    ::mctools::simulated_data::hit_handle_collection_type & the_hits)
  {
    uint32_t count = 0;
    // 2011-04-14 FM : BUG FIX : several processors may use the same
    // 'hit_handle_collection_type' as output, so we must NOT clear 'the_hits' here :
    ////>>>>>the_hits.clear();<<<<
    // We garantee the capacity is enough to host the former stored hits + the new ones :
    size_t current_size = the_hits.size();
    the_hits.reserve(current_size + the_base_step_hits.size());
    for (base_step_hit_processor::step_hit_ptr_collection_type::const_iterator ihit
           = the_base_step_hits.begin();
         ihit != the_base_step_hits.end();
         ihit++) {
      const base_step_hit * the_sim_step_hit =(*ihit);
      // pickup a hit from the pool and add it at the end of the collection of hits(handles) :
      this->add_new_hit(the_hits);
      // get a reference to the last inserted MC base step hit :
      base_step_hit & the_step_hit = the_hits.back().grab();
      the_step_hit = *the_sim_step_hit; // copy the hit in the one stored through the current handle
      the_step_hit.set_hit_id(count); // attribute an unique ID to the hit
      if (are_visu_highlighted_hits()) {
        the_step_hit.grab_auxiliaries().update_flag(mctools::hit_utils::HIT_VISU_HIGHLIGHTED_KEY);
      }
      if (is_record_mc_step_hit_processor()) {
        the_step_hit.grab_auxiliaries().update(mctools::hit_utils::HIT_MC_STEP_PROCESSOR_KEY,
                                               get_name());
      }
      count++; // increment the hit counter
    }
    return;
  }

  void push_all_step_hit_processor::process(
    const ::mctools::base_step_hit_processor::step_hit_ptr_collection_type & the_base_step_hits,
    ::mctools::simulated_data::hit_collection_type & the_plain_hits)
  {
    uint32_t count = 0;
    size_t current_size = the_plain_hits.size();
    the_plain_hits.reserve(current_size + the_base_step_hits.size());
    for (base_step_hit_processor::step_hit_ptr_collection_type::const_iterator ihit
           = the_base_step_hits.begin();
         ihit != the_base_step_hits.end();
         ihit++) {
      const base_step_hit * the_sim_step_hit =(*ihit);
      {
        base_step_hit dummy;
        the_plain_hits.push_back(dummy);
      }
      // get a reference to the new MC base step hit :
      base_step_hit & the_step_hit = the_plain_hits.back();
      the_step_hit = *the_sim_step_hit; // copy the hit in the one newly stored
      the_step_hit.set_hit_id(count); // attribute an unique ID to the hit
      count++; // increment the hit counter
    }
    return;
  }

  MCTOOLS_STEP_HIT_PROCESSOR_REGISTRATION_IMPLEMENT(push_all_step_hit_processor,
                                                    "mctools::push_all_step_hit_processor")

  kill_all_step_hit_processor::kill_all_step_hit_processor()
    : base_step_hit_processor()
  {
    return;
  }

  kill_all_step_hit_processor::~kill_all_step_hit_processor()
  {
    return;
  }

  void kill_all_step_hit_processor::initialize(const ::datatools::properties & config_,
                                               ::datatools::service_manager & service_mgr_)
  {
    if (get_hit_category().empty()) {
      set_hit_category("__null");
    }
    this->base_step_hit_processor::_initialize(config_, service_mgr_);
    return;
  }

  void kill_all_step_hit_processor::process(
     const ::mctools::base_step_hit_processor::step_hit_ptr_collection_type & /*the_base_step_hits*/,
     ::mctools::simulated_data::hit_handle_collection_type & /*the_hits*/)
  {
    // nothing to be done here.
    return;
  }

  void kill_all_step_hit_processor::process(
    const ::mctools::base_step_hit_processor::step_hit_ptr_collection_type & /*the_base_step_hits*/,
    ::mctools::simulated_data::hit_collection_type & /*the_plain_hits*/)
  {
    // nothing to be done here.
    return;
  }

  MCTOOLS_STEP_HIT_PROCESSOR_REGISTRATION_IMPLEMENT(kill_all_step_hit_processor,
                                                    "mctools::kill_all_step_hit_processor")

} // end of namespace mctools

  /***************
   * OCD support *
   ***************/

// OCD support for class '::mctools::push_all_step_hit_processor' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::mctools::push_all_step_hit_processor,ocd_)
{
  ocd_.set_class_name ("mctools::push_all_step_hit_processor");
  ocd_.set_class_description ("A Monte Carlo step hit post-processor that preserves all sensitive hits it processes");
  ocd_.set_class_library ("mctools");
  ocd_.set_class_documentation ("The *push all* step hit processor is used whenever one wants  \n"
                                "to preserve and store all Monte Carlo truth hits generated by \n"
                                "some particle tracking/simulation engine (i.e. Geant4) and    \n"
                                "associated to a given sensitive detector.                     \n"
                                "These hits are thus collected through the *push all* processor            \n"
                                "and stored as is in some output collection of hits addressed by a         \n"
                                "so-called *hit category*.                                                 \n"
                                "This approach enables to preserve all the physics informations            \n"
                                "produced by the simulation engine at the price of memory and storage.     \n"
                                "The raw simulation information is thus not downgraded, nor reduced.       \n"
                                "                                                                          \n"
                                "This is particularly useful when we want to visualize the truth step      \n"
                                "hits from an external rendering tools. In such case, we conventionnaly    \n"
                                "use a hit category named ``__visu.tracks`` that collects truth MC hits    \n"
                                "from various *push all* processors attached to various sensitive detectors.\n"
                                "However this name ``__visu.tracks`` is not mandatory and users are free to\n"
                                "use another name. We recommend that this name starts with a double        \n"
                                "underscore prefix to highlight the fact that the output hit category is   \n"
                                "likely to be used for visualisation, debugging but does not strictly      \n"
                                "represent the output of a realistic sensitive detector.                   \n"
                                "                                                                          \n"
                                "Another usage of this processor is when we don't want to loose some raw   \n"
                                "physics truth informations in order to apply an off-line processing       \n"
                                "(digitization...). In this case, the output hit category should use       \n"
                                "an explicit name (example: ``\"calo\"``).                                 \n"
                                "                                                                          \n"
                                "Keep in mind that, depending of your setup, the *push all* processor is   \n"
                                "generally expensive in term of CPU usage and storage because it tracks    \n"
                                "and stores many truth hits (delta-rays, secondary particles...).           \n"
                                "As an alternative, consider to use your own step hit processor            \n"
                                "as a backend processing of truth hits in order to produce a reduced sets  \n"
                                "of output hits from which your digitization algorithm will be able to     \n"
                                "extract and use valuable informations.                                    \n"
                                "The ``mctools::calorimeter_step_hit_processor`` class implements          \n"
                                "such a processor.\n"
                                ""
                                );

  // Inherits configuration properties from its base class:
  ::mctools::base_step_hit_processor::init_ocd(ocd_);

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("visu.highlighted_hits")
      .set_terse_description("Tag the output hits with some special 'highlight' property")
      .set_from("mctools::base_step_hit_processor")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Do not flag the hits::                      \n"
                   "                                            \n"
                   "  visu.highlighted_hits : boolean = 0       \n"
                   "                                            \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("record_mc_step_hit_processor")
      .set_terse_description("Record the name of the Monte Carlo step hit processor which processed the hits")
      .set_from("mctools::base_step_hit_processor")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Do not flag the hits::                       \n"
                   "                                             \n"
                   "  record_mc_step_hit_processor : boolean = 0 \n"
                   "                                             \n"
                   )
      ;
  }

  ocd_.set_configuration_hints ("A *push all* processor can be associated to an existing sensitive \n"
                                "detector addressed through its *sensitive category*, this category\n"
                                "being *officially* declared in the description of the geometry    \n"
                                "logicial volume/model. In this case, the processor connects and   \n"
                                "scans the sensitive raw hits produced by the detector and stores  \n"
                                "them unchanged in its target hit collection.                      \n"
                                "                                                                  \n"
                                "The MC step hit processor factory uses a 'datatools::properties'  \n"
                                "object to initialize its behaviour and contents.                  \n"
                                "                                                                  \n"
                                "Example of configuration::                                        \n"
                                "                                                                  \n"
                                "  logging.priority      : string  = \"warning\"                   \n"
                                "  sensitive.category    : string  = \"ecalo_SD\"                  \n"
                                "  use_private_pool      : boolean = 1                             \n"
                                "  private_pool_capacity : integer = 50                            \n"
                                "  hit.category          : string  = \"ecalo\"                     \n"
                                "                                                                  \n"
                                "Another possibility is to attach a *push all* processor to some   \n"
                                "non officially existing sensitive detector(s), i.e. volumes not   \n"
                                "considered as *sensitive* in the description of the experiment).  \n"
                                "This is for example the case of the film wrapped around a scintillator\n"
                                "block from which we don't expect to collect any signal unlike the  \n"
                                "scintillator block itself which is considered as the *official*    \n"
                                "source of some collectable physics informations for digitization.  \n"
                                "However, we may be interested in the study                         \n"
                                "of the energy loss of some kind of particle in the wrapping film   \n"
                                "or in the visualization of backscattered tracks in the film.       \n"
                                "A *push all* processor can then be associated to the logical volume\n"
                                "of the wrapping film. A *non-official* sensitive detector will then\n"
                                "be created and associated on the fly to the processor.             \n"
                                "                                                                   \n"
                                "Note that various properties are systematically considered, namely \n"
                                "the ones of which the name starts with the following prefixes:     \n"
                                "                                                                        \n"
                                " * ``sensitive.`` : properties related to the sensitive detector traits \n"
                                "   and the informations we want to store in the output hits.            \n"
                                "   See the ``mctools::g4::sensitive_detector`` class OCD manual         \n"
                                "   from the ``Bayeux_mctools_geant4`` plugin library.                   \n"
                                " * ``geometry.`` : geometry related properties used to associate the    \n"
                                "   processor to some specific volumes of the virtual geometry.          \n"
                                "                                                                        \n"
                                "Example for a step hit processor attached to all volumes with some      \n"
                                "specific material::                                                     \n"
                                "                                                                        \n"
                                "  sensitive.category    : string  = \"__gas_visu_SD\"                   \n"
                                "  hit.category          : string  = \"__visu.tracks\"                   \n"
                                "  geometry.volumes.with_materials : string[1] = \"tracking_gas\"        \n"
                                "                                                                        \n"
                                "Example for a step hit processor attached to some specific volumes::    \n"
                                "                                                                        \n"
                                "  sensitive.category    : string  = \"__calos_visu_SD\"                 \n"
                                "  hit.category          : string  = \"__visu.tracks\"                   \n"
                                "  geometry.volumes : string[2] = \"hcalo.model.log\" \"ecalo.model.log\"\n"
                                "                                                                        \n"
                                "Example for a step hit processor attached to all volumes of the virtual \n"
                                "geometry but some specific volumes. This configuration is generally     \n"
                                "extremely expensive in term of memory and storage but can be very useful\n"
                                "for debugging and/or visualization tasks::                              \n"
                                "                                                                        \n"
                                "  sensitive.category    : string  = \"__calos_visu_SD\"                 \n"
                                "  hit.category          : string  = \"__visu.tracks\"                   \n"
                                "  geometry.volumes.all  : boolean = 1                                   \n"
                                "  geometry.volumes.excluded  : string[3] = \\                           \n"
                                "     \"wires.model.log\"  \\                                            \n"
                                "     \"bold.model.log\"  \\                                             \n"
                                "     \"screw.model.log\"                                                \n"
                                "                                                                        \n"
                                )
    ;

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::mctools::push_all_step_hit_processor,
                               "mctools::push_all_step_hit_processor")


// OCD support for class '::mctools::kill_all_step_hit_processor' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::mctools::kill_all_step_hit_processor,ocd_)
{
  ocd_.set_class_name ("mctools::kill_all_step_hit_processor");
  ocd_.set_class_description ("A Monte Carlo step hit post-processor that discards all sensitive hits it processes");
  ocd_.set_class_library ("mctools");

  // Inherits configuration properties from its base class:
  ::mctools::base_step_hit_processor::init_ocd(ocd_);

  ocd_.set_configuration_hints ("The MC step hit processor factory uses a 'datatools::properties'  \n"
                                "object to initialize its behaviour and contents.                  \n"
                                "                                                                  \n"
                                "Example of configuration::                                        \n"
                                "                                                                  \n"
                                "  sensitive.category    : string = \"off_detector_SD\"            \n"
                                "  use_private_pool      : boolean = 1                             \n"
                                "  private_pool_capacity : integer = 100                           \n"
                                "  hit.category          : string = \"NULL\"                       \n"
                                "                                                                  \n"
                                "                                                                  \n"
                               )
    ;

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::mctools::kill_all_step_hit_processor,
                               "mctools::kill_all_step_hit_processor")

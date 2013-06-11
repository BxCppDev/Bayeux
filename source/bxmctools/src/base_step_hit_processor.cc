// -*- mode: c++ ; -*-
/* base_step_hit_processor.cc
 */

#include <mctools/base_step_hit_processor.h>
#include <mctools/utils.h>

#include <datatools/ioutils.h>
#include <datatools/service_manager.h>
#include <datatools/exception.h>


#include <geomtools/geometry_service.h>

namespace mctools {

  using namespace std;

  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(base_step_hit_processor, "mctools::base_step_hit_processor/__system__");

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
       _logging_priority = datatools::logger::PRIO_NOTICE;
    }
    return;
  }

  const std::string & base_step_hit_processor::get_name() const
  {
    return _name;
  }

  void base_step_hit_processor::set_name(const string & n_)
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

  void base_step_hit_processor::set_external_rng(mygsl::rng & rng_)
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
                   logic_error,
                   "Cannot resize private pool for processor '" << get_name() << "' !");
      if (capacity_ < _private_pool->get_capacity()) {
        return;
      } else {
        _private_pool->resize(capacity_);
      }
    }
    else
      {
        DT_LOG_WARNING(_logging_priority,
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
                 logic_error,
                 "Processor '" << get_name() << "' does not use any pool of hits !");
    return *_pool;
  }

  bool base_step_hit_processor::has_pool() const
  {
    return _pool != 0;
  }

  void base_step_hit_processor::add_new_hit(simulated_data::hit_handle_collection_type & the_hits)
  {
    if (has_pool())
      {
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
      }
    else
      {
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
    _logging_priority = datatools::logger::PRIO_NOTICE;
    _geom_manager = 0;
    _private_pool = 0;
    _pool_owner = false;
    _pool = 0;
    return;
  }

  base_step_hit_processor::~base_step_hit_processor()
  {
    if (_private_pool != 0) {
      delete _private_pool;
    }
    return;
  }

  const string & base_step_hit_processor::get_hit_category() const
  {
    return _hit_category;
  }

  void base_step_hit_processor::set_hit_category(const string & hc_)
  {
    _hit_category = hc_;
    return;
  }

  const string & base_step_hit_processor::get_sensitive_category() const
  {
    return _sensitive_category;
  }

  void base_step_hit_processor::set_sensitive_category(const string & sc_)
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
    if (config_.has_flag ("verbose"))  {
      set_logging_priority(datatools::logger::PRIO_NOTICE);
    }

    if (config_.has_flag ("debug")) {
      set_logging_priority(datatools::logger::PRIO_DEBUG);
    }

    if (config_.has_key("logging.priority")) {
      std::string prio_label = config_.fetch_string("logging.priority");
      datatools::logger::priority p = datatools::logger::get_priority(prio_label);
      DT_THROW_IF(p == datatools::logger::PRIO_UNDEFINED,
                  std::domain_error,
                  "Unknow logging priority ``" << prio_label << "`` !");
      set_logging_priority(p);
    }


    if (! has_geom_manager())  {
        if (config_.has_key("geometry_service.name")) {
            std::string geo_service_name = config_.fetch_string("geometry_service.name");
            DT_THROW_IF (! service_mgr_.has(geo_service_name),
                         logic_error,
                         "No service named '" << geo_service_name << "' found in the service manager !");
            DT_THROW_IF (! service_mgr_.is_a<geomtools::geometry_service>(geo_service_name),
                         logic_error,
                         "Service named '" << geo_service_name << "' is not a geometry service !");
            const geomtools::geometry_service & geo_srv = service_mgr_.get<geomtools::geometry_service>(geo_service_name);
            DT_LOG_NOTICE(_logging_priority,
                          "Set the geometry manager for '" << geo_service_name
                          << "' service in the '" <<  get_name() << "' step hit processor.");
            set_geom_manager(geo_srv.get_geom_manager());
          }
      }

    if (config_.has_key("hit.category")) {
        string hit_cat = config_.fetch_string("hit.category");
        set_hit_category(hit_cat);
      }

    if (config_.has_key("sensitive.category")) {
        string sens_cat = config_.fetch_string("sensitive.category");
        set_sensitive_category(sens_cat);
      }

    // Export others 'sensitive' properties :
    config_.export_starting_with(_auxiliaries, "sensitive.");


    // Export others 'geometry' properties :
    config_.export_starting_with(_auxiliaries, "geometry.");
    /*
      if (config_.has_key("geometry.models"))
      {
      vector<string> models;
      config_.fetch("geometry.models" ,models);
      _auxiliaries.store("geometry.models", models);
      }

      if (config_.has_key("geometry.models.with_materials"))
      {
      vector<string> models_with_materials;
      config_.fetch("geometry.models.with_materials", models_with_materials);
      _auxiliaries.store("geometry.models.with_materials", models_with_materials);
      }

      if (config_.has_key("geometry.models.excluded"))
      {
      vector<string> models_excluded;
      config_.fetch("geometry.models.excluded", models_excluded);
      _auxiliaries.store("geometry.models.excluded", models_excluded);
      }
    */

    // Check:
    DT_THROW_IF (_hit_category.empty(),
                 logic_error,
                 "Missing hit category !");
    // Check:
    DT_THROW_IF (_sensitive_category.empty(),
                 logic_error,
                 "Missing sensitive category !");

    // Check if we should install a private hit pool in this processor :
    if (! has_pool()) {
        if (config_.has_flag("use_private_pool")) {
            size_t capacity = DEFAULT_POOL_CAPACITY;

            // Use a dedicated capacity for the private pool :
            if (config_.has_key("private_pool_capacity")) {
                int cap = config_.fetch_integer("private_pool_capacity");
                DT_THROW_IF (cap < 1,logic_error,"Invalid hit pool capacity(" << cap << ") !");
                capacity =(size_t) cap;
              }
            setup_private_pool(capacity);
            if (is_debug()) _private_pool->dump(clog, "Private hit pool: ");
          }
      }

    return;
  }

  void base_step_hit_processor::process(const base_step_hit_processor::step_hit_ptr_collection_type & the_base_step_hits,
                                        simulated_data & a_sim_data)
  {
    bool devel = false;

    if (a_sim_data.use_handle_hit_collection())
      {
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


  void base_step_hit_processor::process(const base_step_hit_processor::step_hit_ptr_collection_type & the_base_step_hits,
                                        simulated_data::hit_collection_type & the_plain_hits)
  {
    DT_THROW_IF(true, runtime_error, "Not implemented !");
    return;
  }

  void base_step_hit_processor::tree_dump(ostream & a_out,
                                          const string & a_title,
                                          const string & a_indent,
                                          bool a_inherit) const
  {
    std::string indent;
    if (! a_indent.empty()) {
      indent = a_indent;
    }
    if ( ! a_title.empty()) {
      a_out << indent << a_title << endl;
    }

    namespace du = datatools;

    a_out << indent << du::i_tree_dumpable::tag
          << "Logging priority   : " << datatools::logger::get_priority_label(_logging_priority) << endl;
    a_out << indent << du::i_tree_dumpable::tag
          << "Name               : '" << _name << "'" << endl;
    a_out << indent << du::i_tree_dumpable::tag
          << "Hit category       : '" << _hit_category << "'" << endl;
    a_out << indent << du::i_tree_dumpable::tag
          << "Sensitive category : '" << _sensitive_category << "'" << endl;
    a_out << indent << du::i_tree_dumpable::tag
          << "Geometry manager   : " << _geom_manager << endl;
    {
      a_out << indent << du::i_tree_dumpable::tag
            << "Auxiliary properties : " << endl;
      std::ostringstream indent_ss;
      indent_ss << indent << du::i_tree_dumpable::skip_tag ;
      _auxiliaries.tree_dump(a_out, "", indent_ss.str());
    }
    a_out << indent << du::i_tree_dumpable::tag
          << "Private pool : " << _private_pool << endl;
    a_out << indent << du::i_tree_dumpable::tag
          << "Pool owner   : " << _pool_owner << endl;
    a_out << indent << du::i_tree_dumpable::inherit_tag(a_inherit)
          << "Pool         : " << _pool << endl;

    return;
  }

  /****************************************************/

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

  MCTOOLS_STEP_HIT_PROCESSOR_INITIALIZE_IMPLEMENT_HEAD(push_all_step_hit_processor,
                                                       config_,
                                                       service_mgr_)
  {
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

  MCTOOLS_STEP_HIT_PROCESSOR_PROCESS_HANDLE_IMPLEMENT_HEAD(push_all_step_hit_processor,
                                                           the_base_step_hits,
                                                           the_hits)
  {
    bool devel = true;
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

  MCTOOLS_STEP_HIT_PROCESSOR_PROCESS_PLAIN_IMPLEMENT_HEAD(push_all_step_hit_processor,
                                                          the_base_step_hits,
                                                          the_plain_hits)
  {
    bool devel = true;
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
                                                    "mctools::push_all_step_hit_processor");

  /****************************************************/

  kill_all_step_hit_processor::kill_all_step_hit_processor()
    : base_step_hit_processor()
  {
    return;
  }

  kill_all_step_hit_processor::~kill_all_step_hit_processor()
  {
    return;
  }

  MCTOOLS_STEP_HIT_PROCESSOR_INITIALIZE_IMPLEMENT_HEAD(kill_all_step_hit_processor,
                                                       config_,
                                                       service_mgr_)
  {
    this->base_step_hit_processor::initialize(config_,
                                              service_mgr_);
    return;
  }

  MCTOOLS_STEP_HIT_PROCESSOR_PROCESS_HANDLE_IMPLEMENT_HEAD(kill_all_step_hit_processor,
                                                           the_base_step_hits,
                                                           the_hits)
  {
    // nothing to be done here.
    return;
  }

  MCTOOLS_STEP_HIT_PROCESSOR_PROCESS_PLAIN_IMPLEMENT_HEAD(kill_all_step_hit_processor,
                                                          the_base_step_hits,
                                                          the_plain_hits)
  {
    // nothing to be done here.
    return;
  }

  MCTOOLS_STEP_HIT_PROCESSOR_REGISTRATION_IMPLEMENT(kill_all_step_hit_processor,
                                                    "mctools::kill_all_step_hit_processor");

} // end of namespace mctools

// end of base_step_hit_processor.cc

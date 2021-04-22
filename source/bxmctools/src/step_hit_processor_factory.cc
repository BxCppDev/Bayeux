/// \file mctools/step_hit_processor_factory.cc

// Ourselves:
#include <mctools/step_hit_processor_factory.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/service_manager.h>
#include <datatools/exception.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>
#include <geomtools/geometry_service.h>

namespace mctools {

  // static
  const std::string & step_hit_processor_factory::default_detailed_hit_collection()
  {
    static const std::string _name("__visu.tracks");
    return _name;
  }

  // static
  const std::string & step_hit_processor_factory::matching_output_profiles_key()
  {
    static const std::string _name("output_profiles.matching");
    return _name;
  }

  step_hit_processor_factory::processor_entry_type::~processor_entry_type()
  {
    reset();
    return;
  }

  void step_hit_processor_factory::processor_entry_type::reset()
  {
    datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
    // logging = datatools::logger::PRIO_TRACE;
    DT_LOG_TRACE(logging,"Entering...");
    DT_LOG_TRACE(logging,"name='" << name << "'");
    DT_LOG_TRACE(logging,"type='" << type << "'");
    if (handle.has_data()) {
      DT_LOG_TRACE(logging,"handle=" << &handle.get());
    } else {
      DT_LOG_TRACE(logging,"handle=" << "<none>");
    }
    handle.reset(0);
    config.reset();
    DT_LOG_TRACE(logging,"Exiting.");
    return;
  }

  bool step_hit_processor_factory::is_initialized () const
  {
    return _initialized_;
  }

  datatools::logger::priority step_hit_processor_factory::get_logging_priority() const
  {
    return _logging_priority_;
  }

  void step_hit_processor_factory::set_logging_priority(datatools::logger::priority p)
  {
    _logging_priority_ = p;
  }

  const std::string & step_hit_processor_factory::get_description () const
  {
    return _description_;
  }

  void step_hit_processor_factory::set_description (const std::string & description_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error,
                 "Factory is initialized and locked !");
    _description_ = description_;
    return;
  }

  void step_hit_processor_factory::set_instantiate_at_loading(bool ial_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error,
                 "Factory is initialized and locked !");
    _instantiate_at_loading_ = ial_;
    return;
  }

  bool step_hit_processor_factory::has_external_prng () const
  {
    return _external_prng_ != 0;
  }

  void step_hit_processor_factory::set_external_prng (mygsl::rng & prng_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error,
                 "Factory is initialized and locked !");
    _external_prng_ = &prng_;
    return;
  }

  mygsl::rng & step_hit_processor_factory::grab_external_prng()
  {
    DT_THROW_IF (_external_prng_ == 0, std::logic_error,
                 "No external PRNG is referenced !");
    return *_external_prng_;
  }

  void step_hit_processor_factory::set_output_profiles(const std::set<std::string> & output_profiles_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error,
                 "Factory is initialized and locked !");
    _output_profiles_ = output_profiles_;
    return;
  }

  void step_hit_processor_factory::reset_output_profiles()
  {
    _output_profiles_.clear();
    return;
  }

  bool step_hit_processor_factory::has_geometry_manager () const
  {
    return _geom_manager_ != 0;
  }

  void step_hit_processor_factory::set_geometry_manager (const geomtools::manager & gmgr_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error,
                 "Factory is initialized and locked !");
    _geom_manager_ = &gmgr_;
    return;
  }

  const geomtools::manager & step_hit_processor_factory::get_geometry_manager () const
  {
    return *_geom_manager_;
  }

  bool step_hit_processor_factory::has_service_manager () const
  {
    return _service_manager_ != 0;
  }


  void step_hit_processor_factory::set_service_manager (datatools::service_manager & smgr_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Factory is initialized and locked !");
    _service_manager_ = &smgr_;
    return;
  }

  datatools::service_manager & step_hit_processor_factory::grab_service_manager ()
  {
    return *_service_manager_;
  }

  const datatools::service_manager & step_hit_processor_factory::get_service_manager () const
  {
    return *_service_manager_;
  }

  bool step_hit_processor_factory::has_processor (const std::string & name_) const
  {
    return _entries_.find(name_) != _entries_.end ();
    // return _processors_.find (name_) != _processors_.end ();
  }

  bool step_hit_processor_factory::is_processor_instantiated(const std::string & name_) const
  {
    processor_entry_dict_type::const_iterator found = _entries_.find(name_);
    DT_THROW_IF(found ==  _entries_.end(), std::logic_error,
                "Processor '" << name_ << "' does not exist !");
    return found->second.handle.has_data();
  }

  bool step_hit_processor_factory::is_processor_instantiable(const std::string & name_) const
  {
    bool result = true;
    processor_entry_dict_type::const_iterator found = _entries_.find(name_);
    DT_THROW_IF(found ==  _entries_.end(), std::logic_error,
                "Processor '" << name_ << "' does not exist !");
    const processor_entry_type & pe = found->second;
    // Access to configuration properties:
    const datatools::properties & config = pe.config;

    // Analyse the possible output profiles for this processor:
    std::vector<std::string> matching_output_profile_ids;
    if (config.has_key(matching_output_profiles_key())) {
      // Does the processor have specific simulation output profiles:
      config.fetch(matching_output_profiles_key(), matching_output_profile_ids);
    }

    if (matching_output_profile_ids.size()) {
      // We consider the processor instantiable only if the
      // factory has activated at least one of the matching
      // output profiles:
      result = false;
      int matching_count = 0;
      for (int i = 0; i < (int) matching_output_profile_ids.size(); i++) {
        const std::string & profile_id = matching_output_profile_ids[i];
        if (_output_profiles_.count(profile_id) == 1) {
          matching_count++;
        }
      }
      if (matching_count > 0) {
        result = true;
      }
    }

    return result;
  }

  void step_hit_processor_factory::_initialize()
  {
    DT_LOG_TRACE(_logging_priority_,"Entering...");
    DT_THROW_IF (is_initialized(), std::logic_error, "Factory is already initialized !");

    for (processor_entry_dict_type::iterator i = _entries_.begin();
         i != _entries_.end();
         i++) {
      processor_entry_type & pe = i->second;
      const std::string & processor_name = pe.name;

      if (! pe.handle.has_data()) {
        base_step_hit_processor * proc = _create(processor_name);
        if (proc != nullptr) {
          DT_LOG_NOTICE(get_logging_priority(),
                        "Instantiation of step hit processor '" << processor_name << "' as "
                        << "a '" <<  pe.type << "'.");
        } else {
          DT_LOG_WARNING(get_logging_priority(),
                        "No instantiation of step hit processor '" << processor_name << "' as "
                        << "a '" <<  pe.type << "' !");
        }
      }
    }

    _initialized_ = true;
    DT_LOG_TRACE(_logging_priority_,"Exiting.");
    return;
   }

  void step_hit_processor_factory::initialize()
  {
    DT_THROW_IF (is_initialized (), std::logic_error, "Factory is already initialized !");
    _initialize();
    return;
  }

  void step_hit_processor_factory::initialize(const datatools::properties & config_)
  {
    DT_THROW_IF (is_initialized (), std::logic_error, "Factory is already initialized !");

    if (config_.has_flag("logging.priority")) {
      std::string prio_label = config_.fetch_string("logging.priority");
      datatools::logger::priority p = datatools::logger::get_priority(prio_label);
      DT_THROW_IF(p == datatools::logger::PRIO_UNDEFINED,
                  std::domain_error,
                  "Unknow logging priority ``" << prio_label << "`` !");
      set_logging_priority(p);
    }

    if (config_.has_flag("instantiate_at_loading")) {
      set_instantiate_at_loading(true);
    }

    if (_description_.empty()) {
      if (config_.has_key("description")) {
        set_description(config_.fetch_string("description"));
      }
    }

    if (! has_geometry_manager()) {
      if (has_service_manager()) {
        if (config_.has_key("services.geometry")) {
          const std::string & geo_service_label = config_.fetch_string("services.geometry");
          DT_THROW_IF (! get_service_manager().has(geo_service_label), std::logic_error,
                       "No service with name '" << geo_service_label << "' !");
          DT_THROW_IF (! get_service_manager().is_a<geomtools::geometry_service> ("geo_service_label"),
                       std::logic_error,
                       "Service with name '" << geo_service_label << "' is not a geometry service !");
          const geomtools::geometry_service & geo_serv
            = get_service_manager().get<geomtools::geometry_service>("geo_service_label");
          set_geometry_manager(geo_serv.get_geom_manager ());
        }
      }
    }

    std::vector<std::string> proc_configs;
    if (config_.has_key("processors.configuration")) {
      config_.fetch("processors.configuration", proc_configs);
    }

    for (size_t i = 0; i < proc_configs.size(); i++) {
      std::string filename = proc_configs[i];
      datatools::fetch_path_with_env(filename);
      datatools::multi_properties mp;
      datatools::read_config(filename, mp);
      load(mp);
    }

    _initialize();
    return;
  }

  void step_hit_processor_factory::reset ()
  {
    datatools::logger::priority logging = _logging_priority_;
    // logging = datatools::logger::PRIO_TRACE;
    DT_LOG_TRACE(logging,"Entering...");
    DT_THROW_IF (! is_initialized (), std::logic_error,
                 "Factory is not initialized !");
    _initialized_ = false;
    _processors_.clear();
    usleep(1000);
    for (processor_entry_dict_type::iterator i = _entries_.begin ();
         i != _entries_.end ();
         i++) {
      processor_entry_type & pe = i->second;
      DT_LOG_TRACE(logging, "Cleaning '" << pe.name << "'...");
      pe.handle.reset(0);
      DT_LOG_TRACE(logging, " --> handle is reset.");
      pe.config.clear();
      DT_LOG_TRACE(logging, " --> config is reset.");
      DT_LOG_TRACE(logging, "Cleaning '" << pe.name << "'... done.");
    }
    _entries_.clear();
    _output_profiles_.clear ();
    _instantiate_at_loading_ = false;
    _service_manager_ = nullptr;
    _geom_manager_ = nullptr;
    _external_prng_ = nullptr;
    DT_LOG_TRACE(logging,"Exiting.");
    return;
  }

  // Constructor :
  step_hit_processor_factory::step_hit_processor_factory (datatools::logger::priority logging_)
  {
    _initialized_ = false;
    _logging_priority_ = logging_;
    _factory_register_.set_label ("mctools::base_step_hit_processor/factory");
    _factory_register_.set_logging_priority (logging_);
    _instantiate_at_loading_ = false;
    _service_manager_ = nullptr;
    _geom_manager_ = nullptr;
    _external_prng_ = nullptr;
    bool preload = true;
    if (preload) {
      _factory_register_.import (DATATOOLS_FACTORY_GET_SYSTEM_REGISTER (::mctools::base_step_hit_processor));
    }
    return;
  }

  // Destructor :
  step_hit_processor_factory::~step_hit_processor_factory ()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  const base_step_hit_processor &
  step_hit_processor_factory::get_processor (const std::string & name_) const
  {
    step_hit_processor_factory * mutable_this = const_cast<step_hit_processor_factory *>(this);
    return const_cast<base_step_hit_processor &>(mutable_this->grab_processor(name_));
  }

  base_step_hit_processor &
  step_hit_processor_factory::grab_processor (const std::string & name_)
  {
    processor_entry_dict_type::iterator found = _entries_.find (name_);
    DT_THROW_IF (found == _entries_.end (), std::logic_error,
                 "No step hit processor named '" << name_ << "' !");
    processor_entry_type & pe = found->second;
    base_step_hit_processor * proc_ptr = nullptr;
    if (! pe.handle.has_data()) {
      proc_ptr = this->_create(name_);
      DT_THROW_IF (proc_ptr == nullptr, std::logic_error,
                   "Step hit processor named '" << name_ << "' cannot be instantiated !");

    }
    return pe.handle.grab();
    /*
    processor_dict_type::iterator found = _processors_.find (name_);
    DT_THROW_IF (found == _processors_.end (), std::logic_error,
                 "No step hit processor named '" << name_ << "' !");
    return *(found->second);
    */
  }

  // step_hit_processor_factory::processor_dict_type &
  // step_hit_processor_factory::grab_processors ()
  // {
  //   return _processors_;
  // }

  const step_hit_processor_factory::processor_dict_type &
  step_hit_processor_factory::get_processors () const
  {
    DT_THROW_IF (! is_initialized(), std::logic_error,
                 "Factory is not yet initialized and locked !");
    return _processors_;
  }

  base_step_hit_processor * step_hit_processor_factory::_create(const std::string & name_)
  {
    DT_LOG_TRACE (get_logging_priority (), "Entering...");
    base_step_hit_processor * proc = nullptr;
    DT_THROW_IF (name_.empty(), std::logic_error,
                 "Missing step hit processor name !");
    processor_entry_dict_type::iterator found = _entries_.find(name_);
    DT_THROW_IF (found == _entries_.end(), std::logic_error,
                 "Step hit processor named '" << name_ << "' does not exists !");
    processor_entry_type & pe = found->second;

    bool create_it = false;
    if (is_processor_instantiable(pe.name)) {
      create_it = true;
    }

    if (create_it) {
      const base_step_hit_processor::factory_register_type::factory_type & the_factory
        = _factory_register_.get(pe.type);
      DT_LOG_DEBUG (get_logging_priority (), "About to create a new step hit processor of type '"
                    << pe.type << "' with name '" << pe.name << "'...");
      proc = the_factory();
      DT_LOG_DEBUG (get_logging_priority (), "Store the new instantiated '" << pe.type
                    << "' step hit processor with name '" << pe.name << "' ...");
      proc->set_name(pe.name);

      if (proc->accept_external_rng () && has_external_prng ()) {
        proc->set_external_rng (*_external_prng_);
      }
      DT_LOG_DEBUG (get_logging_priority (), "Initialize the new processor '" << pe.name << "'...");
      bool accept_geom_manager = true;
      if (pe.config.has_flag("reject_geometry_manager_from_factory")) {
        accept_geom_manager = false;
      }

      if (accept_geom_manager && has_geometry_manager ()) {
        proc->set_geom_manager(get_geometry_manager ());
      }

      if (has_service_manager()) {
        proc->initialize(pe.config, *_service_manager_);
      } else {
        proc->initialize(pe.config);
      }
      // Initialize the handle with the newly created processor:
      pe.handle.reset(proc);
      // Record the processor address in the dictionnary of instantiated processors:
      _processors_[pe.name] = &pe.handle.grab();
    }

    DT_LOG_TRACE (get_logging_priority (), "Exiting");
    return proc;
  }

  void step_hit_processor_factory::fetch_processor_names(std::vector<std::string> & pnames_,
                                                         bool only_instantiated_)  const
  {
    for (processor_entry_dict_type::const_iterator i = _entries_.begin();
         i != _entries_.end();
         i++) {
      bool fetch_it = true;
      if (only_instantiated_) {
        if (! i->second.handle.has_data()) {
           fetch_it = false;
        }
      }
      if (fetch_it) {
        pnames_.push_back(i->first);
      }
    }
    return;
  }


  bool
  step_hit_processor_factory::create (const std::string & name_,
                                      const std::string & type_,
                                      const datatools::properties & config_)
  {
    DT_LOG_TRACE (get_logging_priority (), "Entering...");
    DT_THROW_IF (name_.empty (), std::logic_error,
                 "Missing step hit processor name !");
    DT_THROW_IF (type_.empty (), std::logic_error,
                 "Missing step hit processor type !");

    DT_THROW_IF (has_processor (name_), std::logic_error,
                 "Step hit processor named '" << name_ << "' already exists !");
    DT_THROW_IF (! _factory_register_.has (type_), std::logic_error,
                 "No registered step hit processor class with ID '"
                 << type_ << "' for step hit processor named '" << name_ << " !");

    processor_entry_type pe;
    pe.name = name_;
    pe.type = type_;
    pe.config = config_;
    _entries_[pe.name] = pe;

    DT_LOG_TRACE (get_logging_priority (), "Exiting.");
    base_step_hit_processor * proc = _create(name_);
    if (proc == 0) return false;
    return true;
    /*
    const base_step_hit_processor::factory_register_type::factory_type & the_factory
      = _factory_register_.get (type_);
    DT_LOG_TRACE (get_logging_priority (), "About to create a new step hit processor of type '"
                  << type_ << "' with name '" << name_ << "'...");
    proc = the_factory ();
    DT_LOG_TRACE (get_logging_priority (), "Store the new '" << type_
                  << "' step hit processor with name '" << name_ << "' ...");
    proc->set_name (name_);

    if (proc->accept_external_rng () && has_external_prng ()) {
      proc->set_external_rng (*_external_prng_);
    }
    DT_LOG_DEBUG (get_logging_priority (), "Initialize the new processor...");

    bool accept_geom_manager = true;
    if (config_.has_flag("reject_geometry_manager_from_factory")) {
      accept_geom_manager = false;
    }

    if (accept_geom_manager && has_geometry_manager ()) {
      proc->set_geom_manager(get_geometry_manager ());
    }

    if (has_service_manager()) {
      proc->initialize (config_, *_service_manager_);
    } else {
      proc->initialize (config_);
    }
    // Store the new step hit processor in the dictionnary :
    processor_handle_type new_handle (proc);
    _handles_[name_] = new_handle;
    _processors_[name_] = proc;
    DT_LOG_TRACE (get_logging_priority (), "Exiting.");
    return *proc;
    */
  }

  void step_hit_processor_factory::load(const datatools::multi_properties & mprop_)
  {
    DT_LOG_TRACE (get_logging_priority (), "Entering...");
    if (get_logging_priority() >= datatools::logger::PRIO_DEBUG) {
        DT_LOG_DEBUG (get_logging_priority (), "Step hit processors factory rules:");
        mprop_.tree_dump (std::clog);
      }
    for (datatools::multi_properties::entries_col_type::const_iterator i
           = mprop_.entries ().begin ();
         i != mprop_.entries ().end ();
         i++) {
      const datatools::multi_properties::entry & e = i->second;
      processor_entry_type pe;
      pe.name = e.get_key();
      pe.type = e.get_meta();
      pe.config = e.get_properties();
      _entries_[pe.name] = pe;
      DT_LOG_TRACE (get_logging_priority (), "Processor name = '" << pe.name << "'");
      DT_LOG_TRACE (get_logging_priority (), "Processor type = '" << pe.type << "'");
      if (_instantiate_at_loading_) {
        this->create (pe.name, pe.type, pe.config);
      }
    }
    return;
  }


  void step_hit_processor_factory::tree_dump (std::ostream & a_out,
                                              const std::string & a_title,
                                              const std::string & a_indent,
                                              bool a_inherit) const
  {
    std::string indent;
    if (! a_indent.empty ()) {
      indent = a_indent;
    }
    if ( ! a_title.empty ()) {
      a_out << indent << a_title << std::endl;
    }

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Initialized : " << is_initialized() << std::endl;
    {
      a_out << indent << datatools::i_tree_dumpable::tag
            << "Factory register : " << std::endl;
      std::ostringstream indent_ss;
      indent_ss << indent << datatools::i_tree_dumpable::skip_tag;
      _factory_register_.tree_dump (a_out, "", indent_ss.str ());
    }
    a_out << indent << datatools::i_tree_dumpable::tag
          << "Logging priority : " << datatools::logger::get_priority_label(_logging_priority_) << std::endl;
    a_out << indent << datatools::i_tree_dumpable::tag
          << "Description      : '" << _description_ << "'" << std::endl;
    a_out << indent << datatools::i_tree_dumpable::tag
          << "Service manager  : " << _service_manager_ << std::endl;
    a_out << indent << datatools::i_tree_dumpable::tag
          << "Output profiles  : " << _output_profiles_.size() << std::endl;
    for (std::set<std::string>::const_iterator i = _output_profiles_.begin();
         i != _output_profiles_.end();
         i++) {
      std::set<std::string>::const_iterator j = i;
      j++;
      a_out << indent << datatools::i_tree_dumpable::skip_tag;
      if (j == _output_profiles_.end()) {
        a_out << datatools::i_tree_dumpable::last_tag;
      } else {
        a_out << datatools::i_tree_dumpable::tag;
      }
      a_out << "Output profile : '" << *i << "'" << std::endl;
    }

    {
      a_out << indent << datatools::i_tree_dumpable::tag
            << "Processor entries  : ";
      if ( _entries_.size() ) {
        a_out << _entries_.size();
      } else {
        a_out << "<none>";
      }
      a_out << std::endl;
      for (processor_entry_dict_type::const_iterator i = _entries_.begin ();
           i != _entries_.end ();
           i++) {
        a_out << indent << datatools::i_tree_dumpable::skip_tag;
        processor_entry_dict_type::const_iterator j = i;
        j++;
        if (j == _entries_.end ()) {
          a_out << datatools::i_tree_dumpable::last_tag;
        } else {
          a_out << datatools::i_tree_dumpable::tag;
        }
        const processor_entry_type & pe =  i->second;
        a_out << "Name: '" << pe.name << "' Type: '" << pe.type << "' ";
        if (pe.handle) {
          a_out << "(address: " << &pe.handle.get() << ")";
        }
        a_out << std::endl;
      }
    }

    {
      a_out << indent << datatools::i_tree_dumpable::tag
            << "Instantiated processors: ";
      if ( _processors_.size() ) {
        a_out << _processors_.size();
      } else {
        a_out << "<none>";
      }
      a_out << std::endl;
      for (processor_dict_type::const_iterator i = _processors_.begin ();
           i != _processors_.end ();
           i++) {
        a_out << indent << datatools::i_tree_dumpable::skip_tag;
        processor_dict_type::const_iterator j = i;
        j++;
        std::ostringstream indent_ss;
        indent_ss << indent << datatools::i_tree_dumpable::skip_tag;
        if (j == _processors_.end ()) {
          a_out << datatools::i_tree_dumpable::last_tag;
          indent_ss << datatools::i_tree_dumpable::last_skip_tag;
        } else {
          a_out << datatools::i_tree_dumpable::tag;
          indent_ss << datatools::i_tree_dumpable::skip_tag;
        }
        a_out << "Name: '" << i->first << "' address @ " << i->second << std::endl;
        i->second->tree_dump (a_out, "", indent_ss.str ());
      }

    }
    a_out << indent << datatools::i_tree_dumpable::inherit_tag (a_inherit)
          << "External PRNG : ";
    if (has_external_prng()) {
      a_out << _external_prng_;
    } else {
      a_out << "<none>";
    }
    a_out << std::endl;

    return;
  }

  void step_hit_processor_factory::dump (std::ostream & out_) const
  {
    tree_dump (out_, "mctools::step_hit_processor_factory: ");
    return;
  }

} // end of namespace mctools


  /***************
   * OCD support *
   ***************/

#include <datatools/ocd_macros.h>

// OCD support for class '::mctools::step_hit_processor_factory' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::mctools::step_hit_processor_factory,ocd_)
{
  ocd_.set_class_name ("mctools::step_hit_processor_factory");
  ocd_.set_class_description ("A factory for Monte-Carlo step hit post-processors");
  ocd_.set_class_library ("mctools");

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("logging.priority")
      .set_terse_description("The logging priority threshold")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("Default value is ``\"fatal\"``.         \n"
                            "Example::                               \n"
                            "                                        \n"
                            "  logging.priority : string : \"fatal\" \n"
                            "                                        \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("description")
      .set_terse_description("The embeded description string")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("Superseded by a former call of :                                \n"
                            "``mctools::step_hit_processor_factory::set_description(...)``   \n"
                            "Example::                                                       \n"
                            "                                                                \n"
                            "   description : string : \"The MC hit processors factory\"     \n"
                            "                                                                \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("services.geometry")
      .set_terse_description("The name of the geometry service")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_complex_triggering_conditions(true)
      .set_long_description("This property is used only if :                      \n"
                            "                                                     \n"
                            " * a service manager is set in the factory,          \n"
                            " * no geometry manager has been set in the factory.  \n"
                            "                                                     \n"
                             "Example::                                           \n"
                            "                                                     \n"
                            "   services.geometry : string = \"Geo\"              \n"
                            "                                                     \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("processors.configuration")
      .set_terse_description("A list of configuration file names for embeded MC step hit processors")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_path(true)
      .set_long_description("A list of filenames from where the MC step hit processors  \n"
                            "loads the directives to dynamically instantiate new        \n"
                            "embedded MC step hit processors objects. The filenames      \n"
                            "main contain some environment variables.                   \n"
                            "Example::                                                  \n"
                            "                                                           \n"
                            "   processors.configuration : string[1] as path = \\       \n"
                            "      \"${CONFIG_REPOSITORY_DIR}/scin_mc_hit_proc.conf\"   \n"
                            "                                                           \n"
                            "The target files must use the format of the                \n"
                            "``datatools::multi_properties`` class.                     \n"
                            "The loading order of the files is critical                 \n"
                            "because some step hit processors may depend on other       \n"
                            "ones which should thus be defined *before* their           \n"
                            "dependers.                                                 \n"
                            "Extends the instantiation of step hit processors triggered \n"
                            "by former calls to :                                       \n"
                            "``myctools::step_hit_processor_factory::load(...)``        \n"
                            )
      ;
  }

  ocd_.set_configuration_hints ("The MC step hit processor factory uses a 'datatools::properties'  \n"
                                "object to initialize its behaviour and contents.                  \n"
                                "                                                                  \n"
                                "Example of configuration::                                        \n"
                                "                                                                  \n"
                                "   logging.priority : string : \"fatal\"                          \n"
                                "   description : string = \"The MC hit processor factory for Geant4\"\n"
                                "   services.geometry : string = \"Geo\"                           \n"
                                "   processors.configuration : string[2] as path = \\              \n"
                                "      \"${CONFIG_REPOSITORY_DIR}/telescope_mc_hit_proc.conf\" \\  \n"
                                "      \"${CONFIG_REPOSITORY_DIR}/NaI_mc_hit_proc.conf\"           \n"
                                "                                                                  \n"
                                "Each processor configuration file uses the format of the          \n"
                                "``datatools::multi_properties`` class.                            \n"
                                "See OCD support dedicated to  MC hit processor classes.           \n"
                                )
    ;

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::mctools::step_hit_processor_factory,
                               "mctools::step_hit_processor_factory")

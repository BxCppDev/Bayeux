/* manager.cc */

// Ourselves
#include <genbb_help/manager.h>
#include <genbb_help/i_genbb.h>

// Standard Library
#include <stdexcept>
#include <sstream>

// Third Party

// Datatools
#include <datatools/ioutils.h>
#include <datatools/utils.h>
#include <datatools/properties.h>
#include <datatools/multi_properties.h>
#include <datatools/exception.h>

// Mygsl
#include <mygsl/rng.h>
#include <mygsl/random_utils.h>

namespace genbb {

  //----------------------------------------------------------------------
  // Public Interface Definitions
  //

  datatools::logger::priority manager::get_logging_priority() const
  {
    return _logging_priority_;
  }

  void manager::set_logging_priority(datatools::logger::priority p_)
  {
    _logging_priority_ = p_;
  }

  bool manager::is_debug() const {
    return _logging_priority_ >= datatools::logger::PRIO_DEBUG;
  }


  void manager::set_debug(bool debug_) {
    if (debug_) set_logging_priority(datatools::logger::PRIO_DEBUG);
    else set_logging_priority(datatools::logger::PRIO_WARNING);
  }

  void manager::set_default_generator(const std::string & def_gen_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Manager is already initialized !");
    _default_generator_ = def_gen_;
    return;
  }

  bool manager::has_default_generator() const
  {
    return !_default_generator_.empty();
  }

  const std::string & manager::get_default_generator() const
  {
    return _default_generator_;
  }


  bool manager::is_initialized() const
  {
    return _initialized_;
  }


  bool manager::has_external_prng() const
  {
    return _external_prng_ != 0;
  }

  void manager::reset_external_prng()
  {
    _external_prng_ = 0;
    return;
  }

  void manager::set_external_prng(mygsl::rng & r_)
  {
    _external_prng_ = &r_;
    return;
  }

  mygsl::rng & manager::grab_external_prng()
  {
    DT_THROW_IF(! has_external_prng(), std::logic_error, "No available external PRNG !");
    return *_external_prng_;
  }

  const mygsl::rng & manager::get_external_prng() const
  {
    DT_THROW_IF(! has_external_prng(), std::logic_error, "No available external PRNG !");
    return *_external_prng_;
  }

  mygsl::rng & manager::grab_embedded_prng()
  {
    return _embedded_prng_;
  }

  const mygsl::rng & manager::get_embedded_prng() const
  {
    return _embedded_prng_;
  }

  mygsl::rng & manager::grab_prng()
  {
    if (has_external_prng()) return grab_external_prng();
    return grab_embedded_prng();
  }

  const mygsl::rng & manager::get_prng() const
  {
    if (has_external_prng()) return get_external_prng();
    return get_embedded_prng();
  }

  void manager::set_embedded_prng_seed(int seed_)
  {
    DT_THROW_IF(this->is_initialized(), std::logic_error,
                "Manager is already initialized !");
    DT_THROW_IF(! mygsl::rng::is_seed_valid(seed_), std::logic_error,
                "Invalid seed value for embedded PRNG !");
    _embedded_prng_seed_ = seed_;
    return;
  }

  void manager::load(const std::string & name,
                     const std::string & id,
                     const datatools::properties & config)
  {
    this->_load_pg(name, id, config);
  }

  void manager::load(const datatools::multi_properties & config)
  {
    DT_THROW_IF(this->is_initialized(), std::logic_error,
                "Particle generator manager is already initialized !");

    for (datatools::multi_properties::entries_ordered_col_type::const_iterator i
           = config.ordered_entries().begin();
         i != config.ordered_entries().end();
         ++i) {
      datatools::multi_properties::entry* mpe = *i;
      const std::string& pg_name = mpe->get_key();
      const std::string& pg_id = mpe->get_meta();
      DT_LOG_DEBUG(get_logging_priority(), "Configuration for pg named '"
                   << pg_name << "' with ID '" << pg_id << "'...");
      this->_load_pg(pg_name, pg_id, mpe->get_properties());
    }
    return;
  }


  void manager::initialize(const datatools::properties & config) {
    DT_THROW_IF(this->is_initialized(), std::logic_error,
                "Manager is already initialized !");

    if (_logging_priority_ == datatools::logger::PRIO_WARNING) {
      if (config.has_flag("debug")) {
        set_logging_priority(datatools::logger::PRIO_DEBUG);
      }
    }

    if (config.has_key("logging.priority")) {
      std::string prio_label = config.fetch_string("logging.priority");
      datatools::logger::priority p = datatools::logger::get_priority(prio_label);
      DT_THROW_IF(p == datatools::logger::PRIO_UNDEFINED,
                  std::domain_error,
                  "Unknow logging priority ``" << prio_label << "`` !");
      set_logging_priority(p);
    }

    // Particle generators :
    {
      typedef std::vector<std::string> CFList;
      typedef CFList::iterator CFListIterator;
      std::string conf_file_key("generators.configuration_files");

      CFList conf_file_list;

      if (config.has_key(conf_file_key)) {
        config.fetch(conf_file_key, conf_file_list);
      }

      for (CFListIterator i = conf_file_list.begin();
           i < conf_file_list.end();
           ++i) {
        datatools::fetch_path_with_env(*i);
        datatools::multi_properties mconfig;
        datatools::multi_properties::read_config(*i, mconfig);
        this->load(mconfig);
      }
    }

    if (has_external_prng()) {
      DT_THROW_IF(! _external_prng_->is_initialized(),
                  std::logic_error,
                  "External PRNG is not initialized !");
    } else {
      if (config.has_key("seed")) {
        int seed = config.fetch_integer("seed");
        set_embedded_prng_seed(seed);
      }

      _embedded_prng_.initialize("taus2", _embedded_prng_seed_);
    }

    if (config.has_key("generator")) {
      std::string def_gen = config.fetch_string("generator");
      DT_THROW_IF(!has(def_gen), std::logic_error, "Default generator '" << def_gen << "' is not known!");
      set_default_generator(def_gen);
    }

    _initialized_ = true;
    return;
  }


  void manager::reset()
  {
    DT_LOG_TRACE(get_logging_priority(), "Entering...");
    DT_THROW_IF(!_initialized_, std::logic_error, "Manager is not initialized !");

    _initialized_ = false;
    size_t count = _particle_generators_.size();
    size_t initial_size = _particle_generators_.size();
    while (_particle_generators_.size() > 0) {
      for (detail::pg_dict_type::iterator it = _particle_generators_.begin();
           it != _particle_generators_.end();
           ++it) {
        detail::pg_entry_type& entry = it->second;
        DT_LOG_DEBUG(get_logging_priority(), "Removing particle generator '"
                     << entry.get_name() << "'...");
        this->_reset_pg(entry);
        _particle_generators_.erase(it);
        --count;
        break;
      }
      if (count == initial_size) {
        break;
      }
    }

    _particle_generators_.clear();
    _factory_register_.reset();
    _force_initialization_at_load_ = false;
    _preload_ = true;

    if (_embedded_prng_.is_initialized()) {
      _embedded_prng_.reset();
      _embedded_prng_seed_ = mygsl::random_utils::SEED_INVALID;
    }
    _external_prng_ = nullptr;
    _default_generator_.clear();

    DT_LOG_TRACE(get_logging_priority(), "Exiting.");
    return;
  }


  bool manager::has_service_manager() const
  {
    return _service_mgr_ != nullptr;
  }

  void manager::reset_service()
  {
    _service_mgr_ = nullptr;
    return;
  }

  void manager::set_service_manager(datatools::service_manager & service_mgr_)
  {
    _service_mgr_ = &service_mgr_;
    return;
  }

  // Constructor :
  manager::manager(datatools::logger::priority p, int flags)
    : _factory_register_("genbb::i_genbb/pg_factory",
                         p >= datatools::logger::PRIO_NOTICE ?
                         i_genbb::factory_register_type::verbose : 0)
  {
    _service_mgr_ = nullptr;
    _initialized_ = false;
    set_logging_priority(p);

    _force_initialization_at_load_ = false;
    if (flags & FORCE_INITIALIZATION_AT_LOAD) {
      _force_initialization_at_load_ = true;
    }

    bool preload = true;
    if (flags & NO_PRELOAD) {
      preload = false;
    }

    _external_prng_ = nullptr;
    _embedded_prng_seed_ = mygsl::random_utils::SEED_INVALID;

    this->_set_preload_(preload);
    return;
  }


  // Destructor :
  manager::~manager()
  {
    if (_initialized_) this->reset();
    return;
  }


  /***************************
   *   Particle generators   *
   ***************************/

  const i_genbb & manager::get(const std::string & name) const
  {
    manager* mgr = const_cast<manager*>(this);
    return const_cast<i_genbb&>(mgr->grab(name));
  }


  i_genbb & manager::grab(const std::string & name)
  {
    detail::pg_dict_type::iterator found = _particle_generators_.find(name);
    DT_THROW_IF(found == _particle_generators_.end(), std::logic_error,
                "Particle generator '" << name << "' does not exist !");
    detail::pg_entry_type& entry = found->second;
    if (!entry.is_initialized()) {
      this->_initialize_pg(entry);
    }
    return entry.grab_handle().grab();
  }


  bool manager::has(const std::string & name) const
  {
    return _particle_generators_.find(name) != _particle_generators_.end();
  }


  bool manager::is_initialized(const std::string & name) const
  {
    detail::pg_dict_type::const_iterator found = _particle_generators_.find(name);
    return found != _particle_generators_.end() && found->second.is_initialized();
  }

  void manager::reset(const std::string & name)
  {
    detail::pg_dict_type::iterator found = _particle_generators_.find(name);
    DT_THROW_IF(found == _particle_generators_.end(), std::logic_error,
                "Particle generator '" << name << "' does not exist !");
    detail::pg_entry_type& entry = found->second;
    this->_reset_pg(entry);
    return ;
  }


  bool manager::can_drop(const std::string & name) const
  {
    detail::pg_dict_type::const_iterator found = _particle_generators_.find(name);
    DT_THROW_IF(found == _particle_generators_.end(), std::logic_error,
                "Particle generator '" << name << "' does not exist !");
    return true;
  }

  const std::string & manager::get_id(const std::string & name) const
  {
    detail::pg_dict_type::const_iterator found = _particle_generators_.find(name);
    DT_THROW_IF(found == _particle_generators_.end(), std::logic_error,
                "Particle generator '" << name << "' does not exist !");
    const detail::pg_entry_type& entry = found->second;
    return entry.get_id();
  }


  void manager::drop(const std::string & name)
  {
    detail::pg_dict_type::iterator found = _particle_generators_.find(name);
    DT_THROW_IF(found == _particle_generators_.end(), std::logic_error,
                "Particle generator '" << name << "' does not exist !");
    detail::pg_entry_type& entry = found->second;
    // if (!found->second.can_be_dropped()) {
    //   std::ostringstream message;
    //   message << "genbb::manager::drop: "
    //           << "Particle generator '"
    //           << name
    //           << "' cannot be dropped !";
    //   th row std::logic_error(message.str());
    // }

    DT_LOG_DEBUG(get_logging_priority(),
                 "Reset & remove particle generator '" << name << "' !");
    this->_reset_pg(entry);
    _particle_generators_.erase(found);
    return;
  }

  void manager::dump_particle_generators(std::ostream & out,
                                         const std::string & title,
                                         const std::string & an_indent) const
  {
    print_particle_generators(out, title, an_indent, "tree");
    return;
  }

  void manager::print_particle_generators(std::ostream & out,
                                          const std::string & title,
                                          const std::string & an_indent,
                                          const std::string & mode) const
  {
    std::string print_mode = mode;
    if (print_mode.empty()) {
      print_mode = "tree";
    }
    std::string indent;
    if (!an_indent.empty()) indent = an_indent;
    if (!title.empty()) out << title << ":" << std::endl;
    // Particle generators:
    {
      size_t sz = _particle_generators_.size();
      size_t count = 0;
      for (detail::pg_dict_type::const_iterator it = _particle_generators_.begin();
           it != _particle_generators_.end();
           ++it) {
        count++;
        out << indent;
        const std::string& pg_name = it->first;
        const detail::pg_entry_type& pg_record = it->second;
        const std::string& pg_desc = it->second.get_config().get_description();

        if (print_mode == "tree") {
          // tree mode:
          if (count == sz) {
            out << "`-- ";
          } else {
            out << "|-- ";
          }
          out.setf(std::ios::left, std::ios::adjustfield);
          out.width(30);
          out << pg_name << " : ";
          out << pg_desc << " : ";
          out << pg_record.get_id() << " ";
          out << '(';
          if (pg_record.is_initialized()) {
            out << "initialized";
          } else {
            out << "not initialized";
          }
          out << ')';
          out << std::endl;
        } else if (print_mode == "raw") {
          // raw mode:
          out << pg_name << ':' << pg_desc << ':' << pg_record.get_id()
              << std::endl;
        }
      }
    }
    return;
  }


  bool manager::has_pg_type(const std::string & id) const
  {
    return _factory_register_.has(id);
  }


  void manager::unregister_pg_type(const std::string & id)
  {
    _factory_register_.unregistration(id);
    return;
  }


  void manager::tree_dump(std::ostream & out,
                          const std::string & title,
                          const std::string & a_indent,
                          bool inherit) const
  {
    std::string indent;
    if (!a_indent.empty()) indent = a_indent;

    if (!title.empty()) out << indent << title << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Logging priority : "
        << datatools::logger::get_priority_label(_logging_priority_) << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Preload        : "
        << _preload_
        << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Force initialization : "
        << _force_initialization_at_load_
        << std::endl;

    out << indent << i_tree_dumpable::tag
        << "List of registered particle generators : " << std::endl;
    {
      std::ostringstream indent_oss;
      indent_oss << indent << i_tree_dumpable::skip_tag;

      _factory_register_.print(out, indent_oss.str());
    }

    {
      out << indent << i_tree_dumpable::tag
          << "Particle generators       : ";
      size_t sz = _particle_generators_.size();
      if (sz == 0) {
        out << "<none>";
      }

      out << std::endl;
      for (detail::pg_dict_type::const_iterator i = _particle_generators_.begin();
           i != _particle_generators_.end();
           ++i) {
        const std::string& pg_name = i->first;
        const detail::pg_entry_type& pg_entry = i->second;
        out << indent << i_tree_dumpable::skip_tag;

        std::ostringstream indent_oss;
        indent_oss << indent << i_tree_dumpable::skip_tag;
        detail::pg_dict_type::const_iterator j = i;
        j++;
        if (j == _particle_generators_.end()) {
          out << i_tree_dumpable::last_tag;
          indent_oss << i_tree_dumpable::last_skip_tag;
        } else {
          out << i_tree_dumpable::tag;
          indent_oss << i_tree_dumpable::skip_tag;
        }
        out << "Particle generator : '" << pg_name << "'" << std::endl;
        pg_entry.tree_dump(out, "", indent_oss.str());
      }
    }

    out << indent << i_tree_dumpable::tag
        << "Default generator  : ";
    if (!has_default_generator()) {
      out << "<none>";
    } else {
      out << "'" << get_default_generator() << "'";
    }
    out << std::endl;

    out << indent << i_tree_dumpable::inherit_tag(inherit)
        << "Initialized    : "
        << this->is_initialized()
        << std::endl;
    return;
  }

  const datatools::factory_register<i_genbb> & manager::get_factory_register() const
  {
    return _factory_register_;
  }

  datatools::factory_register<i_genbb> & manager::grab_factory_register()
  {
    return _factory_register_;
  }

  //----------------------------------------------------------------------
  // Protected Interface Definitions
  //

  void manager::_load_pg(const std::string & name,
                         const std::string & id,
                         const datatools::properties & config)
  {
    DT_LOG_TRACE(get_logging_priority(), "Entering...");
    DT_THROW_IF(this->has(name), std::logic_error,
                "Particle generator '" << name << "' already exists !");
    {
      // Add a new entry :
      detail::pg_entry_type tmp_entry;
      tmp_entry.set_name(name);
      DT_LOG_DEBUG(get_logging_priority(), "Add an entry for particle generator '"
                   << name << "'...");
      _particle_generators_[name] = tmp_entry;
    }
    // fetch a reference on it and update :
    detail::pg_entry_type& new_entry = _particle_generators_.find(name)->second;
    new_entry.set_id(id);
    new_entry.set_config(config);
    new_entry.set_manager(*this);

    if (_force_initialization_at_load_) {
      //this->_create_pg(new_entry);
      this->_initialize_pg(new_entry);
    }

    DT_LOG_TRACE(get_logging_priority(), "Exiting.");
    return;
  }


  void manager::_preload_global_dict()
  {
    _factory_register_.import(DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(genbb::i_genbb));
    return;
  }


  void manager::_create_pg(detail::pg_entry_type & entry)
  {
    if (!entry.is_created()) {
      DT_LOG_DEBUG(get_logging_priority(), "Creating particle generator named '"
                   <<  entry.get_name() << "'...");
      detail::create(entry,
                     &_factory_register_,
                     &grab_prng());
    }
    return;
  }


  void manager::_initialize_pg(detail::pg_entry_type & entry)
  {
    // If not created, first do it :
    if (!entry.is_created()) {
      this->_create_pg(entry);
    }

    // If not initialized, do it :
    if (!entry.is_initialized()) {
      DT_LOG_DEBUG(get_logging_priority(), "Initializing particle generator named '"
                   << entry.get_name() << "'...");
      detail::initialize(entry,
                         (has_service_manager() ? _service_mgr_ : 0),
                         &_particle_generators_,
                         &_factory_register_,
                         &grab_prng());
      // (has_external_prng()? &grab_external_prng() : 0));
    }
    return;
  }


  void manager::_reset_pg(detail::pg_entry_type & entry)
  {
    detail::reset(entry);
    return;
  }


  //----------------------------------------------------------------------
  // Private Interface Definitions

  void manager::_set_preload_(bool preload_)
  {
    _preload_ = preload_;
    if (_preload_) {
      this->_preload_global_dict();
    }
    return;
  }


}  // end of namespace genbb


/***************
 * OCD support *
 ***************/

#include <datatools/ocd_macros.h>

// OCD support for class '::genbb::manager' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::genbb::manager,ocd_)
{
  ocd_.set_class_name("genbb::manager");
  ocd_.set_class_description("A generic manager for event generators");
  ocd_.set_class_library("genbb_help");
  ocd_.set_class_documentation("The ``genbb::manager`` class is responsible of the     \n"
                               "instantiation, configuration and memory management of  \n"
                               "a collection of event generators requested by the user,\n"
                               "typically from a set of ASCII configuration files.     \n"
                               "The manager can provide its own pseudo random number   \n"
                               "generator (PRNG) or accept an external PRNG provided by\n"
                               "the user.                                              \n"
                               );

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("logging.priority")
      .set_terse_description("Set the logging priority threshold")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("Allowed values are:                           \n"
                            "                                              \n"
                            " * ``\"trace\"``                              \n"
                            " * ``\"debug\"``                              \n"
                            " * ``\"information\"``                        \n"
                            " * ``\"notice\"``                             \n"
                            " * ``\"warning\"``                            \n"
                            " * ``\"error\"``                              \n"
                            " * ``\"critical\"``                           \n"
                            " * ``\"fatal\"``                              \n"
                            "                                              \n"
                            "Default value: ``\"warning\"``                \n"
                            "Example::                                     \n"
                            "                                              \n"
                            "  logging.priority: string = \"notice\"       \n"
                            "                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("debug")
      .set_terse_description("Flag to activate debug logging")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("Shortcut flag to activate debug logging.      \n"
                            "Example::                                     \n"
                            "                                              \n"
                            "  debug: boolean = 1                          \n"
                            "                                              \n"
                            "Superseded by the ``logging.priority`` property. \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("seed")
      .set_terse_description("Embedded PRNG's seed")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(false)
      .set_complex_triggering_conditions(true)
      .set_long_description("The seed of the embedded PRNG.                \n"
                            "Not used if some external PRNG is used.       \n"
                            "Example::                                     \n"
                            "                                              \n"
                            "  seed : integer = 314159                     \n"
                            "                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("generators.configuration_files")
      .set_terse_description("A list of configuration file names for event generators")
      .set_traits(datatools::TYPE_STRING,
                  configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_path(true)
      .set_long_description("A list of filenames from where the manager                  \n"
                            "loads the directives to dynamically instantiate             \n"
                            "new event generators. The filenames may contains            \n"
                            "some environment variables.                                 \n"
                            "Example::                                                   \n"
                            "                                                            \n"
                            "   generators.configuration_files : string[1] as path = \\  \n"
                            "     \"${CONFIG_REPOSITORY_DIR}/eg.conf\"                   \n"
                            "                                                            \n"
                            "The target files must use the format of the                 \n"
                            "``datatools::multi_properties`` class.                      \n"
                            "The loading order of the files is critical                  \n"
                            "because some generators may depend on other ones            \n"
                            "which should thus be defined **before** their               \n"
                            "dependers.                                                  \n"
                            )
      ;
  }

  ocd_.set_configuration_hints("Example::                                                   \n"
                               "                                                            \n"
                               "  logging.priority: string = \"notice\"                     \n"
                               "  seed: integer = 314159                                    \n"
                               "  generators.configuration_files : string[1] as path = \\   \n"
                               "     \"${CONFIG_REPOSITORY_DIR}/event_generators.conf\"     \n"
                               "                                                            \n"
                               "The generator files use the ``datatools::multi_properties`` \n"
                               "format. Some possible generators are:                       \n"
                               "                                                            \n"
                               " * ``genbb::wdecay0`` : see OCD support                     \n"
                               " * ``genbb::single_particle_generator`` : see OCD support   \n"
                               "                                                            \n"
                               "Example of a generator file: ::                             \n"
                               "                                                            \n"
                               "  #@key_label  \"name\"                                     \n"
                               "  #@meta_label \"type\"                                     \n"
                               "                                                            \n"
                               "  [name=\"Bi207\" type=\"genbb::wdecay0\"]                  \n"
                               "  decay_type    : string  = \"background\"                  \n"
                               "  decay_isotope : string  = \"Bi207+Pb207m\"                \n"
                               "                                                            \n"
                               "  [name=\"bb0nu_Se82\" type=\"genbb::wdecay0\"]             \n"
                               "  decay_type    : string  = \"DBD\"                         \n"
                               "  decay_isotope : string  = \"Se82\"                        \n"
                               "  decay_dbd_level : integer = 0                             \n"
                               "  decay_dbd_mode  : integer = 1                             \n"
                               "                                                            \n"
                               );
  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(genbb::manager,"genbb::manager")

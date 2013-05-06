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

// Mygsl
#include <mygsl/rng.h>
#include <mygsl/random_utils.h>

namespace genbb {

  //----------------------------------------------------------------------
  // Public Interface Definitions
  //

  bool manager::is_debug() const {
    return _debug_;
  }


  void manager::set_debug(bool debug) {
    _debug_ = debug;
  }


  bool manager::is_initialized() const {
    return _initialized_;
  }


  bool manager::has_external_prng () const
  {
    return _external_prng_ != 0;
  }

  void manager::reset_external_prng ()
  {
    _external_prng_ = 0;
    return;
  }

  void manager::set_external_prng (mygsl::rng & r_)
  {
    _external_prng_ = &r_;
    return;
  }


  mygsl::rng & manager::grab_external_prng ()
  {
    if (! has_external_prng ())
      {
        throw std::logic_error ("genbb::manager::grab_external_prng: No available external PRNG !");
      }
    return *_external_prng_;
  }


  const mygsl::rng & manager::get_external_prng () const
  {
    if (! has_external_prng ())
      {
        throw std::logic_error ("genbb::manager::get_external_prng: No available external PRNG !");
      }
    return *_external_prng_;
  }

  mygsl::rng & manager::grab_embeded_prng ()
  {
    return _embeded_prng_;
  }

  const mygsl::rng & manager::get_embeded_prng () const
  {
    return _embeded_prng_;
  }

  mygsl::rng & manager::grab_prng ()
  {
    if (has_external_prng()) return grab_external_prng ();
    return grab_embeded_prng ();
  }

  const mygsl::rng & manager::get_prng () const
  {
    if (has_external_prng()) return get_external_prng ();
    return get_embeded_prng ();
  }

  void manager::set_embeded_prng_seed(int seed_)
  {
    if (!_initialized_) {
      std::ostringstream message;
      message << "genbb::manager::set_embeded_prng_seed: "
              << "Manager is already initialized !";
      throw std::logic_error(message.str());
    }
    if (! mygsl::rng::is_seed_valid(seed_)) {
      throw std::logic_error("genbb::manager::set_embeded_prng_seed: Invalid seed for embeded PRNG !");
    }
    _embeded_prng_seed_ = seed_;
    return;
  }

  void manager::load(const std::string& name,
                     const std::string& id,
                     const datatools::properties& config) {
    this->_load_pg(name, id, config);
  }

  void manager::load(const datatools::multi_properties& config) {
    bool debug = this->is_debug();
    if (debug) {
      std::clog << datatools::io::debug
                << "genbb::manager::load: "
                << "Entering..."
                << std::endl;
    }

    if (this->is_initialized()) {
      std::ostringstream message;
      message << "manager::load: "
              << "Particle generator manager is already initialized !";
      throw std::logic_error(message.str());
    }

    for (datatools::multi_properties::entries_ordered_col_type::const_iterator i
           = config.ordered_entries().begin();
         i != config.ordered_entries().end();
         ++i) {
      datatools::multi_properties::entry* mpe = *i;
      const std::string& pg_name = mpe->get_key();
      const std::string& pg_id = mpe->get_meta();
      if (debug) {
        std::clog << datatools::io::debug
                  << "genbb::manager::load: "
                  << "Configuration for pg named '"
                  << pg_name << "' "
                  << " with ID '"
                  << pg_id << "'..."
                  << std::endl;
      }
      this->_load_pg(pg_name, pg_id, mpe->get_properties());
    }
    return;
  }


  void manager::initialize(const datatools::properties& config) {
    if (this->is_initialized()) {
      std::ostringstream message;
      message << "genbb::manager::initialize: "
              << "Manager is already initialized !";
      throw std::logic_error(message.str());
    }

    if (!this->is_debug()) {
      if (config.has_flag("debug")) {
        this->set_debug(true);
      }
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
        mconfig.read(*i);
        /*
        if (is_debug()) {
        // XXX
        }
        */
        this->load(mconfig);
      }
    }

    if (has_external_prng())
      {
        if (! _external_prng_->is_initialized())
          {
            std::ostringstream message;
            message << "genbb::manager::initialize: "
                    << "External PRNG is not initialized !";
            throw std::logic_error(message.str());
          }
      }
    else
      {
        if (config.has_key("seed")) {
          int seed = config.fetch_integer("seed");
          set_embeded_prng_seed(seed);
        }

        _embeded_prng_.initialize("taus2", _embeded_prng_seed_);
      }

    _initialized_ = true;
    return;
  }


  void manager::reset() {
    if (this->is_debug()) {
      std::clog << datatools::io::debug
                << "genbb::manager::reset: "
                << "Entering..."
                << std::endl;
    }

    if (!_initialized_) {
      std::ostringstream message;
      message << "genbb::manager::reset: "
              << "Manager is not initialized !";
      throw std::logic_error(message.str());
    }

    size_t count = _particle_generators_.size();
    size_t initial_size = _particle_generators_.size();
    while (_particle_generators_.size() > 0) {
      for (detail::pg_dict_type::iterator it = _particle_generators_.begin();
           it != _particle_generators_.end();
           ++it) {
        detail::pg_entry_type& entry = it->second;
        if (this->is_debug()) {
          std::clog << datatools::io::debug
                    << "genbb::manager::reset: "
                    << "Removing particle generator '"
                    << entry.get_name ()
                    << "'..."
                    << std::endl;
        }
        this->_reset_pg(entry);
        _particle_generators_.erase(it);
        --count;
        break;
      }
      if (count == initial_size) {
        break;
      }
    }

    if (_particle_generators_.size() > 0) {
      std::clog << datatools::io::warning
                << "genbb::manager::reset: "
                << "There are some left particle generators !"
                << std::endl;
    }
    _particle_generators_.clear();
    _factory_register_.reset();
    _force_initialization_at_load_ = false;
    _preload_ = true;

    if (_embeded_prng_.is_initialized())
      {
        _embeded_prng_.reset();
        _embeded_prng_seed_ = mygsl::random_utils::SEED_INVALID;
      }
    _external_prng_ = 0;

    _initialized_ = false;
    if (this->is_debug()) {
      std::clog << datatools::io::debug
                << "genbb::manager::reset: "
                << "Exiting."
                << std::endl;
    }
    return;
  }


  bool manager::has_service_manager() const
  {
    return _service_mgr_ != 0;
  }

  void manager::reset_service ()
  {
    _service_mgr_ = 0;
    return;
  }

  void manager::set_service_manager (datatools::service_manager & service_mgr_)
  {
    _service_mgr_ = &service_mgr_;
    return;
  }

  // Constructor :
  manager::manager(uint32_t flag)
    : _factory_register_("genbb::i_genbb/pg_factory",
                        flag & VERBOSE ?
                        i_genbb::factory_register_type::verbose : 0) {
    _service_mgr_ = 0;
    _initialized_ = false;
    this->set_debug(flag & DEBUG);
    _force_initialization_at_load_ = false;

    if (flag & FORCE_INITIALIZATION_AT_LOAD) {
      _force_initialization_at_load_ = true;
    }

    bool preload = true;
    if (flag & NO_PRELOAD) {
      preload = false;
    }

    _external_prng_ = 0;
    _embeded_prng_seed_ = mygsl::random_utils::SEED_INVALID;

    this->_set_preload_(preload);
    return;
  }


  // Destructor :
  manager::~manager () {
    if (_initialized_) this->reset();
    return;
  }


  /***************************
   *   Particle generators   *
   ***************************/

  const i_genbb & manager::get(const std::string& name) const
  {
    manager* mgr = const_cast<manager*>(this);
    return const_cast<i_genbb&>(mgr->grab(name));
  }


  i_genbb & manager::grab(const std::string& name)
  {
    detail::pg_dict_type::iterator found = _particle_generators_.find(name);
    if (found == _particle_generators_.end())
      {
        std::ostringstream message;
        message << "genbb::manager::grab: "
                << "Particle generator '"
                << name
                << "' does not exist !";
        throw std::logic_error(message.str());
      }
    detail::pg_entry_type& entry = found->second;
    if (!entry.is_initialized()) {
      this->_initialize_pg(entry);
    }
    return entry.grab_handle().grab();
  }


  bool manager::has(const std::string& name) const {
    return _particle_generators_.find(name) != _particle_generators_.end();
  }


  bool manager::is_initialized(const std::string& name) const {
    detail::pg_dict_type::const_iterator found = _particle_generators_.find(name);
    return found != _particle_generators_.end() && found->second.is_initialized();
  }

  void manager::reset(const std::string& name) {
    detail::pg_dict_type::iterator found = _particle_generators_.find(name);
    if (found == _particle_generators_.end()) {
      std::ostringstream message;
      message << "genbb::manager::can_drop: "
              << "Particle generator '"
              << name
              << "' does not exist !";
      throw std::logic_error(message.str());
    }
    detail::pg_entry_type& entry = found->second;
    this->_reset_pg(entry);
    return ;
  }


  bool manager::can_drop(const std::string& name) const {
    detail::pg_dict_type::const_iterator found = _particle_generators_.find(name);
    if (found == _particle_generators_.end()) {
      std::ostringstream message;
      message << "genbb::manager::can_drop: "
              << "Particle generator '"
              << name
              << "' does not exist !";
      throw std::logic_error(message.str());
    }
    return true;
  }

  const std::string & manager::get_id(const std::string& name) const
  {
    detail::pg_dict_type::const_iterator found = _particle_generators_.find(name);
    if (found == _particle_generators_.end()) {
      std::ostringstream message;
      message << "genbb::manager::can_drop: "
              << "Particle generator '"
              << name
              << "' does not exist !";
      throw std::logic_error(message.str());
    }
    const detail::pg_entry_type& entry = found->second;
    return entry.get_id();
  }


  void manager::drop(const std::string& name) {
    detail::pg_dict_type::iterator found = _particle_generators_.find(name);
    if (found == _particle_generators_.end()) {
      std::ostringstream message;
      message << "genbb::manager::drop: "
              << "Particle generator '"
              << name
              << "' does not exist !";
      throw std::logic_error(message.str());
    }

    detail::pg_entry_type& entry = found->second;
    // if (!found->second.can_be_dropped()) {
    //   std::ostringstream message;
    //   message << "genbb::manager::drop: "
    //           << "Particle generator '"
    //           << name
    //           << "' cannot be dropped !";
    //   throw std::logic_error(message.str());
    // }

    if (this->is_debug()) {
      std::clog << datatools::io::debug
                << "genbb::manager::drop: "
                << "Reset & remove particle generator '"
                << name
                << "' !"
                << std::endl;
    }

    this->_reset_pg(entry);
    _particle_generators_.erase(found);
    return;
  }


  void manager::dump_particle_generators(std::ostream& out,
                                         const std::string& title,
                                         const std::string& an_indent) const {
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
        if (count == sz) {
          out << "`-- ";
        } else {
          out << "|-- ";
        }
        const std::string& pg_name = it->first;
        const detail::pg_entry_type& pg_record = it->second;

        out << "Name : '" << pg_name << "' "
            << "Type : '" << pg_record.get_id () << "' ";
        out << '(';
        int count = 0;
        if (pg_record.is_initialized()) {
          out << "initialized";
          count++;
        }
        out << ')';
        out << std::endl;
      }
    }
    return;
  }


  bool manager::has_pg_type(const std::string& id) const {
    return _factory_register_.has(id);
  }


  void manager::unregister_pg_type(const std::string& id) {
    _factory_register_.unregistration(id);
    return;
  }


  void manager::tree_dump(std::ostream& out,
                          const std::string& title,
                          const std::string& a_indent,
                          bool inherit) const {
    std::string indent;
    if (!a_indent.empty()) indent = a_indent;

    if (!title.empty()) out << indent << title << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Debug          : "
        << _debug_ << std::endl;

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

  void manager::_load_pg(const std::string& name,
                             const std::string& id,
                             const datatools::properties& config) {
    bool debug = this->is_debug();
    if (debug) {
      std::clog << datatools::io::debug
                << "genbb::manager::_load_pg: "
                << "Entering..."
                << std::endl;
    }
    if (this->has(name)) {
      std::ostringstream message;
      message << "genbb::manager::_load_pg: "
              << "Particle generator '"
              << name
              << "' already exists !";
      throw std::logic_error(message.str());
    }

    {
      // Add a new entry :
      detail::pg_entry_type tmp_entry;
      tmp_entry.set_name(name);
      if (debug) {
        std::clog << datatools::io::debug
                  << "genbb::manager::_load_pg: "
                  << "Add an entry for particle generator '"
                  << name
                  << "'..."
                  << std::endl;
      }
      _particle_generators_[name] = tmp_entry;
    }
    // fetch a reference on it and update :
    detail::pg_entry_type& new_entry = _particle_generators_.find(name)->second;
    new_entry.set_id(id);
    new_entry.set_config(config);
    new_entry.set_manager(*this);

    if (debug) {
      std::clog << datatools::io::debug
                << "genbb::manager::_load_pg: "
                << "Fetch..."
                << std::endl;
    }

    if (_force_initialization_at_load_) {
      //this->_create_pg(new_entry);
      this->_initialize_pg(new_entry);
    }

    if(debug) {
      std::clog << datatools::io::debug
                << "genbb::manager::_load_pg: "
                << "Exiting."
                << std::endl;
    }
    return;
  }


  void manager::_preload_global_dict() {
    bool devel = false;
    if (devel) {
      std::clog << datatools::io::devel
                << "genbb::manager::_preload_global_dict: "
                << "Entering..."
                << std::endl;
    }
    _factory_register_.import(DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(genbb::i_genbb));
    return;
  }


  void manager::_create_pg(detail::pg_entry_type& entry) {
    if (!entry.is_created()) {
      if (this->is_debug()) {
        std::clog << datatools::io::debug
                  << "genbb::manager::_create_pg: "
                  << "Creating particle generator named '"
                  <<  entry.get_name()
                  << "'..."
                  << std::endl;
      }
      detail::create(entry,
                     &_factory_register_,
                     &grab_prng());
      //(has_external_prng() ? &grab_external_prng() : 0));
    }
    return;
  }


  void manager::_initialize_pg(detail::pg_entry_type& entry) {
    // If not created, first do it :
    if (!entry.is_created()) {
      this->_create_pg(entry);
    }

    // If not initialized, do it :
    if (!entry.is_initialized()) {
      if (this->is_debug()) {
        std::clog << datatools::io::debug
                  << "genbb::manager::_initialize_pg: "
                  << "Initializing particle generator named '"
                  <<  entry.get_name()
                  << "'..."
                  << std::endl;
      }
      detail::initialize(entry,
                         (has_service_manager() ? _service_mgr_ : 0),
                         &_particle_generators_,
                         &_factory_register_,
                         &grab_prng());
      // (has_external_prng()? &grab_external_prng() : 0));
    }
    return;
  }


  void manager::_reset_pg(detail::pg_entry_type& entry) {
    detail::reset(entry);
    return;
  }


  //----------------------------------------------------------------------
  // Private Interface Definitions

  void manager::_set_preload_(bool preload) {
    _preload_ = preload;
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
  ocd_.set_class_name ("genbb::manager");
  ocd_.set_class_description ("A generic manager for event generators");
  ocd_.set_class_library("genbb_help");
  //ocd_.set_class_documentation("blah-blah...");

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("debug")
      .set_terse_description("Flag to activate debugging output")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("Superseded by a former call of :              \n"
                            "  genbb::manager::set_debug(true)             \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("seed")
      .set_terse_description("Embeded PRNG's seed")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(false)
      .set_complex_triggering_conditions(true)
      .set_long_description("The seed of the embeded PRNG.                 \n"
                            "Not used if some external PRNG is used.       \n"
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
                            "Example:                                                    \n"
                            "  |                                                         \n"
                            "  | generators.configuration_files : string[1] as path = \\ \n"
                            "  |   \"${CONFIG_REPOSITORY_DIR}/eg.conf\"                  \n"
                            "  |                                                         \n"
                            "The target files must use the format of the                 \n"
                            "'datatools::multi_properties' class.                        \n"
                            "The loading order of the files is critical                  \n"
                            "because some generators may depend on other ones            \n"
                            "which should thus be defined *before* their                 \n"
                            "dependers.                                                  \n"
                          )
      ;
  }

  //ocd_.set_configuration_hints ("Nothing special.");
  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()

DOCD_CLASS_SYSTEM_REGISTRATION(genbb::manager,"genbb::manager")

// end of manager.cc

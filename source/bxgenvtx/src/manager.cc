/* manager.cc
 *
 * Copyright 2007-2013 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

// Ourselves:
#include <genvtx/manager.h>

// Standard library:
#include <ctime>
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <list>

// Third party:
// - Boost
#include <boost/type_traits.hpp>
// - Bayeux/datatools
#include <datatools/service_manager.h>
#include <datatools/exception.h>
// - Bayeux/geomtools
#include <geomtools/manager.h>
// // - Bayeux/cuts
// #include <cuts/cut_manager.h>

// This project:
#include <genvtx/not_in_daughters_vertex_validator.h>

namespace genvtx {

  datatools::logger::priority
  manager::get_logging_priority() const
  {
    return _logging_priority;
  }

  void manager::set_logging_priority(datatools::logger::priority p_)
  {
    _logging_priority = p_;
  }

  bool manager::is_initialized() const
  {
    return _initialized_;
  }

  bool manager::is_debug() const
  {
    return _logging_priority >= datatools::logger::PRIO_DEBUG;
  }

  void manager::set_debug(bool debug_)
  {
    if (debug_) {
      _logging_priority = datatools::logger::PRIO_DEBUG;
    } else {
      _logging_priority = datatools::logger::PRIO_FATAL;
    }
    return;
  }

  const std::string & manager::get_generator_name() const
  {
    return _current_vg_name_;
  }

  bool manager::has_generator_name() const
  {
    return ! _current_vg_name_.empty();
  }

  bool manager::has_external_random() const
  {
    return _external_random_ != 0;
  }

  void manager::set_external_random(mygsl::rng & prng_)
  {
    _external_random_ = &prng_;
    return;
  }

  bool manager::has_random_seed() const
  {
    return _random_seed_ < mygsl::random_utils::SEED_INVALID;
  }

  void manager::set_random_seed(int rseed_)
  {
    DT_THROW_IF (_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
    if (rseed_ < 0) {
      _random_seed_ = mygsl::random_utils::SEED_INVALID;
    } else {
      _random_seed_ = rseed_;
    }
    return;
  }

  int manager::get_random_seed() const
  {
    return _random_seed_;
  }

  void manager::set_rng_id(const std::string & rng_id_)
  {
    DT_THROW_IF (_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
    _rng_id_ = rng_id_;
    return;
  }

  const std::string & manager::get_rng_id() const
  {
    return _rng_id_;
  }

  const mygsl::rng & manager::get_random() const
  {
    if (_external_random_ != 0) {
      return *_external_random_;
    }
    return _random_;
  }

  mygsl::rng & manager::grab_random()
  {
    if (_external_random_ != 0) {
      return *_external_random_;
    }
    return _random_;
  }

  bool manager::has_service_manager() const
  {
    return _service_manager_ != 0;
  }

  void manager::set_service_manager(datatools::service_manager & sm_)
  {
    DT_THROW_IF (_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
    _service_manager_ = &sm_;
    return;
  }

  datatools::service_manager & manager::grab_service_manager()
  {
    DT_THROW_IF (_service_manager_ == 0, std::logic_error, "Missing service manager !");
    return *_service_manager_;
  }

  const datatools::service_manager & manager::get_service_manager() const
  {
    DT_THROW_IF (_service_manager_ == 0, std::logic_error, "Missing service manager !");
    return *_service_manager_;
  }

  bool manager::has_geometry_manager() const
  {
    return _geometry_manager_ != 0;
  }

  void manager::set_geometry_manager(const geomtools::manager & gm_)
  {
    DT_THROW_IF (_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
    _geometry_manager_ = &gm_;
    return;
  }

  const geomtools::manager & manager::get_geometry_manager() const
  {
    DT_THROW_IF (_geometry_manager_ == 0, std::logic_error, "Missing geometry manager !");
    return *_geometry_manager_;
  }

  void manager::set_generator_name(const std::string & gen_name_)
  {
    DT_THROW_IF (_initialized_, std::logic_error, "Operation prohibited ! Manager is locked !");
    _current_vg_name_ = gen_name_;
    return;
  }

  void manager::vg_names(std::vector<std::string> & names_,
                         std::vector<std::string> & descriptions_,
                         std::string & current_) const
  {
    names_.clear();
    descriptions_.clear();
    current_.clear();
    names_.reserve(_vg_store_.size());
    descriptions_.reserve(_vg_store_.size());
    for(genvtx::vg_dict_type::const_iterator i = _vg_store_.begin();
         i != _vg_store_.end();
         i++) {
      const std::string & vg_name = i->first;
      const genvtx::vg_entry_type & the_vg_entry = i->second;
      const datatools::properties & vg_conf = the_vg_entry.get_vg_config();
      const std::string & vg_description = vg_conf.get_description();
      names_.push_back(vg_name);
      descriptions_.push_back(vg_description);
    }
    current_ = _current_vg_name_;
  }

  // Constructor :
  manager::manager(datatools::logger::priority p_)
  {
    _logging_priority = p_;
    _initialized_ = false;

    _rng_id_ = "taus2";
    _random_seed_ = mygsl::random_utils::SEED_INVALID;
    _service_manager_  = 0;
    _geometry_manager_ = 0;
    _external_random_  = 0;
    _current_vg_name_ = "";

    _factory_preload_ = true;

    return;
  }

  // Destructor :
  manager::~manager()
  {
    if (_initialized_) {
      reset();
    }
    return;
  }

  void manager::init(const datatools::properties & config_)
  {
    this->initialize(config_);
    return;
  }

  void manager::initialize(const datatools::properties & config_)
  {
    DT_THROW_IF (_initialized_, std::logic_error, "Manager is already initialized !");
    std::string rng_id;
    std::string vg_name; // vertex generator name
    std::vector<std::string> vg_list;

    {
      // Logging priority:
      datatools::logger::priority lp
        = datatools::logger::extract_logging_configuration(config_,
                                                           datatools::logger::PRIO_FATAL);
      DT_THROW_IF (lp ==  datatools::logger::PRIO_UNDEFINED,
                  std::logic_error,
                  "Invalid logging priority !");
      set_logging_priority(lp);
    }

    if (config_.has_key("factory.logging.priority")) {
      std::string prio_label = config_.fetch_string("factory.logging.priority");
      datatools::logger::priority p = datatools::logger::get_priority(prio_label);
      DT_THROW_IF(p == datatools::logger::PRIO_UNDEFINED,
                  std::domain_error,
                  "Unknow logging priority ``" << prio_label << "`` !");
      _factory_register_.set_logging_priority(p);
    }

    if (config_.has_flag("factory.no_preload")) {
      _factory_preload_ = false;
    }

    // 2011-02-25 FM : only search for 'seed' property if 'random seed' is not set :
    if (! has_random_seed()) {
      if (config_.has_key("prng_seed")) {
        int rseed = config_.fetch_integer("prng_seed");
        set_random_seed(rseed);
      }
    }

    if (config_.has_key("prng_id")) {
      rng_id = config_.fetch_string("prng_id");
      set_rng_id(rng_id);
    }

    if (config_.has_key("vg_list")) {
      config_.fetch("vg_list", vg_list);
    }

    _factory_register_.set_label("genvtx::manager/factory");
    if (_factory_preload_) {
      _factory_register_.import(DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(::genvtx::i_vertex_generator));
    }

    if (! has_generator_name()) {
      if (config_.has_key("generator")) {
        vg_name = config_.fetch_string("generator");
      }
      set_generator_name(vg_name);
    }

    // Initialization sequence :

    for(std::vector<std::string>::const_iterator i = vg_list.begin();
         i != vg_list.end();
         i++) {
      std::string filename = *i;
      datatools::fetch_path_with_env(filename);
      datatools::multi_properties configs;
      DT_LOG_NOTICE(get_logging_priority(),
                    "Loading vertex generators from file '"
                    << filename << "'...");
      datatools::read_config(filename, configs);
      _load_vgs(configs);
    }

    if (_external_random_ == 0) {
      _random_.init(_rng_id_, _random_seed_);
    }

    // If available, try to activate the current VG for its name :
    if (! _current_vg_name_.empty()) {
      activate_current_vg();
    }

    // // Vertex validator manager:
    // _vv_manager_.reset(new cuts::cut_manager(cuts::cut_manager::FACTORY_NOPRELOAD));
    // _vv_manager_->register_cut_type<genvtx::not_in_daughters_vertex_validator>("genvtx::not_in_daughters_vertex_validator");

    _initialized_ = true;
    return;
  }

  void manager::create_vg(genvtx::vg_entry_type & vg_entry_)
  {
    if (! vg_entry_.is_created()) {
      this->_create_vg(vg_entry_);
    }
    return;
  }

  void manager::_create_vg(genvtx::vg_entry_type & vg_entry_)
  {
    if (vg_entry_.is_created()) {
      return;
    }

    // Search for the vertex generator's id in the factory dictionary:
    DT_THROW_IF (! _factory_register_.has(vg_entry_.get_vg_id()),
                 std::logic_error,
                 "Cannot find vertex generator factory with ID '"
                 << vg_entry_.get_vg_id() << "' for vg named '"
                 << vg_entry_.get_vg_name() << "' !");
    const genvtx::i_vertex_generator::factory_register_type::factory_type & the_factory
      = _factory_register_.get(vg_entry_.get_vg_id());

    genvtx::i_vertex_generator * vg_ptr = the_factory();
    DT_THROW_IF (vg_ptr == 0,
                 std::logic_error,
                 "Creation of '" << vg_entry_.get_vg_name() << "' vertex generator of type '"
                 << vg_entry_.get_vg_id() << "' failed !");
    vg_entry_.set_ptr(vg_ptr);
    vg_entry_.set_created();
    // Force the embedded vertex generator's name :
    vg_ptr->set_name(vg_entry_.get_vg_name());

    DT_LOG_NOTICE(get_logging_priority(),"Vertex generator named '"
                  <<  vg_entry_.get_vg_name()
                  << "' has been created !");
    return;
  }


  void manager::initialize_vg(genvtx::vg_entry_type & vg_entry_)
  {
    if (! vg_entry_.is_initialized()) {
      _initialize_vg(vg_entry_);
    }
    return;
  }

  void manager::_initialize_vg(genvtx::vg_entry_type & vg_entry_)
  {
    // If not created, first do it :
    if (! vg_entry_.is_created()) {
      _create_vg(vg_entry_);
    }

    // If not initialized, do it :
    if (! vg_entry_.is_initialized()) {
      DT_LOG_DEBUG(get_logging_priority(),
                   "Initializing vg named '"
                   <<  vg_entry_.get_vg_name()
                   << "'...");
      genvtx::i_vertex_generator & the_vg = vg_entry_.grab_vg();
      if (get_logging_priority() >= datatools::logger::PRIO_DEBUG) {
        the_vg.set_debug(true);
      }
      if (has_geometry_manager()) {
        the_vg.set_geom_manager(get_geometry_manager());
      }
      if (has_service_manager()) {
        the_vg.initialize(vg_entry_.get_vg_config(),
                           grab_service_manager(),
                           _vg_store_);
      } else {
        the_vg.initialize_with_dictionary_only(vg_entry_.get_vg_config(),
                                                _vg_store_);
      }
      // // check covariance for specific SuperNEMO VGs :
      // snemo::genvertex::base_vg * bvg = dynamic_cast< snemo::genvertex::base_vg*>(&the_vg);
      // if (bvg != 0)
      //   {
      //     // Apply them a specific geometry manager if any:
      //     if (has_geometry_manager())
      //       {
      //         bvg->set_geom_manager(get_geometry_manager());
      //       }
      //   }
      // the_vg.initialize_with_dictionary_only(vg_entry_.get_vg_config(), _vg_store_);
      vg_entry_.set_initialized();
    }
    return;
  }

  void manager::_reset_vg(genvtx::vg_entry_type & vg_entry_)
  {
    if (vg_entry_.is_initialized()) {
      genvtx::i_vertex_generator & the_vg = vg_entry_.grab_vg();
      the_vg.reset();
      vg_entry_.set_uninitialized();
    }
    return;
  }


  void manager::_load_vg(const std::string & vg_name_,
                          const std::string & vg_id_,
                          const datatools::properties & vg_config_)
  {
    // bool debug = is_debug();
    DT_LOG_TRACE(get_logging_priority(), "Entering...");

    DT_THROW_IF (has_generator(vg_name_),
                 std::logic_error, "Vertex generator '" << vg_name_ << "' already exists !");

    {
      // Add a new entry :
      genvtx::vg_entry_type dummy_vg_entry;
      DT_LOG_DEBUG(get_logging_priority(),
                   "Add an entry for vertex generator '" << vg_name_ << "'...");
      _vg_store_[vg_name_] = dummy_vg_entry;
    }

    // fetch a reference on it and update its internal values :
    genvtx::vg_entry_type & the_vg_entry = _vg_store_.find(vg_name_)->second;
    the_vg_entry.set_vg_name(vg_name_);
    the_vg_entry.set_manager(*this);
    the_vg_entry.set_vg_id(vg_id_);
    the_vg_entry.set_vg_config(vg_config_);
    the_vg_entry.set_blank();
    DT_LOG_DEBUG(get_logging_priority(), "Fetch...");

    bool force_initialization_at_load = false;

    if (vg_config_.has_flag("force_initialization_at_load")) {
      force_initialization_at_load = true;
    }

    if (force_initialization_at_load) {
      _initialize_vg(the_vg_entry);
    }

    DT_LOG_TRACE(get_logging_priority(),"Exiting.");
    return;
  }

  void manager::load_vg(const std::string & vg_name_,
                         const std::string & vg_id_,
                         const datatools::properties & vg_config_)
  {
    DT_THROW_IF (is_initialized(),
                 std::logic_error,
                 "VG manager named '" << vg_name_ << "' is already initialized !");
    _load_vg(vg_name_, vg_id_, vg_config_);
    return;
  }

  void manager::_load_vgs(const datatools::multi_properties & config_)
  {
    using namespace datatools;
    const multi_properties::entries_ordered_col_type & oe = config_.ordered_entries();
    for(multi_properties::entries_ordered_col_type::const_iterator i =
           oe.begin();
         i != oe.end();
         ++i) {
      const multi_properties::entry & the_entry = **i;
      const std::string & vg_name = the_entry.get_key();
      DT_LOG_NOTICE(get_logging_priority(),
                    "Loading vgs '" << vg_name << "'...");
      DT_THROW_IF (has_generator(vg_name),
                   std::logic_error,
                   "Vertex generator manager already has a vertex generator named '"
                   << vg_name << "' !");
      const std::string & vg_id = the_entry.get_meta();
      const properties & vg_config = the_entry.get_properties();
      _load_vg(vg_name, vg_id, vg_config);
    }
    return;
  }

  genvtx::i_vertex_generator &
  manager::grab(const std::string & vg_name_)
  {
    genvtx::vg_dict_type::iterator found = _vg_store_.find(vg_name_);
    DT_THROW_IF (found == _vg_store_.end(),
                 std::logic_error,
                 "No vertex generator named '" << vg_name_ << "' !");
    if (! found->second.is_initialized()) {
      _initialize_vg(found->second);
    }
    return found->second.grab_vg();
  }

  const genvtx::i_vertex_generator &
  manager::get(const std::string & vg_name_) const
  {
    manager * mutable_this = const_cast<manager*>(this);
    return const_cast<genvtx::i_vertex_generator &>(mutable_this->grab(vg_name_));
  }

  void manager::reset()
  {
    DT_THROW_IF (! _initialized_, std::logic_error, "Not initialized !");
    _initialized_ = false;
    _current_vg_.reset();
    _current_vg_name_ = "";
    _vg_store_.clear();
    _random_.reset();
    _random_seed_ = mygsl::random_utils::SEED_INVALID;
    _rng_id_ = "taus2";
    _service_manager_ = 0;
    _geometry_manager_ = 0;
    _external_random_ = 0;
    // if (_vv_manager_) {
    //   _vv_manager_.reset();
    // }
    return;
  }

  bool manager::has_generator(const std::string & vg_name_) const
  {
    return _vg_store_.find(vg_name_) != _vg_store_.end();
  }

  genvtx::vg_handle_type manager::grab_generator(const std::string & vg_name_)
  {
    genvtx::vg_dict_type::iterator found = _vg_store_.find(_current_vg_name_);
    DT_THROW_IF (found == _vg_store_.end(),
                 std::logic_error,
                 "Cannot find the '" << vg_name_ << "' from the dictionnary of vertex generators !");
    return found->second.grab_initialized_vg_handle();
  }

  bool manager::has_current_vg() const
  {
    return _current_vg_ && _current_vg_.get().is_initialized();
  }

  void manager::activate_current_vg(const std::string & vg_name_)
  {
    std::string vg_name;
    if (vg_name_.empty()) {
      vg_name = _current_vg_name_;
    } else {
      vg_name = vg_name_;
    }
    DT_THROW_IF (vg_name.empty(),
                 std::logic_error,
                 "Empty vertex generator name !");
    DT_THROW_IF (! has_generator(vg_name),
                 std::logic_error,
                 "Cannot find the '" << vg_name << "' from the dictionnary of vertex generators !");
    _current_vg_name_ = vg_name;
    _current_vg_ = grab_generator(_current_vg_name_);
    return;
  }

  void manager::desactivate_current_vg()
  {
    if (_current_vg_) {
      _current_vg_.reset();
    }
    return;
  }

  void manager::tree_dump(std::ostream & out_,
                           const std::string & title_,
                           const std::string & indent_,
                           bool inherit_) const
  {
    namespace du = datatools;
    std::string indent;
    if (! indent_.empty()) indent = indent_;
    if (! title_.empty()) {
      out_ << indent << title_ << std::endl;
    }

    out_ << indent << du::i_tree_dumpable::tag
         << "Initialized  : " << (_initialized_? "Yes": "No") << std::endl;

    out_ << indent << du::i_tree_dumpable::tag
         << "Logging priority : \"" << datatools::logger::get_priority_label(_logging_priority) << '"' << std::endl;

    out_ << indent << du::i_tree_dumpable::tag
         << "Debug VG     : " << (_debug_vg_? "Yes": "No") << std::endl;

    out_ << indent << du::i_tree_dumpable::tag
         << "Random seed  : " << _random_seed_ << std::endl;

    out_ << indent << du::i_tree_dumpable::tag
         << "Random ID    : " << _rng_id_ << std::endl;

    out_ << indent << du::i_tree_dumpable::tag
         << "Service manager  : " << _service_manager_ << "'" << std::endl;

    out_ << indent << du::i_tree_dumpable::tag
         << "Geometry manager : " << _geometry_manager_ << "'" << std::endl;

    {
      out_ << indent << du::i_tree_dumpable::tag
           << "Vertex generators : " << std::endl;
      for(genvtx::vg_dict_type::const_iterator i = _vg_store_.begin();
           i != _vg_store_.end();
           i++) {
        genvtx::vg_dict_type::const_iterator j = i;
        j++;
        out_ << indent << du::i_tree_dumpable::skip_tag;
        if (j == _vg_store_.end()) {
          out_ << du::i_tree_dumpable::last_tag;
        }
        else {
          out_ << du::i_tree_dumpable::tag;
        }
        const std::string & vg_name = i->first;
        const vg_entry_type & vge = i->second;
        out_ << "Name : '" << vg_name << "' of type='" << vge.get_vg_id() << "' [status="
             <<(is_initialized()? "initialized": "-") << "]";
        out_ << std::endl;
      }
    }

    out_ << indent << du::i_tree_dumpable::tag
         << "Current vertex generator name : '" << _current_vg_name_ << "'" << std::endl;

    out_ << indent << du::i_tree_dumpable::tag
         << "Can shoot vertex    : " << can_shoot_vertex() << "" << std::endl;

    {
      out_ << indent << du::i_tree_dumpable::inherit_tag(inherit_)
           << "Factory register  : " << std::endl;
      {
        std::ostringstream indent2;
        indent2 << indent << du::i_tree_dumpable::inherit_skip_tag(inherit_);
        _factory_register_.tree_dump(out_, "", indent2.str());
      }
    }

    return;
  }

  bool manager::can_shoot_vertex() const
  {
    return is_initialized()
      && has_current_vg()
      && _current_vg_.get().has_next_vertex();
  }

  bool manager::is_time_generator() const
  {
    if (!can_shoot_vertex()) {
      return false;
    }
    return _current_vg_.get().is_time_generator();
  }

  void manager::shoot_vertex(geomtools::vector_3d & vertex_)
  {
    DT_THROW_IF (! is_initialized(),
                 std::logic_error, "Generator has not been locked !");
    if (! has_current_vg()) {
      activate_current_vg();
    }
    _current_vg_.grab().shoot_vertex(_random_, vertex_);
    return;
  }

  void manager::shoot_vertex_and_time(geomtools::vector_3d & vertex_, double & time_)
  {
    DT_THROW_IF (! is_initialized(),
                 std::logic_error, "Generator has not been locked !");
    if (! has_current_vg()) {
      activate_current_vg();
    }
    DT_THROW_IF(!_current_vg_.get().is_time_generator(),
                std::logic_error,
                "Current generator cannot generate time associated to vertex!");
    _current_vg_.grab().shoot_vertex_and_time(_random_, vertex_, time_);
    return;
  }

} // end of namespace genvtx

/***************
 * OCD support *
 ***************/

#include <datatools/ocd_macros.h>

// OCD support' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::genvtx::manager,ocd_)
{
  ocd_.set_class_name("genvtx::manager");
  ocd_.set_class_description("A generic manager for vertex generators");
  ocd_.set_class_library("genvtx");
  ocd_.set_class_documentation("The vertex generator manager is responsible of the creation, \n"
                               "initialization, publishing and finally destruction           \n"
                               "of some collection of vertex generator inherited from        \n"
                               "the ``genvtx::i_vertex_generator``.                          \n"
                               );

  datatools::logger::declare_ocd_logging_configuration(ocd_,
                                                       "fatal",
                                                       "",
                                                       "genvtx::manager");

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("factory.debug")
      .set_terse_description("Flag to activate debugging output for vertex generators factory")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("This flag activates debugging output dedicated to \n"
                            "the embeded vertex generators factory.            \n"
                            "It is not recommended for a production run.       \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("factory.no_preload")
      .set_terse_description("Flag to inhibit the preloading of registered vertex generator factories")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("This flag inhibits the preloading of registered vertex \n"
                            "generator factories at manager's initialization.       \n"
                            "It is not recommended for a normal usage.              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("prng_seed")
      .set_terse_description("The seed of the embeded PRNG")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(false)
      .set_complex_triggering_conditions(true)
      .set_long_description("This sets the seed of the embedded PRNG. This property   \n"
                            "is only used if no external PRNG is used by the manager. \n"
                            "The seed value must be a positive integer.               \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("prng_id")
      .set_terse_description("The type of the embedded PRNG")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_complex_triggering_conditions(true)
      .set_long_description("This sets the type of the embedded PRNG. This property   \n"
                            "is only used if no external PRNG is used by the manager. \n"
                            "The type is chosen from the list of PRNG defined in GSL. \n"
                            "Default value is 'taus2'.                                \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("vg_list")
      .set_terse_description("The list of files that define vertex generators' build rules")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_path(true)
      .set_mandatory(false)
      .set_long_description("This sets the list of files where vertex generators are       \n"
                            "defined. Each filename can contain some environment variable. \n"
                            )
      .add_example("Use a single definition file ::                               \n"
                   "                                                              \n"
                   "  vg_list : string[1] as path = \"${VG_CONF_DIR}/vg.conf\"    \n"
                   "                                                              \n"
                   "Each file must follow the 'datatools::multi_properties'       \n"
                   "ASCII file syntax ::                                          \n"
                   "                                                              \n"
                   "  #@description A list of vertex generators                   \n"
                   "  #@key_label   \"name\"                                      \n"
                   "  #@meta_label  \"type\"                                      \n"
                   "                                                              \n"
                   "  [name=\"from_everywhere.vg\" type=\"genvtx::box_model_vg\"] \n"
                   "  debug  : boolean = 0                                        \n"
                   "  origin : string = \" category='lab.gc' \"                   \n"
                   "  mode   : string  = \"bulk\"                                 \n"
                   "                                                              \n"
                   "  [name=\"from_the_roof.vg\" type=\"genvtx::box_model_vg\"]   \n"
                   "  debug  : boolean = 0                                        \n"
                   "  origin : string = \" category='lab.gc' \"                   \n"
                   "  mode   : string  = \"surface\"                              \n"
                   "  mode.surface.top : boolean = 1                              \n"
                   )
      ;
  }


  //ocd_.set_configuration_hints("Nothing special.");
  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()

DOCD_CLASS_SYSTEM_REGISTRATION(genvtx::manager,"genvtx::manager")

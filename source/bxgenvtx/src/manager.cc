// -*- mode: c++ ; -*-
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

#include <ctime>
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <list>

#include <boost/type_traits.hpp>

#include <geomtools/manager.h>

#include <datatools/service_manager.h>

#include <genvtx/manager.h>


namespace genvtx {

  bool manager::is_initialized () const
  {
    return _initialized_;
  }

  bool manager::is_debug () const
  {
    return _debug_;
  }

  void manager::set_debug (bool new_value_)
  {
    _debug_ = new_value_;
    return;
  }

  bool manager::is_debug_vg () const
  {
    return _debug_vg_;
  }

  void manager::set_debug_vg (bool new_value_)
  {
    _debug_vg_ = new_value_;
    return;
  }

  bool manager::is_factory_debug () const
  {
    return _factory_debug_;
  }

  void manager::set_factory_debug (bool new_value_)
  {
    _factory_debug_ = new_value_;
    return;
  }

  const std::string & manager::get_generator_name () const
  {
    return _current_vg_name_;
  }

  bool manager::has_generator_name () const
  {
    return ! _current_vg_name_.empty ();
  }

  bool manager::has_external_random () const
  {
    return _external_random_ != 0;
  }

  void manager::set_external_random (mygsl::rng & prng_)
  {
    _external_random_ = &prng_;
    return;
  }
 
  bool manager::has_random_seed () const
  {
    return _random_seed_ < mygsl::random_utils::SEED_INVALID;
  }

  void manager::set_random_seed (int rseed_)
  {
    if (_initialized_)
      {
        throw std::logic_error ("genvtx::manager::set_random_seed: Operation prohibited ! Manager is locked !");
      }
    if (rseed_ < 0)
      {
        _random_seed_ = mygsl::random_utils::SEED_INVALID;
      }
    else
      {
        _random_seed_ = rseed_;
      }
    return;
  }

  int manager::get_random_seed () const
  {
    return _random_seed_;
  }

  void manager::set_rng_id (const std::string & rng_id_)
  {
    if (_initialized_)
      {
        throw std::logic_error ("manager::set_rng_id: Manager is locked ! Operation is not allowed !");
      }
    _rng_id_ = rng_id_;
    return;
  }

  const std::string & manager::get_rng_id () const
  {
    return _rng_id_;
  }

  const mygsl::rng & manager::get_random () const
  {
    if (_external_random_ != 0)
      {
        return *_external_random_;
      }
    return _random_;
  }

  mygsl::rng & manager::grab_random ()
  {
    if (_external_random_ != 0)
      {
        return *_external_random_;
      }
    return _random_;
  }

  bool manager::has_service_manager () const
  {
    return _service_manager_ != 0;
  }

  void manager::set_service_manager (datatools::service_manager & sm_)
  {
    if (_initialized_)
      {
        throw std::logic_error ("genvtx::manager::set_service_manager: Manager is locked ! Operation is not allowed !");
      }
    _service_manager_ = &sm_;
    return;
  }

  datatools::service_manager & manager::grab_service_manager ()
  {
    if (_service_manager_ == 0)
      {
        throw std::logic_error ("genvtx::manager::grab_service_manager: Missing service manager !");
      }
    return *_service_manager_;
  }

  const datatools::service_manager & manager::get_service_manager () const
  {
    if (_service_manager_ == 0)
      {
        throw std::logic_error ("genvtx::manager::get_service_manager: Missing service manager !");
      }
    return *_service_manager_;
  }

  bool manager::has_geometry_manager () const
  {
    return _geometry_manager_ != 0;
  }

  void manager::set_geometry_manager (const geomtools::manager & gm_)
  {
    if (_initialized_)
      {
        throw std::logic_error ("genvtx::manager::set_geometry_manager: Manager is locked ! Operation is not allowed !");
      }
    _geometry_manager_ = &gm_;
    return;
  }

  const geomtools::manager & manager::get_geometry_manager () const
  {
    if (_geometry_manager_ == 0)
      {
        throw std::logic_error ("genvtx::manager::get_geometry_manager: Missing geometry manager !");
      }
    return *_geometry_manager_;
  }

  void manager::set_generator_name (const std::string & gen_name_)
  {
    if (_initialized_)
      {
        throw std::logic_error ("genvtx::manager::set_generator_name: Operation prohibited ! Manager is locked !");
      }
    _current_vg_name_ = gen_name_;
    return;
  }

  void manager::set_vg_name (const std::string & vg_name_)
  {
    set_generator_name (_current_vg_name_);
    return;
  }

  const std::string & manager::get_vg_name () const
  {
    return get_generator_name ();
  }

  // Constructor :
  manager::manager (bool debug_, bool debug_vg_, bool verbose_factory_)
  {
    _debug_ = debug_;
    _initialized_ = false;


    _rng_id_ = "taus2";
    _random_seed_ = mygsl::random_utils::SEED_INVALID;
    _service_manager_ = 0;
    _geometry_manager_ = 0;
    _external_random_ = 0;

    _debug_vg_ = debug_vg_;
    _current_vg_name_ = "";

    _factory_preload_ = true;
    _factory_debug_ = verbose_factory_;

    return;
  }

  // Destructor :
  manager::~manager ()
  {
    if (_initialized_)
      {
        reset ();
      }
    return;
  }

  void manager::init (const datatools::properties & config_)
  {
    this->initialize (config_);
    return;
  }

  void manager::initialize (const datatools::properties & config_)
  {
    if (_initialized_)
      {
        throw std::logic_error ("genvtx::manager::init: Manager is already initialized !");
      }
    std::string rng_id;
    std::string vg_name; // vertex generator name
    std::vector<std::string> vg_list;
    //std::string geom_setup_label;
    //std::string geom_setup_version;
    //std::string geom_setup_description;

    if (config_.has_flag ("debug"))
      {
        set_debug (true);
      }

    if (config_.has_flag ("debug_vg"))
      {
        set_debug_vg (true);
      }

    if (config_.has_flag ("factory.debug"))
      {
        set_factory_debug (true);
      }

    if (config_.has_flag ("factory.no_preload"))
      {
        _factory_preload_ = false;
      }

    // 2011-02-25 FM : only search for 'seed' property if 'random seed' is not set :
    if (! has_random_seed ())
      {
        if (config_.has_key ("prng_seed"))
          {
            int rseed = config_.fetch_integer ("prng_seed");
            set_random_seed (rseed);
          }
      }

    if (config_.has_key ("prng_id"))
      {
        rng_id = config_.fetch_string ("prng_id");
        set_rng_id (rng_id);
      }

    if (config_.has_key ("vg_list"))
      {
        config_.fetch ("vg_list", vg_list);
      }

    _factory_register_.set_label ("genvtx::manager/factory");
    _factory_register_.set_verbose (_factory_debug_);
    if (_factory_preload_)
      {
        _factory_register_.import (DATATOOLS_FACTORY_GET_SYSTEM_REGISTER (::genvtx::i_vertex_generator));
      }

    if (! has_generator_name ())
      {
        if (config_.has_key ("generator"))
          {
            vg_name = config_.fetch_string ("generator");
          }
        // else
        //   {
        //     std::ostringstream message;
        //     message << "genvtx::manager::initialize: "
        //             << "Missing 'generator' property !";
        //     throw std::logic_error (message.str ());
        //   }
        set_generator_name (vg_name);
      }

    // Initialization sequence :

    for (std::vector<std::string>::const_iterator i = vg_list.begin ();
         i != vg_list.end ();
         i++)
      {
        std::string filename = *i;
        datatools::fetch_path_with_env (filename);
        datatools::multi_properties configs;
        std::clog << datatools::io::notice
                  << "genvtx::manager::initialize: "
                  << "Loading vertex generators from file '"
                  << filename << "'..." << std::endl;
        configs.read (filename);
        _load_vgs (configs);
      }

    if (_external_random_ == 0)
      {
        _random_.init (_rng_id_, _random_seed_);
      }

    // If available, try to activate the current VG for its name :
    if (! _current_vg_name_.empty())
      {
        activate_current_vg();
      }

    _initialized_ = true;
    return;
  }

  void manager::create_vg (genvtx::vg_entry_type & vg_entry_)
  {
    if (! vg_entry_.is_created ())
      {
        this->_create_vg (vg_entry_);
      }
    return;
  }

  void manager::_create_vg (genvtx::vg_entry_type & vg_entry_)
  {
    if (vg_entry_.is_created ())
      {
        return;
      }

    if (is_debug ())
      {
        std::clog << datatools::io::debug
                  << "genvtx::manager::_create_vg: "
                  << "Creating vertex generator named '"
                  <<  vg_entry_.get_vg_name ()
                  << "'..."
                  << std::endl;
      }

    // search for the cut's label in the factory dictionary:
    if (! _factory_register_.has (vg_entry_.get_vg_id ()))
      {
        std::ostringstream message;
        message << "genvtx::manager::_create_vg: "
                << "Cannot find vertex generator factory with ID '"
                << vg_entry_.get_vg_id () << "' for vg named '"
                << vg_entry_.get_vg_name () << "' !";
        throw std::logic_error (message.str ());
      }
    const genvtx::i_vertex_generator::factory_register_type::factory_type & the_factory
      = _factory_register_.get (vg_entry_.get_vg_id ());

    genvtx::i_vertex_generator * vg_ptr = the_factory ();
    if (vg_ptr == 0)
      {
        std::ostringstream message;
        message << "genvtx::manager::_create_vg: "
                << "Creation of '" << vg_entry_.get_vg_name () << "' vertex generator of type '"
                << vg_entry_.get_vg_id () << "' failed !";
        throw std::logic_error (message.str ());
      }
    vg_entry_.set_ptr (vg_ptr);
    vg_entry_.set_created ();

    if (is_debug ())
      {
        std::clog << datatools::io::debug
                  << "genvtx::manager::_create_vg: "
                  << "Vertex generator named '"
                  <<  vg_entry_.get_vg_name ()
                  << "' has been created !"
                  << std::endl;
      }

    return;
  }


  void manager::initialize_vg (genvtx::vg_entry_type & vg_entry_)
  {
    if (! vg_entry_.is_initialized ())
      {
        _initialize_vg (vg_entry_);
      }
    return;
  }

  void manager::_initialize_vg (genvtx::vg_entry_type & vg_entry_)
  {
    // If not created, first do it :
    if (! vg_entry_.is_created ())
      {
        _create_vg (vg_entry_);
      }

    // If not initialized, do it :
    if (! vg_entry_.is_initialized ())
      {
        if (is_debug ())
          {
            std::clog << datatools::io::debug
                      << "genvtx::_manager::_initialize_vg: "
                      << "Initializing vg named '"
                      <<  vg_entry_.get_vg_name ()
                      << "'..."
                      << std::endl;
          }
        genvtx::i_vertex_generator & the_vg = vg_entry_.grab_vg ();
        if (is_debug_vg ())
          {
            the_vg.set_debug (true);
          }
        if (has_geometry_manager ())
          {
            the_vg.set_geom_manager (get_geometry_manager ());
          }
        if (has_service_manager ())
          {
            the_vg.initialize (vg_entry_.get_vg_config (),
                               grab_service_manager (),
                               _vg_store_);
          }
        else
          {
            the_vg.initialize_with_dictionary_only (vg_entry_.get_vg_config (),
                                                    _vg_store_);
          }
        // // check covariance for specific SuperNEMO VGs :
        // snemo::genvertex::base_vg * bvg = dynamic_cast< snemo::genvertex::base_vg*>(&the_vg);
        // if (bvg != 0)
        //   {
        //     // Apply them a specific geometry manager if any:
        //     if (has_geometry_manager ())
        //       {
        //         bvg->set_geom_manager (get_geometry_manager ());
        //       }
        //   }
        // the_vg.initialize_with_dictionary_only (vg_entry_.get_vg_config (), _vg_store_);
        vg_entry_.set_initialized ();
      }
    return;
  }

  void manager::_reset_vg (genvtx::vg_entry_type & vg_entry_)
  {
    if (vg_entry_.is_initialized ())
      {
        genvtx::i_vertex_generator & the_vg = vg_entry_.grab_vg ();
        the_vg.reset ();
        vg_entry_.set_uninitialized ();
      }
    return;
  }


  void manager::_load_vg (const std::string & vg_name_,
                          const std::string & vg_id_,
                          const datatools::properties & vg_config_)
  {
    bool debug = is_debug ();
    if (debug)
      {
        std::clog << datatools::io::debug
                  << "genvtx::manager::_load_vg: "
                  << "Entering..."
                  << std::endl;
      }

    if (has_generator (vg_name_))
      {
        std::ostringstream message;
        message << "genvtx::manager::_load_vg: "
                << "Vertex generator '" << vg_name_ << "' already exists !";
        throw std::logic_error (message.str());
      }

    {
      // Add a new entry :
      genvtx::vg_entry_type dummy_vg_entry;
      if (debug)
        {
          std::clog << datatools::io::debug
                    << "genvtx::manager::_load_vg: "
                    << "Add an entry for vertex generator '" << vg_name_ << "'..."
                    << std::endl;
        }
      _vg_store_[vg_name_] = dummy_vg_entry;
    }

    // fetch a reference on it and update its internal values :
    genvtx::vg_entry_type & the_vg_entry = _vg_store_.find (vg_name_)->second;
    the_vg_entry.set_vg_name (vg_name_);
    the_vg_entry.set_manager (*this);
    the_vg_entry.set_vg_id (vg_id_);
    the_vg_entry.set_vg_config (vg_config_);
    the_vg_entry.set_blank ();
    if (debug)
      {
        std::clog << datatools::io::debug
                  << "genvtx::manager::_load_vg: "
                  << "Fetch..."
                  << std::endl;
      }
    //_create_vg (the_vg_entry);

    bool force_initialization_at_load = false;

    if (vg_config_.has_flag ("force_initialization_at_load"))
      {
        force_initialization_at_load = true;
      }

    if (force_initialization_at_load) //|| this->is_initialization_at_load ())
      {
        _initialize_vg (the_vg_entry);
      }

    if (debug)
      {
        std::clog << datatools::io::debug
                  << "genvtx::manager::_load_vg: "
                  << "Exiting."
                  << std::endl;
      }
    return;
  }

  void manager::load_vg (const std::string & vg_name_,
                         const std::string & vg_id_,
                         const datatools::properties & vg_config_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "genvtx::manager::load_vg: "
                << "VG manager is already initialized !";
        throw std::logic_error (message.str ());
      }
    _load_vg (vg_name_, vg_id_, vg_config_);
    return;
  }

  void manager::_load_vgs (const datatools::multi_properties & config_)
  {
    using namespace datatools;
    if (is_debug())
      {
        config_.tree_dump (std::clog,
                           "genvtx::manager::_load_vgs: ",
                           "DEBUG: ");
      }
    const multi_properties::entries_ordered_col_type & oe = config_.ordered_entries ();
    for (multi_properties::entries_ordered_col_type::const_iterator i =
           oe.begin ();
         i != oe.end ();
         ++i)
      {
        const multi_properties::entry & the_entry = **i;
        const std::string & vg_name = the_entry.get_key ();
        std::clog << datatools::io::notice
                  << "genvtx::manager::_load_vgs: "
                  << "Loading vgs '" << vg_name << "'..." << std::endl;
        if (has_generator (vg_name))
          {
            std::ostringstream message;
            message << "genvtx::manager::load_vgs_: "
                    << "Vertex generator manager already has a vertex generator named '"
                    << vg_name << "' !";
            throw std::logic_error (message.str ());
          }
        const std::string & vg_id = the_entry.get_meta ();
        const properties & vg_config = the_entry.get_properties ();
        _load_vg (vg_name, vg_id, vg_config);
      }
    return;
  }

  genvtx::i_vertex_generator &
  manager::grab (const std::string & vg_name_)
  {
    genvtx::vg_dict_type::iterator found = _vg_store_.find (vg_name_);
    if (found == _vg_store_.end ())
      {
        std::ostringstream message;
        message << "genvtx::manager::grab: "
                << "No vertex generator named '" << vg_name_ << "' !";
        throw std::logic_error (message.str ());
      }
    /*
    std::clog << "DEVEL: " << "snemo::core::processing::vg_manager::grab: "
              << "Vertex generator '" << vg_name_ << "' initialization status '" << found->second.is_initialized () << "'"
              << std::endl;
    */
    if (! found->second.is_initialized ())
      {
        _initialize_vg (found->second);
      }
    else
      {
        /*
        std::clog << "DEVEL: " << "genvtx::manager::grab: "
                  << "Vertex generator '" << vg_name_ << "' is already initialized."
                  << std::endl;
        */
      }
    return found->second.grab_vg ();
  }

  const genvtx::i_vertex_generator &
  manager::get (const std::string & vg_name_) const
  {
    manager * mutable_this = const_cast<manager*> (this);
    return const_cast<genvtx::i_vertex_generator &> (mutable_this->grab (vg_name_));
  }

  void manager::reset ()
  {
    if (! _initialized_)
      {
        throw std::logic_error ("genvtx::manager::reset: Not initialized !");
      }
    _initialized_ = false;
    _current_vg_.reset ();
    _current_vg_name_ = "";
    _vg_store_.clear ();
    _random_.reset ();
    _random_seed_ = mygsl::random_utils::SEED_INVALID;
    _rng_id_ = "taus2";
    _service_manager_ = 0;
    _geometry_manager_ = 0;
    _external_random_ = 0;
    return;
  }

  bool manager::has_generator (const std::string & vg_name_) const
  {
    return _vg_store_.find (vg_name_) != _vg_store_.end ();
  }

  genvtx::vg_handle_type manager::grab_generator (const std::string & vg_name_)
  {
    genvtx::vg_dict_type::iterator found = _vg_store_.find (_current_vg_name_);
    if (found == _vg_store_.end ())
      {
        std::ostringstream message;
        message << "genvtx::manager::grab_generator: "
                << "Cannot find the '" << vg_name_ << "' from the dictionnary of vertex generators !";
        throw std::logic_error (message.str ());
      }
    return found->second.grab_initialized_vg_handle ();
  }

  bool manager::has_current_vg () const
  {
    return _current_vg_.has_data ()
      && _current_vg_.get ().is_initialized ();
  }

  void manager::activate_current_vg (const std::string & vg_name_)
  {
    std::string vg_name;
    if (vg_name_.empty ())
      {
        vg_name = _current_vg_name_;
      }
    else
      {
        vg_name = vg_name_;
      }
    if (! has_generator (vg_name))
      {
        std::ostringstream message;
        message << "genvtx::manager::activate_current_vg: "
                << "Cannot find the '" << vg_name << "' from the dictionnary of vertex generators !";
        throw std::logic_error (message.str ());
      }
    _current_vg_name_ = vg_name;
    _current_vg_ = grab_generator (_current_vg_name_);
    return;
  }

  void manager::desactivate_current_vg ()
  {
    if (_current_vg_.has_data ())
      {
        _current_vg_.reset ();
      }
    return;
  }

  void manager::tree_dump (std::ostream & out_,
                           const std::string & title_,
                           const std::string & indent_,
                           bool inherit_) const
  {
    namespace du = datatools;
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ())
      {
        out_ << indent << title_ << std::endl;
      }

    out_ << indent << du::i_tree_dumpable::tag
         << "Initialized  : " <<  (_initialized_? "Yes": "No") << std::endl;

    out_ << indent << du::i_tree_dumpable::tag
         << "Debug        : " <<  (_debug_? "Yes": "No") << std::endl;

    out_ << indent << du::i_tree_dumpable::tag
         << "Debug VG     : " <<  (_debug_vg_? "Yes": "No") << std::endl;

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
      for (genvtx::vg_dict_type::const_iterator i = _vg_store_.begin();
           i != _vg_store_.end();
           i++)
        {
          genvtx::vg_dict_type::const_iterator j = i;
          j++;
          out_ << indent << du::i_tree_dumpable::skip_tag;
          if  (j == _vg_store_.end())
            {
              out_ << du::i_tree_dumpable::last_tag;
            }
          else
            {
              out_ << du::i_tree_dumpable::tag;
            }
          const std::string & vg_name = i->first;
          const vg_entry_type & vge = i->second;
          out_ << "Name : '" << vg_name << "' of type='" << vge.get_vg_id () << "' [status="
               << (is_initialized()? "initialized": "-") << "]";
          out_ << std::endl;
        }
    }

    out_ << indent << du::i_tree_dumpable::tag
         << "Current vertex generator name : '" << _current_vg_name_ << "'" << std::endl;
 
    out_ << indent << du::i_tree_dumpable::tag
         << "Can shoot vertex    : " << can_shoot_vertex () << "" << std::endl;

    {
      out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)
           << "Factory register  : " << std::endl;
      {
        std::ostringstream indent2;
        indent2 << indent << du::i_tree_dumpable::inherit_skip_tag (inherit_);
        _factory_register_.tree_dump (out_, "", indent2.str());
      }
    }

    return;
  }

  bool manager::can_shoot_vertex () const
  {
    return is_initialized ()
      && _current_vg_.has_data ()
      && _current_vg_.get ().is_initialized ()
      && _current_vg_.get ().has_next_vertex ();
  }

  void manager::shoot_vertex (geomtools::vector_3d & vertex_)
  {
    if (! is_initialized ())
      {
        throw std::logic_error ("genvtx::manager::shoot_vertex: Generator has not been locked !");
      }
    if (! has_current_vg ())
      {
        activate_current_vg ();
        //throw std::logic_error ("genvtx::manager::shoot_vertex: No vertex generator has been allocated !");
      }
    _current_vg_.grab ().shoot_vertex (_random_, vertex_);
    return;
  }

} // end of namespace genvtx

// end of manager.cc

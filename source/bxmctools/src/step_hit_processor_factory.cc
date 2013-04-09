// -*- mode: c++ ; -*-
/* step_hit_processor_factory.cc
 */

#include <mctools/step_hit_processor_factory.h>

#include <datatools/properties.h>
#include <datatools/service_manager.h>

#include <mygsl/rng.h>

#include <geomtools/manager.h>
#include <geomtools/geometry_service.h>

namespace mctools {

  using namespace std;

  bool step_hit_processor_factory::is_initialized () const
  {
    return _initialized_;
  }

  bool step_hit_processor_factory::is_debug () const
  {
    return _debug_;
  }

  void step_hit_processor_factory::set_debug (bool d_)
  {
    _debug_ = d_;
    return;
  }

  const string & step_hit_processor_factory::get_description () const
  {
    return _description_;
  }

  void step_hit_processor_factory::set_description (const string & description_)
  {
    _description_ = description_;
    return;
  }

  bool step_hit_processor_factory::has_external_prng () const
  {
    return _external_prng_ != 0;
  }

  void step_hit_processor_factory::set_external_prng (mygsl::rng & prng_)
  {
    if (is_initialized()) {
        throw std::logic_error("mctools::step_hit_processor_factory::set_external_prng: Factory is initialized and locked !");
      
    }
    _external_prng_ = &prng_;
    return;
  }
  
  mygsl::rng & step_hit_processor_factory::grab_external_prng()
  {
    if (_external_prng_ == 0)
      {
        throw std::logic_error("mctools::step_hit_processor_factory::grab_external_prng: No external PRNG is referenced !");
      }
    return *_external_prng_;
  }

  bool step_hit_processor_factory::has_geometry_manager () const
  {
    return _geom_manager_ != 0;
  }

  void step_hit_processor_factory::set_geometry_manager (const geomtools::manager & gmgr_)
  {
    if (is_initialized()) {
      throw std::logic_error("mctools::step_hit_processor_factory::set_geometry_manager: Factory is initialized and locked !");     
    }
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
    if (is_initialized()) {
      throw std::logic_error("mctools::step_hit_processor_factory::set_service_manager: Factory is initialized and locked !");     
    }
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

  bool step_hit_processor_factory::has_processor (const string & name_)
  {
    return _processors_.find (name_) != _processors_.end ();
  }

  void step_hit_processor_factory::initialize(const datatools::properties & config_)
  {
    if (is_initialized ()) {
      throw std::logic_error("mctools::step_hit_processor_factory::initialize: Factory is already initialized !");
    }

    if (config_.has_flag("debug")) {
      set_debug(true);
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
          if (! get_service_manager().has(geo_service_label)) {
            std::ostringstream message;
            message << "mctools::step_hit_processor_factory::initialize: "
                    << "No service with name '" << geo_service_label << "' !";
            throw std::logic_error(message.str());
          }
          if (! get_service_manager().is_a<geomtools::geometry_service> ("geo_service_label")) {
            std::ostringstream message;
            message << "mctools::step_hit_processor_factory::initialize: "
                    << "Service with name '" << geo_service_label << "' is not a geometry service !";
            throw std::logic_error(message.str());
          }
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

    for (int i = 0; i < proc_configs.size(); i++) {
      std::string filename = proc_configs[i];
      datatools::fetch_path_with_env(filename);
      datatools::multi_properties mp;
      mp.read(filename);
      load(mp);
    }
    
    _initialized_ = true;
    return;
  }

  void step_hit_processor_factory::reset ()
  {
    if (! is_initialized ()) {
      throw std::logic_error("mctools::step_hit_processor_factory::reset: Factory is not initialized !");
    }
    _initialized_ = false;
    _processors_.clear ();
    _handles_.clear ();
    return;
  }

  // Constructor :
  step_hit_processor_factory::step_hit_processor_factory (bool debug_)
  {
    _initialized_ = false;
    _debug_ = debug_;
 
    _factory_register_.set_label ("mctools::base_step_hit_processor/factory");
    _factory_register_.set_verbose (_debug_);

    bool preload = true;
    if (preload)
      {
        _factory_register_.import (DATATOOLS_FACTORY_GET_SYSTEM_REGISTER (::mctools::base_step_hit_processor));
      }
    return;
  }

  // Destructor :
  step_hit_processor_factory::~step_hit_processor_factory ()
  {
    if (is_initialized()) reset ();
    return;
  }

  const base_step_hit_processor &
  step_hit_processor_factory::get_processor (const string & name_) const
  {
    step_hit_processor_factory * mutable_this = const_cast<step_hit_processor_factory *>(this);
    return const_cast<base_step_hit_processor &>(mutable_this->grab_processor(name_));
  }

  base_step_hit_processor &
  step_hit_processor_factory::grab_processor (const string & name_)
  {
    processor_dict_type::iterator found = _processors_.find (name_);
    if (found == _processors_.end ())
      {
        ostringstream message;
        message << "mctools::step_hit_processor_factory::grab_processor: "
                << "No step hit processor named '" << name_ << "' !";
        throw logic_error (message.str ());
      }
    return *(found->second);
  }

  step_hit_processor_factory::processor_dict_type &
  step_hit_processor_factory::grab_processors ()
  {
    return _processors_;
  }

  const step_hit_processor_factory::processor_dict_type &
  step_hit_processor_factory::get_processors () const
  {
    return _processors_;
  }

  base_step_hit_processor &
  step_hit_processor_factory::create (const string & name_,
                                      const string & type_,
                                      const datatools::properties & config_)
  {
    using namespace datatools;
    bool debug = is_debug ();
    if (is_debug ())
      {
        clog << io::debug
             << "mctools::step_hit_processor_factory::create: "
             << "Entering..." << endl;
      }
    base_step_hit_processor * proc = 0;
    if (name_.empty ())
      {
        ostringstream message;
        message << "mctools::step_hit_processor_factory::create: "
                << "Missing step hit processor name !";
        throw logic_error (message.str ());
      }
    if (type_.empty ())
      {
        ostringstream message;
        message << "mctools::step_hit_processor_factory::create: "
                << "Missing step hit processor type !";
        throw logic_error (message.str ());
      }

    if (has_processor (name_))
      {
        ostringstream message;
        message << "mctools::step_hit_processor_factory::create: "
                << "Step hit processor named '" << name_ << "' already exists !";
        throw logic_error (message.str ());
      }

    if (! _factory_register_.has (type_))
      {
        std::ostringstream message;
        message << "mctools::step_hit_processor_factory::create: " 
                << "No registered step hit processor class with ID '" 
                << type_ << "' for step hit processor named '" << name_ << " !";
        throw std::logic_error (message.str ());
      }

    const base_step_hit_processor::factory_register_type::factory_type & the_factory 
      = _factory_register_.get (type_);
    if (debug)
      {
        std::clog << "DEBUG: snemo::core::processing::step_hit_processor_factory::create: "
                  << "About to create a new step hit processor of type \"" << type_
                  << "\" with name \"" << name_ << "\"..." << std::endl;
      }
    proc = the_factory ();
    if (is_debug ())
      {
        clog << io::debug
             << "mctools::step_hit_processor_factory::create: "
             << "Store the new '" << type_ << "' step hit processor with name '"
             << name_ << "' ..." << endl;
      }
    proc->set_name (name_);
    
    if (proc->accept_external_rng () && has_external_prng ())
      {
        proc->set_external_rng (*_external_prng_); 
      }
    if (is_debug ())
      {
        clog << io::debug
             << "mctools::step_hit_processor_factory::create: "
             << "Initialize the new processor..." << endl;
      } 

    bool accept_geom_manager = true;
    if (config_.has_flag("reject_geometry_manager_from_factory"))
      {
        accept_geom_manager = false;
      }

    if (accept_geom_manager && has_geometry_manager ())
      {
        proc->set_geom_manager(get_geometry_manager ());
      }

    if (has_service_manager())
      {
        proc->initialize (config_, *_service_manager_);
      }
    else
      {
        proc->initialize (config_);
      }
    // Store the new step hit processor in the dictionnary :
    processor_handle_type new_handle (proc);
    _handles_[name_] = new_handle; 
    _processors_[name_] = proc;
    if (is_debug ())
      {
        clog << io::debug
             << "mctools::step_hit_processor_factory::create: "
             << "Exiting." << endl;
      }
    return *proc;
  }

  void step_hit_processor_factory::load (const datatools::multi_properties & mprop_)
  {
    using namespace datatools;
    if (is_debug ())
      {
        clog << io::debug << "mctools::step_hit_processor_factory::load: "
             << "Entering..." << endl;
        mprop_.tree_dump (std::cerr, 
                          "Step hit processors factory rules :" , 
                          "DEBUG: step_hit_processor_factory::load: ");
      }
    for (multi_properties::entries_col_type::const_iterator i
           = mprop_.entries ().begin ();
         i != mprop_.entries ().end ();
         i++)
      {
        const multi_properties::entry & e = i->second;
        const string & name = e.get_key ();
        const string & type = e.get_meta ();
        const properties & config = e.get_properties ();
        if (is_debug ())
          {
            clog << io::debug
                 << "mctools::step_hit_processor_factory::load: "
                 << "Processor name = '" << name << "'" << endl;
            clog << io::debug
                 << "mctools::step_hit_processor_factory::load: "
                 << "Processor type = '" << type << "'" << endl;
          }

        base_step_hit_processor & SHP = this->create (name,
                                                      type,
                                                      config);
      }
    return;
  }

    
  void step_hit_processor_factory::tree_dump (ostream & a_out, 
                                              const string & a_title,
                                              const string & a_indent,
                                              bool a_inherit) const
  {
    string indent;
    if (! a_indent.empty ())
      {
        indent = a_indent;
      }
    if ( ! a_title.empty () ) 
      {
        a_out << indent << a_title << endl;
      }  
      
    namespace du = datatools;

    {
      a_out << indent << du::i_tree_dumpable::tag 
            << "Factory register : " << endl;
      ostringstream indent_ss;
      indent_ss << indent << du::i_tree_dumpable::skip_tag;
      _factory_register_.tree_dump (a_out, "", indent_ss.str ());
    }
    a_out << indent << du::i_tree_dumpable::tag 
          << "Debug       : " << _debug_ << endl;
    a_out << indent << du::i_tree_dumpable::tag 
          << "Description : '" << _description_ << "'" << endl;
    a_out << indent << du::i_tree_dumpable::tag 
          << "Service manager : " << _service_manager_ << endl;
    a_out << indent << du::i_tree_dumpable::tag 
          << "Processor handles  : " << _handles_.size () << endl;
    {
      a_out << indent << du::i_tree_dumpable::tag 
            << "Processors : " << endl;
      for (processor_dict_type::const_iterator i = _processors_.begin ();
           i != _processors_.end ();
           i++)
        {
          a_out << indent << du::i_tree_dumpable::skip_tag;
          processor_dict_type::const_iterator j = i;
          j++;
          ostringstream indent_ss;
          indent_ss << indent << du::i_tree_dumpable::skip_tag;
          if (j == _processors_.end ())
            {
              a_out << du::i_tree_dumpable::last_tag;
              indent_ss << du::i_tree_dumpable::last_skip_tag;
            }
          else
            {
              a_out << du::i_tree_dumpable::tag;
              indent_ss << du::i_tree_dumpable::skip_tag;
            }
          a_out << "Name: '" << i->first << "' address @ " << i->second << endl;
          i->second->tree_dump (a_out, "", indent_ss.str ());
        }
          
    }
    a_out << indent << du::i_tree_dumpable::inherit_tag (a_inherit)  
          << "External PRNG : " << _external_prng_ << endl;;

    return;
  }

  void step_hit_processor_factory::dump (ostream & out_) const
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
   
  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("debug")
      .set_terse_description("The debug flag string")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("This flag triggers debug printing.      \n"
                            "Default value is 0.                     \n"
                            "Not recommended for production run.     \n"
                            "Example :                               \n"
                            "  |                                     \n"
                            "  | debug : boolean : 0                 \n"
                            "  |                                     \n"
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
                            "  mctools::step_hit_processor_factory::set_description(...)     \n"
                            "Example :                                                       \n"
                            "  |                                                             \n"
                            "  | description : string : \"The MC hit processors factory\"    \n"
                            "  |                                                             \n"
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
                            " - a service manager is set in the factory,          \n"
                            " - no geometry manager has been set in the factory.  \n"
                            "Example:                                             \n"
                            "  |                                                  \n"
                            "  | services.geometry : string = \"Geo\"             \n"
                            "  |                                                  \n"
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
                            "embeded MC step hit processors objects. The filenames      \n"
                            "main contain some environment variables.                   \n"
                            "Example:                                                   \n"
                            "  |                                                        \n"
                            "  | processors.configuration : string[1] as path = \\      \n"
                            "  |    \"${CONFIG_REPOSITORY_DIR}/scin_mc_hit_proc.conf\"  \n"
                            "  |                                                        \n"
                            "The target files must use the format of the                \n"
                            "'datatools::multi_properties' class.                       \n"
                            "The loading order of the files is critical                 \n"
                            "because some step hit processors may depend on other       \n"
                            "ones which should thus be defined *before* their           \n"
                            "dependers.                                                 \n"
                            "Extends the instantiation of step hit processors triggered \n"
                            " by former calls to :                                      \n"
                            "  myctools::step_hit_processor_factory::load(...)          \n"
                            )
      ;
  }  

  ocd_.set_configuration_hints ("The MC step hit processor factory uses a 'datatools::properties'\n"
                                "object to initialize its behaviour and contents.                \n"
                                "                                                                \n"
                                "Example of configuration :                                      \n"
                                "  |                                                             \n"
                                "  | debug        : boolean = 0                                  \n"
                                "  | description  : string = \"The MC hit processor factory for Geant4\"\n"
                                "  | services.geometry : string = \"Geo\"                          \n"
                                "  | processors.configuration : string[2] as path = \\             \n"
                                "  |    \"${CONFIG_REPOSITORY_DIR}/telescope_mc_hit_proc.conf\" \\ \n"
                                "  |    \"${CONFIG_REPOSITORY_DIR}/NaI_mc_hit_proc.conf\"          \n"
                                "  |                                                               \n"
                                "Each processor configuration file uses the format of the          \n"
                                "'datatools::multi_properties' class.                              \n"
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

// end of step_hit_processor_factory.cc

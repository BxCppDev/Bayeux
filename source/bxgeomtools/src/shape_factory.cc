// shape_factory.cc

// Ourselves:
#include <geomtools/shape_factory.h>

// Standard library:
#include <fstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

// This project:
#include <geomtools/i_shape_3d.h>

namespace geomtools {

  shape_factory::~shape_factory()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  void shape_factory::keys(std::vector<std::string> & keys_) const
  {
    keys_.clear();
    keys_.reserve(_shapes_.size());
    for (i_object_3d::handle_dict_type::const_iterator i = _shapes_.begin();
         i != _shapes_.end();
         i++) {
      keys_.push_back(i->first);
    }
    return;
  }

  void shape_factory::keys(std::unordered_set<std::string> & keys_) const
  {
    keys_.clear();
    for (i_object_3d::handle_dict_type::const_iterator i = _shapes_.begin();
         i != _shapes_.end();
         i++) {
      keys_.insert(i->first);
    }
    return;
  }

  const i_object_3d::handle_dict_type & shape_factory::get_shapes() const
  {
    return _shapes_;
  }

  std::unique_ptr<i_object_3d> shape_factory::_create(const std::string & type_id_,
                                                      const datatools::properties & config_)
  {
    DT_THROW_IF(! _factory_register_.has(type_id_),
                std::runtime_error,
                "Cannot find any registered 3D object/shape class factory with ID '"
                << type_id_ << "'!");
    const i_object_3d::factory_register_type::factory_type & the_factory
      = _factory_register_.get(type_id_);
    std::unique_ptr<i_object_3d> obj(the_factory());
    obj->initialize(config_, &_shapes_);
    if (obj->get_dimensional() == i_object_3d::DIMENSIONAL_3) {
      i_shape_3d * sh_ptr = dynamic_cast<i_shape_3d*>(obj.get());
      if (!sh_ptr->is_locked()) {
        DT_LOG_NOTICE(get_logging_priority(), "Locking shape of type '" << type_id_ << "'");
        sh_ptr->lock();
      }
    }
    return obj;
  }
 
  std::unique_ptr<i_object_3d> shape_factory::create_unique_ptr(const std::string & type_id_,
                                                                const datatools::properties & config_)
  {
    return _create(type_id_, config_);
  }

  i_object_3d::handle_type shape_factory::create(const std::string & name_,
                                                 const std::string & type_id_,
                                                 const datatools::properties & config_)
  {
    DT_THROW_IF(has(name_), std::logic_error,
                "Object named '" << name_ << "' already exists!");
    i_object_3d::handle_type sp = _create(type_id_, config_);
    _shapes_.emplace(name_, i_object_3d::object_entry(name_, type_id_, config_, sp));
    DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "Shape factory now stores object='" << name_ << "'");
    return sp;
  }

  const i_object_3d & shape_factory::create_reference(const std::string & name_,
                                                      const std::string & type_id_,
                                                      const datatools::properties & config_)
  {
    auto sp = create(name_, type_id_, config_);
    return *sp;
  }

  bool shape_factory::has(const std::string & name_, const std::string & type_id_) const
  {
    i_object_3d::handle_dict_type::const_iterator found = _shapes_.find(name_);
    if (found == _shapes_.end()) {
      return false;
    }
    const i_object_3d::object_entry & oe = found->second;
    if (! oe.has_object()) {
      return false;
    }
    if (! type_id_.empty() && type_id_ != oe.get_type_id()) {
      return false;
    }
    return true;
  }

  const i_object_3d & shape_factory::get(const std::string & name_) const
  {
    i_object_3d::handle_dict_type::const_iterator found = _shapes_.find(name_);
    DT_THROW_IF(found == _shapes_.end(), std::logic_error,
                "No entry named '" << name_ << "' exists in the shape factory!");
    const i_object_3d::object_entry & oe = found->second;
    DT_THROW_IF(! oe.has_object(), std::logic_error,
                "Entry named '" << name_ << "' has no object!");
    return oe.get_object();
  }

  i_object_3d::const_handle_type shape_factory::get_shared_ptr(const std::string & name_) const
  {
    i_object_3d::handle_dict_type::const_iterator found = _shapes_.find(name_);
    DT_THROW_IF(found == _shapes_.end(), std::logic_error,
                "No entry named '" << name_ << "' exists in the shape factory!");
    const i_object_3d::object_entry & oe = found->second;
    return oe.get_handle();
  }

  i_object_3d::handle_type shape_factory::grab_shared_ptr(const std::string & name_)
  {
    i_object_3d::handle_dict_type::iterator found = _shapes_.find(name_);
    DT_THROW_IF(found == _shapes_.end(), std::logic_error,
                "No entry named '" << name_ << "' exists in the shape factory!");
    i_object_3d::object_entry & oe = found->second;
    return oe.grab_handle(); 
  }

  datatools::logger::priority shape_factory::get_logging_priority() const
  {
    return _logging_priority_;
  }

  void shape_factory::set_logging_priority(datatools::logger::priority p_)
  {
    _logging_priority_ = p_;
    return;
  }

  bool shape_factory::is_initialized() const
  {
    return _initialized_;
  }

  void shape_factory::initialize_simple()
  {
    datatools::properties dummy_config;
    initialize(dummy_config);
    return;
  }

  void shape_factory::initialize(const datatools::properties & config_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Shape factory is already initialized!");

    set_logging_priority(datatools::logger::extract_logging_configuration(config_,get_logging_priority()));

    _factory_register_.import(DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(::geomtools::i_object_3d));
    /*
    DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(::geomtools::i_object_3d).tree_dump(std::cerr,
                                                                              "3D-object factory: ");
    */

    std::string factory_shapes_list;
    std::vector<std::string> factory_shapes_files;

    if (config_.has_key ("shapes_list")) {
      factory_shapes_list = config_.fetch_string("shapes_list");
    }

    if (config_.has_key ("shapes_files")) {
      config_.fetch("shapes_files", factory_shapes_files);
    }

    _initialized_ = true;

    // Post initialization:
    if (! factory_shapes_list.empty ()) {
      load_list(factory_shapes_list);
    }

    for (std::vector<std::string>::const_iterator i
           = factory_shapes_files.begin();
         i != factory_shapes_files.end();
         i++) {
      std::string shapes_filename = *i;
      datatools::fetch_path_with_env(shapes_filename);
      DT_LOG_NOTICE(get_logging_priority(),
                    "Loading shape configuration file '" << shapes_filename << "'... ");
      load(shapes_filename);
      DT_LOG_NOTICE(get_logging_priority(),
                    "Shapes were loaded from file '" << shapes_filename << "'.");
    }
    return;
  }

  void shape_factory::reset()
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Shape factory is not initialized!");
    _initialized_ = false;
    if (datatools::logger::is_debug(get_logging_priority())) {
      this->tree_dump(std::cerr, "About to reset the shape factory: ", "[debug] ");
    }
    // for (auto & p : _shapes_) {
    //   p.second.reset();
    // }
    _shapes_.clear();
    return;
  }

  void shape_factory::load(const std::string & shapes_def_file_)
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Shape factory is not initialized !");
    datatools::multi_properties mp;
    mp.set_key_label("name");
    mp.set_meta_label("type");
    std::string ifile = shapes_def_file_;
    datatools::fetch_path_with_env(ifile);
    mp.read(ifile);
    for (datatools::multi_properties::entries_ordered_col_type::const_iterator i
           = mp.ordered_entries().begin();
         i != mp.ordered_entries().end();
         i++) {
      const datatools::multi_properties::entry * e = *i;
      const std::string & shape_name = e->get_key();
      const std::string & shape_type_id = e->get_meta();
      this->create(shape_name,
                   shape_type_id,
                   e->get_properties());
    }
    return;
  }

  void shape_factory::load_list(const std::string & shape_list_file_)
  {
    std::string list_filename = shape_list_file_;
    datatools::fetch_path_with_env(list_filename);

    std::ifstream finlist(list_filename.c_str ());
    DT_THROW_IF(! finlist, std::logic_error,
                 "Cannot open file '" << list_filename << "' !");
    while (finlist) {
      std::string line;
      std::getline(finlist, line);
      DT_THROW_IF(! finlist, std::logic_error,
                  "I/O error while reading file '" << list_filename << "' !");
      std::string word;
      std::istringstream line_iss(line);
      line_iss >> word;
      if (word.length() < 1) {
        // skip blank line
        continue;
      }
      if (word[0] == '#') {
        continue;
      }
      std::string shapes_filename = word;
      datatools::fetch_path_with_env(shapes_filename);
      load(shapes_filename);
      finlist >> std::ws;
      if (finlist.eof()) {
        break;
      }
    }
    return;
  }

  /**
   *  names : string[3] = "Base" "More" "Full"
   *
   *  type_id.Base : string = "geomtools::box"
   *  params.Base.x       : real as length = 3.2 cm
   *  params.Base.y       : real as length = 1.6 cm
   *  params.Base.z       : real as length = 0.8 cm
   *
   *  type_id.More : string = "geomtools::cylinder"
   *  params.More.r       : real as length = 0.8 cm
   *  params.More.z       : real as length = 0.5 cm
   *
   *  type_id.Full : string = "geomtools::union_3d"
   *  params.Full.first_shape.name       : string = "Base"
   *  params.Full.second_shape.name      : string = "More"
   *  params.Full.second_shape.placement : string = "0.0 0.0 0.4 (mm)"
   *
   *
   */
  void shape_factory::parse_shapes(const datatools::properties & defs_)
  {

    std::vector<std::string> shapes_names;
    if (defs_.has_key("names")) {
      defs_.fetch("names", shapes_names);
    }

    for (int i = 0; i < (int) shapes_names.size(); i++) {
      const std::string & shape_name = shapes_names[i];
      DT_THROW_IF(has(shape_name), std::logic_error, "Shape with name '" << shape_name << "' already exists in the factory!");
      datatools::properties shape_def;
      std::ostringstream shape_params_prefix;
      shape_params_prefix << "params." << shape_name << ".";
      defs_.export_and_rename_starting_with(shape_def, shape_params_prefix.str(), "");
      std::ostringstream shape_type_id_oss;
      shape_type_id_oss << "shape_type." << shape_name;
      DT_THROW_IF(!defs_.has_key(shape_type_id_oss.str()), std::logic_error,
                  "Missing shape type id property ('" << shape_type_id_oss.str()
                  << "' for shape named '" << shape_name << "'!");
      std::string shape_type_id = defs_.fetch_string(shape_type_id_oss.str());
      create(shape_name, shape_type_id, shape_def);
    }

    return;
  }

  void shape_factory::_at_init()
  {
    return;
  }

  void shape_factory::tree_dump(std::ostream & out_,
                                const std::string & title_,
                                const std::string & indent_,
                                bool inherit_) const
  {
    if (! title_.empty ()) {
      out_ << indent_ << title_ << std::endl;
    }

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Logging priority : '"
         << datatools::logger::get_priority_label(get_logging_priority()) << "'" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Initialized : "
         << std::boolalpha << _initialized_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Shapes : "
         << '[' << _shapes_.size() << ']' << std::endl;

    {
      size_t count = 0;
      for (const auto & p : _shapes_) {
        out_ << indent_ << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
        if (count + 1 == _shapes_.size()) {
          out_ << last_tag;
        } else {
          out_ << tag;
        }
        out_ << "Shape '" << p.first << "' of type='" << p.second.get_object().get_shape_name() << "' (entry=" << &p.second << ", address=" << &p.second.get_object() << ")" << std::endl;
        count++;
      }
    }

    return;
  }

} // end of namespace geomtools


/****************************************************************/
// OCD support for class '::geomtools::shape_factory' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::geomtools::shape_factory, ocd_)
{
  ocd_.set_class_name("geomtools::shape_factory");
  ocd_.set_class_library("geomtools");
  ocd_.set_class_description("A 3D object factory");
  ocd_.set_class_documentation("The 3D shape factory enables to create and store \n"
                               "arbitrary 3D objects (shapes)                    \n"
                               );

  ocd_.set_configuration_hints("To do \n"
                               );


  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::geomtools::shape_factory,"geomtools::shape_factory")

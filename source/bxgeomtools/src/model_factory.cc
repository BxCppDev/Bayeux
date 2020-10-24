// model_factory.cc

// Ourselves:
#include <geomtools/model_factory.h>

// Standard library:
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

// Third party:
// - Boost:
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/exception.h>

// This project:
#include <geomtools/detail/model_tools.h>
#include <geomtools/i_model.h>
#include <geomtools/logical_volume.h>
#include <geomtools/physical_volume.h>
#include <geomtools/visibility.h>

namespace geomtools {

  // static
  const std::string & model_factory::default_world_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "world";
    }
    return label;
  }

  // static
  bool model_factory::validate_name_for_gdml(const std::string & candidate_)
  {
    /* Using colon in a name is not allowed
     * because of GDML/XML restrictions on the possible names
     * for logical and physical volumes.
     * Also note that the "0x" pattern is arbitrarily (and stupidely)
     * cut from names by the Geant4/GDML interface.
     * See the G4GDMLRead::Strip, G4GDMLRead::StripName and G4GDMLRead::StripNames
     * methods.
     *
     * Examples:
     * a)  "scintillator:block" will be rejected by the GDML restrictions.
     * b)  "block_30x32x15cm3" will be cut as "block_3") by the Geant4/GDML stripping action, which
     *     doesn't match the original name in geomtools.
     */
    if (candidate_.find(":") != std::string::npos) return false;
    if (candidate_.find("0x") != std::string::npos) return false;
    return true;
  }

  bool model_factory::is_locked() const
  {
    return _locked_;
  }

  bool model_factory::is_debug() const
  {
    return _logging_priority_ >= datatools::logger::PRIO_DEBUG;
  }

  bool model_factory::is_trace() const
  {
    return _logging_priority_ >= datatools::logger::PRIO_TRACE;
  }

  void model_factory::set_debug(bool new_value_)
  {
    if (new_value_) {
      _logging_priority_ = datatools::logger::PRIO_DEBUG;
    } else {
      _logging_priority_ = datatools::logger::PRIO_FATAL;
    }
    return;
  }

  datatools::logger::priority model_factory::get_logging_priority() const
  {
    return _logging_priority_;
  }

  void model_factory::set_logging_priority(datatools::logger::priority p_)
  {
    _logging_priority_ = p_;
    return;
  }

  bool model_factory::has_shape_factory() const
  {
    return _shape_factory_ != 0;
  }

  void model_factory::set_shape_factory(shape_factory & sf_)
  {
    _shape_factory_ = &sf_;
    return;
  }

  shape_factory & model_factory::grab_shape_factory()
  {
    DT_THROW_IF(!has_shape_factory(),
                std::logic_error,
                "No referenced shape factory!");
    return *_shape_factory_;
  }

  const shape_factory & model_factory::get_shape_factory() const
  {
    DT_THROW_IF(!has_shape_factory(),
                std::logic_error,
                "No referenced shape factory!");
    return *_shape_factory_;
  }

  const models_col_type & model_factory::get_models() const
  {
    return _models_;
  }

  const logical_volume::dict_type & model_factory::get_logicals() const
  {
    return _logicals_;
  }

  void model_factory::_basic_construct_()
  {
    _locked_ = false;
    _mp_.set_key_label("name");
    _mp_.set_meta_label("type");
    _mp_.set_description("Geometry models setup");
    bool preload = true;
    if (preload) {
      _factory_register_.import(DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(::geomtools::i_model));
      if (_logging_priority_ >= datatools::logger::PRIO_INFORMATION) {
        _factory_register_.tree_dump(std::cerr, "Geometry model factory: ", "INFO: ");
      }
    }
    return;
  }

  model_factory::model_factory(datatools::logger::priority lp_)
    :  _factory_register_("geomtools::i_model/model_factory",
                          (lp_ >= datatools::logger::PRIO_NOTICE) ? i_model::factory_register_type::verbose : 0)
  {
    _shape_factory_ = 0;
    _locked_ = false;
    _logging_priority_ = lp_;
    _basic_construct_();
    return;
  }

  model_factory::model_factory(bool debug_, bool core_factory_verbose_)
    :  _factory_register_("geomtools::i_model/model_factory",
                          core_factory_verbose_ ? i_model::factory_register_type::verbose : 0)

  {
    _shape_factory_ = 0;
    _locked_ = false;
    _logging_priority_ = datatools::logger::PRIO_FATAL;
    if (debug_) _logging_priority_ = datatools::logger::PRIO_DEBUG;
    _basic_construct_();
    return;
  }

  model_factory::~model_factory()
  {
    if (_locked_) {
      reset();
    }
    return;
  }

  // 2012-05-25 FM : add support for loading a file that contains a list of geometry filenames :
  void model_factory::load_geom_list(const std::string & geom_list_file_)
  {
    std::vector<std::string> model_filenames;
    std::string geom_lis_filename = geom_list_file_;
    datatools::fetch_path_with_env(geom_lis_filename);
    DT_LOG_NOTICE(_logging_priority_, "Opening geometry models filename list...");
    DT_THROW_IF(! boost::filesystem::exists(geom_lis_filename), std::logic_error,
                "File '" << geom_lis_filename << "' does not exist!");
    std::ifstream finlist(geom_lis_filename.c_str());
    DT_THROW_IF(! finlist, std::logic_error,
                "Cannot open file '" << geom_lis_filename << "' !");
    DT_LOG_NOTICE(_logging_priority_, "Geometry models filename list if open.");
    while (finlist) {
      std::string line;
      std::getline(finlist, line);
      DT_LOG_NOTICE(_logging_priority_, "line='" << line << "'");
      DT_THROW_IF(! finlist, std::logic_error,
                  "I/O error while reading file '" << geom_lis_filename << "' !");
      std::string word;
      std::istringstream line_iss(line);
      line_iss >> word;
      // Skip blank line:
      if (word.length() > 0) {
        // Skip comment:
        if (word[0] != '#') {
          std::string geom_filename = word;
          model_filenames.push_back(geom_filename);
        }
      }
      finlist >> std::ws;
      if (finlist.eof()) {
        break;
      }
    }
    for (int i = 0; i < (int) model_filenames.size(); i++) {
      std::string geom_filename = model_filenames[i];
      datatools::fetch_path_with_env(geom_filename);
      DT_LOG_NOTICE(_logging_priority_, "Loading geometry models from file '" << geom_filename << "'...");
      load(geom_filename);
    }
    return;
  }

  const datatools::multi_properties & model_factory::get_mp() const
  {
    return _mp_;
  }

  datatools::multi_properties & model_factory::grab_mp()
  {
    DT_THROW_IF(_locked_, std::logic_error, "Model factory is locked !");
    return _mp_;
  }

  void model_factory::load(const std::string & mprop_file_)
  {
    DT_THROW_IF(_locked_, std::logic_error, "Model factory is locked !");
    datatools::read_config(mprop_file_, _mp_);
    if (is_trace()) {
      DT_LOG_TRACE(_logging_priority_, "Model factory multi-configuration: ");
      _mp_.tree_dump(std::clog, "", "[trace]: ");
    }
    return;
  }

  void model_factory::_lock_()
  {
    DT_THROW_IF(_locked_, std::logic_error, "Model factory is locked !");
    _construct_();
    _mp_.reset();
    return;
  }

  void model_factory::_unlock_()
  {
    return;
  }

  void model_factory::unlock()
  {
    if (! _locked_) return;
    _unlock_();
    _locked_ = false;
    return;
  }

  void model_factory::lock()
  {
    DT_THROW_IF(_locked_, std::logic_error, "Model factory is locked !");
    _lock_();
    _locked_ = true;
    return;
  }

  void model_factory::reset()
  {
    if (_locked_) {
      unlock();
    }
    // The container of logicals does not have ownership of the pointers :
    _logicals_.clear();
    // Memory leak to be fixed:
    for (models_col_type::iterator i = _models_.begin();
         i != _models_.end();
         i++) {
      const std::string & model_name = i->first;
      i_model * model_ptr = i->second;
      if (model_ptr != 0) {
        DT_LOG_DEBUG(_logging_priority_,"Deleting registered model '" << model_name << "'.");
        delete model_ptr;
      }
    }
    _models_.clear();
    _mp_.reset();
    _property_prefixes_.clear();
    _shape_factory_ = 0;
    return;
  }

  // Add a property prefix to be preserved in logicals
  void model_factory::add_property_prefix(const std::string & prefix_)
  {
    DT_THROW_IF(prefix_.empty(),
                std::logic_error,
                "Property prefix to be preserved cannot be empty !");
    DT_THROW_IF((std::find(_property_prefixes_.begin(),
                           _property_prefixes_.end(),
                           prefix_) != _property_prefixes_.end()),
                std::logic_error,
                "Property prefix to be preserved '" << prefix_<< "' already exists !");
    _property_prefixes_.push_back(prefix_);
    return;
  }

  void model_factory::_construct_()
  {
    DT_LOG_TRACE(_logging_priority_, "Entering...");
    for (datatools::multi_properties::entries_ordered_col_type::const_iterator i
           = _mp_.ordered_entries().begin();
         i != _mp_.ordered_entries().end();
         i++) {
      const datatools::multi_properties::entry * ptr_entry = *i;
      const datatools::multi_properties::entry & e = *ptr_entry;
      std::string model_name = e.get_key();
      DT_THROW_IF(!validate_name_for_gdml(model_name), std::logic_error,
                  "Geometry model name '" << model_name << "' is not supported for GDML export in Geant4 !");
      std::string model_type = e.get_meta();
      // if (! _factory_register_.has(model_type)) {
      //   DT_LOG_TRACE(_logging_priority_,
      //                "No registered class with ID '"
      //                << model_type << "' for model named '" << model_name << " !");
      //   continue;
      // }
      DT_THROW_IF(! _factory_register_.has(model_type),
                  std::runtime_error,
                  "Cannot find any registered geometry model class factory with ID '"
                  << model_type << "' for model named '" << model_name << " !");
      const i_model::factory_register_type::factory_type & the_factory
        = _factory_register_.get(model_type);
      DT_LOG_TRACE(_logging_priority_,
                   "About to create a new model of type \"" << model_type
                   << "\" with name \"" << model_name << "\"...");
      i_model * model = the_factory();
      // Plug the shape factory in the model:
      if (has_shape_factory()) {
        model->set_shape_factory(grab_shape_factory());
      }
      model->construct(model_name, e.get_properties(), _property_prefixes_, &_models_);
      _models_[model_name] = model;
      DT_LOG_DEBUG(_logging_priority_,"Adding model '" << model_name << "'...");
      std::string log_name = model->get_logical().get_name();
      if (_logicals_.find(log_name) == _logicals_.end()) {
        _logicals_[log_name] = &(model->get_logical());
      } else {
        DT_THROW_IF(true, std::runtime_error,
                    "Logical volume '" << log_name << "' is already referenced in the model factory !");
      }
      for (logical_volume::physicals_col_type::const_iterator iphys = model->get_logical().get_physicals().begin();
           iphys != model->get_logical().get_physicals().end();
           iphys++) {
        const physical_volume & phys_vol = *iphys->second;
        if (phys_vol.has_logical()) {
          const logical_volume & phys_log = phys_vol.get_logical();
          if (_logicals_.find(phys_log.get_name()) == _logicals_.end()) {
            _logicals_[phys_log.get_name()] = &phys_log;
          }
        }
      }

      DT_LOG_DEBUG(_logging_priority_,"New model is:");
      if (is_debug()) model->tree_dump(std::clog, "", "[debug]: ");
    }
    DT_LOG_TRACE(_logging_priority_,"Exiting.");
    return;
  }

  const std::vector<std::string> & model_factory::get_property_prefixes() const
  {
    return _property_prefixes_;
  }

  void model_factory::tree_dump(std::ostream & out_,
                                const std::string & title_,
                                const std::string & indent_,
                                bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty()) indent = indent_;
    if (! title_.empty()) {
      out_ << indent << title_ << std::endl;
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Logging priority threshold       : \""
         << datatools::logger::get_priority_label(_logging_priority_) << "\"" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Locked  : " <<  (_locked_? "Yes": "No") << std::endl;

    // Configurations:
    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Multi-properties : ";
      if ( _mp_.entries().size() == 0) {
        out_ << "<empty>";
      }
      out_ << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _mp_.tree_dump (out_, "", indent_oss.str());
      }
    }

    // Logicals:
    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Logicals : ";
      if ( _logicals_.size() == 0) {
        out_ << "<empty>";
      } else {
        out_ << "[" << _logicals_.size() << "]";
      }
      out_ << std::endl;
      // for (logical_volume::dict_type::const_iterator i = _logicals_.begin();
      //      i != _logicals_.end();
      //      i++) {
      //   const string & key = i->first;
      //   const logical_volume * a_logical = i->second;
      // }
    }

    // Models:
    {
      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Geometry models : ";
      if ( _models_.size() == 0) {
        out_ << "<empty>";
      } else {
        out_ << "[" << _models_.size() << "]";
      }
      out_ << std::endl;
      for (models_col_type::const_iterator i = _models_.begin();
           i != _models_.end();
           i++) {
        const std::string & key = i->first;
        const i_model * a_model = i->second;
        std::ostringstream indent_oss;
        out_ << indent << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
        indent_oss << indent << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
        models_col_type::const_iterator j = i;
        j++;
        if (j == _models_.end()) {
          out_ << datatools::i_tree_dumpable::inherit_tag(inherit_);
          indent_oss << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
        } else {
          out_ << datatools::i_tree_dumpable::tag;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
        }
        out_ << "Model : " << '"' << key << '"' << std::endl;
        a_model->tree_dump(out_, "", indent_oss.str());
      }
    }

    return;
  }

  int model_factory::print_list_of_models(const geomtools::model_factory & mf_,
                                          const std::string & command_,
                                          const std::string & options_,
                                          std::ostream & out_)
  {
    //std::cerr << "DEVEL: model_factory::print_list_of_models: Entering... \n";
    std::vector<std::string> requested_patterns;

    bool with_title = true;
    bool with_multicolumn = true;

    //std::cerr << "DEVEL: model_factory::print_list_of_models: Parsing options... \n";
    // Parse options :
    std::istringstream options_iss(options_);
    options_iss >> std::ws;
    while (options_iss && ! options_iss.eof()) {
      std::string option;
      options_iss >> option >> std::ws;

      if (option=="--help" || option=="-h") {
        out_ << "  Usage: \n";
        out_ << "     " << command_ << " [OPTIONS...]  \n"
             << "\n";
        out_ << "  Options: \n";
        out_ << "    -h | --help            Print this help\n"
             << "    -t | --with-title      Print a title line\n"
             << "    -T | --without-title   Do not print a title line\n"
             << "    -m | --multicolumn     Print in multicolumn mode\n"
             << "    -1 | --onecolumn       Print in one column mode\n"
             << "    -p | --pattern PATTERN Select geometry models with matching pattern PATTERN\n"
             << std::endl;
        return -1;
      }
      else if (option == "--with-title" || option=="-t") with_title = true;
      else if (option == "--without-title" || option=="-T") with_title = false;
      else if (option == "--multicolumn" || option=="-m") with_multicolumn = true;
      else if (option == "--onecolumn" || option=="-1") with_multicolumn = false;
      else if (option == "-p" || option == "--pattern") {
        std::string pattern;
        options_iss >> pattern >> std::ws;
        datatools::remove_quotes(pattern);
        if (pattern.empty()) {
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Empty geometry model pattern !");
          return 1;
        }
        requested_patterns.push_back(pattern);
      } else {
        DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Invalid option '" << option << "' !");
        return 1;
      }
      if (options_iss.eof()) break;
    }
    //std::cerr << "DEVEL: model_factory::print_list_of_models: Parsed options. \n";

    std::vector<const std::string*> selected_models;
    for (geomtools::models_col_type::const_iterator i
           = mf_.get_models().begin();
         i != mf_.get_models().end();
         i++) {
      const std::string & model_name = i->second->get_name();
      if (requested_patterns.size() == 0) {
        selected_models.push_back(&model_name);
      } else {
        bool selected = false;
        for (size_t j = 0; j < requested_patterns.size(); j++) {
          try {
            boost::regex expression(requested_patterns[j]);
            if (boost::regex_search(model_name.begin(), model_name.end(), expression)) {
              selected = true;
              break;
            }
          } catch (std::exception & error) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Regular expression error: " << error.what());
            return 1;
          }
        }
        if (selected) {
          selected_models.push_back(&model_name);
        }
      }
    }

    //std::cerr << "DEVEL: model_factory::print_list_of_models: Nb sel. models = " << selected_models.size() << " \n";

    if (with_title) {
      out_ << std::flush << "List of available geometry models : " << std::endl;
    }
    size_t max_width = 32;
    int count = 0;
    for (std::vector<const std::string*>::const_iterator i
           = selected_models.begin();
         i != selected_models.end();
         i++) {
      const std::string & model_name = **i;
      if (with_multicolumn) {
        bool long_name = false;
        if (model_name.size() > max_width) {
          long_name = true;
        }
        if (count != 0 && (count % 2) == 0) {
          out_ << std::endl;
        }
        out_  << "  " << std::setw(max_width)
              << std::setiosflags(std::ios::left)
              << std::resetiosflags(std::ios::right)
              << model_name << "  ";
        if (long_name) {
          out_ << std::endl;
          count = 0;
        }
        count++;
      } else {
        out_ << model_name;
        out_ << std::endl;
      }
    }
    if (with_multicolumn) {
      if ((count % 2) == 1)  {
        out_ << std::endl;
      }
      out_ << std::endl;
    }
    return 0;
  }

  int model_factory::print_list_of_logicals(const geomtools::model_factory & mf_,
                                            const std::string & command_,
                                            const std::string & options_,
                                            std::ostream & out_)
  {
    std::vector<std::string> requested_patterns;

    bool with_title = true;
    bool with_multicolumn = true;

    // Parse options :
    std::istringstream options_iss(options_);
    options_iss >> std::ws;
    while (options_iss && ! options_iss.eof()) {
      std::string option;
      options_iss >> option >> std::ws;
      // std::cerr << "DEVEL: Option = '" << option << "'\n";
      if (option=="--help" || option=="-h") {
        out_ << "  Usage: \n";
        out_ << "     " << command_ << " [OPTIONS...]  \n"
             << "\n";
        out_ << "  Options: \n";
        out_ << "    -h | --help            Print this help\n"
             << "    -t | --with-title      Print a title line\n"
             << "    -T | --without-title   Do not print a title line\n"
             << "    -m | --multicolumn     Print in multicolumn mode\n"
             << "    -1 | --onecolumn       Print in one column mode\n"
             << "    -p | --pattern PATTERN Select logical volumes with matching pattern PATTERN\n"
             << std::endl;
        return -1;
      }
      else if (option == "--with-title" || option=="-t") with_title = true;
      else if (option == "--without-title" || option=="-T") with_title = false;
      else if (option == "--multicolumn" || option=="-m") with_multicolumn = true;
      else if (option == "--onecolumn" || option=="-1") with_multicolumn = false;
      else if (option == "-p" || option == "--pattern") {
        std::string pattern;
        options_iss >> pattern >> std::ws;
        datatools::remove_quotes(pattern);
        if (pattern.empty()) {
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Empty geometry model pattern !");
          return 1;
        }
        requested_patterns.push_back(pattern);
      } else {
        DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Invalid option '" << option << "' !");
        return 1;
      }
      if (options_iss.eof()) break;
    }

    std::vector<const std::string*> selected_logicals;
    for (logical_volume::dict_type::const_iterator i
           = mf_.get_logicals().begin();
         i != mf_.get_logicals().end();
         i++) {
      const std::string & logical_name = i->second->get_name();
      if (requested_patterns.size() == 0) {
        selected_logicals.push_back(&logical_name);
      } else {
        bool selected = false;
        for (size_t j = 0; j < requested_patterns.size(); j++) {
          try {
            boost::regex expression(requested_patterns[j]);
            if (boost::regex_search(logical_name.begin(), logical_name.end(), expression)) {
              selected = true;
              break;
            }
          } catch (std::exception & error) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Regular expression error: " << error.what());
            return 1;
          }
        }
        if (selected) {
          selected_logicals.push_back(&logical_name);
        }
      }
    }

    if (with_title) {
      out_ << std::flush << "List of available logical volumes : " << std::endl;
    }
    size_t max_width = 32;
    int count = 0;
    for (std::vector<const std::string*>::const_iterator i
           = selected_logicals.begin();
         i != selected_logicals.end();
         i++) {
      const std::string & logical_name = **i;
      if (with_multicolumn) {
        bool long_name = false;
        if (logical_name.size() > max_width) {
          long_name = true;
        }
        if (count != 0 && (count % 2) == 0) {
          out_ << std::endl;
        }
        out_  << "  " << std::setw(max_width)
              << std::setiosflags(std::ios::left)
              << std::resetiosflags(std::ios::right)
              << logical_name << "  ";
        if (long_name) {
          out_ << std::endl;
          count = 0;
        }
        count++;
      } else {
        out_ << logical_name;
        out_ << std::endl;
      }
    }
    if (with_multicolumn) {
      if ((count % 2) == 1)  {
        out_ << std::endl;
      }
      out_ << std::endl;
    }
    return 0;
  }

} // end of namespace geomtools


/****************************************************************/
// OCD support for class '::geomtools::model_factory' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::geomtools::model_factory,ocd_)
{
  ocd_.set_class_name("geomtools::model_factory");
  ocd_.set_class_library("geomtools");
  ocd_.set_class_description("A virtual geometry model_factory");
  ocd_.set_class_documentation("The geometry model factory is the core engine of the\n"
                               "'geomtools' geometry system. The factory is able to \n"
                               "load the definitions of many 'geometry models' from \n"
                               "ASCII configuration files.                          \n"
                               "A geometry model is a special object that describes \n"
                               "a geometry volume and its main characteristics : shapes,\n"
                               "materials, daughter volumes with their placements, and\n"
                               "also arbitrary properties that can be used at some point.\n"
                               "A geometry model must fulfil the interface of the \n"
                               "'geomtools::i_model' class.                       \n"
                               );


  ocd_.set_configuration_hints("A geometry model factory is configured through some configuration     \n"
                               "files that obeys the 'datatools::multi_properties' class.             \n"
                               "Each file contains a list of sections, one section per geometry model.\n"
                               "A geometry model object describes a possibly complex geometry volume  \n"
                               "with its shape, materials, daughter volumes and many other attributes \n"
                               "that can be used for various tasks (ex: display, GID mapping).        \n"
                               "A geometry model is dynamically instantiated and initialized/constructed\n"
                               "by the geometry factory.                                                \n"
                               );

  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::geomtools::model_factory,"geomtools::model_factory")

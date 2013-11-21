// -*- mode: c++ ; -*-
/* manager.cc
 */

#include <stdexcept>
#include <sstream>

#include <datatools/multi_properties.h>
#include <datatools/utils.h>

#include <materials/materials_config.h>
#include <materials/isotope.h>
#include <materials/element.h>
#include <materials/material.h>
#include <materials/manager.h>

namespace materials {

  using namespace std;
  using namespace datatools;

  bool manager::is_initialized () const
  {
    return is_locked();
  }

  void manager::set_logging_priority(datatools::logger::priority p)
  {
    _logging_priority_ = p;
  }

  datatools::logger::priority manager::get_logging_priority() const
  {
    return _logging_priority_;
  }

  bool manager::is_debug () const
  {
    return _logging_priority_ >= datatools::logger::PRIO_DEBUG;
  }

  void manager::set_debug (bool new_value_)
  {
    if (new_value_) set_logging_priority(datatools::logger::PRIO_DEBUG);
    else set_logging_priority(datatools::logger::PRIO_WARNING);
    return;
  }

  bool manager::is_locked () const
  {
    return _locked_;
  }

  void manager::set_alias_allow_overload (bool aao_)
  {
    _alias_allow_overload_ = aao_;
    return;
  }

  bool manager::is_alias_allow_overload () const
  {
    return _alias_allow_overload_;
  }

  bool manager::has_material(const std::string & entry_name_) const
  {
    return _materials_.find(entry_name_) != _materials_.end();
  }

  bool manager::is_alias(const std::string & entry_name_) const
  {
    return ! manager::alias_of(entry_name_).empty();
  }

  std::string manager::alias_of(const std::string & entry_name_) const
  {
    material_dict_type::const_iterator found    = _materials_.find(entry_name_);
    const ::materials::smart_ref<material> & sr = found->second;
    std::string mat_alias;
    if (sr.is_alias ()) {
      return sr.get_alias_of();
    }
    return mat_alias;
  }

  const isotope_dict_type & manager::get_isotopes () const
  {
    return _isotopes_;
  }

  const element_dict_type & manager::get_elements () const
  {
    return _elements_;
  }

  const material_dict_type & manager::get_materials () const
  {
    return _materials_;
  }

  const std::list<std::string> & manager::get_ordered_materials () const
  {
    return _ordered_materials_;
  }

  // ctor:
  manager::manager ()
  {
    _logging_priority_ = datatools::logger::PRIO_WARNING;
    _locked_ = false;
    _alias_allow_overload_= false;
    return;
  }

  // dtor:
  manager::~manager ()
  {
    _materials_.clear ();
    _elements_.clear ();
    _isotopes_.clear ();
    _ordered_materials_.clear ();
    return;
  }

  void manager::lock ()
  {
    _locked_ = true;
    return;
  }

  void manager::unlock ()
  {
    _locked_ = false;
    return;
  }

  void manager::initialize (const datatools::properties & setup_)
  {
    DT_THROW_IF (is_locked (), std::logic_error, "Manager is locked !");

    // Logging priority:
    datatools::logger::priority lp = datatools::logger::extract_logging_configuration (setup_);
    DT_THROW_IF(lp == datatools::logger::PRIO_UNDEFINED,
                std::logic_error,
                "Invalid logging priority level for geometry manager !");
    set_logging_priority(lp);

    if (setup_.has_flag("debug")) {
      set_debug(true);
    }

    if (setup_.has_flag("alias_allow_overload")) {
      set_alias_allow_overload(true);
    }
    std::vector<std::string> conf_files;

    if (setup_.has_key("configuration_files")) {
      setup_.fetch("configuration_files", conf_files);
    }

    for (size_t i = 0; i < conf_files.size(); i++) {
      std::string filename = conf_files[i];
      datatools::fetch_path_with_env(filename);
      datatools::multi_properties mconfig;
      mconfig.read(filename);
      load(mconfig);
    }

    lock();
    return;
  }

  void manager::load (const datatools::multi_properties & config_)
  {
    DT_THROW_IF (is_locked (), std::logic_error, "Manager is locked !");

    for (multi_properties::entries_ordered_col_type::const_iterator i
           = config_.ordered_entries ().begin ();
         i != config_.ordered_entries ().end ();
         i++) {
      multi_properties::entry * e = *i;
      const string & name = e->get_key ();
      const string & type = e->get_meta ();
      const datatools::properties & props = e->get_properties ();

      DT_LOG_DEBUG(_logging_priority_, "Load name = '" << name << "'");
      DT_LOG_DEBUG(_logging_priority_, "Load type = '" << type << "'");
      if (type == "isotope") {
        DT_THROW_IF (_isotopes_.find (name) != _isotopes_.end (),
                     std::logic_error,
                     "Isotope with name '" << name << "' already exists !");
        isotope * iso = _creator_.create_isotope (name, props);
        _isotopes_[iso->get_name ()] = materials::smart_ref<isotope> ();
        _isotopes_[iso->get_name ()].set_ref (iso);
        DT_LOG_DEBUG(_logging_priority_,"Add new isotope = '" << iso->get_zai_name () << "'");
      }
      else if (type == "element") {
        DT_THROW_IF (_elements_.find (name) != _elements_.end (),
                     std::logic_error,
                     "Element with name '" << name << "' already exists !");
        bool unique_element_material = false;
        DT_THROW_IF (unique_element_material,
                     std::logic_error,
                     "Material with name '" << name << "' already exists !");
        element * elmt = _creator_.create_element (name, props, _isotopes_);
        _elements_[elmt->get_name ()] = materials::smart_ref<element> ();
        _elements_[elmt->get_name ()].set_ref (elmt);
        DT_LOG_DEBUG(_logging_priority_,"Add new element = '" << elmt->get_name () << "'");
      } else if (type == "material") {
        DT_THROW_IF (_materials_.find (name) != _materials_.end (),
                     std::logic_error,
                     "Material with name '" << name << "' already exists !");
        bool unique_element_material = false;
        if (unique_element_material) {
          DT_THROW_IF (_elements_.find (name) != _elements_.end (),
                       std::logic_error,
                       "Elements with name '" << name << "' already exists !");
        }
        material * matl = _creator_.create_material (name,
                                                     props,
                                                     _elements_,
                                                     _materials_);
        _materials_[matl->get_name ()] = materials::smart_ref<material> ();
        _materials_[matl->get_name ()].set_ref (matl);
        DT_LOG_DEBUG(_logging_priority_,"Add new material = '" << matl->get_name () << "'");
        _ordered_materials_.push_back (matl->get_name ());
      }
      else if (type == "alias") {
        material_dict_type::const_iterator mat_found = _materials_.find (name);
        if (mat_found != _materials_.end ()) {
          const smart_ref<material> & sref = mat_found->second;
          DT_THROW_IF (! sref.is_alias(),
                       std::logic_error,
                       "Material with name '" << name
                       << "' already exists ! It cannot be overloaded !");
          // Already existing material alias:
          DT_THROW_IF (! is_alias_allow_overload(),
                       std::logic_error,
                       "Alias with name '" << name << "' cannot be overloaded !");
          DT_LOG_WARNING(_logging_priority_,"Redefinition of alias '" << name << "' !");
        }
        DT_THROW_IF (! props.has_key ("material"),
                     std::logic_error,
                     "Missing property 'material' for a material alias !");
        std::string alias_material = props.fetch_string ("material");
        material_dict_type::iterator found = _materials_.find (alias_material);
        DT_THROW_IF (found == _materials_.end (),
                     std::logic_error,
                     "Aliased material named '" << alias_material << "' does not exist !");
        DT_THROW_IF (found->second.is_alias(),
                     std::logic_error,
                     "Material alias with name '" << name
                     << "' cannot refer itself to another alias ('" << alias_material << "') !");
        _materials_[name] = materials::smart_ref<material> ();
        _materials_[name].set_ref (found->second.grab_ref ());
        _materials_[name].set_alias_of(alias_material);
        std::list<std::string>::iterator ofound = std::find(_ordered_materials_.begin(),_ordered_materials_.end(), name);
        if (ofound != _ordered_materials_.end()) {
          _ordered_materials_.erase(ofound);
        }
        _ordered_materials_.push_back (name);
        DT_LOG_DEBUG(_logging_priority_, "Add new material alias = '" << name << "' for material '" << alias_material << "'");
      }

    } // for

    return;
  }

  void manager::reset ()
  {
    _isotopes_.clear ();
    _elements_.clear ();
    _materials_.clear ();
    _ordered_materials_.clear ();
    return;
  }

  void manager::tree_dump (ostream & out_,
                           const string & title_,
                           const string & indent_,
                           bool inherit_) const
  {
    namespace du = datatools;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ()) {
      out_ << indent << title_ << endl;
    }

    out_ << indent << i_tree_dumpable::tag
         << "Debug : " << is_debug () << endl;

    {
      out_ << indent << du::i_tree_dumpable::tag
           << "Isotopes";
      if ( _isotopes_.size () == 0) {
        out_ << " : <empty>" << endl;
      } else {
        out_ << " [" << _isotopes_.size () << "] :" << endl;
        for (isotope_dict_type::const_iterator i = _isotopes_.begin ();
             i != _isotopes_.end ();
             i++) {
          std::ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << du::i_tree_dumpable::skip_tag;
          isotope_dict_type::const_iterator j = i;
          j++;
          out_ << indent << du::i_tree_dumpable::skip_tag;;
          if (j != _isotopes_.end ()) {
            out_ << du::i_tree_dumpable::tag;
            indent_oss << du::i_tree_dumpable::skip_tag;;
          } else {
            out_ << du::i_tree_dumpable::last_tag;
            indent_oss << du::i_tree_dumpable::last_skip_tag;;
          }
          out_ << "Isotope '" << i->first << "' :" << endl;
          i->second.get_ref ().tree_dump (out_, "", indent_oss.str ());
        }
      }
    }

    {
      out_ << indent << du::i_tree_dumpable::tag
           << "Elements";
      if ( _elements_.size () == 0) {
        out_ << " : <empty>" << endl;
      } else {
        out_ << " [" << _elements_.size () << "] :" << endl;
        for (element_dict_type::const_iterator i = _elements_.begin ();
             i != _elements_.end ();
             i++) {
          std::ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << du::i_tree_dumpable::skip_tag;
          element_dict_type::const_iterator j = i;
          j++;
          out_ << indent << du::i_tree_dumpable::skip_tag;;
          if (j != _elements_.end ()) {
            out_ << du::i_tree_dumpable::tag;
            indent_oss << du::i_tree_dumpable::skip_tag;;
          } else {
            out_ << du::i_tree_dumpable::last_tag;
            indent_oss << du::i_tree_dumpable::last_skip_tag;;
          }
          out_ << "Element '" << i->first << "' :" << endl;
          i->second.get_ref ().tree_dump (out_, "", indent_oss.str ());
        }
      }
    }

    {
      out_ << indent << du::i_tree_dumpable::tag
           << "Materials";
      if ( _materials_.size () == 0) {
        out_ << " : <empty>" << endl;
      } else {
        out_ << " [" << _materials_.size () << "] :" << endl;
        for (material_dict_type::const_iterator i = _materials_.begin ();
             i != _materials_.end ();
             i++) {
          std::ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << du::i_tree_dumpable::skip_tag;
          material_dict_type::const_iterator j = i;
          j++;
          out_ << indent << du::i_tree_dumpable::skip_tag;;
          if (j != _materials_.end ()) {
            out_ << du::i_tree_dumpable::tag;
            indent_oss << du::i_tree_dumpable::skip_tag;;
          } else {
            out_ << du::i_tree_dumpable::last_tag;
            indent_oss << du::i_tree_dumpable::last_skip_tag;;
          }
          out_ << "Material '" << i->first << "'";
          if (i->second.is_alias ()) {
            out_ << " (alias of '" << i->second.get_alias_of() << "')";
            out_ << endl;
          } else {
            if (! i->second.is_owned ()) {
              out_ << " (external)";
            }
            out_ << " : " << endl;
            i->second.get_ref ().tree_dump (out_, "", indent_oss.str ());
          }
        }
      }
    }


    {
      out_ << indent << du::i_tree_dumpable::tag
           << "Materials (ordered)";
      if ( _ordered_materials_.size () == 0) {
        out_ << " : <empty>" << endl;
      } else {
        out_ << " [" << _ordered_materials_.size () << "] :" << endl;
        for (std::list<std::string>::const_iterator i = _ordered_materials_.begin ();
             i != _ordered_materials_.end ();
             i++) {
          std::ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << du::i_tree_dumpable::skip_tag;
          std::list<std::string>::const_iterator j = i;
          j++;
          out_ << indent << du::i_tree_dumpable::skip_tag;
          if (j != _ordered_materials_.end ()) {
            out_ << du::i_tree_dumpable::tag;
            indent_oss << du::i_tree_dumpable::skip_tag;
          } else {
            out_ << du::i_tree_dumpable::last_tag;
            indent_oss << du::i_tree_dumpable::last_skip_tag;;
          }
          out_ << "Material '" << *i << "' " << endl;
        }
      }
    }

    out_ << indent << i_tree_dumpable::last_tag
         << "Locked       : " << (is_locked()? "Yes": "No") << endl;

    return;

  }

} // end of namespace materials


/****************************************************************/
// OCD support for class '::materials::manager' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::materials::manager,ocd_)
{
  ocd_.set_class_name("materials::manager");
  ocd_.set_class_description("A manager to describe and handle isotopes, elements and materials objects");
  ocd_.set_class_library("materials");
  ocd_.set_class_documentation("not documented yet");

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("debug")
      .set_terse_description("Flag to activate debugging output")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .set_long_description("Superseded by a previous call to :              \n"
                            "  ``materials::manager::set_debug(true)``       \n"
                            "                                                \n"
                            "Example::                                       \n"
                            "                                                \n"
                            "  debug : boolean = 0                           \n"
                            "                                                \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("alias_allow_overload")
      .set_terse_description("Flag to allow material alias overloading")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .set_long_description("Users are invited to define material aliases, i.e. some     \n"
                            "human-friendly character strings that are associated        \n"
                            "to material objects stored in the manager's dictionnary  .  \n"
                            "Typically, one may want to use the alias ``metal`` as a     \n"
                            "shortcut for the ``std::inox`` material.                    \n"
                            "If the user changes his/her mind, it is easy to make the    \n"
                            "``metal`` string the alias of the ``std::copper`` material. \n"
                            "This gives some flexibility by just changing the definition \n"
                            "of an alias without changing the definitions of standard    \n"
                            "materials like copper or inox registered in some standard   \n"
                            "dictionnary.                                                \n"
                            "When the ``alias_allow_overload`` is set, it is possible to \n"
                            "provide duplicated definitions of an alias, the last one    \n"
                            "overloading the former ones. Otherwise, multiple alias      \n"
                            "definition is considered as an error.                       \n"
                            "Superseded by a previous call of :                          \n"
                            "  ``materials::manager::set_alias_allow_overload(true)``    \n"
                            "                                                \n"
                            "Example::                                       \n"
                            "                                                \n"
                            "  alias_allow_overload : boolean = 1            \n"
                            "                                                \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("configuration_files")
      .set_terse_description("A list of configuration file names for isotopes, elements and materials")
      .set_traits(datatools::TYPE_STRING,
                  configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_path(true)
      .set_long_description("A list of filenames from where the manager loads the      \n"
                            "directives to dynamically instantiate new isotopes,       \n"
                            "elements, materials and material aliases objects.         \n"
                            "The filenames may contains some environment variables.    \n"
                            "                                                          \n"
                            "Example::                                                  \n"
                            "                                                          \n"
                            "   configuration_files : string[4] as path =   \\         \n"
                            "     \"${CONFIG_REPOSITORY_DIR}/isotopes.conf\"  \\       \n"
                            "     \"${CONFIG_REPOSITORY_DIR}/elements.conf\"  \\       \n"
                            "     \"${CONFIG_REPOSITORY_DIR}/materials.conf\" \\       \n"
                            "     \"${CONFIG_REPOSITORY_DIR}/aliases.conf\"            \n"
                            "                                                          \n"
                            "The target files must use the format of the               \n"
                            "``datatools::multi_properties`` class.                      \n"
                            "The loading order of the files is critical                \n"
                            "because some isotopes, elements or materials objects may  \n"
                            "depend on other ones which should thus be defined **before**\n"
                            "their dependers.                                          \n"
                            "Extends the instantiation of objects triggered by         \n"
                            "previous calls to :                                         \n"
                            "  ``materials::manager::load(...)``                          \n"
                            )
      ;
  }

  ocd_.set_configuration_hints ("A materials manager is configured through a set of configuration      \n"
                                "files that obeys the format of ``datatools::multi_properties`` setup  \n"
                                "file.                                                                 \n"
                                "The method used to instantiate isotopes, elements and materials       \n"
                                "objects is :                                                          \n"
                                "   ``void manager::load(const datatools::multi_properties & config_);``\n"
                                "Each file contains a list of records that describe isotopes, elements,\n"
                                "materials and possibly material aliases.                              \n"
                                "The order used to load the files is important because some materials  \n"
                                "may depend on some elements or other materials, elements themselves   \n"
                                "may depend on isotopes. Also material aliases depends on existing     \n"
                                "materials. It is thus recommended to load the files in such a way     \n"
                                "first isotopes, then elements, then materials and finaly material aliases are\n"
                                "described, possibly from separated files for clarity and reusability. \n"
                                "                                                                      \n"
                                "Here is an example of a single file that contains all kinds of objects::\n"
                                "                                                                      \n"
                                "    #@description Definitions for some isotopes, elements and materials\n"
                                "    #@key_label   \"name\"                                           \n"
                                "    #@meta_label  \"type\"                                           \n"
                                "                                                                     \n"
                                "    [name=\"H\" type=\"isotope\"]                                    \n"
                                "    #@config The H hydrogen isotope                                  \n"
                                "    z : integer = 1                                                  \n"
                                "    a : integer = 1                                                  \n"
                                "                                                                     \n"
                                "    [name=\"D\" type=\"isotope\"]                                    \n"
                                "    #@config The deuterium hydrogen isotope                          \n"
                                "    z : integer = 1                                                  \n"
                                "    a : integer = 2                                                  \n"
                                "                                                                     \n"
                                "    [name=\"T\" type=\"isotope\"]                                    \n"
                                "    #@config The tritium hydrogen isotope                            \n"
                                "    z : integer = 1                                                  \n"
                                "    a : integer = 3                                                  \n"
                                "                                                                     \n"
                                "    [name=\"O-16\" type=\"isotope\"]                                 \n"
                                "    #@config The O-16 oxygen isotope                                 \n"
                                "    z : integer = 8                                                  \n"
                                "    a : integer = 16                                                 \n"
                                "                                                                     \n"
                                "    [name=\"O-17\" type=\"isotope\"]                                 \n"
                                "    #@config The O-17 oxygen isotope                                 \n"
                                "    z : integer = 8                                                  \n"
                                "    a : integer = 17                                                 \n"
                                "                                                                     \n"
                                "    [name=\"O-18\" type=\"isotope\"]                                 \n"
                                "    #@config The O-18 oxygen isotope                                 \n"
                                "    z : integer = 8                                                  \n"
                                "    a : integer = 18                                                 \n"
                                "                                                                     \n"
                                "    [name=\"Hydrogen\" type=\"element\"]                             \n"
                                "    #@config The H natural element                                   \n"
                                "    z               : integer   = 1                                  \n"
                                "    isotope.names   : string[2] = \"H\"   \"D\"                      \n"
                                "    isotope.weights : real[2]   = 99.9885 0.0115                     \n"
                                "                                                                     \n"
                                "    [name=\"Oxygen\" type=\"element\"]                               \n"
                                "    #@config The O natural element                                   \n"
                                "    z               : integer   = 8                                  \n"
                                "    isotope.names   : string[3] = \"O-16\" \"O-17\" \"O-18\"         \n"
                                "    isotope.weights : real[3]   =  99.757 0.038  0.205               \n"
                                "                                                                     \n"
                                "    [name=\"Water\" type=\"material\"]                               \n"
                                "    #@config Water material                                          \n"
                                "    density          : real   = 1.0                                  \n"
                                "    density.unit     : string = \"g/cm3\"                            \n"
                                "    temperature      : real   = 300.                                 \n"
                                "    temperature.unit : string = \"kelvin\"                           \n"
                                "    pressure         : real   = 1.                                   \n"
                                "    pressure.unit    : string = \"bar\"                              \n"
                                "    state            : string = \"liquid\"                              \n"
                                "    composition.mode            : string       = \"number_of_atoms\"    \n"
                                "    composition.names           : string  [2]  = \"Hydrogen\" \"Oxygen\"\n"
                                "    composition.number_of_atoms : integer [2]  =      2        1        \n"
                                "    mpt.op.pp        : real [5] = 1.0    2.0  3.0   5.0  10.0           \n"
                                "    mpt.op.rindex    : real [5] = 1.33  1.34  1.35  1.40  1.42          \n"
                                "                                                                        \n"
                                "    [name=\"Fluid\" type=\"alias\"]                                     \n"
                                "    #@config An material alias for water                                \n"
                                "    material : string = \"Water\"                                       \n"
                                "                                                                        \n"
                                );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::materials::manager,"materials::manager")

// end of manager.cc

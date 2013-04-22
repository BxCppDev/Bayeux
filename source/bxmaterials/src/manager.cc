// -*- mode: c++ ; -*-
/* manager.cc
 */

#include <stdexcept>
#include <sstream>

#include <datatools/multi_properties.h>

#include <materials/isotope.h>
#include <materials/element.h>
#include <materials/material.h>
#include <materials/manager.h>

namespace materials {

  using namespace std;
  using namespace datatools;

  bool manager::is_debug () const
  {
    return _debug_;
  }

  void manager::set_debug (bool new_value_)
  {
    _debug_ = new_value_;
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
    if (sr.is_alias ())
      {
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
    _debug_ = false;
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

  void manager::load (const datatools::multi_properties & config_)
  {
    if (is_locked ())
      {
        throw std::logic_error ("materials::manager::load: Manager is locked !");
      }

    for (multi_properties::entries_ordered_col_type::const_iterator i
           = config_.ordered_entries ().begin ();
         i != config_.ordered_entries ().end ();
         i++)
      {
        multi_properties::entry * e = *i;
        const string & name = e->get_key ();
        const string & type = e->get_meta ();
        const datatools::properties & props = e->get_properties ();

        if (_debug_)
          {
            clog << "DEBUG: " << "materials::manager::load: " << "Name = '" << name << "'" << endl;
            clog << "DEBUG: " << "materials::manager::load: " << "Type = '" << type << "'" << endl;
          }
        if (type == "isotope")
          {
            if (_isotopes_.find (name) != _isotopes_.end ())
              {
                std::ostringstream message;
                message << "materials::manager::load: "
                        << "Isotope with name '" << name << "' already exists !";
                throw std::logic_error (message.str ());
              }
            isotope * iso = _creator_.create_isotope (name, props);
            _isotopes_[iso->get_name ()] = materials::smart_ref<isotope> ();
            _isotopes_[iso->get_name ()].set_ref (iso);
            if (_debug_) clog << "DEBUG: " << "materials::manager::load: " << "Add new isotope = '" << iso->get_zai_name () << "'" << endl;
          }
        else if (type == "element")
          {
            if (_elements_.find (name) != _elements_.end ())
              {
                std::ostringstream message;
                message << "materials::manager::load: "
                        << "Element with name '" << name << "' already exists !";
                throw std::logic_error (message.str ());
              }
            bool unique_element_material = false;
            if (unique_element_material)
              {
                if (_materials_.find (name) != _materials_.end ())
                  {
                    std::ostringstream message;
                    message << "materials::manager::load: "
                            << "Material with name '" << name << "' already exists !";
                    throw std::logic_error (message.str ());
                  }
              }
            element * elmt = _creator_.create_element (name, props, _isotopes_);
            _elements_[elmt->get_name ()] = materials::smart_ref<element> ();
            _elements_[elmt->get_name ()].set_ref (elmt);
            if (_debug_) clog << "DEBUG: " << "materials::manager::load: " << "Add new element = '" << elmt->get_name () << "'" << endl;
          }
        else if (type == "material")
          {
            if (_materials_.find (name) != _materials_.end ())
              {
                std::ostringstream message;
                message << "materials::manager::load: "
                        << "Material with name '" << name << "' already exists !";
                throw std::logic_error (message.str ());
              }
            bool unique_element_material = false;
            if (unique_element_material)
              {
                if (_elements_.find (name) != _elements_.end ())
                  {
                    std::ostringstream message;
                    message << "materials::manager::load: "
                            << "Elements with name '" << name << "' already exists !";
                    throw std::logic_error (message.str ());
                  }
              }
            material * matl = _creator_.create_material (name,
                                                         props,
                                                         _elements_,
                                                         _materials_);
            _materials_[matl->get_name ()] = materials::smart_ref<material> ();
            _materials_[matl->get_name ()].set_ref (matl);
            if (_debug_) clog << "DEBUG: " << "materials::manager::load: "
                              << "Add new material = '"
                              << matl->get_name () << "'" << endl;
            _ordered_materials_.push_back (matl->get_name ());
          }
        else if (type == "alias")
          {
            material_dict_type::const_iterator mat_found = _materials_.find (name);
            if (mat_found != _materials_.end ())
              {
                const smart_ref<material> & sref = mat_found->second;
                if (! sref.is_alias())
                  {
                    // Already existing plain material:
                    std::ostringstream message;
                    message << "materials::manager::load: "
                            << "Material with name '" << name
                            << "' already exists ! It cannot be overloaded !";
                    throw std::logic_error (message.str ());
                  }
                else
                  {
                    // Already existing material alias:
                    if (! is_alias_allow_overload())
                      {
                        std::ostringstream message;
                        message << "materials::manager::load: "
                                << "Alias with name '" << name
                                << "' cannot be overloaded !";
                        throw std::logic_error (message.str ());
                      }
                    else
                      {
                        std::clog << "WARNING: "
                                  << "materials::manager::load: "
                                  << "Redefinition of alias '" << name << "' !"
                                  << std::endl;
                      }
                  }
              }
            if (! props.has_key ("material"))
              {
                std::ostringstream message;
                message << "materials::manager::load: "
                        << "Missing property 'material' for a material alias !";
                throw std::logic_error (message.str ());
              }
            std::string alias_material = props.fetch_string ("material");
            material_dict_type::iterator found = _materials_.find (alias_material);
            if (found == _materials_.end ())
              {
                std::ostringstream message;
                message << "materials::manager::load: "
                        << "Aliased material named '" << alias_material << "' does not exist !";
                throw std::logic_error (message.str ());
              }
            if (found->second.is_alias())
              {
                // An alias:
                std::ostringstream message;
                message << "materials::manager::load: "
                        << "Material alias with name '" << name
                        << "' cannot refer itself to another alias ('" << alias_material << "') !";
                throw std::logic_error (message.str ());
              }
            _materials_[name] = materials::smart_ref<material> ();
            _materials_[name].set_ref (found->second.grab_ref ());
            _materials_[name].set_alias_of(alias_material);
            std::list<std::string>::iterator ofound = std::find(_ordered_materials_.begin(),_ordered_materials_.end(), name);
            if (ofound != _ordered_materials_.end())
              {
                _ordered_materials_.erase(ofound);
              }
            _ordered_materials_.push_back (name);
            if (_debug_) clog << "DEBUG: " << "materials::manager::load: " << "Add new material alias = '" << name << "' for material '" << alias_material << "'" << endl;
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
    if (! title_.empty ())
      {
        out_ << indent << title_ << endl;
      }

    out_ << indent << i_tree_dumpable::tag
         << "Debug : " << is_debug () << endl;

    {
      out_ << indent << du::i_tree_dumpable::tag
           << "Isotopes";
      if ( _isotopes_.size () == 0)
        {
          out_ << " : <empty>" << endl;
        }
      else
        {
          out_ << " [" << _isotopes_.size () << "] :" << endl;
          for (isotope_dict_type::const_iterator i = _isotopes_.begin ();
               i != _isotopes_.end ();
               i++)
            {
              std::ostringstream indent_oss;
              indent_oss << indent;
              indent_oss << du::i_tree_dumpable::skip_tag;
              isotope_dict_type::const_iterator j = i;
              j++;
              out_ << indent << du::i_tree_dumpable::skip_tag;;
              if (j != _isotopes_.end ())
                {
                  out_ << du::i_tree_dumpable::tag;
                  indent_oss << du::i_tree_dumpable::skip_tag;;
                }
              else
                {
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
      if ( _elements_.size () == 0)
        {
          out_ << " : <empty>" << endl;
        }
      else
        {
          out_ << " [" << _elements_.size () << "] :" << endl;
          for (element_dict_type::const_iterator i = _elements_.begin ();
               i != _elements_.end ();
               i++)
            {
              std::ostringstream indent_oss;
              indent_oss << indent;
              indent_oss << du::i_tree_dumpable::skip_tag;
              element_dict_type::const_iterator j = i;
              j++;
              out_ << indent << du::i_tree_dumpable::skip_tag;;
              if (j != _elements_.end ())
                {
                  out_ << du::i_tree_dumpable::tag;
                  indent_oss << du::i_tree_dumpable::skip_tag;;
                }
              else
                {
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
      if ( _materials_.size () == 0)
        {
          out_ << " : <empty>" << endl;
        }
      else
        {
          out_ << " [" << _materials_.size () << "] :" << endl;
          for (material_dict_type::const_iterator i = _materials_.begin ();
               i != _materials_.end ();
               i++)
            {
              std::ostringstream indent_oss;
              indent_oss << indent;
              indent_oss << du::i_tree_dumpable::skip_tag;
              material_dict_type::const_iterator j = i;
              j++;
              out_ << indent << du::i_tree_dumpable::skip_tag;;
              if (j != _materials_.end ())
                {
                  out_ << du::i_tree_dumpable::tag;
                  indent_oss << du::i_tree_dumpable::skip_tag;;
                }
              else
                {
                  out_ << du::i_tree_dumpable::last_tag;
                  indent_oss << du::i_tree_dumpable::last_skip_tag;;
                }
              out_ << "Material '" << i->first << "'";
              if (i->second.is_alias ())
                {
                  out_ << " (alias of '" << i->second.get_alias_of() << "')";
                  out_ << endl;
                }
              else
                {
                  if (! i->second.is_owned ())
                    {
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
      if ( _ordered_materials_.size () == 0)
        {
          out_ << " : <empty>" << endl;
        }
      else
        {
          out_ << " [" << _ordered_materials_.size () << "] :" << endl;
          for (std::list<std::string>::const_iterator i = _ordered_materials_.begin ();
               i != _ordered_materials_.end ();
               i++)
            {
              std::ostringstream indent_oss;
              indent_oss << indent;
              indent_oss << du::i_tree_dumpable::skip_tag;
              std::list<std::string>::const_iterator j = i;
              j++;
              out_ << indent << du::i_tree_dumpable::skip_tag;
              if (j != _ordered_materials_.end ())
                {
                  out_ << du::i_tree_dumpable::tag;
                  indent_oss << du::i_tree_dumpable::skip_tag;
                }
              else
                {
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
  ocd_.set_class_description("A manager for isotopes, elements and materials");
  ocd_.set_class_library("materials");
  ocd_.set_class_documentation("not documented yet");

  ocd_.set_configuration_hints ("A materials manager is configured through a set of configuration      \n"
                                "files that obeys the format of 'datatools::multi_properties' setup    \n"
                                "file.                                                                 \n"
                                "The method used to instantiate isotopes, elements and materials       \n"
                                "objects is :                                                          \n"
                                "   void manager::load (const datatools::multi_properties & config_);  \n"
                                "Each file contains a list of records that describe isotopes, elements,\n"
                                "materials and possibly material aliases.                              \n"
                                "The order used to load the files is important because some materials  \n"
                                "may depend on some elements or other materials, elements themselves   \n"
                                "may depend on isotopes. Also material aliases depends on existing     \n"
                                "materials. It is thus recommended to load the files in such a way     \n"
                                "isotopes, elements, then materials than material aliases are          \n"
                                "described, possibly from separated files for clarity and reusability. \n"
                                "                                                                      \n"
                                "Here is an example of a single file that contains all kind of objects:\n"
                                "  |                                                                   \n"
                                "  | #@key_label   \"name\"                                           \n"
                                "  | #@meta_label  \"type\"                                           \n"
                                "  |                                                                  \n"
                                "  | [name=\"H\" type=\"isotope\"]                                    \n"
                                "  | #@config The H hydrogen isotope                                  \n"
                                "  | z : integer = 1                                                  \n"
                                "  | a : integer = 1                                                  \n"
                                "  |                                                                  \n"
                                "  | [name=\"D\" type=\"isotope\"]                                    \n"
                                "  | #@config The deuterium hydrogen isotope                          \n"
                                "  | z : integer = 1                                                  \n"
                                "  | a : integer = 2                                                  \n"
                                "  |                                                                  \n"
                                "  | [name=\"T\" type=\"isotope\"]                                    \n"
                                "  | #@config The tritium hydrogen isotope                            \n"
                                "  | z : integer = 1                                                  \n"
                                "  | a : integer = 3                                                  \n"
                                "  |                                                                  \n"
                                "  | [name=\"O-16\" type=\"isotope\"]                                 \n"
                                "  | #@config The O-16 oxygen isotope                                 \n"
                                "  | z : integer = 8                                                  \n"
                                "  | a : integer = 16                                                 \n"
                                "  |                                                                  \n"
                                "  | [name=\"O-17\" type=\"isotope\"]                                 \n"
                                "  | #@config The O-17 oxygen isotope                                 \n"
                                "  | z : integer = 8                                                  \n"
                                "  | a : integer = 17                                                 \n"
                                "  |                                                                  \n"
                                "  | [name=\"O-18\" type=\"isotope\"]                                 \n"
                                "  | #@config The O-18 oxygen isotope                                 \n"
                                "  | z : integer = 8                                                  \n"
                                "  | a : integer = 18                                                 \n"
                                "  |                                                                  \n"
                                "  | [name=\"Hydrogen\" type=\"element\"]                             \n"
                                "  | #@description The H natural element                              \n"
                                "  | z               : integer   = 1                                  \n"
                                "  | isotope.names   : string[2] = \"H\"   \"D\"                      \n"
                                "  | isotope.weights : real[2]   = 99.9885 0.0115                     \n"
                                "  |                                                                  \n"
                                "  | [name=\"Oxygen\" type=\"element\"]                               \n"
                                "  | #@description The O natural element                              \n"
                                "  | z               : integer   = 8                                  \n"
                                "  | isotope.names   : string[3] = \"O-16\" \"O-17\" \"O-18\"         \n"
                                "  | isotope.weights : real[3]   =  99.757 0.038  0.205               \n"
                                "  |                                                                  \n"
                                "  | [name=\"Water\" type=\"material\"]                               \n"
                                "  | #@config Water material                                          \n"
                                "  | density          : real   = 1.0                                  \n"
                                "  | density.unit     : string = \"g/cm3\"                            \n"
                                "  | temperature      : real   = 300.                                 \n"
                                "  | temperature.unit : string = \"kelvin\"                           \n"
                                "  | pressure         : real   = 1.                                   \n"
                                "  | pressure.unit    : string = \"bar\"                              \n"
                                "  | state            : string = \"liquid\"                              \n"
                                "  | composition.mode            : string       = \"number_of_atoms\"    \n"
                                "  | composition.names           : string  [2]  = \"Hydrogen\" \"Oxygen\"\n"
                                "  | composition.number_of_atoms : integer [2]  =      2        1        \n"
                                "  | mpt.op.pp        : real [5] = 1.0    2.0  3.0   5.0  10.0           \n"
                                "  | mpt.op.rindex    : real [5] = 1.33  1.34  1.35  1.40  1.42          \n"
                                "  |                                                                     \n"
                                "  | [name=\"Fluid\" type=\"alias\"]                                     \n"
                                "  | #@config An material alias for water                                \n"
                                "  | material : string = \"Water\"                                       \n"
                                "  |                                                                     \n"
                                );

  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::materials::manager,"materials::manager")

// end of manager.cc

// -*- mode: c++ ; -*-
/* manager.cc
 */

#include <materials/manager.h>
#include <datatools/multi_properties.h>

#include <materials/isotope.h>
#include <materials/element.h>
#include <materials/material.h>

#include <stdexcept>
#include <sstream>

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
        throw runtime_error ("materials::manager::load: Manager is locked !");
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
                ostringstream message;
                message << "materials::manager::load: "
                        << "Isotope with name '" << name << "' already exists !";
                throw runtime_error (message.str ());
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
                ostringstream message;
                message << "materials::manager::load: "
                        << "Element with name '" << name << "' already exists !";
                throw runtime_error (message.str ());
              }
            bool unique_element_material = false;
            if (unique_element_material)
              {
                if (_materials_.find (name) != _materials_.end ())
                  {
                    ostringstream message;
                    message << "materials::manager::load: "
                            << "Material with name '" << name << "' already exists !";
                    throw runtime_error (message.str ());
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
                ostringstream message;
                message << "materials::manager::load: "
                        << "Material with name '" << name << "' already exists !";
                throw runtime_error (message.str ());
              }
            bool unique_element_material = false;
            if (unique_element_material)
              {
                if (_elements_.find (name) != _elements_.end ())
                  {
                    ostringstream message;
                    message << "materials::manager::load: "
                            << "Elements with name '" << name << "' already exists !";
                    throw runtime_error (message.str ());
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
            if (_materials_.find (name) != _materials_.end ())
              {
                ostringstream message;
                message << "materials::manager::load: "
                        << "Material with name '" << name
                        << "' already exists !";
                throw runtime_error (message.str ());
              }
            bool unique_element_material = false;
            if (unique_element_material)
              {
                if (_elements_.find (name) != _elements_.end ())
                  {
                    ostringstream message;
                    message << "materials::manager::load: "
                            << "Elements with name '"
                            << name << "' already exists !";
                    throw runtime_error (message.str ());
                  }
              }
            if (! props.has_key ("material"))
              {
                ostringstream message;
                message << "materials::manager::load: "
                        << "Missing property 'material' for a material alias !";
                throw runtime_error (message.str ());
              }
            string alias_material = props.fetch_string ("material");
            material_dict_type::iterator found = _materials_.find (alias_material);
            if (found == _materials_.end ())
              {
                ostringstream message;
                message << "materials::manager::load: "
                        << "Aliased material named '" << alias_material << "' does not exist !";
                throw runtime_error (message.str ());
              }
            _materials_[name] = materials::smart_ref<material> ();
            _materials_[name].set_ref (found->second.get_ref ());
            _ordered_materials_.push_back (name);
            if (_debug_) clog << "DEBUG: " << "materials::manager::load: " << "Add new material alias = '" << name << "'" << endl;
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
              ostringstream indent_oss;
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
              ostringstream indent_oss;
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
              ostringstream indent_oss;
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
              if (! i->second.is_owned ())
                {
                  out_ << " (alias)";
                }
              out_ << " : " << endl;
              i->second.get_ref ().tree_dump (out_, "", indent_oss.str ());
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
          for (list<string>::const_iterator i = _ordered_materials_.begin ();
               i != _ordered_materials_.end ();
               i++)
            {
              ostringstream indent_oss;
              indent_oss << indent;
              indent_oss << du::i_tree_dumpable::skip_tag;
              list<string>::const_iterator j = i;
              j++;
              out_ << indent << du::i_tree_dumpable::skip_tag;;
              if (j != _ordered_materials_.end ())
                {
                  out_ << du::i_tree_dumpable::tag;
                  indent_oss << du::i_tree_dumpable::skip_tag;;
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

// end of manager.cc

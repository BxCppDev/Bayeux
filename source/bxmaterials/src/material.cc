// -*- mode: c++ ; -*-
/* material.cc
 */

#include <cstdlib>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

#include <datatools/clhep_units.h>
#include <datatools/units.h>

#include <datatools/exception.h>
#include <datatools/logger.h>

#include <materials/materials_config.h>
#include <materials/material.h>
#include <materials/isotope.h>
#include <materials/element.h>

namespace materials {

  using namespace std;

  const double material::g_per_cm3 = CLHEP::g / CLHEP::cm3;

  //________________________________________________________________________
  compound_entry::compound_entry ()
  {
    owned = false;

    elt_ptr = 0;
    mat_ptr = 0;

    weight      = -1;
    nb_of_atoms = -1;
    return;
  }

  //________________________________________________________________________
  compound_entry::~compound_entry ()
  {
    if (elt_ptr != 0) {
      if (owned) delete elt_ptr;
      elt_ptr = 0;
    }

    if (mat_ptr != 0) {
      if (owned) delete mat_ptr;
      mat_ptr = 0;
    }

    owned = false;

    weight = -1;
    nb_of_atoms = -1;
    return;
  }

  //________________________________________________________________________
  bool compound_entry::is_valid () const
  {
    return ((weight > 0.0) || (nb_of_atoms > 0.0));
  }

  //________________________________________________________________________
  bool material::is_composed_by_mean_z_a () const
  {
    return _proportion_unit_ == MEAN_ZA;
  }

  //________________________________________________________________________
  bool material::is_composed_by_number_of_atoms () const
  {
    return _proportion_unit_ == NUMBER_OF_ATOMS;
  }

  //________________________________________________________________________
  bool material::is_composed_by_fraction_mass () const
  {
    return _proportion_unit_ == FRACTION_MASS;
  }

  //________________________________________________________________________
  const composition_map_type & material::get_composition () const
  {
    return _composition_;
  }

  //________________________________________________________________________   ctor:
  material::material ()
  {
    _name_ = "";
    _proportion_unit_ = KP_UNDEFINED;
    _locked_ = false;
    _density_ = -1.0 * g_per_cm3;
    _mean_z_ = -1.0;
    _mean_a_ = -1.0;
    return;
  }

  //________________________________________________________________________   ctor:
  material::material (const string & name_)
  {
    set_name (name_);
    _density_ = -1.0 * g_per_cm3;
    _locked_ = false;
    return;
  }

  //________________________________________________________________________   dtor:
  material::~material ()
  {
    return;
  }

  //________________________________________________________________________
  const string & material::get_name () const
  {
    return _name_;
  }

  //________________________________________________________________________
  void material::set_name (const string & name_)
  {
    _name_ = name_;
    return;
  }

  //________________________________________________________________________
  double  material::get_mean_z () const
  {
    return _mean_z_;
  }

  //________________________________________________________________________
  void  material::_set_mean_z_ (double z_)
  {
    DT_THROW_IF (z_ <= 0.0,
                 std::logic_error,
                 "Invalid value for atomic number !");
    _mean_z_ = z_;
    return;
  }

  //________________________________________________________________________
  double  material::get_mean_a () const
  {
    return _mean_a_;
  }

  //________________________________________________________________________
  void  material::_set_mean_a_ (double a_)
  {
    DT_THROW_IF (a_ <= 0.0,
                 std::logic_error,
                 "Invalid value for mass number !");
    _mean_a_ = a_;
    return;
  }

  //________________________________________________________________________
  void material::set_mean_z_a (double z_, double a_)
  {
    DT_THROW_IF (is_locked (),std::logic_error, "Operation not allowed ! Object is locked !");
    if (_proportion_unit_ == KP_UNDEFINED) {
      _proportion_unit_ = KP_ZA;
    } else {
      DT_THROW_IF (_proportion_unit_ != KP_ZA,
                   std::logic_error,
                   "Setting Z/A is not allowed in this mode !");
    }
    _set_mean_z_ (z_);
    _set_mean_a_ (a_);
    return;
  }

  //________________________________________________________________________
  double material::get_density () const
  {
    return _density_;
  }

  //________________________________________________________________________
  void material::set_density (double density_)
  {
    DT_THROW_IF (density_ <= 0.0,
                 std::logic_error,
                 "Invalid value for density !");
    _density_ = density_;
    return;
  }

  //________________________________________________________________________
  void material::add_element_by_mass (const element & elt_ref_ , double weight_ , bool owned_ )
  {
    DT_THROW_IF (is_locked (),std::logic_error, "Operation not allowed ! Object is locked !");

    // check proportion_unit_type when add a new element
    // if you are adding the first element then set the proportion_unit

    if ( _composition_.size () == 0) {
      _proportion_unit_ = KP_MASS;
    } else {
      DT_THROW_IF(_proportion_unit_!=KP_MASS,
                  std::logic_error,
                  "Unmatching proportion unit  !");
    }

    // check if the element is locked (which mean valid)

    DT_THROW_IF (! elt_ref_.is_locked (),
                 std::logic_error,
                 "Element '" << elt_ref_.get_name () << "' is not locked !'");

    // check the positive value of weight

    DT_THROW_IF ( weight_ <= 0.,
                  std::logic_error,
                  "Weight value '" << weight_  << "' is not positive !");

    compound_entry entry;
    entry.owned = false;
    entry.elt_ptr = &elt_ref_;
    entry.weight = weight_;

    _composition_[entry.elt_ptr->get_name ()] = entry;
    _composition_[entry.elt_ptr->get_name ()].owned = owned_;
    return;
  }

  //________________________________________________________________________
  void  material::add_element_by_mass (const element * elt_ptr_ , double weight_  )
  {
    add_element_by_mass (*elt_ptr_, weight_);
    return;
  }

  //________________________________________________________________________
  void material::add_element_by_nb_of_atoms(const element & elt_ref_ , int nb_of_atoms_ , bool owned_ )
  {
    DT_THROW_IF (is_locked (),std::logic_error, "Operation not allowed ! Object is locked !");

    // check proportion_unit_type when add a new element
    // if you are adding the first element then set the proportion_unit

    if (_composition_.size () == 0) {
      _proportion_unit_ = KP_ATOM;
    } else {
      DT_THROW_IF (_proportion_unit_ != KP_ATOM,
                   std::logic_error,
                   "Unmatching proportion unit  !");
    }

    // check if the element is locked (which mean valid)

    DT_THROW_IF (! elt_ref_.is_locked (),
                 std::logic_error,
                 "Element '" << elt_ref_.get_name () << "' is not locked !'");

    // check the positive value of weight

    DT_THROW_IF ( nb_of_atoms_ <= 0.,
                  std::logic_error,
                  "Nb of atoms value '" << nb_of_atoms_ << "' is not positive !");

    compound_entry entry;
    entry.owned = false;
    entry.elt_ptr = &elt_ref_;
    entry.nb_of_atoms = nb_of_atoms_;

    _composition_[entry.elt_ptr->get_name ()] = entry;
    _composition_[entry.elt_ptr->get_name ()].owned = owned_;
    return;
  }

  //________________________________________________________________________
  void  material::add_element_by_nb_of_atoms (const element * elt_ptr_ ,  int nb_of_atoms_  )
  {
    add_element_by_nb_of_atoms (*elt_ptr_, nb_of_atoms_);
    return;
  }


  //________________________________________________________________________
  void material::add_material_by_mass(const material & mat_ref_ , double weight_ , bool owned_ )
  {
    DT_THROW_IF (is_locked (),std::logic_error, "Operation not allowed ! Object is locked !");

    // check proportion_unit_type when add a new element
    // if you are adding the first element then set the proportion_unit

    if ( _composition_.size () == 0) {
      _proportion_unit_ = KP_MASS;
    } else {
      DT_THROW_IF(_proportion_unit_ != KP_MASS,
                  std::logic_error,
                  "Unmatching proportion unit  !");
    }

    // check if the element is locked (which mean valid)

    DT_THROW_IF (! mat_ref_.is_locked (),
                 std::logic_error,
                 "Material '" << mat_ref_.get_name ()
                 << "' is not locked !'");

    // check the positive value of weight

    DT_THROW_IF ( weight_ <= 0.,
                  std::logic_error,
                  "Material weight value '" << weight_ << "' is not positive !");

    compound_entry entry;
    entry.owned = false;
    entry.mat_ptr = &mat_ref_;
    entry.weight = weight_;

    _composition_[entry.mat_ptr->get_name ()] = entry;
    _composition_[entry.mat_ptr->get_name ()].owned = owned_;

    return;
  }

  //________________________________________________________________________
  void  material::add_material_by_mass (const material * mat_ptr_ , double weight_  )
  {
    add_material_by_mass (*mat_ptr_, weight_);
    return;
  }

  //________________________________________________________________________
  void material::build ()
  {
    bool devel = false;
    //devel = true;
    if (devel) std::cerr << "DEVEL: material::build: Entering..." << std::endl;
    DT_THROW_IF (is_locked (),std::logic_error, "Operation not allowed ! Object is locked !");
    DT_THROW_IF (_density_ <= 0.0, std::logic_error, "Invalid density !");
    if (is_composed_by_mean_z_a ()) {
      if (devel) std::cerr << "DEVEL: material::build: by mean Z/A..." << std::endl;
      DT_THROW_IF (_mean_z_ < 0.0, std::logic_error, "Invalid mean atomic number (Z) !");
      DT_THROW_IF (_mean_a_ < 0.0, std::logic_error, "Invalid mean mass number (A) !");
    } else {
      if (devel) std::cerr << "DEVEL: material::build: by composition: " << _composition_.size () << std::endl;
      DT_THROW_IF(_composition_.size () <= 0,
                  std::logic_error,
                  "Missing compound(s) in the current material ! Not implemented yet !");
      if (is_composed_by_fraction_mass ()) {
        if (devel) std::cerr << "DEVEL: material::build: by fraction mass..." << std::endl;
       _normalize_weights_ ();
      }
      _compute_molar_mass_ ();
    }
    _lock_ ();
    if (devel) std::cerr << "DEVEL: material::build: Exiting." << std::endl;
    return;
  }

  //________________________________________________________________________
  void material::_normalize_weights_ ()
  {
    DT_THROW_IF (! is_composed_by_fraction_mass (),
                 std::logic_error, "Invalid composition mode !");
    DT_THROW_IF (_composition_.size () == 0,
                 std::logic_error,
                 "Empty composition map !");
    // compute the sum of weight (fraction of mass):
    double sum_weight = 0.0;
    for (composition_map_type::const_iterator i
           = _composition_.begin ();
         i != _composition_.end ();
         i++) {
      const compound_entry & entry = i->second;
      sum_weight += entry.weight;
    }
    // normalize all weights (fraction of mass):
    for (composition_map_type::iterator i = _composition_.begin ();
         i != _composition_.end ();
         i++) {
      compound_entry & entry = i->second;
      entry.weight /= sum_weight;
    }
    return;
  }

  //________________________________________________________________________
  void  material::_compute_molar_mass_ ()
  {
    /*  double molar_mass =0.;

        for (isotope_weight_map_t::const_iterator i = _composition_.begin ();
        i != _composition_.end ();
        i++)
        {
        const iso_entry & entry = i->second;
        molar_mass += (entry.weight) * (entry.iso_ptr->get_mass ());
        }
        __set_molar_mass (molar_mass);
    */
    return;
  }

  //________________________________________________________________________
  void material::_lock_ ()
  {
    _locked_ = true;
    return;
  }


  //________________________________________________________________________
  void material::tree_dump (ostream & out_,
                            const string & title_,
                            const string & indent_,
                            bool inherit_) const
  {
    string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ())
      {
        out_ << indent << title_ << endl;
      }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Name         : \"" << get_name() <<"\"" << endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Density      : " << get_density() / g_per_cm3 << " g/cm3" << endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Composition  : ";
    if (is_composed_by_mean_z_a ()) {
      out_ << endl;
      out_ << indent << datatools::i_tree_dumpable::skip_tag
           << datatools::i_tree_dumpable::tag << "Mean Z : " <<_mean_z_ << endl;
      out_ << indent << datatools::i_tree_dumpable::skip_tag
           << datatools::i_tree_dumpable::last_tag << "Mean A : "
           << _mean_a_ << " g/mol" << endl;
    } else {
      if ( _composition_.size () == 0) {
        out_ << " <empty>" << endl;
      } else {
        if (_proportion_unit_ == KP_ATOM) {
          out_ << "(# atoms) "<< endl;
          for (composition_map_type::const_iterator i
                 = _composition_.begin ();
               i != _composition_.end ();
               i++) {
            const string & key = i->first;
            const compound_entry & entry = i->second;
            composition_map_type::const_iterator j = i;
            j++;
            string atag = datatools::i_tree_dumpable::TAG;
            if (j == _composition_.end ()) atag = datatools::i_tree_dumpable::LAST_TAG;
            out_ << indent << datatools::i_tree_dumpable::skip_tag << atag << "Element '"
                 << entry.elt_ptr->get_name () << "'" << " : "
                 << entry.nb_of_atoms << " "
                 << (entry.owned? "(owned)": "(not owned)") << endl;
          }
        }

        if (_proportion_unit_ == KP_MASS) {
          out_ << "(% mass)  "<< endl;
          for (composition_map_type::const_iterator i
                 = _composition_.begin ();
               i != _composition_.end ();
               i++) {
            const string & key = i->first;
            const compound_entry & entry = i->second;
            composition_map_type::const_iterator j = i;
            j++;
            string atag = datatools::i_tree_dumpable::TAG;
            if (j == _composition_.end ()) atag = datatools::i_tree_dumpable::LAST_TAG;
            out_ << indent << datatools::i_tree_dumpable::skip_tag << atag;
            if (entry.mat_ptr == 0) {
              out_  << "Element '" << entry.elt_ptr->get_name ();
            } else {
              out_  << "Material '" << entry.mat_ptr->get_name ();
            }
            out_ << "'" << " : " << entry.weight << " "
                 << (entry.owned? "(owned)": "(not owned)") << endl;
          }
        }
      }
    }

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Properties   : ";
      if ( _properties_.size () == 0) {
        out_ << "<empty>" << endl;
      } else {
        out_ << endl;
        ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _properties_.tree_dump (out_,"",indent_oss.str ());
      }
    }

    {
      out_ << indent << i_tree_dumpable::last_tag
           << "Locked       : " << (is_locked()? "Yes": "No") << endl;
    }

    return;

  }

} // end of namespace materials

// end of material.cc

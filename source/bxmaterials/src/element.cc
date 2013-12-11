// -*- mode: c++ ; -*-
/* element.cc
 */
// - Ourselves
#include <materials/element.h>

// Standard Library
#include <stdexcept>
#include <sstream>
#include <fstream>

// Third Party
// - datatools
#include <datatools/exception.h>
#include <datatools/logger.h>

// This Project
#include <materials/resource.h>
#include <materials/materials_config.h>
#include <materials/isotope.h>
#include <materials/chemical_symbol.h>

namespace materials {

  using namespace std;

  //________________________________________________________________________// iso_entry ctor
  iso_entry::iso_entry ()
  {
    owned = false;
    iso_ptr = 0;
    weight = -1.0;
    return;
  }


  //________________________________________________________________________// iso_entry dtor
  iso_entry::~iso_entry ()
  {
    if(iso_ptr != 0){
      if (owned) delete iso_ptr;
      iso_ptr = 0;
    }
    owned = false;
    weight = -1.0;
    return;
  }


  //________________________________________________________________________//  iso_entry is_valid ()
  bool iso_entry::is_valid () const
  {
    return weight > 0.0;
  }

  bool element::is_built_by_isotopic_composition () const
  {
    return (is_locked () && _composition_.size () > 0);
  }

  //________________________________________________________________________// element ctor
  element::element ()
  {
    _locked_ = false;
    _molar_mass_ = -1.;
    _name_   = "?";
    _symbol_ = "?";
    _z_ = Z_UNDEFINED;
    return;
  }


  //________________________________________________________________________// element ctor
  element::element (const string & name_, int z_, bool build_)
  {
    _locked_ = false;
    _molar_mass_ = -1.;
    _name_   = "?";
    _symbol_ = "?";
    _z_ = Z_UNDEFINED;
    set_name (name_);
    set_z (z_);
    if (build_) build ();
    return;
  }

  //________________________________________________________________________// element ctor
  element:: element (const string & name_, int z_, double molar_mass_, bool build_)
  {
    _locked_ = false;
    _molar_mass_ = -1.;
    _name_   = "?";
    _symbol_ = "?";
    _z_ = Z_UNDEFINED;
    set_name (name_);
    set_z (z_);
    set_molar_mass (molar_mass_);
    if (build_) build ();
  }

  //________________________________________________________________________// element ctor
  element::element (const  string & name_, const string & symbol_, bool build_)
  {
    _locked_ = false;
    _molar_mass_ = -1.;
    _name_   = "?";
    _symbol_ = "?";
    _z_ = Z_UNDEFINED;
    set_name (name_);
    set_z (symbol_);
    if (build_) build ();
    return;
  }

  //________________________________________________________________________//
  const isotope_weight_map_type & element::get_composition () const
  {
    return _composition_;
  }


  //________________________________________________________________________// /dtor
  element::~element ()
  {
    _composition_.clear ();
    return;
  }

  //________________________________________________________________________
  void element::_lock_ ()
  {
    _locked_ = true;
    return;
  }

  //________________________________________________________________________
  void element::_unlock_ ()
  {
    _locked_ = false;
    return;
  }

  //________________________________________________________________________
  void element::set_name (const string & name_)
  {
    DT_THROW_IF(is_locked(),std::logic_error,"Operation not allowed ! Object is locked !");
    _name_ = name_;
    return;
  }

  //________________________________________________________________________
  void element::set_z (const string & symbol_)
  {
    DT_THROW_IF(is_locked(),std::logic_error,"Operation not allowed ! Object is locked !");

    bool is_symbol_found = false;
    size_t i_Z = 0;

    while (i_Z < chemical_symbol::NB_CHEMICAL_SYMBOLS)
      {
        if (symbol_ == chemical_symbol::table[i_Z])
          {
            is_symbol_found = true;
            break;
          }
        i_Z++;
      }

    DT_THROW_IF(! is_symbol_found,
                std::logic_error,
                "Symbol '" << symbol_<< "' not found !");
    _symbol_ = symbol_;
    _z_ = i_Z;
    return;
  }
  //________________________________________________________________________
  void element::set_z (const int z_)
  {
    DT_THROW_IF(is_locked(),std::logic_error,"Operation not allowed ! Object is locked !");

    if ((z_ >= 0) && (z_ <= (int)chemical_symbol::NB_CHEMICAL_SYMBOLS))
      {
        _z_ = z_;
        _symbol_ = chemical_symbol::table[_z_];
      }
    else
      {
        DT_THROW_IF(true,std::logic_error,"Invalid Z value : '" << z_ << "' !");
      }
    return;
  }

  //________________________________________________________________________
  void element::add_isotope (const isotope & iso_ptr_,  double weight_,  bool owned_)
  {
    DT_THROW_IF(is_locked(),std::logic_error,"Operation not allowed ! Object is locked !");

    // 2010-10-11 by FM: not compatible with molar mass manual setting:
    DT_THROW_IF ((_molar_mass_ > 0.0) && (_composition_.size () == 0),
                 std::logic_error,
                 "Operation not allowed ! Molar mass is set by hand !");
    // set Z and symbol when add a new isotope.

    if ( _composition_.size () == 0)
      {
        if (_z_ != Z_UNDEFINED)
          {
            DT_THROW_IF (_z_ != iso_ptr_.get_z (),
                         std::logic_error,
                         "Unmatching mother element/daughter isotope Z values !");
          }
        set_z (iso_ptr_.get_z ());
      }

    // check if the isotope is locked (which mean valid)

    DT_THROW_IF (! iso_ptr_.is_locked (),
                 std::logic_error,
                 "Isotope '" << iso_ptr_.get_name () << "' is not locked !'");
    // check if Z value of isotope to be added is consistant with Z value of the element

    DT_THROW_IF ( iso_ptr_.get_z () != _z_,
                  std::logic_error,
                  "The Z value '" << iso_ptr_.get_z () <<"' of isotope '" << iso_ptr_.get_name ()
                  << "' to be added is not consistant with the Z value '"
                  << _z_ << "' of the element '" << get_name () << "' !");

    // check the positive value of weight

    DT_THROW_IF ( weight_ <= 0.,
                  std::logic_error,
                  "weight value '" << weight_ << "' is not positive !");

    // check if isotope is already in the composition of the element

    DT_THROW_IF (_composition_.find (iso_ptr_.get_name ()) != _composition_.end (),
                 std::logic_error,
                 "Adding the isotope '" << iso_ptr_.get_name ()
                 << "' which is already in the composition of element '" << _name_ <<"' !");

    iso_entry entry;
    entry.owned = false;
    entry.iso_ptr = &iso_ptr_;
    entry.weight = weight_;

    _composition_[entry.iso_ptr->get_name ()] = entry;
    _composition_[entry.iso_ptr->get_name ()].owned = owned_;
    return;
  }


  //________________________________________________________________________
  void  element::add_isotope (const isotope * iso_ptr_, double weight_)
  {
    add_isotope (*iso_ptr_, weight_, true);
    return;
  }


  //________________________________________________________________________
  void element::build ()
  {
    if (is_locked ()) return;
    if (_composition_.size () > 0)
      {
        _norm_weights_ ();
        _compute_molar_mass_ ();
      }
    else if (_molar_mass_ > 0.0)
      {
        // ok
      }
    else
      {
        DT_THROW_IF(true, std::logic_error, "Missing explicit isotope(s) in the current element ! Build from NIST database is not implemented yet !");
        //build_from_nist ();
      }
    _lock_ ();
    return;
  }

  //________________________________________________________________________
  void   element::build_from_nist ()
  {

    //-----------------  Open an ifstream from file atomic_weight_nist.dat  ----------------------------
    std::string tape_name = materials::get_resource("data/isotopic_compositions_nist.dat", true);
    ifstream ifstr_tape;
    ifstr_tape.open (tape_name.c_str ());
    DT_THROW_IF(! ifstr_tape.is_open (),
                  std::logic_error,
                "Cannot open file '" << tape_name << "' !");

    //----------------------------  Read the ifstream  ----------------------------

    bool is_z_found  = false;
    _composition_.clear();
    _molar_mass_ = -1.;

    string nist_line;

    for (int i = 0 ; i<=74 ; i++) getline(ifstr_tape,nist_line);
    /*
      bool stop = false;
      while (! stop)
      {
      string tmp;
      getline (ifstr_tape, tmp);
      istringstream iss (tmp);
      string word;
      iss >> word;
      if (word.empty ()) continue;
      else if (word[0] != '#') continue;
      else stop = true;
      }
    */

    while( !is_z_found && getline(ifstr_tape,nist_line) )
      {
        if( atoi ((nist_line.substr (0, 4)).c_str ()) == _z_
            && nist_line.find (chemical_symbol::symbol_from_z (_z_)) != string::npos)
          {
            cout << nist_line.substr (4, 4) << endl;
            is_z_found = true ;
          }
      }

    if(! is_z_found)
      {
        DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                       "Z values  '" << _z_
                       << "' not found in file 'isotopic_compositions_nist.dat' !");
        // throw logic_error (message.str ());
      }
    return;
  }
  //________________________________________________________________________
  void element::_norm_weights_ ()
  {
    double norm =0.;

    for (isotope_weight_map_type::const_iterator i = _composition_.begin ();
         i != _composition_.end ();
         i++)
      {
        const iso_entry & e = i->second;
        norm += e.weight;
      }

    DT_THROW_IF(norm <= 0.,
                std::logic_error,
                "Sum of weights for isotope '" << get_name () <<"' is negative !'");
    if(norm != 1.0)
      {
        for (isotope_weight_map_type::iterator i = _composition_.begin ();
             i !=  _composition_.end ();
             i++)
          {
            iso_entry & e = i->second;
            e.weight /= norm;
          }
      }
    return;
  }

  //________________________________________________________________________
  void element::set_molar_mass(double molar_mass_)
  {
    DT_THROW_IF (_composition_.size () > 0,
                 std::logic_error,
                 "Operation not allowed when build mode uses isotopic composition !");
    DT_THROW_IF (molar_mass_ < 0,
                 std::logic_error,
                 "Invalid mass value : '" << molar_mass_ << "' !");
    _molar_mass_ = molar_mass_;
    return;
  }

  //________________________________________________________________________
  void element::_set_molar_mass_(const double molar_mass_)
  {
    DT_THROW_IF (molar_mass_ < 0,
                 std::logic_error,
                 "Invalid mass value : '" << molar_mass_ << "' !");
    _molar_mass_ = molar_mass_;
    return;
  }

  //________________________________________________________________________
  void  element::_compute_molar_mass_ ()
  {
    double molar_mass =0.;

    for (isotope_weight_map_type::const_iterator i = _composition_.begin ();
         i != _composition_.end ();
         i++)
      {
        const iso_entry & entry = i->second;
        molar_mass += (entry.weight) * (entry.iso_ptr->get_mass ());
      }
    _set_molar_mass_ (molar_mass);
    return;
  }


  //________________________________________________________________________
  void element::tree_dump (ostream & out_,
                           const string & title_,
                           const string & indent_,
                           bool /*inherit_*/) const
  {
    string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ()) {
      out_ << indent << title_ << endl;
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Name         : '"<< get_name() << "'" << endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Chem. symbol : '"<< get_symbol() << "'" << endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Z            : " << get_z() << endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Composition  : ";
    if ( _composition_.size () == 0) {
        out_ << "<empty>" << endl;
      } else {
        out_ << endl;
        for (isotope_weight_map_type::const_iterator i = _composition_.begin ();
             i !=  _composition_.end ();
             i++) {
            const iso_entry & entry = i->second;
            isotope_weight_map_type::const_iterator j = i;
            j++;
            string atag = datatools::i_tree_dumpable::tags::item();
            if (j == _composition_.end ()) {
              atag = datatools::i_tree_dumpable::tags::last_item();
            }
            out_  << indent
                  << i_tree_dumpable::skip_tag
                  << atag
                  << "Isotope '" << entry.iso_ptr->get_name () << "'" << " : "
                  << entry.weight << " "
                  << (entry.owned? "(owned)": "(not owned)") << endl;
          }
      }

    out_ << indent << i_tree_dumpable::tag
         << "Molar mass (A) : " <<  _molar_mass_ << " [g/mol]" << endl;

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Properties   : ";
      if ( _properties_.size () == 0) {
          out_ << "<empty>" <<endl;
        } else {
          out_ << endl;
          ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
          _properties_.tree_dump (out_, "", indent_oss.str ());
        }
    }

    out_ << indent << i_tree_dumpable::last_tag
         << "Locked       : " << (is_locked()? "Yes": "No") << endl;

    return;
  }


} // end of namespace materials

// end of element.cc

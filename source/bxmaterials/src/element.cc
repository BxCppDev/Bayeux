// -*- mode: c++ ; -*-
/* element.cc
 */

#include <materials/element.h>

#include <stdexcept>
#include <sstream>
#include <fstream>

#include <materials/isotope.h>
#include <materials/ch_symbol.h>

namespace mat {

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


  //________________________________________________________________________//
  void element::_lock_check (const string & where_) const
  {
    if (is_locked ())
      {
        ostringstream message;
        if (! where_.empty ())
          {
            message << where_ << ": ";
          }
        message << "Operation not allowed ! Object is locked !";
        throw runtime_error (message.str ());
      }
    return;
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


  //________________________________________________________________________// /dtor
  element::~element ()
  {
    _composition_.clear ();
    return;
  }

  //________________________________________________________________________
  /*
    void element::_init_()
    {
    _unlock_ ();

    _composition_.clear ();

    _molar_mass_ = -1.;
    }
  */

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
  /*
    void element::set_symbol (const string & symbol_)
    {
    _lock_check ("element::set_symbol");
    _symbol_ = symbol_;
    return;
    }
  */

  //________________________________________________________________________
  void element::set_name (const string & name_)
  {
    _lock_check ("element::set_name");
    _name_ = name_;
    return;
  }

  //________________________________________________________________________
  void element::set_z (const string & symbol_)
  {
    _lock_check ("element::set_z");

    bool is_symbol_found = false;
    int i_Z = 0;

    while (i_Z < chemical_symbol::NB_CHEMICAL_SYMBOLS)
      {
        if (symbol_ == chemical_symbol::table[i_Z])
          {
            is_symbol_found = true;
            break;
          }
        i_Z++;
      }

    if(! is_symbol_found)
      {
        ostringstream message;
        message << endl << "element::set_symbol() ; symbol '"
                << symbol_<< "' not found !" << endl;
        throw runtime_error (message.str ());
      }
    _symbol_ = symbol_;
    _z_ = i_Z;
    return;
  }
  //________________________________________________________________________
  void element::set_z (const int z_)
  {
    _lock_check ("element::set_z");

    if ((z_ >= 0) && (z_ <= chemical_symbol::NB_CHEMICAL_SYMBOLS))
      {
        _z_ = z_;
        _symbol_ = chemical_symbol::table[_z_];
      }
    else
      {
        ostringstream message;
        message << "element::__set_z () : Invalid Z value : '" << z_ << "' !";
        throw logic_error (message.str () );
      }
    return;
  }

  //________________________________________________________________________
  void element::add_isotope (const isotope & iso_ptr_,  double weight_,  bool owned_)
  {
    _lock_check ("element::add_isotope");

    // 2010-10-11 by FM: not compatible with molar mass manual setting:
    if ((_molar_mass_ > 0.0) && (_composition_.size () == 0))
      {
        ostringstream message;
        message << "element::add_isotope: Operation not allowed ! Molar mass is set by hand !";
        throw logic_error (message.str () );
      }

    // set Z and symbol when add a new isotope.

    if ( _composition_.size () == 0)
      {
        if (_z_ != Z_UNDEFINED)
          {
            if (_z_ != iso_ptr_.get_z ())
              {
                ostringstream message;
                message << "element::add_isotope : Unmatching mother element/daughter isotope Z values !";
                throw logic_error (message.str () );
              }
          }
        set_z (iso_ptr_.get_z ());
      }

    // check if the isotope is locked (which mean valid)

    if (! iso_ptr_.is_locked ())
      {
        ostringstream message;
        message << endl << "element::add_isotope: Isotope '"
                << iso_ptr_.get_name ();
        message << "' is not locked !'";
        throw logic_error (message.str ());
      }

    // check if Z value of isotope to be added is consistant with Z value of the element

    if ( iso_ptr_.get_z () != _z_)
      {
        ostringstream message;
        message << endl << "element::add_isotope: the Z value '"
                << iso_ptr_.get_z () <<"' of isotope '";
        message << iso_ptr_.get_name ()
                << "' to be added is not consistant with the Z value '"
                << _z_ << "' of the element '";
        message << get_name () << "' !";
        throw logic_error (message.str ());
      }

    // check the positive value of weight

    if ( weight_ <= 0.)
      {
        ostringstream message;
        message << endl << "element::add_isotope: weight value '" << weight_ ;
        message << "' is not positive !" << endl ;
        throw logic_error (message.str ());
      }

    // check if isotope is already in the composition of the element

    if (_composition_.find (iso_ptr_.get_name ()) != _composition_.end ())
      {
        ostringstream message;
        message << endl << "!! warning !! element::add_isotope() you add the isotope '" << iso_ptr_.get_name ();
        message <<"' which is already in the composition of element '" << _name_ <<"' !" << endl;
        clog << message.str ();
      }


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
        throw runtime_error ("element::build: Missing isotope(s) in the current element ! Not implemented yet !");
        //build_from_nist ();
      }
    _lock_ ();
    return;
  }

  //________________________________________________________________________
  void   element::build_from_nist ()
  {

    //-----------------  Open an ifstream from file atomic_weight_nist.dat  ----------------------------

    string tape_name;

    if (getenv("MATERIALS_RESOURCES_DIR")==NULL)
      {
        ostringstream message;
        message << "element::build_from_nist : env. variable 'MATERIALS_RESOURCES_DIR'  not found !";
        throw runtime_error (message.str ());
      }
    else
      {
        tape_name.assign (getenv ("MATERIALS_RESOURCES_DIR"));
        tape_name += "/isotopic_compositions_nist.dat";
      }

    ifstream ifstr_tape;
    ifstr_tape.open (tape_name.c_str ());
    if(! ifstr_tape.is_open ())
      {
        ostringstream message;
        message << "element::build_from_nist : ifstream  '"
                << tape_name << "'  not open !";
        throw runtime_error (message.str ());
      }

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
        if( atoi ((nist_line.substr (0, 4)).c_str ()) == _z_ && nist_line.find (chemical_symbol::symbol_from_z (_z_)) != string::npos)
          {
            cout << nist_line.substr (4, 4) << endl;
            is_z_found = true ;
          }
      }

    if(! is_z_found)
      {
        ostringstream message;
        message << "!!! warning !! element::build_from_nist () : Z values  '"
                << _z_
                << "' not found in file isotopic_compositions_nist.dat !"
                << endl;
        // throw runtime_error (message.str ());
        clog << message << endl;
      }
    return;
  }
  //________________________________________________________________________
  void element::_norm_weights_ ()
  {
    double norm =0.;

    for (isotope_weight_map_t::const_iterator i = _composition_.begin ();
         i != _composition_.end ();
         i++)
      {
        const iso_entry & e = i->second;
        norm += e.weight;
      }

    if(norm <= 0.)
      {
        ostringstream message;
        message << endl << "element::_norm_weights_::  sum of weights for isotope '";
        message << get_name () <<"' is negative!'" <<endl ;
        throw logic_error (message.str ());
      }

    if(norm != 1.0)
      {
        /*  ostringstream message;
            message << endl << "warning !! element::_norm_weights_:: renormalize weights of element  '";
            message << get_name () <<"'!'"<< endl;
            clog << message.str ();
        */

        for (isotope_weight_map_t::iterator i = _composition_.begin ();
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
    if (_composition_.size () > 0)
      {
        ostringstream message;
        message << "element:::set_molar_mass: Operation not allowed when build mode uses isotopic composition !";
        throw logic_error (message.str ());
      }
    if (molar_mass_ < 0)
      {
        ostringstream message;
        message << "element:::set_molar_mass: Invalid mass value : '"
                << molar_mass_ << "' !";
        throw logic_error (message.str ());
      }
    _molar_mass_ = molar_mass_;
    return;
  }

  //________________________________________________________________________
  void element::_set_molar_mass_(const double molar_mass_)
  {
    if (molar_mass_ < 0)
      {
        ostringstream message;
        message << endl << "element:::_set_molar_mass_() : Invalid mass value : '"
                << molar_mass_ << "' !" << endl;
        throw logic_error (message.str ());
      }
    _molar_mass_ = molar_mass_;
    return;
  }

  //________________________________________________________________________
  void  element::_compute_molar_mass_ ()
  {
    double molar_mass =0.;

    for (isotope_weight_map_t::const_iterator i = _composition_.begin ();
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
                           bool inherit_) const
  {
    namespace du = datatools::utils;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ())
      {
        out_ << indent << title_ << endl;
      }

    out_ << indent << i_tree_dumpable::tag
         << "Name         : \""<< get_name() <<"\"" << endl;

    out_ << indent << i_tree_dumpable::tag
         << "Chem. symbol : \""<< get_symbol() <<"\"" << endl;

    out_ << indent << du::i_tree_dumpable::tag
         << "Composition  : ";
    if ( _composition_.size () == 0)
      {
        out_ << "<empty>" << endl;
      }
    else
      {
        out_ << endl;
        for (isotope_weight_map_t::const_iterator i = _composition_.begin ();
             i !=  _composition_.end ();
             i++)
          {
            const string & key = i->first;
            const iso_entry & entry = i->second;
            isotope_weight_map_t::const_iterator j = i;
            j++;
            string atag = du::i_tree_dumpable::TAG;
            if (j == _composition_.end ()) atag = du::i_tree_dumpable::LAST_TAG;
            out_  << indent
                  << i_tree_dumpable::skip_tag
                  << atag
                  << "Isotope '" << entry.iso_ptr->get_name () << "'" << " : "
                  << entry.weight << " "
                  << (entry.owned? "(owned)": "(not owned)") << endl;
          }
      }

    out_ << indent << i_tree_dumpable::tag
         << "Molar mass   : " <<  _molar_mass_ << " [g/mol]" << endl;

    {
      out_ << indent << du::i_tree_dumpable::tag
           << "Properties   : ";
      if ( _properties_.size () == 0)
        {
          out_ << "<empty>" <<endl;
        }
      else
        {
          out_ << endl;
          ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << du::i_tree_dumpable::skip_tag;
          _properties_.tree_dump (out_, "", indent_oss.str ());
        }
    }


    out_ << indent << i_tree_dumpable::last_tag
         << "Locked       : " << (is_locked()? "Yes": "No") << endl;

    return;
  }


} // end of namespace mat

// end of element.cc

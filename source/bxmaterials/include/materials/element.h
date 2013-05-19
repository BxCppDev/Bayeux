// -*- mode: c++ ; -*-
/* element.h
 * Author (s) : Benoit Guillon  <guillon@lpccaen.in2p3.fr>
 * Creation date: 2010-03-14
 * Last modified: 2010-03-14
 *
 * Description:  A simple class to handle "abstract" chemical elements and their basic properties.
 */

/*!
  - A simple class to handle "abstract" chemical elements and their basic properties.
  - Elements is simply a collection of isotopes (weighted by proportion) with same number
  of protons (Z).
  - They are construct and validate when invoking build() function.
  - When they are build,  derived properties such as molar mass are computed.
  \author BG
  @version 0.1
*/

#ifndef MATERIALS_ELEMENT_H_
#define MATERIALS_ELEMENT_H_ 1

#include <string>
#include <map>

#include <datatools/properties.h>
#include <datatools/i_tree_dump.h>

namespace materials {

  class isotope;

  struct iso_entry
  {
    bool            owned;
    const isotope * iso_ptr;
    double          weight;

    iso_entry ();
    ~iso_entry ();
    bool is_valid () const;
  };

  typedef std::map<std::string, iso_entry> isotope_weight_map_type;

  class element : public datatools::i_tree_dumpable
  {
  public:

    static const int Z_UNDEFINED = -1;

  public:

    /* constructor / destructor */
    element ();          //!< Defaut Constructor

    //! Normal Constructor.
    /*!
      \param name_ : Name of the element
      \param z_ : Z of the element
      \param build_ : On the fly building
    */
    element (const std::string & name_, int z_, bool build_ = false);

    //! Normal Constructor.
    /*!
      \param name_ : Name of the element
      \param z_ : Z of the element
      \param molar_mass_ : Molar mass of the element
      \param build_ : On the fly building
    */
    element (const std::string & name_, int z_, double molar_mass_, bool build_ = false);

    //! Normal Constructor.
    /*!
      \param name_ : Name of the element
      \param symbol_ : Chemical symbol of the element
      \param build_ : On the fly building
    */
    element (const  std::string & name_, const std::string & symbol_ = "", bool build_ = false);

    virtual ~element ();          //!<  Destructor

    /* public methods */
  public :
    bool   is_built_by_isotopic_composition () const;

    void   set_name (const std::string  & name_); //!<  Set the name of the element
    void   set_z (int z_);            //!<  Set the Z (number of protons) & the chemical symbol of the element.

    void   set_z (const std::string  & symbol_);  //!<  Set the Z (number of protons) & the chemical symbol of the element.

    void   set_molar_mass(double molar_mass_); //!<  Set the molar mass of the element.

    void   add_isotope (const isotope & iso_ref_ , double weight_ = 1.0, bool owned_ = false);  //!<  Add an isotope with weight

    void   add_isotope (const isotope * iso_ptr_ , double weight_ = 1.0);   //!<  Add an isotope with weight

    void   build ();            //!<  Build the element : norm weights, compute molar mass and lock (or not).

    void   build_from_nist ();  //!<  Build the element using nist composition data, then normalize weights, compute molar mass and lock (or not).


    /* public retrieval methods */
  public :

    const std::string & get_name () const {return _name_;}              //!<  Return the name.

    const std::string & get_symbol () const {return _symbol_;}          //!<  Return the chemical symbol.

    int get_z () const {return _z_;}                    //!<  Return the number of protons (Z).

    double get_molar_mass () const {return _molar_mass_;} //!<  Return the molar mass [g/mol].

    bool is_locked () const {return _locked_;}           //!<  Return true if composition is valid, weights are normalized and molar mass is computed.

    const isotope_weight_map_type & get_composition () const;

    const datatools::properties & get_properties () const {return _properties_;} //!< Get reference of datatools::properties private attribute

    datatools::properties & grab_properties () {return _properties_;} //!< Get reference of datatools::properties private attribute

    virtual void tree_dump (std::ostream & out_  = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_ = false) const;  //!<  print info virtual method


   /* private set & find methods */
  private :

    void   _norm_weights_ ();       //!<  Normalize sum of weights to unity

    void   _compute_molar_mass_ (); //!<  Compute molar mass in[g/mol]

    void   _set_molar_mass_ (double molar_mass_);  //!< Set the molar mass in [g/mol]

    void   _lock_ ();   //!<  Lock the element : boolean flag '_locked_' is set to true when composition is valid & derived properties are computed.

    void   _unlock_ (); //!<  Unlock the element - boolean flag '_locked_' is set to false.

    /* private attribute */
  private :

    std::string             _name_;        //!<  Name
    std::string             _symbol_;      //!<  Chemical symbol of the element
    int                     _z_;           //!<  Number of protons of the element
    isotope_weight_map_type _composition_; //!<  Isotopic composition of the element [std::map<string,  iso_entry>]
    double                  _molar_mass_;  //!<  Molar mass in [g/mol]
    datatools::properties   _properties_;  //!<  datatools properties
    bool                    _locked_;      //!<  boolean flag : true when composition is validated & derived properties are computed

  }; // end of class element

} // end of namespace materials

#endif // MATERIALS_ELEMENT_H_

// end of element.h

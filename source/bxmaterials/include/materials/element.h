/// \file materials/element.h
/* Author (s) : Benoit Guillon  <guillon@lpccaen.in2p3.fr>
 * Creation date: 2010-03-14
 * Last modified: 2010-03-14
 *
 * Description:  A simple class to handle "abstract" chemical elements and their basic properties.
 */

/*!
  - A simple class to handle "abstract" chemical elements and their basic properties.
  - Elements is simply a collection of isotopes (weighted by proportion) with same number
  of protons (Z).
  - They are construct and validated when invoking build() function.
  - When they are built, derived properties such as molar mass are computed.
  \author BG
  @version 0.1
*/

#ifndef MATERIALS_ELEMENT_H
#define MATERIALS_ELEMENT_H 1

// Standard Library:
#include <string>
#include <map>

// Third Party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/i_tree_dump.h>

/// \brief Top-level namespace of the Bayeux/materials module library
namespace materials {

  // Forward declaration:
  class isotope;

  /// \brief Isotope composition entry
  class iso_entry
  {
  public:

    /// Constructor
    iso_entry();

    /// Destructor
    ~iso_entry();

    /// Check validity
    bool is_valid() const;

    /// Check for an owned compound
    bool is_owned() const;

    /// Check for an isotope compound
    bool has_isotope() const;

    /// Check the weight
    bool has_weight() const;

    /// Return the weight
    double get_weight() const;

    /// Set the weight
    void set_weight(double);

    /// Set the isotope
    void set_isotope(const isotope & iso_, bool owned_ = false);

    /// Set the isotope
    void set_isotope(const isotope * iso_, bool owned_ = true);

    /// Return the compound isotope
    const isotope & get_isotope() const;

    /// Reset
    void reset();

    /// Reset the compound isotope
    void reset_iso();

  private:

    bool            _owned_;   //!< Flag indicating if the pointer to the isotope instance should be deleted
    const isotope * _iso_ptr_; //!< Pointer to the isotope instance
    double          _weight_;  //!< Weight

  };

  /// Dictionary of isotope composition entries
  typedef std::map<std::string, iso_entry> isotope_weight_map_type;

  /// \brief The description of an element
  class element : public datatools::i_tree_dumpable
  {
  public:

    static const int Z_UNDEFINED = -1;

    /// \brief Build flags
    enum build_flag_type {
      BF_NIST_COMPOSITION = datatools::bit_mask::bit00, //!< Build from the NIST table
      BF_LOCK             = datatools::bit_mask::bit31  //!< Lock after building
    };

    /// Defaut Constructor
    element();

    /// Normal Constructor.
    /*!
      \param name_ : Name of the element
      \param z_ : Z of the element
      \param build_flags_ : On the fly building
    */
    explicit element(const std::string & name_, int z_, unsigned int build_flags_ = 0);

    /// Normal Constructor.
    /*!
      \param name_ : Name of the element
      \param z_ : Z of the element
      \param build_flags_ : On the fly building
    */
    explicit element(const char * name_, int z_, unsigned int build_flags_ = 0);

    /// Normal Constructor.
    /*!
      \param name_ : Name of the element
      \param z_ : Z of the element
      \param molar_mass_ : Molar mass of the element
      \param build_flags_ : On the fly building
    */
    explicit element(const std::string & name_, int z_, double molar_mass_, unsigned int build_flags_ = 0);

    /// Normal Constructor.
    /*!
      \param name_ : Name of the element
      \param z_ : Z of the element
      \param molar_mass_ : Molar mass of the element
      \param build_flags_ : On the fly building
    */
    explicit element(const char * name_, int z_, double molar_mass_, unsigned int build_flags_ = 0);

    //! Normal Constructor.
    /*!
      \param name_ : Name of the element
      \param symbol_ : Chemical symbol of the element
      \param build_flags_ : On the fly building
    */
    explicit element(const std::string & name_, const std::string & symbol_ = "", unsigned int build_flags_ = 0);

    //! Normal Constructor.
    /*!
      \param name_ : Name of the element
      \param symbol_ : Chemical symbol of the element
      \param build_flags_ : On the fly building
    */
    explicit element(const char * name_, const  char * symbol_ = "", unsigned int build_flags_ = 0);

    /// Destructor
    ~element() override;

    /// Return true if the element is built by isotopic composition
    bool is_built_by_isotopic_composition () const;

    /// Set the name of the element
    void set_name(const std::string  & name_);

    /// Set the Z (number of protons) & the chemical symbol of the element.
    void set_z(int z_);

    /// Set the Z (number of protons) & the chemical symbol of the element.
    void set_z(const std::string & symbol_);

    /// Set the molar mass of the element [g/mol]
    /** @code
     *  double g_per_mol = CLHEP::g / CLHEP::mol;
     *  double molar_mass = 12.34 * g_per_mol;
     *  materials::element elmt;
     *  elmt.set_molar_mass(molar_mass / g_per_mol);
     *  @endcode
     *  or
     *  @code
     *  double molar_mass = 12.34; // implicit [g/mol]
     *  materials::element elmt;
     *  elmt.set_molar_mass(molar_mass);
     *  @endcode     */
    void set_molar_mass(double molar_mass_);

    /// Add an isotope with weight
    void add_isotope(const isotope & iso_ref_ , double weight_ = 1.0, bool owned_ = false);

    /// Add an isotope with weight
    void add_isotope(const isotope * iso_ptr_ , double weight_ = 1.0);

    /// Build the element : norm weights, compute molar mass.
    void build(unsigned int build_flags_ = 0);

    /// Build the element using NIST composition data, then normalize weights, compute molar mass.
    void build_from_nist();

    /// Return the name.
    const std::string & get_name() const;

    /// Return the chemical symbol.
    const std::string & get_symbol() const;

    /// Return the number of protons (Z).
    int get_z() const;

    ///  Return the molar mass [g/mol]
    /** @code
     *  double g_per_mol = CLHEP::g/CLHPE::mol;
     *  double molar_mass = 12.34 * g_per_mol;
     *  materials::element elmt;
     *  elmt.set_molar_mass(molar_mass/g_per_mol);
     *  double reloaded_molar_mass = elmt.get_molar_mass() * g_per_mol;
     *  @endcode
     */
    double get_molar_mass() const;

    /// Return true if composition is valid, weights are normalized and molar mass is computed.
    bool is_locked() const;

    /// Lock the element data structure
    void lock();

    /// Unlock the element data structure
    void unlock();

    /// Return the isotopic composition of the element
    const isotope_weight_map_type & get_composition() const;

    /// Get reference of auxiliary properties
    const datatools::properties & get_properties() const;

    /// Get reference of auxiliary properties
    datatools::properties & grab_properties();

    /// Reset
    void reset();

    /// Smart print
    void tree_dump(std::ostream & out_  = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_ = false) const override;

  protected:

    /// Set default values
    void _set_defaults();

  private :

    /// Normalize sum of weights to unity
    void   _norm_weights_();

    /// Compute molar mass in[g/mol]
    void   _compute_molar_mass_();

    /// Set the molar mass in [g/mol]
    void   _set_molar_mass_(double molar_mass_);

  private :

    bool                    _locked_;      //!< boolean flag
    std::string             _name_;        //!< Name
    std::string             _symbol_;      //!< Chemical symbol of the element
    int                     _z_;           //!< Number of protons of the element
    isotope_weight_map_type _composition_; //!< Isotopic composition of the element [std::map<string,  iso_entry>]
    double                  _molar_mass_;  //!< Molar mass in [g/mol]
    datatools::properties   _properties_;  //!< Auxiliary properties

  }; // end of class element

} // end of namespace materials

#endif // MATERIALS_ELEMENT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

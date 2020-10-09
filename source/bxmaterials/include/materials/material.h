/// \file materials/material.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-22
 * Last modified: 2014-06-13
 *
 * License:
 *
 * Description:
 *
 *   A simple class to handle "abstract" material and their basic properties
 *
 *  Note:
 *   Some useful data here:
 *     http://www.nucsafe.com/cms/Selecting+Gamma+Detector/43.html
 *
 */

#ifndef MATERIALS_MATERIAL_H
#define MATERIALS_MATERIAL_H

// Standard Library:
#include <iostream>
#include <string>
#include <map>

// Third Party:
// - Bayeux/datatools:
#include <datatools/i_tree_dump.h>
#include <datatools/properties.h>

namespace materials {

  // Forward declarations:
  class material;
  class element;

  /// \brief Material compound entry
  class compound_entry
  {
  public:

    /// Constructor
    compound_entry();

    /// Destructor
    ~compound_entry();

    /// Check validity
    bool is_valid() const;

    /// Check for a element compound
    bool has_element() const;

    /// Check for a material compound
    bool has_material() const;

    /// Check for an owned compound
    bool is_owned() const;

    /// Check the weight
    bool has_weight() const;

    /// Check the number of atoms
    bool has_number_of_atoms() const;

    /// Return the weight
    double get_weight() const;

    /// Set the weight
    void set_weight(double);

    /// Return the number of atoms
    int get_number_of_atoms() const;

    /// Set the number of atoms
    void set_number_of_atoms(int);

    /// Set the element
    void set_element(const element & elt_, bool owned_ = false);

    /// Set the material
    void set_material(const material & mat_, bool owned_ = false);

    /// Set the element
    void set_element(const element * elt_, bool owned_ = true);

    /// Set the material
    void set_material(const material * mat_, bool owned_ = true);

    /// Return the compound element
    const element & get_element() const;

    /// Return the compound material
    const material & get_material() const;

    /// Reset
    void reset();

    /// Reset the compound
    void reset_compound();

  private:

    bool             _owned_;       //!< Flag indicating if the pointer to the element or material instance should be deleted
    const element *  _elt_ptr_;     //!< Pointer to the element instance
    const material * _mat_ptr_;     //!< Pointer to the material instance
    double           _weight_;      //!< Weight
    int              _nb_of_atoms_; //!< Number of atoms

  };

  /// Dictionary of material compounds
  typedef std::map<std::string, compound_entry> composition_map_type;

  /// \brief The description of a material
  class material : public datatools::i_tree_dumpable
  {

  public:

    /// \brief State of the material
    enum state_type {
      STATE_UNKNOWN = 0, //!< Unknown state
      STATE_GAS     = 1, //!< Gas
      STATE_LIQUID  = 2, //!< Liquid
      STATE_SOLID   = 3  //!< Solid
    };

    /// Return label associated to a state value
    static std::string state_to_label(state_type);

    /// Return state value associated to a label
    static state_type label_to_state(const std::string &);

    /// \brief Define the proportion units :
    /*! KP_ATOM (number of atoms by molecule) or KP_MASS (% mass)
     */
    enum proportion_unit_type {
      KP_UNDEFINED    = -1, //!< Undefined
      KP_ATOM         =  0, //!< Proportion by number of atoms
      NUMBER_OF_ATOMS =  0, //!< Proportion by number of atoms
      KP_MASS         =  1, //!< Proportion by fraction of mass
      FRACTION_MASS   =  1, //!< Proportion by fraction of mass
      KP_ZA           =  2, //!< Proportion by mean Z, A
      MEAN_ZA         =  2  //!< Proportion by mean Z, A
    };

    /// \brief Build flags
    enum build_flag_type {
      BF_MASS_DATA  = datatools::bit_mask::bit00, //!< Extract mass excess and mass from tabulated isotopes
      BF_LOCK       = datatools::bit_mask::bit31  //!< Lock after building
    };

    /// Default density unit
    static double g_per_cm3();

    /// Defaut Constructor
    material();

    /// Constructor
    /*!
     * \param name_ : Name of the material
     */
    explicit material(const std::string & name_);

    /// Constructor
    /*!
     * \param name_ : Name of the material
     */
    explicit material(const char * name_);

    /// Destructor
    ~material() override;

    /// Check if the material is composed by mean Z and A
    bool is_composed_by_mean_z_a() const;

    /// Check if the material is composed by number of atoms
    bool is_composed_by_number_of_atoms() const;

    /// Check if the material is composed by fraction of mass
    bool is_composed_by_fraction_mass() const;

    /// Return the composition map
    const composition_map_type &  get_composition() const;

    /// Return the material name
    const std::string & get_name() const;

    /// Set the material name
    void set_name(const std::string &);

    /// Return the mean Z (atomic number)
    double get_mean_z() const;

    /// Return the mean A (atomic mass)
    double get_mean_a() const;

    /// Set the mean Z and A
    void set_mean_z_a(double z_, double a_);

    /// Check state
    bool has_state() const;

    /// Set the state
    void set_state(state_type state_);

    /// Return the state
    state_type get_state() const;

    /// Check density
    bool has_density() const;

    /// Return the density of the material
    double get_density() const;

    /// Set the density of the material
    void set_density(double);

    /// Check if pressure is defined
    bool has_pressure() const;

    /// Set the pressure of the material
    void set_pressure(double);

    /// Return the pressure of the material
    double get_pressure() const;

    /// Check if temperature is defined
    bool has_temperature() const;

    /// Set the temperature of the material
    void set_temperature(double);

    /// Return the pressure of the material
    double get_temperature() const;

    ///  Add an element with weight in KP_MASS proportion unit
    void add_element_by_mass(const element & elt_ref_ , double weight_  , bool owned_ = false);

    ///  Add an element with weight in KP_MASS proportion unit
    void add_element_by_mass(const element * elt_ptr_ , double weight_ );

    ///  Add an element  in KP_ATOM proportion unit
    void add_element_by_nb_of_atoms(const element & elt_ref_ , int nb_of_atoms_  , bool owned_ = false);

    ///  Add an element in KP_ATOM proportion unit
    void add_element_by_nb_of_atoms(const element * elt_ptr_ , int nb_of_atoms_   );

    /// Add an material with weight in KP_MASS proportion unit
    void add_material_by_mass(const material & mat_ref_ , double weight_  , bool owned_ = false);

    /// Add an element with weight in KP_MASS proportion unit
    void add_material_by_mass(const material * mat_ptr_ , double weight_ );

    /// Build the material :  compute molar mass and lock (or not).
    void build(unsigned int build_flags_ = 0);

    /// Return true if composition is valid, weights are normalized and molar mass is computed.
    bool is_locked() const;

    /// Lock the material data structure
    void lock();

    /// Unlock the material data structure
    void unlock();

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

    /// Compute molar mass in[g/mol]
    void _compute_molar_mass_();

    /// Set the mean Z
    void _set_mean_z_(double);

    /// Set the mean Z
    void _set_mean_a_(double);

    /// Normalize  weights of compounds
    void _normalize_weights_();

  private:

    bool        _locked_;  //!< boolean flag : true when composition is validated & derived properties are computed
    std::string _name_;    //!< Name
    double      _density_; //!< Density
    state_type  _state_; //!< State
    double      _pressure_; //!< Pressure
    double      _temperature_; //!< Temperature
    double      _mean_z_;  //!< Mean atomic number
    double      _mean_a_;  //!< Mean mass number in g/mol
    datatools::properties _properties_;      //!< Auxiliary properties
    proportion_unit_type  _proportion_unit_; //!< KP_ATOM (number of atoms by molecule) or KP_MASS (% mass)
    composition_map_type  _composition_;     //!< Composition of the material [std::map<string, compound_entry>]

  };

} // end of namespace materials

#endif // MATERIALS_MATERIAL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

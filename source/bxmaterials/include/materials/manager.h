/// \file materials/manager.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-22
 * Last modified: 2016-02-11
 *
 * License:
 *
 * Description:
 *
 *   A manager for isotopes, elements and materials (and material aliases)
 *
 */

#ifndef MATERIALS_MANAGER_H
#define MATERIALS_MANAGER_H 1

// Standard library:
#include <ostream>
#include <string>
#include <list>
#include <set>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/logger.h>

// This project:
//#include <materials/factory.h>
#include <materials/isotope.h>
#include <materials/element.h>
#include <materials/material.h>
#include <materials/detail/tools.h>

namespace datatools {
  // Forward declaration:
  class multi_properties;
}

namespace materials {

  // Forward declaration:
  class factory;

  /// \brief The manager object for isotopes, elements and materials
  ///
  /// The material manager stores dictionaries for isotopes, elements and materials.
  /// Materials can be addressed through material aliases.
  /// The manager uses an embedded factory to instantiate isotopes, elements and materials objects.
  class manager : public datatools::i_tree_dumpable
  {
  public:

    /// Validate a name for isotope, element or material with respect to forseen GDML export
    static bool validate_name_for_gdml(const std::string & candidate_);

    /// Default constructor
    manager();

    /// Destructor
    virtual ~manager();

    /// Check initialization flag
    bool is_initialized() const;

    /// Check debug flag
    bool is_debug() const;

    /// Set debug flag
    void set_debug(bool);

    /// Check lock flag
    bool is_locked() const;

    /// Check if alias of alias are allowed
    bool is_alias_of_alias_allowed() const;

    /// Set the alias of alias allow flag
    void set_alias_of_alias_allowed(bool aoaa_);

    /// Check if aliases can be overloaded by other aliases declared after them
    bool is_alias_allow_overload() const;

    /// Set the alias overloading flag
    void set_alias_allow_overload(bool aao_);

    /// Check if a given isotope is defined
    bool has_isotope(const std::string & entry_name_) const;

    /// Return a reference to a isotope, given its name
    const isotope & get_isotope(const std::string & entry_name_) const;

    /// Check if a given element is defined
    bool has_element(const std::string & entry_name_) const;

    /// Return a reference to a element, given its name
    const element & get_element(const std::string & entry_name_) const;

    /// Check if a given material is defined
    bool has_material(const std::string & entry_name_) const;

    /// Return a reference to a material, given its name
    const material & get_material(const std::string & entry_name_) const;

    /// Check if an alias with given name is defined
    bool is_alias(const std::string & entry_name_) const;

    /// Return the material name of which an alias is associated
    std::string alias_of(const std::string & entry_name_) const;

    /// Return the dictionary of isotopes
    const isotope_dict_type & get_isotopes() const;

    /// Return the dictionary of elements
    const element_dict_type & get_elements() const;

    /// Return the dictionary of materials
    const material_dict_type & get_materials() const;

    /// Return the ordered list of materials
    const std::list<std::string> & get_ordered_materials() const;

    /// Load a configuration file
    void load(const datatools::multi_properties & config_);

    /// Initialize
    void initialize(const datatools::properties & setup_);

    /// Reset
    void reset();

    /// Lock the manager
    void lock();

    /// Unlock the manager
    void unlock();

    /// Smart print
    virtual void tree_dump(std::ostream & out_    = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_          = false) const;

    /// Set logging priority
    void set_logging_priority(datatools::logger::priority);

    /// Return the logging priority
    datatools::logger::priority get_logging_priority() const;

    /// Test the flag to load mass data for isotopes
    bool is_load_isotope_mass_data() const;

    /// Set the flag to load mass data for isotopes
    void set_load_isotope_mass_data(bool);

    /// Test the flag to load decay data for isotopes
    bool is_load_isotope_decay_data() const;

    /// Set the flag to load decay data for isotopes
    void set_load_isotope_decay_data(bool);

    /// Add an exported property prefix for materials
    /**
     * 2012-11-03 FM : support for various "Material Properties Tables"
     * ala Geant4 or display properties:
     *
     * \code
     *   materials::manager mgr;
     *   mgr.add_material_exported_prefix("mpt.");
     *   mgr.add_material_exported_prefix("visualization.");
     * \endcode
     */
    void add_material_exported_prefix(const std::string &);

    /// Return the set of exported property prefixes for materials
    const std::set<std::string> & get_material_exported_prefixes() const;

  protected:

    /// Set default values to attributes
    void _set_defaults();

  private:

    // Management:
    bool                        _locked_;           //!< Lock flag
    datatools::logger::priority _logging_priority_; //!< Logging priority
    // Configuration attributes:
    bool                   _load_isotope_mass_data_;     //!< Flag to load isotope mass data
    bool                   _load_isotope_decay_data_;    //!< Flag to load isotope decay data
    std::set<std::string>  _material_exported_prefixes_; //!< List of property prefixes exported to materials
    bool                   _alias_allow_overload_;       //!< Flag to allow material alias overloading
    bool                   _alias_of_alias_allowed_;     //!< Flag to allow alias of material alias
    // Dynamic attributes:
    boost::scoped_ptr<factory> _creator_; //!< Embeded factory
    isotope_dict_type      _isotopes_; //!< Dictionary of isotopes
    element_dict_type      _elements_; //!< Dictionary of elements
    material_dict_type     _materials_; //!< Dictionary of materials
    std::list<std::string> _ordered_materials_; //!< Ordered list of materials by name

  };

} // end of namespace materials

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(materials::manager)

#endif // MATERIALS_MANAGER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

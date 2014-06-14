// -*- mode: c++ ; -*-
/// \file materials/factory.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-22
 * Last modified: 2014-06-13
 *
 * License:
 *
 * Description:
 *   A factory for dynamic creation of isotopes/elements/materials
 *
 * History:
 *
 */

#ifndef MATERIALS_FACTORY_H
#define MATERIALS_FACTORY_H 1

// Standard library:
#include <string>

// This project:
#include <materials/detail/tools.h>

namespace datatools {
  // Forward declarations:
  class properties;
}

namespace materials {

  // Forward declarations:
  class isotope;
  class element;
  class material;
  class manager;

  /// \brief Factory for isotopes, elements and materials
  class factory
  {
  public:

    /// Check debug status
    bool is_debug() const;

    /// Set debug status
    void set_debug(bool);

    /// Constructor
    factory();

    /// Destructor
    virtual ~factory();

    /// Check for a manager
    bool has_manager() const;

    /// Set a manager
    void set_manager(manager &);

    /// Isotope factory method
    isotope * create_isotope(const std::string & name_,
                             const datatools::properties & config_) const;

    /// Element factory method
    element * create_element(const std::string & name_,
                             const datatools::properties & config_,
                             const isotope_dict_type & isotopes_) const;

    /// Material factory method
    material * create_material(const std::string & name_,
                               const datatools::properties & config_,
                               const element_dict_type & elements_,
                               const material_dict_type & materials_) const;

  private:

    bool _debug_; /// Debug flag
    manager * _manager_; /// Handle to a manager

  };

} // end of namespace materials

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(materials::isotope)
DOCD_CLASS_DECLARATION(materials::element)
DOCD_CLASS_DECLARATION(materials::material)

#endif // MATERIALS_FACTORY_H

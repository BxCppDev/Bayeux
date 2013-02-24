// -*- mode: c++ ; -*-
/* manager.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-22
 * Last modified: 2013-02-23
 *
 * License:
 *
 * Description:
 *   A manager for isotopes/elements and materials
 *
 * History:
 *
 */

#ifndef MATERIALS_MANAGER_H_
#define MATERIALS_MANAGER_H_ 1

#include <ostream>
#include <string>
#include <list>

#include <materials/factory.h>
#include <materials/isotope.h>
#include <materials/element.h>
#include <materials/material.h>
#include <datatools/multi_properties.h>
#include <geomtools/gdml_writer.h>

namespace materials {

  class manager : public datatools::i_tree_dumpable
  {

  public:
    bool is_debug () const;
    void set_debug (bool);
    bool is_locked () const;
    const isotope_dict_type & get_isotopes () const;
    const element_dict_type & get_elements () const;
    const material_dict_type & get_materials () const;

    /// Constructor
    manager ();

    /// Destructor
    virtual ~manager ();

    void load (const datatools::multi_properties & config_);

    void reset ();

    void lock ();

    void unlock ();

    //! Print info
    virtual void tree_dump (std::ostream & out_    = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const;

    void export_gdml (geomtools::gdml_writer & gw_) const;

    void export_gdml (std::ostream & out_) const;

  private:

    bool               _debug_;
    factory            _creator_;
    isotope_dict_type  _isotopes_;
    element_dict_type  _elements_;
    material_dict_type _materials_;
    std::list<std::string>  _ordered_materials_;
    bool               _locked_;

  };

} // end of namespace materials

namespace mat = materials;

#endif // MATERIALS_MANAGER_H_

// end of manager.h

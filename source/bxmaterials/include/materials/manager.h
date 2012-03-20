// -*- mode: c++ ; -*-
/* manager.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-22
 * Last modified: 2010-09-22
 *
 * License:
 *
 * Description:
 *   A manager for isotopes/elements and materials
 *
 * History:
 *
 */

#ifndef __materials__manager_h
#define __materials__manager_h 1

#include <ostream>
#include <string>
#include <list>

#include <materials/factory.h>
#include <materials/isotope.h>
#include <materials/element.h>
#include <materials/material.h>
#include <datatools/utils/multi_properties.h>
#include <geomtools/gdml_writer.h>

namespace mat {

  class manager : public datatools::utils::i_tree_dumpable
  {
  private:

    bool            _debug_;
    factory         _creator_;
    isotope_dict_t  _isotopes_;
    element_dict_t  _elements_;
    material_dict_t _materials_;
    std::list<string>    _ordered_materials_;
    bool            _locked_;

  public:
    bool is_debug () const;
    void set_debug (bool);
    bool is_locked () const;
    const isotope_dict_t & get_isotopes () const;
    const element_dict_t & get_elements () const;
    const material_dict_t & get_materials () const;

  public:
    // ctor:
    manager ();

    // dtor:
    virtual ~manager ();

    void load (const datatools::utils::multi_properties & config_);

    void reset ();

    void lock ();

    void unlock ();

    //! Print info
    virtual void tree_dump (std::ostream & out_    = std::clog,
                            const string & title_  = "",
                            const string & indent_ = "",
                            bool inherit_          = false) const;

    void export_gdml (geomtools::gdml_writer & gw_) const;

    void export_gdml (std::ostream & out_) const;

  };

} // end of namespace mat

#endif // __materials__manager_h

// end of manager.h

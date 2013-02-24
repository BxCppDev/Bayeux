// -*- mode: c++ ; -*- 
/* multiple_items_model.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-20
 * Last modified: 2010-02-20
 * 
 * License: 
 * 
 * Description: 
 *   A model that hosts several objects (items) in a box mother shape
 * 
 * History: 
 * 
 */

#ifndef GEOMTOOLS_MULTIPLE_ITEMS_MODEL_H_
#define GEOMTOOLS_MULTIPLE_ITEMS_MODEL_H_ 1

#include <string>

#include <datatools/properties.h>

#include <geomtools/i_boxed_model.h>
#include <geomtools/model_with_internal_items_tools.h>
#include <geomtools/box.h>

namespace geomtools {
 
  GEOMTOOLS_BOXED_MODEL_CLASS_DECLARE(multiple_items_model)
  {

  private:

    const std::string & _get_material_name_ () const;

  public: 

    MWIM & get_internals ();
    const MWIM & get_internals () const;

    virtual const geomtools::box & get_box () const;
    const box & get_solid () const;

    const std::string & get_material_name () const;
    void set_material_name (const std::string &);
     
    virtual std::string get_model_id () const;

  public: 

    // ctor:
    multiple_items_model ();

    // dtor:
    virtual ~multiple_items_model ();

  public: 
    virtual void tree_dump (std::ostream & out_         = std::clog, 
                            const std::string & title_  = "", 
                            const std::string & indent_ = "", 
                            bool inherit_          = false) const;

  protected:

    virtual void _at_construct (const std::string & label_,
                                const datatools::properties & config_,
                                models_col_type * models_ = 0);

  private:

    std::string      _material_name_;
    box              _solid_;
    MWIM             _internals_;

    // registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(multiple_items_model);

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_MULTIPLE_ITEMS_MODEL_H_

// end of multiple_items_model.h

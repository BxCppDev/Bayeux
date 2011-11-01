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

#ifndef __geomtools__multiple_items_model_h
#define __geomtools__multiple_items_model_h 1

#include <string>

#include <datatools/utils/properties.h>

#include <geomtools/i_boxed_model.h>
#include <geomtools/model_with_internal_items_tools.h>
#include <geomtools/box.h>

namespace geomtools {
    
  using namespace std;  
 
  class multiple_items_model : public i_boxed_model
  {

  private:

    const string & _get_material_name_ () const;

  public: 

    MWIM & get_internals ();
    const MWIM & get_internals () const;

    virtual const geomtools::box & get_box () const;
    const box & get_solid () const;

    const string & get_material_name () const;
    void set_material_name (const string &);
     
    virtual string get_model_id () const;

  public: 

    // ctor:
    multiple_items_model ();

    // dtor:
    virtual ~multiple_items_model ();

  public: 
    virtual void tree_dump (ostream & out_         = clog, 
                            const string & title_  = "", 
                            const string & indent_ = "", 
                            bool inherit_          = false) const;

  protected:

    virtual void _at_construct (const string & label_,
                                const datatools::utils::properties & config_,
                                models_col_t * models_ = 0);

  private:

    string           _material_name_;
    box              _solid_;
    MWIM             _internals_;

  private:

    static i_model::creator_registration<multiple_items_model> _g_cr_;

  };

} // end of namespace geomtools

#endif // __geomtools__multiple_items_model_h

// end of multiple_items_model.h

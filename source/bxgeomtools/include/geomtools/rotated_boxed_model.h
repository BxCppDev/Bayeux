// -*- mode: c++ ; -*- 
/* rotated_boxed_model.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-24
 * Last modified: 2010-02-24
 * 
 * License: 
 * 
 * Description: 
 *   Factory for geometry models
 * 
 * History: 
 * 
 */

#ifndef GEOMTOOLS_ROTATED_BOXED_MODEL_H_
#define GEOMTOOLS_ROTATED_BOXED_MODEL_H_ 1

#include <iostream>
#include <string> 

#include <geomtools/i_boxed_model.h>

#include <geomtools/box.h>
#include <geomtools/physical_volume.h>
#include <geomtools/placement.h>

namespace geomtools {

  // define a geometry model with a boxed model rotated 
  // by some simple rotation:
  GEOMTOOLS_BOXED_MODEL_CLASS_DECLARE(rotated_boxed_model)
  {

  public: 
    void set_boxed_model (const i_model &);

    const i_model & get_boxed_model () const;

    virtual const geomtools::box & get_box () const;

    const geomtools::box & get_solid () const;
  
    rotated_boxed_model ();
  
    virtual ~rotated_boxed_model ();

    virtual std::string get_model_id () const;

    virtual void tree_dump (std::ostream & out_         = std::clog, 
                            const std::string & title_  = "", 
                            const std::string & indent_ = "", 
                            bool inherit_          = false) const;

  protected:
  
    virtual void _at_construct (const std::string & name_,
                                const datatools::properties & config_,
                                models_col_type * models_ = 0);
  private:
    
    const i_model *  _boxed_model_;
    placement        _boxed_placement_;
    physical_volume  _boxed_phys_;
    geomtools::box   _solid_;

    // registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(rotated_boxed_model);
  
  };

} // end of namespace geomtools

#endif // GEOMTOOLS_ROTATED_BOXED_MODEL_H_

// end of rotated_boxed_model.h

// -*- mode: c++ ; -*- 
/* simple_world_model.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-24
 * Last modified: 2010-02-24
 * 
 * License: 
 * 
 * Description: 
 *   Simple model for world geometry
 * 
 * History: 
 * 
 */

#ifndef GEOMTOOLS_SIMPLE_WORLD_MODEL_H_
#define GEOMTOOLS_SIMPLE_WORLD_MODEL_H_ 1

#include <iostream>
#include <string> 

#include <geomtools/i_model.h>
#include <geomtools/model_macros.h>
#include <geomtools/placement.h>
#include <geomtools/box.h>
#include <geomtools/physical_volume.h>

namespace geomtools {

  // define a geometry model with a single box: 
  GEOMTOOLS_MODEL_CLASS_DECLARE(simple_world_model) 
  {
  public:

    static const std::string SETUP_LABEL;

  public: 

    const geomtools::box & get_solid () const;

  public:
  
    simple_world_model ();
  
    virtual ~simple_world_model ();

  public:

    virtual std::string get_model_id () const;

  protected:
  
    virtual void _at_construct (const std::string & name_,
                                const datatools::properties & setup_,
                                models_col_type * models_ = 0);
      
  public: 

    virtual void tree_dump (std::ostream & out_         = std::clog, 
                            const std::string & title_  = "", 
                            const std::string & indent_ = "", 
                            bool inherit_          = false) const;

  private:

    const i_model      *        _setup_model_;
    geomtools::placement        _setup_placement_;
    geomtools::physical_volume  _setup_phys_;
    geomtools::box              _solid_;
    double                      _world_x_;
    double                      _world_y_;
    double                      _world_z_;
    std::string                 _material_;

    // registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(simple_world_model);
  
  };

} // end of namespace geomtools

#endif // GEOMTOOLS_SIMPLE_WORLD_MODEL_H_

// end of simple_world_model.h

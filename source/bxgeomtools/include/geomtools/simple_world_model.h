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

#ifndef __geomtools__simple_world_model_h
#define __geomtools__simple_world_model_h 1

#include <iostream>
#include <string> 

#include <geomtools/i_model.h>
#include <geomtools/model_macros.h>
#include <geomtools/placement.h>
#include <geomtools/box.h>
#include <geomtools/physical_volume.h>

namespace geomtools {

  using namespace std;

  // define a geometry model with a single box: 
  GEOMTOOLS_MODEL_CLASS_DECLARE(simple_world_model) 
  {
  public:

    static const string SETUP_LABEL;

  public: 

    const geomtools::box & get_solid () const;

  public:
  
    simple_world_model ();
  
    virtual ~simple_world_model ();

  public:

    virtual string get_model_id () const;

  protected:
  
    virtual void _at_construct (const string & name_,
                                const datatools::utils::properties & setup_,
                                models_col_t * models_ = 0);
      
  public: 

    virtual void tree_dump (ostream & out_         = clog, 
                            const string & title_  = "", 
                            const string & indent_ = "", 
                            bool inherit_          = false) const;

  private:

    const i_model      *        _setup_model_;
    geomtools::placement        _setup_placement_;
    geomtools::physical_volume  _setup_phys_;
    geomtools::box              _solid_;
    double                      _world_x_;
    double                      _world_y_;
    double                      _world_z_;
    string                      _material_;

    // registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(simple_world_model);
  
  };

} // end of namespace geomtools

#endif // __geomtools__simple_world_model_h

// end of simple_world_model.h

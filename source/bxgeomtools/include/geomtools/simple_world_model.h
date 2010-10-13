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
#include <exception>

#include <geomtools/i_model.h>

#include <geomtools/box.h>
#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/visibility.h>

#include <geomtools/i_model.h>

namespace geomtools {

  using namespace std;

  // define a geometry model with a single box: 
  class simple_world_model : public geomtools::i_model 
    {
    public:

      static const string SETUP_LABEL;

    private:
      const i_model      *        __setup_model;
      geomtools::placement        __setup_placement;
      geomtools::physical_volume  __setup_phys;
      geomtools::box              __solid;
      double                      __world_x;
      double                      __world_y;
      double                      __world_z;
      string                      __material;

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

    private:

      static creator_registration<simple_world_model> __CR;
      
    public: 

      virtual void tree_dump (ostream & out_         = clog, 
                              const string & title_  = "", 
                              const string & indent_ = "", 
                              bool inherit_          = false) const;
  
    };

} // end of namespace geomtools

#endif // __geomtools__simple_world_model_h

// end of simple_world_model.h

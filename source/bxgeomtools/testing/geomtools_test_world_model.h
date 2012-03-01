// -*- mode: c++ ; -*- 
/* test_world_model.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-24
 * Last modified: 2010-02-24
 * 
 * License: 
 * 
 * Description: 
 *   Test model for world geometry
 * 
 * History: 
 * 
 */

#ifndef __geomtools__test__test_world_model_h
#define __geomtools__test__test_world_model_h 1

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
  class test_world_model : public geomtools::i_model 
    {
    private:
      const i_model      *        __setup_model;
      geomtools::placement        __setup_placement;
      geomtools::physical_volume  __setup_phys;
      geomtools::box              __solid;

    public: 

      const geomtools::box & get_solid () const;

    public:
  
      test_world_model ();
  
      virtual ~test_world_model ();

    public:

      virtual string get_model_id () const;

    protected:
  
      virtual void _at_construct (const string & name_,
				  const datatools::utils::properties & setup_,
				  models_col_t * models_ = 0);

    private:

      static creator_registration<test_world_model> __CR;
      
    public: 
      virtual void tree_dump (ostream & out_         = clog, 
                              const string & title_  = "", 
                              const string & indent_ = "", 
                              bool inherit_          = false) const;
  
    };

} // end of namespace geomtools

#endif // __geomtools__test__test_world_model_h

// end of test_world_model.h

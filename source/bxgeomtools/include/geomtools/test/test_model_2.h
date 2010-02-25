// -*- mode: c++ ; -*- 
/* test_model_2.h
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

#ifndef __geomtools__test__test_model_2_h
#define __geomtools__test__test_model_2_h 1

#include <iostream>
#include <string> 
#include <exception>

#include <geomtools/i_model.h>

#include <geomtools/box.h>
#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>

#include <geomtools/test/test_model_1.h>

namespace geomtools {

  using namespace std;

  // define a geometry model with a single box: 
  class test_model_2 : public geomtools::i_model 
    {
    private:
      double                      __distance;
      double                      __phi;
      double                      __theta;
      const test_model_1 *        __sub1_model;
      const test_model_1 *        __sub2_model;
      geomtools::placement        __sub1_placement;
      geomtools::placement        __sub2_placement;
      geomtools::physical_volume  __sub1_phys;
      geomtools::physical_volume  __sub2_phys;
      geomtools::box              __solid;

    public: 

      const geomtools::box & get_solid () const;

    public:
  
      test_model_2 ();
  
      virtual ~test_model_2 ();

    public:

      virtual string get_model_id () const;

    protected:
  
      virtual void _at_construct (const string & name_,
				  const datatools::utils::properties & config_,
				  models_col_t * models_ = 0);

    private:

      static creator_registration<test_model_2> __CR;
      
    public: 
      virtual void tree_dump (ostream & out_         = clog, 
                              const string & title_  = "", 
                              const string & indent_ = "", 
                              bool inherit_          = false) const;
  
    };

} // end of namespace geomtools

#endif // __geomtools__test__test_model_2_h

// end of test_model_2.h

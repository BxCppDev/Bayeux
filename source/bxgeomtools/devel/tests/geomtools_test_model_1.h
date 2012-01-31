// -*- mode: c++ ; -*- 
/* test_model_1.h
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

#ifndef __geomtools__test__test_model_1_h
#define __geomtools__test__test_model_1_h 1

#include <iostream>
#include <string> 
#include <exception>

#include <geomtools/i_model.h>
#include <geomtools/box.h>
#include <geomtools/visibility.h>

namespace geomtools {

  using namespace std;

  // define a geometry model with a single box: 
  class test_model_1 : public geomtools::i_model 
    {
    private:

      geomtools::box __solid;

    public: 

      const geomtools::box & get_solid () const;

    public:
  
      test_model_1 ();
  
      virtual ~test_model_1 ();

    public:

      virtual string get_model_id () const;

    protected:
  
      virtual void _at_construct (const string & name_,
				  const datatools::utils::properties & config_,
				  models_col_t * models_ = 0);

    private:

      static creator_registration<test_model_1> __CR;
      
    public: 
      virtual void tree_dump (ostream & out_         = clog, 
                              const string & title_  = "", 
                              const string & indent_ = "", 
                              bool inherit_          = false) const;
  
    };

} // end of namespace geomtools

#endif // __geomtools__test__test_model_1_h

// end of test_model_1.h

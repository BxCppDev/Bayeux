// -*- mode: c++ ; -*-
/* geomtools_test_model_1.h
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

#ifndef GEOMTOOLS_TEST_TEST_MODEL_1_H_
#define GEOMTOOLS_TEST_TEST_MODEL_1_H_ 1

#include <iostream>
#include <string>
#include <exception>

#include <geomtools/i_model.h>
#include <geomtools/box.h>
#include <geomtools/visibility.h>

namespace geomtools {

  using namespace std;

  // define a geometry model with a single box:
  GEOMTOOLS_MODEL_CLASS_DECLARE(test_model_1)
  {
    public:

      const geomtools::box & get_solid () const;

      test_model_1 ();

      virtual ~test_model_1 ();

      virtual string get_model_id () const;

      virtual void tree_dump (ostream & out_         = clog,
                              const string & title_  = "",
                              const string & indent_ = "",
                              bool inherit_          = false) const;

    protected:

      virtual void _at_construct (const datatools::properties & config_,
                                  models_col_type * models_ = 0);
    private:

      geomtools::box _solid_; //!< The mother box solid volume


      // registration interface :
      GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(test_model_1)

    };

} // end of namespace geomtools

#endif // GEOMTOOLS_TEST_TEST_MODEL_1_H_

// end of geomtools_test_model_1.h

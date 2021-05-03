// -*- mode: c++ ; -*-
/* geomtools_test_world_model
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

#ifndef GEOMTOOLS_TEST_TEST_WORLD_MODEL_H_
#define GEOMTOOLS_TEST_TEST_WORLD_MODEL_H_ 1

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
  GEOMTOOLS_MODEL_CLASS_DECLARE(test_world_model)
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

      virtual std::string get_model_id () const;

    protected:

      virtual void _at_construct (const datatools::properties & setup_,
                                  models_col_type * models_ = 0);

    public:

      virtual void tree_dump (std::ostream & out_         = std::clog,
                              const std::string & title_  = "",
                              const std::string & indent_ = "",
                              bool inherit_          = false) const;

      // registration interface :
      GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(test_world_model)

    };

} // end of namespace geomtools

#endif // GEOMTOOLS_TEST_TEST_WORLD_MODEL_H_

// end of geomtools_test_world_model.h

/* geomtools_test_model_2.h
 * Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef GEOMTOOLS_TEST_TEST_MODEL_2_H_
#define GEOMTOOLS_TEST_TEST_MODEL_2_H_ 1

#include <iostream>
#include <string>
#include <exception>

#include <geomtools/i_model.h>

#include <geomtools/box.h>
#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/visibility.h>

#include <geomtools_test_model_1.h>

namespace geomtools {

  /// \brief Define a geometry model with a single box that contains 2 other boxes
  class test_model_2 : public geomtools::i_model
    {

    public:

      const geomtools::box & get_solid () const;

      test_model_2 ();

      virtual ~test_model_2 ();

      virtual string get_model_id () const;

      virtual void tree_dump (std::ostream & out_         = clog,
                              const std::string & title_  = "",
                              const std::string & indent_ = "",
                              bool inherit_          = false) const;

    protected:

      virtual void _at_construct (const datatools::properties & config_,
                                  models_col_type * models_ = nullptr);
     private:

      double                      _distance_;
      double                      _phi_;
      double                      _theta_;
      const test_model_1 *        _sub1_model_;
      const test_model_1 *        _sub2_model_;
      geomtools::placement        _sub1_placement_;
      geomtools::placement        _sub2_placement_;
      geomtools::physical_volume  _sub1_phys_;
      geomtools::physical_volume  _sub2_phys_;
      geomtools::box              _solid_;

      // Registration interface :
      GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(test_model_2)

    };

} // end of namespace geomtools

#endif // GEOMTOOLS_TEST_TEST_MODEL_2_H_

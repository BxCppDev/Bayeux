/// \file geomtools/replicated_boxed_model.h
/* Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef GEOMTOOLS_REPLICATED_BOXED_MODEL_H
#define GEOMTOOLS_REPLICATED_BOXED_MODEL_H 1

// Standard libraries:
#include <string>
#include <iostream>

// This project:
#include <geomtools/i_boxed_model.h>
#include <geomtools/regular_linear_placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/box.h>

namespace geomtools {

  class box;

  // \brief A geometry model implementing a single box with daughter boxes replicated along an axis
  class replicated_boxed_model : public i_boxed_model
  {

  public:

    void set_number_of_items (size_t);
    size_t get_number_of_items ();
    void set_boxed_model (const i_model &);
    const i_model & get_boxed_model () const;

    double get_x () const;
    double get_y () const;
    double get_z () const;
    const geomtools::box & get_box () const override;
    const geomtools::box & get_solid () const;

    replicated_boxed_model ();

    ~replicated_boxed_model () override;

    std::string get_model_id () const override;

    void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const override;

  protected:

    void _at_construct (const datatools::properties & config_,
                        models_col_type * models_ = nullptr) override;
  private:

    const i_model *            _boxed_model_ = nullptr;
    regular_linear_placement   _boxed_replica_placement_;
    physical_volume            _boxed_phys_;
    size_t         _number_of_items_ = 0;
    double         _x_ = datatools::invalid_real();
    double         _y_ = datatools::invalid_real();
    double         _z_ = datatools::invalid_real();
    geomtools::box _solid_;

    // registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(replicated_boxed_model)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_REPLICATED_BOXED_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

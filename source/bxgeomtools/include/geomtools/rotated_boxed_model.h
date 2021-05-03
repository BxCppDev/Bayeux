/// \file geomtools/rotated_boxed_model.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-24
 * Last modified: 2021-04-23
 *
 * Description:
 *   Factory for geometry models
 *
 */

#ifndef GEOMTOOLS_ROTATED_BOXED_MODEL_H
#define GEOMTOOLS_ROTATED_BOXED_MODEL_H 1

// Standard libraries:
#include <iostream>
#include <string>

// This project:
#include <geomtools/i_boxed_model.h>
#include <geomtools/box.h>
#include <geomtools/physical_volume.h>
#include <geomtools/placement.h>

namespace geomtools {

  /// \brief A geometry model implementing a box with an unique rotated boxed model
  class rotated_boxed_model : public i_boxed_model
  {

  public:
    void set_boxed_model (const i_model &);

    const i_model & get_boxed_model () const;

    const geomtools::box & get_box () const override;

    const geomtools::box & get_solid () const;

    const placement & get_boxed_placement () const;

    rotated_boxed_model ();

    ~rotated_boxed_model () override;

    std::string get_model_id () const override;

    void tree_dump (std::ostream & out_ = std::clog,
                    const std::string & title_ = "",
                    const std::string & indent_ = "",
                    bool inherit_ = false) const override;

  protected:

    void _at_construct (const datatools::properties & config_,
                        models_col_type * models_ = nullptr) override;
  private:

    const i_model *  _boxed_model_ = nullptr;
    placement        _boxed_placement_;
    physical_volume  _boxed_phys_;
    geomtools::box   _solid_;

    // Registration interface
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(rotated_boxed_model)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_ROTATED_BOXED_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

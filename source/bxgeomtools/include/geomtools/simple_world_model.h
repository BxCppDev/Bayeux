/// \file geomtools/simple_world_model.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-24
 * Last modified: 2014-05-19
 *
 * License:
 *
 * Description:
 *
 *   Simple model for world geometry
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_SIMPLE_WORLD_MODEL_H
#define GEOMTOOLS_SIMPLE_WORLD_MODEL_H 1

// Standard libraries:
#include <iostream>
#include <string>

// This project:
#include <geomtools/i_model.h>
#include <geomtools/model_macros.h>
#include <geomtools/placement.h>
#include <geomtools/box.h>
#include <geomtools/physical_volume.h>

namespace geomtools {

  /// \brief A geometry model implementing a top-level/world box volume with an unique setup daughter volume
  class simple_world_model : public i_model
  {
  public:

    static const std::string SETUP_LABEL;

    const geomtools::box & get_solid () const;

    simple_world_model ();

    ~simple_world_model () override;

    std::string get_model_id () const override;

    void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const override;

  protected:

    void _at_construct (const datatools::properties & setup_,
                        models_col_type * models_) override;

  private:

    const i_model      *        _setup_model_ = nullptr;
    geomtools::placement        _setup_placement_;
    geomtools::physical_volume  _setup_phys_;
    geomtools::box              _solid_;
    double                      _world_x_;
    double                      _world_y_;
    double                      _world_z_;
    //std::string                 _material_;

    // Registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(simple_world_model)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_SIMPLE_WORLD_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

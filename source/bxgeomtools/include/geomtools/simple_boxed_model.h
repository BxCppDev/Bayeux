/// \file geomtools/simple_boxed_model.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-24
 * Last modified: 2010-02-24
 *
 * License:
 *
 * Description:
 *   Simple boxed models
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_SIMPLE_BOXED_MODEL_H
#define GEOMTOOLS_SIMPLE_BOXED_MODEL_H 1

// Standard library:
#include <string>

// This project:
#include <geomtools/i_boxed_model.h>
#include <geomtools/box.h>

namespace geomtools {

  /// \brief A geometry model implementing a single box with no daughter volumes
  class simple_boxed_model : public i_boxed_model
  {

  public:

    void set_x (double x_);

    void set_y (double y_);

    void set_z (double z_);

    double get_x () const;

    double get_y () const;

    double get_z () const;

    void set_material_name (const std::string &);

    const std::string & get_material_name () const;

    const geomtools::box & get_box () const override;

    const geomtools::box & get_solid () const;

    simple_boxed_model ();

    ~simple_boxed_model () override;

    std::string get_model_id () const override;

    void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const override;

  protected:

    void _at_construct (const std::string & name_,
                                const datatools::properties & config_,
                                models_col_type * models_ = 0) override;
  private:

    std::string    _material_name_;
    double         _x_;
    double         _y_;
    double         _z_;
    geomtools::box _solid_;

    // registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(simple_boxed_model)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_SIMPLE_BOXED_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

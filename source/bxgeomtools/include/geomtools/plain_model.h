/// \file geomtools/plain_model.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2021-04-20
 * Last modified: 2021-04-20
 *
 * Description:
 *
 *   Model implementing a logical volume with an externally referenced shape
 *
 */

#ifndef GEOMTOOLS_PLAIN_MODEL_H
#define GEOMTOOLS_PLAIN_MODEL_H 1

// Standard library:
#include <string>
#include <iostream>

// This project:
#include <geomtools/i_model.h>
#include <geomtools/placement.h>

namespace geomtools {

  /// \brief A geometry model implementing an unique simple 3D shape, referenced from outside
  class plain_model
    : public i_model
  {
  public:
    
    plain_model() = default;

    plain_model(const geomtools::i_shape_3d & solid_, const std::string & material_name_);

    ~plain_model() override = default;

    std::string get_model_id() const override;

    void set_solid(const geomtools::i_shape_3d & solid_);

    const geomtools::i_shape_3d & get_solid() const;

    void set_material_name(const std::string & material_name_);

    const std::string & get_material_name() const;
    
    void tree_dump(std::ostream & out_         = std::clog,
                   const std::string & title_  = "",
                   const std::string & indent_ = "",
                   bool inherit_          = false) const override;


  protected:

    void _pre_construct (datatools::properties & setup_,
                         models_col_type * models_) override;

    void _at_construct (const std::string & name_,
                        const datatools::properties & config_,
                        models_col_type * models_ = 0) override;

  private:
    
    const geomtools::i_shape_3d * _solid_ = nullptr; //!< Solid handle
    std::string _material_name_; //!< The name of the material the shape is made of

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_PLAIN_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

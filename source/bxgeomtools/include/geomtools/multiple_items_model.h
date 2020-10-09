/// \file geomtools/multiple_items_model.h
/* Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-20
 * Last modified: 2014-05-19
 *
 * License:
 *
 * Description:
 *
 *   A model that hosts several objects (items) in a box mother shape
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_MULTIPLE_ITEMS_MODEL_H
#define GEOMTOOLS_MULTIPLE_ITEMS_MODEL_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>

// This project:
#include <geomtools/i_boxed_model.h>
#include <geomtools/model_with_internal_items_tools.h>
#include <geomtools/box.h>

namespace geomtools {

  /// \brief A geometry model implementing a simple box with several daughter volumes
  class multiple_items_model : public i_boxed_model
  {
  public:

    MWIM & grab_internals ();

    const MWIM & get_internals () const;

    const geomtools::box & get_box () const override;

    const box & get_solid () const;

    const std::string & get_material_name () const;

    void set_material_name (const std::string &);

    std::string get_model_id () const override;

    /// Default constructor
    multiple_items_model ();

    /// Destructor
    ~multiple_items_model () override;

    /// Smart print
    void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const override;

  protected:

    /// Construction
    void _at_construct (const std::string & label_,
                                const datatools::properties & config_,
                                models_col_type * models_ = 0) override;

  private:

    std::string      _material_name_; /// Name of the material
    box              _solid_; /// The mother box
    MWIM             _internals_; /// The register of internal/daughter volumes

    // registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(multiple_items_model)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_MULTIPLE_ITEMS_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

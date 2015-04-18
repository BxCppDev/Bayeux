/// \file geomtools/replicated_model.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-02-21
 * Last modified: 2014-05-17
 *
 * License:
 *
 * Description:
 *   Replicated model
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_REPLICATED_MODEL_H
#define GEOMTOOLS_REPLICATED_MODEL_H 1

// Standard library:
#include <string>
#include <iostream>

// This project:
#include <geomtools/i_boxed_model.h>
#include <geomtools/regular_linear_placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/box.h>
#include <geomtools/i_stackable.h>
#include <geomtools/model_with_internal_items_tools.h>

namespace geomtools {

  class box;

  // \brief A geometry model implementing a single box with daughter volumes replicated along an axis
  class replicated_model : public i_boxed_model
  {

  public:

    /// Set the number of replicated items
    void set_number_of_items (size_t);

    /// Return the number of replicated items
    size_t get_number_of_items() const;

    /// Set the replicated model
    void set_model (const i_model &);

    /// Return  a reference to the replicated model
    const i_model & get_model () const;

    /// Return the replication step along the axis
    double get_step () const;

    /// Return the X dimension of the mother box
    double get_x () const;

    /// Return the Y dimension of the mother box
    double get_y () const;

    /// Return the Z dimension of the mother box
    double get_z () const;

    /// Return a reference to the mother box
    virtual const geomtools::box & get_box () const;

    /// Return a reference to the mother box
    const geomtools::box & get_solid () const;

    /// Constructor
    replicated_model ();

    /// Destructor
    virtual ~replicated_model ();

    /// Return the model unique class Id
    virtual std::string get_model_id() const;

    /// Smart print
    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const;

  protected:

    /// Pre-construction
    virtual void _pre_construct (datatools::properties & setup_,
                                 models_col_type * models_);

    /// Post construction
    virtual void _post_construct (datatools::properties & setup_,
                                  models_col_type * models_);

    /// Construction
    virtual void _at_construct (const std::string & name_,
                                const datatools::properties & config_,
                                models_col_type * models_ = 0);
  private:

    const i_model *            _model_; /// Replicated model
    regular_linear_placement   _replica_placement_; /// Replication placement along an axis
    physical_volume            _phys_;  /// Physical volume
    size_t         _number_of_items_;   /// Number of replicated volumes
    double         _x_;         /// X dimension of the mother box
    double         _y_;         /// Y dimension of the mother box
    double         _z_;         /// Z dimension of the mother box
    double         _step_;      /// Replication step length
    stackable_data _sd_;        /// Not used ?
    geomtools::box _solid_;     /// Mother box
    MWIM           _internals_; /// internal items within the shape

    // registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(replicated_model);

  };

} // end of namespace geomtools

#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(geomtools::replicated_model)

#endif // GEOMTOOLS_REPLICATED_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

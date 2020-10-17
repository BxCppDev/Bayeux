/// \file geomtools/replicated_circular_model.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-05-15
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

#ifndef GEOMTOOLS_REPLICATED_CIRCULAR_MODEL_H
#define GEOMTOOLS_REPLICATED_CIRCULAR_MODEL_H 1

// Standard library:
#include <string>
#include <iostream>

// This project:
#include <geomtools/i_boxed_model.h>
#include <geomtools/regular_circular_placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/tube.h>
#include <geomtools/i_stackable.h>
#include <geomtools/model_with_internal_items_tools.h>

namespace geomtools {


  // Define a geometry model with a replicated circular volume within a tube:
  class replicated_circular_model : public i_model
  {

  public:

    /// Set the replicated model
    void set_model(const i_model &);

    /// Return the replicated model
    const i_model & get_model() const;

    /// Return the mother solid
    const geomtools::tube & get_solid() const;

    /// Return the model Id
    std::string get_model_id() const override;

    /// Return a reference to the embedded the regular circular placement object
    const regular_circular_placement & get_replica_placement() const;

    /// Return a reference to the embedded MWIM object
    const MWIM & get_internals () const;

    /// Default constructor
    replicated_circular_model();

    /// Destructor
    ~replicated_circular_model() override;

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const override;

  protected:

    /// Pre-construction
    void _pre_construct(datatools::properties & setup_,
                                models_col_type * models_ = 0) override;

    /// Construction
    void _at_construct(const std::string & name_,
                               const datatools::properties & config_,
                               models_col_type * models_ = 0) override;

    /// Post-construction
    void _post_construct (datatools::properties & setup_,
                                  models_col_type * models_ = 0) override;

  private:

    const i_model *            _model_; /// The handle to the geoemtey model to be replicated
    regular_circular_placement _replica_placement_; /// The circular replication rules
    physical_volume            _phys_; /// The physical volume
    // stackable_data             _sd_;
    geomtools::tube            _solid_; /// The mother shape
    MWIM                       _internals_; /// Internal items within the mother shape

    // registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(replicated_circular_model)

  };

} // end of namespace geomtools

#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(geomtools::replicated_circular_model)

#endif // GEOMTOOLS_REPLICATED_CIRCULAR_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

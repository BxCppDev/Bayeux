/// \file emfield/placement_field.h
/* Author (s):    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-04-27
 * Last modified: 2013-02-24
 *
 * License:
 *
 * Description:
 *
 *   Placement field
 *
 * History:
 *
 */

#ifndef EMFIELD_PLACEMENT_FIELD_H
#define EMFIELD_PLACEMENT_FIELD_H 1

// Third party:
// - Bayeux/geomtools:
#include <geomtools/placement.h>

// This project:
#include <emfield/base_electromagnetic_field.h>

namespace emfield {

  /** \brief Class that recomputes a given field in some arbitrary coordinate system
   *         using a placement object to operate the transformation with respect to
   *         the global coordinate system.
   */
  EMFIELD_CLASS_DECLARE(placement_field)
  {
  public:

    EMFIELD_INTERFACE_CTOR_DTOR(placement_field);

    /// Return the placement
    const geomtools::placement & get_placement() const;

    /// Set the placement
    void set_placement(const geomtools::placement & p_);

    /// Set the referenced field
    void set_field(base_electromagnetic_field::handle_type &);

  private:

    base_electromagnetic_field::handle_type _field_; //!< Handle to the field to be moved
    geomtools::placement _placement_; //!< Placement (translation and rotation) of the field

    // Macro to automate the registration of the EM field :
    EMFIELD_REGISTRATION_INTERFACE(placement_field);

  };

} // end of namespace emfield

#endif // EMFIELD_PLACEMENT_FIELD_H

// Local Variables: --
// mode: c++ --
// End: --

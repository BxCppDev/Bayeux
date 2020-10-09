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
  class placement_field : public base_electromagnetic_field
  {
  public:

    /// Default constructor
    placement_field(uint32_t = 0);

    /// Destructor
    ~placement_field() override;

    /// Initialization
    void initialize(const ::datatools::properties &,
                            ::datatools::service_manager &,
                            ::emfield::base_electromagnetic_field::field_dict_type &) override;

    /// Reset
    void reset() override;

    /// Compute electric field
    int compute_electric_field(const ::geomtools::vector_3d & position_,
                                       double time_,
                                       ::geomtools::vector_3d & electric_field_) const override;

    /// Compute magnetic field
    int compute_magnetic_field(const ::geomtools::vector_3d & position_,
                                       double time_,
                                       geomtools::vector_3d & magnetic_field_) const override;

    /// Return the placement
    const geomtools::placement & get_placement() const;

    /// Set the placement
    void set_placement(const geomtools::placement & p_);

    /// Set the referenced field
    void set_field(base_electromagnetic_field::handle_type &);

    /// Smart print
    void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_               = false) const override;

  protected:

    void _set_defaults();

  private:

    base_electromagnetic_field::handle_type _field_; //!< Handle to the field to be moved
    geomtools::placement _placement_; //!< Placement (translation and rotation) of the field

    // Macro to automate the registration of the EM field :
    EMFIELD_REGISTRATION_INTERFACE(placement_field)

  };

} // end of namespace emfield

#endif // EMFIELD_PLACEMENT_FIELD_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

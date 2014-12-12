/// \file emfield/oscillating_field.h
/* Author (s):    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-05-22
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

#ifndef EMFIELD_OSCILLATING_FIELD_H
#define EMFIELD_OSCILLATING_FIELD_H 1

// Third party:
// - Bayeux/geomtools:
#include <geomtools/placement.h>

// This project:
#include <emfield/base_electromagnetic_field.h>

namespace emfield {

  /** Class representing an uniform (in space) and constant (in time)
   *  electric field
   */
  EMFIELD_CLASS_DECLARE(oscillating_field)
  {
  public:

    EMFIELD_INTERFACE_CTOR_DTOR(oscillating_field);

    double get_frequency () const;

    double get_period () const;

    double get_phase () const;

    void set_frequency (double);

    void set_phase (double);

    void set_field (base_electromagnetic_field::handle_type &);

  private:

    base_electromagnetic_field::handle_type _field_; //!< Handle to the field to be oscillating
    double _frequency_; //!< Oscillation frequency
    double _phase_; //!< Phase

    // Macro to automate the registration of the EM field :
    EMFIELD_REGISTRATION_INTERFACE(oscillating_field);

  };

} // end of namespace emfield

#endif // EMFIELD_OSCILLATING_FIELD_H

// Local Variables: --
// mode: c++ --
// End: --

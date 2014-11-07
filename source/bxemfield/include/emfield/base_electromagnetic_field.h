/// \file emfield/base_electromagnetic_field.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-04-24
 * Last modified: 2012-05-22
 *
 * License:
 *
 * Description:
 *
 *   Base class for all electro-magnetic fields
 *
 * History:
 *
 */

#ifndef EMFIELD_BASE_ELECTROMAGNETIC_FIELD_H
#define EMFIELD_BASE_ELECTROMAGNETIC_FIELD_H 1

// Standard library:
#include <iostream>
#include <string>
#include <map>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/factory_macros.h>
#include <datatools/handle.h>
#include <datatools/bit_mask.h>
#include <datatools/i_tree_dump.h>
// - Bayeux/geomtoolsdatatools:
#include <geomtools/utils.h>

namespace datatools {
    class properties;
    class service_manager;
  }

namespace emfield {

  class base_electromagnetic_field :
    public datatools::i_tree_dumpable
  {

  public:

    typedef datatools::handle<base_electromagnetic_field> handle_type;
    typedef std::map<std::string, handle_type>            field_dict_type;

    enum flag_type
      {
        DEBUG          = datatools::bit_mask::bit01,
        ELECTRIC_FIELD = datatools::bit_mask::bit02,
        MAGNETIC_FIELD = datatools::bit_mask::bit03,
        ELECTRIC_FIELD_CAN_BE_COMBINED = datatools::bit_mask::bit04,
        MAGNETIC_FIELD_CAN_BE_COMBINED = datatools::bit_mask::bit05,
        ELECTRIC_FIELD_IS_TIME_DEPENDENT = datatools::bit_mask::bit06,
        MAGNETIC_FIELD_IS_TIME_DEPENDENT = datatools::bit_mask::bit07
     };

    enum status_type
      {
        STATUS_SUCCESS          = 0,
        STATUS_ERROR            = 1,
        STATUS_NOT_INITIALIZED  = 2,
        STATUS_INVALID_POSITION_TIME = 3,
        STATUS_NO_ELECTRIC_FIELD = 4,
        STATUS_NO_MAGNETIC_FIELD = 5
      };

    static const char ELECTRIC_FIELD_LABEL = 'E';
    static const char MAGNETIC_FIELD_LABEL = 'B';

    void set_logging_priority (datatools::logger::priority priority_);

    datatools::logger::priority get_logging_priority () const;

    bool is_initialized () const;

    bool is_error () const;

    bool is_debug () const;

    void set_debug (bool debug_);

    bool is_electric_field () const;

    bool is_magnetic_field () const;

    bool electric_field_can_be_combined () const;

    bool magnetic_field_can_be_combined () const;

    bool electric_field_is_time_dependent () const;

    bool magnetic_field_is_time_dependent () const;

    base_electromagnetic_field (uint32_t flags_ = 0);

    virtual ~base_electromagnetic_field ();

    virtual int compute_electromagnetic_field (const geomtools::vector_3d & position_,
                                               double time_,
                                               geomtools::vector_3d & electric_field_,
                                               geomtools::vector_3d & magnetic_field_) const;

    virtual int compute_field (char what_,
                               const geomtools::vector_3d & position_,
                               double time_,
                               geomtools::vector_3d & field_) const;

    virtual void initialize_simple ();

    virtual void initialize_standalone (const datatools::properties & setup_);

    virtual void initialize_with_service_only (const datatools::properties & setup_,
                                               datatools::service_manager & service_manager_);

    virtual void initialize_with_dictionary_only (const datatools::properties & setup_,
                                                  field_dict_type & dictionary_);

    /*** Minimal interface ***/

    virtual void initialize (const datatools::properties & setup_,
                             datatools::service_manager & service_manager_,
                             field_dict_type & dictionary_) = 0;

    virtual void reset () = 0;

    virtual bool position_and_time_are_valid (const geomtools::vector_3d & position_,
                                              double time_) const;

    virtual int compute_electric_field (const geomtools::vector_3d & position_,
                                        double time_,
                                        geomtools::vector_3d & electric_field_) const;

    virtual int compute_magnetic_field (const geomtools::vector_3d & position_,
                                        double time_,
                                        geomtools::vector_3d & magnetic_field_) const;

    /// Smart print
    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_               = false) const;

  protected:

    void _set_initialized (bool initialized_);

    void _set_error (bool error_);

    void _set_electric_field (bool = true);

    void _set_magnetic_field (bool = true);

    void _set_electric_field_can_be_combined (bool = true);

    void _set_magnetic_field_can_be_combined (bool = true);

    void _set_electric_field_is_time_dependent (bool = true);

    void _set_magnetic_field_is_time_dependent (bool = true);

    void _parse_basic_parameters (const datatools::properties & setup_,
                                  datatools::service_manager & service_manager_,
                                  field_dict_type & dictionary_);

  private:

    void _set_defaults_ ();

    void _terminate_ ();

  private:

    datatools::logger::priority _logging_priority_; /// Logging priority threshold

    bool     _initialized_;    /// Initialization flag
    bool     _error_;          /// Error flag
    bool     _electric_field_; /// Field can provide a value for the electric field
    bool     _magnetic_field_; /// Field can provide a value for the magnetic field

    bool     _electric_field_can_be_combined_;
    bool     _magnetic_field_can_be_combined_;
    bool     _electric_field_is_time_dependent_;
    bool     _magnetic_field_is_time_dependent_;

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(base_electromagnetic_field);

  };

} // end of namespace emfield

#include <emfield/electromagnetic_field_macros.h>

#endif // EMFIELD_BASE_ELECTROMAGNETIC_FIELD_H

// Local Variables: --
// mode: c++ --
// End: --

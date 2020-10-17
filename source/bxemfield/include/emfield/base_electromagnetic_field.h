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
// - Bayeux/geomtools:
#include <geomtools/utils.h>

namespace datatools {
  class properties;
  class service_manager;
}

namespace emfield {

  /// \brief Abstract base class for all electromagnetic field
  class base_electromagnetic_field : public datatools::i_tree_dumpable
  {

  public:

    typedef datatools::handle<base_electromagnetic_field> handle_type;
    typedef std::map<std::string, handle_type>            field_dict_type;

    /// \brief Initialization flags
    enum flag_type {
      DEBUG          = datatools::bit_mask::bit01,
      ELECTRIC_FIELD = datatools::bit_mask::bit02,
      MAGNETIC_FIELD = datatools::bit_mask::bit03,
      ELECTRIC_FIELD_CAN_BE_COMBINED = datatools::bit_mask::bit04,
      MAGNETIC_FIELD_CAN_BE_COMBINED = datatools::bit_mask::bit05,
      ELECTRIC_FIELD_IS_TIME_DEPENDENT = datatools::bit_mask::bit06,
      MAGNETIC_FIELD_IS_TIME_DEPENDENT = datatools::bit_mask::bit07
    };

    /// \brief Status at field computation
    enum status_type {
      STATUS_SUCCESS               = 0,
      STATUS_ERROR                 = 1,
      STATUS_NOT_INITIALIZED       = 2,
      STATUS_INVALID_POSITION_TIME = 3,
      STATUS_NO_ELECTRIC_FIELD     = 4,
      STATUS_NO_MAGNETIC_FIELD     = 5
    };

    static const char ELECTRIC_FIELD_LABEL = 'E';
    static const char MAGNETIC_FIELD_LABEL = 'B';

    /// Return the logging priority
    void set_logging_priority(datatools::logger::priority priority_);

    /// Set the logging priority
    datatools::logger::priority get_logging_priority() const;

    /// Check initialization status
    bool is_initialized() const;

    /// Set the name of the field
    void set_name(const std::string &);

    /// Return the name of the field
    const std::string & get_name() const;

    /// Check debug status
    bool is_debug() const;

    /// Set debug status
    void set_debug(bool debug_);

    /// Check if the field is an electric field
    bool is_electric_field() const;

    /// Check if the field is a magnetic field
    bool is_magnetic_field() const;

    /// Check if the electric field can be combined
    bool electric_field_can_be_combined() const;

    /// Check if the magnetic field can be combined
    bool magnetic_field_can_be_combined() const;

    /// Check if the electric field depends on time
    bool electric_field_is_time_dependent() const;

    /// Check if the magnetic field depends on time
    bool magnetic_field_is_time_dependent() const;

    /// Constructor
    base_electromagnetic_field(uint32_t flags_ = 0);

    /// Destructor
    ~base_electromagnetic_field() override;

    /// Compute the coordinates of the electric and magnetic fields at given position and time
    virtual int compute_electromagnetic_field(const geomtools::vector_3d & position_,
                                              double time_,
                                              geomtools::vector_3d & electric_field_,
                                              geomtools::vector_3d & magnetic_field_) const;

    /// Compute the coordinates of the electric or magnetic field at given position and time
    virtual int compute_field(char label_,
                              const geomtools::vector_3d & position_,
                              double time_,
                              geomtools::vector_3d & field_) const;

    /// Naked initialization
    virtual void initialize_simple();

    /// Standalone initialization, no external resources are needed, only a set of parameters
    virtual void initialize_standalone(const datatools::properties & setup_);

    /// Initialization using a set of parameters and a service manager
    virtual void initialize_with_service_only(const datatools::properties & setup_,
                                              datatools::service_manager & service_manager_);

    /// Initialization using a set of parameters and an external dictionary of EM fields
    virtual void initialize_with_dictionary_only(const datatools::properties & setup_,
                                                 field_dict_type & dictionary_);

    /// Main initialization
    virtual void initialize(const datatools::properties & setup_,
                            datatools::service_manager & service_manager_,
                            field_dict_type & dictionary_) = 0;

    /// Reset the field
    virtual void reset() = 0;

    /// Return the class string identifier
    virtual std::string get_class_id() const = 0;

    /// Check if position and time are valid for this field
    virtual bool position_and_time_are_valid(const geomtools::vector_3d & position_,
                                             double time_) const;

    /// Compute the coordinates of the electric field at given position and time
    virtual int compute_electric_field(const geomtools::vector_3d & position_,
                                       double time_,
                                       geomtools::vector_3d & electric_field_) const;

    /// Compute the coordinates of the magnetic field at given position and time
    virtual int compute_magnetic_field(const geomtools::vector_3d & position_,
                                       double time_,
                                       geomtools::vector_3d & magnetic_field_) const;

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const override;

  protected:

    void _set_initialized(bool initialized_);

    void _set_electric_field(bool = true);

    void _set_magnetic_field(bool = true);

    void _set_electric_field_can_be_combined(bool = true);

    void _set_magnetic_field_can_be_combined(bool = true);

    void _set_electric_field_is_time_dependent(bool = true);

    void _set_magnetic_field_is_time_dependent(bool = true);

    void _parse_basic_parameters(const datatools::properties & setup_,
                                 datatools::service_manager & service_manager_,
                                 field_dict_type & dictionary_);

  protected:

    /// Set default attributes values
    void _set_defaults();

  private:

    std::string _name_;        //!< Name of the field
    bool        _initialized_; //!< Initialization flag
    datatools::logger::priority _logging_priority_; //!< Logging priority threshold
    bool     _electric_field_;                   //!< Field can provide a value for the electric field
    bool     _magnetic_field_;                   //!< Field can provide a value for the magnetic field
    bool     _electric_field_can_be_combined_;   //!< Flag to allow the combinaison of the electric field with other fields
    bool     _magnetic_field_can_be_combined_;   //!< Flag to allow the combinaison of the magnetic field with other fields
    bool     _electric_field_is_time_dependent_; //!< Flag that indicates that the electric field is time dependent
    bool     _magnetic_field_is_time_dependent_; //!< Flag that indicates that the magnetic field is time dependent

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(base_electromagnetic_field)

  };

} // end of namespace emfield

#include <emfield/electromagnetic_field_macros.h>

#endif // EMFIELD_BASE_ELECTROMAGNETIC_FIELD_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

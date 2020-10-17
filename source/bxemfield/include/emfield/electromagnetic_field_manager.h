/// \file emfield/electromagnetic_field_manager.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-04-24
 * Last modified: 2013-06-19
 *
 * License:
 *
 * Description:
 *
 *   Manager for a collection of electro-magnetic fields
 *
 * History:
 *
 */

#ifndef EMFIELD_ELECTROMAGNETIC_FIELD_MANAGER_H
#define EMFIELD_ELECTROMAGNETIC_FIELD_MANAGER_H 1

// Standard library:
#include <iostream>
#include <string>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/i_tree_dump.h>
#include <datatools/multi_properties.h>
#include <datatools/bit_mask.h>

// This project:
#include <emfield/base_electromagnetic_field.h>

namespace datatools {
  class service_manager;
}

namespace geomtools {
  class manager;
}

/// Top-level namespace of the Bayeux/emfield module library
namespace emfield {

  class geom_map;

  /// \brief Electromagnetic field manager
  class electromagnetic_field_manager : public datatools::i_tree_dumpable
  {

  public:

    /// \brief Initialization flag
    enum init_flags {
      INIT_DEBUG      = datatools::bit_mask::bit00,
      INIT_NO_PRELOAD = datatools::bit_mask::bit01
    };

    /// Type alias for the factory
    typedef base_electromagnetic_field::factory_register_type emfield_factory_type;

    /// Set the logging priority
    void set_logging_priority(datatools::logger::priority priority_);

    /// Return the logging priority
    datatools::logger::priority get_logging_priority() const;

    /// Check initialization status
    bool is_initialized() const;

    /// Set the factory preload flag
    void set_factory_preload(bool);

    /// Set the factory verbose flag
    void set_factory_verbose(bool);

    /// Check if a service manager is referenced
    bool has_service_manager() const;

    /// Set the reference to a service manager
    void set_service_manager(datatools::service_manager &);

    /// Reset the reference to a service manager
    void reset_service_manager();

    /// Check if a geometry manager is referenced
    bool has_geometry_manager() const;

    /// Set the reference to a geometry manager
    void set_geometry_manager(const geomtools::manager &);

    /// Return a const reference to a geometry manager
    const geomtools::manager & get_geometry_manager() const;

    /// Check if a geometry/EM-fields association map is defined
    bool has_geom_map() const;

    /// Default constructor
    electromagnetic_field_manager(uint32_t flags_ = 0);

    /// Destructor
    ~electromagnetic_field_manager() override;

    /// Initialize the manager
    void initialize(const datatools::properties & setup_);

    /// Reset the manager
    void reset();

    /// Load the definitions of some EM fields from a file, given its name
    void load(const std::string & field_definitions_filename_);

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_               = false) const override;

    /// Return the dictionary of created fields
    const base_electromagnetic_field::field_dict_type & get_fields() const;

    /// CHeck if a field exists, given its name
    bool has_field(const std::string & field_name_) const;

    /// Return a mutable reference to an existing field
    base_electromagnetic_field & grab_field(const std::string & field_name_);

    /// Return a const reference to an existing field
    const base_electromagnetic_field & get_field(const std::string & field_name_) const;

    /// Return the dictionary of geometry/EM-field associations
    const geom_map & get_geom_map() const;

  protected:

    /// Set default attributes' values
    void _set_defaults();

    /// Set the initialization flag
    void _set_initialized(bool initialized_);

  private:

    /// Construct the fields
    void _construct_();

    /// Construct the geometry/EM-field association map
    void _construct_geomap_(const datatools::properties & setup_);

  private:

    // Management:
    bool _initialized_;                                   //!< Initialization flag

    // Parameters:
    datatools::logger::priority _logging_priority_;       //!< Logging priority threshold
    bool                        _factory_verbose_;        //!< Flag to activate verbosity of the factory
    bool                        _factory_preload_;        //!< Flag to preload the system factory
    bool                        _build_geom_map_;         //!< Flag to build a geometry-volume/EM-field association map
    datatools::multi_properties _field_definitions_;      //!< Parameters for fields

    // Working data:
    boost::scoped_ptr<emfield_factory_type> _factory_register_; //!< The factory register for EM field types
    datatools::service_manager * _service_manager_;       //!< Service manager
    const geomtools::manager *   _geom_manager_;          //!< Geometry manager
    base_electromagnetic_field::field_dict_type _fields_; //!< Dictionary of fields
    boost::scoped_ptr<geom_map> _geom_map_;               //!< Geometry/EM field associations map (what field is activated and where ?)

  };

} // end of namespace emfield

#include <emfield/electromagnetic_field_macros.h>

#endif // EMFIELD_ELECTROMAGNETIC_FIELD_MANAGER_H 1

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

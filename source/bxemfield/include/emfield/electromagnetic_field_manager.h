// -*- mode: c++ ; -*-
/* electromagnetic_field_manager.h
 * Author (s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#include <iostream>
#include <string>

#include <boost/cstdint.hpp>
#include <boost/scoped_ptr.hpp>

#include <datatools/i_tree_dump.h>
#include <datatools/multi_properties.h>

#include <emfield/base_electromagnetic_field.h>

#ifndef EMFIELD_ELECTROMAGNETIC_FIELD_MANAGER_H_
#define EMFIELD_ELECTROMAGNETIC_FIELD_MANAGER_H_ 1

namespace datatools {
  class service_manager;
}

namespace geomtools {
  class manager;
}

/// Top-level namespace of the Bayeux/emfield module library
namespace emfield {

  class geom_map;

  /// \brief Electromagnetic fields manager class
  class electromagnetic_field_manager :
    public datatools::i_tree_dumpable
  {

  public:

    void set_logging_priority (datatools::logger::priority priority_);

    datatools::logger::priority get_logging_priority () const;

    bool is_initialized () const;

    bool is_debug () const;

    void set_debug (bool debug_);

    bool has_service_manager () const;

    void set_service_manager (datatools::service_manager &);

    void reset_service_manager ();

    bool has_geometry_manager() const;

    bool has_geom_map () const;

    void set_geometry_manager(const geomtools::manager &);

    const geomtools::manager & get_geometry_manager() const;

    electromagnetic_field_manager (uint32_t flags_ = 0);

    virtual ~electromagnetic_field_manager ();

    void initialize (const datatools::properties & setup_);

    void reset ();

    void load (const std::string & field_definitions_filename_);

    /// Smart print
    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_               = false) const;


    const base_electromagnetic_field::field_dict_type & get_fields () const;

    bool has_field (const std::string & field_name_) const;

    base_electromagnetic_field & grab_field (const std::string & field_name_);

    const base_electromagnetic_field & get_field (const std::string & field_name_) const;

    const geom_map & get_geom_map() const;

  protected:

    void _set_initialized (bool initialized_);

  private:

    void _construct_ ();

    void _construct_geomap_ (const datatools::properties & setup_);

  private:

    datatools::logger::priority _logging_priority_;       /// Logging priority threshold
    bool _initialized_;                                   /// Initialization flag
    bool _factory_preload_;                               /// Flag to preload the system factory
    base_electromagnetic_field::factory_register_type _factory_register_; /// The factory register for EM field types
    datatools::service_manager * _service_manager_;       /// Service manager
    const geomtools::manager *   _geom_manager_;          /// Geometry manager
    datatools::multi_properties  _field_definitions_;     /// Parameters for fields
    base_electromagnetic_field::field_dict_type _fields_; /// Dictionnary of fields
    boost::scoped_ptr<geom_map> _geom_map_;               /// Geometry/EM field associations map (what field and where ?)

  };

} // end of namespace emfield

#include <emfield/electromagnetic_field_macros.h>

#endif // EMFIELD_ELECTROMAGNETIC_FIELD_MANAGER_H_ 1

// end of electromagnetic_field_manager.h

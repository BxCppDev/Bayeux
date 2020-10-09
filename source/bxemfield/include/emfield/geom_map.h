/// \file emfield/geom_map.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-03-18
 * Last modified: 2013-03-18
 *
 * License:
 *
 * Description:
 *
 *   Associative map of electro-magnetic fields and logical volumes
 *
 * History:
 *
 */

#ifndef EMFIELD_GEOM_MAP_H
#define EMFIELD_GEOM_MAP_H

// Standard library:
#include <iostream>
#include <string>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/i_tree_dump.h>
#include <datatools/multi_properties.h>

// This project:
#include <emfield/base_electromagnetic_field.h>

namespace datatools {
  class properties;
}

namespace geomtools {
  class manager;
  class logical_volume;
}

namespace emfield {

  class electromagnetic_field_manager;

  /// \brief Mapping of electromagnetic fields with geometry volume
  class geom_map :
    public datatools::i_tree_dumpable
  {

  public:

    struct association_entry
    {
    public:
      association_entry();
      inline const std::string & get_label() const { return label; }
      inline const std::string & get_logical_volume_name() const { return logical_volume_name; }
      inline bool has_logvol() const { return logvol != 0;}
      inline const geomtools::logical_volume & get_logvol() const { return *logvol;  }
      inline const std::string & get_field_name() const { return field_name; }
      inline bool has_field() const { return field != 0; }
      inline const emfield::base_electromagnetic_field & get_field() const { return *field; }
    public:
      std::string                                 label;
      std::string                                 logical_volume_name;
      const geomtools::logical_volume *           logvol;
      std::string                                 field_name;
      const emfield::base_electromagnetic_field * field;
      datatools::properties                       auxiliaries;
    };

    typedef std::map<std::string,association_entry> association_dict_type;

    geom_map();

    ~geom_map() override;

    const association_dict_type & get_associations() const;

    association_dict_type & grab_associations();

    void set_logging_priority(datatools::logger::priority priority_);

    datatools::logger::priority get_logging_priority() const;

    bool is_initialized() const;

    void initialize(const datatools::properties& config_);

    void reset();

    bool has_geometry_manager() const;

    void set_geometry_manager(const geomtools::manager & gmgr_);

    const geomtools::manager & get_geometry_manager() const;

    bool has_fields_manager() const;

    void set_fields_manager(const electromagnetic_field_manager & emfmgr_);

    const electromagnetic_field_manager & get_fields_manager() const;

    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title = "",
                           const std::string & indent_= "",
                           bool inherit_ = false) const override;

  protected :

    void _construct(const datatools::properties& config_);

  private:

    datatools::logger::priority           _logging_priority_; //!< Logging priority threshold
    bool                                  _initialized_;      //!< Initialization flag
    const geomtools::manager *            _geom_manager_;     //!< The geometry manager
    const electromagnetic_field_manager * _fields_manager_;   //!< The EM fields manager
    association_dict_type                 _associations_map_; //!< Dictionnary of "logical volumes/EM field" associations

  };

} // end of namespace emfield

#endif // EMFIELD_GEOM_MAP_H 1

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

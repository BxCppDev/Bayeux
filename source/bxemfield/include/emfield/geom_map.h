// -*- mode: c++ ; -*- 
/* geom_map.h
 * Author (s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#include <iostream>
#include <string>

#include <boost/cstdint.hpp>

#include <datatools/i_tree_dump.h>
#include <datatools/multi_properties.h>

#include <emfield/base_electromagnetic_field.h>

#ifndef EMFIELD_GEOM_MAP_H_
#define EMFIELD_GEOM_MAP_H_

namespace datatools {
  class properties;
}

namespace geomtools {
  class manager;
  class logical_volume;
}

namespace emfield {

  class electromagnetic_field_manager;

  /// \brief Electromagnetic fields manager class
  class geom_map :
    public datatools::i_tree_dumpable
  {

  public:
 
    struct association_entry
    {
    public:
      association_entry();
      inline const std::string & get_label() const { return label; }
      inline const std::string & get_geom_model_name() const { return geom_model_name; }
      inline const std::string & get_logical_volume_name() const { return logical_volume_name; }
      inline bool has_logvol() const { return logvol != 0;}
      inline const geomtools::logical_volume & get_logvol() const { return *logvol;  }
      inline const std::string & get_field_name() const { return field_name; }
      inline bool has_field() const { return field != 0; }
      inline const emfield::base_electromagnetic_field & get_field() const { return *field; }
    public:
      std::string label;
      std::string geom_model_name;
      std::string logical_volume_name;
      const geomtools::logical_volume * logvol;
      std::string field_name;
      const emfield::base_electromagnetic_field * field;
      datatools::properties auxiliaries;
    };

    typedef std::map<std::string,association_entry> association_dict_type;
     
    geom_map();

    virtual ~geom_map();

    const association_dict_type & get_associations() const;

    association_dict_type & grab_associations();

    bool is_initialized() const;

    void initialize(const datatools::properties& config_);

    void reset();

    bool is_debug() const;
    
    void set_debug(bool d_);

    bool has_geometry_manager() const;

    void set_geometry_manager(const geomtools::manager & gmgr_);

    const geomtools::manager & get_geometry_manager() const;

    bool has_fields_manager() const;

    void set_fields_manager(const electromagnetic_field_manager & emfmgr_);

    const electromagnetic_field_manager & get_fields_manager() const;

    virtual void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title = "",
                           const std::string & indent_= "",
                           bool inherit_ = false) const;

  protected :

    void _construct(const datatools::properties& config_);

  private:

    bool _initialized_; /// Initialization flag
    bool _debug_; /// Debug flag
    const geomtools::manager * _geom_manager_; /// The geometry manager
    const electromagnetic_field_manager * _fields_manager_; /// The EM fields manager
    association_dict_type _associations_map_; /// Dictionnary of "geometry/EM field" associations

  };

} // end of namespace emfield

#endif // EMFIELD_GEOM_MAP_H_ 1

// end of geom_map.h

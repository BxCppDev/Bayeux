// -*- mode: c++ ; -*- 
/* electromagnetic_field_manager.h
 * Author (s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-04-24
 * Last modified: 2013-02-24
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

namespace emfield {

  class geom_map;

  /// \brief Electromagnetic fields manager class
  class electromagnetic_field_manager :
    public datatools::i_tree_dumpable
  {

  public:
 
    /*
    /// \brief Internal record emfield handling
    class emfield_entry : public datatools::i_tree_dumpable  
    {
    public:
      enum status_type {
        STATUS_BLANK             = 0x0,
        STATUS_CREATED           = 0x1,
        STATUS_INITIALIZED       = 0x2,
      };

      /// Set the name of the emfield
      void set_name(const std::string&);

      /// Get the name of the emfield
      const std::string& get_name() const;

      /// Set the description of the emfield
      void set_description(const std::string&);
      
      /// Check is the description is not empty
      bool has_description() const;
      
      /// Get the description of the emfield
      const std::string& get_description() const;
      
      /// Set the configuration 
      const datatools::properties & get_config () const;
      
      /// Get the configuration 
      void set_config (const datatools::properties &);

      /// Grab the configuration 
      datatools::properties & grab_config ();

      /// Check if emfield is created
      bool is_created () const;

      /// Check if emfield is initialized
      bool is_initialized () const;

      /// Check if emfield can be dropped by the manager
      bool can_be_dropped () const;

      emfield_entry();

      ~emfield_entry();

      base_emfield & grab ();

      const base_emfield & get () const;
 
      virtual void tree_dump(std::ostream &      out_ = std::clog, 
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool                inherit_ = false) const;

    protected:

      void _set_status (uint32_t);

    public:

      std::string           _name_;        //!< Name of the emfield
      std::string           _id_;          //!< The ID (type) of the emfield
      std::string           _description_; //!< Description of the emfield
      datatools::properties _config_;      //!< The configuration of the emfield 
      uint32_t              _status_;      //!< Status of the emfield
      emfield_handle_type   _handle_;      //!< Handle to the emfield

      friend class manager;

    };

    */

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
  
    /*** Minimal interface ***/

    void initialize (const datatools::properties & setup_);

    void reset ();

    void load (const std::string & rules_filename_);
   
    /// Smart print
    virtual void tree_dump (std::ostream & out_         = std::clog, 
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_               = false) const;

 
    const base_electromagnetic_field::field_dict_type & get_fields () const;

    bool has_field (const std::string & field_name_) const;

    base_electromagnetic_field & grab_field (const std::string & field_name_);

    const base_electromagnetic_field & get_field (const std::string & field_name_) const;

    //geom_map & grab_geom_map();

    const geom_map & get_geom_map() const;

  protected:

    void _set_initialized (bool initialized_);

  private:

    void _construct_ ();

    void _construct_geomap_ (const datatools::properties & setup_);

  private:

    bool _factory_preload_; /// Flag to preload the system factory
    base_electromagnetic_field::factory_register_type _factory_register_; /// The factory register for EM field types
    bool     _initialized_;    /// Initialization flag
    bool     _debug_;          /// Debug flag
    datatools::service_manager * _service_manager_; /// Service manager
    const geomtools::manager *   _geom_manager_; /// Geometry manager
    datatools::multi_properties  _rules_;     /// Build rules for fields 
    base_electromagnetic_field::field_dict_type _fields_; /// Dictionnary of fields
    boost::scoped_ptr<geom_map> _geom_map_; /// Geometry/EM field associations map (what field and where ?)

  };

} // end of namespace emfield

#include <emfield/electromagnetic_field_macros.h>

#endif // EMFIELD_ELECTROMAGNETIC_FIELD_MANAGER_H_ 1

// end of electromagnetic_field_manager.h

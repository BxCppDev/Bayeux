// -*- mode: c++ ; -*- 
/* electromagnetic_field_manager.h
 * Author (s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-04-24
 * Last modified: 2012-04-24
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
#include <emfield/base_electromagnetic_field.h>
#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/multi_properties.h>

#ifndef __emfield__electromagnetic_field_manager_h
#define __emfield__electromagnetic_field_manager_h 1

namespace datatools {
  namespace service {
    class service_manager;
  }}

namespace emfield {

  class electromagnetic_field_manager :
   public datatools::utils::i_tree_dumpable
  {

  public:

    bool is_initialized () const;

    bool is_debug () const;

    void set_debug (bool debug_);

    bool has_service_manager () const;

    void set_service_manager (datatools::service::service_manager &);

    void reset_service_manager ();

    electromagnetic_field_manager (uint32_t flags_ = 0);

    virtual ~electromagnetic_field_manager ();
  
    /*** Minimal interface ***/

    void initialize (const datatools::utils::properties & setup_);

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

  protected:

    void _set_initialized (bool initialized_);

  private:

    void _construct_ ();

  private:

    base_electromagnetic_field::factory_register_type _factory_register_; /// The factory register for EM field types
    bool     _initialized_;    /// Initialization flag
    bool     _debug_;          /// Debug flag
    datatools::service::service_manager *       _service_manager_; /// Service manager
    datatools::utils::multi_properties          _rules_;     /// Build rules for fields 
    base_electromagnetic_field::field_dict_type _fields_;    /// Dictionnary of fields

  };

} // end of namespace emfield

#include <emfield/electromagnetic_field_macros.h>

#endif // __emfield__electromagnetic_field_manager_h 1

// end of electromagnetic_field_manager.h

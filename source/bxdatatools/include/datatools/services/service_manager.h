/* service_manager.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2012-04-10
 *
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Copyright (C) 2012 Ben Morgan <Ben.Morgan@warwick.ac.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *
 * Description:
 *
 *   A service manager.
 *
 * History:
 *
 */
#ifndef DATATOOLS_SERVICES_SERVICE_MANAGER_H_
#define DATATOOLS_SERVICES_SERVICE_MANAGER_H_

// Standard Library
#include <string>
#include <sstream>
#include <typeinfo>
#include <stdexcept>

// Third Party
// - Boost
#include <boost/cstdint.hpp>

// Datatools
#include <datatools/services/service_tools.h>
#include <datatools/services/base_service.h>
#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/properties.h>
#include <datatools/utils/bit_mask.h>


namespace datatools {

namespace utils {
class properties;
class multi_properties;
}

namespace service {
class service_manager : public datatools::utils::i_tree_dumpable {
 public:
  enum flag_type {
    BLANK              = 0,
    NO_PRELOAD         = datatools::utils::bit_mask::bit00,
    FORCE_INITIALIZATION_AT_LOAD = datatools::utils::bit_mask::bit01,
    DEBUG              = datatools::utils::bit_mask::bit02,
    VERBOSE            = datatools::utils::bit_mask::bit03,
  };

 public:
  /// Constructor
  service_manager(const std::string& name = "",
                  const std::string & description = "",
                  uint32_t flag = BLANK);

  /// Destructor
  ~service_manager();

  /// Set the name of the service
  void set_name(const std::string& name);

  /// Set the description of the service
  void set_description(const std::string& description);

  /// Get the name of the service
  const std::string& get_name() const;

  /// Get the description of the service
  const std::string& get_description() const;

  /// Check the debug flag
  bool is_debug() const;

  /// Set the debug flag
  void set_debug(bool debug = true);

  /// Check the initialization flag
  bool is_initialized() const;

  /// Initialize the manager
  void initialize();

  /// Initialize the manager from a container of properties 
  void initialize(const datatools::utils::properties& config);

  /// Reset the manager
  void reset();

  bool has_service_type(const std::string& id) const;

  template <class ServiceClass>
  void register_service_type(const std::string& id);

  void unregister_service_type(const std::string& id);

  /**  @param name The name of the service to be checked
   *   @return true if the manager hosts the service requested by name
   */
  bool has(const std::string& name) const;

  /**  @param name The name of the service to be checked
   *   @return true if the service is of the requested type
   */
  template <class T>
  bool is_a(const std::string& name) const;

  /** Deprecated, please use the 'grab' method.
   *  @param name The name of the service to be checked
   *  @return a mutable reference to the service instance requested by name and type
   */
  template<class T>
  T& get(const std::string& name);

  /**  Same as the mutable 'get' method
   *   @param name The name of the service to be checked
   *   @return a mutable reference to the service instance requested by name and type
   */
  template<class T>
  T& grab(const std::string& name);

  /**  @param name The name of the service to be checked
   *   @return a const reference to the service instance requested by name and type
   */
  template<class T>
  const T& get(const std::string& name) const;

  bool can_drop(const std::string& name);

  void drop(const std::string& name);

  void load(const std::string& name,
            const std::string& id,
            const datatools::utils::properties& config);

  void load(const datatools::utils::multi_properties& config);

  const service_dict_type& get_services() const;

  service_dict_type& get_services();

  void dump_services(std::ostream& out = std::clog,
                     const std::string& title  = "",
                     const std::string& indent = "") const;

  /// Smart print
  virtual void tree_dump(std::ostream& out         = std::clog,
                         const std::string& title  = "",
                         const std::string& indent = "",
                         bool inherit              = false) const;
 protected:
  void load_service(const std::string& name,
                    const std::string& id,
                    const datatools::utils::properties& config);

  void preload_global_dict();

  void create_service(service_entry& entry);

  void initialize_service(service_entry& entry);

  void reset_service(service_entry& entry);


 private:
  /// Set the factory preload flag
  void set_preload(bool preload);

 private:
  bool         initialized_;
  std::string  name_;
  std::string  description_;
  bool         debug_;
  bool         preload_;
  bool         force_initialization_at_load_;

// 2012-04-09 FM : support for datatools::factory system :
  base_service::factory_register_type  factory_register_;
  service_dict_type                    services_;
};


//----------------------------------------------------------------------
// class service_manager template method implementations.
// TODO : Refactor into -inl.h file

template <class T>
bool service_manager::is_a(const std::string& name) const {
  service_dict_type::const_iterator found = services_.find(name);
  if (found == services_.end()) {
    std::ostringstream message;
    message << "datatools::services::service_manager::is_a: No service named '" << name << "' !";
    throw std::logic_error(message.str());
  }

  const std::type_info& ti = typeid(T);
  const std::type_info& tf = typeid(found->second.service_handle.get());
  return (ti == tf);
}


template<class T>
T& service_manager::get(const std::string& name) {
  service_dict_type::iterator found = services_.find(name);
  if (found == services_.end()) {
    std::ostringstream message;
    message << "datatools::services::service_manager::get: "
        << "No service named '" 
        << name 
        << "' !";
    throw std::logic_error(message.str());
  }
  service_entry& entry = found->second;
  if (!(entry.service_status & service_entry::STATUS_INITIALIZED)) {
    this->initialize_service(entry);
  }
  return dynamic_cast<T&>(entry.service_handle.get());
}


template<class T>
T& service_manager::grab(const std::string& name) {
  return this->get<T>(name);
}


template<class T>
const T& service_manager::get(const std::string& name) const {
  service_manager* sm = const_cast<service_manager*>(this);
  return const_cast<T&>(sm->get<T>(name));
}


template <class ServiceClass>
void service_manager::register_service_type(const std::string& id) {
  factory_register_.registration(id, boost::factory<ServiceClass*>());
}


}  // end of namespace service
}  // end of namespace datatools

#endif // DATATOOLS_SERVICES_SERVICE_MANAGER_H_


/* service_manager-inl.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2013-02-24
 */

#ifndef DATATOOLS_SERVICE_MANAGER_INL_H_
#define DATATOOLS_SERVICE_MANAGER_INL_H_

namespace datatools {

//----------------------------------------------------------------------
// class service_manager template method implementations.

template <class T>
bool service_manager::is_a(const std::string& name) const {
  service_dict_type::const_iterator found = services_.find(name);
  if (found == services_.end()) {
    std::ostringstream message;
    message << "datatools::service_manager::is_a: No service named '" << name << "' !";
    throw std::logic_error(message.str());
  }
  const service_entry& entry = found->second;
  if (!entry.is_initialized()) {
    std::clog << "NOTICE: " << "datatools::service_manager::is_a<>: "
	      << "Forcing initialization of the service named '" <<name 
	      << "'..." << std::endl;
    service_manager * mutable_this = const_cast<service_manager *>(this);
    service_entry& mutable_entry = const_cast<service_entry &>(entry);
    mutable_this->initialize_service(mutable_entry);
  }

  const std::type_info& ti = typeid(T);
  const std::type_info& tf = typeid(found->second.get_service_handle().get());
  return (ti == tf);
}


template<class T>
T& service_manager::grab(const std::string& name) {
  service_dict_type::iterator found = services_.find(name);
  if (found == services_.end()) {
    std::ostringstream message;
    message << "datatools::service_manager::grab: "
        << "No service named '" 
        << name 
        << "' !";
    throw std::logic_error(message.str());
  }
  service_entry& entry = found->second;
  if (!entry.is_initialized()) {
    this->initialize_service(entry);
  }
  return dynamic_cast<T&>(entry.grab_service_handle().grab());
}


template<class T>
T& service_manager::get(const std::string& name) {
  return this->grab<T>(name);
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
}  // end of namespace datatools

#endif // DATATOOLS_SERVICE_MANAGER_INL_H_

// end of service_manager-inl.h

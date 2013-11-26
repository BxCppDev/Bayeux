// -*- mode: c++; -*-
/* component_manager-inl.h
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2013-11-12
 * Last modified : 2013-11-13
 */

#ifndef ELECTRONICS_COMPONENT_MANAGER_INL_H_
#define ELECTRONICS_COMPONENT_MANAGER_INL_H_

namespace electronics {

  //----------------------------------------------------------------------
  // class component_manager template method implementations.

  template <class T>
  bool component_manager::is_a(const std::string& name) const {
    component_pool_type::const_iterator found = _components_.find(name);
    DT_THROW_IF (found == _components_.end(),
                 std::logic_error,
                 "No component named '" << name << "' !");
    const component_entry& entry = found->second;
    if (!entry.is_initialized()) {
      DT_LOG_NOTICE(get_logging_priority(),"Forcing initialization of the component named '" <<name << "'...");
      component_manager * mutable_this = const_cast<component_manager *>(this);
      component_entry& mutable_entry = const_cast<component_entry &>(entry);
      mutable_this->initialize_component(mutable_entry);
    }
    const std::type_info& ti = typeid(T);
    const std::type_info& tf = typeid(found->second.get_component_handle().get());
    return (ti == tf);
  }


  template<class T>
  T& component_manager::grab(const std::string& name) {
    component_pool_type::iterator found = _components_.find(name);
    DT_THROW_IF (found == _components_.end(),
                 std::logic_error,
                 "No component named '" << name << "' !");
    component_entry& entry = found->second;
    if (!entry.is_initialized()) {
      this->initialize_component(entry);
    }
    return dynamic_cast<T&>(entry.grab_component_handle().grab());
  }


  template<class T>
  T& component_manager::get(const std::string& name) {
    return this->grab<T>(name);
  }


  template<class T>
  const T& component_manager::get(const std::string& name) const {
    component_manager* sm = const_cast<component_manager*>(this);
    return const_cast<T&>(sm->get<T>(name));
  }


  template <class ComponentClass>
  void component_manager::register_component_type(const std::string& id) {
    _factory_register_.registration(id, boost::factory<ComponentClass*>());
  }

} // end of namespace electronics

#endif // ELECTRONICS_COMPONENT_MANAGER_INL_H_

// end of component_manager-inl.h

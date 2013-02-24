/* things-inl.h */

#ifndef DATATOOLS_THINGS_INL_H_
#define DATATOOLS_THINGS_INL_H_

namespace datatools {

//----------------------------------------------------------------------
// things template methods
//
template<class T>
T& things::add(const std::string& a_name, const std::string& a_desc, 
               bool a_const) {
  T* new_obj = 0;
  new_obj = new T;

  if (new_obj == 0) {
    std::ostringstream message;
    message << "datatools::things::add<T>: " 
        << "Cannot allocate object '" << a_name << "' !";
    throw std::logic_error(message.str());
  }

  datatools::i_serializable* new_cast 
      = dynamic_cast<datatools::i_serializable*>(new_obj); 
  if (new_cast == 0) {
    if (new_obj != 0) {
      delete new_obj;
    }
    const std::type_info& ti = typeid(T);
    std::ostringstream message;
    message << "datatools::things::add<T>: Request type '" 
        << ti.name() 
        << "' does not inherit from the '" 
        << "datatools::i_serializable"  
        << "' base class !";
    throw bad_things_cast(message.str());
  }
  this->add_impl(a_name, new_cast, a_desc, a_const);
  return *new_obj;
}


template<class T>
T* things::pop(const std::string& a_name) {
  dict_type::iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::things::pop: No stored object has name '" 
        << a_name << "' !";
    throw std::logic_error(message.str());
    return 0;
  }
  const std::type_info& ti = typeid(T);
  const std::type_info& tf = typeid(*(found->second.handle));
  if (ti != tf) {
    T tmp;
    std::ostringstream message;
    message << "datatools::things::pop: Request type '" 
        << ti.name() 
        << "' ('" << tmp.get_serial_tag() 
        << "') does not match the type '" 
        << tf.name() 
        << "' of the stored object named '" 
        << a_name 
        << "' ('" 
        << found->second.handle->get_serial_tag() 
        << "') !";
    throw bad_things_cast(message.str());
  }
  T* ptr = dynamic_cast<T*>(found->second.handle);
  found->second.handle = 0;
  things_.erase(found);
  return ptr;
}


template<class T>
T& things::grab(const std::string& a_name) {
  dict_type::iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::things::grab: No stored object has name '" 
        << a_name << "' !";
    throw std::logic_error(message.str());
  }
  const std::type_info& ti = typeid(T);
  const std::type_info& tf = typeid(*(found->second.handle));
  if (ti != tf) {
    T tmp;
    std::ostringstream message;
    message << "datatools::things::grab: Request type '" 
        << ti.name() 
        << "' ('" 
        << tmp.get_serial_tag() 
        << "') does not match the type '" 
        << tf.name() 
        << "' of the stored object named '" 
        << a_name 
        << "' ('" 
        << found->second.handle->get_serial_tag() 
        << "') !";
    throw bad_things_cast(message.str());
  }
  if (found->second.is_const()) {
    std::ostringstream message;
    message << "datatools::things::grab: Object named '" 
        << a_name << "' is constant !";
    throw std::logic_error(message.str());
  }
  return *(dynamic_cast<T*>(found->second.handle));
}


template<class T>
bool things::is_a(const std::string& a_name) const {
  dict_type::const_iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::things::is_a: No object named '" 
        << a_name << "' !";
    throw std::logic_error(message.str());
  }
  const std::type_info& ti = typeid(T);
  const std::type_info& tf = typeid(*found->second.handle);
  return (ti == tf); 
}


template<class T>
const T& things::get(const std::string& a_name) const {
  dict_type::const_iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::things::get: No object named '" 
        << a_name << "' !";
    throw std::logic_error(message.str());
  }
  const std::type_info& ti = typeid(T);
  const std::type_info& tf = typeid(*found->second.handle);
  if (ti != tf) {
    T tmp;
    std::ostringstream message;
    message << "datatools::things::get: Request type '" 
        << ti.name() 
        << "' ('" 
        << tmp.get_serial_tag() 
        << "') does not match the type '" 
        << tf.name() 
        << "' of the stored object named '" << a_name 
        << "' ('" 
        << found->second.handle->get_serial_tag() 
        << "') !";
    throw bad_things_cast(message.str());
  }
  return *(dynamic_cast<const T*>(found->second.handle));
}

}  // end of namespace datatools

#endif // DATATOOLS_THINGS_INL_H_

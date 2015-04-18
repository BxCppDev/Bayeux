/// \file datatools/things-inl.h

#ifndef DATATOOLS_THINGS_INL_H
#define DATATOOLS_THINGS_INL_H

// This project:
#include <datatools/exception.h>

namespace datatools {

  // Template methods of the things class:

  // Add a new bank of given type, name, description and constness
  template<class T>
  T& things::add(const std::string& a_name, const std::string& a_desc,
                 bool a_const) {
    T* new_obj = 0;
    new_obj = new T;
    DT_THROW_IF (new_obj == 0,
                 std::logic_error,
                 "Cannot allocate object '" << a_name << "' !");
    datatools::i_serializable* new_cast
      = dynamic_cast<datatools::i_serializable*>(new_obj);
    if (new_cast == 0) {
      if (new_obj != 0) {
        delete new_obj;
      }
      const std::type_info& ti = typeid(T);
      std::ostringstream message;
      message << "datatools::things::add<T>: Request type '"
              << ti.name() << "' does not inherit from the '" << "datatools::i_serializable" << "' base class !";
      DT_THROW_IF(true,
                  datatools::bad_things_cast,
                  "Request type '" << ti.name() << "' does not inherit from the '" << "datatools::i_serializable" << "' base class !");
    }
    this->add_impl(a_name, new_cast, a_desc, a_const);
    return *new_obj;
  }


  template<class T>
  T* things::pop(const std::string& a_name) {
    dict_type::iterator found = things_.find(a_name);
    DT_THROW_IF (found == things_.end(),
                 std::logic_error,
                 "No stored object has name '"  << a_name << "' !");
    const std::type_info& ti = typeid(T);
    const std::type_info& tf = typeid(*(found->second.handle));
    T tmp;
    DT_THROW_IF (ti != tf,
                 datatools::bad_things_cast,
                 "Request type '" << ti.name() << "' ('" << tmp.get_serial_tag()
                 << "') does not match the type '" << tf.name() << "' of the stored object named '"
                 << a_name << "' ('" << found->second.handle->get_serial_tag() << "') !");
    T* ptr = dynamic_cast<T*>(found->second.handle);
    found->second.handle = 0;
    things_.erase(found);
    return ptr;
  }


  template<class T>
  T& things::grab(const std::string& a_name) {
    dict_type::iterator found = things_.find(a_name);
    DT_THROW_IF (found == things_.end(),
                 std::logic_error,
                 "No stored object has name '" << a_name << "' !");
    const std::type_info& ti = typeid(T);
    const std::type_info& tf = typeid(*(found->second.handle));
    T tmp;
    DT_THROW_IF (ti != tf,
                 datatools::bad_things_cast,
                 "Request type '" << ti.name() << tmp.get_serial_tag() << "') does not match the type '"
                 << tf.name() << "' of the stored object named '" << a_name << "' ('"
                 << found->second.handle->get_serial_tag() << "') !");
    DT_THROW_IF (found->second.is_const(),
                 std::logic_error,
                 "Object named '" << a_name << "' is constant !");
    return *(dynamic_cast<T*>(found->second.handle));
  }


  template<class T>
  bool things::is_a(const std::string& a_name) const {
    dict_type::const_iterator found = things_.find(a_name);
    DT_THROW_IF (found == things_.end(),
                 std::logic_error,
                 "No object named '" << a_name << "' !");
    const std::type_info& ti = typeid(T);
    const std::type_info& tf = typeid(*found->second.handle);
    return (ti == tf);
  }


  template<class T>
  const T& things::get(const std::string& a_name) const {
    dict_type::const_iterator found = things_.find(a_name);
    DT_THROW_IF (found == things_.end(),
                 std::logic_error,
                 "No object named '" << a_name << "' !");
    const std::type_info& ti = typeid(T);
    const std::type_info& tf = typeid(*found->second.handle);
    T tmp;
    DT_THROW_IF (ti != tf,
                 datatools::bad_things_cast,
                 "Request type '" << ti.name() << "' ('" << tmp.get_serial_tag()
                 << "') does not match the type '" << tf.name() << "' of the stored object named '" << a_name
                 << "' ('" << found->second.handle->get_serial_tag() << "') !");
    return *(dynamic_cast<const T*>(found->second.handle));
  }

}  // end of namespace datatools

#endif // DATATOOLS_THINGS_INL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

// -*- mode: c++ ; -*-
/* manager-inl.h
 *
 */

#ifndef GEOMTOOLS_DETAIL_MANAGER_INL_H_
#define GEOMTOOLS_DETAIL_MANAGER_INL_H_ 1

#include <datatools/exception.h>

namespace geomtools {

  template<class T>
  bool manager::is_plugin_a (const std::string & a_plugin_name) const
  {
    plugins_dict_type::const_iterator found = _plugins_.find(a_plugin_name);
    DT_THROW_IF (found == _plugins_.end (),
                 std::logic_error,
                 "No plugin named '" << a_plugin_name << "' !");
    const plugin_entry& entry = found->second;
    if (! entry.is_initialized ()) {
      DT_LOG_NOTICE(_logging,
                    "Triggering initialization of the plugin named '"
                    << a_plugin_name
                    << "'...");
      manager * mutable_this = const_cast<manager *>(this);
      plugin_entry& mutable_entry = const_cast<plugin_entry &>(entry);
      mutable_this->_initialize_plugin(mutable_entry);
    }
    const std::type_info& ti = typeid(T);
    const std::type_info& tf = typeid(found->second.get());
    return (ti == tf);
  }

  template<class T>
  T& manager::grab_plugin (const std::string & a_plugin_name)
  {
    plugins_dict_type::iterator found = _plugins_.find(a_plugin_name);
    DT_THROW_IF (found == _plugins_.end (),
                 std::logic_error,
                 "No plugin named '" << a_plugin_name << "' !");
    plugin_entry& entry = found->second;
    if (! entry.is_initialized ()) {
      this->_initialize_plugin(entry);
    }
    const std::type_info& ti = typeid(T);
    const std::type_info& tf = typeid(found->second.get());
    DT_THROW_IF (ti != tf,
                 std::logic_error,
                 "Requested plugin type '"
                 << ti.name() << "' does not match the type '"
                 << tf.name()
                 << "' of the plugin named '" << a_plugin_name
                 << "' !");
    return dynamic_cast<T&>(found->second.grab());
  }

  template<class T>
  const T& manager::get_plugin (const std::string & a_plugin_name) const
  {
    manager* sm = const_cast<manager*>(this);
    return const_cast<T&>(sm->grab_plugin<T>(a_plugin_name));
  }

  template <class PluginClass>
  void manager::register_plugin_type(const std::string& id) {
    _plugins_factory_register_.registration(id, boost::factory<PluginClass*>());
  }


} // end of namespace geomtools

#endif // GEOMTOOLS_DETAIL_MANAGER_INL_H_


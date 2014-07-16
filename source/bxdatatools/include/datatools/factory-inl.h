
#ifndef DATATOOLS_FACTORY_INL_H
#define DATATOOLS_FACTORY_INL_H

//----------------------------------------------------------------------
// class factory_register implementation section
namespace datatools {

// Constructor:
template <typename BaseType>
factory_register<BaseType>::factory_register()
    : label_(),
      verbose_(false),
      registered_() {}

// Constructor:
template <typename BaseType>
factory_register<BaseType>::factory_register(const std::string& label,
                                             unsigned int flags)
    : label_(label),
      verbose_(false) {
  if (flags & verbose) {
    verbose_ = true;
    DT_LOG_INFORMATION(datatools::logger::PRIO_INFORMATION,"Construction (label='" << label_ << "'...");
  }
  if (this->is_verbose()) {
    DT_LOG_INFORMATION(datatools::logger::PRIO_INFORMATION,"Construction is done (label='" << label_ << "'...");
  }
}


// Destructor:
template <typename BaseType>
factory_register<BaseType>::~factory_register() {
  if (this->is_verbose()) {
    DT_LOG_INFORMATION(datatools::logger::PRIO_INFORMATION,"Destruction (label='" << label_ << "'...");
  }

  this->clear();

  if (this->is_verbose()) {
    DT_LOG_INFORMATION(datatools::logger::PRIO_INFORMATION,"Destruction is done (label='" << label_ << "'...");
  }
}


template <typename BaseType>
bool factory_register<BaseType>::is_verbose() const {
  return verbose_;
}


template <typename BaseType>
void factory_register<BaseType>::set_verbose(bool verbose) {
  verbose_ = verbose;
}


template <typename BaseType>
const std::string& factory_register<BaseType>::get_label() const {
  return label_;
}


template <typename BaseType>
void factory_register<BaseType>::set_label(const std::string& label) {
  label_ = label;
}


template <typename BaseType>
void factory_register<BaseType>::list_of_factories(
    std::vector<std::string>& ids) const {
  for (typename factory_map_type::const_iterator i = registered_.begin();
       i != registered_.end();
       ++i) {
    ids.push_back(i->first);
  }
}


template <typename BaseType>
bool factory_register<BaseType>::has(const std::string& id) const {
  return registered_.find(id) != registered_.end();
}


template <typename BaseType>
void factory_register<BaseType>::clear() {
  for (typename factory_map_type::iterator i = registered_.begin();
       i != registered_.end();
       ++i) {
    if (i->second != 0) {
      if (this->is_verbose()) {
        DT_LOG_INFORMATION(datatools::logger::PRIO_INFORMATION,"Destroying registered allocator/functor '" << i->first << "'");
      }
    }
  }
  registered_.clear();
}


template <typename BaseType>
void factory_register<BaseType>::reset() {
  this->clear();
  label_.clear();
  verbose_ = false;
}


template <typename BaseType>
typename factory_register<BaseType>::factory_type&
factory_register<BaseType>::grab(const std::string& id) {
  typename factory_map_type::iterator found = registered_.find(id);
  DT_THROW_IF (found == registered_.end(),
               std::logic_error,
               "Class ID '" << id << "' is not registered !");
  return found->second;
}


template <typename BaseType>
const typename factory_register<BaseType>::factory_type&
factory_register<BaseType>::get(const std::string& id) const {
  typename factory_map_type::const_iterator found = registered_.find(id);
  DT_THROW_IF (found == registered_.end(),
               std::logic_error,
               "Class ID '" << id << "' is not registered !");
  return found->second;
}


template <typename BaseType>
void factory_register<BaseType>::registration(const std::string& id,
                                              const factory_type& factory) {
  if (this->is_verbose()) {
    DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                  "Registration of class with ID '" << id << "'");
  }

  typename factory_map_type::const_iterator found = registered_.find(id);
  DT_THROW_IF (found != registered_.end(),
               std::logic_error,
               "Class ID '" << id << "' is already registered !");
  registered_[id] = factory;
}


template <typename BaseType>
void factory_register<BaseType>::unregistration(const std::string& id) {
  if (this->is_verbose()) {
    DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                  "Unregistration of class with ID '" << id << "'");
  }
  typename factory_map_type::const_iterator found = registered_.find(id);
  DT_THROW_IF (found == registered_.end(),
               std::logic_error,
               "Class ID '" << id << "' is not registered !");
  registered_.erase(id);
}


template <typename BaseType>
void factory_register<BaseType>::import(const factory_register& other) {
  if (this->is_verbose()) {
    DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                  "Importing registered factories from register '" << other.get_label() << "'...");
  }
  for (typename factory_map_type::const_iterator i = other.registered_.begin();
       i !=  other.registered_.end();
       ++i) {
    const factory_type& the_out_factory = i->second;
    this->registration(i->first, the_out_factory);
  }
  if (this->is_verbose()) {
    DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Done.");
  }
}


template <typename BaseType>
void factory_register<BaseType>::print(std::ostream& out,
                                       const std::string& indent) const {
  std::ostringstream title_oss;
  title_oss << "List of registered allocators/functors for label \""
            << label_
            << "\" : ";
  this->tree_dump(out, title_oss.str(), indent);
}


template <typename BaseType>
void factory_register<BaseType>::tree_dump(std::ostream& out,
                                           const std::string& title,
                                           const std::string& a_indent,
                                           bool inherit) const {
  std::string indent;
  if (!a_indent.empty()) indent = a_indent;

  if (!title.empty()) { out << indent << title << std::endl; }

  out << indent << i_tree_dumpable::tag
      << "Label   : '"
      << label_ << "'" << std::endl;
  out << indent << i_tree_dumpable::tag
      << "Verbose : "
      << verbose_ << std::endl;
  out << indent << i_tree_dumpable::inherit_tag(inherit)
      << "Registered factories : " << registered_.size() << std::endl;

  for (typename factory_map_type::const_iterator i = registered_.begin();
       i != registered_.end ();
       ++i) {
    out << a_indent;
    out << i_tree_dumpable::inherit_skip_tag(inherit);
    typename factory_map_type::const_iterator j = i;
    j++;
    if (j == registered_.end()) {
      out << i_tree_dumpable::last_tag;
    } else {
      out << i_tree_dumpable::tag;
    }

    out << "ID: \"" << i->first << "\" @ " << &i->second << std::endl;
  }
}

} // namespace datatools

#endif // DATATOOLS_FACTORY_INL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

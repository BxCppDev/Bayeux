/// \file datatools/factory-inl.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2012-03-19
 * Last modified : 2013-04-22
 *
 */

#ifndef DATATOOLS_FACTORY_INL_H
#define DATATOOLS_FACTORY_INL_H

// class factory_register implementation section
namespace datatools {

  // Constructor:
  template <typename BaseType>
  factory_register<BaseType>::factory_register()
    : label_(),
      logging_(datatools::logger::PRIO_FATAL),
      registered_() {}

  // Constructor:
  template <typename BaseType>
  factory_register<BaseType>::factory_register(const std::string& label,
                                               unsigned int flags)
    : label_(label),
      logging_(datatools::logger::PRIO_FATAL) {
    if (flags & verbose) {
      set_logging_priority(datatools::logger::PRIO_INFORMATION);
    }
    DT_LOG_INFORMATION(logging_,"Construction (label='" << label_ << "'...");
    DT_LOG_INFORMATION(logging_,"Construction is done (label='" << label_ << "'...");
  }

  // Destructor:
  template <typename BaseType>
  factory_register<BaseType>::~factory_register() {
    DT_LOG_INFORMATION(logging_,"Destruction (label='" << label_ << "'...");

    this->clear();

    DT_LOG_INFORMATION(logging_,"Destruction is done (label='" << label_ << "'...");
  }

  template <typename BaseType>
  datatools::logger::priority factory_register<BaseType>::get_logging_priority() const {
    return logging_;
  }

  template <typename BaseType>
  void factory_register<BaseType>::set_logging_priority(datatools::logger::priority logging) {
    logging_ = logging;
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
  void factory_register<BaseType>::list_of_factories(std::vector<std::string>& ids) const {
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
        DT_LOG_INFORMATION(logging_,"Destroying registered allocator/functor '" << i->first << "'");
      }
    }
    registered_.clear();
  }

  template <typename BaseType>
  void factory_register<BaseType>::reset() {
    this->clear();
    label_.clear();
    logging_ = datatools::logger::PRIO_FATAL;
  }

  template <typename BaseType>
  typename factory_register<BaseType>::factory_type&
  factory_register<BaseType>::grab(const std::string& id) {
    typename factory_map_type::iterator found = registered_.find(id);
    DT_THROW_IF(found == registered_.end(), std::logic_error,
                "Class ID '" << id << "' is not registered !");
    return found->second;
  }

  template <typename BaseType>
  const typename factory_register<BaseType>::factory_type&
  factory_register<BaseType>::get(const std::string& id) const {
    typename factory_map_type::const_iterator found = registered_.find(id);
    DT_THROW_IF(found == registered_.end(), std::logic_error,
                "Class ID '" << id << "' is not registered !");
    return found->second;
  }

  template <typename BaseType>
  void factory_register<BaseType>::registration(const std::string& id,
                                                const factory_type& factory) {
    DT_LOG_NOTICE(logging_, "Registration of class with ID '" << id << "'");

    typename factory_map_type::const_iterator found = registered_.find(id);
    DT_THROW_IF(found != registered_.end(), std::logic_error,
                "Class ID '" << id << "' is already registered !");
    registered_[id] = factory;
  }

  template <typename BaseType>
  void factory_register<BaseType>::unregistration(const std::string & id) {
    DT_LOG_NOTICE(logging_, "Unregistration of class with ID '" << id << "'");

    typename factory_map_type::const_iterator found = registered_.find(id);
    DT_THROW_IF(found == registered_.end(), std::logic_error,
                "Class ID '" << id << "' is not registered !");
    registered_.erase(id);
  }

  template <typename BaseType>
  void factory_register<BaseType>::import(const factory_register & other) {
    DT_LOG_NOTICE(logging_, "Importing registered factories from register '" << other.get_label() << "'...");
    for (typename factory_map_type::const_iterator i = other.registered_.begin();
         i != other.registered_.end();
         ++i) {
      const factory_type& the_out_factory = i->second;
      this->registration(i->first, the_out_factory);
    }
    DT_LOG_NOTICE(logging_, "Done.");
  }

  template <typename BaseType>
  void factory_register<BaseType>::import_some(const factory_register & other,
                                               const std::vector<std::string> & imported_factories) {
    DT_LOG_NOTICE(logging_, "Importing registered factories from register '" << other.get_label() << "'...");
    for (typename factory_map_type::const_iterator i = other.registered_.begin();
         i != other.registered_.end();
         ++i) {
      if (std::find(imported_factories.begin(), imported_factories.end(), i->first) != imported_factories.end()) {
        const factory_type& the_out_factory = i->second;
        this->registration(i->first, the_out_factory);
      }
    }
    DT_LOG_NOTICE(logging_, "Done.");
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
        << "Logging threshold : "
        << datatools::logger::get_priority_label(logging_) << std::endl;
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

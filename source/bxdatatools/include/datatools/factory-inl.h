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
    : _label_()
    , _logging_(datatools::logger::PRIO_FATAL)
    ,  _registered_()
  {
    return;
  }

  // Constructor:
  template <typename BaseType>
  factory_register<BaseType>::factory_register(const std::string & label_,
                                               unsigned int flags_)
    : _label_(label_),
      _logging_(datatools::logger::PRIO_FATAL) {
    if (flags_ & verbose) {
      set_logging_priority(datatools::logger::PRIO_INFORMATION);
    }
    DT_LOG_INFORMATION(_logging_,"Construction (label='" << _label_ << "'...");
    DT_LOG_INFORMATION(_logging_,"Construction is done (label='" << _label_ << "'...");
    return;
  }

  // Destructor:
  template <typename BaseType>
  factory_register<BaseType>::~factory_register()
  {
    DT_LOG_INFORMATION(_logging_,"Destruction (label='" << _label_ << "'...");
    this->clear();
    DT_LOG_INFORMATION(_logging_,"Destruction is done (label='" << _label_ << "'...");
    return;
  }

  template <typename BaseType>
  datatools::logger::priority factory_register<BaseType>::get_logging_priority() const
  {
    return _logging_;
  }

  template <typename BaseType>
  void factory_register<BaseType>::set_logging_priority(datatools::logger::priority logging_)
  {
    _logging_ = logging_;
    return;
  }

  template <typename BaseType>
  const std::string & factory_register<BaseType>::get_label() const
  {
    return _label_;
  }

  template <typename BaseType>
  void factory_register<BaseType>::set_label(const std::string & label_)
  {
    _label_ = label_;
    return;
  }

  template <typename BaseType>
  void factory_register<BaseType>::list_of_factories(std::vector<std::string> & ids_) const
  {
    for (typename factory_map_type::const_iterator i = _registered_.begin();
         i != _registered_.end();
         ++i) {
      ids_.push_back(i->first);
    }
    return;
  }

  template <typename BaseType>
  bool factory_register<BaseType>::has(const std::string & id_) const
  {
    return _registered_.find(id_) != _registered_.end();
  }

  template <typename BaseType>
  void factory_register<BaseType>::clear()
  {
    for (typename factory_map_type::iterator i = _registered_.begin();
         i != _registered_.end();
         ++i) {
      if (i->second.fact != nullptr) {
        DT_LOG_INFORMATION(_logging_,"Destroying registered allocator/functor '" << i->first << "'");
      }
    }
    _registered_.clear();
    return;
  }

  template <typename BaseType>
  void factory_register<BaseType>::reset()
  {
    this->clear();
    _label_.clear();
    _logging_ = datatools::logger::PRIO_FATAL;
    return;
  }

  template <typename BaseType>
  typename factory_register<BaseType>::factory_type &
  factory_register<BaseType>::grab(const std::string & id_)
  {
    typename factory_map_type::iterator found = _registered_.find(id_);
    DT_THROW_IF(found == _registered_.end(), std::logic_error,
                "Class ID '" << id_ << "' is not registered !");
    return found->second.fact;
  }

  template <typename BaseType>
  const typename factory_register<BaseType>::factory_type &
  factory_register<BaseType>::get(const std::string & id_) const
  {
    typename factory_map_type::const_iterator found = _registered_.find(id_);
    DT_THROW_IF(found == _registered_.end(), std::logic_error,
                "Class ID '" << id_ << "' is not registered !");
    return found->second.fact;
  }

  template <typename BaseType>
  const typename factory_register<BaseType>::factory_record_type &
  factory_register<BaseType>::get_record(const std::string & id_) const
  {
    typename factory_map_type::const_iterator found = _registered_.find(id_);
    DT_THROW_IF(found == _registered_.end(), std::logic_error,
                "Class ID '" << id_ << "' is not registered !");
    return found->second;
  }
  
  template <typename BaseType>
  bool factory_register<BaseType>::fetch_type_id(const std::type_info & tinfo_, std::string & id_) const
  {
    id_.clear();
    for (typename factory_map_type::const_iterator i = _registered_.begin();
         i != _registered_.end();
         ++i) {
      if (&tinfo_ == i->second.tinfo) {
        id_ = i->first;
        return true;
      }
    }
    return false;
  }                    
 
  template <typename BaseType>
  template<class DerivedType>
  bool factory_register<BaseType>::fetch_type_id(std::string & id_) const
  {
    id_.clear();
    bool fatal = !boost::is_base_of<BaseType, DerivedType>::value;
    DT_THROW_IF(fatal,
                std::logic_error,
                "Class ID '" << id_ << "' is not registrable!");
    // if (fatal) {
    //   return false;
    // }
    for (typename factory_map_type::const_iterator i = _registered_.begin();
         i != _registered_.end();
         ++i) {
      if (&typeid(DerivedType) == i->second.tinfo) {
        id_ = i->first;
        return true;
      }
    }
    return false;
  }

  template <typename BaseType>
  template <typename DerivedType>
  void factory_register<BaseType>::registration(const std::string & id_,
                                                const std::string & description_,
                                                const std::string & category_)
  {
    registration(id_,
                 boost::factory<DerivedType*>(),
                 typeid(DerivedType),
                 description_,
                 category_);
    return;
  }

  template <typename BaseType>
  void factory_register<BaseType>::registration(const std::string & id_,
                                                const factory_type & factory_,
                                                const std::type_info & tinfo_,
                                                const std::string & description_,
                                                const std::string & category_)
  {
    DT_LOG_NOTICE(_logging_, "Registration of class with ID '" << id_ << "'");
    typename factory_map_type::const_iterator found = _registered_.find(id_);
    DT_THROW_IF(found != _registered_.end(), std::logic_error,
                "Class ID '" << id_ << "' is already registered !");
    factory_record_type record;
    record.type_id = id_;
    record.description = description_;
    record.category = category_;
    record.fact = factory_;
    record.tinfo = &tinfo_;
    _registered_[id_] = record;
    return;
  }

  template <typename BaseType>
  void factory_register<BaseType>::unregistration(const std::string & id_)
  {
    DT_LOG_NOTICE(_logging_, "Unregistration of class with ID '" << id_ << "'");
    typename factory_map_type::const_iterator found = _registered_.find(id_);
    DT_THROW_IF(found == _registered_.end(), std::logic_error,
                "Class ID '" << id_ << "' is not registered !");
    _registered_.erase(id_);
    return;
  }

  template <typename BaseType>
  void factory_register<BaseType>::import(const factory_register & other_)
  {
    DT_LOG_NOTICE(_logging_, "Importing registered factories from register '" << other_.get_label() << "'...");
    for (typename factory_map_type::const_iterator i = other_._registered_.begin();
         i != other_._registered_.end();
         ++i) {
      const factory_record_type & the_out_factory_record = i->second;
      this->registration(i->first,
                         the_out_factory_record.fact,
                         *the_out_factory_record.tinfo,
                         the_out_factory_record.description, 
                         the_out_factory_record.category);
    }
    DT_LOG_NOTICE(_logging_, "Done.");
    return;
  }

  template <typename BaseType>
  void factory_register<BaseType>::import_some(const factory_register & other_,
                                               const std::vector<std::string> & imported_factories_)
  {
    DT_LOG_NOTICE(_logging_, "Importing registered factories from register '" << other_.get_label() << "'...");
    for (typename factory_map_type::const_iterator i = other_._registered_.begin();
         i != other_._registered_.end();
         ++i) {
      if (std::find(imported_factories_.begin(),
                    imported_factories_.end(),
                    i->first) != imported_factories_.end()) {
        const factory_record_type & the_out_factory_record = i->second;
        this->registration(i->first,
                           the_out_factory_record.fact,
                           *the_out_factory_record.tinfo,
                           the_out_factory_record.description,
                           the_out_factory_record.category);
      }
    }
    DT_LOG_NOTICE(_logging_, "Done.");
    return;
  }

  template <typename BaseType>
  void factory_register<BaseType>::print(std::ostream & out_,
                                         const std::string & indent_) const
  {
    std::ostringstream title_oss;
    title_oss << "List of registered allocators/functors for label \""
              << _label_
              << "\" : ";
    this->tree_dump(out_, title_oss.str(), indent_);
    return;
  }

  template <typename BaseType>
  void factory_register<BaseType>::tree_dump(std::ostream & out_,
                                             const std::string & title_,
                                             const std::string & indent_,
                                             bool inherit_) const
  {
    if (!title_.empty()) { out_ << indent_ << title_ << std::endl; }

    out_ << indent_ << i_tree_dumpable::tag
        << "Label   : '"
        << _label_ << "'" << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
        << "Logging threshold : "
        << datatools::logger::get_priority_label(_logging_) << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
        << "Registered factories : " << _registered_.size() << std::endl;

    for (typename factory_map_type::const_iterator i = _registered_.begin();
         i != _registered_.end();
         ++i) {
      out_ << indent_;
      out_ << i_tree_dumpable::inherit_skip_tag(inherit_);
      typename factory_map_type::const_iterator j = i;
      j++;
      if (j == _registered_.end()) {
        out_ << i_tree_dumpable::last_tag;
      } else {
        out_ << i_tree_dumpable::tag;
      }
      out_ << "ID: \"" << i->first << "\" @ " << &i->second.fact;
      if (!i->second.description.empty()) {
        out_ << ": " << i->second.description;
      }
      if (!i->second.category.empty()) {
        out_ << " (" << i->second.category << ')';
      }
      out_ << std::endl;
    }
    return;
  }

} // namespace datatools

#endif // DATATOOLS_FACTORY_INL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

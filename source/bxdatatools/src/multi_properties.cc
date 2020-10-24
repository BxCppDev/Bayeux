// multi_properties.cc

// Ourselves:
#include <datatools/multi_properties.h>

// Standard Library:
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <iomanip>

// - Boost:
#include <boost/algorithm/string.hpp>

// This Project:
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/ioutils.h>
#include <datatools/multi_properties_config.h>

// Support for serialization tag :
DATATOOLS_SERIALIZATION_EXT_SERIAL_TAG_IMPLEMENTATION(::datatools::multi_properties,
                                                      "datatools::multi_properties")

// Support for old serialization tag :
DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_IMPLEMENTATION(::datatools::multi_properties,
                                                               "datatools::utils::multi_properties")


namespace datatools {

  DATATOOLS_SERIALIZATION_IMPLEMENTATION_ADVANCED(multi_properties, "datatools::multi_properties")

  const std::string & multi_properties::defaults::key_label()
  {
    static const std::string _label("name");
    return _label;
  }

  const std::string & multi_properties::defaults::meta_label()
  {
    static const std::string _label("type");
    return _label;
  }

  //----------------------------------------------------------------------
  // entry class impl
  //
  multi_properties::entry::entry(const std::string & key_,
                                 const std::string & meta_)
  {
    if (!key_.empty()) this->set_key(key_);
    this->set_meta(meta_);
    return;
  }

  const properties& multi_properties::entry::get_properties() const
  {
    return _properties_;
  }

  properties & multi_properties::entry::grab_properties()
  {
    return _properties_;
  }

  const std::string & multi_properties::entry::get_key() const
  {
    return _key_;
  }

  void multi_properties::entry::set_key(const std::string & key_)
  {
    DT_THROW_IF (key_.empty(),
                 std::logic_error,
                 "Empty key is not allowed !");
    _key_ = key_;
    return;
  }

  const std::string & multi_properties::entry::get_meta() const
  {
    return _meta_;
  }

  void multi_properties::entry::set_meta(const std::string & meta_)
  {
    _meta_ = meta_;
    return;
  }

  bool multi_properties::entry::has_meta() const
  {
    return (!_meta_.empty());
  }

  void multi_properties::entry::print_tree(std::ostream & out_,
                                           const boost::property_tree::ptree & options_) const
  {
    i_tree_dumpable::base_print_options popts;
    popts.configure_from(options_);
    std::ostringstream outs;
    if (! popts.title.empty ()) {
      outs << popts.indent << popts.title << std::endl;
    }

    outs << popts.indent << i_tree_dumpable::tag
          << "Key        : \"" <<  _key_ << "\"" << std::endl;

    outs << popts.indent << i_tree_dumpable::tag
          << "Meta       : \"" <<  _meta_ << "\"" << std::endl;

    {
      outs << popts.indent << i_tree_dumpable::inherit_tag(popts.inherit)
            << "Properties : ";
      if (_properties_.size () == 0) {
        outs << "<empty>";
      } else {
        outs << '[' << _properties_.size() << ']';
      }
      outs << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << popts.indent;
        indent_oss << i_tree_dumpable::inherit_skip_tag(popts.inherit);
        boost::property_tree::ptree options2;
        options2.put(base_print_options::indent_key(), indent_oss.str());
        _properties_.print_tree(outs, options2);
      }
    }

    out_ << outs.str();
    return;
  }

  void multi_properties::entry::tree_dump(std::ostream & out_,
                                          const std::string & title_,
                                          const std::string & indent_,
                                          bool inherit) const
  {
    if (!title_.empty()) out_ << indent_ << title_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
          << "Key        : \"" <<  _key_ << "\"" << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
          << "Meta       : \"" <<  _meta_ << "\"" << std::endl;

    {
      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit)
            << "Properties : ";
      if (_properties_.size () == 0) {
        out_ << "<empty>";
      } else {
        out_ << '[' << _properties_.size() << ']';
      }
      out_ << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent_;
        indent_oss << i_tree_dumpable::inherit_skip_tag(inherit);
        _properties_.tree_dump(out_, "", indent_oss.str());
      }
    }
    return;
  }

  //----------------------------------------------------------------------
  // multiproperties class impl
  //
  DATATOOLS_CLONEABLE_IMPLEMENTATION(multi_properties)

  multi_properties::multi_properties()
  {
    _init_("", "", "");
    return;
  }

  multi_properties::multi_properties(const std::string & key_label_,
                                     const std::string & meta_label_)
  {
    _init_(key_label_,meta_label_,"");
    return;
  }

  multi_properties::multi_properties(const std::string & key_label_,
                                     const std::string & meta_label_,
                                     const std::string & description_)
  {
    _init_(key_label_,meta_label_,description_);
    return;
  }

  void multi_properties::_init_(const std::string & key_label_,
                                const std::string & meta_label_,
                                const std::string & description_)
  {
    if (_key_label_.empty()) _key_label_ = defaults::key_label();
    if (_meta_label_.empty()) _meta_label_ = defaults::meta_label();
    if (!key_label_.empty()) this->set_key_label(key_label_);
    this->set_meta_label(meta_label_);
    this->set_description(description_);
    return;
  }


  multi_properties::multi_properties(const multi_properties & source_)
  {
    _copy_impl_(source_);
    return;
  }

  multi_properties & multi_properties::operator=(const multi_properties & source_)
  {
    if (this != &source_) {
      // protect against invalid self-assignment
      _copy_impl_(source_);
    }
    return *this;
  }

  void multi_properties::_copy_impl_(const multi_properties & source_)
  {
    this->_description_ = source_._description_;
    this->_key_label_   = source_._key_label_;
    this->_meta_label_  = source_._meta_label_;
    this->_entries_     = source_._entries_;

    for (const auto& e : source_._ordered_entries_) {
      auto found = this->_entries_.find(e->get_key());
      this->_ordered_entries_.push_back(&found->second);
    }

    return;
  }

  bool multi_properties::has_description() const
  {
    return !_description_.empty();
  }

  const std::string & multi_properties::get_description() const
  {
    return _description_;
  }

  void multi_properties::set_description(const std::string & description_)
  {
    _description_ = description_;
    return;
  }

  bool multi_properties::has_key_label() const
  {
    return ! _key_label_.empty();
  }

  const std::string & multi_properties::get_key_label() const
  {
    return _key_label_;
  }

  void multi_properties::clear_key_label()
  {
    DT_THROW_IF (this->size(),
                 std::logic_error,
                 "Reseting key label '" << _key_label_ << "' is not allowed in non-empty multi-properties '" << _description_ << "'!");
    _key_label_.clear();
    return;
  }

  void multi_properties::set_key_label(const std::string & key_label_)
  {
    DT_THROW_IF (key_label_.empty(),
                 std::logic_error,
                 "Empty key is not allowed !");
    DT_THROW_IF (this->size(),
                 std::logic_error,
                 "Changing key label from '" << _key_label_ << "' to '" << key_label_ << "' is not allowed in multi-properties '" << _description_ << "'!");
    _key_label_ = key_label_;
    return;
  }

  bool multi_properties::has_meta_label() const
  {
    return ! _meta_label_.empty();
  }

  const std::string & multi_properties::get_meta_label() const
  {
    return _meta_label_;
  }

  void multi_properties::clear_meta_label()
  {
    DT_THROW_IF(this->size(),
                std::logic_error,
                "Reseting meta label '" << _meta_label_ << "' is not allowed in non-empty multi-properties '" << _description_ << "'!");
    _meta_label_.clear();
    return;
  }

  void multi_properties::set_meta_label(const std::string & meta_label_)
  {
    DT_THROW_IF (this->size(),
                 std::logic_error,
                 "Changing meta label from '" << _meta_label_ << "' to '" << meta_label_ << "' is not allowed in multi-properties '" << _description_ << "'!");
    _meta_label_ = meta_label_;
    return;
  }

  size_t multi_properties::size() const
  {
    DT_THROW_IF (_ordered_entries_.size() != _entries_.size(),
                 std::runtime_error,
                 "Internal containers are broken !");
    return _entries_.size();
  }

  bool multi_properties::empty() const
  {
    return this->size() == 0;
  }

  void multi_properties::reset()
  {
    _description_.clear();
    _key_label_.clear();
    _meta_label_.clear();
    this->clear();
    return;
  }

  void multi_properties::clear()
  {
    _ordered_entries_.clear();
    _entries_.clear();
    return;
  }


  bool multi_properties::has_key(const std::string & key_) const
  {
    auto found = _entries_.find(key_);
    return found != _entries_.end();
  }

  bool multi_properties::has_key_with_meta(const std::string & key_, const std::string & a_meta) const
  {
    auto found = _entries_.find(key_);
    if (found == _entries_.end()) return false;
    return found->second.get_meta() == a_meta;
  }

  const std::string & multi_properties::key (size_t key_index_) const
  {
    DT_THROW_IF(key_index_ >= size(),
                std::out_of_range,
                "Out of range key index '"
                << key_index_ << "' !");
    auto iter = _entries_.begin();
    std::advance(iter, key_index_);
    return iter->first;
  }

  void multi_properties::keys(std::vector<std::string>&k) const
  {
    for (const auto & e : _entries_) {
      k.push_back(e.first);
    }
    return;
  }

  std::vector<std::string> multi_properties::keys () const
  {
    std::vector<std::string> k;
    keys(k);
    return k;
  }

  const std::string & multi_properties::ordered_key (size_t key_index_) const
  {
    DT_THROW_IF(key_index_ >= size(),
                std::out_of_range,
                "Out of range ordered key index '"
                << key_index_ << "' !");
    auto iter = _ordered_entries_.begin();
    std::advance(iter, key_index_);
    return (*iter)->get_key ();
  }

  std::vector<std::string> multi_properties::ordered_keys() const
  {
    std::vector<std::string> k;
    ordered_keys(k);
    return k;
  }

  void multi_properties::ordered_keys(std::vector<std::string> & keys_) const
  {
    for (const entry * e : _ordered_entries_) {
      keys_.push_back(e->get_key());
    }
    return;
  }


  bool multi_properties::has_section(const std::string & key_) const
  {
    return this->has_key(key_);
  }

  const properties& multi_properties::get_section(const std::string & key_) const
  {
    return this->get(key_).get_properties();
  }

  properties& multi_properties::grab_section(const std::string & key_)
  {
    return this->grab(key_).grab_properties();
  }


  const multi_properties::entry& multi_properties::get(const std::string & key_) const
  {
    auto found = _entries_.find(key_);
    DT_THROW_IF (found == _entries_.end(),
                 std::logic_error,
                 "Key '" << key_ << "' is not found !");
    return found->second;
  }

  multi_properties::entry& multi_properties::grab(const std::string & key_)
  {
    auto found = _entries_.find(key_);
    DT_THROW_IF (found == _entries_.end(),
                 std::logic_error,
                 "Key '" << key_ << "' is not found !");
    return found->second;
  }

  const multi_properties::entries_col_type & multi_properties::entries() const
  {
    return _entries_;
  }

  const multi_properties::entries_ordered_col_type &
  multi_properties::ordered_entries() const
  {
    return _ordered_entries_;
  }

  void multi_properties::add(const std::string & key_,
                             const std::string & meta_)
  {
    this->add_impl2(key_, meta_);
  }

  void multi_properties::add(const std::string & key_,
                             const properties & a_props)
  {
    this->add_impl2(key_) = a_props;
    return;
  }

  void multi_properties::add(const std::string & key_,
                             const std::string & meta_,
                             const properties & props_)
  {
    this->add_impl2(key_, meta_) = props_;
    return;
  }

  properties& multi_properties::add_section(const std::string & key_,
                                            const std::string & meta_)
  {
    return add_impl2(key_, meta_);
  }

  properties& multi_properties::add_impl2(const std::string & key_,
                                          const std::string & meta_)
  {
    DT_THROW_IF (this->has_key(key_),
                 std::logic_error,
                 "Key '" << key_ << "' is already used !");
    // 2017-03-17, FM: I don't know what to do with the following lines!
    // So wrap it only in debug mode...
    if (_meta_label_.empty() && !meta_.empty()) {
        DT_LOG_DEBUG(datatools::logger::PRIO_WARNING,
                      "Key '" << key_ << "' will ignore meta '" << meta_ << "' !");
    }

    _entries_[key_] = entry(key_, meta_);
    _ordered_entries_.push_back(&_entries_[key_]);
    return _entries_[key_].grab_properties ();
  }

  void multi_properties::remove(const std::string & key_)
  {
    DT_THROW_IF (! this->has_key(key_),
                 std::logic_error,
                 "Key '" << key_ << "' is not used !");
    _ordered_entries_.remove_if([&key_](entry* e) {return (e->get_key() == key_);});
    _entries_.erase(key_);
    return;
  }

  void multi_properties::write(const std::string & filename_,
                               uint32_t options_) const
  {
    multi_properties_config w(options_);
    w.write(filename_, *this);
    return;
  }

  void multi_properties::read(const std::string & filename_,
                              uint32_t options_)
  {
    multi_properties_config r(options_);
    r.read(filename_, *this);
    return;
  }


  void multi_properties::merge_with(const multi_properties & other_,
                                    bool allow_override_sections_,
                                    bool allow_override_props_)
  {
    DT_THROW_IF(this->_key_label_ != other_._key_label_,
                std::logic_error,
                "Unmatching key label '" << other_._key_label_ << "' vs '" << this->_key_label_ << "'!");
    DT_THROW_IF(this->_meta_label_ != other_._meta_label_,
                std::logic_error,
                "Unmatching meta label '" << other_._meta_label_ << "' vs '" << this->_meta_label_ << "'!");
    std::vector<std::string> okeys;
    other_.ordered_keys(okeys);
    for (const std::string & okey : okeys) {
      const entry & oentry = other_.get(okey);
      if (!this->has_key(okey)) {
        this->add(okey, oentry.get_meta(), oentry.get_properties());
      } else {
        DT_THROW_IF(!allow_override_sections_, std::logic_error,
                    "Overriding section '" << okey << "' is not allowed!");
        entry & tentry = this->grab(okey);
        DT_THROW_IF(tentry.get_meta() != oentry.get_meta(),
                    std::logic_error,
                    "Incompatible meta for section '" << okey << "'!");
        const properties & oprop = oentry.get_properties();
        properties & tprop = tentry.grab_properties();
        tprop.merge_with(oprop, allow_override_props_);
      }
    }
    return;
  }

  void multi_properties::dump(std::ostream & out_) const
  {
    this->tree_dump(out_, "datatools::multi_properties:");
    return;
  }

  void multi_properties::print_tree(std::ostream & out_,
                                    const boost::property_tree::ptree & options_) const
  {
    i_tree_dumpable::base_print_options popts;
    popts.configure_from(options_);
    std::ostringstream outs;
    if (! popts.title.empty ()) {
      outs << popts.indent << popts.title << std::endl;
    }

    if (!_description_.empty()) {
      outs << popts.indent << i_tree_dumpable::tag
            << "Description  : " <<  this->get_description() << std::endl;
    }

    if (!_key_label_.empty()) {
      outs << popts.indent << i_tree_dumpable::tag
            << "Key label    : \""
            << this->get_key_label()
            << "\"" << std::endl;
    }

    if (!_meta_label_.empty()) {
      outs << popts.indent << i_tree_dumpable::tag
            << "Meta label   : \"" << this->get_meta_label()
            << "\"" << std::endl;
    }

    {
      outs << popts.indent << i_tree_dumpable::tag
            << "Entries      : ";
      if (_entries_.size() == 0) {
        outs << "<empty>";
      } else {
        outs << "[" << _entries_.size() << "]";
      }

      outs << std::endl;
      for (auto i = _ordered_entries_.begin();
           i != _ordered_entries_.end();
           ++i) {
        const entry& a_entry = **i;
        const std::string & local_key = a_entry.get_key();
        outs << popts.indent;
        outs << i_tree_dumpable::skip_tag;

        std::ostringstream indent_oss;
        indent_oss << popts.indent;
        indent_oss << i_tree_dumpable::skip_tag;

        auto j = i;

        if (++j == _ordered_entries_.end()) {
          outs << i_tree_dumpable::last_tag;
          indent_oss << i_tree_dumpable::last_skip_tag;
        } else {
          outs << i_tree_dumpable::tag;
          indent_oss << i_tree_dumpable::skip_tag;
        }
        outs << "Entry : " << '"' << local_key << '"';
        if (properties::key_is_private(local_key)) outs << " [private]";
        outs << std::endl;
        boost::property_tree::ptree options2;
        options2.put(base_print_options::indent_key(), indent_oss.str());
        a_entry.print_tree(outs, options2);
      }
    }

    {
      int rank = 0;
      outs << popts.indent << i_tree_dumpable::inherit_tag(popts.inherit)
            << "Ordered entries      : ";
      if (_ordered_entries_.size() == 0) {
        outs << "<empty>";
      } else {
        outs << "[" << _ordered_entries_.size() << "]";
      }
      outs << std::endl;

      for (auto i = _ordered_entries_.begin();
           i != _ordered_entries_.end();
           ++i) {
        const entry *p_entry = *i;
        outs << popts.indent;
        outs << i_tree_dumpable::inherit_skip_tag(popts.inherit);

        std::ostringstream indent_oss;
        indent_oss << popts.indent;
        indent_oss << i_tree_dumpable::inherit_skip_tag(popts.inherit);

        auto j = i;
        j++;

        if (j == _ordered_entries_.end()) {
          outs << i_tree_dumpable::last_tag;
          indent_oss << i_tree_dumpable::inherit_skip_tag(popts.inherit);
        } else {
          outs << i_tree_dumpable::tag;
          indent_oss << i_tree_dumpable::skip_tag;
        }

        const std::string & local_key = p_entry->get_key();
        outs << "Entry [rank=" << rank << "] : " << '"' << local_key << '"';

        if (properties::key_is_private(local_key)) outs << " [private]";

        outs << std::endl;
        rank++;
      }
    }

    out_ << outs.str();
    return;
  }

  void multi_properties::tree_dump(std::ostream & out_,
                                   const std::string & title_,
                                   const std::string & indent_,
                                   bool inherit_) const
  {
   if (!title_.empty()) out_ << indent_ << title_ << std::endl;

    if (!_description_.empty()) {
      out_ << indent_ << i_tree_dumpable::tag
            << "Description  : " <<  this->get_description() << std::endl;
    }

    if (!_key_label_.empty()) {
      out_ << indent_ << i_tree_dumpable::tag
            << "Key label    : \""
            << this->get_key_label()
            << "\"" << std::endl;
    }

    if (!_meta_label_.empty()) {
      out_ << indent_ << i_tree_dumpable::tag
            << "Meta label   : \"" << this->get_meta_label()
            << "\"" << std::endl;
    }

    {
      out_ << indent_ << i_tree_dumpable::tag
            << "Entries      : ";
      if (_entries_.size() == 0) {
        out_ << "<empty>";
      } else {
        out_ << "[" << _entries_.size() << "]";
      }

      out_ << std::endl;
      for (auto i = _ordered_entries_.begin();
           i != _ordered_entries_.end();
           ++i) {
        const entry& a_entry = **i;
        const std::string & local_key = a_entry.get_key();
        out_ << indent_;
        out_ << i_tree_dumpable::skip_tag;

        std::ostringstream indent_oss;
        indent_oss << indent_;
        indent_oss << i_tree_dumpable::skip_tag;

        auto j = i;

        if (++j == _ordered_entries_.end()) {
          out_ << i_tree_dumpable::last_tag;
          indent_oss << i_tree_dumpable::last_skip_tag;
        } else {
          out_ << i_tree_dumpable::tag;
          indent_oss << i_tree_dumpable::skip_tag;
        }
        out_ << "Entry : " << '"' << local_key << '"';

        if (properties::key_is_private(local_key)) out_ << " [private]";

        out_ << std::endl;
        a_entry.tree_dump(out_, "", indent_oss.str());
      }
    }

    {
      int rank = 0;
      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
            << "Ordered entries      : ";
      if (_ordered_entries_.size() == 0) {
        out_ << "<empty>";
      } else {
        out_ << "[" << _ordered_entries_.size() << "]";
      }
      out_ << std::endl;

      for (auto i = _ordered_entries_.begin();
           i != _ordered_entries_.end();
           ++i) {
        const entry *p_entry = *i;
        out_ << indent_;
        out_ << i_tree_dumpable::inherit_skip_tag(inherit_);

        std::ostringstream indent_oss;
        indent_oss << indent_;
        indent_oss << i_tree_dumpable::inherit_skip_tag(inherit_);

        auto j = i;
        j++;

        if (j == _ordered_entries_.end()) {
          out_ << i_tree_dumpable::last_tag;
          indent_oss << i_tree_dumpable::inherit_skip_tag(inherit_);
        } else {
          out_ << i_tree_dumpable::tag;
          indent_oss << i_tree_dumpable::skip_tag;
        }

        const std::string & local_key = p_entry->get_key();
        out_ << "Entry [rank=" << rank << "] : " << '"' << local_key << '"';

        if (properties::key_is_private(local_key)) out_ << " [private]";

        out_ << std::endl;
        rank++;
      }
    }
    return;
  }

} // end of namespace datatools

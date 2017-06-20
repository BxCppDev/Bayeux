// datatools/urn_info.cc
//
// Copyright (c) 2017 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of datatools.
//
// datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools. If not, see <http://www.gnu.org/licenses/>.

// Standard library:
#include <sstream>

// Ourselves:
#include <datatools/urn_info.h>

// This project:
#include <datatools/exception.h>
#include <datatools/utils.h>
#include <datatools/urn.h>

namespace datatools {

  DATATOOLS_SERIALIZATION_IMPLEMENTATION_ADVANCED(urn_info, "datatools::urn_info")

  // static
  bool urn_info::validate_urn(const std::string & urn_)
  {
    {
      urn u;
      if (! u.from_string(urn_) || ! u.is_valid()) return false;
    }
    return true;
  }

  // static
  bool urn_info::validate_category(const std::string & category_)
  {
    static const uint32_t nv_flags = datatools::NV_MODEL;
    if (!datatools::name_validation(category_, nv_flags)) return false;
    return true;
  }

  // static
  bool urn_info::validate_topic(const std::string & topic_)
  {
    static const uint32_t nv_flags = datatools::NV_MODEL;
    if (!datatools::name_validation(topic_, nv_flags)) return false;
    return true;
  }

  // static
  const std::string & urn_info::anonymous_topic()
  {
    static const std::string _ac("__anonymous__");
    return _ac;
  }

  urn_info::urn_info()
  {
    return;
  }

  urn_info::urn_info(const std::string & urn_,
                     const std::string & category_,
                     const std::string & description_)
  {
    set_urn(urn_);
    if (!category_.empty()) {
      set_category(category_);
    }
    if (!description_.empty()) {
      set_description(description_);
    }
    return;
  }

  urn_info::~urn_info()
  {
    return;
  }

  bool urn_info::is_valid() const
  {
    return !_urn_.empty();
  }

  void urn_info::lock()
  {
    _locked_ = true;
    return;
  }

  void urn_info::unlock()
  {
    _locked_ = false;
    return;
  }

  bool urn_info::is_locked() const
  {
    return _locked_;
  }

  void urn_info::initialize(const properties & config_)
  {
    if (_urn_.empty()) {
      if (config_.has_key("urn")) {
        const std::string & u = config_.fetch_string("urn");
        set_urn(u);
      }
    }

    if (_category_.empty()) {
      if (config_.has_key("category")) {
        const std::string & c = config_.fetch_string("category");
        set_category(c);
      }
    }

    if (config_.has_key("description")) {
      const std::string & d = config_.fetch_string("description");
      set_description(d);
    }

    if (config_.has_key("components")) {
      std::vector<std::string> anonymous_components;
      config_.fetch("components", anonymous_components);
      for (std::size_t i = 0; i < anonymous_components.size(); i++) {
        add_component(anonymous_components[i]);
      }
    }

    std::vector<std::string> topics;
    if (config_.has_key("topics")) {
      config_.fetch("topics", topics);
      for (std::size_t i = 0; i < topics.size(); i++) {

        bool single_component_topic = false;
        {
          std::ostringstream oss;
          oss << "topic." << topics[i] << ".component";
          std::string topic_comp_key = oss.str();
          if (config_.has_key(topic_comp_key)) {
            const std::string & topic_component = config_.fetch_string(topic_comp_key);
            add_component(topic_component, topics[i]);
            single_component_topic = true;
          }
        }

        if (!single_component_topic) {
          std::ostringstream oss;
          oss << "topic." << topics[i] << ".components";
          std::string topic_comps_key = oss.str();
          std::vector<std::string> topic_components;
          DT_THROW_IF(!config_.has_key(topic_comps_key),
                      std::logic_error,
                      "Missing '" << topic_comps_key << "' key!");
          config_.fetch(topic_comps_key, topic_components);
          for (std::size_t j = 0; j < topic_components.size(); j++) {
            add_component(topic_components[j], topics[i]);
          }
        }

      }
    }

    if (config_.has_flag("lock")) {
      lock();
    }

    return;
  }

  void urn_info::reset()
  {
    if (is_locked()) {
      unlock();
    }
    remove_all_components();
    _description_.clear();
    _category_.clear();
    _urn_.clear();
    return;
  }

  void urn_info::set_urn(const std::string & urn_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "URN record is locked!");
    DT_THROW_IF(! validate_urn(urn_),
                std::logic_error,
                "Invalid URN representation '" << urn_ << "'!");
    _urn_ = urn_;
    return;
  }

  const std::string & urn_info::get_urn() const
  {
    return _urn_;
  }

  bool urn_info::has_category() const
  {
    return !_category_.empty();
  }

  void urn_info::set_category(const std::string & category_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "URN record is locked!");
    DT_THROW_IF(!validate_category(category_),
                std::logic_error,
                "Invalid category '" << category_ << "'!");
    _category_ = category_;
    return;
  }

  const std::string & urn_info::get_category() const
  {
    return _category_;
  }

  void urn_info::set_description(const std::string & description_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "URN record is locked!");
    _description_ = description_;
    return;
  }

  bool urn_info::has_description() const
  {
    return !_description_.empty();
  }

  const std::string & urn_info::get_description() const
  {
    return _description_;
  }

  void urn_info::remove_all_components()
  {
    DT_THROW_IF(is_locked(), std::logic_error, "URN record is locked!");
    _components_.clear();
    return;
  }

  bool urn_info::has_component(const std::string & comp_urn_) const
  {
    for (component_topic_dict_type::const_iterator itopic = _components_.begin();
         itopic != _components_.end();
         itopic++) {
      if (std::find(itopic->second.begin(), itopic->second.end(), comp_urn_)
          != itopic->second.end()) return true;
    }
    return false;
  }

  std::size_t urn_info::get_number_of_components() const
  {
    std::size_t noc = 0;
    for (component_topic_dict_type::const_iterator itopic = _components_.begin();
         itopic != _components_.end();
         itopic++) {
      noc += itopic->second.size();
    }
    return noc;
  }

  bool urn_info::has_topic(const std::string & comp_topic_) const
  {
    std::string topic = comp_topic_;
    if (topic.empty()) {
      topic = anonymous_topic();
    }
    return _components_.count(topic) == 1;
  }

  const std::vector<std::string> &
  urn_info::get_components_by_topic(const std::string & comp_topic_) const
  {
    std::string topic = comp_topic_;
    if (topic.empty()) {
      topic = anonymous_topic();
    }
    component_topic_dict_type::const_iterator found = _components_.find(topic);
    DT_THROW_IF(found == _components_.end(), std::logic_error,
                "No component topic with name '" << topic << "'!");
    return found->second;
  }

  std::string urn_info::get_component(const std::string & comp_topic_, int rank_) const
  {
    const std::vector<std::string> & comps = get_components_by_topic(comp_topic_);
    DT_THROW_IF(rank_ < 0 || rank_ >= (int) comps.size(),
                std::range_error,
                "Invalid component rank=[" << rank_ << "] for topic='" << comp_topic_ << "'!");
    return comps[rank_];
  }

  std::size_t urn_info::get_number_of_components_by_topic(const std::string & comp_topic_) const
  {
    return get_components_by_topic(comp_topic_).size();
  }

  bool urn_info::has_component_in_topic(const std::string & comp_urn_,
                                        const std::string & comp_topic_) const
  {
    std::string topic = comp_topic_;
    if (topic.empty()) {
      topic = anonymous_topic();
    }
    for (component_topic_dict_type::const_iterator itopic = _components_.begin();
         itopic != _components_.end();
         itopic++) {
      if (itopic->first != topic) continue;
      if (std::find(itopic->second.begin(), itopic->second.end(), comp_urn_) != itopic->second.end()) return true;
    }
    return false;
  }

  void urn_info::add_component(const std::string & comp_urn_,
                               const std::string & comp_topic_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "URN record is locked!");
    DT_THROW_IF(comp_urn_ == _urn_,
                std::logic_error,
                "URN '" << _urn_ << "' cannot be its own component!");
    DT_THROW_IF(! validate_urn(comp_urn_),
                std::logic_error,
                "Invalid component's URN representation '" << comp_urn_ << "'!");
    DT_THROW_IF(has_component(comp_urn_), std::logic_error,
                "A component with URN '" << comp_urn_ << "' is already set!");
    std::string topic = comp_topic_;
    if (topic.empty()) {
      topic = anonymous_topic();
    } else {
      DT_THROW_IF(!validate_topic(topic), std::logic_error,
                  "Invalid component's topic name '" << topic << "'!");
    }
    component_topic_dict_type::iterator found_topic = _components_.find(topic);
    if (found_topic == _components_.end()) {
      std::vector<std::string> only_urn;
      only_urn.push_back(comp_urn_);
      _components_.emplace(topic, only_urn);
    } else {
      found_topic->second.push_back(comp_urn_);
    }
    return;
  }

  void urn_info::remove_component(const std::string & comp_urn_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "URN record is locked!");
    DT_THROW_IF(!has_component(comp_urn_), std::logic_error,
                "No component with URN '" << comp_urn_ << "' is set!");
    component_topic_dict_type::iterator tag_topic = _components_.end();
    for (component_topic_dict_type::iterator itopic = _components_.begin();
         itopic != _components_.end();
         itopic++) {
      std::vector<std::string>::iterator found_urn
        = std::find(itopic->second.begin(), itopic->second.end(), comp_urn_);
      if (found_urn != itopic->second.end()) {
        itopic->second.erase(found_urn);
        tag_topic = itopic;
        break;
      }
    }
    if (tag_topic != _components_.end() && tag_topic->second.size() == 0) {
      _components_.erase(tag_topic);
    }
    return;
  }

  bool urn_info::has_components() const
  {
    return get_number_of_components() > 0;
  }

  std::vector<std::string> urn_info::get_topics() const
  {
    std::vector<std::string> topics;
    build_topics(topics);
    return topics;
  }

  void urn_info::build_topics(std::vector<std::string> & topics_) const
  {
    topics_.clear();
    topics_.reserve(_components_.size());
    for (component_topic_dict_type::const_iterator itopic = _components_.begin();
         itopic != _components_.end();
         itopic++) {
      if (itopic->second.size()) {
        topics_.push_back(itopic->first);
      }
    }
    return;
  }

  void urn_info::tree_dump(std::ostream& out_,
                           const std::string& title_,
                           const std::string& indent_,
                           bool inherit_) const
  {
    if (! title_.empty()) out_ << indent_ << title_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "URN : '" << _urn_ << "'" << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Category : '" << _category_ << "'" << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Description : ";
    if (!has_description()) {
      out_ << "<none>";
    } else {
      out_ << _description_;
    }
    out_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Components : [" << get_number_of_components() << ']' << std::endl;
    for (component_topic_dict_type::const_iterator itopic = _components_.begin();
         itopic != _components_.end();
         itopic++) {
      component_topic_dict_type::const_iterator jtopic = itopic;
      jtopic++;
      out_ << indent_ << i_tree_dumpable::skip_tag;
      bool last_topic = false;
      if (jtopic == _components_.end()) {
        out_ << i_tree_dumpable::last_tag;
        last_topic = true;
      } else {
        out_ << i_tree_dumpable::tag;
      }
      out_ << "Topic '" << itopic->first << "' : [" << itopic->second.size() << "]" << std::endl;
      for (std::size_t i = 0; i < itopic->second.size(); i++) {
        out_ << indent_ << i_tree_dumpable::skip_tag;
        if (last_topic) {
          out_ << i_tree_dumpable::last_skip_tag;
        } else {
          out_ << i_tree_dumpable::skip_tag;
        }
        if ((i + 1) == itopic->second.size()) {
          out_ << i_tree_dumpable::last_tag;
        } else {
          out_ << i_tree_dumpable::tag;
        }
        out_ << "Component : '" << itopic->second[i] << "'" << std::endl;
      }
    }

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Locked : " << std::boolalpha << is_locked() << std::endl;

    return;
  }

  bool urn_info::operator<(const urn_info & other_) const
  {
    return this->_urn_ < other_._urn_;
  }

} // namespace datatools

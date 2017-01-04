// datatools/configuration/variant_dependency.cc
/*
 * Copyright (C) 2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

// Ourselves:
#include <datatools/configuration/variant_dependency.h>

// This package:
#include <datatools/configuration/variant_repository.h>
#include <datatools/configuration/variant_registry.h>
#include <datatools/configuration/variant_object_info.h>
#include <datatools/configuration/parsers.h>
#include <datatools/configuration/variant_dependency_logic.h>
#include <datatools/configuration/variant_dependency_logic_parsing.h>
#include <datatools/configuration/variant_dependency_logic_builder.h>
#include <datatools/configuration/variant_record.h>
#include <datatools/configuration/parameter_model.h>

namespace datatools {

  namespace configuration {

    variant_dependency::variant_dependency(const variant_repository & repository_)
    {
      _registry_ = nullptr;
      _repository_ = &repository_;
      return;
    }

    variant_dependency::variant_dependency(const variant_registry & registry_)
    {
      _registry_ = &registry_;
      _repository_ = nullptr;
      if (_registry_->has_parent_repository()) {
        _repository_ = &_registry_->get_parent_repository();
      }
      return;
    }

    variant_dependency::~variant_dependency()
    {
      return;
    }

    bool variant_dependency::has_repository() const
    {
      return _repository_ != nullptr;
    }

    const variant_repository & variant_dependency::get_repository() const
    {
      return *_repository_;
    }

    bool variant_dependency::has_registry() const
    {
      return _registry_ != nullptr;
    }

    const variant_registry & variant_dependency::get_registry() const
    {
      DT_THROW_IF(!has_registry(), std::logic_error, "No registry is set!");
      return *_registry_;
    }

    bool variant_dependency::has_global_scope() const
    {
      return !is_local();
    }

    bool variant_dependency::has_local_scope() const
    {
      return is_local();
    }

    bool variant_dependency::is_local() const
    {
      return has_registry();
    }

    bool variant_dependency::is_valid() const
    {
      if (!_depender_.is_valid()) return false;
      for (const auto & dependee: _dependees_) {
        if (!dependee.second.is_valid()) return false;
      }
      if (_logic_.get() == nullptr) return false;
      return true;
    }

    void variant_dependency::set_depender(const variant_object_info & depender_)
    {
      DT_THROW_IF(!depender_.is_valid(), std::logic_error, "Depender is not valid!");
      if (depender_.is_parameter()) {
        DT_THROW(std::logic_error,
                 "Variant parameter '" << depender_ << "' cannot depend on some variant!");
      }
      if (has_local_scope()) {
        DT_THROW_IF(!depender_.is_local(), std::logic_error, "Depender '" << depender_ << "' is not local!");
        if (depender_.is_variant()) {
          DT_LOG_DEBUG(get_logging(), "Local depender path '" << depender_.get_variant_local_path() << "'");
          _registry_->tree_dump(std::cerr, "Registry :", "[debug] ");

          DT_THROW_IF(!_registry_->has_variant_record(depender_.get_variant_local_path()), std::logic_error,
                      "Variant registry '" << _registry_->get_name()
                      << "' has no variant named '" << depender_.get_variant_local_path() << "'!");
        } else if (depender_.is_parameter_value_group()) {
          DT_THROW_IF(!_registry_->has_parameter_record(depender_.get_parameter_local_path()), std::logic_error,
                      "Variant registry '" << _registry_->get_name()
                      << "' has no variant parameter named '" << depender_.get_parameter_local_path() << "'!");
          const variant_record & parrec = _registry_->get_parameter_record(depender_.get_parameter_local_path());
          DT_THROW_IF(!parrec.get_parameter_model().has_group(depender_.get_parameter_value_group_name()),
                      std::logic_error,
                      "Unknown group '" << depender_.get_parameter_value_group_name()
                      << "' in model of parameter named '"
                      << depender_.get_parameter_local_path() << "' for variant registry '"
                      << _registry_->get_name() << "'!");
        }

      } else if (has_global_scope()) {
        DT_THROW_IF(!depender_.is_global(), std::logic_error, "Depender '" << depender_ << "' is not global!");
        if (depender_.is_registry()) {
          DT_THROW_IF(!_repository_->has_registry(depender_.get_registry_name()), std::logic_error,
                      "Variant repository has no registry named '" << depender_.get_registry_name() << "'!");
        } else {
          if (depender_.is_variant()) {
            DT_THROW_IF(!_repository_->has_variant(depender_.get_registry_name(),
                                                   depender_.get_variant_local_path()), std::logic_error,
                        "Variant repository has no variant named '" << depender_.get_variant_local_path() << "' "
                        << "in registry '" << depender_.get_registry_name() << "'!");
          } else if (depender_.is_parameter_value_group()) {
            DT_THROW_IF(!_repository_->has_variant_parameter(depender_.get_registry_name(),
                                                             depender_.get_parameter_local_path()), std::logic_error,
                        "Variant repository has no variant parameter named '" << depender_.get_parameter_local_path() << "' "
                        << "in registry '" << depender_.get_registry_name() << "'!");
            const variant_registry & vreg = _repository_->get_registry(depender_.get_registry_name());
            const variant_record & parrec = vreg.get_parameter_record(depender_.get_parameter_local_path());
            DT_THROW_IF(!parrec.get_parameter_model().has_group(depender_.get_parameter_value_group_name()),
                        std::logic_error,
                        "Unknown group '" << depender_.get_parameter_value_group_name()
                        << "' in model of parameter named '"
                        << depender_.get_parameter_local_path() << "' for variant registry '"
                        << depender_.get_registry_name() << "'!");
          }
        }
      }
      _depender_ = depender_;
      return;
    }

    void variant_dependency::set_depender_by_path(const std::string & depender_path_, bool registry_scope_)
    {
      uint32_t parsing_flags = 0;
      if (registry_scope_) {
        parsing_flags |= variant_object_info::PARSE_NO_GLOBAL;
      } else {
        parsing_flags |= variant_object_info::PARSE_NO_LOCAL;
      }
      variant_object_info voinfo;
      DT_THROW_IF(!voinfo.parse_from_string(depender_path_, parsing_flags),
                  std::logic_error,
                  "Invalid depender identifier format (='" << depender_path_ << "'!");
      set_depender(voinfo);
      return;
    }

    const variant_object_info & variant_dependency::get_depender() const
    {
      return _depender_;
    }

    std::size_t variant_dependency::get_number_of_dependees() const
    {
      return _dependees_.size();
    }

    unsigned int variant_dependency::add_dependee(const variant_object_info & dependee_,
                                                  const unsigned int slot_)
    {
      DT_THROW_IF(!dependee_.is_valid(), std::logic_error, "Dependee requested at slot=[" << slot_ << "] is not valid!");
      if (has_local_scope()) {
        DT_THROW_IF(!dependee_.is_local(), std::logic_error, "Dependee '" << dependee_ << "' is not local!");
      } else if (has_global_scope()) {
        DT_THROW_IF(!dependee_.is_global(), std::logic_error, "Dependee '" << dependee_ << "' is not global!");
      }
      if (!dependee_.is_variant()) {
        DT_THROW(std::logic_error,
                 "Dependee '" << dependee_ << "' requested at slot=[" << slot_ << "] is not a variant!");
      }
      unsigned int slot = _dependees_.size();
      if (slot_ != std::numeric_limits<unsigned int>::max()) {
        slot = slot_;
      }
      _dependees_[slot] = dependee_;
      return slot;
    }

    unsigned int variant_dependency::add_dependee_by_path(const std::string & dependee_path_,
                                                          const unsigned int slot_)
    {
      variant_object_info voinfo;
      uint32_t parsing_flags = 0;
      DT_THROW_IF(!voinfo.parse_from_string(dependee_path_, parsing_flags),
                  std::logic_error,
                  "Invalid dependee identifier format!");
      return add_dependee(voinfo, slot_);
    }

    void variant_dependency::build_list_of_dependee_slots(std::set<unsigned int> & slots_) const
    {
      slots_.clear();
      for (const auto & dependee_pair : _dependees_) {
        slots_.insert(dependee_pair.first);
      }
      return;
    }

    bool variant_dependency::has_dependee(unsigned int slot_) const
    {
      return _dependees_.count(slot_) != 0;
    }

    bool variant_dependency::has_dependee_by_path(const std::string & dependee_path_) const
    {
      for (const auto & voi_pair : _dependees_) {
        const auto & voi = voi_pair.second;
        if (voi.match_path(dependee_path_)) {
          return true;
        }
      }
      return false;
    }

    const variant_object_info &
    variant_dependency::get_dependee(const unsigned int slot_) const
    {
      // std::cerr << "=== DEVEL: variant_dependency::get_dependee: " << "Entering..." << std::endl;
      // for (const auto & dependee_pair : _dependees_) {
      //   std::cerr << "===        Dependee slot = [" << dependee_pair.first << "]" << std::endl;
      // }
      std::map<unsigned int, variant_object_info>::const_iterator found = _dependees_.find(slot_);
      DT_THROW_IF(found == _dependees_.end(), std::domain_error, "Invalid dependee slot [" << slot_ << "]!");
      return found->second;
    }

    base_dependency_logic &
    variant_dependency::create_logic(const std::string & guid_)
    {
      DT_LOG_DEBUG(_logging_, "Creating logic of type '" << guid_ << "'...");
      dependency_logic_factory_dict_type & facts = dependency_logic_factories();
      DT_THROW_IF(!facts.count(guid_), std::logic_error, "No logic with GUID='" << guid_ << "'!");
      dependency_logic_factory_type fact = facts.find(guid_)->second;
      _logic_.reset(fact(*this));
      return *_logic_.get();
    }

    const base_dependency_logic & variant_dependency::get_logic() const
    {
      return *_logic_.get();
    }

    base_dependency_logic & variant_dependency::grab_logic()
    {
      return *_logic_.get();
    }

    void variant_dependency::build_logic_from_formula(const std::string & logic_expression_)
    {
      datatools::configuration::parsing::dependency_logic_ast ast;
      bool success = ast.parse(logic_expression_);
      DT_THROW_IF(!success, std::logic_error,
                  "Invalid dependency logic formula '" << logic_expression_ << "'!");
      variant_dependency_logic_builder builder(ast, *this);
      dependency_logic_ptr_type log_ptr = builder.build();
      _logic_ = log_ptr;
      // Note: 2016-11-02 FM: one could use :
      //   builder.build();
      // with the logic internal pointer set from within the  builder::build method...
      //
      return;
    }

    bool variant_dependency::is_locked() const
    {
      return _locked_;
    }

    void variant_dependency::lock()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      if (! _locked_) {
        DT_LOG_DEBUG(_logging_, "Locking...");
        _lock();
        _locked_ = true;
      }
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    datatools::logger::priority variant_dependency::get_logging() const
    {
      return _logging_;
    }

    void variant_dependency::set_logging(const datatools::logger::priority p_)
    {
      _logging_ = p_;
      return;
    }

    void variant_dependency::_lock()
    {
      _check();
      return;
    }

    void variant_dependency::_check()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      DT_LOG_DEBUG(_logging_, "Checking...");
      DT_THROW_IF(!_depender_.is_valid(),
                  std::logic_error,
                  "Invalid variant depender!");
      DT_THROW_IF(_dependees_.size() == 0,
                  std::logic_error,
                  "No dependees!");
      if (_logic_.get() == nullptr) {
        DT_LOG_DEBUG(_logging_, "Installing a default logic...");
        _install_default_logic();
      }
      DT_THROW_IF(!_logic_->is_valid(),
                  std::logic_error,
                  "Embedded logic is not valid!");
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void variant_dependency::_install_default_logic()
    {
      auto & top_logic = this->create_logic("and");
      unsigned int port = 0;
      if (_dependees_.size() > 1) {
        // If there are several dependees, we force an AND operator:
        // all dependees must be fulfilled to enable the dependency:
        //                       +-------+      +----------+
        //                    ,--o Slot  |<==[0]s Dependee | connected
        // +-------+         /   | logic |      +----------+
        // |  AND  i[0]--<--'    +-------+      +----------+
        // o       |                         [1]s Dependee | unused
        // | logic i[1]--<--,                   +----------+
        // +-------+         \   +-------+      +----------+
        //                    `--o Slot  |<==[7]s Dependee | connected
        //                       | logic |      +----------+
        //                       +-------+
        for (const auto & slot : _dependees_) {
          top_logic.connect_dependee(port++, slot.first);
        }
      } else {
        // It there is only one dependee, then the embedded logic
        // is a single SLOT logic connected to the unique dependee slot:
        //
        //                +----------+
        //             [0]s Dependee | unused
        // +-------+      +----------+
        // | Slot  |      +----------+
        // o       |<==[1]s Dependee | connected
        // | logic |      +----------+
        // +-------+      +----------+
        //             [7]s Dependee | unused
        //                +----------+
        //
        _logic_.reset(new slot_logic(*this));
        slot_logic * sl = dynamic_cast<slot_logic*>(_logic_.get());
        sl->set_dependee_slot(_dependees_.begin()->first);
      }
      return;
    }

    bool variant_dependency::operator()() const
    {
      DT_THROW_IF(!is_locked(), std::logic_error,
                  "Dependency is not locked!");
      return _logic_->operator()();
    }

    void variant_dependency::tree_dump(std::ostream & out_,
                                       const std::string & title_,
                                       const std::string & indent_,
                                       bool inherit_) const
    {
      if (!title_.empty()) {
        out_ << indent_ << title_ << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::tag << "Logging: '"
           << datatools::logger::get_priority_label(_logging_) << "'" << std::endl;

      out_ << indent_ << i_tree_dumpable::tag << "Depender: ";
      if (!_depender_.is_valid()) {
        out_ << "<none>";
      } else {
        out_ << "'" << _depender_ << "' ";
        switch (_depender_.get_type()) {
        case variant_object_info::OBJECT_REGISTRY: out_ << "(registry)"; break;
        case variant_object_info::OBJECT_VARIANT: out_ << "(variant)"; break;
        case variant_object_info::OBJECT_PARAMETER_VALUE_GROUP: out_ << "(parameter value group)"; break;
        case variant_object_info::OBJECT_PARAMETER_ENUM_STRING_VALUE: out_ << "(parameter enumerate string value)"; break;
        }
      }
      out_ << std::endl;

      out_ << indent_ << i_tree_dumpable::tag << "Dependees: ";
      if (_dependees_.size() == 0) {
        out_ << "<none>";
      } else {
        out_ << "[" << _dependees_.size() << "]";
      }
      out_ << std::endl;
      std::size_t dep_counter = 0;
      for (const auto & dependee : _dependees_) {
        out_ << indent_ << i_tree_dumpable::skip_tag;
        if (++dep_counter == _dependees_.size()) {
          out_ << i_tree_dumpable::last_tag;
        } else {
          out_ << i_tree_dumpable::tag;
        }
        out_ << "Slot[#" << dependee.first << "] = '" << dependee.second << "'";
        out_ << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::tag
           << "Logic: ";
      if (_logic_.get() == nullptr) {
        out_ << "<none>";
      }
      out_ << std::endl;
      if (_logic_.get() != nullptr) {
        std::ostringstream indent2;
        indent2 << indent_ << i_tree_dumpable::skip_tag;
        _logic_->tree_dump(out_, "", indent2.str());
      }

      out_ << indent_ << i_tree_dumpable::tag
           << "Validity : " << (is_valid() ? "<yes>" : "<no>")
           << std::endl;

      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
           << "Locked : " << (is_locked() ? "<yes>" : "<no>")
           << std::endl;

      return;
    }

  }  // end of namespace configuration

}  // end of namespace datatools

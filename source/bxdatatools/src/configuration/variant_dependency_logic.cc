// datatools/configuration/variant_dependency_logic.cc
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
#include <datatools/configuration/variant_dependency_logic.h>

// Standard library:
#include <string>
#include <vector>

// This project:
#include <datatools/configuration/variant_object_info.h>
#include <datatools/configuration/variant_repository.h>
#include <datatools/configuration/variant_registry.h>
#include <datatools/configuration/variant_dependency_logic_parsing.h>

namespace datatools {

  namespace configuration {

    base_dependency_logic::base_dependency_logic(variant_dependency & vdep_)
      : _owner_dependency_(&vdep_)
    {
      if (vdep_.has_repository()) {
        _repository = &vdep_.get_repository();
      }
      return;
    }

    base_dependency_logic::~base_dependency_logic()
    {
      return;
    }

    const variant_dependency & base_dependency_logic::get_owner_dependency() const
    {
      return *_owner_dependency_;
    }

    variant_dependency & base_dependency_logic::_grab_owner_dependency()
    {
      return *_owner_dependency_;
    }

    const variant_repository & base_dependency_logic::get_repository() const
    {
      return *_repository;
    }

    void base_dependency_logic::tree_dump(std::ostream & out_,
                                          const std::string & title_,
                                          const std::string & indent_,
                                          bool inherit_) const
    {
      if (!title_.empty()) {
        out_ << indent_ << title_ << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::tag << "GUID: "
           << "'" << guid() << "'"
           << std::endl;

      if (min_ports()) {
        out_ << indent_ << i_tree_dumpable::tag << "Min ports: ";
        out_ << min_ports();
        out_ << std::endl;
      }

      if (max_ports()) {
        out_ << indent_ << i_tree_dumpable::tag << "Max ports: ";
        if (max_ports() == std::numeric_limits<std::size_t>::max()) {
          out_ << "<unlimited>";
        } else {
          out_ << max_ports();
        }
        out_ << std::endl;
      }

      if (max_ports() > 0) {
        out_ << indent_ << i_tree_dumpable::tag << "Input ports: ";
        if (_inputs.size()) {
          out_ << '[' << _inputs.size() << ']';
        } else {
          out_ << "<none>";
        }
        out_ << std::endl;
        bool has_invalid_inputs = false;
        std::size_t counter = 0;
        for (const auto & input : _inputs) {
          out_ << indent_ << i_tree_dumpable::skip_tag;
          std::ostringstream indent2;
          indent2 << indent_ << i_tree_dumpable::skip_tag;
          if (++counter == _inputs.size()) {
            out_ << i_tree_dumpable::last_tag;
            indent2 << i_tree_dumpable::last_skip_tag;
          } else {
            out_ << i_tree_dumpable::tag;
            indent2 << i_tree_dumpable::skip_tag;
          }
          out_ << "Port[#" << input.first << "]" << std::endl;
          input.second->tree_dump(out_, "", indent2.str());
        }
      }

      out_ << indent_ << i_tree_dumpable::tag << "Local: "
           << (_owner_dependency_->is_local() ? "<yes>" : "<no>")
           << std::endl;

      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
           << "Validity : " << (is_valid() ? "<yes>" : "<no>")
           << std::endl;

      return;
    }

    bool base_dependency_logic::is_valid() const
    {
      if (_inputs.size() < min_ports()) return false;
      for (const auto & input : _inputs) {
        if (!input.second->is_valid()) return false;
      }
      return true;
    }

    // virtual
    std::size_t base_dependency_logic::min_ports() const
    {
      return 1;
    }

    // virtual
    std::size_t base_dependency_logic::max_ports() const
    {
      return std::numeric_limits<std::size_t>::max();
    }

    void base_dependency_logic::connect_dependee(const unsigned int port_,
                                                 const unsigned int dependee_slot_)
    {
      unsigned int port = port_;
      if (!_inputs.count(port)) {
        DT_THROW_IF(_inputs.size() == max_ports(), std::logic_error,
                    "Maximum number of ports is reached!");
      }
      DT_LOG_DEBUG(_owner_dependency_->get_logging(),
                   "Creating 'slot' logic from dependee slot [#" << dependee_slot_ << "]...");
      slot_logic * sl = new slot_logic(_grab_owner_dependency());
      dependency_logic_ptr_type lgc(sl);
      _inputs[port] = lgc;
      sl->set_dependee_slot(dependee_slot_);
      return;
    }

    void base_dependency_logic::disconnect(const unsigned int port_)
    {
      _inputs.erase(port_);
      return;
    }

    void base_dependency_logic::connect(const unsigned int port_, dependency_logic_ptr_type logic_)
    {
      unsigned int port = port_;
      DT_THROW_IF(_inputs.count(port), std::logic_error,
                  "Input port [#" << port << "] is already connected to a dependency logic!");
      if (!_inputs.count(port)) {
        DT_THROW_IF(_inputs.size() == max_ports(), std::logic_error,
                    "Maximum number of input ports (=" << max_ports()  << ") is reached for dependency logic with GUID='" << guid() << "'!");
      }
      _inputs[port] = logic_;
      return;
    }

    base_dependency_logic &
    base_dependency_logic::connect(const unsigned int port_,
                                   const std::string & guid_)
    {
      dependency_logic_ptr_type lgc;
      dependency_logic_factory_dict_type & facts = dependency_logic_factories();
      DT_THROW_IF(!facts.count(guid_),
                  std::logic_error,
                  "No logic with guid='" << guid_ << "'!");
      dependency_logic_factory_type fact = facts.find(guid_)->second;
      lgc.reset(fact(_grab_owner_dependency()));
      connect(port_, lgc);
      return *_inputs.find(port_)->second;
    }

    bool base_dependency_logic::check_active_variant(const variant_object_info & voinfo_) const
    {
      bool checked = false;
      DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "VO info = '" << voinfo_ << "'!");
      DT_THROW_IF(!voinfo_.is_variant(), std::logic_error, "Only variant are supported!");

      if (_owner_dependency_->has_local_scope()) {
        DT_THROW_IF(!voinfo_.is_local(), std::logic_error, "Only local variants are supported!");
        bool active_variant =
          _owner_dependency_->get_registry().is_active_variant(voinfo_.get_variant_local_path());
        if (active_variant) {
          checked = true;
        }
      }

      if (_owner_dependency_->has_global_scope()) {
        DT_THROW_IF(!voinfo_.is_global(), std::logic_error, "Only global variants are supported!");
        bool active_variant = _repository->is_active_variant(voinfo_.get_registry_name(),
                                                             voinfo_.get_variant_local_path());
        if (active_variant) {
          checked = true;
        }
      }

      return checked;
    }

    dependency_logic_factory_dict_type & dependency_logic_factories()
    {
      static std::unique_ptr<dependency_logic_factory_dict_type> _factories;
      if (_factories.get() == nullptr) {
        _factories.reset(new dependency_logic_factory_dict_type);
        _factories->operator[]("and") = boost::factory<and_logic*>();
        _factories->operator[]("or")  = boost::factory<or_logic*>();
        _factories->operator[]("not") = boost::factory<not_logic*>();
        _factories->operator[]("xor") = boost::factory<xor_logic*>();
      }
      return *_factories.get();
    }

    slot_logic::slot_logic(variant_dependency & vdep_)
      : base_dependency_logic(vdep_)
    {
      return;
    }

    slot_logic::~slot_logic()
    {
      return;
    }

    // virtual
    bool slot_logic::operator()() const
    {
      DT_THROW_IF(!is_valid(), std::logic_error, "Invalid logic!");
      const variant_object_info & dependee = get_owner_dependency().get_dependee(_dependee_slot_);
      bool checked = this->check_active_variant(dependee);
      return checked;
    }

    const char * slot_logic::guid() const
    {
      return "slot";
    }

    std::size_t slot_logic::min_ports() const
    {
      return 0;
    }

    std::size_t slot_logic::max_ports() const
    {
      return 0;
    }

    void slot_logic::set_dependee_slot(const unsigned int dependee_slot_)
    {
      DT_LOG_DEBUG(get_owner_dependency().get_logging(),
                   "Setting dependee slot [#" << dependee_slot_ << "]...");
      DT_THROW_IF(!get_owner_dependency().has_dependee(dependee_slot_),
                  std::logic_error,
                  "Parent dependency has no dependee at slot=[" << dependee_slot_ << "]!");
      const variant_object_info & dependee = get_owner_dependency().get_dependee(dependee_slot_);
      const std::string & variant_local_path = dependee.get_variant_local_path();

      if (get_owner_dependency().has_global_scope()) {
        const std::string & registry_name = dependee.get_registry_name();
        DT_THROW_IF(!get_repository().has_registry(registry_name), std::logic_error,
                    "Variant repository has no registry named '" << registry_name << "'!");
        DT_THROW_IF(!get_repository().has_variant(registry_name, variant_local_path),
                    std::logic_error,
                    "Variant repository has no variant with path '"
                    << variant_local_path << "' in registry '" << registry_name << "'!");
      }

      if (get_owner_dependency().has_local_scope()) {
        DT_THROW_IF(!get_owner_dependency().get_registry().has_variant_record(variant_local_path),
                    std::logic_error,
                    "Variant registry has no variant with path '"
                    << variant_local_path << "'!");
      }

      _dependee_slot_ = dependee_slot_;
      return;
    }

    bool slot_logic::is_valid() const
    {
      if (!base_dependency_logic::is_valid()) return false;
      if (_dependee_slot_ == std::numeric_limits<unsigned int>::max()) return false;
      return true;
    }

    void slot_logic::tree_dump(std::ostream & out_,
                               const std::string & title_,
                               const std::string & indent_,
                               bool inherit_) const
    {
      this->base_dependency_logic::tree_dump(out_, title_, indent_, true);

      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
           << "Variant dependee slot : " << _dependee_slot_
           << " (" << get_owner_dependency().get_dependee(_dependee_slot_) << ')'
           << std::endl;

      return;
    }

    and_logic::and_logic(variant_dependency & vdep_)
      : base_dependency_logic(vdep_)
    {
      return;
    }

    and_logic::~and_logic()
    {
      return;
    }

    // virtual
    const char * and_logic::guid() const
    {
      return "and";
    }

    // virtual
    bool and_logic::operator()() const
    {
      DT_THROW_IF(!is_valid(), std::logic_error, "Invalid logic!");
      bool checked = true;
      for (const auto & input : _inputs) {
        unsigned int   port = input.first;
        base_dependency_logic & lgc  = *input.second;
        if (!lgc()) {
          checked = false;
          break;
        }
      }
      return checked;
    }

    or_logic::or_logic(variant_dependency & vdep_)
      : base_dependency_logic(vdep_)
    {
      return;
    }

    or_logic::~or_logic()
    {
      return;
    }

    // virtual
    const char * or_logic::guid() const
    {
      return "or";
    }

    // virtual
    bool or_logic::operator()() const
    {
      DT_THROW_IF(!is_valid(), std::logic_error, "Invalid logic!");
      bool checked = false;
      for (const auto & input : _inputs) {
        unsigned int   port = input.first;
        base_dependency_logic & lgc  = *input.second;
        if (lgc()) {
          checked = true;
          break;
        }
      }
      return checked;
    }

    xor_logic::xor_logic(variant_dependency & vdep_)
      : base_dependency_logic(vdep_)
    {
      return;
    }

    xor_logic::~xor_logic()
    {
      return;
    }

    // virtual
    const char * xor_logic::guid() const
    {
      return "xor";
    }

    // virtual
    bool xor_logic::operator()() const
    {
      DT_THROW_IF(!is_valid(), std::logic_error, "Invalid logic!");
      unsigned int check_counter = 0;
      bool checked = false;
      for (const auto & input : _inputs) {
        unsigned int   port = input.first;
        base_dependency_logic & lgc  = *input.second;
        if (lgc()) {
          check_counter++;
          if (check_counter > 1) return false;
        }
      }
      return check_counter == 1;
    }

    not_logic::not_logic(variant_dependency & vdep_)
      : base_dependency_logic(vdep_)
    {
      return;
    }

    not_logic::~not_logic()
    {
      return;
    }

    // virtual
    std::size_t not_logic::max_ports() const
    {
      return 1;
    }

    // virtual
    const char * not_logic::guid() const
    {
      return "not";
    }

    // virtual
    bool not_logic::operator()() const
    {
      DT_THROW_IF(!is_valid(), std::logic_error, "Invalid logic!");
      base_dependency_logic & lgc  = *(_inputs.begin()->second);
      return ! lgc();
    }

  }  // end of namespace configuration

}  // end of namespace datatools

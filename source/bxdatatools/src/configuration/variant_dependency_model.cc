// datatools/configuration/variant_dependency_model.cc
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
#include <datatools/configuration/variant_dependency_model.h>

// Standard library:
#include <string>
#include <set>
#include <iomanip>

// This package:
#include <datatools/configuration/variant_repository.h>
#include <datatools/configuration/variant_registry.h>
#include <datatools/configuration/variant_object_info.h>

namespace datatools {

  namespace configuration {

    variant_dependency_model::variant_dependency_model(const variant_repository & repository_)
    {
      _registry_ = nullptr;
      _repository_ =  &repository_;
      return;
    }

    variant_dependency_model::variant_dependency_model(const variant_registry & registry_)
    {
      _registry_ = &registry_;
      if (_registry_->has_parent_repository()) {
        _repository_ = &_registry_->get_parent_repository();
      }
      return;
    }

    bool variant_dependency_model::has_repository() const
    {
      return _repository_ != nullptr;
    }

    const variant_repository & variant_dependency_model::get_repository() const
    {
      DT_THROW_IF(!has_repository(), std::logic_error, "No repository is set!");
      return *_repository_;
    }

    bool variant_dependency_model::has_registry() const
    {
      return _registry_ != nullptr;
    }

    const variant_registry & variant_dependency_model::get_registry() const
    {
      DT_THROW_IF(!has_registry(), std::logic_error, "No registry is set!");
      return *_registry_;
    }

    datatools::logger::priority variant_dependency_model::get_logging() const
    {
      return _logging_;
    }

    void variant_dependency_model::set_logging(const datatools::logger::priority p_)
    {
      _logging_ = p_;
      return;
    }

    bool variant_dependency_model::is_local() const
    {
      return has_registry();
    }

    bool variant_dependency_model::is_valid() const
    {
      return true;
    }

    bool variant_dependency_model::is_initialized() const
    {
      return _initialized_;
    }

    unsigned int variant_dependency_model::add_dependee_record(unsigned int slot_,
                                                               const std::string & dependee_path_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Model is already initialized!");
      DT_THROW_IF(dependee_path_.empty(), std::logic_error, "Missing dependee path!");
      DT_THROW_IF(has_dependee_record_by_path(dependee_path_),
                  std::logic_error,
                  "A slot with dependee '" << dependee_path_ << "' already exists!");
      unsigned int slot = slot_;
      dependee_record dee_rec;
      dee_rec.dependee_path = dependee_path_;
      _dependee_records_[slot] = dee_rec;
      return slot;
    }

    bool variant_dependency_model::has_dependee_record_by_path(const std::string & dependee_path_) const
    {
      for (const auto & slot_pair : _dependee_records_) {
        if (slot_pair.second.dependee_path == dependee_path_) return true;
      }
      return false;
    }

    bool variant_dependency_model::has_dependee_record(unsigned int slot_) const
    {
      return _dependee_records_.count(slot_) == 1;
    }

    const variant_dependency_model::dependee_record_dict_type &
    variant_dependency_model::get_dependee_records() const
    {
      return _dependee_records_;
    }

    const std::string & variant_dependency_model::get_dependee_record_path(unsigned int slot_) const
    {
      dependee_record_dict_type::const_iterator found = _dependee_records_.find(slot_);
      DT_THROW_IF(found == _dependee_records_.end(), std::logic_error,
                  "No dependee slot [#" << slot_ << "]!");
      return found->second.dependee_path;
    }

    void variant_dependency_model::build_list_of_dependee_slots(std::set<unsigned int> & dep_slots_) const
    {
      dep_slots_.clear();
      for (const auto & slot_pair : _dependee_records_) {
        dep_slots_.insert(slot_pair.first);
      }
      return;
    }

    bool variant_dependency_model::has_dependency_record(const std::string & name_) const
    {
      return _dependency_records_.count(name_) == 1;
    }

    bool variant_dependency_model::has_dependency_record_by_depender_path(const std::string & depender_path_) const
    {
      for (const auto & dep_rec_pair : _dependency_records_) {
        if (dep_rec_pair.second.depender_path == depender_path_) return true;
      }
      return false;
    }

    void variant_dependency_model::add_dependency_record(const std::string & name_,
                                                         const std::string & depender_path_,
                                                         const std::set<unsigned int> & input_slots_,
                                                         const std::string & logic_expression_,
                                                         const datatools::logger::priority logging_)
    {
      DT_THROW_IF(name_.empty(), std::logic_error,
                  "Empty dependency name!");
      DT_THROW_IF(depender_path_.empty(), std::logic_error,
                  "Empty depender path!");
      DT_THROW_IF(input_slots_.size() == 0, std::logic_error,
                  "Empty set of input slots!");
      DT_THROW_IF(has_dependency_record_by_depender_path(depender_path_),
                  std::logic_error,
                  "A dependency associated to depender '" << depender_path_ << "' already exists!");
      dependency_record dep_rec;
      dep_rec.depender_path = depender_path_;
      dep_rec.input_slots   = input_slots_;
      dep_rec.logic_expression = logic_expression_;
      dep_rec.logging = logging_;
      _dependency_records_[name_] = dep_rec;
      return;
    }

    void variant_dependency_model::build_list_of_dependency_records(std::set<std::string> & dep_names_) const
    {
      dep_names_.clear();
      for (const auto & dep_rec_pair : _dependency_records_) {
        dep_names_.insert(dep_rec_pair.first);
      }
      return;
    }

    const variant_dependency_model::dependency_record_dict_type &
    variant_dependency_model::get_dependency_records() const
    {
      return _dependency_records_;
    }

    void variant_dependency_model::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Model is already initialized!");

      // Parse logging priority:
      {
        logger::priority prio = logger::extract_logging_configuration(config_, _logging_, true);
        if (prio != logger::PRIO_UNDEFINED) {
          set_logging(prio);
        }
      }
      DT_LOG_TRACE_ENTERING(_logging_);

      // Build the set of slots to be attached to dependee variants:
      std::set<unsigned int> slots;
      if (config_.has_key("dependees.slots")) {
        config_.fetch_positive("dependees.slots", slots);
      } else if (config_.has_key("dependees.number_of_slots")) {
        unsigned int noslots = config_.fetch_positive_integer("dependees.number_of_slots");
        for (unsigned int islot = 0; islot < noslots; islot++) {
          slots.insert(islot);
        }
      }
      // Associated dependee variants:
      for (auto islot : slots) {
        std::ostringstream slot_variant_key_oss;
        slot_variant_key_oss << "dependees.slot_" << islot << ".variant";
        std::string slot_variant_key = slot_variant_key_oss.str();
        DT_THROW_IF(!config_.has_key(slot_variant_key), std::logic_error,
                    "Missing '" << slot_variant_key << "' property!");
        std::string dependee_variant = config_.fetch_string(slot_variant_key);
        add_dependee_record(islot, dependee_variant);
      }

      if (config_.has_key("dependencies")) {
        std::set<std::string> depnames;
        config_.fetch("dependencies", depnames);
        for (const auto & depname : depnames) {
          dependency_record deprec;
          {
            std::ostringstream dep_depender_key_oss;
            dep_depender_key_oss << "dependencies." << depname << ".depender";
            std::string dep_depender_key = dep_depender_key_oss.str();
            DT_THROW_IF(!config_.has_key(dep_depender_key), std::logic_error,
                        "Missing '" << dep_depender_key << "' property!");
            deprec.depender_path = config_.fetch_string(dep_depender_key);
          }
          {
            std::ostringstream dep_slots_key_oss;
            dep_slots_key_oss << "dependencies." << depname << ".slots";
            std::string dep_slots_key = dep_slots_key_oss.str();
            DT_THROW_IF(!config_.has_key(dep_slots_key), std::logic_error,
                        "Missing '" << dep_slots_key << "' property!");
            config_.fetch_positive(dep_slots_key, deprec.input_slots);
          }
          {
            std::ostringstream dep_logexpr_key_oss;
            dep_logexpr_key_oss << "dependencies." << depname << ".logic";
            std::string dep_logexpr_key = dep_logexpr_key_oss.str();
            if (config_.has_key(dep_logexpr_key)) {
              deprec.logic_expression = config_.fetch_string(dep_logexpr_key);
            }
          }
          {
            std::ostringstream dep_logging_key_oss;
            dep_logging_key_oss << "dependencies." << depname << ".logging";
            std::string dep_logging_key = dep_logging_key_oss.str();
            if (config_.has_key(dep_logging_key)) {
              datatools::logger::priority logging =
                datatools::logger::get_priority(config_.fetch_string(dep_logging_key));
              if (logging != datatools::logger::PRIO_UNDEFINED) {
                deprec.logging = logging;
              }
            }
          }
          add_dependency_record(depname,
                                deprec.depender_path,
                                deprec.input_slots,
                                deprec.logic_expression,
                                deprec.logging);
        }
      }

      _at_init();

      _initialized_ = true;
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void variant_dependency_model::reset()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      DT_THROW_IF(!is_initialized(), std::logic_error, "Model is not initialized!");
      _initialized_ = false;
      _at_reset();
      return;
    }

    void variant_dependency_model::_at_init()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      for (const auto & dep_pair_rec : _dependency_records_) {
        const dependency_record & dep_rec = dep_pair_rec.second;
        // Build a new variant dependency from the record:
        dependency_ptr_type dep_ptr;
        if (is_local()) {
          dep_ptr.reset(new variant_dependency(*_registry_));
        } else {
          dep_ptr.reset(new variant_dependency(*_repository_));
        }
        variant_dependency & vdep = *dep_ptr.get();
        vdep.set_logging(dep_rec.logging);
        std::string depender_path = dep_rec.depender_path;
        bool registry_scope = false;
        if (is_local()) {
          registry_scope = true;
        }
        vdep.set_depender_by_path(depender_path, registry_scope);
        const std::set<unsigned int> & input_slots = dep_rec.input_slots;
        for (const unsigned int islot : input_slots) {
          DT_THROW_IF(!has_dependee_record(islot), std::domain_error,
                      "No dependee at slot #" << islot << "!");
          vdep.add_dependee_by_path(get_dependee_record_path(islot),
                                    islot);
        }
        const std::string & logic_expression = dep_rec.logic_expression;
        if (!logic_expression.empty()) {
          vdep.build_logic_from_formula(logic_expression);
        }
        vdep.lock();
        // _dependencies_[depender_path] = dep_ptr;
        _dependencies_.insert(std::pair<std::string,dependency_ptr_type>(depender_path, dep_ptr));
      }

      _at_init_build_table_of_dependers_();

      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void variant_dependency_model::_at_init_build_table_of_dependers_()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      for (const auto & dependency_pair : _dependencies_) {
        const variant_dependency & dependency = *dependency_pair.second.get();
        std::string depender_path = dependency.get_depender().get_full_path();
        DT_LOG_TRACE(_logging_, "depender = '" << depender_path << "'");
        std::set<unsigned int> dependee_slots;
        dependency.build_list_of_dependee_slots(dependee_slots);
        for (auto dependee_slot : dependee_slots) {
          DT_LOG_TRACE(_logging_, "depender slot = [" << dependee_slot << "]");
          const variant_object_info & dependee_voi = dependency.get_dependee(dependee_slot);
          std::string dependee_path = dependee_voi.get_full_path();
          DT_LOG_TRACE(_logging_, "dependee = '" << dependee_path << "'");
          dependers_table_type::iterator found = _dependers_.find(dependee_path);
          if (found == _dependers_.end()) {
            std::set<std::string> first;
            first.insert(depender_path);
            _dependers_.insert(std::pair<std::string, std::set<std::string>>(dependee_path, first));
          } else {
            found->second.insert(depender_path);
          }
        }
      }
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void variant_dependency_model::_at_reset()
    {
      _dependers_.clear();
      _dependencies_.clear();
      _dependency_records_.clear();
      _dependee_records_.clear();
      return;
    }

    bool variant_dependency_model::has_dependency(const std::string & path_) const
    {
      return _dependencies_.count(path_) == 1;
    }

    const variant_dependency &
    variant_dependency_model::get_dependency(const std::string & path_) const
    {
      DT_THROW_IF(!has_dependency(path_), std::domain_error,
                  "No dependency associated to path '" << path_ << "'!");
      return *_dependencies_.find(path_)->second.get();
    }

    const variant_dependency_model::dependency_ptr_type &
    variant_dependency_model::get_dependency_ptr(const std::string & path_) const
    {
      DT_THROW_IF(!has_dependency(path_), std::domain_error,
                  "No dependency associated to path '" << path_ << "'!");
      return _dependencies_.find(path_)->second;
    }

    const variant_dependency_model::dependency_dict_type & variant_dependency_model::get_dependencies() const
    {
      return _dependencies_;
    }

    const variant_dependency_model::dependers_table_type &
    variant_dependency_model::get_dependers_per_dependee() const
    {
      return _dependers_;
    }

    void variant_dependency_model::tree_dump(std::ostream & out_,
                                             const std::string & title_,
                                             const std::string & indent_,
                                             bool inherit_) const
    {
      if (!title_.empty()) {
        out_ << indent_ << title_ << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::tag
           << "Repository: ";
      if (has_repository()) {
        out_ << "'" << _repository_->get_name() << "'";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Registry: ";
      if (has_registry()) {
        out_ << "'" << _registry_->get_name() << "'";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Local : " << (is_local() ? "<yes>" : "<no>")
           << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Dependee records: ";
      if (_dependency_records_.size()) {
        out_ << "[" << _dependee_records_.size() << "]";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;
      {
        std::size_t depee_rec_counter = 0;
        for (const auto & depee_rec_pair : _dependee_records_) {
          out_ << indent_ << i_tree_dumpable::skip_tag;
          if (++depee_rec_counter == _dependee_records_.size()) {
            out_ << i_tree_dumpable::last_tag;
          } else {
            out_ << i_tree_dumpable::tag;
          }
          out_ << "Slot[#" << depee_rec_pair.first << "] "
               << ": variant '" << depee_rec_pair.second.dependee_path << "'";
          out_ << std::endl;
        }
      }

      out_ << indent_ << i_tree_dumpable::tag
           << "Dependency records: ";
      if (_dependency_records_.size()) {
        out_ << "[" << _dependency_records_.size() << "]";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;
      {
        std::size_t dep_counter = 0;
        for (const auto & dep_rec_pair : _dependency_records_) {
          out_ << indent_ << i_tree_dumpable::skip_tag;
          std::ostringstream indent2;
          indent2 << indent_ << i_tree_dumpable::skip_tag;
          if (++dep_counter == _dependency_records_.size()) {
            out_ << i_tree_dumpable::last_tag;
            indent2 << i_tree_dumpable::last_skip_tag;
          } else {
            out_ << i_tree_dumpable::tag;
            indent2 << i_tree_dumpable::skip_tag;
          }
          out_ << "Dependency record '" << dep_rec_pair.first << "' : " << std::endl;
          out_ << indent2.str() << i_tree_dumpable::tag << "Depender: "
               << "'" << dep_rec_pair.second.depender_path << "'"
               << std::endl;
          out_ << indent2.str() << i_tree_dumpable::tag << "Input slots: ";
          if (dep_rec_pair.second.input_slots.size()) {
            std::size_t nslots = 0;
            for (auto islot : dep_rec_pair.second.input_slots) {
              if (nslots++ > 0) out_ << ", ";
              out_ << "[#" << islot << "]";
            }
          } else {
            out_ << "<none>";
          }
          out_ << std::endl;
          out_ << indent2.str() << i_tree_dumpable::last_tag << "Logic expression: "
               << "'" << dep_rec_pair.second.logic_expression << "'"
               << std::endl;

        }
      }

      out_ << indent_ << i_tree_dumpable::tag
           << "Dependencies: ";
      if (_dependencies_.size()) {
        out_ << "[" << _dependencies_.size() << "]";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

      {
        std::size_t dep_counter = 0;
        for (const auto & dep_pair : _dependencies_) {
          out_ << indent_ << i_tree_dumpable::skip_tag;
          std::ostringstream indent2;
          indent2 << indent_ << i_tree_dumpable::skip_tag;
          if (++dep_counter == _dependencies_.size()) {
            out_ << i_tree_dumpable::last_tag;
            indent2 << i_tree_dumpable::last_skip_tag;
          } else {
            out_ << i_tree_dumpable::tag;
            indent2 << i_tree_dumpable::skip_tag;
          }
          out_ << "Dependency '" << dep_pair.first << "' : " << std::endl;
          dep_pair.second->tree_dump(out_, "", indent2.str());
        }
      }

      out_ << indent_ << i_tree_dumpable::tag
           << "Dependers per dependee: ";
      if (_dependers_.size()) {
        out_ << "[" << _dependers_.size() << "]";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;
      {
        std::size_t entry_counter = 0;
        for (const auto & entry_pair : _dependers_) {
          out_ << indent_ << i_tree_dumpable::skip_tag;
          std::ostringstream indent2;
          indent2 << indent_ << i_tree_dumpable::skip_tag;
          if (++entry_counter == _dependers_.size()) {
            out_ << i_tree_dumpable::last_tag;
            indent2 << i_tree_dumpable::last_skip_tag;
          } else {
            out_ << i_tree_dumpable::tag;
            indent2 << i_tree_dumpable::skip_tag;
          }
          out_ << "Dependee: '" << entry_pair.first << "' : " << std::endl;
          {
            std::size_t depender_count = 0;
            for (const auto & depender : entry_pair.second) {
              out_ << indent2.str();
              // out_ << indent_ << i_tree_dumpable::skip_tag << i_tree_dumpable::skip_tag;
              if (++depender_count == entry_pair.second.size()) {
                out_ << i_tree_dumpable::last_tag;
              } else {
                out_ << i_tree_dumpable::tag;
              }
              out_ << "Depender: '" << depender << "'";
              out_ << std::endl;
            }
          }
          //dep_pair.second->tree_dump(out_, "", indent2.str());
        }
      }

      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
           << "Initialized : " << (is_initialized() ? "<yes>" : "<no>")
           << std::endl;

      return;
    }

    void variant_dependency_model::print_rst(std::ostream & out_,
                                             const uint32_t flags_,
                                             const std::string & indent_) const
    {
      bool with_title = true;
      bool with_scope = true;
      if (flags_ & PRINT_RST_NO_TITLE) {
        with_title = false;
      }
      if (flags_ & PRINT_RST_NO_SCOPE) {
        with_scope = false;
      }

      if (with_title) {
        std::ostringstream titleoss;
        titleoss << "Variant dependency model";
        out_ << std::setw(titleoss.str().length()) << std::setfill('=') << "" << std::endl;
        out_ << titleoss.str() << std::endl;
        out_ << std::setw(titleoss.str().length()) << std::setfill('=') << "" << std::endl;
        out_ << std::endl;
      }

      {
        std::size_t count = 0;

        if (with_scope) {
          if (!is_local() && has_repository()) {
            out_ << indent_;
            out_ << "* Global to repository ``\"" << get_repository().get_name() << "\"``" << std::endl;
            count++;
          }

          if (is_local()) {
            out_ << indent_;
            out_ << "* Local to registry : ``\"" << get_registry().get_name() << "\"``" << std::endl;
            count++;
          }
        }

        {
          out_ << indent_;
          out_ << "* Dependees : " << _dependee_records_.size() << std::endl;
          count++;
          if (_dependee_records_.size()) {
            out_ << std::endl;
            for (const auto & depee : _dependee_records_) {
              out_ << indent_;
              out_ << "  * Dependee slot [" << depee.first << "] : ``\"" <<  depee.second.dependee_path << "\"``" << std::endl;
            }
            out_ << std::endl;
          }
        }

        {
          out_ << indent_;
          out_ << "* Dependencies : " << _dependency_records_.size() << std::endl;
          count++;
          if (_dependency_records_.size()) {
            out_ << std::endl;
            for (const auto & depy : _dependency_records_) {
              out_ << indent_;
              out_ << "  * Dependency : ``\"" << depy.first << "\"``" << std::endl;
              const dependency_record & deprec = depy.second;
              out_ << std::endl;
              out_ << indent_;
              out_ << "    * Depender : ``\"" << deprec.depender_path << "\"``" << std::endl;
              out_ << indent_;
              out_ << "    * Input dependee slots : ";
              for (auto slot : deprec.input_slots) {
                out_ << '[' << slot << "] ";
              }
              out_ << std::endl;
              out_ << indent_;
             out_ << "    * Logic : ``\"" << deprec.logic_expression << "\"``" << std::endl;
            }
            out_ << std::endl;
          }
        }

        if (count) {
          out_ << std::endl;
        }
      }

      return;
    }

  }  // end of namespace configuration

}  // end of namespace datatools

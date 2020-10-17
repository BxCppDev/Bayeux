// signal_shape_builder.cc
/* Copyright (C) 2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Copyright (C) 2016 Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>
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
#include <mctools/signal/signal_shape_builder.h>

// Third party:
// Boost:
#include <boost/algorithm/string.hpp>
// Bayeux:
#include <datatools/service_manager.h>
#include <datatools/factory_macros.h>
#include <datatools/multi_properties.h>

// This module:
#include <mctools/signal/base_signal.h>

namespace mctools {

  namespace signal {

    // static
    const std::string & signal_shape_builder::reference_functor_prefix()
    {
      static const std::string _p("@ref:");
      return _p;
    }

    signal_shape_builder::signal_shape_builder()
      : _initialized_(false)
      , _logging_(datatools::logger::PRIO_FATAL)
    {
      return;
    }

    signal_shape_builder::~signal_shape_builder()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    datatools::logger::priority signal_shape_builder::get_logging_priority() const
    {
      return _logging_;
    }

    void signal_shape_builder::set_logging_priority(const datatools::logger::priority p_)
    {
      _logging_ = p_;
      return;
    }

    bool signal_shape_builder::has_service_mgr() const
    {
      return _service_mgr_ != nullptr;
    }

    void signal_shape_builder::set_service_mgr(const datatools::service_manager & smgr_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Signal shape builder is already initialized!");
      _service_mgr_ = &smgr_;
      return;
    }

    const datatools::service_manager & signal_shape_builder::get_service_mgr() const
    {
      DT_THROW_IF(!has_service_mgr(), std::logic_error, "No service manager!");
      return *_service_mgr_;
    }

    bool signal_shape_builder::has_category() const
    {
      return !_category_.empty();
    }

    void signal_shape_builder::set_category(const std::string & c_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Signal shape builder is already initialized!");
      _category_ = c_;
      return;
    }

    void signal_shape_builder::reset_category()
    {
      _category_.clear();
      return;
    }

    const std::string & signal_shape_builder::get_category() const
    {
      return _category_;
    }

    void signal_shape_builder::add_registered_shape_type_id(const std::string & id_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Signal shape builder is already initialized!");
      _registered_shape_type_ids_.insert(id_);
      return;
    }

    bool signal_shape_builder::has_registered_shape_type_id(const std::string & id_) const
    {
      return _registered_shape_type_ids_.count(id_);
    }

    bool signal_shape_builder::is_initialized() const
    {
      return _initialized_;
    }

    void signal_shape_builder::initialize_simple()
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Signal shape builder is already initialized!");
      _init_(nullptr);
      return;
    }

    void signal_shape_builder::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Signal shape builder is already initialized!");
      // Logging priority:
      datatools::logger::priority p = datatools::logger::extract_logging_configuration(config_);
      DT_THROW_IF(p == datatools::logger::PRIO_UNDEFINED,
                  std::logic_error,
                  "Invalid logging priority level !");
      set_logging_priority(p);

      _init_(&config_);
      return;
    }

    void signal_shape_builder::_init_registration_()
    {
      DT_LOG_DEBUG(_logging_, "Private shape functor factory registration...");
      const freg_type & system_freg =
        DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(mygsl::i_unary_function);
      if (datatools::logger::is_debug(_logging_)) {
        system_freg.tree_dump(std::cerr, "Shape functor factory system register:", "[debug] ");
      }
      for (const auto & id : _registered_shape_type_ids_) {
        DT_THROW_IF(!system_freg.has(id), std::logic_error,
                    "No factory with identifier '" << id
                    << "' is registered from the system factory register!");
        const freg_type::factory_record_type & the_out_factory_record = system_freg.get_record(id);
        const freg_type::factory_type & the_out_factory = the_out_factory_record.fact;
        const std::type_info * the_out_tinfo = the_out_factory_record.tinfo;
        _freg_.registration(id,
                            the_out_factory,
                            *the_out_tinfo,
                            the_out_factory_record.description,
                            the_out_factory_record.category);
      }
      return;
    }

    void signal_shape_builder::_init_(const datatools::properties * config_)
    {
      if (_initialized_) return;
      DT_LOG_DEBUG(_logging_, "Private initialization...");
      if (has_service_mgr()) {
        // Unused
      }

      if (config_ != nullptr) {

        if (config_->has_key("category")) {
          const std::string & cat = config_->fetch_string("category");
          set_category(cat);
        }
      }
      // DT_THROW_IF(!has_category(), std::logic_error, "Signal shape builder has no category!");

      if (config_ != nullptr) {
        std::vector<std::string> reference_functors_filedefs;
        if (config_->has_key("reference_functors")) {
          config_->fetch("reference_functors", reference_functors_filedefs);
        }
        for (std::size_t i = 0; i < reference_functors_filedefs.size(); i++) {
          const std::string & filedef = reference_functors_filedefs[i];
          load_reference_functors_from_file(filedef);
        }
      }

      if (config_ != nullptr) {
        std::set<std::string> registered_shape_type_ids;
        if (config_->has_key("registered_shapes")) {
          config_->fetch("registered_shapes", registered_shape_type_ids);
          for (const auto & id : registered_shape_type_ids) {
            add_registered_shape_type_id(id);
          }
        }
      }
      if (!has_registered_shape_type_id("mctools::signal::multi_signal_shape")) {
        add_registered_shape_type_id("mctools::signal::multi_signal_shape");
      }
      DT_THROW_IF(_registered_shape_type_ids_.size() == 0, std::logic_error, "Signal shape builder has no registered shape type identifiers!");

      _init_registration_();

      _initialized_ = true;
      return;
    }

    void signal_shape_builder::_update_all_functors_()
    {
      _all_functors_.clear();
      for (const auto& rfpair : _reference_functors_) {
        std::string key;
        std::ostringstream key_oss;
        key_oss << reference_functor_prefix() << rfpair.first;
        key = key_oss.str();
        _all_functors_[key] = rfpair.second;
      }
      for (const auto& fpair : _functors_) {
        _all_functors_[fpair.first] = fpair.second;
      }
      return;
    }

    void signal_shape_builder::reset()
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Signal shape builder is not initialized!");
      _reset_();
      return;
    }

    void signal_shape_builder::_reset_()
    {
      if (!_initialized_) return;
      _initialized_ = false;
      _all_functors_.clear();
      _functors_.clear();
      _reference_functors_.clear();
      reset_category();
      _registered_shape_type_ids_.clear();
      return;
    }

    void signal_shape_builder::tree_dump(std::ostream & out_,
                                         const std::string & title_,
                                         const std::string & indent_,
                                         bool inherit_) const
    {
      if (!title_.empty()) {
        out_ << indent_ << title_ << std::endl;
      }

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Logging : '" << datatools::logger::get_priority_label(_logging_) << "'" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Category : ";
      if (has_category()) {
        out_ << "'" << _category_ << "'";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Registered shape type identifiers : ";
      if (_registered_shape_type_ids_.size()) {
        out_ << '[' << _registered_shape_type_ids_.size() << ']';
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;
      {
        std::size_t count = 0;
        for (const auto & id : _registered_shape_type_ids_) {
          out_ << indent_ << datatools::i_tree_dumpable::skip_tag;
          if (++count == _registered_shape_type_ids_.size()) {
            out_ << datatools::i_tree_dumpable::last_tag;
          } else {
            out_ << datatools::i_tree_dumpable::tag;
          }
          out_ << "Registered signal shape : '" << id << "'";
          out_ << std::endl;
        }
      }

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Reference functors : ";
      if (_reference_functors_.size()) {
        out_ << '[' << _reference_functors_.size() << ']';
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;
      {
        std::size_t count = 0;
        for (const auto & f : _reference_functors_) {
          out_ << indent_ << datatools::i_tree_dumpable::skip_tag;
          if (++count == _reference_functors_.size()) {
            out_ << datatools::i_tree_dumpable::last_tag;
          } else {
            out_ << datatools::i_tree_dumpable::tag;
          }
          out_ << "Reference functor : '" << f.first << "'";
          out_ << std::endl;
        }
      }

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Signal shape user functors : ";
      if (_functors_.size()) {
        out_ << '[' << _functors_.size() << ']';
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;
      {
        std::size_t count = 0;
        for (const auto & f : _functors_) {
          out_ << indent_ << datatools::i_tree_dumpable::skip_tag;
          if (++count == _functors_.size()) {
            out_ << datatools::i_tree_dumpable::last_tag;
          } else {
            out_ << datatools::i_tree_dumpable::tag;
          }
          out_ << "User functor : '" << f.first << "'";
          out_ << std::endl;
        }
      }

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "All functors : ";
      if (_all_functors_.size()) {
        out_ << '[' << _all_functors_.size() << ']';
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Initialized : " << (is_initialized() ? "<yes>" : "<no>" )  << std::endl;

      return;
    }

    void signal_shape_builder::load_reference_functors_from_file(const std::string & filename_)
    {
      std::string filename = filename_;
      datatools::fetch_path_with_env(filename);
      datatools::multi_properties mprops;
      mprops.read(filename);
      const datatools::multi_properties::entries_ordered_col_type & ordered = mprops.ordered_entries();
      for (const auto he : ordered) {
        const datatools::multi_properties::entry & mpe = *he;
        add_reference_functor(mpe.get_key(),
                              mpe.get_meta(),
                              mpe.get_properties());
      }
      return;
    }

    void signal_shape_builder::add_reference_functor(const std::string & key_,
                                                     const std::string & shape_type_id_,
                                                     const datatools::properties & shape_params_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Signal shape builder is already initialized!");
      DT_THROW_IF(has_reference_functor(key_), std::logic_error,
                  "A reference functor with key '" << key_ << "' already exists!");
      mygsl::unary_function_handle_type fh;
      const freg_type & system_freg =
        DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(mygsl::i_unary_function);
      DT_THROW_IF(!system_freg.has(shape_type_id_), std::logic_error,
                  "Functor system factory with type identifier '" << shape_type_id_ << "' does not exist!");
      const freg_type::factory_type & the_factory = system_freg.get(shape_type_id_);
      fh.reset(the_factory());
      fh.grab().initialize(shape_params_, _reference_functors_);
      _reference_functors_[key_] = fh;
      _update_all_functors_();
      return;
    }

    const mygsl::i_unary_function &
    signal_shape_builder::create_signal_shape(const std::string & key_,
                                              const std::string & shape_type_id_,
                                              const datatools::properties & shape_params_)
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Signal shape builder is not initialized!");
      DT_THROW_IF(key_.empty() ||
                  boost::algorithm::starts_with(key_, reference_functor_prefix()),
                  std::logic_error,
                  "Invalid signal shape key '" << key_ << "'!");
      DT_THROW_IF(has_functor(key_), std::logic_error,
                  "A functor with key '" << key_ << "' already exists!");
      mygsl::unary_function_handle_type fh;
      DT_THROW_IF(!_freg_.has(shape_type_id_), std::logic_error,
                  "Signal shape functor factory with type identifier '" << shape_type_id_ << "' does not exist!");
      const freg_type::factory_type & the_factory = _freg_.get(shape_type_id_);
      fh.reset(the_factory());
      fh.grab().initialize(shape_params_, _all_functors_);
      _functors_[key_] = fh;
      _update_all_functors_();
      return fh.get();
    }

    std::string signal_shape_builder::_generate_functor_key()
    {
      std::string k;

      return k;
    }

    bool signal_shape_builder::has_functor_factory(const std::string & id_) const
    {
      return _freg_.has(id_);
    }

    bool signal_shape_builder::has_reference_functor(const std::string & key_) const
    {
      return _reference_functors_.count(key_);
    }

    const mygsl::i_unary_function & signal_shape_builder::get_reference_functor(const std::string & key_) const
    {
      DT_THROW_IF(!has_reference_functor(key_), std::logic_error,
                  "No reference functor with key '" << key_ << "'!");
      return _reference_functors_.find(key_)->second.get();
    }

    void signal_shape_builder::build_list_of_reference_functors(std::set<std::string> & list_) const
    {
      list_.clear();
      for (const auto & fpair : _reference_functors_) {
        list_.insert(fpair.first);
      }
      return;
    }

    bool signal_shape_builder::has_functor(const std::string & key_) const
    {
      return _functors_.count(key_);
    }

    void signal_shape_builder::build_list_of_functors(std::set<std::string> & list_) const
    {
      list_.clear();
      for (const auto & fpair : _functors_) {
        list_.insert(fpair.first);
      }
      return;
    }

    mygsl::i_unary_function & signal_shape_builder::grab_functor(const std::string & key_)
    {
      DT_THROW_IF(!has_functor(key_), std::logic_error,
                  "No functor with key '" << key_ << "'!");
      return _functors_.find(key_)->second.grab();
    }

    mygsl::const_unary_function_handle_type
    signal_shape_builder::get_functor_handle(const std::string & key_) const
    {
      DT_THROW_IF(!has_functor(key_), std::logic_error,
                  "No functor with key '" << key_ << "'!");
      mygsl::const_unary_function_handle_type ch;
      ch = _functors_.find(key_)->second.to_const();
      return ch; //_functors_.find(key_)->second;
    }

    const mygsl::i_unary_function & signal_shape_builder::get_functor(const std::string & key_) const
    {
      DT_THROW_IF(!has_functor(key_), std::logic_error,
                  "No functor with key '" << key_ << "'!");
      return _functors_.find(key_)->second.get();
    }

    void signal_shape_builder::clear_functor(const std::string & key_)
    {
      _functors_.erase(key_);
      _update_all_functors_();
      return;
    }

    void signal_shape_builder::clear_functors()
    {
      _functors_.clear();
      _update_all_functors_();
      return;
    }

    const mygsl::unary_function_dict_type & signal_shape_builder::get_functors() const
    {
      return _functors_;
    }

  } // end of namespace signal

} // end of namespace mctools

// base_signal.cc
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2015-04-10
 * Last modified : 2016-09-27
 *
 * Copyright (C) 2015-2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#include <mctools/signal/base_signal.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/factory.h>
#include <datatools/clhep_units.h>
// - Bayeux/mygsl:
#include <mygsl/i_unary_function.h>

// This project
#include <mctools/signal/signal_shape_builder.h>

namespace mctools {

  namespace signal {

    // Serialization implementation :
    DATATOOLS_SERIALIZATION_IMPLEMENTATION(base_signal, "mctools::signal::base_signal")

    // static
    const std::string & base_signal::shape_parameter_prefix()
    {
      static const std::string _p("__shape.");
      return _p;
    }

    base_signal::base_signal()
      : _logging_(datatools::logger::PRIO_FATAL)
    {
      return;
    }

    base_signal::base_signal(const base_signal & sig_)
      : geomtools::base_hit(sig_)
      , _logging_(datatools::logger::PRIO_FATAL)
   {
      _copy_(sig_);
      return;
    }

    base_signal::~base_signal()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    datatools::logger::priority base_signal::get_logging() const
    {
      return _logging_;
    }

    void base_signal::set_logging(const datatools::logger::priority p_)
    {
      _logging_ = p_;
      return;
    }

    void base_signal::_copy_(const base_signal & sig_)
    {
      if (this == &sig_) return;
      this->geomtools::base_hit::operator=(sig_);
      _initialized_ = sig_._initialized_;
      _logging_ =  sig_._logging_;
      _shape_type_id_ = sig_._shape_type_id_;
      if (!_shape_type_id_.empty()) {
        // If a shape ID is available, then the object
        // can be restored by the embedded factory using
        // shape configuration parameters stored in the
        // auxiliaries container:
        _reset_shape_();
        _owned_shape_ = false;
        _shape_ = nullptr;
      } else {
        if (sig_._shape_ != nullptr) {
          // TO BE DONE: use shape cloneable interface if available:
          // if (datatools::i_cloneable::is_cloneable(*sig_._shape_)) {
          //   _shape_ = sig_.get_shape()->clone();
          //   _owned_shape_ = true;
          // }
        }
      }
      return;
    }

    bool base_signal::is_copyable() const
    {
      if (has_shape_type_id()) {
        return true;
      }
      if (_shape_ != nullptr) {
         const datatools::i_cloneable * test_cloneable
           = dynamic_cast<const datatools::i_cloneable *>(_shape_);
         if (test_cloneable) {
           return true;
         }
      }
      return false;
    }

    base_signal & base_signal::operator=(const base_signal & sig_)
    {
      if (&sig_ == this) {
        return *this;
      }
      DT_THROW_IF(!sig_.is_copyable(), std::runtime_error,
                  "Source signal cannot be copied!");
      this->geomtools::base_hit::operator=(sig_);
      _initialized_ = sig_._initialized_;
      if (sig_.has_shape_type_id()) {
        _shape_type_id_ = sig_._shape_type_id_;
      } else {
        // XXX
      }
      _owned_shape_ = false;
      _shape_ = nullptr;
      return *this;
    }

    bool base_signal::is_valid() const
    {
      if (!this->geomtools::base_hit::is_valid()) {
        return false;
      }
      if (!datatools::is_valid(_time_ref_)) {
        return false;
      }
      if (!is_shape_instantiated()) {
        if (!has_shape_type_id()) {
          return false;
        }
      }
      return true;
    }

    bool base_signal::has_time_ref() const
    {
      return datatools::is_valid(_time_ref_);
    }

    double base_signal::get_time_ref() const
    {
      return _time_ref_;
    }

    void base_signal::set_time_ref(const double tref_)
    {
      _time_ref_ = tref_;
      return;
    }

    void base_signal::reset_time_ref()
    {
      datatools::invalidate(_time_ref_);
      return;
    }

    bool base_signal::has_shape_builder() const
    {
      return _shape_builder_ != nullptr;
    }

    const signal_shape_builder & base_signal::get_shape_builder() const
    {
      DT_THROW_IF(!has_shape_builder(), std::logic_error, "No shape builder is defined!");
      return *_shape_builder_;
    }

    signal_shape_builder & base_signal::grab_shape_builder()
    {
      DT_THROW_IF(!has_shape_builder(), std::logic_error, "No shape builder is defined!");
      return *_shape_builder_;
    }

    void base_signal::set_shape_builder(signal_shape_builder & sb_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Signal is already initialized!");
      DT_THROW_IF(!sb_.is_initialized(), std::logic_error, "Signal shape builder is not initialized!");
      _shape_builder_ = &sb_;
      return;
    }

    bool base_signal::has_shape_type_id() const
    {
      return !_shape_type_id_.empty();
    }

    const std::string & base_signal::get_shape_type_id() const
    {
      return _shape_type_id_;
    }

    void base_signal::set_shape_type_id(const std::string & id_)
    {
      _shape_type_id_ = id_;
      return;
    }

    void base_signal::set_shape_parameters(const datatools::properties & params_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Signal is already initialized!");
      reset_shape_parameters();
      params_.export_all_adding_prefix(grab_auxiliaries(), shape_parameter_prefix());
      return;
    }

    void base_signal::reset_shape_parameters()
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Signal is already initialized!");
      grab_auxiliaries().erase_all_starting_with(shape_parameter_prefix());
      return;
    }

    void base_signal::reset_shape_parameter(const std::string & key_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Signal is already initialized!");
      grab_auxiliaries().erase(shape_parameter_prefix() + key_);
      return;
    }

    void base_signal::set_shape_boolean_parameter(const std::string & key_,
                                                  const bool value_,
                                                  const std::string & desc_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Signal is already initialized!");
      if (grab_auxiliaries().has_key(shape_parameter_prefix() + key_)) {
        grab_auxiliaries().erase(shape_parameter_prefix() + key_);
      }
      grab_auxiliaries().store_boolean(shape_parameter_prefix() + key_, value_, desc_);
      return;
    }

    void base_signal::set_shape_boolean_parameters(const std::string & key_,
                                                   const std::vector<bool> & values_,
                                                   const std::string & desc_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Signal is already initialized!");
      if (grab_auxiliaries().has_key(shape_parameter_prefix() + key_)) {
        grab_auxiliaries().erase(shape_parameter_prefix() + key_);
      }
      grab_auxiliaries().store(shape_parameter_prefix() + key_, values_, desc_);
      return;
    }

    void base_signal::set_shape_integer_parameter(const std::string & key_,
                                                  const int value_,
                                                  const std::string & desc_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Signal is already initialized!");
      if (grab_auxiliaries().has_key(shape_parameter_prefix() + key_)) {
        grab_auxiliaries().erase(shape_parameter_prefix() + key_);
      }
      grab_auxiliaries().store_integer(shape_parameter_prefix() + key_, value_, desc_);
      return;
    }

    void base_signal::set_shape_integer_parameters(const std::string & key_,
                                                   const std::vector<int> & values_,
                                                   const std::string & desc_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Signal is already initialized!");
      if (grab_auxiliaries().has_key(shape_parameter_prefix() + key_)) {
        grab_auxiliaries().erase(shape_parameter_prefix() + key_);
      }
      grab_auxiliaries().store(shape_parameter_prefix() + key_, values_, desc_);
      return;
    }

    void base_signal::set_shape_string_parameter(const std::string & key_,
                                                 const std::string & value_,
                                                 const std::string & desc_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Signal is already initialized!");
      if (grab_auxiliaries().has_key(shape_parameter_prefix() + key_)) {
        grab_auxiliaries().erase(shape_parameter_prefix() + key_);
      }
      grab_auxiliaries().store_string(shape_parameter_prefix() + key_, value_, desc_);
      return;
    }

    void base_signal::set_shape_string_parameters(const std::string & key_,
                                                  const std::vector<std::string> & values_,
                                                  const std::string & desc_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Signal is already initialized!");
      if (grab_auxiliaries().has_key(shape_parameter_prefix() + key_)) {
        grab_auxiliaries().erase(shape_parameter_prefix() + key_);
      }
      grab_auxiliaries().store(shape_parameter_prefix() + key_, values_, desc_);
      return;
    }

    void base_signal::set_shape_real_parameter(const std::string & key_,
                                               const double value_,
                                               const std::string & desc_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Signal is already initialized!");
      if (grab_auxiliaries().has_key(shape_parameter_prefix() + key_)) {
        grab_auxiliaries().erase(shape_parameter_prefix() + key_);
      }
      grab_auxiliaries().store_real(shape_parameter_prefix() + key_, value_, desc_);
      return;
    }

    void base_signal::set_shape_real_parameters(const std::string & key_,
                                                const std::vector<double> & values_,
                                                const std::string & desc_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Signal is already initialized!");
      if (grab_auxiliaries().has_key(shape_parameter_prefix() + key_)) {
        grab_auxiliaries().erase(shape_parameter_prefix() + key_);
      }
      grab_auxiliaries().store(shape_parameter_prefix() + key_, values_, desc_);
      return;
    }

    void base_signal::set_shape_real_parameter_with_explicit_unit(const std::string & key_,
                                                                  const double value_,
                                                                  const std::string & unit_symbol_,
                                                                  const std::string & desc_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Signal is already initialized!");
      if (grab_auxiliaries().has_key(shape_parameter_prefix() + key_)) {
        grab_auxiliaries().erase(shape_parameter_prefix() + key_);
      }
      grab_auxiliaries().store_with_explicit_unit(shape_parameter_prefix() + key_, value_, desc_);
      grab_auxiliaries().set_unit_symbol(shape_parameter_prefix() + key_, unit_symbol_);
      return;
    }

    void base_signal::set_shape_real_parameters_with_explicit_unit(const std::string & key_,
                                                                   const std::vector<double> & values_,
                                                                   const std::string & unit_symbol_,
                                                                   const std::string & desc_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Signal is already initialized!");
      if (grab_auxiliaries().has_key(shape_parameter_prefix() + key_)) {
        grab_auxiliaries().erase(shape_parameter_prefix() + key_);
      }
      grab_auxiliaries().store(shape_parameter_prefix() + key_, values_, desc_);
      grab_auxiliaries().set_unit_symbol(shape_parameter_prefix() + key_, unit_symbol_);
      return;
    }

    bool base_signal::is_shape_instantiated() const
    {
      return _shape_ != nullptr;
    }

    const mygsl::i_unary_function & base_signal::get_shape() const
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Signal is not initialized!");
      return const_cast<mygsl::i_unary_function &>(const_cast<base_signal*>(this)->_get_shape_());
    }

    void base_signal::reset_shape()
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Signal is initialized and locked!");
      _reset_shape_();
      return;
    }

    void base_signal::_reset_shape_()
    {
     if (_shape_ != nullptr) {
        if (_owned_shape_) {
          DT_LOG_DEBUG(_logging_, "Deleting embedded shape functor of type '" << _shape_type_id_ << "'...");
          delete _shape_;
        }
        _shape_ = nullptr;
        _owned_shape_ = false;
      }
      return;
    }

    void base_signal::set_shape_owned(const mygsl::i_unary_function * shape_ptr_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Signal is initialized and locked!");
      _set_shape_(*shape_ptr_, true);
      return;
    }

    void base_signal::set_shape(const mygsl::i_unary_function & shape_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Signal is initialized and locked!");
      _set_shape_(shape_, false);
      return;
    }

    void base_signal::unlock()
    {
      if (!is_initialized()) return;
      DT_THROW_IF(is_shape_instantiated(), std::logic_error,
                  "Signal shape functor is already instantiated!");
      _initialized_ = false;
      return;
    }

    void base_signal::relock()
    {
      if (is_initialized()) return;
      _initialized_ = true;
      return;
    }

    bool base_signal::is_initialized() const
    {
      return _initialized_;
    }

    void base_signal::initialize_simple()
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Signal is already initialized!");
      _init_();
      return;
    }

    void base_signal::_init_()
    {
      DT_THROW_IF(!has_shape_type_id(), std::logic_error,
                  "Missing shape type identifier!");
      if (has_shape_builder()) {
        DT_THROW_IF(!get_shape_builder().has_registered_shape_type_id(get_shape_type_id()),
                    std::logic_error, "Signal shape type ID '" << get_shape_type_id() << "' is not registered in the shape builder!");
      } else {
        // No check
      }
      _initialized_ = true;
      return;
    }

    void base_signal::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Signal is already initialized!");

      if (_shape_ == nullptr) {
        if (!has_shape_type_id()) {
          DT_THROW_IF(!config_.has_key("shape_type_id"),
                      std::logic_error,
                      "Missing shape type identifier!");
          const std::string & shtypeid = config_.fetch_string("shape_type_id");
          set_shape_type_id(shtypeid);
        }
        config_.export_starting_with(grab_auxiliaries(), shape_parameter_prefix());
      }

      _initialized_ = true;
      return;
    }

    void base_signal::_reset_()
    {
      _initialized_ = false;
      reset_shape();
      reset_time_ref();
      _shape_type_id_.clear();
      this->geomtools::base_hit::reset();
      return;
    }

    void base_signal::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Signal is not initialized!");
      _reset_();
      return;
    }

    void base_signal::_set_shape_(const mygsl::i_unary_function & shape_, bool owned_)
    {
      if (_shape_ != nullptr) {
        if (_shape_ == &shape_) {
          return;
        }
        _reset_shape_();
      }
      _shape_ = &shape_;
      _owned_shape_ = owned_;
      return;
    }

    const mygsl::i_unary_function & base_signal::_get_shape_()
    {
      if (_shape_ == nullptr) {
        if (has_shape_builder()) {
          // Ask the shape builder to instantiate the shape object:
          DT_LOG_DEBUG(_logging_, "Using external signal shape builder to instantiate the shape object...");
          std::ostringstream key_ss;
          key_ss << "signal_" << get_hit_id();
          if (_shape_builder_->has_functor(key_ss.str())) {
            _shape_builder_->clear_functor(key_ss.str());
          }
          base_signal::build_signal_shape(*_shape_builder_, key_ss.str(), *this, _logging_);
        } else {
          // Fallback : ask the system shape factory registry to instantiate the shape object:
          DT_LOG_DEBUG(_logging_, "Using the system factory register to instantiate the shape object...");
          mygsl::i_unary_function::factory_register_type the_factory_register =
            DATATOOLS_FACTORY_GRAB_SYSTEM_REGISTER(mygsl::i_unary_function);
          DT_THROW_IF(!the_factory_register.has(_shape_type_id_),
                      std::runtime_error,
                      "Shape type identifier '" << _shape_type_id_ << "' is not registered in the system register of unary functions!");
          const mygsl::i_unary_function::factory_register_type::factory_type & the_factory =
            the_factory_register.get(_shape_type_id_);
          // std::cerr << "DEVEL: mctools::signal::base_signal::_grab_shape_: Instantiate the shape function..." << std::endl;
          mygsl::i_unary_function * new_shape = nullptr;
          try {
            DT_LOG_DEBUG(_logging_, "Instantiating the shape functor...");
            new_shape = the_factory();
            if (new_shape != nullptr) {
              datatools::properties shape_config;
              get_auxiliaries().export_and_rename_starting_with(shape_config, shape_parameter_prefix(), "");
              new_shape->initialize_standalone(shape_config);
              _set_shape_(*new_shape, true);
              new_shape = nullptr;
            } else {
              DT_THROW(std::runtime_error, "Factory could not instantiate a signal shape of type '" << _shape_type_id_ << "'!");
            }
          } catch (std::exception & error) {
            if (new_shape != nullptr) {
              delete new_shape;
              _shape_ = nullptr;
              _owned_shape_ = false;
            }
            throw;
          }
        }
      }
      return *_shape_;
    }

    double base_signal::compute_shape(double time_) const
    {
      return get_shape().eval(time_);
    }

    void base_signal::tree_dump(std::ostream & out_,
                                const std::string & title_,
                                const std::string & indent_,
                                bool inherit_) const
    {
      this->geomtools::base_hit::tree_dump(out_, title_, indent_, true);

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Time reference : ";
      if (has_time_ref()) {
        out_ << _time_ref_ / CLHEP::ns << " [ns]";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Shape builder : ";
      if (has_shape_builder()) {
        out_ << "'" << _shape_builder_->get_category() << "'";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Signal shape type ID : ";
      if (has_shape_type_id()) {
        out_ << "'" << _shape_type_id_ << "'";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Initialized : " << ( is_initialized() ? "<yes>" : "<no>" )
           << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Instantiated shape : " << ( is_shape_instantiated() ? "<yes>" : "<not yet>" )
           << std::endl;

      return;
    }

    // static
    bool base_signal::build_signal_shape(signal_shape_builder & builder_,
                                         const std::string & key_,
                                         base_signal & signal_,
                                         const  datatools::logger::priority logging_)
    {
      datatools::logger::priority logging = logging_;
      DT_THROW_IF(!builder_.is_initialized(), std::logic_error, "Signal shape builder is not initialized!");
      DT_THROW_IF(!signal_.is_initialized(), std::logic_error, "Signal shape functor is not initialized!");
      bool success = false;
      try {
        signal_.reset_shape();
        datatools::properties shape_config;
        signal_.get_auxiliaries().export_and_rename_starting_with(shape_config,
                                                                  base_signal::shape_parameter_prefix(),
                                                                  "");
        const mygsl::i_unary_function & fs = builder_.create_signal_shape(key_,
                                                                          signal_.get_shape_type_id(),
                                                                          shape_config);
        signal_.set_shape(fs);
        success = true;
      } catch (std::exception) {
        DT_LOG_ERROR(logging, "Could not build signal shape '" << key_ << "'!");
      }
      return success;
    }

  } // end of namespace signal

} // end of namespace mctools

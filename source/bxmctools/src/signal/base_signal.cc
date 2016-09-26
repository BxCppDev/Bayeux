// base_signal.cc
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2015-04-10
 * Last modified : 2016-09-27
 *
 * Copyright (C) 2015 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
// - Bayeux/mygsl:
#include <mygsl/i_unary_function.h>

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
    {
      return;
    }

    base_signal::~base_signal()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    bool base_signal::is_valid() const
    {
      if (!this->geomtools::base_hit::is_valid()) {
        return false;
      }
      if (!is_shape_instantiated()) {
        if (!has_shape_type_id()) {
          return false;
        }
      }
      return true;
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

    void base_signal::set_shape_boolean_parameter(const std::string & key_,
                                                  const bool value_,
                                                  const std::string & desc_)
    {
      grab_auxiliaries().store_boolean(shape_parameter_prefix() + key_, value_, desc_);
      return;
    }

    void base_signal::set_shape_boolean_parameters(const std::string & key_,
                                                   const std::vector<bool> & values_,
                                                   const std::string & desc_)
    {
      grab_auxiliaries().store(shape_parameter_prefix() + key_, values_, desc_);
      return;
    }

    void base_signal::set_shape_integer_parameter(const std::string & key_,
                                                  const int value_,
                                                  const std::string & desc_)
    {
      grab_auxiliaries().store_integer(shape_parameter_prefix() + key_, value_, desc_);
      return;
    }

    void base_signal::set_shape_integer_parameters(const std::string & key_,
                                                   const std::vector<int> & values_,
                                                   const std::string & desc_)
    {
      grab_auxiliaries().store(shape_parameter_prefix() + key_, values_, desc_);
      return;
    }

    void base_signal::set_shape_string_parameter(const std::string & key_,
                                                 const std::string & value_,
                                                 const std::string & desc_)
    {
      grab_auxiliaries().store_string(shape_parameter_prefix() + key_, value_, desc_);
      return;
    }

    void base_signal::set_shape_string_parameters(const std::string & key_,
                                                  const std::vector<std::string> & values_,
                                                  const std::string & desc_)
    {
      grab_auxiliaries().store(shape_parameter_prefix() + key_, values_, desc_);
      return;
    }

    void base_signal::set_shape_real_parameter(const std::string & key_,
                                               const double value_,
                                               const std::string & desc_)
    {
      grab_auxiliaries().store_real(shape_parameter_prefix() + key_, value_, desc_);
      return;
    }

    void base_signal::set_shape_real_parameters(const std::string & key_,
                                                const std::vector<double> & values_,
                                                const std::string & desc_)
    {
      grab_auxiliaries().store(shape_parameter_prefix() + key_, values_, desc_);
      return;
    }

    void base_signal::set_shape_real_parameter_with_explicit_unit(const std::string & key_,
                                                                  const double value_,
                                                                  const std::string & unit_symbol_,
                                                                  const std::string & desc_)
    {
      grab_auxiliaries().store_with_explicit_unit(shape_parameter_prefix() + key_, value_, desc_);
      grab_auxiliaries().set_unit_symbol(shape_parameter_prefix() + key_, unit_symbol_);
      return;
    }

    void base_signal::set_shape_real_parameters_with_explicit_unit(const std::string & key_,
                                                                   const std::vector<double> & values_,
                                                                   const std::string & unit_symbol_,
                                                                   const std::string & desc_)
    {
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
      return const_cast<mygsl::i_unary_function &>(const_cast<base_signal*>(this)->_grab_shape());
    }

    mygsl::i_unary_function & base_signal::grab_shape()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Signal is not initialized!");
      return _grab_shape();
    }

    void base_signal::reset_shape()
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Signal is initialized and locked!");
      if (_shape_ != nullptr) {
        if (_owned_shape_) {
          delete _shape_;
        }
        _shape_ = nullptr;
        _owned_shape_ = false;
      }
      return;
    }

    void base_signal::set_shape(mygsl::i_unary_function * shape_ptr_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Signal is initialized and locked!");
      if (shape_ptr_ == _shape_) {
        return;
      }
      reset_shape();
      _owned_shape_ = false;
      _shape_ = shape_ptr_;
      return;
    }

    void base_signal::set_shape(mygsl::i_unary_function & shape_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Signal is initialized and locked!");
      if (&shape_ == _shape_) {
        return;
      }
      reset_shape();
      _owned_shape_ = false;
      _shape_ = &shape_;
      return;
    }

    bool base_signal::is_initialized() const
    {
      return _initialized_;
    }

    void base_signal::initialize_simple()
    {
      datatools::properties dummy_config;
      initialize(dummy_config);
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

    void base_signal::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Signal is not initialized!");
      _initialized_ = false;
      reset_shape();

      _shape_type_id_.clear();
      this->geomtools::base_hit::reset();
      return;
    }

    mygsl::i_unary_function & base_signal::_grab_shape()
    {
      if (_shape_ == nullptr) {
        mygsl::i_unary_function::factory_register_type the_factory_register =
          DATATOOLS_FACTORY_GRAB_SYSTEM_REGISTER(mygsl::i_unary_function);
        DT_THROW_IF(!the_factory_register.has(_shape_type_id_),
                    std::runtime_error,
                    "Shape type identifier '" << _shape_type_id_ << "' is not registered in the system register of unary functions!");
        const mygsl::i_unary_function::factory_register_type::factory_type & the_factory =
          the_factory_register.get(_shape_type_id_);
        // std::cerr << "DEVEL: mctools::signal::base_signal::_grab_shape: Instantiate the shape function..." << std::endl;
        _shape_ = the_factory();
        _owned_shape_ = true;
        datatools::properties shape_config;
        get_auxiliaries().export_and_rename_starting_with(shape_config, shape_parameter_prefix(), "");
        _shape_->initialize_standalone(shape_config);
      }
      return *_shape_;
    }

    void base_signal::tree_dump(std::ostream & out_,
                                const std::string & title_,
                                const std::string & indent_,
                                bool inherit_) const
    {
      this->geomtools::base_hit::tree_dump(out_, title_, indent_, true);

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Signal shape type ID : ";
      if (has_shape_type_id()) {
        out_ << "'" << _shape_type_id_ << "'";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Instantiated shape : " << ( is_shape_instantiated() ? "<yes>" : "<no>" )
           << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Initialized : " << ( is_initialized() ? "<yes>" : "<no>" )
           << std::endl;

      return;
    }

  } // end of namespace signal

} // end of namespace mctools

// multi_signal_shape.cc
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2016-11-30
 * Last modified : 2016-11-30
 *
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
#include <mctools/signal/multi_signal_shape.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/utils.h>
#include <datatools/clhep_units.h>
#include <datatools/properties.h>

namespace mctools {

  namespace signal {

    MYGSL_UNARY_FUNCTOR_REGISTRATION_IMPLEMENT(multi_signal_shape,
                                               "mctools::signal::multi_signal_shape")

    DATATOOLS_CLONEABLE_IMPLEMENTATION(multi_signal_shape)

    // Record describing a component signal shape
    struct multi_signal_shape::component_record
    {
      component_record(const std::string & key_,
                       const mygsl::const_unary_function_handle_type & shape_,
                       double time_shift_ = 0.0,
                       double scaling_ = 1.0)
        : _key_(key_)
        , _shape_(shape_)
        , _pshape_(nullptr)
        , _time_shift_(time_shift_)
        , _scaling_(scaling_)
      {
      }
      component_record(const std::string & key_,
                       const mygsl::i_unary_function & shape_,
                       double time_shift_ = 0.0,
                       double scaling_ = 1.0)
        : _key_(key_)
        , _shape_()
        , _pshape_(&shape_)
        , _time_shift_(time_shift_)
        , _scaling_(scaling_)
      {
      }
      const i_unary_function & sh() const
      {
       if (_pshape_) return *_pshape_;
       return _shape_.get();
      }
      const std::string & get_key() const
      {
        return _key_;
      }
      double get_time_shift() const
      {
        return _time_shift_;
      }
      double get_scaling() const
      {
        return _scaling_;
      }
    private:
      std::string _key_;
      mygsl::const_unary_function_handle_type _shape_; //!< Handle to the component shape
      const mygsl::i_unary_function * _pshape_ = nullptr; //!<  Plain pointer to the component shape
      double _time_shift_ = 0.0; //!< Time shift
      double _scaling_ = 1.0; //!< Scaling factor
    };

    void multi_signal_shape::_set_defaults()
    {
      _explicit_domain_of_definition_ = false;
      _non_zero_domain_min_ = -std::numeric_limits<double>::infinity();
      _non_zero_domain_max_ =  std::numeric_limits<double>::infinity();
      return;
    }

    multi_signal_shape::multi_signal_shape()
    {
      _set_defaults();
      return;
    }

    multi_signal_shape::~multi_signal_shape()
    {
      return;
    }

    bool multi_signal_shape::has_explicit_domain_of_definition() const
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Multi signal shape function is not initialized!");
      return _explicit_domain_of_definition_;
    }

    bool multi_signal_shape::is_in_domain_of_definition(double x_) const
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Multi signal shape function is not initialized!");
      bool in_dod = true;
      for (std::size_t i = 0; i < _components_.size(); i++) {
        if (_components_[i].sh().has_explicit_domain_of_definition()) {
          if (!_components_[i].sh().is_in_domain_of_definition(x_ - _components_[i].get_time_shift())) {
            in_dod = false;
            break;
          }
        }
      }
      return in_dod;
    }

    std::size_t multi_signal_shape::get_number_of_components() const
    {
      return _components_.size();
    }

    void multi_signal_shape::remove(const std::size_t index_)
    {
      if (index_ >= _components_.size()) return;
      _components_.erase(_components_.begin() + index_);
      return;
    }

    std::size_t multi_signal_shape::add(const std::string & key_,
                                        const mygsl::const_unary_function_handle_type & shape_,
                                        double time_shift_,
                                        double scaling_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Signal shape is already initialized!");
      DT_THROW_IF(key_.empty(), std::logic_error, "Invalid functor key!");
      DT_THROW_IF(!shape_.get().is_initialized(), std::logic_error, "Component functor is not initialized!");
      DT_THROW_IF(!std::isfinite(time_shift_), std::logic_error, "Invalid component timeshift!");
      DT_THROW_IF(!std::isfinite(scaling_), std::logic_error, "Invalid component timeshift!");
      component_record cr(key_, shape_, time_shift_, scaling_);
      std::size_t pos = _components_.size();
      _components_.push_back(cr);
      _recompute_();
      return pos;
    }

    void multi_signal_shape::_recompute_()
    {
      _explicit_domain_of_definition_ = false;
      datatools::invalidate(_non_zero_domain_min_);
      datatools::invalidate(_non_zero_domain_max_);

      for (std::size_t i = 0; i < _components_.size(); i++) {
        if (_components_[i].sh().has_explicit_domain_of_definition()) {
          _explicit_domain_of_definition_ = true;
          break;
        }
      }

      for (const auto & crec : _components_) {
        if (crec.get_scaling() == 0.0) continue;
        const mygsl::i_unary_function & func = crec.sh();
        if (!func.has_non_zero_domain_min()) {
          // Unable to define a non zero domain min bound because at least one component
          // is not left bounded:
          datatools::invalidate(_non_zero_domain_min_);
          break;
        }
        if (!datatools::is_valid(_non_zero_domain_min_)) {
          _non_zero_domain_min_ = func.get_non_zero_domain_min() + crec.get_time_shift();
        } else if (func.get_non_zero_domain_min() + crec.get_time_shift() > _non_zero_domain_min_) {
          _non_zero_domain_min_ = func.get_non_zero_domain_min() + crec.get_time_shift();
        }
      }
      for (const auto & crec : _components_) {
        if (crec.get_scaling() == 0.0) continue;
        const mygsl::i_unary_function & func = crec.sh();
        if (!func.has_non_zero_domain_max()) {
          // Unable to define a non zero domain min bound because at least one component
          // is not right bounded:
          datatools::invalidate(_non_zero_domain_max_);
          break;
        }
        if (!datatools::is_valid(_non_zero_domain_max_)) {
          _non_zero_domain_max_ = func.get_non_zero_domain_max() + crec.get_time_shift();
        } else if (func.get_non_zero_domain_max() + crec.get_time_shift() < _non_zero_domain_max_) {
          _non_zero_domain_max_ = func.get_non_zero_domain_max() + crec.get_time_shift();
        }
      }
      return;
    }

    bool multi_signal_shape::is_valid() const
    {
      return _components_.size() > 0;
    }

    bool multi_signal_shape::is_initialized() const
    {
      return _initialized_;
    }

    void multi_signal_shape::initialize(const datatools::properties & config_,
                                        const mygsl::unary_function_dict_type & functors_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Multi signal shape is already initialized!");
      this->mygsl::i_unary_function::_base_initialize(config_, functors_);

      static const double the_default_time_unit = mctools::signal::default_time_unit().get_value();
      static const double the_default_voltage_unit = mctools::signal::default_voltage_unit().get_value();

      std::set<std::string> comp_labels;
      if (config_.has_key("components")) {
        config_.fetch("components", comp_labels);
      }

      for (const auto & comp_label : comp_labels) {
        std::string comp_key;
        double comp_timeshift = 0.0;
        double comp_scaling = 1.0;
        {
          std::ostringstream comp_key_oss;
          comp_key_oss << "components." << comp_label << ".key";
          DT_THROW_IF(!config_.has_key(comp_key_oss.str()), std::logic_error,
                      "Missing '" << comp_key_oss.str() << "' property!");
          comp_key = config_.fetch_string(comp_key_oss.str());
        }
        {
          std::ostringstream comp_timeshift_oss;
          comp_timeshift_oss << "components." << comp_label << ".time_shift";
          if (config_.has_key(comp_timeshift_oss.str())) {
            comp_timeshift = config_.fetch_real_with_explicit_dimension(comp_timeshift_oss.str(),
                                                                        "time");
          }
        }
        {
          std::ostringstream comp_scaling_oss;
          comp_scaling_oss << "components." << comp_label << ".scaling";
          if (config_.has_key(comp_scaling_oss.str())) {
            comp_scaling = config_.fetch_dimensionless_real(comp_scaling_oss.str());
          }
        }
        DT_THROW_IF(!functors_.count(comp_key), std::domain_error,
                    "No component signal shape functor with key '"
                    << comp_key << "' is available from the pool of functors!");
        mygsl::const_unary_function_handle_type ch = functors_.find(comp_key)->second.to_const();
        add(comp_key, ch /*functors_.find(comp_key)->second*/, comp_timeshift, comp_scaling);
      }

      _init_();

      return;
    }

    void multi_signal_shape::_init_()
    {
      if (is_initialized()) return;
      DT_THROW_IF(!is_valid(), std::logic_error, "No component signal shape functors!");
      _initialized_ = true;
      return;
    }

    void multi_signal_shape::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Multi signal shape is not initialized!");
      _reset_();
      return;
    }

    void multi_signal_shape::_reset_()
    {
      if (!is_initialized()) return;
      DT_THROW_IF(!is_valid(), std::logic_error, "No component signal shape functors!");
      _initialized_ = false;
      _components_.clear();
      _set_defaults();
      this->i_unary_function::reset();
      return;
    }

    // virtual
    double multi_signal_shape::get_non_zero_domain_min() const
    {
      return _non_zero_domain_min_;
    }

    // virtual
    double multi_signal_shape::get_non_zero_domain_max() const
    {
      return _non_zero_domain_max_;
    }

    double multi_signal_shape::_eval(double x_) const
    {
      double res = 0.0;
      for (const auto & fcomp : _components_) {
        double val = fcomp.get_scaling() * fcomp.sh().eval(x_ - fcomp.get_time_shift());
        res += val;
      }
      return res;
    }

    void multi_signal_shape::tree_dump(std::ostream & out_,
                                       const std::string & title_,
                                       const std::string & indent_,
                                       bool inherit_) const
    {
      this->i_unary_function::tree_dump(out_, title_, indent_, true);

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Components: ";
      if (_components_.size()) {
        out_ << '[' << _components_.size() << ']';
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;
      {
        std::size_t count = 0;
        for (const auto & f : _components_) {
          out_ << indent_ << datatools::i_tree_dumpable::skip_tag;
          if (++count == _components_.size()) {
            out_ << datatools::i_tree_dumpable::last_tag;
          } else {
            out_ << datatools::i_tree_dumpable::tag;
          }
          out_ << "Components functor : '" << f.get_key() << "' shifted by "
               << f.get_time_shift() / CLHEP::ns << " ns ans scaled by factor " << f.get_scaling();
          out_ << std::endl;
        }
      }

      return;
    }

  } // end of namespace signal

} // end of namespace mctools

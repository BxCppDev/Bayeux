// Ourselves:
#include <mygsl/rectangular_function.h>

// Standard library:
#include <limits>
#include <cmath>

// Third party:
// - Bayeux/datatools :
#include <datatools/exception.h>

namespace mygsl {

  MYGSL_UNARY_FUNCTOR_REGISTRATION_IMPLEMENT(rectangular_function,
                                             "mygsl::rectangular_function")

  DATATOOLS_CLONEABLE_IMPLEMENTATION(rectangular_function)

  void rectangular_function::_set_defaults()
  {
    _convention_ = CONVENTION_INVALID;
    return;
  }

  rectangular_function::rectangular_function(convention_type convention_)
  {
    _set_defaults();
    set_convention(convention_);
    return;
  }

  rectangular_function::~rectangular_function()
  {
    return;
  }

  void rectangular_function::set_convention(convention_type convention_)
  {
    _convention_ = convention_;
    return;
  }

  rectangular_function::convention_type rectangular_function::get_convention() const
  {
    return _convention_;
  }

  bool rectangular_function::is_initialized() const
  {
    if (_convention_ == CONVENTION_INVALID) return false;
    return true;
  }

  void rectangular_function::initialize(const datatools::properties & config_,
                                        unary_function_dict_type & functors_)
  {
    // Parse configuration:
    this->i_unary_function::_base_initialize(config_, functors_);

    if (_convention_ == CONVENTION_INVALID) {
      if (config_.has_key("convention")) {
        const std::string & conv = config_.fetch_string("convention");
        if (conv == "half_maximum") {
          set_convention(CONVENTION_HALF_MAXIMUM);
        } else if (conv == "inner_continuous") {
          set_convention(CONVENTION_INNER_CONTINUOUS);
        } else if (conv == "outer_continuous") {
          set_convention(CONVENTION_OUTER_CONTINUOUS);
        } else if (conv == "continuous") {
          set_convention(CONVENTION_CONTINUOUS);
        } else {
          DT_THROW(std::logic_error, "Invalid convention '" << conv << "'!");
        }
      }
    }

    if (_convention_ == CONVENTION_INVALID) {
      set_convention(CONVENTION_HALF_MAXIMUM);
    }

    return;
  }

  void rectangular_function::reset()
  {
    _set_defaults();
    this->i_unary_function::_base_reset();
    return;
  }

  double rectangular_function::get_non_zero_domain_min() const
  {
   if (_convention_ == CONVENTION_OUTER_CONTINUOUS) return -0.5 + get_epsilon();
    return -0.5 - get_epsilon();
  }

  double rectangular_function::get_non_zero_domain_max() const
  {
   if (_convention_ == CONVENTION_OUTER_CONTINUOUS) return +0.5 - get_epsilon();
    return +0.5 + get_epsilon();
  }

  double rectangular_function::_eval(double x_) const
  {
    double eps = get_epsilon();
    double res = 0.0;
    double absx = std::abs(x_);
    if (_convention_ == CONVENTION_HALF_MAXIMUM) {
      if (absx < 0.5 - eps) res = 1.0;
      else if (absx < 0.5 + eps) res = 0.5;
    } else if (_convention_ == CONVENTION_INNER_CONTINUOUS) {
      if (absx < 0.5 + eps) res = 1.0;
    } else if (_convention_ == CONVENTION_OUTER_CONTINUOUS) {
      if (absx < 0.5 - eps) res = 1.0;
    } else if (_convention_ == CONVENTION_CONTINUOUS) {
      if (absx < 0.5 - eps) res = 1.0;
      else if (absx < 0.5 + eps) {
        res = (0.5 + eps - absx) / (2 * eps);
      }
    }
    return res;
  }

  void rectangular_function::tree_dump(std::ostream & out_,
                                    const std::string & title_,
                                    const std::string & indent_,
                                    bool inherit_) const
  {
    this->i_unary_function::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Convention : " << _convention_ << std::endl;

    return;
  }

} // namespace mygsl

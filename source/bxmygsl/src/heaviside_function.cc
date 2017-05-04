// Standard library:
#include <limits>

// This project:
#include <mygsl/heaviside_function.h>

namespace mygsl {

  MYGSL_UNARY_FUNCTOR_REGISTRATION_IMPLEMENT(heaviside_function,
                                             "mygsl::heaviside_function")

  DATATOOLS_CLONEABLE_IMPLEMENTATION(heaviside_function)

  void heaviside_function::_set_defaults()
  {
    _convention_ = CONVENTION_INVALID;
    return;
  }

  heaviside_function::heaviside_function(convention_type convention_)
  {
    _set_defaults();
    set_convention(convention_);
    return;
  }

  heaviside_function::~heaviside_function()
  {
    return;
  }

  void heaviside_function::set_convention(convention_type convention_)
  {
    _convention_ = convention_;
    return;
  }

  heaviside_function::convention_type heaviside_function::get_convention() const
  {
    return _convention_;
  }

  bool heaviside_function::is_initialized() const
  {
    if (_convention_ == CONVENTION_INVALID) return false;
    return true;
  }

  void heaviside_function::initialize(const datatools::properties & config_,
                                      const unary_function_dict_type & functors_)
  {
    // Parse configuration:
    this->i_unary_function::_base_initialize(config_, functors_);

    if (_convention_ == CONVENTION_INVALID) {
      if (config_.has_key("convention")) {
        const std::string & conv = config_.fetch_string("convention");
        if (conv == "half_maximum") {
          set_convention(CONVENTION_HALF_MAXIMUM);
        } else if (conv == "right_continuous") {
          set_convention(CONVENTION_RIGHT_CONTINUOUS);
        } else if (conv == "left_continuous") {
          set_convention(CONVENTION_LEFT_CONTINUOUS);
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

  void heaviside_function::reset()
  {
    _set_defaults();
    this->i_unary_function::_base_reset();
    return;
  }

  double heaviside_function::get_non_zero_domain_min() const
  {
    if (_convention_ == CONVENTION_LEFT_CONTINUOUS) return +get_epsilon();
    return -get_epsilon();
  }

  double heaviside_function::_eval(double x_) const
  {
    double eps = get_epsilon();
    if (x_ < -eps) return 0.0;
    else if (x_ > eps) return 1.0;
    else {
      if (_convention_ == CONVENTION_HALF_MAXIMUM) return 0.5;
      else if (_convention_ == CONVENTION_RIGHT_CONTINUOUS) return 1.0;
      else if (_convention_ == CONVENTION_LEFT_CONTINUOUS) return 0.0;
      else if (_convention_ == CONVENTION_CONTINUOUS) {
        return 0.5 * ( 1.0 + x_ / eps);
      }
    }
    return 0.0;
  }

  void heaviside_function::tree_dump(std::ostream & out_,
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

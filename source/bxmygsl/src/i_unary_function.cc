/// i_unary_function.cc

// Ourselves:
#include <mygsl/i_unary_function.h>

// Standard library:
#include <cmath>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/ioutils.h>
#include <datatools/utils.h>
#include <datatools/units.h>
#include <datatools/exception.h>
#include <datatools/properties.h>

namespace mygsl {

  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(i_unary_function,
                                                   "mygsl::i_unary_function/__system__")

  const double i_unary_function::DEFAULT_EPSILON = 1.0e-7;
  const double i_unary_function::AUTO_EPSILON    = 0.0;

  void i_unary_function::_set_defaults()
  {
    datatools::invalidate(_epsilon_);
    return;
  }

  void i_unary_function::set_epsilon(double eps_)
  {
    if (eps_ <= 0.0) {
      _epsilon_ = 0.0;
    } else {
      _epsilon_ = eps_;
    }
    return;
  }

  void i_unary_function::_compute_auto_epsilon()
  {
    if (_epsilon_ != AUTO_EPSILON) return;
    if (has_non_zero_domain_min() && has_non_zero_domain_max()) {
      double dx = get_non_zero_domain_max() - get_non_zero_domain_min();
      _epsilon_ = dx / 1.e7;
      return;
    }
    if (!has_non_zero_domain_min() && has_non_zero_domain_max()) {
      double dx = std::abs(get_non_zero_domain_max());
      _epsilon_ = dx / 1.e7;
      return;
    }
    if (has_non_zero_domain_min() && !has_non_zero_domain_max()) {
      double dx = std::abs(get_non_zero_domain_min());
      _epsilon_ = dx / 1.e7;
      return;
    }
    _epsilon_ = DEFAULT_EPSILON;
    return;
  }

  double i_unary_function::get_epsilon() const
  {
    if (_epsilon_ == AUTO_EPSILON) {
      const_cast<i_unary_function *>(this)->_compute_auto_epsilon();
    }
    return _epsilon_;
  }

  i_unary_function::i_unary_function(double epsilon_)
  {
    _set_defaults();
    set_epsilon(epsilon_);
    return;
  }

  i_unary_function::~i_unary_function ()
  {
    return;
  }

  void i_unary_function::_base_initialize(const datatools::properties & config_,
                                          unary_function_dict_type & /*functors_*/)
  {
    if (! datatools::is_valid(_epsilon_)) {
      if (config_.has_key("epsilon")) {
        double eps = config_.fetch_real("epsilon");
        DT_THROW_IF(eps < 0.0, std::domain_error, "Invalid value for precision !");
        set_epsilon(eps);
      }
    }

    if (! datatools::is_valid(_epsilon_)) {
      _epsilon_ = AUTO_EPSILON;
    }

    return;
  }

  void i_unary_function::_base_reset()
  {
    _set_defaults();
    return;
  }

  bool i_unary_function::is_initialized() const
  {
    return datatools::is_valid(_epsilon_);
  }

  void i_unary_function::initialize_simple()
  {
    datatools::properties dummy_config;
    initialize_standalone(dummy_config);
    return;
  }

  void i_unary_function::initialize_standalone(const datatools::properties & config_)
  {
    unary_function_dict_type dummy_dict;
    initialize(config_, dummy_dict);
    return;
  }

  void i_unary_function::initialize(const datatools::properties & config_,
                                    unary_function_dict_type & functors_)
  {
    _base_initialize(config_, functors_);

    return;
  }

  void i_unary_function::reset()
  {
    _base_reset();
    return;
  }

  bool i_unary_function::has_explicit_domain_of_definition() const
  {
    return false;
  }

  bool i_unary_function::is_in_domain_of_definition(double /* x_ */) const
  {
    return true;
  }

  double i_unary_function::get_non_zero_domain_min() const
  {
    return -std::numeric_limits<double>::infinity();
    //return std::numeric_limits<double>::quiet_NaN();
  }

  bool i_unary_function::has_non_zero_domain_min() const
  {
    double nzdmin = get_non_zero_domain_min();
    if (!datatools::is_valid(nzdmin)) return false;
    return nzdmin != -std::numeric_limits<double>::infinity();
  }

  double i_unary_function::get_non_zero_domain_max() const
  {
    return +std::numeric_limits<double>::infinity();
    //return std::numeric_limits<double>::quiet_NaN();
  }

  bool i_unary_function::has_non_zero_domain_max() const
  {
    double nzdmax = get_non_zero_domain_max();
    if (!datatools::is_valid(nzdmax)) return false;
    return nzdmax != +std::numeric_limits<double>::infinity();
  }

  bool i_unary_function::is_in_non_zero_domain(double x_) const
  {
    if (x_ < get_non_zero_domain_min()) return false;
    if (x_ > get_non_zero_domain_max()) return false;
    return true;
  }

  bool i_unary_function::is_in_zero_domain(double x_) const
  {
    return !is_in_non_zero_domain(x_);
  }

  bool i_unary_function::has_zero_domain() const
  {
    if (has_non_zero_domain_min()) return true;
    if (has_non_zero_domain_max()) return true;
    return false;
  }

  double i_unary_function::eval_no_check(double x_) const
  {
    return _eval(x_);
  }

  double i_unary_function::evaluate(double x_) const
  {
    return this->eval(x_);
  }

  double i_unary_function::eval(double x_) const
  {
    if (has_explicit_domain_of_definition()) {
      if (! is_in_domain_of_definition(x_)) {
        DT_THROW(std::logic_error,
                 "Argument '" << x_ << "' is not in the function's domain of defintiion!");
        return std::numeric_limits<double>::quiet_NaN();
      }
    }
    if (! is_in_non_zero_domain(x_)) return 0.0;
    return eval_no_check(x_);
  }

  double i_unary_function::operator() (double x_) const
  {
    return this->eval(x_);
  }

  void i_unary_function::write_ascii_with_units(std::ostream & out_,
                                                double min_, double max_, unsigned int nsamples_,
                                                double x_unit_,
                                                double fx_unit_,
                                                int x_precision_,
                                                int fx_precision_,
                                                uint32_t options_) const
  {
    // std::cerr << "DEVEL: i_unary_function::write_ascii_with_units: fx_unit_ = " << fx_unit_ << std::endl;
    int xprecision = x_precision_;
    if (xprecision <= 0) xprecision = datatools::io::REAL_PRECISION;
    int fxprecision = fx_precision_;
    if (fxprecision <= 0) fxprecision = datatools::io::REAL_PRECISION;
    DT_THROW_IF (! out_,  std::runtime_error, "Cannot write in the output stream !");
    double dx = (max_ - min_) / nsamples_;
    for (size_t i = 0; i < nsamples_; i++) {
      double x = min_ + i * dx;
      if (options_ & wo_skip_odod) {
        if (this->has_explicit_domain_of_definition()) {
          if (! this->is_in_domain_of_definition(x)) {
            continue;
          }
        }
      }
      if (options_ & wo_skip_onzd) {
        if (this->is_in_zero_domain(x)) {
          continue;
        }
      }
      double y = this->eval(x);
      if (std::isnan(y)) {
        if (options_ & wo_skip_nan) continue;
      }
      if (std::isinf(y)) {
        if (options_ & wo_skip_inf) continue;
      }
      datatools::io::write_real_number(out_, x / x_unit_, xprecision);
      out_ << ' ';
      // if (std::abs(y) > 1e-40) {
      //   std::cerr << "DEVEL: i_unary_function::write_ascii_with_units: y      = " << y  << std::endl;
      //   std::cerr << "DEVEL: i_unary_function::write_ascii_with_units: y(out) = " << y / fx_unit_  << std::endl;
      // }
      datatools::io::write_real_number(out_, y / fx_unit_, fxprecision);
      out_ << '\n';
    }
    if (options_ & wo_data_index) out_ << '\n' << '\n';
    return;
  }


  void i_unary_function::write_ascii(std::ostream & out_,
                                     double min_, double max_,
                                     unsigned int nsamples_,
                                     int x_precision_,
                                     int fx_precision_,
                                     uint32_t options_) const
  {
    write_ascii_with_units(out_, min_, max_, nsamples_,
                           1.0, 1.0,
                           x_precision_, fx_precision_,
                           options_);
    return;
  }


  void i_unary_function::write_ascii_file_with_units(const std::string & filename_,
                                                     double min_, double max_, unsigned int nsamples_,
                                                     const std::string & x_unit_label_,
                                                     const std::string & fx_unit_label_,
                                                     int x_precision_,
                                                     int fx_precision_,
                                                     uint32_t options_) const
  {
    std::string filename = filename_;
    datatools::fetch_path_with_env(filename);
    std::ofstream fout;
    std::ios_base::openmode flags = std::ios_base::out;
    if (options_ & wo_append) flags |= std::ios::app;
    fout.open(filename.c_str(), flags);
    DT_THROW_IF (! fout, std::runtime_error, "Cannot open output file '" << filename << "' !");
    double x_unit = 1.0;
    double fx_unit = 1.0;
    if (!x_unit_label_.empty()) {
      x_unit = datatools::units::get_unit(x_unit_label_);
      fout << "#@x_unit=" << x_unit_label_ << std::endl;
      // fout << "#@x_unit.value=" << x_unit << std::endl;
    }
    if (!fx_unit_label_.empty()) {
      fx_unit = datatools::units::get_unit(fx_unit_label_);
      fout << "#@fx_unit=" << fx_unit_label_ << std::endl;
      // fout << "#@fx_unit.value=" << fx_unit << std::endl;
    }
    fout << "#@x_min=" << min_ / x_unit << ' ' << x_unit_label_ << std::endl;
    fout << "#@x_max=" << max_ / x_unit << ' ' << x_unit_label_ << std::endl;
    fout << "#@nsamples=" << nsamples_ << std::endl;
    write_ascii_with_units(fout, min_, max_, nsamples_, x_unit, fx_unit, x_precision_, fx_precision_, options_);
    fout.close();
    return;
  }

  void i_unary_function::write_ascii_file(const std::string & filename_,
                                          double min_, double max_, unsigned int  nsamples_,
                                          int x_precision_,
                                          int fx_precision_,
                                          uint32_t options_) const
  {
    write_ascii_file_with_units(filename_,  min_, max_, nsamples_, "", "", x_precision_, fx_precision_, options_);

    // std::string filename = filename_;
    // datatools::fetch_path_with_env(filename);
    // std::ofstream fout;
    // std::ios_base::openmode flags = std::ios_base::out;
    // if (options_ & wo_append) flags |= std::ios::app;
    // fout.open(filename.c_str(), flags);
    // DT_THROW_IF (! fout, std::runtime_error, "Cannot open output file '" << filename << "' !");
    // write_ascii(fout, min_, max_, nsamples_, x_precision_, fx_precision_, options_);
    // fout.close();
    return;
  }

  void i_unary_function::tree_dump(std::ostream & out_,
                                   const std::string & title_,
                                   const std::string & indent_,
                                   bool inherit_) const
  {
    if (! title_.empty()) out_ << indent_ << title_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Epsilon : " << _epsilon_ << std::endl;

    if (is_initialized()) {
      out_ << indent_ << i_tree_dumpable::tag
           << "Explicit domain of definition : " << has_explicit_domain_of_definition() << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Non zero domain minimum: ";
      if (has_non_zero_domain_min()) {
        out_ << get_non_zero_domain_min();
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Non zero domain maximum: ";
      if (has_non_zero_domain_max()) {
        out_ << get_non_zero_domain_max();
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;
    }

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Initialized : " << is_initialized() << std::endl;

    return;
  }

  // static
  double i_unary_function::g_function(double x_, void * functor_)
  {
    const i_unary_function * f = static_cast<const i_unary_function *>(functor_);
    return f->eval(x_);
  }

  MYGSL_UNARY_FUNCTOR_REGISTRATION_IMPLEMENT(identity_function,
                                             "mygsl::identity_function")

  identity_function::identity_function()
  {
    return;
  }

  identity_function::~identity_function()
  {
    return;
  }

  double identity_function::_eval(double x_) const
  {
    return x_;
  }

} // namespace mygsl

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

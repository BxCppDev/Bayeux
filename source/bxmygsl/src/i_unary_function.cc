// -*- mode:c++; -*-
// i_unary_function.cc

#include <mygsl/i_unary_function.h>

#include <cmath>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <datatools/ioutils.h>
#include <datatools/utils.h>
#include <datatools/exception.h>

namespace mygsl {

  const double i_unary_function::DEFAULT_EPSILON = 1.0e-7;
  const double i_unary_function::AUTO_EPSILON    = 0.0;

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
      _epsilon_ = dx / 10000;
    } else if (!has_non_zero_domain_min() && has_non_zero_domain_max()) {
      double dx = get_non_zero_domain_max() - get_non_zero_domain_min();
      _epsilon_ = dx / 10000;
    } else {
      _epsilon_ = DEFAULT_EPSILON;
    }
   return;
  }

  double i_unary_function::get_epsilon() const
  {
    if (_epsilon_ == AUTO_EPSILON) {
      //i_unary_function * mutable_this = const_cast<i_unary_function *>(this);
      //mutable_this->_compute_auto_epsilon();
      const_cast<i_unary_function *>(this)->_compute_auto_epsilon();
    }
    return _epsilon_;
  }

  i_unary_function::i_unary_function(double epsilon_)
  {
    _epsilon_ = AUTO_EPSILON;
    set_epsilon(epsilon_);
    return;
  }

  i_unary_function::~i_unary_function ()
  {
    return;
  }

  bool i_unary_function::has_explicit_domain_of_definition() const
  {
    return false;
  }

  bool i_unary_function::is_in_domain_of_definition(double x_) const
  {
    return true;
  }

  double i_unary_function::get_non_zero_domain_min() const
  {
    return -std::numeric_limits<double>::infinity();
  }

  bool i_unary_function::has_non_zero_domain_min() const
  {
    return get_non_zero_domain_min() != -std::numeric_limits<double>::infinity();
  }

  double i_unary_function::get_non_zero_domain_max() const
  {
    return +std::numeric_limits<double>::infinity();
  }

  bool i_unary_function::has_non_zero_domain_max() const
  {
    return get_non_zero_domain_max() != +std::numeric_limits<double>::infinity();
  }

  /// Check if a value is in the non-zero domain
  bool i_unary_function::is_in_non_zero_domain(double x_) const
  {
    if (x_ < get_non_zero_domain_min()) return false;
    if (x_ > get_non_zero_domain_max()) return false;
    return true;
  }

  /// Check if a value is in the zero domain
  bool i_unary_function::is_in_zero_domain(double x_) const
  {
    return !is_in_non_zero_domain(x_);
  }

  /// The function evaluation method
  double i_unary_function::eval_no_check(double x_) const
  {
    return _eval(x_);
  }

  /// The function evaluation method
  double i_unary_function::evaluate(double x_) const
  {
    return this->eval(x_);
  }

  /// The function evaluation method
  double i_unary_function::eval(double x_) const
  {
    if (has_explicit_domain_of_definition()) {
      if (! is_in_domain_of_definition(x_)) {
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

  /// Write the (x,y=f(x)) value pairs in an ASCII stream :
  void i_unary_function::write_ascii(std::ostream & out_,
                               double min_, double max_,
                               unsigned int nsamples_,
                               int x_precision_,
                               int fx_precision_,
                               uint32_t options_) const
  {
    int xprecision = x_precision_;
    if (xprecision <= 0) xprecision=datatools::io::REAL_PRECISION;
    int fxprecision = fx_precision_;
    if (fxprecision <= 0) fxprecision=datatools::io::REAL_PRECISION;
    DT_THROW_IF (! out_,  std::runtime_error, "Cannot write in the output stream !");
    double dx = (max_ - min_) / nsamples_;
    double x = min_;
    for (int i = 0; i < nsamples_; i++) {
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
      if (isnan(y)) {
        if (options_ & wo_skip_nan) continue;
      }
      if (isinf(y)) {
        if (options_ & wo_skip_inf) continue;
      }
      datatools::io::write_real_number(out_, x, xprecision);
      out_ << ' ';
      datatools::io::write_real_number(out_, y, fxprecision);
      out_ << '\n';
    }
    if (options_ & wo_data_index) out_ << '\n' << '\n';
    return;
  }

  /// Write the (x,y=f(x)) ASCII :
  void i_unary_function::write_ascii_file(const std::string & filename_,
                                    double min_, double max_, unsigned int  nsamples_,
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
    DT_THROW_IF (! fout,std::runtime_error, "Cannot open output file '" << filename << "' !");
    write_ascii(fout, min_, max_, nsamples_, x_precision_, fx_precision_, options_);
    fout.close();
    return;
  }

  // static
  double i_unary_function::g_function(double x_, void * functor_)
  {
    const i_unary_function * f = static_cast<const i_unary_function *>(functor_);
    return f->eval(x_);
  }

  /***********************************************************/

  void plain_function_wrapper::set_plain_function(const plain_function_type & func_)
  {
    _plain_function_ = &func_;
    return;
  }

  plain_function_wrapper::plain_function_wrapper(const plain_function_type & func_)
  {
    set_plain_function(func_);
    return;
  }

  plain_function_wrapper::~plain_function_wrapper()
  {
    _plain_function_ = 0;
    return;
  }

  double plain_function_wrapper::_eval(double x_) const
  {
    return (*_plain_function_)(x_);
  }

  /***********************************************************/


  void function_with_domain::set_domain_of_definition(const interval & domain_)
  {
    _domain_of_definition_ = domain_;
    return;
  }

  const interval & function_with_domain::get_domain_of_definition() const
  {
    return _domain_of_definition_;
  }

  function_with_domain::function_with_domain(const i_unary_function & functor_,
                                             const interval & domain_)
  {
    _functor_ = &functor_;
    set_domain_of_definition(domain_);
    return;
  }

  function_with_domain::function_with_domain(const i_unary_function & functor_)
  {
    _functor_ = &functor_;
    _domain_of_definition_ = interval::make_no_limit ();
    return;
  }

  function_with_domain::~function_with_domain()
  {
    return;
  }

  bool function_with_domain::has_explicit_domain_of_definition() const
  {
    return ! _domain_of_definition_.is_no_limit();
  }

  bool function_with_domain::is_in_domain_of_definition(double x_) const
  {
    return _domain_of_definition_.is_in (x_);
  }

  double function_with_domain::_eval(double x_) const
  {
    return _functor_->eval(x_);
  }

} // namespace mygsl

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

// end of i_unary_function.cc

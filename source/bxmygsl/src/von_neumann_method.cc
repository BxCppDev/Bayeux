// von_neumann_method.cc

// Ourselves:
#include <mygsl/von_neumann_method.h>

// Standard Library:
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <limits>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

namespace mygsl {

  using namespace std;

  // static
  const double von_neumann_method::AUTO_FMAX = 0.0;
  const size_t von_neumann_method::DEFAULT_NSAMPLES;
  const size_t von_neumann_method::MIN_NSAMPLES;
  const size_t von_neumann_method::DEFAULT_MAX_COUNTS;
  const size_t von_neumann_method::NO_MAX_COUNTS;

  bool von_neumann_method::is_initialized() const
  {
    return _func_ != 0;
  }

  double von_neumann_method::get_xmin() const
  {
    return _xmin_;
  }

  double von_neumann_method::get_xmax() const
  {
    return _xmax_;
  }

  double von_neumann_method::get_fmax() const
  {
    return _fmax_;
  }

  size_t von_neumann_method::get_max_counts() const
  {
    return _max_counts_;
  }

  von_neumann_method::von_neumann_method()
  {
    _xmin_ = std::numeric_limits<double>::quiet_NaN();
    _xmax_ = std::numeric_limits<double>::quiet_NaN();
    _fmax_ = std::numeric_limits<double>::quiet_NaN();
    _func_ = 0;
    _max_counts_ = DEFAULT_MAX_COUNTS;
    return;
  }

  von_neumann_method::von_neumann_method(double xmin_,
                                         double xmax_,
                                         const i_unary_function & func_,
                                         double fmax_,
                                         size_t nsamples_,
                                         size_t max_counts_)
  {
    _xmin_ = std::numeric_limits<double>::quiet_NaN();
    _xmax_ = std::numeric_limits<double>::quiet_NaN();
    _fmax_ = std::numeric_limits<double>::quiet_NaN();
    _func_ = 0;
    _max_counts_ = DEFAULT_MAX_COUNTS;
    init(xmin_, xmax_, func_, fmax_, nsamples_, max_counts_);
    return;
  }

  void von_neumann_method::dump(std::ostream & out_) const
  {
    out_ << "mygsl::von_neumann_method:" << std::endl;
    out_ << "|-- x(min)      : " << _xmin_ << std::endl;
    out_ << "|-- x(max)      : " << _xmax_ << std::endl;
    out_ << "|-- f(max)      : " << _fmax_ << std::endl;
    out_ << "|-- func        : " << std::hex << _func_
         << std::dec << std::endl;
    out_ << "`-- counts(max) : " << _max_counts_ << std::endl;
    return;
  }

  void von_neumann_method::reset()
  {
    if (! is_initialized()) return;
    _xmin_ = std::numeric_limits<double>::quiet_NaN();
    _xmax_ = std::numeric_limits<double>::quiet_NaN();
    _fmax_ = std::numeric_limits<double>::quiet_NaN();
    _func_ = 0;
    _max_counts_ = DEFAULT_MAX_COUNTS;
    return;
  }

  void von_neumann_method::init(double xmin_,
                                double xmax_,
                                const i_unary_function & func_,
                                double fmax_,
                                size_t nsamples_,
                                size_t max_counts_)
  {
    initialize(xmin_, xmax_, func_, fmax_, nsamples_, max_counts_);
    return;
  }

  void von_neumann_method::initialize(double xmin_,
                                      double xmax_,
                                      const i_unary_function & func_,
                                      double fmax_,
                                      size_t nsamples_,
                                      size_t max_counts_)
  {
    // using namespace std;
    DT_THROW_IF(is_initialized(), std::logic_error, "Method is already initalized !");
    DT_THROW_IF(xmin_ > xmax_, std::domain_error, "Invalid range!");
    _xmin_ = xmin_;
    _xmax_ = xmax_;
    DT_THROW_IF(max_counts_ != NO_MAX_COUNTS && max_counts_ < 10,
                std::domain_error,
                "Invalid maximum number of tries!");
    _max_counts_ = max_counts_;
    _func_ = &func_;
    if (fmax_ > AUTO_FMAX) {
      _fmax_ = fmax_;
    } else {
      double fmax = -1.0;
      double f1 = -1.0;
      double f2 = -1.0;
      double dfmax = -1.0;
      DT_THROW_IF(nsamples_ < MIN_NSAMPLES, std::domain_error, "Invalid sampling  !");
      double dx =(_xmax_ - _xmin_) / nsamples_;
      for (double x = _xmin_;
           x <  (_xmax_ + 0.5 *dx);
           x += dx) {
        double f = (*_func_)(x);
        if (f1 < 0.0) {
          f1 = f;
        } else {
          f1 = f2;
        }
        f2 = f;
        if (std::abs(f) > fmax) {
          fmax = std::abs(f);
          double df = (f2 - f1) / dx;
          if (std::abs(df) > dfmax) dfmax = std::abs(df);
        }
      }
      _fmax_ = fmax + dfmax * 2. * dx;
    }
    return;
  }

  double von_neumann_method::shoot(rng & ran_)
  {
    double res;
    size_t counts = 0;
    while (true) {
      double x = ran_.flat(_xmin_, _xmax_);
      double y = ran_.flat(0.0, _fmax_);
      counts++;
      double f = (*_func_)(x);
      if (y < f) {
        res = x;
        break;
      }
      DT_THROW_IF((_max_counts_ != NO_MAX_COUNTS) && (counts > _max_counts_),
                  std::domain_error, "Maximum number of tries has been reached!");
    }
    return res;
  }

}

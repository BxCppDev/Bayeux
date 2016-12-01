// mygsl/polynomial.cc

// Ourselves:
#include <mygsl/polynomial.h>

// Standard library:
#include <stdexcept>
#include <sstream>
#include <cmath> // for std::abs
#include <complex> // for std::abs

// Third party:
// - GSL:
#include <gsl/gsl_errno.h>
#include <gsl/gsl_fit.h>
#include <gsl/gsl_poly.h>
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/utils.h>
#include <datatools/properties.h>

namespace mygsl {

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(polynomial, "mygsl::polynomial")

  MYGSL_UNARY_FUNCTOR_REGISTRATION_IMPLEMENT(polynomial,
                                             "mygsl::polynomial")

  DATATOOLS_CLONEABLE_IMPLEMENTATION(polynomial)

  bool polynomial::is_valid() const {
    return _c_.size() > 0;
  }

  unsigned int polynomial::get_ncoeffs() const {
    return _c_.size();
  }

  unsigned int polynomial::get_degree() const {
    return get_ncoeffs() - 1;
  }

  double polynomial::get_coeff(unsigned int i_) const {
    return _c_.at(i_);
  }

  void polynomial::set_coeff(unsigned int i_, double c_) {
    if (i_ > get_degree()) {
      unsigned int sz = _c_.size();
      _c_.resize(i_ + 1);
      for (size_t i = sz; i < _c_.size(); ++i) {
        _c_[i] = 0.0;
      }
    }
    _c_.at(i_) = c_;
  }

  polynomial::polynomial() {
  }

  polynomial::polynomial(unsigned int degree_) {
    _c_.assign(degree_ + 1, 0.0);
    _c_[degree_] = 1.0;
  }

  polynomial::polynomial(double c0_) {
    _c_.assign(1, c0_);
  }

  polynomial::polynomial(double c0_, double c1_) {
    _c_.assign(2, 0.0);
    _c_[0] = c0_;
    _c_[1] = c1_;
  }

  polynomial::polynomial(double c0_, double c1_, double c2_) {
    _c_.assign(3, 0.0);
    _c_[0] = c0_;
    _c_[1] = c1_;
    _c_[2] = c2_;
  }

  polynomial::polynomial(const std::vector<double>& c_) {
    set_coefficients(c_);
  }

  polynomial::polynomial(const polynomial& p_) : i_unary_function(p_) {
    _c_ = p_._c_;
  }

  polynomial::~polynomial() {
    _c_.clear();
  }

  void polynomial::set_coefficients(const std::vector<double>& c_)
  {
    _c_ = c_;
  }

  const std::vector<double> & polynomial::get_coefficients() const
  {
    return _c_;
  }

  bool polynomial::is_initialized() const
  {
    return this->is_valid();
  }

  void polynomial::initialize(const datatools::properties & config_,
                              const unary_function_dict_type & functors_)
  {
    this->i_unary_function::_base_initialize(config_, functors_);

    // Build from an array of coefficients:
    if (_c_.size() == 0) {
      if (config_.has_key("coefficients")) {
        std::vector<double> coeffs;
        config_.fetch("coefficients", coeffs);
        DT_THROW_IF(coeffs.size() == 0, std::logic_error,
                    "Empty array of coefficients!");
        set_coefficients(coeffs);
      }
    }

    // Build from individual coefficients given by power:
    if (_c_.size() == 0) {
      int max_power = -1;
      if (config_.has_key("max_power")) {
        max_power = config_.fetch_integer("max_power");
        DT_THROW_IF(max_power < 0, std::range_error, "Invalid max power !");
        for (int ipow = 0; ipow <= max_power; ipow++) {
          double coeff = 0.0;
          std::ostringstream coeff_key;
          coeff_key << "coefficient_" << ipow;
          if (config_.has_key(coeff_key.str())) {
            coeff = config_.fetch_real(coeff_key.str());
            DT_THROW_IF(datatools::is_valid(coeff),
                        std::logic_error,
                        "Invalid coefficient at power [" << ipow << "]!");
          }
          set_coeff((unsigned int) ipow, coeff);
        }
      }
    }

    // Build from 3 points which define a parabola:
    if (_c_.size() == 0) {
      datatools::properties parabola_config;
      config_.export_and_rename_starting_with(parabola_config, "parabola.", "");
      if (parabola_config.size()) {
        double x[3];
        double y[3];
        for (int i = 0; i < 3; i++) {
          x[i] = datatools::invalid_real();
          y[i] = datatools::invalid_real();
          std::ostringstream x_key;
          x_key << "x" << i;
          std::ostringstream y_key;
          y_key << "y" << i;
          if (parabola_config.has_key(x_key.str())) {
            x[i] = parabola_config.fetch_real(x_key.str());
          }
          if (parabola_config.has_key(y_key.str())) {
            y[i] = parabola_config.fetch_real(y_key.str());
          }
        }
        bool mkp = make_parabola(x[0], x[1], x[2], y[0], y[1], y[2]);
        DT_THROW_IF(!mkp, std::logic_error, "Cannot build a parabola polynomial!");
      }
    }

    // Checking...
    DT_THROW_IF(_c_.size() == 0, std::logic_error,
                "No coefficients are defined !");
    return;
  }

  void polynomial::reset()
  {
    _c_.clear();
    this->i_unary_function::_base_reset();
    return;
  }

  double polynomial::_eval(double x_) const {
    unsigned int sz = _c_.size();
    const double * first_arg = 0;
    if (sz > 0) {
      first_arg = &(_c_[0]);
    }
    return gsl_poly_eval(first_arg, sz, x_);
  }

  void polynomial::tree_dump(std::ostream & out_,
                             const std::string & title_,
                             const std::string & indent_,
                             bool inherit_) const
  {
    this->i_unary_function::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << i_tree_dumpable::tag
         << "Degree :  [" << get_degree() << "]" << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Coefficients :  [" << get_ncoeffs() << "]" << std::endl;
    for (std::size_t ipow = 0; ipow < _c_.size(); ipow++) {
      out_ << indent_ << i_tree_dumpable::inherit_skip_tag(inherit_);
      if (ipow == _c_.size() - 1) {
        out_ << i_tree_dumpable::last_tag;
      } else {
        out_ << i_tree_dumpable::tag;
      }
      out_ << "Power [" << ipow << "] = " << _c_[ipow] << std::endl;
    }

    return;
  }

  void polynomial::print(std::ostream & out_, int format_, bool eol_) const {
    std::string prod_sym = "×";
    std::string exp_sym  = "^";
    prod_sym = "*";
    exp_sym  = "**";
    if (format_ == 1) {
      prod_sym = "×";
      exp_sym  = "^";
    }
    bool first = true;
    for (size_t deg = 0; deg <= get_degree(); ++deg) {
      if (_c_[deg] == 0.0) {
        continue;
      } else {
        if (!first) {
          out_ << (_c_[deg] > 0? " + ": " - ");
        } else {
          out_ << (_c_[deg] > 0? "": " - ");
          first = false;
        }
        out_.unsetf(std::ios::showpos);
        // 2011-06-16 FM: add std::abs to remove template ambiguity for some compiler :
        out_ << std::abs(_c_[deg]);
        if (deg >= 1) {
          out_ << ' ' << prod_sym << ' ' << 'X';
        }
        if (deg >= 2) {
          out_ << exp_sym << deg;
        }
      }
    }
    if (eol_) out_ << std::endl;
  }

  bool polynomial::solve_linear(double p0_, double p1_, unsigned int& nsols_,
                                double &x0_) {
    if (p1_ == 0.0) {
      if (p0_ == 0.0) {
        nsols_ = -1;
        return false;
      } else {
        nsols_ = 0;
        return false;
      }
      x0_ = GSL_NAN;
    }
    x0_ = -p0_ / p1_;
    nsols_ = 1;
    return true;
  }

  bool polynomial::solve_quadratic(double p0_, double p1_, double p2_,
                                   unsigned int& nsols_,
                                   double& x0_, double& x1_) {
    if (p2_ == 0) {
      x1_ = GSL_NAN;
      return solve_linear(p0_, p1_, nsols_, x0_);
    }
    nsols_ = (unsigned int)gsl_poly_solve_quadratic(p2_, p1_, p0_, &x0_, &x1_);
    if (nsols_ < 1) x0_ = GSL_NAN;

    if (nsols_ < 2) x1_ = GSL_NAN;

    return nsols_ > 0;
  }

  bool polynomial::solve_cubic(double p0_, double p1_, double p2_,
                               unsigned int& nsols_,
                               double& x0_, double& x1_, double& x2_) {
    nsols_ = (unsigned int)gsl_poly_solve_cubic(p2_, p1_, p0_, &x0_, &x1_, &x2_);

    if (nsols_ < 1) x0_ = GSL_NAN;

    if (nsols_ < 2) x1_ = GSL_NAN;

    if (nsols_ < 3) x2_ = GSL_NAN;

    return nsols_ > 0;
  }

  bool polynomial::solve_cubic(double p0_, double p1_, double p2_, double p3_,
                               unsigned int& nsols_,
                               double& x0_, double& x1_, double& x2_) {
    if (p3_ == 0) {
      x2_ = GSL_NAN;
      return solve_quadratic(p0_, p1_, p2_, nsols_, x0_, x1_);
    }

    nsols_ = (unsigned int)solve_cubic(p2_/p3_, p1_/p3_, p0_/p3_, nsols_,
                                       x0_, x1_, x2_);
    if (nsols_ < 1) x0_ = GSL_NAN;

    if (nsols_ < 2) x1_ = GSL_NAN;

    if (nsols_ < 3) x2_ = GSL_NAN;

    return nsols_ > 0;
  }

  bool polynomial::make_parabola(double x1_, double x2_, double x3_,
                                 double y1_, double y2_, double y3_)
  {
    this->reset();
    std::vector<double> cba;
    cba.assign(3, 0.0);
    bool success = compute_parabola(x1_, x2_, x3_, y1_, y2_, y3_, cba[2], cba[1], cba[0]);
    if (success) {
      this->set_coefficients(cba);
    }
    return success;
  }


  // static
  bool polynomial::compute_parabola(double x1_, double x2_, double x3_,
                                    double y1_, double y2_, double y3_,
                                    double & a_, double & b_, double & c_)
  {
    bool success = false;
    datatools::invalidate(a_);
    datatools::invalidate(b_);
    datatools::invalidate(c_);
    /* Maxima output:

       |eq1:D1=a*t1^2+b*t1+c$
       |eq2:D2=a*t2^2+b*t2+c$
       |eq3:D3=a*t3^2+b*t3+c$
       |solve([eq1,eq2,eq3],[a,b,c]);
       |(%i10) solve([eq1,eq2,eq3],[a,b,c]);
       |               t1 (D3 - D2) - t2 D3 + t3 D2 + (t2 - t3) D1
       |(%o10) [[a = ------------------------------------------------,
       |                   2     2         2     2        2
       |             t1 (t3  - t2 ) - t2 t3  + t2  t3 + t1  (t2 - t3)
       |        2               2        2         2     2
       |      t1  (D3 - D2) - t2  D3 + t3  D2 + (t2  - t3 ) D1
       |b = - ------------------------------------------------,
       |            2     2         2     2        2
       |      t1 (t3  - t2 ) - t2 t3  + t2  t3 + t1  (t2 - t3)
       |          2        2         2                      2           2
       |    t1 (t3  D2 - t2  D3) + t1  (t2 D3 - t3 D2) + (t2  t3 - t2 t3 ) D1
       |c = -----------------------------------------------------------------]]
       |                  2     2         2     2        2
       |            t1 (t3  - t2 ) - t2 t3  + t2  t3 + t1  (t2 - t3)
    */
    double t1 = x1_;
    double t2 = x2_;
    double t3 = x3_;
    double D1 = y1_;
    double D2 = y2_;
    double D3 = y3_;

    double D = t1 * (t3 * t3 - t2 * t2) - t2 * t3 * t3 + t2 * t2 * t3 + t1 * t1 * (t2 - t3);
    if (D != 0.0) {
      a_ = t1 * (D3 - D2) - t2 * D3 + t3 * D2 + (t2 - t3) * D1;
      b_ = - (t1 * t1 * (D3 - D2) - t2 * t2 * D3 + t3 *t3 * D2 + (t2 * t2 - t3 * t3) * D1);
      c_ = t1 * (t3 * t3 * D2 - t2 * t2 * D3) + t1 * t1 * (t2 * D3 - t3 * D2) + (t2 * t2 * t3 - t2 * t3 * t3) * D1;
      a_ /= D;
      b_ /= D;
      c_ /= D;
      success = true;
    }
    return success;
  }


  //----------------------------------------------------------------------
  // Inner solver class implementation
  //
  struct polynomial::solver::solver_impl {
    int                          _status_;
    unsigned int                 _sz_;
    std::vector<double>          _z_;
    gsl_poly_complex_workspace * _ws_;
    /*
    unsigned int _capacity_;
    double * _z_;
    */
  };

  bool polynomial::solver::is_initialized() const
  {
    return pImpl->_ws_ != 0;
  }

  bool polynomial::solver::is_success() const
  {
    return pImpl->_status_ == GSL_SUCCESS;
  }

  polynomial::solver::solver(unsigned int sz_) {
    pImpl = new solver_impl;
    pImpl->_status_ = GSL_EFAILED;
    pImpl->_sz_ = 0;
    pImpl->_ws_ = 0;
    _init_(sz_);
  }

  polynomial::solver::~solver() {
    _reset_();
  }

  void polynomial::solver::_init_(unsigned int sz_) {
    pImpl->_status_ = GSL_EFAILED;
    if (is_initialized()) {
      _reset_();
    }
    if (sz_ > 1) {
      pImpl->_ws_ = gsl_poly_complex_workspace_alloc(sz_);
      pImpl->_sz_ = sz_;
      pImpl->_z_.assign(2 * (sz_ - 1), std::numeric_limits<double>::quiet_NaN());
    }
  }

  void polynomial::solver::_reset_() {
    if (pImpl->_ws_ != 0) {
      gsl_poly_complex_workspace_free(pImpl->_ws_);
      pImpl->_ws_ = 0;
    }
    pImpl->_z_.clear();
    pImpl->_sz_ = 0;
    pImpl->_status_ = GSL_EFAILED;
  }

  size_t polynomial::solver::fetch_solutions(std::vector<std::complex<double> > & zsols_)
  {
    zsols_.clear();
    if (pImpl->_status_ != GSL_EFAILED) {
      zsols_.reserve(pImpl->_sz_ - 1);
      for (int i = 0; i < (int) pImpl->_sz_ - 1; i++) {
        std::complex<double> zi(pImpl->_z_[2 * i], pImpl->_z_[2 * i + 1]);
        zsols_.push_back(zi);
      }
    }
    return zsols_.size();
  }

  size_t polynomial::solver::fetch_real_solutions(std::vector<double> & rsols_, double precision_)
  {
    double precision = precision_;
    if (precision <= std::numeric_limits<double>::epsilon()) {
      precision = std::numeric_limits<double>::epsilon();
    }
    rsols_.clear();
    if (pImpl->_status_ != GSL_EFAILED) {
      rsols_.reserve(pImpl->_sz_ - 1);
      for (int i = 0; i < (int) pImpl->_sz_ - 1; i++) {
        std::complex<double> zi(pImpl->_z_[2 * i], pImpl->_z_[2 * i + 1]);
        if (std::abs(zi.imag()) < precision) {
          rsols_.push_back(zi.real());
        }
      }
    }
    return rsols_.size();
  }

  bool polynomial::solver::solve(const polynomial& p_) {
    unsigned int deg = p_.get_degree();
    size_t n = deg + 1;
    if (! is_initialized()) {
      _init_(n);
    }
    {
      const double * a = &(p_.get_coefficients()[0]); // C++11 uses .data()
      pImpl->_status_ = gsl_poly_complex_solve(a,
                                               pImpl->_sz_ ,
                                               pImpl->_ws_,
                                               &pImpl->_z_[0]);
    }
    return pImpl->_status_ == GSL_SUCCESS;
  }

} // namespace mygsl

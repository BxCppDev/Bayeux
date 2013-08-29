// mygsl/polynomial.cc

#include <mygsl/polynomial.h>

#include <stdexcept>
#include <sstream>
#include <cmath> // for std::abs

#include <gsl/gsl_errno.h>
#include <gsl/gsl_fit.h>
#include <gsl/gsl_poly.h>

#include <datatools/exception.h>

namespace mygsl {

using namespace std;

DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(polynomial,"mygsl::polynomial")

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

polynomial::polynomial(const vector<double>& c_) {
  set_coefficients(c_);
}

polynomial::polynomial(const polynomial& p_) {
  _c_ = p_._c_;
}

polynomial::~polynomial() {
  _c_.clear();
}

void polynomial::set_coefficients(const std::vector<double>& c_)
{
  _c_ = c_;
}

double polynomial::_eval(double x_) const {
  unsigned int sz = _c_.size();
  const double * first_arg = 0;
  if (sz > 0) {
    first_arg = &(_c_[0]);
  }
  return gsl_poly_eval(first_arg, sz, x_);
}

void polynomial::print(ostream & out_, int format_, bool eol_) const {
  string prod_sym = "×";
  string exp_sym  = "^";
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
      out_.unsetf(ios::showpos);
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
  if (eol_) out_ << endl;
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

//----------------------------------------------------------------------
// Inner solver class implementation
//
struct polynomial::solver::solver_impl {
  int    _status_;
  unsigned int _sz_;
  gsl_poly_complex_workspace *_ws_;
};

polynomial::solver::solver(unsigned int sz_) {
  pImpl = new solver_impl;
  pImpl->_status_ = 0;
  pImpl->_ws_ = 0;
  _init_(sz_);
}

polynomial::solver::~solver() {
  _reset_();
}

void polynomial::solver::_init_(unsigned int sz_) {
  pImpl->_status_ = GSL_EFAILED;
  if (pImpl->_ws_ != 0) {
    if (pImpl->_sz_ >= sz_) {
      return;
    } else {
      _reset_();
    }
  }
  if (sz_ > 1) {
    pImpl->_ws_ = gsl_poly_complex_workspace_alloc(sz_);
    pImpl->_sz_ = sz_;
  }
}

void polynomial::solver::_reset_() {
  if (pImpl->_ws_ != 0) {
    gsl_poly_complex_workspace_free(pImpl->_ws_);
    pImpl->_ws_ = 0;
    pImpl->_sz_ = 0;
  }
  pImpl->_status_ = GSL_EFAILED;
}

bool polynomial::solver::solve(const polynomial& p_) {
  unsigned int deg = p_.get_degree();
  _init_(deg + 1);
  {
    DT_THROW_IF(true, std::logic_error, "Polynomial solver is not implemented yet !");
  }
  return pImpl->_status_ == GSL_SUCCESS;
}

} // namespace mygsl

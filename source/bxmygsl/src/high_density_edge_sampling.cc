// \file mygsl/high_density_edge_sampling.cc

// Ourselves:
#include <mygsl/high_density_edge_sampling.h>

// Third party:
#include <datatools/utils.h>

// This project:
#include <mygsl/compare_value.h>

namespace mygsl {

  high_density_edge_sampling::high_density_edge_sampling()
  {
    datatools::invalidate(_xmin_);
    datatools::invalidate(_xmax_);
    datatools::invalidate(_xedge_left_);
    datatools::invalidate(_xedge_right_);
    _nsteps_medium_ = 0;
    _nsteps_edge_left_ = 0;
    _nsteps_edge_right_ = 0;
  }

  high_density_edge_sampling::~high_density_edge_sampling()
  {
    if (is_initialized()) this->high_density_edge_sampling::reset();
  }

  std::size_t high_density_edge_sampling::get_nsteps() const
  {
    return _nsamples_ - 1;
  }

  std::size_t high_density_edge_sampling::get_nsamples() const
  {
    return _nsamples_;
  }

  /*
  void high_density_edge_sampling::set_domain(const double xmin_, const double xmax_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Sampling is already initialized!");
    DT_THROW_IF(xmin_ >= xmax_, std::logic_error, "Invalid sampling domain!");
    _xmin_ = xmin_;
    _xmax_ = xmax_;
  }

  void high_density_edge_sampling::set_sampling_parameters(const double xedge_left_,
                                                           const double xedge_right_,
                                                           const std::size_t nsteps_medium_,
                                                           const std::size_t nsteps_edge_left_,
                                                           const std::size_t nsteps_edge_right_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Sampling is already initialized!");
    DT_THROW_IF(xedge_left_ < _xmin_, std::logic_error, "Invalid left edge limit!");
    DT_THROW_IF(xedge_right_ > _xmax_, std::logic_error, "Invalid right edge limit!");
    DT_THROW_IF(xedge_left_ >= xedge_right_, std::logic_error, "Invalid left/right edge limits!");
    _xedge_left_ = xedge_left_;
    _xedge_right_ = xedge_right_;
    _nsteps_medium_ = nsteps_medium_;
    _nsteps_edge_left_ = nsteps_edge_left_;
    _nsteps_edge_right_ = nsteps_edge_right_;
  }
  */

  void high_density_edge_sampling::make_left_right(const double xmin_,
                                                   const double xmax_,
                                                   const double xedge_left_,
                                                   const double xedge_right_,
                                                   const std::size_t nsteps_medium_,
                                                   const std::size_t nsteps_edge_left_,
                                                   const std::size_t nsteps_edge_right_)
  {
    if (is_initialized()) {
      reset();
    }
    DT_THROW_IF(!datatools::is_valid(xmin_), std::logic_error, "Invalid minimum limit!");
    DT_THROW_IF(!datatools::is_valid(xmax_), std::logic_error, "Invalid maximum limit!");
    DT_THROW_IF(xmin_ >= xmax_, std::logic_error, "Invalid sampling domain!");
    _xmin_ = xmin_;
    _xmax_ = xmax_;
    if (datatools::is_valid(xedge_left_)) {
      DT_THROW_IF(xedge_left_ <= _xmin_, std::logic_error, "Invalid left edge limit!");
    } else {
      DT_THROW_IF(nsteps_edge_left_ != 0, std::logic_error, "Invalid non zero number of steps for left edge!");
    }
    if (datatools::is_valid(xedge_right_)) {
      DT_THROW_IF(xedge_right_ >= _xmax_, std::logic_error, "Invalid right edge limit!");
    } else {
      DT_THROW_IF(nsteps_edge_right_ != 0, std::logic_error, "Invalid non zero number of steps for right edge!");
    }
    if (datatools::is_valid(xedge_left_) && datatools::is_valid(xedge_right_)) {
      DT_THROW_IF(xedge_left_ >= xedge_right_, std::logic_error, "Invalid left/right edge limits!");
    }
    DT_THROW_IF(nsteps_medium_ < 2, std::logic_error, "Number of medium samples is too low!");
    DT_THROW_IF(nsteps_edge_left_ + nsteps_edge_right_ == 0, std::logic_error, "Missing left/right edge sampling!");
    _xedge_left_ = xedge_left_;
    _xedge_right_ = xedge_right_;
    _nsteps_medium_ = nsteps_medium_;
    _nsteps_edge_left_ = nsteps_edge_left_;
    _nsteps_edge_right_ = nsteps_edge_right_;
    _init_();
  }

  void high_density_edge_sampling::make_left(const double xmin_,
                                             const double xmax_,
                                             const double xedge_left_,
                                             const std::size_t nsteps_medium_,
                                             const std::size_t nsteps_edge_left_)
  {
    make_left_right(xmin_, xmax_, xedge_left_, datatools::invalid_real(), nsteps_medium_, nsteps_edge_left_, 0);
  }

  void high_density_edge_sampling::make_right(const double xmin_,
                                             const double xmax_,
                                             const double xedge_right_,
                                             const std::size_t nsteps_medium_,
                                             const std::size_t nsteps_edge_right_)
  {
    make_left_right(xmin_, xmax_, datatools::invalid_real(), xedge_right_, nsteps_medium_, 0, nsteps_edge_right_);
  }

  double high_density_edge_sampling::get_min() const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Not initialized!");
    return _xmin_;
  }

  double high_density_edge_sampling::get_max() const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Not initialized!");
    return _xmax_;
  }

  bool high_density_edge_sampling::is_initialized() const
  {
    return _initialized_;
  }

  void high_density_edge_sampling::initialize(const datatools::properties & /* config_ */)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Sampling is already initialized!");

    _init_();
    if (!_initialized_) _initialized_ = true;
    return;
  }

  void high_density_edge_sampling::reset()
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Sampling is not initialized!");
    _initialized_ = false;
    _reset_();
    return;
  }

  double high_density_edge_sampling::get_medium_step() const
  {
    return (_xedge_right_ - _xedge_left_) / _nsteps_medium_;
  }

  void high_density_edge_sampling::_init_()
  {
    _nsamples_ = _nsteps_edge_left_ + _nsteps_medium_ + _nsteps_edge_right_ + 1;
    std::cerr << "DEVEL: nsamples =" << _nsamples_ << std::endl;
    _il_ = _nsteps_edge_left_;
    std::cerr << "DEVEL: il =" << _il_ << std::endl;
    _ir_ = _nsteps_edge_left_ + _nsteps_medium_;
    std::cerr << "DEVEL: ir =" << _ir_ << std::endl;
    double il = _il_ * 1.0;
    double ir = _ir_ * 1.0;
    _medium_.make_line(_xedge_left_, _xedge_right_, il, ir);
    _medium_.set_epsilon(0.0);
    double am = _medium_.get_coeff(1);
    double xm = _xmin_;
    double xl = _xedge_left_;
    double d = xm * xm - 2 * xl * xm + xl * xl;
    double al = (-il + am * xl - am * xm) / d;
    double bl = (2 * il * xl - am * xl * xl + am * xm * xm) / d;
    double cl = (am * xl * xl * xm + il * xm * xm + xl * (-am * xm * xm - 2 * il * xm)) / d;
    std::vector<double> pl({cl, bl, al});
    _left_.set_coefficients(pl);
    xm = _xmax_;
    double xr = _xedge_right_;
    double im = (_nsamples_ - 1) * 1.0;
    d = xm * xm - 2 * xr * xm + xr * xr;
    double ar = (im - ir + am * xr - am * xm) / d;
    double br = - ((2 * im - 2 * ir) * xr + am * xr * xr - am * xm *xm) / d;
    double cr = ( ir * xm * xm + (-am * xm * xm - 2 * ir * xm) * xr + (am * xm + im) * xr * xr) / d;
    std::vector<double> pr({cr, br, ar});
    _right_.set_coefficients(pr);
    _initialized_ = true;
    _left_.tree_dump(std::cerr,   "Left: ", "DEVEL: ");
    _medium_.tree_dump(std::cerr, "Medium: ", "DEVEL: ");
    _right_.tree_dump(std::cerr,  "Right: ", "DEVEL: ");
  }

  void high_density_edge_sampling::_reset_()
  {
    _initialized_ = false;
    datatools::invalidate(_xmin_);
    datatools::invalidate(_xmax_);
    datatools::invalidate(_xedge_left_);
    datatools::invalidate(_xedge_right_);
    _nsteps_medium_ = 0;
    _nsteps_edge_left_ = 0;
    _nsteps_edge_right_ = 0;
  }

  bool high_density_edge_sampling::value_to_index(const double x_,
                                                  std::size_t & index_,
                                                  sampling::index_classification_type & classification_) const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Sampling is not initialized!");
    classification_ = sampling::INDEX_UNDEFINED;
    index_ = sampling::INVALID_INDEX;
    if (!datatools::is_valid(x_)) {
      return false;
    }

    {
      compare_score_type cs = compare_absolute(x_, _xmin_, 0.0);
      if (cs == COMPARE_LESS) {
        classification_ = sampling::INDEX_UNDERFLOW;
        index_ = sampling::INVALID_INDEX;
        return false;
      }
    }

    {
      compare_score_type cs = compare_absolute(x_, _xmax_, 0.0);
      if (cs == COMPARE_GREATER) {
        classification_ = sampling::INDEX_OVERFLOW;
        index_ = sampling::INVALID_INDEX;
        return false;
      }
    }

    classification_ = sampling::INDEX_NORMAL;
    if (x_ < _xedge_left_) {
      index_ = (std::size_t) _left_(x_);
      std::cerr << "DEVEL: left   : x=" << x_ << " i=" << index_ << std::endl;
    } else if (x_ <= _xedge_right_) {
      index_ = (std::size_t) _medium_(x_);
      std::cerr << "DEVEL: medium : x=" << x_ << " i=" << index_ << std::endl;
    } else {
      index_ = (std::size_t) _right_(x_);
      std::cerr << "DEVEL: right  : x=" << x_ << " i=" << index_ << std::endl;
    }
    return true;
  }

  sampling::index_classification_type
  high_density_edge_sampling::index_to_value(const std::size_t index_,
                                             double & value_) const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Sampling is not initialized!");
    sampling::index_classification_type ic = sampling::INDEX_UNDEFINED;
    datatools::invalidate(value_);
    if (index_ == 0) {
      // min value
      ic = sampling::INDEX_NORMAL;
      value_ = get_min();
    } else if (index_ < _il_) {
      // left edge sampling domain
      unsigned int nsols = 0;
      double val1, val2;
      if (polynomial::solve_quadratic(_left_.get_coeff(0), _left_.get_coeff(1), _left_.get_coeff(2), nsols, val1, val2)) {
        value_ = val1 - index_;
        ic = sampling::INDEX_NORMAL;
      }
    } else if (index_ <= _ir_) {
      // medium sampling domain
      unsigned int nsol = 0;
      double val;
      if (polynomial::solve_linear(_medium_.get_coeff(0) - index_, _medium_.get_coeff(1), nsol, val)) {
        value_ = val;
        ic = sampling::INDEX_NORMAL;
      }
    } else if (index_ < get_last_index()) {
      // right edge sampling domain
      ic = sampling::INDEX_NORMAL;
    } else if (index_ == get_last_index()) {
      // max value
      ic = sampling::INDEX_NORMAL;
      value_ = get_max();
    } else {
      ic = sampling::INDEX_OVERFLOW;
    }
    return ic;
  }


} // end of namespace mygsl

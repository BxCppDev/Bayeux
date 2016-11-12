// materials/refractive_index.cc

// Ourselves:
#include <materials/refractive_index.h>

// Standard library:
#include <cmath>
#include <limits>

// Bayeux:
#include <datatools/utils.h>
#include <datatools/clhep_units.h>

namespace materials {

  refractive_index::refractive_index()
  {
    _formula_ = FORMULA_INVALID;
    _eval_mode_ = EVAL_N;
    datatools::invalidate(_n0_);
    datatools::invalidate(_k0_);
    return;
  }

  refractive_index::~refractive_index()
  {
    if (is_initialized()) reset();
    return;
  }

  bool refractive_index::is_eval_n() const
  {
    return _eval_mode_ == EVAL_N;
  }

  bool refractive_index::is_eval_k() const
  {
    return _eval_mode_ == EVAL_K;
  }

  void refractive_index::set_eval_n()
  {
    _eval_mode_ = EVAL_N;
    return;
  }

  void refractive_index::set_eval_k()
  {
    _eval_mode_ = EVAL_K;
    if (_k_tabfunc_.get() == nullptr) {
      _build_k_tabfunc();
    }
    return;
  }

  bool refractive_index::has_lambda_range() const
  {
    return _lambda_range_.is_valid();
  }

  void refractive_index::set_lambda_range(const double lambda_min_, const double lambda_max_)
  {
    _lambda_range_.set(lambda_min_, lambda_max_);
    return;
  }

  void refractive_index::set_lambda_range(const datatools::real_range & lr_)
  {
    _lambda_range_ = lr_;
    return;
  }

  const datatools::real_range & refractive_index::get_lambda_range() const
  {
    return _lambda_range_;
  }

  void refractive_index::make_constant(const double n_, const double k_)
  {
    _formula_ = FORMULA_CONSTANT;
    DT_THROW_IF(n_ <= 1.0 || n_ > 4.0,
                std::domain_error, "Invalid refractive index value ["
                << n_ << "]!");
    _n0_ = n_;
    if (datatools::is_valid(k_)) {
      DT_THROW_IF(k_ <= 0.0,
                  std::domain_error, "Invalid extinction coefficient value ["
                  << k_ << "]!");
      _k0_ = k_;
    }
    return;
  }

  void refractive_index::make_tabulated(const std::string & n_tab_,
                                        const std::string & k_tab_)
  {
    _formula_ = FORMULA_DISPERSION_TABULATED;
    _n_table_ = n_tab_;
    _build_n_tabfunc();
    if (!k_tab_.empty()) {
      _k_table_ = k_tab_;
      _build_k_tabfunc();
    }
    return;
  }

  void refractive_index::make_dispersion_rii(const dispersion_formula_type formula_,
                                             const coeff_dict_type & coeffs_,
                                             const std::string & k_tab_)
  {
    DT_THROW_IF(formula_ < FORMULA_DISPERSION_RII_SELLMEIER ||
                formula_ > FORMULA_DISPERSION_RII_HERZBERGER,
                std::domain_error,
                "Unsupported RII formula type '" << formula_ << "'!");
    _formula_ = formula_;
    _rii_c_ = coeffs_;
    if (!k_tab_.empty()) {
      _k_table_ = k_tab_;
      _build_k_tabfunc();
    }
    return;
  }

  bool refractive_index::has_rii_coeff(const std::size_t rank_) const
  {
    return _rii_c_.count(rank_) == 1;
  }

  double refractive_index::get_rii_coeff(const std::size_t rank_) const
  {
    coeff_dict_type::const_iterator found = _rii_c_.find(rank_);
    if (found != _rii_c_.end()) {
      return found->second;
    }
    return datatools::invalid_real();
  }

  bool refractive_index::is_initialized() const
  {
    if (_formula_ == FORMULA_INVALID) {
      return false;
    } else if (_formula_ == FORMULA_CONSTANT) {
      if (!datatools::is_valid(_n0_)) return false;
    } else {
      if (_rii_c_.size() < 1) return false;
    }
    return true;
  }

  void refractive_index::initialize(const datatools::properties & config_,
                                    mygsl::unary_function_dict_type & /* functors_ */)
  {
    if (is_initialized()) return;

    if (_formula_ == FORMULA_INVALID) {
      dispersion_formula_type formula = FORMULA_INVALID;
      if (config_.has_key("formula")) {
        const std::string & formula_repr = config_.fetch_string("formula");
        if (formula_repr == "constant") {
          formula = FORMULA_CONSTANT;
        } else if (formula_repr == "tabulated") {
          formula = FORMULA_DISPERSION_TABULATED;
        } else if (formula_repr == "rii::sellmeier") {
          formula = FORMULA_DISPERSION_RII_SELLMEIER;
        } else if (formula_repr == "rii::sellmeier2") {
          formula = FORMULA_DISPERSION_RII_SELLMEIER2;
        } else if (formula_repr == "rii::polynomial") {
          formula = FORMULA_DISPERSION_RII_POLYNOMIAL;
        } else if (formula_repr == "rii::rii-4") {
          formula = FORMULA_DISPERSION_RII_RII4;
        } else if (formula_repr == "rii::cauchy") {
          formula = FORMULA_DISPERSION_RII_CAUCHY;
        } else if (formula_repr == "rii::gases") {
          formula = FORMULA_DISPERSION_RII_GASES;
        } else if (formula_repr == "rii::herzberger") {
          formula = FORMULA_DISPERSION_RII_HERZBERGER;
        // } else if (formula_repr == "rii::retro") {
        //   formula = FORMULA_DISPERSION_RII_RETRO;
        // } else if (formula_repr == "rii::exotic") {
        //   formula = FORMULA_DISPERSION_RII_EXOTIC;
        } else {
          DT_THROW(std::domain_error,
                   "Invalid refractive index definition formula '" << formula_repr << "'!");
        }
      }

      if (formula == FORMULA_CONSTANT) {
        // No dispersion:
        DT_THROW_IF(!config_.has_key("constant.n"), std::logic_error,
                    "Missing 'constant.n' property!");
        double n0 = config_.fetch_dimensionless_real("constant.n");
        double k0 = datatools::invalid_real();
        if(config_.has_key("constant.k")) {
          k0 = config_.fetch_dimensionless_real("constant.k");
        }
        make_constant(n0, k0);
      } else if (formula == FORMULA_DISPERSION_TABULATED) {
        DT_THROW_IF(!config_.has_key("tabulated.n"), std::logic_error,
                    "Missing 'tabulated.n' property!");
        std::string n_table = config_.fetch_string("tabulated.n");
        std::string k_table;
        if (config_.has_key("tabulated.k")) {
          k_table = config_.fetch_string("tabulated.k");
        }
        make_tabulated(n_table, k_table);
      } else if (formula >= FORMULA_DISPERSION_RII_SELLMEIER &&
                 formula <= FORMULA_DISPERSION_RII_HERZBERGER) {
        coeff_dict_type coeffs;
        DT_THROW_IF(!config_.has_key("rii.coeffs"),
                    std::logic_error,
                    "Missing 'rii.coeffs' property!");
        std::vector<double> vcoeffs;
        config_.fetch("rii.coeffs", vcoeffs);
        for (std::size_t ic = 0; ic < vcoeffs.size(); ic++) {
          coeffs[ic + 1] = vcoeffs[ic];
        }
        std::string k_table;
        if (config_.has_key("tabulated.k")) {
          k_table = config_.fetch_string("tabulated.k");
        }
        make_dispersion_rii(formula, coeffs, k_table);
      }
    }

    DT_THROW_IF(_formula_ == FORMULA_INVALID, std::logic_error,
                "Refractive index formula is not defined!");

    if (_formula_ != FORMULA_CONSTANT && _formula_ != FORMULA_DISPERSION_TABULATED) {
      if (!has_lambda_range()) {
        double wl_min = datatools::invalid_real();
        double wl_max = datatools::invalid_real();
        std::size_t nbounds = 0;
        if (config_.has_key("wavelength.min")) {
          wl_min = config_.fetch_real_with_explicit_dimension("wavelength.min", "length");
          nbounds++;
        }
        if (config_.has_key("wavelength.max")) {
          wl_max = config_.fetch_real_with_explicit_dimension("wavelength.max", "length");
          nbounds++;
        }
        DT_THROW_IF(nbounds == 1,
                    std::logic_error,
                    "Missing min or max wavelength limit");
        if (nbounds == 2) {
          set_lambda_range(wl_min, wl_max);
        }
      }
    }
    return;
  }

  void refractive_index::reset()
  {
    if (!is_initialized()) return;
    _formula_ = FORMULA_INVALID;
    datatools::invalidate(_n0_);
    datatools::invalidate(_k0_);
    _n_table_.clear();
    _k_table_.clear();
    _rii_c_.clear();
    _lambda_range_.invalidate();
    if (_n_tabfunc_.get() != nullptr) {
      _n_tabfunc_.reset();
    }
    if (_k_tabfunc_.get() != nullptr) {
      _k_tabfunc_.reset();
    }
    _eval_mode_ = EVAL_N;
    return;
  }

  double refractive_index::compute_principal_dispersion() const
  {
    double nF = eval_refractive_index(486.13 * CLHEP::nanometer);
    double nC = eval_refractive_index(656.27 * CLHEP::nanometer);
    return nF - nC;
  }

  double refractive_index::compute_abbe_number() const
  {
    double nD = eval_refractive_index(589.3 * CLHEP::nanometer);
    return (nD - 1.0) / compute_principal_dispersion();
  }

  double refractive_index::compute_abbe_number_e() const
  {
    double nFe = eval_refractive_index(480.0 * CLHEP::nanometer);
    double nCe = eval_refractive_index(643.8 * CLHEP::nanometer);
    double nDe = eval_refractive_index(546.073 * CLHEP::nanometer);
    return (nDe - 1.0) / (nFe - nCe);
  }

  double refractive_index::eval_refractive_index(const double lambda_) const
  {
    return _eval_n(lambda_);
  }

  double refractive_index::eval_extinction_coefficient(const double lambda_) const
  {
    return _eval_k(lambda_);
  }

  double refractive_index::eval_attenuation_coefficient(const double lambda_) const
  {
    double alpha = 4 * M_PI * eval_extinction_coefficient(lambda_) / lambda_;
    return alpha;
  }

  double refractive_index::eval_penetration_depth(const double lambda_) const
  {
    double delta_p = 1. / eval_attenuation_coefficient(lambda_);
    return delta_p;
  }

  bool refractive_index::has_explicit_domain_of_definition() const
  {
    if (_formula_ == FORMULA_CONSTANT) return false;
    if (_eval_mode_ == EVAL_N) {
      if (has_lambda_range()) return true;
      if (_formula_ == FORMULA_DISPERSION_TABULATED) return true;
    }
    if (_eval_mode_ == EVAL_K) {
      return true;
    }
   return false;
  }

  bool refractive_index::is_in_domain_of_definition(double lambda_) const
  {
    if (_formula_ == FORMULA_CONSTANT) return true;
    if (_eval_mode_ == EVAL_N) {
      if (has_lambda_range()) {
        return _lambda_range_.has(lambda_, 0.0001);
      }
      if (_formula_ == FORMULA_DISPERSION_TABULATED) {
        if (_n_tabfunc_.get() == nullptr) {
          const_cast<refractive_index*>(this)->_build_n_tabfunc();
        }
        return _n_tabfunc_->is_in_domain_of_definition(lambda_);
      }
    }
    if (_eval_mode_ == EVAL_K) {
      if (_k_tabfunc_.get() == nullptr) {
        const_cast<refractive_index*>(this)->_build_k_tabfunc();
      }
      return _k_tabfunc_->is_in_domain_of_definition(lambda_);
    }
    return true;
  }

  void refractive_index::_build_n_tabfunc()
  {
    DT_THROW_IF(_n_table_.empty(), std::logic_error,
                "Missing file name for tabulated refractive index!");
    std::string datafile = _n_table_;
    datatools::fetch_path_with_env(datafile);
    _n_tabfunc_.reset(new mygsl::tabulated_function);
    _n_tabfunc_->load_from_file(datafile);
    if (!_n_tabfunc_->is_table_locked()) {
      _n_tabfunc_->lock_table("linear");
    }
    return;
  }

  void refractive_index::_build_k_tabfunc()
  {
    DT_THROW_IF(_k_table_.empty(), std::logic_error,
                "Missing file name for tabulated extinction coefficient!");
    std::string datafile = _k_table_;
    datatools::fetch_path_with_env(datafile);
    _k_tabfunc_.reset(new mygsl::tabulated_function);
    _k_tabfunc_->load_from_file(datafile);
    if (!_k_tabfunc_->is_table_locked()) {
      _k_tabfunc_->lock_table("linear");
    }
    return;
  }

  double refractive_index::_eval(double lambda_) const
  {
    if (_eval_mode_ == EVAL_N) {
      return _eval_n(lambda_);
    } else {
      return _eval_k(lambda_);
    }
  }

  double refractive_index::_eval_k(double lambda_) const
  {
    if (_formula_ == FORMULA_CONSTANT) {
      return _k0_;
    }
    double k = datatools::invalid_real();
    if (_k_tabfunc_.get() == nullptr) {
      const_cast<refractive_index*>(this)->_build_k_tabfunc();
    }
    k = _k_tabfunc_->eval(lambda_);
    return k;
  }

  double refractive_index::_eval_n(double lambda_) const
  {
    static const double epsilon = std::numeric_limits<double>::epsilon();
    double l = lambda_ / CLHEP::micrometer;
    double l2 = l * l;
    double n = datatools::invalid_real();
    const coeff_dict_type & C = _rii_c_;
    switch (_formula_) {
    case FORMULA_CONSTANT:
      {
        n = _n0_;
        break;
      }
    case FORMULA_DISPERSION_TABULATED:
      {
        if (_n_tabfunc_.get() == nullptr) {
          const_cast<refractive_index*>(this)->_build_n_tabfunc();
        }
        n = _n_tabfunc_->eval(lambda_);
        break;
      }
    case FORMULA_DISPERSION_RII_SELLMEIER:
      {
        double n2 = 1.0;
        if (has_rii_coeff(1)) {
          n2 += C.at(1);
        }
        for (std::size_t iterm = 2; iterm < 18; iterm += 2) {
          if (has_rii_coeff(iterm)) {
            double c = C.at(iterm);
            if (std::abs(c) > epsilon) {
              n2 += c / (1.0 - C.at(iterm + 1) * C.at(iterm + 1) / l2);
            }
          }
        }
        n = std::sqrt(n2);
        break;
      }
    case FORMULA_DISPERSION_RII_SELLMEIER2:
      {
        double n2 = 1.0;
        if (has_rii_coeff(1)) {
          n2 += C.at(1);
        }
        double l2 = l * l ;
        for (std::size_t iterm = 2; iterm < 18; iterm += 2) {
          if (has_rii_coeff(iterm)) {
            double c = C.at(iterm);
            if (std::abs(c) > epsilon) {
              double t =  c / (1.0 - C.at(iterm + 1) / l2);
              n2 += t;
            }
          }
        }
        n = std::sqrt(n2);
        break;
      }
    case FORMULA_DISPERSION_RII_POLYNOMIAL:
      {
        double n2 = 0.0;
        if (has_rii_coeff(1)) {
          n2 += C.at(1);
        }
        for (std::size_t iterm = 2; iterm < 18; iterm += 2) {
          if (has_rii_coeff(iterm)) {
            double c = C.at(iterm);
            if (std::abs(c) > epsilon) {
              n2 += c * std::pow(l,  C.at(iterm+1));
            }
          }
        }
        n = std::sqrt(n2);
        break;
      }
    case FORMULA_DISPERSION_RII_RII4:
      {
        double n2 = 0.0;
        if (has_rii_coeff(1)) {
          n2 += C.at(1);
        }
        if (has_rii_coeff(2) && std::abs(C.at(2)) > epsilon) {
          n2 += (C.at(2) * std::pow(l, C.at(3))) / (l2 - std::pow(C.at(4), C.at(5)));
        }
        if (has_rii_coeff(6) && std::abs(C.at(6)) > epsilon) {
          n2 += (C.at(6) * std::pow(l, C.at(7))) / (l2 - std::pow(C.at(8), C.at(9)));
        }
        for (std::size_t iterm = 10; iterm < 17; iterm += 2) {
          if (has_rii_coeff(iterm)) {
            double c = C.at(iterm);
            if (std::abs(c) > epsilon) {
              n2 += c * std::pow(l, C.at(iterm+1));
            }
          }
        }
        n = std::sqrt(n2);
        break;
      }
    case FORMULA_DISPERSION_RII_CAUCHY:
      {
        double n = 0.0;
        if (has_rii_coeff(1)) {
          n += C.at(1);
        }
        for (std::size_t iterm = 2; iterm < 11; iterm += 2) {
          if (has_rii_coeff(iterm)) {
            double c = C.at(iterm);
            if (std::abs(c) > epsilon) {
              n += c * std::pow(l, C.at(iterm + 1));
            }
          }
        }
        break;
      }
    case FORMULA_DISPERSION_RII_GASES:
      {
        double n = 0.0;
        if (has_rii_coeff(1)) {
          n += C.at(1);
        }
        for (std::size_t iterm = 2; iterm < 11; iterm += 2) {
          if (has_rii_coeff(iterm)) {
            double c = C.at(iterm);
            if (std::abs(c) > epsilon) {
              n += c / ( C.at(iterm + 1) - 1.0 / l2);
            }
          }
        }
        n += 1.0;
        break;
      }
    case FORMULA_DISPERSION_RII_HERZBERGER:
      {
        double n = 0.0;
        if (has_rii_coeff(1)) {
          n += C.at(1);
        }
        if (has_rii_coeff(2) && std::abs(C.at(2)) > epsilon) {
          n += C.at(2) / (l2 - 0.028);
        }
        if (has_rii_coeff(3) && std::abs(C.at(3)) > epsilon) {
          n += C.at(3) * std::pow(l2 - 0.028, -2);
        }
        if (has_rii_coeff(4) && std::abs(C.at(4)) > epsilon) {
          n += C.at(4) * l2;
        }
        if (has_rii_coeff(5) && std::abs(C.at(5)) > epsilon) {
          n += C.at(5) * l2 * l2;
        }
        if (has_rii_coeff(6) && std::abs(C.at(6)) > epsilon) {
          n += C.at(6) * l2 * l2 * l2;
        }
        break;
      }
    default:
      DT_THROW(std::runtime_error, "Formula type is not implemented yet!");
      break;
    }
    return n;
  }

} // namespace materials

//! \file materials/refractive_index.h
//! \brief Refractive index associated to a medium
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef MATERIALS_REFRACTIVE_INDEX_H
#define MATERIALS_REFRACTIVE_INDEX_H

// Standard library:
#include <string>
#include <map>
#include <memory>
#include <limits>

// Bayeux:
#include <datatools/properties.h>
#include <datatools/real_range.h>
#include <mygsl/i_unary_function.h>
#include <mygsl/tabulated_function.h>

namespace materials {

  /// \brief Compute the refractive index or the extinction coefficient
  ///        for a given medium.
  /// \see http://refractiveindex.info/
  /// \see https://github.com/polyanskiy/refractiveindex.info-database/blob/master/database/doc/Dispersion%20formulas.pdf
  /// \see https://github.com/polyanskiy/refractiveindex.info-database/
  /// Typical validity wavelength range from UV to ~2.3 μm, depending of the
  /// parametrization (tabulated function or explicit formula).
  class refractive_index
    : public mygsl::i_unary_function
  {
  public:

    enum dispersion_formula_type {
      FORMULA_INVALID                   =  0, //!< Invalid formula
      FORMULA_CONSTANT                  =  1, //!< No dispersion (constant refractive index)
      FORMULA_DISPERSION_TABULATED      =  2, //!< Tabulated dispersion
      FORMULA_DISPERSION_RII_SELLMEIER  = 11, //!< Sellmeier formula (preferred)
      FORMULA_DISPERSION_RII_SELLMEIER2 = 12, //!< Sellmeier formula (alternative)
      FORMULA_DISPERSION_RII_POLYNOMIAL = 13,
      FORMULA_DISPERSION_RII_RII4       = 14,
      FORMULA_DISPERSION_RII_CAUCHY     = 15,
      FORMULA_DISPERSION_RII_GASES      = 16,
      FORMULA_DISPERSION_RII_HERZBERGER = 17,
      // FORMULA_DISPERSION_RII_RETRO      = 18,
      // FORMULA_DISPERSION_RII_EXOTIC     = 19
    };

    /// \brief Evaluation mode
    enum eval_mode_type {
      EVAL_N = 0,
      EVAL_K = 1
    };

    /// \brief Type alias for a dictionry of parameters
    typedef std::map<std::size_t, double> coeff_dict_type;

    /// Default constructor
    refractive_index();

    /// Destructor
    ~refractive_index() override;

    /// Check the refractive index evaluation mode
    bool is_eval_n() const;

    /// Check the extinction coefficient evaluation mode
    bool is_eval_k() const;

    /// Set to refractive index evaluation mode
    void set_eval_n();

    /// Set to extinction coefficient evaluation mode
    void set_eval_k();

    /// Check if validity range of lambda is set
    bool has_lambda_range() const;

    /// Set the validity range of lambda
    void set_lambda_range(const double lambda_min_, const double lambda_max_);

    /// Set the validity range of lambda
    void set_lambda_range(const datatools::real_range & lr_);

    /// Return the validity range of lambda
    const datatools::real_range & get_lambda_range() const;

    /// Use a constant refractive index (independant of lambda)
    /// and a constant extinction coefficient (optional)
    void make_constant(const double n_,
                       const double k_ = std::numeric_limits<double>::quiet_NaN());

    /// Use a tabulated refractive index and extension coefficient
    void make_tabulated(const std::string & n_tab_, const std::string & k_tab_ = "");

    /// Use a RII dispersion formula refractive index
    void make_dispersion_rii(const dispersion_formula_type formula_,
                             const coeff_dict_type & coeffs_,
                             const std::string & k_tab_ = "");

    /// Check if a RII coeff is set
    bool has_rii_coeff(const std::size_t) const;

    /// Return the RII coeff at given rank
    double get_rii_coeff(const std::size_t) const;

    /// Compute the refractive index from the vacuum wavelength (where E = hc/lambda)
    double eval_refractive_index(const double lambda_) const;

    /// Compute the extinction coefficient from the vacuum wavelength
    double eval_extinction_coefficient(const double lambda_) const;

    /// Compute the attenuation coefficient from the vacuum wavelength
    double eval_attenuation_coefficient(const double lambda_) const;

    /// Compute the penetration depth from the vacuum wavelength
    double eval_penetration_depth(const double lambda_) const;

    /// Compute the principal dispersion (from Fraunhofer F-, C-spectral lines)
    double compute_principal_dispersion() const;

    /// Compute the Abbe number (from Fraunhofer D-, F-, C-spectral lines)
    double compute_abbe_number() const;

    /// Compute the Abbe number (from Fraunhofer Cd, Hg spectral lines)
    double compute_abbe_number_e() const;

    /// Check if a domain of definition is set
    bool has_explicit_domain_of_definition() const override;

    /// Check if a value is in the domain of definition
    bool is_in_domain_of_definition(const double x_) const override;

    /// Check initialization status
    bool is_initialized() const override;

    /// Initialize from properties
    void initialize(const datatools::properties & config_,
                    const mygsl::unary_function_dict_type & /* functors_ */) override;

    /// Reset
    void reset() override;

  protected:

    double _eval(double) const override;

    double _eval_n(double) const;

    double _eval_k(double) const;

    void _build_n_tabfunc();

    void _build_k_tabfunc();

  private:

    dispersion_formula_type _formula_ = FORMULA_INVALID; //!< The dispersion formula
    eval_mode_type _eval_mode_ = EVAL_N; //!< Evaluation mode

    // Constant and simple formula:
    double _n0_; //!< Constant refractive indice
    double _k0_; //!< Constant extinction coefficient

    // Tabulated:
    std::string _n_table_; //!< Name of the file with tabulated n values (refractive index)
    std::string _k_table_; //!< Name of the file with tabulated k values (extinction coefficient)

    // Coefficients and validity range for RII formula (https://en.wikipedia.org/wiki/Sellmeier_equation):
    datatools::real_range _lambda_range_; //!< The validity range of wavelength
    coeff_dict_type _rii_c_;

    // Work data:
    std::shared_ptr<mygsl::tabulated_function> _n_tabfunc_; //!< Tabulated function for n
    std::shared_ptr<mygsl::tabulated_function> _k_tabfunc_; //!< Tabulated function for k

  };

} // namespace materials

#endif // MATERIALS_REFRACTIVE_INDEX_H

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

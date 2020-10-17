// \file mygsl/linear_regression.h

#ifndef MYGSL_LINEAR_REGRESSION_H
#define MYGSL_LINEAR_REGRESSION_H 1

// Standard library:
#include <vector>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.ipp>

// This project:
#include <mygsl/i_unary_function.h>
#include <mygsl/datapoint.h>

namespace mygsl {

  /// \brief The linear regression algorithm
  class linear_regression
  {
  public:

    /// \brief The serializable result data of the linear regression algorithm
    class fit_data : public datatools::i_serializable
    {

    public:

      /// Constructor
      fit_data();

      /// Destructor
      ~fit_data() override;

      /// Check if the fit data are valid
      bool is_valid() const;

      /// Check if the fit was weighted
      bool is_weighted() const;

      /// Check if the linear fit uses a constant parameter
      bool has_constant() const;

      /// Reset the fit data
      void reset();

      /// Return the slope parameter
      double get_slope() const;

      /// Return the constant parameter
      double get_constant() const;

      /// Return the number of degrees of freedom
      size_t get_ndof() const;

      /// Return the chi squared
      double get_chisq() const;

      /// Return the sum of squared residuals
      double get_sumsq() const;

      /// Return the error on the slope
      double get_slope_err() const;

      /// Return the error on the constant
      double get_constant_err() const;

    public:

      uint32_t  n;          /// Number of fitted points
      int32_t   status;     /// Status of the fit 0 == fit results; no fit otherwise
      bool   weighted;      /// Flag for a weighted fut
      bool   with_constant; /// Flag for a linear fit with a constant
      double c1;            /// Slope parameter
      double c0;            /// Constant parameter
      double cov00;         /// Covariance coefficient
      double cov01;         /// Covariance coefficient
      double cov11;         /// Covariance coefficient
      double sumsq;         /// Sum of squared residuals
      double chisq;         /// Chi-square

      DATATOOLS_SERIALIZATION_DECLARATION()

    };

    /// \brief A linear regression fit function
    class function : public i_unary_function {

    public:

      /// Constructor
      function(const fit_data &);

      /// Set the fit fata
      void set_fit_data(const fit_data &);

      /// Return the fit data
      const fit_data & get_fit_data() const;

      /// Evaluate the Y value and associated error for a given X from the result of the fit
      void eval_err(double x_, double & y_, double & yerr_) const;

    protected:

      /// Evaluate the Y value for a given X
      double _eval(double x_) const override;

    private:

      fit_data _fit_data_; /// The result of a linear regression fit

    };

  public:

    /// Minimum number of data points to perform the linear regression fit
    static const unsigned int MINIMUM_NUMBER_OF_DATA_POINTS = 2;

    /// Check initialization flag
    bool is_initialized() const;

    /// Check if a weighted fit can pe performed
    bool can_weighted() const;

    /// Return the result of the fit
    const fit_data & get_fit_data() const;

    /// Default constructor
    linear_regression();

    /// Construct from an array of datapoints (x,y,w) triplets
    linear_regression(const std::vector<datapoint> & p_);

    /// Construct from plain arrays of x, y and w values
    linear_regression(size_t npoints_,
                      const double * x_,
                      const double * y_,
                      const double * w_ = 0);

    /// Construct from vectors of x, y and w values
    linear_regression(const std::vector<double> & x_,
                      const std::vector<double> & y_,
                      const std::vector<double> & w_);

    /// Construct from vectors of x and y values (no weights)
    linear_regression(const std::vector<double> & x_,
                      const std::vector<double> & y_);

    /// Destructor
    virtual ~linear_regression();

    /// Initialize from an array of datapoints (x,y,w) triplets
    void init(const std::vector<datapoint> & p_);

    /// Initialize from plain arrays of x, y and optional w values
    void init(size_t npoints_,
              const double * x_,
              const double * y_,
              const double * w_ = 0);

    /// Initialize from vectors of x, y and w values
    void init(const std::vector<double> & x_,
              const std::vector<double> & y_,
              const std::vector<double> & w_);

    /// Construct from vectors of x and y values (no weights)
    void init(const std::vector<double> & x_,
              const std::vector<double> & y_);

    /// Reset
    void reset();

    /// Perform the linear unweighted fit
    bool fit_linear();

    /// Perform the linear weighted fit
    bool fit_weighted_linear();

    /// Perform the linear unweighted fit without constant parameter
    bool fit_linear_no_constant();

    /// Perform the linear weighted fit without constant parameter
    bool fit_weighted_linear_no_constant();

  private:

    bool        _initialized_; ///< initialization flag
    fit_data    _fit_data_; ///< Fit data result
    bool        _delete_; ///< Delete flag for temporary arrays
    double    * _x_; ///< Handle to working array
    double    * _y_; ///< Handle to working array
    double    * _w_; ///< Handle to working array
    bool        _can_weighted_; ///< Flag for weighted fit

  };

}

#endif // MYGSL_LINEAR_REGRESSION_H

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

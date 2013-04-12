// linear_regression.h

#ifndef MYGSL_LINEAR_REGRESSION_H_ 
#define MYGSL_LINEAR_REGRESSION_H_ 1

#include <vector>
#include <mygsl/i_unary_function.h>
#include <mygsl/datapoint.h>

namespace mygsl {
 
  class linear_regression : public i_unary_function
  {
    class fit_data
    {

    public:

      fit_data ();
      bool is_valid () const;
      bool is_weighted () const;
      bool has_constant () const;
      void reset ();
      double get_slope () const;
      double get_constant () const;
      size_t get_ndof () const;
      double get_chisq () const;
      double get_sumsq () const;
      double get_slope_err () const;
      double get_constant_err () const;

    public:

      int    status; // 0 == fit results; no fit otherwise
      size_t n;
      bool   weighted;
      bool   with_constant;
      double c0;
      double c1;
      double cov00;
      double cov01;
      double cov11;
      double sumsq;
      double chisq;

    };

  public:

    void eval_err (double x_, double & y_, double & yerr_) const;

    bool is_initialized () const;

    bool can_weighted () const;

    const fit_data & get_fit_data () const;

    linear_regression ();

    linear_regression (const std::vector<datapoint> & p_);

    linear_regression (size_t npoints_, 
                       const double * x_, 
                       const double * y_, 
                       const double * w_ = 0);

    linear_regression (const std::vector<double> & x_, 
                       const std::vector<double> & y_, 
                       const std::vector<double> & w_);

    linear_regression (const std::vector<double> & x_, 
                       const std::vector<double> & y_);
      
    virtual ~linear_regression ();

    void init (const std::vector<datapoint> & p_);

    void init (size_t npoints_, 
               const double * x_, 
               const double * y_, 
               const double * w_ = 0);

    void init (const std::vector<double> & x_, 
               const std::vector<double> & y_, 
               const std::vector<double> & w_);

    void init (const std::vector<double> & x_, 
               const std::vector<double> & y_);

    void reset ();

    bool fit_linear ();

    bool fit_weighted_linear ();

    bool fit_linear_no_constant ();

    bool fit_weighted_linear_no_constant ();

  protected:

    virtual double _eval(double x_) const;
      
  private:

    bool        _initialized_;
    fit_data    _fit_data_;
    bool        _delete_;
    double    * _x_;
    double    * _y_;
    double    * _w_;
    bool        _can_weighted_;

  };

}

#endif // MYGSL_LINEAR_REGRESSION_H_

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

// end of linear_regression.h

// -*- mode:c++; -*- 
// i_function.h

#ifndef MYGSL_I_FUNCTION_H_ 
#define MYGSL_I_FUNCTION_H_ 1

#include <iostream>
#include <limits>
#include <functional>
#include <string>
#include <vector>

#include <boost/cstdint.hpp>

#include <mygsl/ioutils.h>
#include <mygsl/interval.h>

namespace mygsl {


  /// \brief Abstract interface for unary functions : R -> R
  class i_unary_function : public std::unary_function<double,double> 
  {
  public:

    static const double DEFAULT_EPSILON;
    static const double AUTO_EPSILON;
    
    /// Typedef for a function C-style function :
    typedef double (*plain_function_type)(double);
    
    /// Typedef for a function C-style function with anonymous parameters :
    typedef double (*plain_function_with_parameters_type)(double, void*);

    /// \brief Options for writing sampled function in an ASCII stream
    enum write_options_type {
      wo_data_index = 0x1, /// Store as a data index for Gnuplot (2 blank lines at the end)
      wo_skip_nan   = 0x2, /// Do not print (x,y=f(x)) pairs  if y is a NaN
      wo_skip_inf   = 0x4, /// Do not print (x,y=f(x)) pairs  if y is a +/-infinity
      wo_skip_onzd  = 0x8, /// Do not print (x,y=f(x)) pairs if x lies out of the non-zero domain
      wo_skip_odod  = 0x10, /// Do not print (x,y=f(x)) pairs if x lies out of domain of definition
      wo_append     = 0x20, /// Append to an existing file
      wo_default    = 0
    };

    /// Constructor
    i_unary_function(double epsilon_ = 0.0);

    /// Destructor
    virtual ~i_unary_function();
    
    /// Check if the function has an explicit domain of definition (default: false)
    virtual bool has_explicit_domain_of_definition() const;

    /// Check if a value is in the domain of definition of the function (default: true)
    virtual bool is_in_domain_of_definition(double x_) const;

    /// The function evaluation method
    virtual double eval(double x_) const;

    /// The function evaluation method
    double evaluate (double x_) const;

    /// The function evaluation method with any value check
    virtual double eval_no_check(double x_) const;

    /// The minimum bound of the non-zero domain (default is minus infinity)
    virtual double get_non_zero_domain_min() const;

    /// Check the minimum bound of the non-zero domain
    bool has_non_zero_domain_min() const;
    
    /// The maximum bound of the non-zero domain (default is plus infinity)
    virtual double get_non_zero_domain_max() const;
    
    /// Check the maximum bound of the non-zero domain
    bool has_non_zero_domain_max() const;
    
    /// Check if a value is in the non-zero domain
    bool is_in_non_zero_domain(double x_) const;
    
    /// Check if a value is in the zero domain
    bool is_in_zero_domain(double x_) const;

    /// Standard C++ functor interface 
    double operator() (double x_) const;

    /// Write the (x,y=f(x)) value pairs in an ASCII stream :
    void write_ascii(std::ostream & fout_, 
                     double min_, double max_, unsigned int nsamples_, 
                     int x_precision_ = 16,
                     int fx_precision_ = 16,
                     uint32_t options_ = wo_default) const;

    /// Write the (x,y=f(x)) value pairs in an ASCII file (typical Gnuplot input) :
    void write_ascii_file(const std::string & filename_, 
                          double min_, double max_, unsigned int nsamples_,
                          int x_precision_ = 16,
                          int fx_precision_ = 16,
                          uint32_t options_ = wo_default) const;
     
    /// A generic static function to feed the GSL gsl_function interface:
    static double g_function(double x_, void * functor_);

    void set_epsilon(double);

    double get_epsilon() const;

  protected:

    void _compute_auto_epsilon();

    /// The function evaluation abstract method
    virtual double _eval(double x_) const = 0;

  private :

    double _epsilon_;
    
  };

  class plain_function_wrapper : public i_unary_function
  {
  public:

    void set_plain_function(const plain_function_type &);

    plain_function_wrapper(const plain_function_type &);

    virtual ~plain_function_wrapper();

  protected:

    virtual double _eval(double x_) const;

  private:

    const plain_function_type * _plain_function_;

  };

  class function_with_domain : public i_unary_function
  {
  public:

    void set_domain_of_definition(const interval & domain_);

    const interval & get_domain_of_definition() const;

    function_with_domain(const i_unary_function & functor_);

    function_with_domain(const i_unary_function & functor_, const interval & domain_);

    virtual ~function_with_domain();

    virtual bool has_explicit_domain_of_definition() const;

    virtual bool is_in_domain_of_definition(double x_) const;

  protected:

    virtual double _eval(double x_) const;
    
  private:

    const i_unary_function * _functor_;
    interval                 _domain_of_definition_;

  };

} // namespace mygsl

#endif // MYGSL_I_UNARY_FUNCTION_H_

// end of i_unary_function.h

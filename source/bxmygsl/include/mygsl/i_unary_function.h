//! \file mygsl/i_unary_function.h

#ifndef MYGSL_I_UNARY_FUNCTION_H
#define MYGSL_I_UNARY_FUNCTION_H 1

// Standard library:
#include <iostream>
#include <limits>
#include <functional>
#include <string>
#include <vector>
#include <map>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/handle.h>
#include <datatools/factory_macros.h>
#include <datatools/i_tree_dump.h>

// This project:
#include <mygsl/ioutils.h>
#include <mygsl/interval.h>

namespace datatools {
  // Forward class declaration:
  class properties;
}

// Registration:

#define MYGSL_UNARY_FUNCTOR_REGISTRATION_INTERFACE(UNARY_FUNCTOR_CLASS) \
  private:                                                              \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(::mygsl::i_unary_function, UNARY_FUNCTOR_CLASS) \
  /**/

#define MYGSL_UNARY_FUNCTOR_REGISTRATION_IMPLEMENT(UNARY_FUNCTOR_CLASS, UNARY_FUNCTOR_CLASS_ID) \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(::mygsl::i_unary_function, UNARY_FUNCTOR_CLASS, UNARY_FUNCTOR_CLASS_ID) \
  /**/

namespace mygsl {

  // Forward class declaration:
  class i_unary_function;

  //! Alias for a handle to an unary function object
  typedef datatools::handle<i_unary_function> unary_function_handle_type;

  //! Alias for disctionary of handles to unary function objects
  typedef std::map<std::string, unary_function_handle_type> unary_function_dict_type;

  //! \brief Abstract interface for unary functions : R -> R
  class i_unary_function : public std::unary_function<double,double>,
                           public datatools::i_tree_dumpable
  {
  public:

    //! Default error
    static const double DEFAULT_EPSILON;

    //! Value for automatic computation of the error
    static const double AUTO_EPSILON;

    //! \brief Options for writing sampled function in an ASCII stream
    enum write_options_type {
      wo_none       = 0,
      wo_data_index = 0x1, //!< Store as a data index for Gnuplot (2 blank lines at the end)
      wo_skip_nan   = 0x2, //!< Do not print (x,y=f(x)) pairs  if y is a NaN
      wo_skip_inf   = 0x4, //!< Do not print (x,y=f(x)) pairs  if y is a +/-infinity
      wo_skip_onzd  = 0x8, //!< Do not print (x,y=f(x)) pairs if x lies out of the non-zero domain
      wo_skip_odod  = 0x10, //!< Do not print (x,y=f(x)) pairs if x lies out of domain of definition
      wo_append     = 0x20, //!< Append to an existing file
      wo_default    = wo_none
    };

    //! Constructor
    i_unary_function(double epsilon_ = 0.0);

    //! Destructor
    virtual ~i_unary_function();

    //! Check if the function has an explicit domain of definition (default: false)
    virtual bool has_explicit_domain_of_definition() const;

    //! Check if a value is in the domain of definition of the function (default: true)
    virtual bool is_in_domain_of_definition(double x_) const;

    //! The function evaluation method
    virtual double eval(double x_) const;

    //! The function evaluation method
    double evaluate(double x_) const;

    //! The function evaluation method with any value check
    virtual double eval_no_check(double x_) const;

    //! The minimum bound of the non-zero domain (default is minus infinity)
    virtual double get_non_zero_domain_min() const;

    //! Check the minimum bound of the non-zero domain
    bool has_non_zero_domain_min() const;

    //! The maximum bound of the non-zero domain (default is plus infinity)
    virtual double get_non_zero_domain_max() const;

    //! Check the maximum bound of the non-zero domain
    bool has_non_zero_domain_max() const;

    //! Check if a value is in the non-zero domain
    bool is_in_non_zero_domain(double x_) const;

    //! Check if a value is in the zero domain
    bool is_in_zero_domain(double x_) const;

    //! Check if the functor has an explicit zero domain
    bool has_zero_domain() const;

    //! Standard C++ functor interface
    double operator() (double x_) const;

    //! Write the (x,y=f(x)) value pairs in an ASCII stream :
    void write_ascii(std::ostream & fout_,
                     double min_, double max_, unsigned int nsamples_,
                     int x_precision_ = 16,
                     int fx_precision_ = 16,
                     uint32_t options_ = wo_default) const;

    //! Write the (x,y=f(x)) value pairs in an ASCII file (typical Gnuplot input) :
    void write_ascii_file(const std::string & filename_,
                          double min_, double max_, unsigned int nsamples_,
                          int x_precision_ = 16,
                          int fx_precision_ = 16,
                          uint32_t options_ = wo_default) const;

    //! A generic static function to feed the GSL gsl_function interface:
    static double g_function(double x_, void * functor_);

    //! Set the precision
    void set_epsilon(double);

    //! Return the precision
    double get_epsilon() const;

    //! Simple initialization
    void initialize_simple();

    //! Initialization from a container of parameters
    void initialize_standalone(const datatools::properties & config_);

    //! Initialization from a container of parameters and a dictionary of functors
    virtual void initialize(const datatools::properties & config_,
                            unary_function_dict_type & functors_);

    //! Reset the functor
    virtual void reset();

    //! Check initialization status
    virtual bool is_initialized() const;

    //! Smart printing
    virtual void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const;

  protected:

    //! The function evaluation abstract method
    virtual double _eval(double x_) const = 0;

    void _base_initialize(const datatools::properties & config_,
                          unary_function_dict_type & functors_);

    void _base_reset();

    void _compute_auto_epsilon();

    //! Set defaults attributes
    void _set_defaults();

  private :

    double _epsilon_; //!< Precision

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(i_unary_function)

  };

  //! \brief The identity function
  class identity_function : public i_unary_function
  {
  public:

    //! Default constructor
    identity_function();

    //! Destructor
    virtual ~identity_function();

  protected:

    //! Evaluation method
    virtual double _eval(double x_) const;

  private:

    //! Registration of the functor class
    MYGSL_UNARY_FUNCTOR_REGISTRATION_INTERFACE(identity_function)

  };

} // namespace mygsl

#endif // MYGSL_I_UNARY_FUNCTION_H

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

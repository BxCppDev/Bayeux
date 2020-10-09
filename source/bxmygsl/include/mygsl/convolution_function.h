//! \file  mygsl/convolution_function.h
//! \brief A function convolution of two other functions
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Bayeux.
//
// Bayeux is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux. If not, see <http://www.gnu.org/licenses/>.

#ifndef MYGSL_CONVOLUTION_FUNCTION_H
#define MYGSL_CONVOLUTION_FUNCTION_H

// This project:
#include <mygsl/i_unary_function.h>

// Third party:
#include <boost/noncopyable.hpp>

namespace mygsl {

  //! \brief Convolution function
  /**
   * \code
   *  h(t) = (f * g)(t) = \int_-inf^+inf f(tau) g(t-tau) dtau
   * \endcode
   * where f is the first function
   * and g is the second function.
   *
   *
   */
  /// \brief Convoluted signal
  class convolution_function
    : public i_unary_function
    , private boost::noncopyable
  {
  public:

    //! Default constructor
    convolution_function();

    //! Constructor
    convolution_function(const i_unary_function & f_,
                         const i_unary_function & g_,
                         std::size_t size_ = 1000);

    //! Constructor
    convolution_function(const const_unary_function_handle_type & hcf_,
                         const const_unary_function_handle_type & hcg_,
                         std::size_t size_ = 1000);

    //! Destructor
    ~convolution_function() override;

    //! Check initialization status
    bool is_initialized() const override;

    //! Initialization from a container of parameters and a dictionary of functors
    void initialize(const datatools::properties & config_,
                            const unary_function_dict_type & functors_) override;

    //! Reset the function
    void reset() override;

    //! Set the first functor
    void set_f(const i_unary_function &);

    //! Set the second functor
    void set_g(const i_unary_function &);

    //! Check the first functor
    bool has_f() const;

    //! Check the second functor
    bool has_g() const;

    //! Return the first functor
    const i_unary_function & get_f() const;

    //! Return the second functor
    const i_unary_function & get_g() const;

    //! Set limit
    void set_limit(std::size_t size_);

    //! Return the limit
    std::size_t get_limit() const;

    //! Smart printing
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

    /// \brief Interface parameters for GSL function
    class convolution_term : public i_unary_function
    {
    public:

      /// Constructor
      convolution_term(const convolution_function & cf_, double t_);

      /// The minimum bound of the non-zero domain
      /* virtual */ double get_non_zero_domain_min() const override;

      /// The minimum bound of the non-zero domain
      /* virtual */ double get_non_zero_domain_max() const override;

      /// Smart print
      void print(std::ostream & = std::clog, const std::string & title_ = "") const;

    protected:

      /// Function interface
      double _eval(double) const override;

    private:

      const convolution_function * _cs_; ///< handle to a convoluted signal object
      double _t_;
      double _tmin_;
      double _tmax_;

    };

    /// Interface integrand function for GSL function
    static double convolution_func(double t_, void * params_);

  protected:

    //! Evaluation
    double _eval(double x_) const override;

    void _at_init();

    void _at_reset();

  private:

    unary_function_handle _f_; //!< First functor
    unary_function_handle _g_; //!< Second functor
    std::size_t           _limit_ = 0; //!< Size of the working space

    //! Forward declaration of the private Pimpl
    class gsl_pimpl;
    gsl_pimpl * _gsl_work_ = nullptr; //!< Hidden GSL resources for convolution algorithm

    //! Registration of the functor class
    MYGSL_UNARY_FUNCTOR_REGISTRATION_INTERFACE(convolution_function)

  };

} // end of namespace mygsl

#endif // MYGSL_CONVOLUTION_FUNCTION_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

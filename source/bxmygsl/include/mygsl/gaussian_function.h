//! \file  mygsl/gaussian_function.h
//! \brief A gaussian function
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef MYGSL_GAUSSIAN_FUNCTION_H
#define MYGSL_GAUSSIAN_FUNCTION_H

// Standard library:
#include <limits>

// This project:
#include <mygsl/i_unary_function.h>

namespace mygsl {

  //! \brief Gaussian function
  class gaussian_function : public i_unary_function
  {
  public:

    //! Default constructor
    gaussian_function();

    //! Constructor
    gaussian_function(double sigma_,
                      double mu_ = 0.0,
                      double amplitude_= std::numeric_limits<double>::quiet_NaN());

    //! Destructor
    virtual ~gaussian_function();

    //! Check initialization status
    virtual bool is_initialized() const;

    //! Initialization from a container of parameters and a dictionary of functors
    virtual void initialize(const datatools::properties & config_,
                            unary_function_dict_type & functors_);

    //! Reset the function
    void reset();

    //! Set the sigma
    void set_sigma(double sigma_);

    //! Set the center
    void set_mu(double mu_);

    //! Set the amplitude
    void set_amplitude(double amplitude_);

    //! Smart printing
    virtual void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const;

  protected :

    //! Evaluation
    double _eval(double x_) const;

    //! Set default attributes values
    void _set_defaults();

   private:

    double _mu_;        //!< Mean
    double _sigma_;     //!< Sigma
    double _amplitude_; //!< Amplitude

    //! Registration of the functor class
    MYGSL_UNARY_FUNCTOR_REGISTRATION_INTERFACE(gaussian_function)

  };

} // end of namespace mygsl

#endif // MYGSL_GAUSSIAN_FUNCTION_H

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

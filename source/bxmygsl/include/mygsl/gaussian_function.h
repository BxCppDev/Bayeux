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

// Third party:
// - Bayeux/datatools :
#include <datatools/i_cloneable.h>

// This project:
#include <mygsl/i_unary_function.h>

namespace mygsl {

  //! \brief Gaussian function
  class gaussian_function
    : public i_unary_function,
      public datatools::i_cloneable
  {
  public:

    //! Default constructor
    gaussian_function();

    //! Constructor
    gaussian_function(double sigma_,
                      double mu_);

    //! Constructor
    gaussian_function(double sigma_,
                      double mu_,
                      double amplitude_);

    //! Destructor
    ~gaussian_function() override;

    //! Check initialization status
    bool is_initialized() const override;

    //! Initialization from a container of parameters and a dictionary of functors
    void initialize(const datatools::properties & config_,
                            const unary_function_dict_type & functors_) override;

    //! Reset the function
    void reset() override;

    //! Set the sigma
    void set_sigma(double sigma_);

    //! Return the sigma
    double get_sigma() const;

    //! Set the center
    void set_mu(double mu_);

    //! Return the mu
    double get_mu() const;

    //! Check if amplitude is forced
    bool has_amplitude() const;

    //! Set the amplitude
    void set_amplitude(double amplitude_);

    //! Return the amplitude
    double get_amplitude() const;

    //! Reset the amplitude
    void reset_amplitude();

    //! Smart printing
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

  protected:

    //! Evaluation
    double _eval(double x_) const override;

    //! Set default attributes values
    void _set_defaults();

  private:

    //! Private initialization
    void _init_();

  private:

    // Configuration:
    double _mu_;        //!< Mean
    double _sigma_;     //!< Sigma
    double _amplitude_; //!< Peak amplitude at x=mean (optional)

    // Working data:
    double _factor_; //!< Scaling factor


    //! Registration of the functor class
    MYGSL_UNARY_FUNCTOR_REGISTRATION_INTERFACE(gaussian_function)

    //! Cloneable interface
    DATATOOLS_CLONEABLE_DECLARATION(gaussian_function)

  };

} // end of namespace mygsl

#endif // MYGSL_GAUSSIAN_FUNCTION_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

//! \file  mygsl/triangle_function.h
//! \brief A triangle function
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

#ifndef MYGSL_TRIANGLE_FUNCTION_H
#define MYGSL_TRIANGLE_FUNCTION_H

// Standard library:
#include <limits>

// Third party:
// - Bayeux/datatools :
#include <datatools/i_cloneable.h>

// This project:
#include <mygsl/i_unary_function.h>

namespace mygsl {

  //! \brief General asymmetric triangle function
  //!
  //!  @code
  //!     ^
  //!     :      head     tail
  //!     :     width     width
  //!     :       :<-->+<--->:      ^
  //!     :       :   /:`    :      :
  //!     :       :  / : `   :      :
  //!     :       : /  :  `  :      : amplitude
  //!     :       :/   :   ` :      :
  //!  ---+-------+....+.....+------v-----> x
  //!    O:          center
  //!     :
  //!  @endcode
  class triangle_function
    : public i_unary_function,
      public datatools::i_cloneable
  {
  public:

    //! Default constructor
    triangle_function();

    //! Constructor of a symmetric triangle function
    triangle_function(double full_width_,
                      double center_,
                      double amplitude_);

    //! Constructor of a generic asymmetric triangle function
    triangle_function(double head_width_,
                      double tail_width_,
                      double center_,
                      double amplitude_);

    //! Destructor
    ~triangle_function() override;

    //! Check initialization status
    bool is_initialized() const override;

    //! Initialization from a container of parameters and a dictionary of functors
    void initialize(const datatools::properties & config_,
                            const unary_function_dict_type & functors_) override;

    //! Reset the function
    void reset() override;

    //! Set the head width
    void set_head_width(double head_width_);

    //! Return the head width
    double get_head_width() const;

    //! Set the tail width
    void set_tail_width(double tail_width_);

    //! Return the tail width
    double get_tail_width() const;

    //! Set the center
    void set_center(double center_);

    //! Return the center
    double get_center() const;

    //! Set the amplitude
    void set_amplitude(double amplitude_);

    //! Return the amplitude
    double get_amplitude() const;

    //! The minimum bound of the non-zero domain (default is plus infinity)
    double get_non_zero_domain_min() const override;

    //! The maximum bound of the non-zero domain (default is plus infinity)
    double get_non_zero_domain_max() const override;

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

    double _center_;     //!< Center
    double _head_width_; //!< Width of the head part
    double _tail_width_; //!< Width of the tail part
    double _amplitude_;  //!< Max amplitude

    //! Registration of the functor class
    MYGSL_UNARY_FUNCTOR_REGISTRATION_INTERFACE(triangle_function)

    //! Cloneable interface
    DATATOOLS_CLONEABLE_DECLARATION(triangle_function)

  };

} // end of namespace mygsl

#endif // MYGSL_TRIANGLE_FUNCTION_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

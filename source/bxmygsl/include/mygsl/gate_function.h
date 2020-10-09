//! \file  mygsl/gate_function.h
//! \brief A gate function
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

#ifndef MYGSL_GATE_FUNCTION_H
#define MYGSL_GATE_FUNCTION_H

// Standard library:
#include <limits>

// Third party:
// - Bayeux/datatools :
#include <datatools/i_cloneable.h>

// This project:
#include <mygsl/i_unary_function.h>

namespace mygsl {

  //! \brief Gate function
  //!
  //!  @code
  //!     ^
  //!     :
  //!     :           width
  //!     :       <- - - - - ->
  //!     :       +-----------+   ^
  //!     :       |           |   :
  //!     :       |           |   : amplitude
  //!     :       |           |   :
  //!  ---+-------+.....+.....+------------> x
  //!    O:            center
  //!     :
  //!  @endcode
  class gate_function
    : public i_unary_function,
      public datatools::i_cloneable
  {
  public:

    //! Default constructor
    gate_function();

    //! Constructor
    gate_function(double width_,
                  double center_ = 0.0,
                  double amplitude_ = std::numeric_limits<double>::quiet_NaN());

    //! Destructor
    ~gate_function() override;

    //! Check initialization status
    bool is_initialized() const override;

    //! Initialization from a container of parameters and a dictionary of functors
    void initialize(const datatools::properties & config_,
                            const unary_function_dict_type & functors_) override;

    //! Reset the function
    void reset() override;

    //! Set the width
    void set_width(double width_);

    //! Return the width
    double get_width() const;

    //! Set the center
    void set_center(double center_);

    //! Return the enter
    double get_center() const;

    //! Set the amplitude
    void set_amplitude(double amplitude_);

    //! Return the mplitude
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

  protected :

    //! Evaluation
    double _eval(double x_) const override;

    //! Set default attributes values
    void _set_defaults();

   private:

    double _center_;    //!< Center of the gate
    double _width_;     //!< Width of the gate
    double _amplitude_; //!< Amplitude of the gate

    //! Registration of the functor class
    MYGSL_UNARY_FUNCTOR_REGISTRATION_INTERFACE(gate_function)

    //! Cloneable interface
    DATATOOLS_CLONEABLE_DECLARATION(gate_function)

  };

} // end of namespace mygsl

#endif // MYGSL_GATE_FUNCTION_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

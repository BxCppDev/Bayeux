/// \file mctools/signal/triangle_gate_signal_shape.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2016-09-27
 * Last modified : 2016-09-27
 *
 * Copyright (C) 2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *   A class representing a gate signal shape with arbitrary polarity, rise
 *   and fall times, starting time and charge. Edges have triangular shapes.
 *
 * History:
 *
 */

#ifndef MCTOOLS_SIGNAL_TRIANGLE_GATE_SIGNAL_SHAPE_H
#define MCTOOLS_SIGNAL_TRIANGLE_GATE_SIGNAL_SHAPE_H

// Third party:
// - Bayeux/datatools :
#include <datatools/i_cloneable.h>
// - Bayeux/mygsl:
#include <mygsl/i_unary_function.h>

//  This project:
#include <mctools/signal/utils.h>

namespace mctools {

  namespace signal {

    //! \brief Triangular gate signal shape
    /**
     * The signal shape :
     *
     * @code
     *              duration
     *          :<---------------------->:
     *         t0    t1           t2     t3
     * ---------+------+----------+------+------------------> time
     *       :   \     :          :     /
     *       :    \ Q  :          :    /
     *       :     \   :          :   /
     *  amplitude   \  :          :  /  polarity == negative
     *       :       \ :          : /
     *       :        \:          :/
     *       v.........\__________/
     *
     * @endcode
     *
     */
    class triangle_gate_signal_shape
      : public mygsl::i_unary_function,
        public datatools::i_cloneable
    {
    public:

      //! Check the validity
      bool is_valid() const;

      //! Default constructor
      triangle_gate_signal_shape();

      //! Destructor
      ~triangle_gate_signal_shape() override;

      //! Initialization
      void initialize(const datatools::properties & config_,
                              const mygsl::unary_function_dict_type & functors_) override;

      //! Reset
      void reset() override;

      //! Check initialization status
      bool is_initialized() const override;

      //! Set the polarity of the signal
      void set_polarity(polarity_type);

      //! Return the polarity of the signal
      polarity_type get_polarity() const;

      //! Reset the polarity of the signal
      void reset_polarity();

      //! Return the rise time of the signal
      double get_rise_time() const;

      //! Return the fall time of the signal
      double get_fall_time() const;

      //! Set the amplitude of the signal
      void set_amplitude(double);

      //! Return the amplitude of the signal
      double get_amplitude() const;

      //! Return the charge of the signal
      double get_q() const;

      //! Set the starting time of the signal
      void set_t0(double);

      //! Return the starting time of the signal
      double get_t0() const;

      //! Set the starting time of the plateau
      void set_t1(double);

      //! Return the starting time of the plateau
      double get_t1() const;

      //! Set the stopping time of the plateau
      void set_t2(double);

      //! Return the stopping time of the plateau
      double get_t2() const;

      //! Return the stopping time of the signal
      double get_t3() const;

      //! Set the stopping time of the signal
      void set_t3(double);

      //! Return the first peak time of the signal
      double get_tpeak() const;

      //! The minimum bound of the non-zero domain (default is minus infinity)
      double get_non_zero_domain_min() const override;

      //! The maximum bound of the non-zero domain (default is plus infinity)
      double get_non_zero_domain_max() const override;

      //! Return the width of the signal
      double get_duration() const;

      //! Smart printing
      void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const override;

    protected:

      //! Set default attributes
      void _set_defaults();

      //! Automatically compute working data parameters
      void _compute_parameters();

      //! Evaluation from parameters
      double _eval(double t_) const override;

    private:

      // Configuration:
      polarity_type _polarity_ = POLARITY_UNKNOWN; //!< Polarity of the signal
      double        _t0_;         //!< Start time
      double        _t1_;         //!< Plateau start time
      double        _t2_;         //!< Plateau stop time
      double        _t3_;         //!< Stop time
      double        _amplitude_;  //!< Amplitude of the signal

      // Working data:
      double _q_;      //!< Charge
      double _a_rise_; //!< Linear slope of the rising edge
      double _b_rise_; //!< Intercept of the rising edge
      double _a_fall_; //!< Linear slope of the falling edge
      double _b_fall_; //!< Intercept of the falling edge

      //! Registration of the functor class
      MYGSL_UNARY_FUNCTOR_REGISTRATION_INTERFACE(triangle_gate_signal_shape)

      //! Cloneable interface
      DATATOOLS_CLONEABLE_DECLARATION(triangle_gate_signal_shape)

    };

  } // end of namespace signal

} // end of namespace mctools

#endif // MCTOOLS_SIGNAL_TRIANGLE_GATE_SIGNAL_SHAPE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

/// \file mctools/signal/triangle_signal_shape.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2015-04-10
 * Last modified : 2015-04-10
 *
 * Copyright (C) 2015 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   A class representing a triangular signal shape with arbitrary polarity, rise and fall times
 *   and using external parameters for timing and amplitude
 *
 * History:
 *
 */

#ifndef MCTOOLS_SIGNAL_TRIANGLE_SIGNAL_SHAPE_H
#define MCTOOLS_SIGNAL_TRIANGLE_SIGNAL_SHAPE_H

// Third party:
// - Boost/mygsl:
#include <boost/scoped_ptr.hpp>
// - Bayeux/mygsl:
#include <mygsl/i_unary_function_with_parameters.h>

//  This project:
#include <mctools/signal/utils.h>

namespace mygsl {

  // Forward class declaration:
  class triangle_function;

}

namespace mctools {

  namespace signal {

    //! \brief Triangular signal shape
    /**
     * The signal shape :
     *
     * @code
     *              duration
     *          :<------------>:
     *         t0    tpeak     t1
     * ---------+------+-------+------------------> time
     *       :   \     :      /
     *       :    \ Q  :     /
     *       :     \   :    /
     *  amplitude   \  :   /  polarity == negative
     *       :       \ :  /
     *       :        \: /
     *       v.........\/
     *
     * @endcode
     *
     */
    class triangle_signal_shape : public mygsl::i_unary_function_with_parameters
    {
    public:

      //! Check the validity
      bool is_valid() const;

      //! Default constructor
      triangle_signal_shape();

      //! Destructor
      virtual ~triangle_signal_shape();

      //! Initialization
      virtual void initialize(const datatools::properties & config_,
                              mygsl::unary_function_dict_type & functors_);

      //! Reset
      virtual void reset();

      //! Check initialization status
      virtual bool is_initialized() const;

      //! Set the polarity of the signal
      void set_polarity(polarity_type);

      //! Return the polarity of the signal
      polarity_type get_polarity() const;

      //! Set the rise time of the signal
      void set_rise_time(double);

      //! Return the rise time of the signal
      double get_rise_time() const;

      //! Set the fall time of the signal
      void set_fall_time(double);

      //! Return the fall time of the signal
      double get_fall_time() const;

      //! The minimum bound of the non-zero domain (default is minus infinity)
      virtual double get_non_zero_domain_min() const;

      //! The maximum bound of the non-zero domain (default is plus infinity)
      virtual double get_non_zero_domain_max() const;

      //! Return the amplitude of the signal
      double get_amplitude() const;

      //! Return the start time of the signal
      double get_t0() const;

      //! Return the start time of the signal
      double get_t1() const;

      //! Return the width of the signal
      double get_duration() const;

      //! Return the peak time of the signal
      double get_tpeak() const;

      //! Smart printing
      virtual void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const;

    protected:

      //! Set default attributes
      void _set_defaults();

      //! Update internal/working data when parameters change
      void _at_parameters_change();

      //! Evaluation from parameters
      double _eval_from_parameters(double x_) const;

    private:

      // Configuration:
      polarity_type _polarity_;   //!< Polarity of the signal
      double        _rise_time_;  //!< Signal rise time (10%-90%)
      double        _fall_time_;  //!< Signal fall time (10%-90%)

      // Extracted parameters:
      int _iq_;     //!< Index of the 'q' charge parameter extracted from a store
      int _it0_;    //!< Index of the 't0' start time parameter extracted from a store
      double _q_;   //!< Current value of the charge parameter extracted from a store
      double _t0_;  //!< Current value of the start time parameter extracted from a store

      // Working data:
      double _tpeak_;     //! Peak time of the signal
      double _t1_;        //! Stop time of the signal
      double _amplitude_; //! Amplitude of the signal
      boost::scoped_ptr<mygsl::triangle_function> _shape_; //!< Shape of the signal

      //! Registration of the functor class
      MYGSL_UNARY_FUNCTOR_REGISTRATION_INTERFACE(triangle_signal_shape)


    };

  } // end of namespace signal

} // end of namespace mctools

#endif // MCTOOLS_SIGNAL_TRIANGLE_SIGNAL_SHAPE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

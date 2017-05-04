/// \file genbb_help/fermi_function.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-06-06
 * Last modified: 2014-06-06
 *
 * License:
 * Copyright 2014 F. Mauger
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
 *   Fermi function.
 *
 * History:
 *
 */

#ifndef GENBB_HELP_FERMI_FUNCTION_H
#define GENBB_HELP_FERMI_FUNCTION_H 1

namespace genbb {

  /// \brief Fermi function for beta+/- decays
  class fermi_function
  {
  public:

    /// \brief Approximation mode for the calculation of the Fermi function
    enum mode_type {
      MODE_NON_RELATIVISTIC = 0x1,      //!< Use the non relativistic approximation
      MODE_SPHERICALLY_SYMMETRIC = 0x2, //!< USe the "spherically symmetric" approximation
      MODE_SHAPE_ONLY = 0x10            //!< Use only the unnormalized shape of the Fermi function
    };

    /// \brief Decay type
    enum decay_type {
      DT_INVALID    = 0,
      DT_BETA_MINUS = 1,
      DT_BETA_PLUS  = 2
    };

    /// Check validity
    bool is_valid() const;

    /// Check beta plus decay type
    bool is_beta_plus() const;

    /// Check beta minus decay type
    bool is_beta_minus() const;

    /// Default constructor
    fermi_function();

    /// Constructor
    fermi_function(int z_,
                   int a_,
                   decay_type beta_type_,
                   unsigned int mode_,
                   double ke_cut_ = -1.0);

    /// Set the function parameters
    void set(int z_,
             int a_,
             decay_type beta_type_,
             unsigned int mode_,
             double ke_cut_ = -1.0);

    /// Reset the function parameters
    void reset();

    /// Evaluation
    double operator()(double ke_) const;

    /// Non relativistic approximation
    static double eval_non_relativistic_approx(int z_daughter_,
                                               decay_type type_,
                                               double ke_cut_,
                                               bool shape_only_,
                                               double ke_);

    /// Spherically symmetric wavefunctions approximation
    static double eval_spherically_symmetric_approx(int z_daughter_,
                                                    int a_daughter_,
                                                    decay_type type_,
                                                    double ke_cut_,
                                                    bool shape_only_,
                                                    double ke_);

  private:

    unsigned int       _mode_;       //!< Calculation mode
    int                _Z_;          //!< Z of the daughter nucleus
    int                _A_;          //!< A of the daughter nucleus
    decay_type         _decay_type_; //!< Beta decay type (<0: beta minus, >0: beta plus)
    double             _ke_cut_;     //!< Low energy cut
  };

} // end of namespace genbb

#endif // GENBB_HELP_FERMI_FUNCTION_H

// Local Variables: --
// mode: c++ --
// End: --

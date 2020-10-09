/// \file genbb_help/nuclear_level.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-06-08
 * Last modified: 2014-06-08
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
 *   Description of a nuclear level.
 *
 * History:
 *
 */

#ifndef GENBB_HELP_NUCLEAR_LEVEL_H
#define GENBB_HELP_NUCLEAR_LEVEL_H 1

// Standard library:
#include <string>
#include <vector>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/handle.h>
#include <datatools/i_tree_dump.h>

namespace genbb {

  // Forward declaration
  class nuclear_decay;

  /// \brief Description of a nuclear level
  class nuclear_level  : public datatools::i_tree_dumpable
  {
  public:

    /// Handle to a nuclear level
    typedef datatools::handle<nuclear_level> handle_type;
    /// Handle to a nuclear decay
    typedef datatools::handle<nuclear_decay> decay_handle_type;

    /// \brief Parity values
    enum parity_type {
      PARITY_INVALID = 0, //!< Invalid value
      PARITY_MINUS   = 1, //!< Negative parity
      PARITY_PLUS    = 2, //!< Positive parity
      PARITY_UNKNOWN = 3  //!< Unknown parity
    };

    /// \brief A decay channel attached to the level
    class decay_channel {
    public:

      /// Default constructor
      decay_channel();

      /// Destructor
      virtual ~decay_channel();

      /// Initialization
      void initialize(const nuclear_decay &, double, double);

      /// Reset
      void reset();

      /// Check validity
      bool is_valid() const;

      /// Set the nuclear decay
      void set_decay(const nuclear_decay &);

      /// Set the branching ratio
      void set_branching_ratio(double);

      /// Set the cumulative probability
      void set_cumul_prob(double);

      /// Return the decay
      const nuclear_decay & get_decay() const;

      /// Return the branching ratio
      double get_branching_ratio() const;

      /// Return the cumulative probability
      double get_cumul_prob() const;

    private:

      const nuclear_decay * _decay_; //!< Handle to the nuclear decay
      double _branching_ratio_;      //!< Branching ratio
      double _cumul_prob_;           //!< Cumulative probability

    };

    /// A collection of decay modes
    typedef std::vector<decay_channel> decay_channels_col_type;

    /// Return a label associated to a parity value
    static std::string label_from_parity(parity_type);

    /// Return the parity value associated to a label
    static parity_type parity_from_label(const std::string &);

    /// Return a label associated to a spin value
    static std::string label_from_spin(double);

    /// Return the spin value associated to a label
    static double spin_from_label(const std::string &);

    /// Default constructor
    nuclear_level();

    /// Desstructor
    ~nuclear_level() override;

    /// Check validity
    bool is_valid() const;

    /// Set the number of nucleons
    void set_A(int);

    /// Set the number of protons
    void set_Z(int);

    /// Set the spin
    void set_spin(double);

    /// Set the parity
    void set_parity(parity_type);

    /// Set the half_life
    void set_half_life(double);

    /// Set the width
    void set_width(double);

    /// Make the level stable
    void set_stable();

    /// Set the energy
    void set_energy(double);

    /// Set the fundamental energy
    void set_fundamental();

    /// Return the number of nucleons
    int get_A() const;

    /// Return the number of protons
    int get_Z() const;

    /// Return the element symbol
    const std::string & get_element_symbol() const;

    /// Return the number of neutrons
    int get_N() const;

    /// Check if the spin is known
    bool spin_is_known() const;

    /// Check if the parity is known
    bool parity_is_known() const;

    /// Return the spin
    double get_spin() const;

    /// Return the parity
    parity_type get_parity() const;

    /// Return the half_life
    double get_half_life() const;

    /// Return the mean life time
    double get_mean_life_time() const;

    /// Return the level width
    double get_width() const;

    /// Check if the level is stable
    bool is_stable() const;

    /// Return the excitation energy
    double get_energy() const;

    /// Check if the level is fundamental
    bool is_fundamental() const;

    /// Check the decay table
    bool has_decay_table() const;

    /// Add a decay channel
    void add_decay_channel(const nuclear_decay & decay_, double branching_ratio_);

    /// Return the number of decay channels
    size_t get_number_of_decay_channels() const;

    /// Return the decay channel at a given index
    const decay_channel & get_decay_channel(int i_) const;

    /// Return auxiliary properties
    const datatools::properties & get_auxiliaries() const;

    /// Return auxiliary properties
    datatools::properties & grab_auxiliaries();

    /// Return decay table configuration properties
    const datatools::properties & get_decay_table_config() const;

    /// Return decay table configuration properties
    datatools::properties & grab_decay_table_config();

    /// Return a descriptive string
    std::string to_string(unsigned int = 0) const;

    /// Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

    /// Check initialization flag
    bool is_initialized() const;

    /// Initialize from a list of parameters and a dictionary of nuclear decays
    void initialize(const datatools::properties & config_);

    /// Standalone initialization
    void initialize_simple();

    /// Reset
    void reset();

    /// Install the decay table
    void install_decay_table(const datatools::properties & config_,
                             const std::map<std::string, datatools::handle<nuclear_decay> > & decays_);

    /// Reset decay table
    void reset_decay_table();

    /// Return the decay channel table
    const decay_channels_col_type & get_decay_channels() const;

  protected:

    /// Set defaults
    void _set_defaults();

    /// Compute the weights associated to the decay
    void _compute_decay_weights();

  private:

    bool _initialized_;   //!< Initialization flag
    int _A_;              //!< Number of nucleons
    int _Z_;              //!< Number of protons
    double _spin_;        //!< The spin
    parity_type _parity_; //!< The parity
    double _half_life_;   //!< Half-life
    double _energy_;      //!< Excitation energy
    datatools::properties   _auxiliaries_;        //!< Auxiliary properties
    datatools::properties   _decay_table_config_; //!< Decay table configuration
    decay_channels_col_type _decay_channels_;     //!< The collection of decay channels

  };

} // end of namespace genbb

#endif // GENBB_HELP_NUCLEAR_LEVEL_H

// Local Variables: --
// mode: c++ --
// End: --

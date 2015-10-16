//! \file   hpge/calorimetry.h
//! \author Arnaud Chapon, François Mauger
//! \brief  Describe the HPGe detector response
//
// Copyright (c) 2015 by Arnaud Chapon <achapon@cerap.fr>
//                       and François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Bayeux/mctools.
//
// Bayeux/mctools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux/mctools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux/mctools. If not, see <http://www.gnu.org/licenses/>.

#ifndef HPGE_CALORIMETRY_H
#define HPGE_CALORIMETRY_H

// Standard library:
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/logger.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>

// This project:
#include <hpge/calorimeter_hit.h>

namespace hpge {

  //! \brief Compute the HPGe detector response from simulated data
  class calorimetry
  {
  public:

    struct hpge_hit : public geomtools::base_hit {
      double energy_deposit;
    };

    //! Default constructor
    calorimetry();

    //! Constructor
    calorimetry(double threshold_, double resolution_);

    //! Destructor
    ~calorimetry();

    //! Check initialization flag
    bool is_initialized() const;

    //! Initialization
    void initialize();

    //! Reset
    void reset();

    //! Set logging priority threshold
    void set_logging(datatools::logger::priority);

    //! Return logging priority threshold
    datatools::logger::priority get_logging() const;

    //! Set the energy threshold
    void set_energy_threshold(double);

    //! Return the energy threshold
    double get_energy_threshold() const;

    //! Set the energy resolution
    void set_energy_resolution(double);

    //! Return the energy resolution
    double get_energy_resolution() const;

    //! Set the energy resolution pedestal
    void set_energy_resolution_pedestal(double);

    //! Return the energy resolution pedestal
    double get_energy_resolution_pedestal() const;

    //! Set the hit category
    void set_hit_category(const std::string & );

    //! Return the hit category
    const std::string & get_hit_category() const;

    //! Set the only gamma flag
    void set_only_gamma(bool);

    //! Check the only gamma flag
    bool is_only_gamma() const;

    //! Set the PRNG's seed
    void set_prng_seed(int);

    //! Return the PRNG's seed
    int get_prng_seed() const;

    //! Process one single simulated event
    void process_event(const mctools::simulated_data & sd_,
                       calorimeter_hit & hit_);

  protected:

    //! Set default values to attributes
    void _set_defaults();

  private:

    // Configuration parameters:
    double      _energy_threshold_;    //!< The energy threshold
    double      _energy_resolution_;   //!< The energy resolution
    double      _energy_resolution_pedestal_; //!< The energy resolution pedestal
    std::string _hit_category_; //!< The category of hits to be processed
    bool        _only_gamma_;   //!< Process only gamma rays
    int         _prng_seed_;    //!< Seed for the embedded PRNG

    // Management:
    bool _initialized_;
    datatools::logger::priority _logging_; //!< Logging priority threshold

    // Working internal data:
    mygsl::rng  _prng_; //!< Embedded pseudo random number generator (PRNG)

  };

} // namespace hpge

#endif // HPGE_CALORIMETRY_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

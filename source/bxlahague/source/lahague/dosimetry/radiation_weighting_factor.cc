// lahague/dosimetry/radiation_weighting_factor.cc
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//                       and Arnaud Chapon <achapon@cerap.fr>
//
// This file is part of LaHague.
//
// LaHague is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LaHague is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with LaHague. If not, see <http://www.gnu.org/licenses/>.
//

// Ourselves:
#include <lahague/dosimetry/radiation_weighting_factor.h>

// Standard Library:
#include <cmath>

// Third Party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/clhep_units.h>

namespace lahague {

  namespace dosimetry {

    // static
    double radiation_weighting_factor::factor(const std::string & particle_name_,
                                              double energy_,
                                              recommendation_id_type rec_id_)
    {
      double f = datatools::invalid_real();
      if (rec_id_ != WR_ICRP_REPORT_103) return f;
      if (particle_name_ == "e-"
          || particle_name_ == "electron"
          || particle_name_ == "beta"
          || particle_name_ == "beta-"
          || particle_name_ == "e+"
          || particle_name_ == "positron"
          || particle_name_ == "beta+"
          || particle_name_ == "muon"
          || particle_name_ == "muon+"
          || particle_name_ == "muon-"
          || particle_name_ == "gamma"
          || particle_name_ == "X"
          || particle_name_ == "x"
          || particle_name_ == "xray") {
        f = 1.0;
      } else if (particle_name_ == "n"
                 || particle_name_ == "neutron") {
        double E = energy_ / CLHEP::MeV;
        if (energy_ < 1.0 * CLHEP::MeV) {
          f = 2.5 + 18.2 * std::exp(-std::pow(std::log(E), 2) / 6.0);
        } else if (energy_ <= 50.0 * CLHEP::MeV) {
          f = 5.0 + 17.0 * std::exp(-std::pow(std::log(2 * E), 2) / 6.0);
        } else {
          f = 2.5 + 3.25 * std::exp(-std::pow(std::log(0.04 * E), 2) / 6.0);
        }
      } else if (particle_name_ == "p"
                 || particle_name_ == "proton"
                 || particle_name_ == "pi+"
                 || particle_name_ == "pi-"
                 || particle_name_ == "pion+"
                 || particle_name_ == "pion-"
                 ) {
        f = 2.0;
      } else {
        // alphas, ions...
        f = 20.0;
      }

      return f;
    }

  } // namespace dosimetry

} // namespace lahague

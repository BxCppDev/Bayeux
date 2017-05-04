//! \file   lahague/dosimetry/radiation_weighting_factor.h
//! \author François Mauger
//! \brief  Describe the radiation weighting factor
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

#ifndef LAHAGUE_DOSIMETRY_RADIATION_WEIGHTING_FACTOR_H
#define LAHAGUE_DOSIMETRY_RADIATION_WEIGHTING_FACTOR_H

// Standard Library:
#include <string>

namespace lahague {

  namespace dosimetry {

    class radiation_weighting_factor
    {
    public:

      enum recommendation_id_type
      {
	// WR_ICRP_REPORT_60  = 1,
	WR_ICRP_REPORT_103 = 2 //!< Default recommendation
      };

      static double factor(const std::string & particle_name_,
			   double energy_,
			   recommendation_id_type = WR_ICRP_REPORT_103);

    };

  } // namespace dosimetry

} // namespace lahague

#endif // LAHAGUE_DOSIMETRY_RADIATION_WEIGHTING_FACTOR_H

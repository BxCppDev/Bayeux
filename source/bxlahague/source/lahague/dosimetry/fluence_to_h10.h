//! \file   lahague/dosimetry/fluence_to_h10.h
//! \author François Mauger, Arnaud Chapon
//! \brief  Tool for neutron or gamma fluence conversion to H10* for Monte-Carlo data
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

#ifndef LAHAGUE_DOSIMETRY_FLUENCE_TO_H10_H
#define LAHAGUE_DOSIMETRY_FLUENCE_TO_H10_H

// Standard Library:
#include <string>
#include <vector>

// Third Party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/properties.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>
#include <geomtools/id_mgr.h>
#include <geomtools/geom_id.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>

// This project:
#include <lahague/dosimetry/fluence_hit.h>

namespace lahague {

  namespace dosimetry {

    //! Return the fluence to H10 conversion factor for gammas at a given energy
    double gamma_fluence_to_h10_conversion(double energy_, uint32_t /* flags_ */ = 0);

    //! Return the fluence to H10 conversion factor for neutrons at a given energy
    double neutron_fluence_to_h10_conversion(double energy_, uint32_t /* flags_ */ = 0);

    //! \brief Compute H*(10) dose associated to fluence hits obtained by Monte-Carlo
    class mc_fluence_to_h10_processor
    {
    public:

      //! Default constructor
      mc_fluence_to_h10_processor();

      //! Destructor
      ~mc_fluence_to_h10_processor();

      //! Check if the external geometry manager is set
      bool has_geometry_manager() const;

      //! Return the external geometry manager
      const geomtools::manager & get_geometry_manager() const;

      //! Set the external geometry manager
      void set_geometry_manager(const geomtools::manager &);

      //!< Check the flag to throw an exception if the source hits have no requested properties
      bool is_throw_on_hit_misconfiguration() const;

      //!< Set the flag to throw an exception if the source hits have no requested properties
      void set_throw_on_hit_misconfiguration(bool);

      //! Check initialization status
      bool is_initialized() const;

      //! Initialization
      void initialize_simple();

      //! Initialization from a set of configuration properties
      void initialize(const datatools::properties &);

      //! Reset
      void reset();

      //! Process a simulated event
      void process(const mctools::simulated_data & event_record_,
                   std::vector<fluence_hit> & flhits_);

      //! Rule to compute ambiant dose in some volumes
      struct compute_dose_config {
        //! Default constructor
        compute_dose_config();
        std::string hit_category;      //!< The hit category to be processed
        std::string geometry_category; //!< The geometry category of volume to be processed
        std::string particle_name;     //!< The name of the particle ("gamma", "neutron")
        uint32_t    geometry_type;     //!< The geometry type
      };

      //! Add a configuration to compute dose
      void add_compute_dose_config(const std::string & label_,
                                   const compute_dose_config &);

    private:

      // Management:
      bool _initialized_; //!< Initialization flag

      // Configuration:
      bool _throw_on_hit_misconfiguration_; //!< Flag to throw an exception if the source hits have none of the requested properties
      const geomtools::manager * _geometry_manager_; //!< Handle to the geometry manager
      typedef std::map<std::string, compute_dose_config> dose_config_dict_type;
      dose_config_dict_type _dose_configs_; //!< Configuration of dose computation in volumes

    };

  } // namespace dosimetry

} // namespace lahague

#endif // LAHAGUE_DOSIMETRY_FLUENCE_TO_H10_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

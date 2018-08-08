// exstore.cxx
/* Copyright (C) 2018 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 */

// Standard library:
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

// Third party:
// - Boost:
#include <boost/filesystem.hpp>
// - Bayeux:
#include <bayeux/bayeux.h>
// - Bayeux/datatools:
#include <bayeux/datatools/io_factory.h>
// - Bayeux/geomtools:
#include <bayeux/geomtools/geom_id.h>
#include <bayeux/geomtools/material.h>
#include <bayeux/geomtools/sensitive.h>
// - Bayeux/mctools:
#include <bayeux/mctools/base_step_hit.h>
#include <bayeux/mctools/simulated_data.h>
#include <bayeux/mctools/utils.h>

int main(int argc_, char **argv_)
{
  bayeux::initialize(argc_, argv_, 0);

  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  int error_code = EXIT_SUCCESS;
  try {

    int hit_lib_version = boost::serialization::version<mctools::base_step_hit>::value;
    int sd_lib_version = boost::serialization::version<mctools::simulated_data>::value;
    std::clog << "[info] Base step hit class library version  = " << hit_lib_version << std::endl;
    std::clog << "[info] Simulated data class library version = " << sd_lib_version << std::endl;

    std::string outfile = "out-" + std::to_string(hit_lib_version) + ".xml";
    std::clog << "[info] outfile = '" << outfile << "'" << std::endl;

    double               time = 1.21 * CLHEP::nanosecond;
    geomtools::vector_3d pos(0.0, 0.0, -2.5 * CLHEP::mm);
    double               ke   = 1234.5 * CLHEP::keV;

    // Initial values:
    double time_start              = time;
    geomtools::vector_3d pos_start = pos;
    double ke_start                = ke;
  
    datatools::data_writer writer(outfile, datatools::using_multi_archives);

    // Step loops:
    for (int i = 0; i < 2; i++) {
      
      double delta_ke = -23.4 * CLHEP::keV;
      double delta_time = 1.234 * CLHEP::nanosecond;
      geomtools::vector_3d delta_pos(0.0, 0.0, +12.3 * CLHEP::mm);

      double time_stop = time_start + delta_time;
      geomtools::vector_3d pos_stop = pos_start + delta_pos;
      double step_length = (pos_stop - pos_start).mag();
      double ke_stop = ke_start + delta_ke;

      // MC hit:
      mctools::base_step_hit mchit;

      mchit.set_hit_id(12 + i);
      mchit.set_geom_id(geomtools::geom_id(100, 0, 1, 2 * i));
      mchit.grab_auxiliaries().store_flag("mock_hit");
      mchit.grab_auxiliaries().store("mock_mc", "bxmctools::exserialization");

      // Version 0:
      mchit.set_position_start(pos_start);
      mchit.set_position_stop(pos_stop);
      mchit.set_time_start(time_start);
      mchit.set_time_stop(time_stop);
      mchit.set_energy_deposit(ke_start - ke_stop);
      mchit.set_particle_name("bayeutron");

      // Version 1:
      mchit.set_biasing_weight(1.0);

      int track_id = 2;
      int parent_track_id = 1;
      std::string creator_process_name = "dummy";
      std::string material_name = "water";
      std::string sensitive_category = "calo";
      std::string g4_volume_name = "baz";
      int g4_volume_copy_number = 42 + i;
      
#ifdef MCTOOLS_VALIDATION_SERIALIZATION_DOWNGRADE
      if (hit_lib_version <= 1) {
        
        mchit.grab_auxiliaries().store(mctools::track_utils::TRACK_ID_KEY, track_id);
        mchit.grab_auxiliaries().store(mctools::track_utils::PARENT_TRACK_ID_KEY, parent_track_id);
        mchit.grab_auxiliaries().store(mctools::track_utils::PRIMARY_PARTICLE_FLAG, true);
        mchit.grab_auxiliaries().store(mctools::track_utils::MAJOR_TRACK_FLAG, true);
        mchit.grab_auxiliaries().store(mctools::track_utils::START_KINETIC_ENERGY_KEY, ke_start);
        mchit.grab_auxiliaries().store(mctools::track_utils::STOP_KINETIC_ENERGY_KEY, ke_stop);
        mchit.grab_auxiliaries().store(mctools::track_utils::CREATOR_PROCESS_KEY, creator_process_name);
        mchit.grab_auxiliaries().store(mctools::track_utils::DELTA_RAY_FROM_ALPHA_FLAG, true);
        mchit.grab_auxiliaries().store(mctools::track_utils::STEP_LENGTH, step_length);

        if (i == 0) {
          mchit.grab_auxiliaries().store(mctools::track_utils::ENTERING_VOLUME_FLAG, true);
        }
        
        if (i == 1) {
          mchit.grab_auxiliaries().store(mctools::track_utils::LEAVING_VOLUME_FLAG, true);
        }

        {
          static std::string material_ref_key =
            geomtools::material::make_key(geomtools::material::material_ref_property());
          mchit.grab_auxiliaries().store(material_ref_key, material_name);
        }

        {
          static std::string sensitive_category_key =
            geomtools::sensitive::make_key(geomtools::sensitive::constants::instance().SENSITIVE_CATEGORY_PROPERTY); 
          mchit.grab_auxiliaries().store(sensitive_category_key, sensitive_category);
        }
        
        mchit.grab_auxiliaries().store(mctools::sensitive_utils::SENSITIVE_G4_VOLUME_NAME_KEY,
                                       g4_volume_name);
        
        mchit.grab_auxiliaries().store(mctools::sensitive_utils::SENSITIVE_G4_VOLUME_COPY_NUMBER_KEY,
                                       g4_volume_copy_number);
      }
#endif // !MCTOOLS_VALIDATION_SERIALIZATION_DOWNGRADE
      
#ifndef MCTOOLS_VALIDATION_SERIALIZATION_DOWNGRADE
      if (hit_lib_version >= 2) {
        
        mchit.set_track_id(track_id);
        mchit.set_parent_track_id(parent_track_id);
        mchit.set_primary_particle(true);
        mchit.set_major_track(true);
        mchit.set_kinetic_energy_start(ke_start);
        mchit.set_kinetic_energy_stop(ke_stop);
        mchit.set_creator_process_name(creator_process_name);
        mchit.set_delta_ray_from_alpha(true);
        mchit.set_step_length(step_length);
        if (i == 0) {
          mchit.set_entering_volume(true);
        }
        if (i == 1) {
          mchit.set_leaving_volume(true);
        }
        mchit.set_material_name(material_name);
        mchit.set_sensitive_category(sensitive_category);
        mchit.set_g4_volume_name(g4_volume_name);
        mchit.set_g4_volume_copy_number(g4_volume_copy_number);
      }
#endif // MCTOOLS_VALIDATION_SERIALIZATION_DOWNGRADE
      
      mchit.tree_dump(std::cout, "Mock base step hit:");
      writer.store("MChit", mchit);

      // Update:
      time_start = time_stop;
      pos_start = pos_stop;
      ke_start = ke_stop;
      
    }
 
  } catch (std::exception& x) {
    DT_LOG_FATAL(logging, x.what());
    error_code = EXIT_FAILURE;
  } catch (...) {
    DT_LOG_FATAL(logging, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }

  bayeux::terminate();
  return error_code;
}

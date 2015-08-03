// -*- mode: c++ ; -*-
/* utils.cc
 */

#include <mctools/utils.h>

namespace mctools {

  // Specific to sensitive steps :
  const std::string sensitive_utils::SENSITIVE_HITS_BUFFER_CAPACITY       = "hits_buffer_capacity";
  const std::string sensitive_utils::SENSITIVE_TRACK_GAMMA                = "track_gamma";
  const std::string sensitive_utils::SENSITIVE_TRACK_OPTICAL_PHOTON       = "track_optical_photon";
  const std::string sensitive_utils::SENSITIVE_TRACK_NEUTRON              = "track_neutron";
  const std::string sensitive_utils::SENSITIVE_DROP_ZERO_ENERGY_DEPOSIT   = "drop_zero_energy_deposit_steps";
  const std::string sensitive_utils::SENSITIVE_RECORD_G4_VOLUME_INFOS     = "record_g4_volume_infos";
  const std::string sensitive_utils::SENSITIVE_G4_VOLUME_NAME_KEY         = "g4_volume.name";
  const std::string sensitive_utils::SENSITIVE_G4_VOLUME_COPY_NUMBER_KEY  = "g4_volume.copy_number";
  const std::string sensitive_utils::SENSITIVE_RECORD_MOMENTUM            = "record_momentum";
  const std::string sensitive_utils::SENSITIVE_RECORD_KINETIC_ENERGY      = "record_kinetic_energy";
  const std::string sensitive_utils::SENSITIVE_RECORD_SENSITIVE_CATEGORY  = "record_sensitive_category";
  const std::string sensitive_utils::SENSITIVE_RECORD_MATERIAL            = "record_material";
  const std::string sensitive_utils::SENSITIVE_RECORD_CREATOR_CATEGORY    = "record_creator_category";
  const std::string sensitive_utils::SENSITIVE_RECORD_CREATOR_PROCESS     = "record_creator_process";
  const std::string sensitive_utils::SENSITIVE_RECORD_MAJOR_TRACK         = "record_major_track";
  const std::string sensitive_utils::SENSITIVE_MAJOR_TRACK_MINIMUM_ENERGY = "major_track_minimum_energy";
  const std::string sensitive_utils::SENSITIVE_RECORD_ALPHA_QUENCHING     = "record_alpha_quenching";
  const std::string sensitive_utils::SENSITIVE_RECORD_TRACK_ID            = "record_track_id";
  const std::string sensitive_utils::SENSITIVE_RECORD_PRIMARY_PARTICLE    = "record_primary_particle";
  const std::string sensitive_utils::SENSITIVE_RECORD_STEP_LENGTH         = "record_step_length";
  const std::string sensitive_utils::SENSITIVE_RECORD_BOUNDARIES          = "record_boundaries";

  // Specific to hits :
  const std::string hit_utils::HIT_MC_BUGGY_KEY            = "hit.mc_buggy";
  const std::string hit_utils::HIT_VISU_HIGHLIGHTED_KEY    = "hit.visu.highlighted";
  const std::string hit_utils::HIT_MC_STEP_PROCESSOR_KEY   = "hit.mc_step_processor";
  const std::string hit_utils::HIT_MC_HIT_ID_KEY           = "hit.mc_hit_id";

  // Specific to tracks :
  const std::string track_utils::TRACK_ID_KEY              = "track.id";
  const std::string track_utils::PARENT_TRACK_ID_KEY       = "track.parent_id";
  const std::string track_utils::PRIMARY_PARTICLE_FLAG     = "track.primary";
  const std::string track_utils::MAJOR_TRACK_FLAG          = "track.major";
  const std::string track_utils::START_KINETIC_ENERGY_KEY  = "track.start_kinetic_energy";
  const std::string track_utils::STOP_KINETIC_ENERGY_KEY   = "track.stop_kinetic_energy"; ;
  const std::string track_utils::CREATOR_PROCESS_KEY       = "track.creator_process";
  const std::string track_utils::CREATOR_CATEGORY_KEY      = "track.creator_category";
  const std::string track_utils::DELTA_RAY_FROM_ALPHA_FLAG = "track.delta_ray_from_alpha";
  const std::string track_utils::STEP_LENGTH               = "track.step_length";
  const std::string track_utils::ENTERING_VOLUME_FLAG      = "track.entering_volume";
  const std::string track_utils::LEAVING_VOLUME_FLAG       = "track.leaving_volume";
  const std::string track_utils::ENTERING_KINETIC_ENERGY_KEY = "track.entering_kinetic_energy";
  const std::string track_utils::LEAVING_KINETIC_ENERGY_KEY  = "track.leaving_kinetic_energy";
  const int         track_utils::INVALID_TRACK_ID;

  // Specific to simulated data :
  const std::string event_utils::EVENT_GENBB_WEIGHT = "event.genbb_weight";
  const std::string event_utils::EVENT_GENBB_LABEL  = "event.genbb_label";
  const std::string event_utils::EVENT_DEFAULT_SIMULATED_DATA_LABEL = "SD";

  // Specific to I/O format :
  const std::string io_utils::GENERAL_INFO_STORE         = "GI";
  const std::string io_utils::PLAIN_SIMULATED_DATA_STORE = "PSD";

} // end of namespace mctools

// end of utils.cc

// -*- mode: c++ ; -*-
/* utils.cc
 */

#include <mctools/utils.h>

namespace mctools {

  // Specific to hits :
  const std::string hit_utils::HIT_MC_BUGGY_KEY           = "hit.mc_buggy";
  const std::string hit_utils::HIT_VISU_HIGHLIGHTED_KEY   = "hit.visu.highlighted";
  const std::string hit_utils::HIT_MC_STEP_PROCESSOR_KEY  = "hit.mc_step_processor";
  const std::string hit_utils::HIT_MC_HIT_ID_KEY          = "hit.mc_hit_id";

  // Specific to tracks :
  const std::string track_utils::TRACK_ID_KEY             = "track.id";
  const std::string track_utils::PARENT_TRACK_ID_KEY      = "track.parent_id";
  const std::string track_utils::PRIMARY_PARTICLE_FLAG    = "track.primary";
  const std::string track_utils::MAJOR_TRACK_FLAG         = "track.major";
  const std::string track_utils::START_KINETIC_ENERGY_KEY = "track.start_kinetic_energy";
  const std::string track_utils::STOP_KINETIC_ENERGY_KEY  = "track.stop_kinetic_energy"; ;
  const std::string track_utils::CREATOR_PROCESS_KEY      = "track.creator_process";
  const std::string track_utils::CREATOR_CATEGORY_KEY     = "track.creator_category";
  const std::string track_utils::DELTA_RAY_FROM_ALPHA_FLAG = "tracK.delta_ray_from_alpha";

  // Specific to simulated data :
  const std::string event_utils::EVENT_GENBB_WEIGHT = "event.genbb_weight";
  const std::string event_utils::EVENT_DEFAULT_SIMULATED_DATA_LABEL = "SD";

} // end of namespace mctools

// end of utils.cc

// -*- mode: c++ ; -*-
/*  utils.h
 * Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-03-08
 * Last modified: 2013-03-08
 *
 * License:
 *
 * Description:
 *
 *   Utilities
 *
 * History:
 *
 */

#ifndef MCTOOLS_UTILS_H_
#define MCTOOLS_UTILS_H_ 1

#include <string>

namespace mctools {

  class sensitive_utils
  {
  public:
    static const std::string SENSITIVE_HITS_BUFFER_CAPACITY;
    static const std::string SENSITIVE_TRACK_GAMMA;
    static const std::string SENSITIVE_TRACK_OPTICAL_PHOTON;
    static const std::string SENSITIVE_TRACK_NEUTRON;
    static const std::string SENSITIVE_DROP_ZERO_ENERGY_DEPOSIT;
    static const std::string SENSITIVE_RECORD_MOMENTUM;
    static const std::string SENSITIVE_RECORD_KINETIC_ENERGY;
    static const std::string SENSITIVE_RECORD_SENSITIVE_CATEGORY;
    static const std::string SENSITIVE_RECORD_MATERIAL;
    static const std::string SENSITIVE_RECORD_CREATOR_CATEGORY;
    static const std::string SENSITIVE_RECORD_CREATOR_PROCESS;
    static const std::string SENSITIVE_RECORD_MAJOR_TRACK;
    static const std::string SENSITIVE_MAJOR_TRACK_MINIMUM_ENERGY;
    static const std::string SENSITIVE_RECORD_ALPHA_QUENCHING;
    static const std::string SENSITIVE_RECORD_TRACK_ID;
    static const std::string SENSITIVE_RECORD_PRIMARY_PARTICLE;
    static const std::string SENSITIVE_RECORD_G4_VOLUME_INFOS;
    static const std::string SENSITIVE_G4_VOLUME_NAME_KEY;
    static const std::string SENSITIVE_G4_VOLUME_COPY_NUMBER_KEY;
    static const std::string SENSITIVE_RECORD_STEP_LENGTH;
    static const std::string SENSITIVE_RECORD_BOUNDARIES;
  };

  class hit_utils
  {
  public:
    static const std::string HIT_VISU_HIGHLIGHTED_KEY;
    static const std::string HIT_MC_STEP_PROCESSOR_KEY;
    static const std::string HIT_MC_BUGGY_KEY;
    static const std::string HIT_MC_HIT_ID_KEY;
  };

  class track_utils
  {
  public:
    static const int         INVALID_TRACK_ID = -1;
    static const std::string TRACK_ID_KEY;
    static const std::string PARENT_TRACK_ID_KEY;
    static const std::string PRIMARY_PARTICLE_FLAG;
    static const std::string MAJOR_TRACK_FLAG;
    static const std::string START_KINETIC_ENERGY_KEY;
    static const std::string STOP_KINETIC_ENERGY_KEY;
    static const std::string CREATOR_PROCESS_KEY;
    static const std::string CREATOR_CATEGORY_KEY;
    static const std::string DELTA_RAY_FROM_ALPHA_FLAG;
    static const std::string STEP_LENGTH;
    static const std::string ENTERING_VOLUME_FLAG;
    static const std::string LEAVING_VOLUME_FLAG;
  };

  class event_utils
  {
  public:
    static const std::string EVENT_GENBB_WEIGHT;
    static const std::string EVENT_DEFAULT_SIMULATED_DATA_LABEL;
  };

  class io_utils
  {
  public:
    static const std::string GENERAL_INFO_STORE;
    static const std::string PLAIN_SIMULATED_DATA_STORE;
  };

} // end of namespace mctools

#endif // MCTOOLS_UTILS_H_

// end of utils.h

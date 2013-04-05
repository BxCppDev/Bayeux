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
  };

  class event_utils
  {
  public:
    static const std::string EVENT_GENBB_WEIGHT;
    static const std::string EVENT_DEFAULT_SIMULATED_DATA_LABEL;
  };

} // end of namespace mctools

#endif // MCTOOLS_UTILS_H_

// end of utils.h

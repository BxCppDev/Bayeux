// -*- mode: c++ ; -*-
/* mapping_utils.h
 */

#ifndef __geomtools__mapping_utils_h
#define __geomtools__mapping_utils_h 1

#include <string>

#include <datatools/properties.h>

namespace geomtools {

  struct mapping_utils
  {
  public:

    static const std::string & mapping_prefix();

    static const std::string & mapping_category_property();

    static const std::string & mapping_address_property();

    static const std::string & mapping_id_property();

    static const std::string & mapping_daughter_id_property();

    static void extract (const datatools::properties & source_,
                         datatools::properties & target_);

    static std::string make_key (const std::string & key_);

    static bool has_key (const datatools::properties & config_,
                         const std::string & key_);

    static bool has_flag (const datatools::properties & config_,
                          const std::string & key_);

    static bool has_id (const datatools::properties & config_);

    static void fetch_id (const datatools::properties & config_,
                          std::string & id_info_);

    static bool has_daughter_id (const datatools::properties & config_,
                                 const std::string & daughter_name_);

    static void fetch_daughter_id (const datatools::properties & config_,
                                   const std::string & daughter_name_,
                                   std::string & id_info_);

  };

} // end of namespace geomtools

#endif // __geomtools__mapping_utils_h

// end of mapping_utils.h

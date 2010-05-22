// -*- mode: c++ ; -*- 
/* mapping_utils.h
 */

#ifndef __geomtools__mapping_utils_h
#define __geomtools__mapping_utils_h 1

#include <string>

#include <datatools/utils/properties.h>

namespace geomtools {

  using namespace std;

  struct mapping_utils
  {
  public:
    
    static const string MAPPING_PREFIX;
    static const string MAPPING_CATEGORY_PROPERTY;
    static const string MAPPING_ADDRESS_PROPERTY;
    static const string MAPPING_ID_PROPERTY;
    static const string MAPPING_DAUGHTER_ID_PROPERTY;

    static void extract (const datatools::utils::properties & source_,
			 datatools::utils::properties & target_);

    static string make_key (const string & key_);
    
    static bool has_key (const datatools::utils::properties & config_,
			 const string & key_);
    
    static bool has_flag (const datatools::utils::properties & config_,
			  const string & key_);
    
    static bool has_id (const datatools::utils::properties & config_);

    static void fetch_id (const datatools::utils::properties & config_,
			  string & id_info_);
    
    static bool has_daughter_id (const datatools::utils::properties & config_,
				 const string & daughter_name_);

    static void fetch_daughter_id (const datatools::utils::properties & config_,
				   const string & daughter_name_,
				   string & id_info_);
    
  };

} // end of namespace geomtools

#endif // __geomtools__mapping_utils_h

// end of mapping_utils.h

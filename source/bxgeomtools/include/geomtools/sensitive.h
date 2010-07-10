// -*- mode: c++; -*- 
/* sensitive.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-13
 * Last modified: 2010-03-13
 * 
 * License: 
 * 
 * Description: 
 *
 *  Utilities for sensitive reference.
 *
 * History: 
 * 
 */

#ifndef __geomtools__sensitive_h
#define __geomtools__sensitive_h 1

#include <iostream>
#include <iomanip>
#include <string>
#include <limits>
#include <cmath>
#include <list>

#include <datatools/utils/properties.h>

namespace geomtools {

  class sensitive
  {
  public:

    static const string SENSITIVE_PREFIX;
    static const string SENSITIVE_CATEGORY_PROPERTY;
    static const string SENSITIVE_ALPHA_QUENCHING_FLAG;

    static string make_key (const string & flag_);

    static void extract (const datatools::utils::properties & source_,
			 datatools::utils::properties & target_);

    static bool has_flag (const datatools::utils::properties & config_,
			  const string & flag_);

    static bool has_key (const datatools::utils::properties & config_,
			 const string & key_);

    static bool is_sensitive (const datatools::utils::properties & config_);

    static bool has_alpha_quenching (const datatools::utils::properties & config_);

    static string get_sensitive_category (const datatools::utils::properties & config_);

    static void set_sensitive_category (datatools::utils::properties & config_,
					const string & cat_name_);

  };


}  // end of namespace geomtools

#endif // __geomtools__sensitive_h

// end of sensitive.h

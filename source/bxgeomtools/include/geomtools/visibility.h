// -*- mode: c++; -*- 
/* visibility.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2009-03-31
 * 
 * License: 
 * 
 * Description: 
 *  Utilities for visualization tools:
 *
 *  Some visibility properties are defined:
 *
 *
 * History: 
 * 
 */

#ifndef __geomtools__visibility_h
#define __geomtools__visibility_h 1

#include <iostream>
#include <iomanip>
#include <string>
#include <limits>
#include <cmath>
#include <list>

#include <datatools/utils/properties.h>

namespace geomtools {

  class visibility
  {
  public:
    static const string VISIBILITY_PREFIX;
    static const string VISIBILITY_HIDDEN_FLAG;
    static const string VISIBILITY_HIDDEN_ENVELOP_FLAG;
    static const string VISIBILITY_HIDDEN_ENVELOPE_FLAG;
    static const string VISIBILITY_DAUGHTERS_HIDDEN_FLAG;
    static const string VISIBILITY_COLOR_PROPERTY;
    static const string VISIBILITY_WIRED_CYLINDER_FLAG;

  public:

    static string make_key (const string & flag_);

    static void extract (const datatools::utils::properties & source_,
			 datatools::utils::properties & target_);

    static bool has_flag (const datatools::utils::properties & config_,
			  const string & flag_);

    static bool has_key (const datatools::utils::properties & config_,
			 const string & key_);

    static bool is_shown (const datatools::utils::properties & config_);

    static bool is_hidden (const datatools::utils::properties & config_);

    static bool is_hidden_envelop (const datatools::utils::properties & config_);

    static bool is_hidden_envelope (const datatools::utils::properties & config_);

    static bool is_daughters_shown (const datatools::utils::properties & config_);

    static bool is_daughters_hidden (const datatools::utils::properties & config_);

    static bool has_color (const datatools::utils::properties & config_);
 
    static string get_color (const datatools::utils::properties & config_);
    
    static void set_color (datatools::utils::properties & config_, 
			   const string & color_);

    static bool is_wired_cylinder (const datatools::utils::properties & config_);

  };


}  // end of namespace geomtools

#endif // __geomtools__visibility_h

// end of visibility.h

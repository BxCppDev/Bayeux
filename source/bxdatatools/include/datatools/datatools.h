// -*- mode: c++; -*- 
//! \file datatools/datatools.h
/* datatools.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-02-25
 * Last modified: 2012-12-12
 * 
 * License: 
 * 
 * 
 * Description: 
 *
 *   datatools package user interface
 *  
 * History: 
 * 
 */

#ifndef __datatools__datatools_h
#define __datatools__datatools_h 1

#include <datatools/datatools_config.h>

#include <datatools/utils/bit_mask.h>
#include <datatools/utils/caster_utils.h>
#include <datatools/utils/clhep_units.h>
#include <datatools/utils/command.h>
#include <datatools/utils/handle.h>
#include <datatools/utils/handle_pool.h>
#include <datatools/utils/histos.h>
#include <datatools/utils/i_clear.h>
#include <datatools/utils/i_cloneable.h>
#include <datatools/utils/i_invokable.h>
#include <datatools/utils/integer_range.h>
#include <datatools/utils/i_named.h>
#include <datatools/utils/ioutils.h>
#include <datatools/utils/i_predicate.h>
#include <datatools/utils/i_propinit.h>
#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/library_loader.h>
#include <datatools/utils/multi_properties.h>
#include <datatools/utils/properties.h>
#include <datatools/utils/range_tools.h>
#include <datatools/utils/real_range.h>
#include <datatools/utils/smart_filename.h>
#include <datatools/utils/smart_ref.h>
#include <datatools/utils/temporary_files.h>
#include <datatools/utils/things.h>
#include <datatools/utils/time_tools.h>
#include <datatools/utils/units.h>
#include <datatools/utils/utils.h>

#include <datatools/services/base_service.h>
#include <datatools/services/service_macros.h>
#include <datatools/services/service_manager.h>
#include <datatools/services/service_tools.h>

#include <datatools/event/event_id.h>

#include <datatools/serialization/archives_instantiation.h>
#include <datatools/serialization/archives_list.h>
#include <datatools/serialization/bio_guard.h>
#include <datatools/serialization/i_serializable.h>
#include <datatools/serialization/io_factory.h>
#include <datatools/serialization/memory_streambuf.h>
#include <datatools/serialization/safe_serial.h>
#include <datatools/serialization/utils.h>

//! \namespace datatools
/* The basic datatools namespace.
 */
namespace datatools {

  class API
  {
  public:
    //static bool g_debug = false;
  };

} // end of namespace datatools

#endif // __datatools__datatools_h

// end of datatools.h

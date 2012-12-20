// -*- mode: c++; -*- 
//! \file datatools/datatools.h
/* datatools.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-02-25
 * Last modified: 2012-12-16
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

#ifndef DATATOOLS_DATATOOLS_H_
#define DATATOOLS_DATATOOLS_H_ 1

#include <datatools/datatools_config.h>

#include <datatools/bit_mask.h>
#include <datatools/caster_utils.h>
#include <datatools/clhep_units.h>
#include <datatools/command.h>
#include <datatools/handle.h>
#include <datatools/handle_pool.h>
#include <datatools/histos.h>
#include <datatools/i_clear.h>
#include <datatools/i_cloneable.h>
#include <datatools/i_invokable.h>
#include <datatools/integer_range.h>
#include <datatools/i_named.h>
#include <datatools/ioutils.h>
#include <datatools/i_predicate.h>
#include <datatools/i_propinit.h>
#include <datatools/i_tree_dump.h>
#include <datatools/library_loader.h>
#include <datatools/multi_properties.h>
#include <datatools/properties.h>
#include <datatools/range_tools.h>
#include <datatools/real_range.h>
#include <datatools/smart_filename.h>
#include <datatools/smart_ref.h>
#include <datatools/temporary_files.h>
#include <datatools/things.h>
#include <datatools/time_tools.h>
#include <datatools/units.h>
#include <datatools/utils.h>

#include <datatools/base_service.h>
#include <datatools/service_macros.h>
#include <datatools/service_manager.h>
#include <datatools/service_tools.h>

#include <datatools/event_id.h>

#include <datatools/archives_instantiation.h>
#include <datatools/archives_list.h>
#include <datatools/bio_guard.h>
#include <datatools/i_serializable.h>
#include <datatools/io_factory.h>
#include <datatools/memory_streambuf.h>
#include <datatools/safe_serial.h>
#include <datatools/serialization_macros.h>

//! \namespace datatools
/* The basic datatools namespace.
 */
namespace datatools {

 
} // end of namespace datatools

#endif // DATATOOLS_DATATOOLS_H_

// end of datatools.h

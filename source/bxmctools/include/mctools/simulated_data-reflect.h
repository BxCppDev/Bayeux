// -*- mode: c++ ; -*-
/* simulated_data-reflect.h
 * Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-06-03
 * Last modified: 2013-06-03
 *
 * License:
 *
 * Description:
 *
 *   Base hit.
 *
 * History:
 *
 */

#ifndef MCTOOLS_SIMULATED_DATA_REFLECT_H_
#define MCTOOLS_SIMULATED_DATA_REFLECT_H_ 1

// Datatools
#include <datatools/reflection_macros.h>

// Geomtools
#include <mctools/simulated_data.h>

// Export the introspection code for the 'mctools::simulated_data' class:
DR_CLASS_EXPORT (::mctools::simulated_data, "mctools::simulated_data")

// Mandatory namespace is datatools::detail::reflection :
namespace datatools {
  namespace detail {
    namespace reflection {

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::mctools::simulated_data, tag_)
      {
        DR_CLASS_DECLARE (::mctools::simulated_data)
          .DR_CLASS_BASE(::datatools::i_serializable)
          .DR_CLASS_CONSTRUCTOR()

	  // Properties :
          .DR_CLASS_PROPERTY_GETTER_SETTER("vertex",
                                           ::mctools::simulated_data::get_vertex,
                                           ::mctools::simulated_data::set_vertex)
          .DR_CLASS_PROPERTY_GETTER_SETTER("primary_event",
                                           ::mctools::simulated_data::get_primary_event,
                                           ::mctools::simulated_data::set_primary_event)
          .DR_CLASS_PROPERTY_GETTER_SETTER("properties",
                                           ::mctools::simulated_data::get_properties,
                                           ::mctools::simulated_data::set_properties)

	  // Methods:
          .DR_CLASS_METHOD_CONST("has_vertex",
                                 ::mctools::simulated_data,
                                 ::mctools::simulated_data::has_vertex,
                                 bool)
          .DR_CLASS_METHOD_CONST("has_data",
                                 ::mctools::simulated_data,
                                 ::mctools::simulated_data::has_data,
                                 bool)
          .DR_CLASS_METHOD1_CONST("has_step_hits",
                                 ::mctools::simulated_data,
                                 ::mctools::simulated_data::has_step_hits,
				  bool,
				  const std::string &)
          .DR_CLASS_METHOD1_CONST("get_number_of_step_hits",
                                 ::mctools::simulated_data,
                                 ::mctools::simulated_data::get_number_of_step_hits,
				  size_t,
				  const std::string &)
          .DR_CLASS_METHOD2_CONST("get_step_hit",
				  ::mctools::simulated_data,
				  ::mctools::simulated_data::get_step_hit,
				  const ::mctools::base_step_hit &,
				  const std::string &, int)
          .DR_CLASS_VOID_METHOD("clear",
				::mctools::simulated_data,
				::mctools::simulated_data::clear)

          ;
        return;
      }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // MCTOOLS_SIMULATED_DATA_REFLECT_H_

// end of simulated_data-reflect.h

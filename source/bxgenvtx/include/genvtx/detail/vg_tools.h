// -*- mode: c++ ; -*- 
/* i_vertex_generator.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-12
 * Last modified: 2010-02-12
 * 
 * License: 
 * 
 * Description: 
 * 
 * History: 
 * 
 */

#ifndef __genvtx__detail__vg_tools_h
#define __genvtx__detail__vg_tools_h 1

#include <string>
#include <map>

#include <datatools/utils/properties.h>

namespace genvtx {

  using namespace std;
  using namespace datatools::utils;

  class i_vertex_generator;

  typedef i_vertex_generator * (*vg_creator_t) (const properties & configuration_);

  typedef map<string, vg_creator_t> vertex_generator_creator_dict_t;

} // end of namespace genvtx

#endif // __genvtx__detail__vg_tools_h

// end of vg_tools.h

// -*- mode: c++ ; -*- 
/* vg_tools.h
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

  typedef i_vertex_generator * (*vg_creator_type) (const properties & configuration_, void * user_);

  typedef map<string, vg_creator_type> vertex_generator_creator_dict_type;

  class vg_tools
  {
  public:
    static const string SHAPE_REF_NONE;
    static const string SHAPE_REF_PLAIN;
    static const string SHAPE_REF_GETTER;
  };


} // end of namespace genvtx

#endif // __genvtx__detail__vg_tools_h

// end of vg_tools.h

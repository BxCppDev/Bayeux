// -*- mode: c++ ; -*- 
/* vg_tools.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-12
 * Last modified: 2012-04-27
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
#include <datatools/utils/handle.h>

namespace genvtx {

  class i_vertex_generator;

  typedef datatools::utils::handle<i_vertex_generator> vg_handle_type;
  typedef std::map<std::string, vg_handle_type>        vg_dict_type;
 
  class vg_tools
  {
  public:
    static const std::string SHAPE_REF_NONE;
    static const std::string SHAPE_REF_PLAIN;
    static const std::string SHAPE_REF_GETTER;
  };

} // end of namespace genvtx

#endif // __genvtx__detail__vg_tools_h

// end of vg_tools.h

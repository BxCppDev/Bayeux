// -*- mode: c++; -*- 
/* cut_tools.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-19
 * Last modified: 2010-09-19
 * 
 * License: 
 * 
 * Description: 
 * 
 * History: 
 * 
 */

#ifndef __cuts__detail__cut_tools_h
#define __cuts__detail__cut_tools_h 1

#include <string>
#include <map>

#include <datatools/utils/properties.h>

namespace cuts {

  using namespace std;
  using namespace datatools::utils;

  class i_cut;
  class factory;

  typedef map<string, i_cut *> cut_dict_t;

  typedef i_cut * (*cut_creator_t) (const properties & configuration_, 
				    cut_dict_t * cut_dict_,
				    void * user_);

  typedef map<string, cut_creator_t> cut_creator_dict_t;

  /*
  class cut_tools
  {
  public:

  };
  */

} // end of namespace cuts

#endif // __cuts__detail__cut_tools_h

// end of cut_tools.h

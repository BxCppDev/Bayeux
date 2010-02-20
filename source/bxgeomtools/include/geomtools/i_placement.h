// -*- mode: c++; -*- 
/* i_placement.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2008-05-23
 * 
 * License: 
 * 
 * Description: 
 *  Utilities.
 *
 * History: 
 * 
 */

#ifndef __geomtools__i_placement_h
#define __geomtools__i_placement_h 1

#include <iostream>
#include <iomanip>
#include <string>

#include <datatools/utils/i_tree_dump.h>

#include <geomtools/utils.h>

namespace geomtools {
  
  using namespace std;
  
  class placement;
  
  class i_placement 
    : public datatools::utils::i_tree_dumpable
  {
  public: 
    // ctor:
    i_placement ();

    // dtor:
    virtual ~i_placement ();

  public: 

    virtual size_t get_number_of_items () const = 0;
    
    virtual void get_placement (int item_, placement & p_) const = 0;
    
    virtual void tree_dump (ostream & out_         = clog, 
			    const string & title_  = "", 
			    const string & indent_ = "", 
			    bool inherit_          = false) const;
    
  };

} // end of namespace geomtools

#endif // __geomtools__i_placement_h

// end of i_placement.h

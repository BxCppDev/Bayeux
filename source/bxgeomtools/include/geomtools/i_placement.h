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
#include <string>
#include <vector>

#include <boost/cstdint.hpp>

#include <datatools/utils/i_tree_dump.h>
#include <boost/serialization/access.hpp>
#include <datatools/serialization/i_serializable.h>

#include <geomtools/utils.h>

namespace geomtools {
  
  class placement;
  
  class i_placement 
    : DATATOOLS_SERIALIZABLE_CLASS , 
      public datatools::utils::i_tree_dumpable
  {
  public: 
    // ctor:
    i_placement ();

    // dtor:
    virtual ~i_placement ();

  public: 

    bool is_multiple () const;
 
    virtual bool has_only_one_rotation () const;
    
    placement get_placement (int item_) const;

    virtual size_t get_number_of_items () const = 0;

    virtual size_t compute_index_map (std::vector<uint32_t> & map_, int item_) const = 0;

    virtual bool is_replica () const = 0;
    
    virtual void get_placement (int item_, placement & p_) const = 0;
    
    virtual void tree_dump (std::ostream & out_         = std::clog, 
                            const std::string & title_  = "", 
                            const std::string & indent_ = "", 
                            bool inherit_          = false) const;
    
    DATATOOLS_SERIALIZATION_DECLARATION();
    
  };

} // end of namespace geomtools

#endif // __geomtools__i_placement_h

// end of i_placement.h

// -*- mode: c++; -*- 
/* multiple_placement.h
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

#ifndef __geomtools__multiple_placement_h
#define __geomtools__multiple_placement_h 1

#include <iostream>
#include <string>
#include <vector>

#include <geomtools/placement.h>

namespace geomtools {

  using namespace std;

  class multiple_placement
    : public i_placement
  {
  public:

    typedef vector<placement> placement_col_t;

  public:
 
    virtual bool is_replica () const;
 
    bool is_valid () const;

    void invalidate ();

    void add (const placement & p_);

    const placement & get_placement (int index_) const;

    placement & get_placement (int index_);

  public: 
    // i_placement interface:
    virtual size_t get_number_of_items () const;
   
    virtual void get_placement (int item_, placement & p_) const;

    virtual size_t compute_index_map (vector<uint32_t> & map_, 
                                      int item_) const;

  public: 

    // ctor:
    multiple_placement ();
                
    // dtor:
    virtual ~multiple_placement ();

    virtual void reset ();

    // i_tree_dump interface:
    virtual void tree_dump (ostream & out_ = clog, 
                            const string & title_ = "geomutils::multiple_placement", 
                            const string & indent_ = "", 
                            bool inherit_ = false) const;

  private:

    placement_col_t _placements_;

  };

} // end of namespace geomtools

#endif // __geomtools__placement_h

// end of placement.h

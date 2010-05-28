// -*- mode: c++ ; -*- 
/* address_set.h
 * Author (s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-08
 * Last modified: 2010-02-08
 * 
 * License: 
 * 
 * Description: 
 *   Set of addresses from a geometry ID
 * 
 */

#ifndef __geomtools__address_set_h
#define __geomtools__address_set_h 1

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>
#include <set>
#include <algorithm>

#include <boost/cstdint.hpp>

namespace geomtools {

  using namespace std;

  class address_set
  {
 
  public:
    
    enum mode_t
    {
      MODE_INVALID = -1,
      MODE_NONE = 0,
      MODE_ALL = 1,
      MODE_RANGE = 2,
      MODE_LIST = 3,
      MODE_DEFAULT = MODE_NONE
    };

  private:
    bool     __reverse;
    int      __mode;
    uint32_t __range_min;
    uint32_t __range_max;
    set<uint32_t> __addrs;

  private:
    void __reset_range ();
    void __reset_list ();

  public:
    bool is_valid () const;
    void invalidate ();
    bool is_reverse () const;
    void set_reverse (bool = true);
    bool is_mode_none () const;
    bool is_mode_all () const;
    bool is_mode_range () const;
    bool is_mode_list () const;
    void set_mode_none ();
    void set_mode_all ();
    void set_mode_range ();
    void set_range (uint32_t min_, uint32_t max_);
    void set_mode_range (uint32_t min_, uint32_t max_);
    void set_mode_list ();
    void add_to_list (uint32_t val_);
    bool match (uint32_t val_) const;

    void reset ();

    // ctor:
    address_set ();

    friend ostream & operator<< (ostream & out_, const address_set & i_);

    friend istream & operator>> (istream & in_, address_set & i_);

  };

} // end of namespace geomtools

#endif // __geomtools__address_set_h

// end of address_set.h

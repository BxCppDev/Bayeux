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

#include <iostream>
#include <set>

#include <boost/cstdint.hpp>

namespace geomtools {

  class address_set
  {
 
  public:
    
    enum mode_type
    {
      MODE_INVALID = -1,
      MODE_NONE    = 0,
      MODE_ALL     = 1,
      MODE_RANGE   = 2,
      MODE_LIST    = 3,
      MODE_DEFAULT = MODE_NONE
    };

  private:
    void _reset_range_ ();
    void _reset_list_ ();

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
    void set_range (uint32_t a_min, uint32_t a_max);
    void set_mode_range (uint32_t a_min, uint32_t a_max);
    void set_mode_list ();
    void add_to_list (uint32_t a_value);
    bool match (uint32_t a_value) const;

    void reset ();

    // ctor:
    address_set ();

    friend std::ostream & operator<< (std::ostream & a_out, const address_set & a_addset);

    friend std::istream & operator>> (std::istream & a_in, address_set & a_addset);

  private:

    bool          _reverse_;
    int           _mode_;
    uint32_t      _range_min_;
    uint32_t      _range_max_;
    std::set<uint32_t> _addresses_;

  };

} // end of namespace geomtools

#endif // __geomtools__address_set_h

// end of address_set.h

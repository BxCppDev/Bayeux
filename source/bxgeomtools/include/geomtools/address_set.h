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

#ifndef GEOMTOOLS_ADDRESS_SET_H
#define GEOMTOOLS_ADDRESS_SET_H 1

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
      MODE_NONE    = 0, //!< No candidate value is valid
      MODE_ALL     = 1, //!< All candidate value are valid
      MODE_RANGE   = 2, //!< A candidate value is valid only if it lies in some specific range of values
      MODE_LIST    = 3, //!< A candidate value is valid only if it belongs to a specific list of values
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

    /// Check if a value matches the collection of addresses
    bool match (uint32_t a_value) const;

    /// Terminate
    void reset ();

    /// Constructor
    address_set ();

    friend std::ostream & operator<< (std::ostream & a_out, const address_set & a_addset);

    friend std::istream & operator>> (std::istream & a_in, address_set & a_addset);

  private:

    int           _mode_;         //!< The mode defining the collection of valid addresses
    uint32_t      _range_min_;    //!< In ``range`` mode, the minimum bound of valid addresses
    uint32_t      _range_max_;    //!< In ``range`` mode, the maximum bound of valid addresses
    std::set<uint32_t> _addresses_; //!< In ``list``mode, the explicit list of valid addresses
    bool          _reverse_; //!< In any mode, this flag reverse the matching of the candidate addresses

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_ADDRESS_SET_H

// end of address_set.h

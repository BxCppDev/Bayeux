/// \file geomtools/geom_id.h
/* Author (s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-08
 * Last modified: 2013-05-30
 *
 * License:
 *
 * Description:
 *
 *   Geometry ID
 *
 * A geometry ID is composed of two parts:
 * - the 'type' is an integer representing the category of geometry object
 *   each type object in a geometry setup has an unique category, thus a unique TYPE integer
 * - the 'address' is a list of integers representing the path to the object located in
 *   the hierarchy tree of the setup.
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_GEOM_ID_H
#define GEOMTOOLS_GEOM_ID_H 1

// Standard library:
#include <iostream>
#include <vector>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools :
#include <datatools/i_serializable.h>
#include <datatools/reflection_interface.h>

// This project :
#include <geomtools/geomtools_config.h>

namespace geomtools {

  class geom_id : public datatools::i_serializable
  {
  public:

    static const uint32_t INVALID_TYPE          = 0xFFFFFFFF;
    static const uint32_t INVALID_ADDRESS       = 0xFFFFFFFF;
    static const uint32_t ANY_ADDRESS           = 0xFFFFFFFE;
    static const uint32_t DEFAULT_ADDRESS_DEPTH = 10;
    static const uint32_t UNIVERSE_TYPE         = 0;
    static const uint32_t WORLD_TYPE            = geom_id::UNIVERSE_TYPE;

    static const char IO_ID_OPEN;
    static const char IO_ID_SEPARATOR;
    static const char IO_ADDRESS_SEPARATOR;
    static const char IO_TYPE_INVALID;
    static const char IO_ADDRESS_INVALID;
    static const char IO_ID_CLOSE;
    static const char IO_ADDRESS_ANY;

    /// Check if the type match a given value
    bool is_type (uint32_t) const;

    /// Return the type
    uint32_t get_type () const;

    /// Set the type
    void set_type (uint32_t);

    /// Set the depth
    void set_depth (uint32_t depth_);

    /// Returns the address' depth
    uint32_t get_depth () const;

    /// Returns the subaddress value at given index
    uint32_t get (int) const;

    /// Set the subaddress at given index to a given value
    void set (int, uint32_t val_);

    /// Set the subaddress at given index to the any value
    void set_any (int);

    /// Invalidate the subaddress at given index to 'any'
    void set_invalid (int);

    /// Check if subaddress at given index is invalid
    bool is_invalid (int) const;

    /// Check if subaddress at given index is 'any'
    bool is_any (int) const;

    /// Set the addresses (up to depth = 10)
    void set_address (uint32_t si0_,
                      uint32_t si1_ = INVALID_ADDRESS,
                      uint32_t si2_ = INVALID_ADDRESS,
                      uint32_t si3_ = INVALID_ADDRESS,
                      uint32_t si4_ = INVALID_ADDRESS,
                      uint32_t si5_ = INVALID_ADDRESS,
                      uint32_t si6_ = INVALID_ADDRESS,
                      uint32_t si7_ = INVALID_ADDRESS,
                      uint32_t si8_ = INVALID_ADDRESS,
                      uint32_t si9_ = INVALID_ADDRESS);

    /// Reset the address
    void reset_address ();

    /// Default constructor
    geom_id ();

    /// Constructor (depth=1)
    geom_id (uint32_t type_,
             uint32_t si0_);

    /// Constructor (depth=2)
    geom_id (uint32_t type_,
             uint32_t si0_,
             uint32_t si1_);

    /// Constructor (depth=3)
    geom_id (uint32_t type_,
             uint32_t si0_,
             uint32_t si1_,
             uint32_t si2_);

    /// Constructor (depth=4)
    geom_id (uint32_t type_,
             uint32_t si0_,
             uint32_t si1_,
             uint32_t si2_,
             uint32_t si3_);

    /// Constructor (up to depth = 10)
    geom_id (uint32_t type_,
             uint32_t si0_,
             uint32_t si1_,
             uint32_t si2_,
             uint32_t si3_,
             uint32_t si4_,
             uint32_t si5_ = INVALID_ADDRESS,
             uint32_t si6_ = INVALID_ADDRESS,
             uint32_t si7_ = INVALID_ADDRESS,
             uint32_t si8_ = INVALID_ADDRESS,
             uint32_t si9_ = INVALID_ADDRESS);

    /// Destructor
    ~geom_id () override = default;

    /// Copy constructor
    geom_id(const geom_id&) = default;

    /// Copy assignment
    geom_id& operator=(const geom_id&) = default;

    /// Move constructor
    geom_id(geom_id&&) = default;

    /// Move assignment
    geom_id& operator=(geom_id&&) = default; 

    /// Reset/invalidate the GID
    void reset ();

    /// Reset/invalidate the GID
    void invalidate ();

    /// Check if the GID is valid
    bool is_valid () const;

    /// Check if the GID is complete
    bool is_complete () const;

    /// Fill the target GID's addresses with current GID's addresses up to the target GID's depth
    void extract_to (geom_id & target_) const;

    void extract_from (const geom_id & source_);

    void inherits_from (const geom_id & source_);

    /// Non-overloaded version for CAMP reflection tools:
    bool match_impl (const geom_id & id_, bool exact_) const;

    bool match (const geom_id & id_, bool exact_ = false) const;

    static bool match (const geom_id & id1_, const geom_id & id2_,
                       bool exact_ = false);

    static int compare (const geom_id & id1_, const geom_id & id2_);

    bool operator== (const geom_id & id_) const;

    bool operator!= (const geom_id & id_) const;

    bool operator< (const geom_id & id_) const;

    bool operator<= (const geom_id & id_) const;

    bool operator>= (const geom_id & id_) const;

    bool operator> (const geom_id & id_) const;

    friend std::ostream & operator<< (std::ostream & out_, const geom_id & id_);

    friend std::istream & operator>> (std::istream & in_, geom_id & id_);

    /// Initialize the GID with a given type and address' depth
    void make (int type_, int depth_);

    static void make (geom_id & id_, int type_, int depth_);

    static bool sub_id_comp (uint32_t si1_, uint32_t si2_);

  private:

    uint32_t              _type_;      //!< The unique 'type' identifier of the geometry category associated to the GID
    std::vector<uint32_t> _addresses_; //!< The array of addresses

    DATATOOLS_SERIALIZATION_DECLARATION()

    //! Reflection interface
    DR_CLASS_RTTI()

  };

} // end of namespace geomtools

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(geomtools::geom_id, "geomtools::geom_id")

//#warning Activate reflection layer for the geomtools::geom_id class
// Activate reflection layer for the geomtools::geom_id class :
DR_CLASS_INIT(::geomtools::geom_id)

// Class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(geomtools::geom_id, 1)

#endif // GEOMTOOLS_GEOM_ID_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

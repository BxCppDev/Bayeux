// -*- mode: c++ ; -*-
/* base_hit.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-16
 * Last modified: 2013-03-08
 *
 * License:
 *
 * Description:
 *
 *   Base hit.
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_BASE_HIT_H_
#define GEOMTOOLS_BASE_HIT_H_ 1

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <limits>
#include <vector>
#include <algorithm>

// Boost  serialization hook :
#include <boost/serialization/access.hpp>
#include <boost/cstdint.hpp>

// Interfaces :
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/i_clear.h>
#include <datatools/i_predicate.h>
#include <datatools/bit_mask.h>

// Attributes :
#include <geomtools/geomtools_config.h>
#include <datatools/properties.h>
#include <geomtools/geom_id.h>

namespace geomtools {

  /// The base class for all hit objects
  class base_hit :
    DATATOOLS_SERIALIZABLE_CLASS,
    public datatools::i_tree_dumpable ,
    public datatools::i_clear
  {
  public:

    /// Masks to automatically tag the attributes to be stored
    enum store_mask_type {
      STORE_NOTHING     = 0,
      STORE_HIT_ID      = datatools::bit_mask::bit00,
      STORE_GEOM_ID     = datatools::bit_mask::bit01,
      STORE_AUXILIARIES = datatools::bit_mask::bit02,
    };

    static const int32_t INVALID_HIT_ID = -1;

    /// Check if the hit ID is valid
    bool has_hit_id () const;

    /// Get the hit ID integer value
    int32_t get_hit_id () const;

    /// Set the hit ID integer value
    void set_hit_id (int32_t);

    /// Reset the hit ID integer value
    void invalidate_hit_id ();

    /// Check if the geometry ID is valid
    bool has_geom_id () const;

    /// Get a reference on the non-mutable geometry ID
    const geomtools::geom_id & get_geom_id () const;

    /// Get a reference on the mutable geometry ID
    geomtools::geom_id & grab_geom_id ();

    /// Set the geometry ID
    void set_geom_id (const geomtools::geom_id &);

    /// Reset the geometry ID
    void invalidate_geom_id ();

    /// Check if there are stored auxiliary properties
    bool has_auxiliaries () const;

    /// Get a non-mutable reference on the auxiliaries container
    const datatools::properties & get_auxiliaries () const;

    /// Get a  mutable reference on the auxiliaries container
    datatools::properties & grab_auxiliaries ();

    /// Set the auxiliaries container
    void set_auxiliaries (const datatools::properties &);

    /// Reset the mutable auxiliaries container
    void invalidate_auxiliaries ();

    /// Default constructor
    base_hit ();

    /// Destructor
    virtual ~base_hit ();

    /// Check if the hit is valid
    /** We consider a base hit valid if:
     * - a valid hit ID is set
     * - a valid geometry ID is set
     */
    virtual bool is_valid () const;

    /// Reset the internals of the hit, making it invalid
    virtual void invalidate ();

    /// Reset the internals of the hit, making it invalid
    void reset ();

    /// Reset the internals of the hit, making it invalid
    virtual void clear ();

    /*** measurement ***/

    /// Generic abstract interface for a measurement on a hit
    class i_measurement
    {
    public:

      /// Main virtual interface method that performs a measurement on a hit
      virtual void do_measurement (base_hit &) = 0;

      /// Functor interface
      base_hit & operator () (base_hit &);

    };

    /// Apply a measurement on the current hit
    base_hit & measure (i_measurement &);

    /*** interface i_tree_dumpable ***/

    /// Smart print
    virtual void tree_dump (std::ostream & a_out    = std::clog,
                            const std::string & a_title  = "",
                            const std::string & a_indent = "",
                            bool a_inherit          = false) const;

    /// Smart print (default behaviour)
    void dump () const;

    /*** predicates ***/

    /// Predicate that checks if the auxiliaries container stores some specific flag
    class has_flag_predicate : public datatools::i_predicate<base_hit>
    {
      std::string flag_; /// The flag to be checked

    public:

      /// Constructor
      explicit has_flag_predicate (const std::string & a_flag)
      {
        flag_ = a_flag;
        return;
      }

      /// Functor interface
      bool operator () (const base_hit & a_hit) const
      {
        return (a_hit.get_auxiliaries ().has_flag (flag_));
      }

    };

    /// Predicate that checks if the auxiliaries container stores some property with a specific key
    class has_key_predicate : public datatools::i_predicate<base_hit>
    {
      std::string key_; /// The property key to be checked

    public:

      /// Constructor
      explicit has_key_predicate (const std::string & a_key)
      {
        key_ = a_key;
        return;
      }

      /// Functor interface
      bool operator () (const base_hit & a_hit) const
      {
        return (a_hit.get_auxiliaries ().has_key (key_));
      }

    };

    /// Predicate that checks if the auxiliaries container stores some property with a specific key
    class has_string_property_predicate : public datatools::i_predicate<base_hit>
    {
      std::string key_; /// The property key to be checked
      std::vector<std::string> values_; /// The vector of accepted values

    public:

      /// Constructor
      explicit has_string_property_predicate (const std::string & a_key,
                                              const std::string & a_value)
      {
        key_ = a_key;
        values_.push_back (a_value);
        return;
      }

      /// Constructor
      explicit has_string_property_predicate (const std::string & a_key,
                                              const std::vector<std::string> & a_values)
      {
        key_ = a_key;
        values_ = a_values;
        return;
      }

      /// Functor interface
      bool operator () (const base_hit & a_hit) const
      {
        if (! a_hit.get_auxiliaries ().has_key (key_)) return false;
        if (! a_hit.get_auxiliaries ().is_string (key_)) return false;
        const std::string & str = a_hit.get_auxiliaries ().fetch_string (key_);
        return (std::find (values_.begin (),
                           values_.end (),
                           str) != values_.end ());
      }

    };

    /// Predicate that checks if the hit has some specific ID
    class has_hit_id_predicate : public datatools::i_predicate<base_hit>
    {
    public:

      /// Constructor
      explicit has_hit_id_predicate (int a_hid)
      {
        hid_ = a_hid;
        return;
      }

      /// Functor interface
      bool operator () (const base_hit & a_hit) const
      {
        return (a_hit.has_hit_id () && a_hit.get_hit_id () == hid_);
      }

    private:

      int hid_; /// The hit ID to be checked

    };


    /// Predicate that checks if the hit has some specific geometry ID
    class has_geom_id_predicate : public datatools::i_predicate<base_hit>
    {
    public:

      /// Constructor
      explicit has_geom_id_predicate (const geomtools::geom_id & a_gid)
      {
        gid_ = a_gid;
        return;
      }

      /// Functor interface
      bool operator () (const base_hit & a_hit) const
      {
        return (a_hit.has_geom_id () && a_hit.get_geom_id () == gid_);
      }

    private:

      geomtools::geom_id gid_; /// The geometry ID to be checked

    };


    /// Predicate that negates another predicate
    class negates_predicate : public datatools::i_predicate<base_hit>
    {
    public:

      /// Constructor
      explicit negates_predicate (const datatools::i_predicate<base_hit> & a_pred)
      {
        pred_ = &a_pred;
        return;
      }

      /// Functor interface
      bool operator () (const base_hit & a_hit) const
      {
        return (! (*pred_)(a_hit));
      }

    private:

      const datatools::i_predicate<base_hit> * pred_;

    };

  protected:

    /** The \a store field records the usage status of
     * the each data field members.
     * One bit is affected to each field:
     *  \a 0 : field member is not used
     *  \a 1 : field member is used
     * This enables to serialize only the useful part of
     * the hit. It is made protected in order to allow
     * daughter classes to use it (32 bits are available)
     */
    uint32_t              _store;

  private:

    int32_t               _hit_id_;      //!< Unique hit ID
    geomtools::geom_id    _geom_id_;     //!< Geometry ID
    datatools::properties _auxiliaries_; //!< Auxiliary properties

    DATATOOLS_SERIALIZATION_DECLARATION();

#if GEOMTOOLS_WITH_REFLECTION == 1
    //! Reflection interface
    DR_CLASS_RTTI();
#endif

  };

} // end of namespace geomtools

#if GEOMTOOLS_WITH_REFLECTION == 1
// Activate reflection layer for the geomtools::base_hit class :
DR_CLASS_INIT(::geomtools::base_hit);
#endif // GEOMTOOLS_WITH_REFLECTION

#endif // GEOMTOOLS_BASE_HIT_H_

// end of base_hit.h

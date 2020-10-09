/// \file geomtools/base_hit.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-16
 * Last modified: 2018-08-16
 *
 * License: GPL3
 *
 * Description:
 *
 *   Base hit. 
 *
 */

#ifndef GEOMTOOLS_BASE_HIT_H
#define GEOMTOOLS_BASE_HIT_H 1

// Third party:
// - Boost:
#include <boost/serialization/access.hpp>
#include <boost/cstdint.hpp>
// - Bayeux/datatools :
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/i_clear.h>
#include <datatools/i_predicate.h>
#include <datatools/bit_mask.h>
#include <datatools/properties.h>
#include <datatools/factory_macros.h>

// This project :
#include <geomtools/geomtools_config.h>
#include <geomtools/geom_id.h>
#include <datatools/properties.h>

namespace geomtools {

  /// \brief The base class for hit objects that locate events in a geometry model.
  class base_hit :
    public datatools::i_serializable,
    public datatools::i_tree_dumpable,
    public datatools::i_clear
  {
  public:

    /// Masks to automatically tag the attributes to be stored
    enum store_mask_type {
      STORE_NOTHING     = 0,
      STORE_HIT_ID      = datatools::bit_mask::bit00, //!< Serialization mask for the hit ID attribute
      STORE_GEOM_ID     = datatools::bit_mask::bit01, //!< Serialization mask for the geometry ID attribute
      STORE_AUXILIARIES = datatools::bit_mask::bit02, //!< Serialization mask for the auxiliary properties attribute
      STORE_RESERVED30  = datatools::bit_mask::bit30, //!< Reserved bit (lock)
      STORE_RESERVED31  = datatools::bit_mask::bit31  //!< Reserved bit (chained)
    };

    static const int32_t INVALID_HIT_ID = -1;

    /// Check if the hit ID is valid
    /// @code
    /// geomtools::base_hit hit0;
    /// hit0.set_hit_id(34);
    /// if (hit0.has_hit_id()) {
    ///   ...
    /// }
    /// @endcode
    bool has_hit_id () const;

    /// Get the hit ID integer value
    /// @code
    /// geomtools::base_hit hit0;
    /// hit0.set_hit_id(34);
    /// std::clog << hit0.get_hit_id() << '\n';
    /// @endcode
    int32_t get_hit_id () const;

    /// Set the hit ID integer value
    void set_hit_id (int32_t);

    /// Reset the hit ID integer value
    /// @code
    /// geomtools::base_hit hit0;
    /// hit0.set_hit_id(34);
    /// hit0.invalidate_hit_id();
    /// if (hit0.has_hit_id()) {
    ///   ...
    /// }
    /// @endcode
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
    base_hit() = default;

    /// Destructor
    ~base_hit() override = default;

    /// Copy constructor
    base_hit(const base_hit &) = default;

    /// Copy assignment
    base_hit & operator=(const base_hit &) = default;

    /// Move constructor
    base_hit(base_hit &&) = default;

    /// Move assignment
    base_hit & operator=(base_hit &&) = default;

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
    void clear () override;

    /*  measurement */

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

    /* interface i_tree_dumpable */

    /// \deprecated Smart print
    void tree_dump (std::ostream & a_out    = std::clog,
                            const std::string & a_title  = "",
                            const std::string & a_indent = "",
                            bool a_inherit          = false) const override;
    //!
    //! Supported options:
    //! \code
    //! {
    //!   "title"    : "My title: ",
    //!   "indent"   : "[debug] ",
    //!   "inherit"  : false,
    //!   "no_list_auxiliaries" : false
    //! }
    //! \endcode
    void print_tree(std::ostream & out_ = std::clog,
                    const boost::property_tree::ptree & options_ = datatools::i_tree_dumpable::empty_options()) const override;
      
    /// Smart print (default behaviour)
    void dump() const;

    /* predicates */

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
      bool operator () (const base_hit & a_hit) const override
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
      bool operator () (const base_hit & a_hit) const override
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
      bool operator () (const base_hit & a_hit) const override
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
      bool operator () (const base_hit & a_hit) const override
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
      bool operator () (const base_hit & a_hit) const override
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
      bool operator () (const base_hit & a_hit) const override
      {
        return (! (*pred_)(a_hit));
      }

    private:

      const datatools::i_predicate<base_hit> * pred_;

    };

  protected:

    /// Set the bits from a mask
    void _store_set(uint32_t bit_mask_);

    /// Unset the bits from a mask
    void _store_unset(uint32_t bit_mask_);

    /// Check the bits from a mask
    bool _store_check(uint32_t bit_mask_) const;

    /** The \a store field records the usage status of
     * the each data field members.
     * One bit is affected to each field:
     *  \a 0 : field member is not used
     *  \a 1 : field member is used
     * This enables to serialize only the useful part of
     * the hit. It is made protected in order to allow
     * daughter classes to use it (32 bits are available)
     */
    uint32_t              _store = STORE_NOTHING;

  private:

    int32_t               _hit_id_ = INVALID_HIT_ID;      //!< Unique hit ID
    geomtools::geom_id    _geom_id_{};     //!< Geometry ID
    datatools::properties _auxiliaries_{}; //!< Auxiliary properties

    DATATOOLS_SERIALIZATION_DECLARATION()

    //! Reflection interface
    DR_CLASS_RTTI()

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(base_hit)

    DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(::geomtools::base_hit, base_hit)
    
  };

} // end of namespace geomtools

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(geomtools::base_hit, "geomtools::base_hit")

// Activate reflection layer for the geomtools::base_hit class :
DR_CLASS_INIT(::geomtools::base_hit)

// Class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(geomtools::base_hit, 1)

#define GEOMTOOLS_HIT_REGISTRATION_INTERFACE(HitClassName)          \
  private:                                                              \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(::geomtools::base_hit,HitClassName) \
  /**/

#define GEOMTOOLS_HIT_REGISTRATION_IMPLEMENT(HitClassName,HitClassId) \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(::geomtools::base_hit,HitClassName,HitClassId) \
  /**/

#endif // GEOMTOOLS_BASE_HIT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

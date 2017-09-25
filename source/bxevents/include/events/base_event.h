/// \file events/base_event.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2017-09-09
 * Last modified: 2017-09-12
 *
 * License: GPL3
 *
 * Description:
 *
 *   Base event in spacetime with given category
 *
 */

#ifndef EVENTS_BASE_EVENT_H
#define EVENTS_BASE_EVENT_H

// Standard library:
#include <iostream>
#include <string>

// Third party:
// - Bayeux/datatools :
#include <datatools/bit_mask.h>
// - Bayeux/geomtools :
#include <geomtools/geomtools_config.h>
#include <geomtools/utils.h>
#include <geomtools/base_hit.h>

namespace events {

  class base_event
    : public geomtools::base_hit
  {

  public:

    /// \brief Masks to automatically tag the attributes to be stored
    enum store_mask_type {
      STORE_CATEGORY  = datatools::bit_mask::bit03, //!< Serialization mask for the event category
      STORE_POSITION  = datatools::bit_mask::bit04, //!< Serialization mask for the position
      STORE_TIME      = datatools::bit_mask::bit05  //!< Serialization mask for the time
    };

    static const int32_t ANY_CATEGORY     = -2;
    static const int32_t INVALID_CATEGORY = -1;
    static const int32_t FIRST_CATEGORY   =  0;

    /// Default constructor
    base_event();

    // Destructor
    virtual ~base_event();

    /// Check if event category is set
    bool has_category() const;

    /// Get the event category
    int32_t get_category() const;

    /// Set the event category
    void set_category(const int32_t);

    /// Reset/invalidate the event category
    void invalidate_category();

    /// Check if event position is set
    bool has_position() const;

    /// Get the position of the hit (the coordinate system and units are arbitrary)
    const geomtools::vector_3d & get_position() const;

    /// Set the position of the hit (the coordinate system and units are arbitrary)
    void set_position(const geomtools::vector_3d &);

    /// Reset/invalidate the position of the hit
    void invalidate_position();

    /// Check if event time is set
    bool has_time() const;

    /// Get the time of the event (units are arbitrary)
    double get_time() const;

    /// Set the time of the event (units are arbitrary)
    void set_time(const double);

    /// Reset/invalidate the time of the event
    void invalidate_time();

    /// Check if the event has a valid internal structure
    virtual bool is_valid() const;

    /// Reset/invalidate the internal structure of the hit
    virtual void invalidate();

    /// Reset/invalidate the internal structure of the event
    void reset();

    /// Reset/invalidate the internal structure of the event
    virtual void clear();

    /// Smart print
    virtual void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const;

  private:

    int32_t              _category_; //!< Event category ID
    geomtools::vector_3d _position_; //!< Position of the event in some arbitrary coordinate system and units
    double               _time_;     //!< Time of the event using some arbitrary time reference system and units

    DATATOOLS_SERIALIZATION_DECLARATION()

#if EVENTS_WITH_REFLECTION == 1
    //! Reflection interface
    DR_CLASS_RTTI()
#endif

  };

} // end of namespace events

#if EVENTS_WITH_REFLECTION == 1
// Activate reflection layer for the events::base_event class :
DR_CLASS_INIT(::events::base_event)
#endif // EVENTS_WITH_REFLECTION

#endif // EVENTS_BASE_EVENT_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

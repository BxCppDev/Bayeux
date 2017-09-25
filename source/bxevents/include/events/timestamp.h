/// \file events/timestamp.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2017-09-13
 * Last modified: 2017-09-25
 *
 * License: GPL3
 *
 * Description:
 *
 *   High precision timestamp for nuclear and particle physics events
 *
 */

#ifndef EVENTS_TIMESTAMP_H
#define EVENTS_TIMESTAMP_H

// Standard library:
#include <limits>

// Bayeux/datatools:
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>

namespace events {

  /// \brief A high precision timestamp
  ///
  ///        Timestamp is defined through the number of seconds
  ///        elapsed from an external time reference and a fractional
  ///        part expressed in femtoseconds.
  class timestamp
    : public datatools::i_serializable
    , public datatools::i_tree_dumpable
  {
  public:

    static const int64_t SECONDS_INVALID        = std::numeric_limits<int64_t>::min();
    static const int64_t SECONDS_MINUS_INFINITY = std::numeric_limits<int64_t>::min() + 1;
    static const int64_t SECONDS_MIN            = std::numeric_limits<int64_t>::min() + 2;
    static const int64_t SECONDS_ZERO           = 0;
    static const int64_t SECONDS_MAX            = std::numeric_limits<int64_t>::max() - 1;
    static const int64_t SECONDS_PLUS_INFINITY  = std::numeric_limits<int64_t>::max();

    // /// \brief Precision type for the second fraction
    // enum precision_type {
    //   PRECISION_INVALID     = 0xF,
    //   PRECISION_SECOND      = 0x0,
    //   PRECISION_MILLISECOND = 0x1,
    //   PRECISION_MICROSECOND = 0x2,
    //   PRECISION_NANOSECOND  = 0x3,
    //   PRECISION_PICOSECOND  = 0x4,
    //   PRECISION_FEMTOSECOND = 0x5,
    //   PRECISION_MAXIMUM     = PRECISION_FEMTOSECOND
    // };

    // /// \brief Time reference type
    // enum time_reference_type {
    //   TIME_REFERENCE_INVALID = 0xF,
    //   TIME_REFERENCE_EPOCH   = 0x0,
    //   TIME_REFERENCE_OTHER   = 0x1
    // };

    static const uint64_t FEMTOSECONDS_MIN     = 0;
    static const uint64_t FEMTOSECONDS_MAX     = 999999999999999999;
    static const uint64_t FEMTOSECONDS_INVALID = 0xFFFFFFFFFFFFFFFF;

    static const char IO_FORMAT_OPEN    = '[';
    static const char IO_FORMAT_SEP     = '.';
    static const char IO_FORMAT_CLOSE   = ']';
    static const char IO_FORMAT_INVALID = '!';
    static const char IO_FORMAT_ANY     = '*';

    static bool is_finite_seconds(const int64_t);

    static bool is_min_seconds(const int64_t);

    static bool is_max_seconds(const int64_t);

    /// Check if seconds are set
    bool has_seconds() const;

    /// Return the number of seconds
    int64_t get_seconds() const;

    /// Set the number of seconds
    void set_seconds(const int64_t);

    /// Reset the number of seconds
    void reset_seconds();

    /// Return the number of milliseconds
    uint64_t get_milliseconds() const;

    /// Return the number of microseconds
    uint64_t get_microseconds() const;

    /// Return the number of nanoseconds
    uint64_t get_nanoseconds() const;

    /// Return the number of picoseconds
    uint64_t get_picoseconds() const;

    /// Return the number of femtoseconds
    uint64_t get_femtoseconds() const;

    /// Set the number of milliseconds
    void set_milliseconds(const uint64_t);

    /// Set the number of microseconds
    void set_microseconds(const uint64_t);

    /// Set the number of nanoseconds
    void set_nanoseconds(const uint64_t);

    /// Set the number of picoseconds
    void set_picoseconds(const uint64_t);

    /// Set the number of femtoseconds
    void set_femtoseconds(const uint64_t);

    /// Reset the fractional part
    void reset_femtoseconds();

    /// Check if femtoseconds are set
    bool has_femtoseconds() const;

    /// Shift number of seconds
    timestamp & add_seconds(const int64_t shift_);

    // /// Add a duration
    // timestamp & add(const int64_t seconds_, const uint64_t femtosec);

    // /// Set the timestamp to the current date/time
    // void now();

    /// Constructor
    timestamp();

    /// Overloaded constructor
    timestamp(const int64_t sec_);

    /// Overloaded constructor
    timestamp(const int64_t sec_, const uint64_t femtosec_);

    /// Destructor
    virtual ~timestamp();

    /// Check if the timestamp object is valid
    bool is_valid() const;

    /// Invalidate the timestamp object
    void invalidate();

    /// Compare with another timestamp
    int compare(const timestamp &) const;

    /// Convert timestamp to real value (explicit time unit)
    double to_real(const int64_t time_ref_ = 0.0) const;

    /// Build timestamp from a real value (explicit time unit)
    bool from_real(const double sec_, const int64_t from_ = 0.0);

    /// Format time stamp as string object and return it
    std::string to_string() const;

    /// Format time stamp as string object and return it as reference
    void to_string(std::string & ) const;

    /// Parse time from string object and set the timestamp object accordingly
    void from_string(const std::string &);

    friend bool operator==(const timestamp &, const timestamp &);
    friend bool operator<(const timestamp &, const timestamp &);
    friend bool operator>(const timestamp &, const timestamp &);
    friend bool operator<=(const timestamp &, const timestamp &);
    friend bool operator>=(const timestamp &, const timestamp &);
    friend std::ostream & operator<<(std::ostream &, const timestamp &);
    friend std::istream & operator>>(std::istream &, timestamp &);

    /// Smart print
    virtual void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const;


  private:

    int64_t  _seconds_;      //!< Number of seconds
    uint64_t _femtoseconds_; //!< Fraction of seconds in femtoseconds

    DATATOOLS_SERIALIZATION_DECLARATION()

  };

} // end of namespace events

// Activate reflection layer for the events::timestamp class :
DR_CLASS_INIT(::events::timestamp)

#endif // EVENTS_TIMESTAMP_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

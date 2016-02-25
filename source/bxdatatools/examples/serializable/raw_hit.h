// -*- mode: c++; -*-
/**  raw_hit.h
 *
 * A serializable class that (naively) represents a raw hit in
 * some physics experiment.
 *
 */

#ifndef RAW_HIT_H
#define RAW_HIT_H 1

// Standard library:
#include <iostream>
#include <string>
#include <vector>

// Third party:
// - Boost:
// Portable integral types (mandatory):
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
// Interface base class from datatools to support serialization tools:
#include <datatools/i_serializable.h>
// Use also this class from the datatools library :
#include <datatools/properties.h>

/// \brief ``raw_hit`` is a serializable class using datatools serialization
class raw_hit : public datatools::i_serializable
{
public:

  // Static constants :
  static const int          INVALID_ID      = -1;
  static const int          INVALID_CHANNEL = -1;
  static const unsigned int MAX_CHANNEL     = 128;
  static const unsigned int MAX_ADC         = 1024;
  static const unsigned int MAX_TDC         = 1048576;
  static const double       LOW_QUALITY_PROB;
  static const std::string  LOW_QUALITY_KEY;

  /// Default constructor
  raw_hit();

  /// Destructor
  virtual ~raw_hit();

  /// Reset
  void reset();

  /// Randomize internal attributes
  void randomize();

  /// Smart print
  void dump(std::ostream & out_ = std::clog,
            const std::string & title_ = "",
            const std::string & indent_ = "") const;

  /* Accessors and mutators */
  void set_id(int);

  int get_id() const;

  void set_simulated(bool);

  bool is_simulated() const;

  void set_channel(int);

  int get_channel() const;

  void set_adc(int);

  int get_adc() const;

  void set_tdc(int);

  int get_tdc() const;

  /// Access the auxiliary properties in read-only mode (accessor)
  const datatools::properties & get_auxiliaries() const;

  /// Access the auxiliary properties in read-write mode (mutator)
  datatools::properties & grab_auxiliaries();

private:

  /** We use here the ``[u]int[NBITS]_t`` Boost typedefs for
   *  integral types. This implies ``#include <boost/cstdint.hpp>``.
   */
  int16_t  _id_;         ///< The unique ID of the hit (16 bits)
  bool     _simulated_;  ///< Simulation flag
  int8_t   _channel_;    ///< The channel ID
  uint16_t _adc_;        ///< The ADC value
  uint32_t _tdc_;        ///< The TDC value
  datatools::properties _auxiliaries_; ///< The container of auxiliary properties
  // The ``datatools::properties`` class is made serializable by Bayeux/datatools
  // so an attribute of this class is safely processed while a raw hit object
  // is serialized.

  /** This macro declares the serialization interface
   *  within the datatools framework
   */
  DATATOOLS_SERIALIZATION_DECLARATION();

};

#endif // RAW_HIT_H

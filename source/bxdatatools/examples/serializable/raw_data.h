// -*- mode: c++; -*-
/** raw_data.h
 *
 * A serializable class that represents the raw data recorded
 * in some experimental physics detector.
 *
 */

#ifndef RAW_DATA_H_
#define RAW_DATA_H_ 1

#include <iostream>
#include <string>
#include <vector>

// Portable integral types (mandatory):
#include <boost/cstdint.hpp>

// Interface base class from datatools to support serialization tools:
#include <datatools/i_serializable.h>

// Use also this class from the datatools library :
#include <datatools/properties.h>

// Use the ``raw_hit`` class :
#include <raw_hit.h>

/** ``raw_data`` is a serializable class using datatools serialization
 *  concept and Boost/Serialization registration mechanism.
 */
class raw_data :
  DATATOOLS_SERIALIZABLE_CLASS
{
public:

  /// Default constructor
  raw_data();

  /// Destructor
  virtual ~raw_data();

  /// Reset
  void reset();

  /// Randomize hits
  void randomize_hits(unsigned int number_of_hits_);

  /// Smart print
  void dump(std::ostream & out_ = std::clog,
            const std::string & title_ = "",
            const std::string & indent_ = "") const;

  /* Accessors and mutators */

  /// Access the collection of raw hits in read-only mode (accessor)
  const std::vector<raw_hit> & get_hits() const;

  /// Access the auxiliary properties in read-write mode (mutator)
  std::vector<raw_hit> & grab_hits();

  /// Access the auxiliary properties in read-only mode (accessor)
  const datatools::properties & get_auxiliaries() const;

  /// Access the auxiliary properties in read-write mode (mutator)
  datatools::properties & grab_auxiliaries();

  /// Check the lock
  bool is_locked() const;

  /// Lock mutable access to the collection of raw hits
  void lock(const std::string & password_ = "");

  /// Unlock mutable access to the collection of raw hits
  void unlock(const std::string & password_ = "");

  /// Set the lock password
  void set_lock_password(const std::string & new_password_,
                         const std::string & old_password_);

private:
  bool _locked_;                       /** A flag to forbid modifications
                                        *  of the collection of raw hits
                                        */
  std::string _lock_password_;         /// Lock password (encrypted with a toy cypher)
  std::vector<raw_hit>  _hits_;        /// The collection of raw hits
  datatools::properties _auxiliaries_; /// The container of auxiliary properties

  /** This macro declares the serialization interface
   *  within the datatools framework
   */
  DATATOOLS_SERIALIZATION_DECLARATION();

};

// Special interface macro to register the class through the Boost/Serialization system :
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(raw_data, "raw_data");

#endif // RAW_DATA_H_

/* end of raw_data.h */

// -*- mode: c++; -*-
/// \file hit.h
#ifndef DPP_EX01_HIT_H
#define DPP_EX01_HIT_H 1

// Standard library:
#include <iostream>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.h>
#include <datatools/properties.h>

namespace dpp_ex01 {

  /// \brief A simple serializable class representing a hit in a detector
  class hit : public datatools::i_serializable
  {
  public:

    /// Default constructor
    hit();

    /// Destructor
    virtual ~hit();

    /// Returns the hit ID
    int32_t get_id() const;

    /// Set the hit ID
    void set_id(int32_t);

    /// Returns the energy
    double get_energy() const;

    /// Set the energy
    void set_energy(double);

    /// Returns the collection of auxiliary properties
    const datatools::properties & get_auxiliaries() const;

    /// Returns the mutable collection of auxiliary properties
    datatools::properties & grab_auxiliaries();

    /// Reset
    void reset();

    /// Smart print
    void dump(std::ostream &, const std::string & title_) const;

  private:

    int32_t  _id_; ///< The unique hit ID
    double   _energy_; ///< The energy deposit
    datatools::properties _auxiliaries_; ///< Auxiliary properties

    // Serialization interface :
    DATATOOLS_SERIALIZATION_DECLARATION();

  };

} // namespace dpp_ex01

#endif // DPP_EX01_HIT_H

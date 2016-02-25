// -*- mode: c++; -*-

#ifndef DPP_EX01_RAW_DATA_H
#define DPP_EX01_RAW_DATA_H 1

// Standard library:
#include <iostream>
#include <vector>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.h>
#include <datatools/properties.h>

// This project:
#include <dpp_ex01/hit.h>

namespace dpp_ex01 {

  /// \brief A simple serializable class representing a raw_data in a detector
  class raw_data : public datatools::i_serializable
  {
  public:

    /// Default constructor
    raw_data();

    /// Destructor
    virtual ~raw_data();

    /// Returns the collection of hits
    const std::vector<hit> & get_hits() const;

    /// Returns the mutable collection of hits
    std::vector<hit> & grab_hits();

    /// Returns the collection of auxiliary properties
    const datatools::properties & get_auxiliaries() const;

    /// Returns the mutable collection of auxiliary properties
    datatools::properties & grab_auxiliaries();

    /// Smart print
    void dump(std::ostream &, const std::string & title_) const;

  private:

    std::vector<hit> _hits_; ///< The collection of hits
    datatools::properties _auxiliaries_; ///< Auxiliary properties

    // Serialization interface (advanced version) :
    DATATOOLS_SERIALIZATION_DECLARATION_ADVANCED(raw_data)

  };

} // namespace dpp_ex01

// Boost/Serialization export interface :
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(dpp_ex01::raw_data, "dpp_ex01::raw_data")

#endif // DPP_EX01_RAW_DATA_H

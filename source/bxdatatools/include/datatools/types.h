// -*- mode: c++; -*-
/// \file datatools/types.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-03-20
 * Last modified: 2014-06-23
 */

#ifndef DATATOOLS_TYPES_H
#define DATATOOLS_TYPES_H

// Standard library:
#include <string>

namespace datatools {

  //! \brief Basic type identifier
  // Note (2014/06/23 FM): we should have used the
  // datatools::bit_mask::bitXX values here, with:
  //   TYPE_NONE    = 0x0,
  //   TYPE_BOOLEAN = datatools::bit_mask::bit00,
  //   TYPE_INTEGER = datatools::bit_mask::bit01,
  //   TYPE_REAL    = datatools::bit_mask::bit02,
  //   TYPE_STRING  = datatools::bit_mask::bit03,
  // However, this is not compatible with the original values
  // below.
  // Any change would imply some modification of the datatools::properties
  // serialization method and version.
  // We do not change it for now.
  enum basic_type
  {
    TYPE_NONE    = 0x0,
    TYPE_BOOLEAN = 0x1,
    TYPE_INTEGER = 0x2,
    TYPE_REAL    = 0x3, // Should be 0x4
    TYPE_STRING  = 0x4, // Should be 0x8
    TYPE_ANY     = TYPE_BOOLEAN | TYPE_INTEGER | TYPE_REAL | TYPE_STRING // This is buggy
  };

  //! \brief Data type
  enum data_type
  {
    DATA_NONE    = 0x0, //!< Not defined
    DATA_SCALAR  = 0x1, //!< Data is a scalar
    DATA_ARRAY   = 0x2, //!< Data is an array (or vector)
    DATA_ANY     = DATA_SCALAR | DATA_ARRAY //!< Data can be both
  };

  //! Return the label associated to a data type
  const std::string get_label_from_type(int type_);

  //! Return the data type associated to a label
  int get_type_from_label(const std::string & label_);

} // end of namespace datatools

#endif // DATATOOLS_TYPES_H

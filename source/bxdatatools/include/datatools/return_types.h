/// \file datatools/return_types.h
/// \brief Common return types

#ifndef DATATOOLS_RETURN_TYPES_H
#define DATATOOLS_RETURN_TYPES_H

#include <cstdlib>

namespace datatools {

  /// \brief Return codes
  enum return_code_type {
    SUCCESS = EXIT_SUCCESS, //!< Returned code in case of success
    ERROR   = EXIT_FAILURE, //!< Returned code in case of failure
    FAILURE = EXIT_FAILURE  //!< Returned code in case of failure
  };

  /// \brief Comparison result code for comparable objects
  enum compare_result_type {
    COMPARE_UNDEFINED = -2, //!< Comparison cannot be performed
    COMPARE_LESS      = -1, //!< Less than
    COMPARE_EQUAL     =  0, //!< Equal to
    COMPARE_GREATER   =  1  //!< Greater than
  };

} // namespace datatools

#endif // DATATOOLS_RETURN_TYPES_H


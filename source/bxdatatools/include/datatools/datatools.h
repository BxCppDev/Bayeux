//! \file   datatools/datatools.h
//! \author François Mauger
//! \brief  Describe the datatools API configuration

#ifndef DATATOOLS_DATATOOLS_H
#define DATATOOLS_DATATOOLS_H 1

// Third Party:
// - Boost:
#include <boost/noncopyable.hpp>

// This project:
#include <datatools/logger.h>

/// \brief Top-level namespace of the Bayeux/datatools module library
namespace datatools {

  /// \brief Nested namespace of the Bayeux/datatools module library (detail)
  namespace detail {

    /// \brief System class for developpers:
    struct sys : private boost::noncopyable
    {
      /// Default constructor
      sys();

      /// Destructor
      virtual ~sys();

      /// Return the logging priority
      logger::priority get_logging() const;

      /// Access to the 'sys' singleton (read-only)
      static const sys & const_instance();

    private:

      logger::priority _logging_; ///< Logging priority threshold

    };

  } // end of namespace detail

  /// Initialize the datatools library and kernel
  void initialize(int argc_ = 0, char * argv_[] = 0);

  /// Terminate the datatools library and kernel
  void terminate();

}

#endif // DATATOOLS_DATATOOLS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

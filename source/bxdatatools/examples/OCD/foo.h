// -*- mode: c++; -*-
/**  foo.h
 *
 * A class with OCD support
 *
 */

#ifndef FOO_H
#define FOO_H 1

// Standard library:
#include <string>
#include <map>

// Third party:
// - Bayeux/datatools:
#include <datatools/datatools_config.h>
#include <datatools/properties.h>
#include <datatools/logger.h>

/// \brief The foo user class with some initialization stuff
struct foo {
  /// Constructor
  foo();
  /// Initialization from a ``datatools::properties`` instance
  void initialize(const datatools::properties & config_);
  /// Reset
  void reset();
  /// Smart print
  void dump(std::ostream &) const;
  /// Set logging priority threshold
  void set_logging_priority(const datatools::logger::priority&);

  // Attributes (should be private but here this is just an example) :
 public:
  bool initialized; ///< Initialization flag
  std::string name; ///< Name
  std::string what; ///< Description
  std::string tmpfile; ///< Temporary filename
  std::map<std::string, std::pair<int,std::string> > dict; ///< A dictionary
  int dummy; ///< A dummy attributes
  double width; ///< Width
  double weight; ///< Weight
  double length; ///< Length

 private:
  bool _logging_; ///< Logging flag
  datatools::logger::priority _logging_level_; ///< Logging priority

};

/***************************
 * OCD support : interface *
 ***************************/

#include <datatools/ocd_macros.h>

// @arg foo the name the class
DOCD_CLASS_DECLARATION(foo)

#endif // FOO_H

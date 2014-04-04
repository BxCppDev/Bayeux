// -*- mode: c++; -*-
/**  foo.h
 *
 * A class with OCD support
 *
 */

#ifndef FOO_H_
#define FOO_H_ 1

#include <string>
#include <map>

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
  bool initialized;
  std::string name;
  std::string what;
  std::string tmpfile;
  std::map<std::string, std::pair<int,std::string> > dict;
  int dummy;
  double width;
  double weight;

 private:
  bool _logging_;
  datatools::logger::priority _logging_level_; /// Logging priority

};

/***************************
 * OCD support : interface *
 ***************************/

#include <datatools/ocd_macros.h>

// @arg foo the name the class
DOCD_CLASS_DECLARATION(foo)

#endif // FOO_H_

/* end of foo.h */

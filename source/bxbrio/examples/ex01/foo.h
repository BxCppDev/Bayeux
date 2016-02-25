// -*- mode: c++; -*-
/** foo.h
 *
 * A serializable sample class
 *
 */

#ifndef FOO_H
#define FOO_H 1

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

/** \brief A sample serializable class using datatools and/or brio serialization
 *  concept.
 */
class foo : public datatools::i_serializable
{

public:

  /// Default constructor
  foo ();

  /// Destructor
  virtual ~foo ();

  /// Randomize internal attributes
  void randomize ();

  /// Print
  void dump (std::ostream & a_out = std::clog,
             const std::string & a_title = "") const;

private:

  bool     _bval_; ///< A boolean
  int8_t   _cval_; ///< A signed char (8 bits, implies boost/cstdint.hpp)
  int32_t  _ival_; ///< A 32 bits signed integral
  float    _fval_; ///< A 32 bits float
  double   _dval_; ///< A 64 bits float
  std::string   _sval_; ///< A STL string (implies boost/serialization/string.hpp)
  std::vector<double> _dval_vec_; ///< A STL vector of 64 bits floats (implies boost/serialization/vector.hpp)

  // interface i_serializable
  DATATOOLS_SERIALIZATION_DECLARATION();

};

#endif // FOO_H

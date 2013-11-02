// -*- mode: c++; -*-
/* bio_link_guard.h */
#ifndef DATATOOLS_DETAIL_BIO_LINK_GUARD_H_
#define DATATOOLS_DETAIL_BIO_LINK_GUARD_H_

namespace datatools {
namespace detail {
/// Nested private namespace of the Bayeux/datatools module library (serialization)
namespace serialization {

/** \brief Data structure to ensure that a given executable is linked
 *        against the datatools_bio DLL with new gcc 4.6 linking strategy
 *        even if no explicit datatools serialization code is explicitely
 *        invoked from the executable.
 */
class dynamic_link_guard {
 public:
  dynamic_link_guard();
  ~dynamic_link_guard();

  static dynamic_link_guard& instance();
 private:
  static bool _g_devel_;
};

} // end namespace serialization
} // end namespace detail
} // end namespace datatools

#endif // DATATOOLS_DETAIL_BIO_LINK_GUARD_H_

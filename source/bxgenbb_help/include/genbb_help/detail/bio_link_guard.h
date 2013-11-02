// -*- mode: c++; -*-
/* bio_link_guard.h */
#ifndef GENBB_HELP_DETAIL_BIO_LINK_GUARD_H_
#define GENBB_HELP_DETAIL_BIO_LINK_GUARD_H_ 1

namespace genbb {
/// Nested private namespace of the Bayeux/geomtools module library (detail)
namespace detail {
/// Nested private namespace of the Bayeux/geomtools module library (serialization)
namespace serialization {

/** \brief Data structure to ensure that a given executable is linked
 *        against the genbb_help_bio DLL with new gcc 4.6 linking strategy
 *        even if no explicit genbb serialization code is explicitely
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
} // end namespace genbb

#endif // GENBB_HELP_DETAIL_BIO_LINK_GUARD_H_

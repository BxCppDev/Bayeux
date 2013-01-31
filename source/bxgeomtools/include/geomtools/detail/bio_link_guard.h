// -*- mode: c++; -*-
/* bio_link_guard.h */
#ifndef GEOMTOOLS_DETAIL_BIO_LINK_GUARD_H_
#define GEOMTOOLS_DETAIL_BIO_LINK_GUARD_H_

namespace geomtools {
/// Non-public material
namespace detail {
/// Non-public material dedicated to serialization   
namespace serialization {

/** \brief Data structure to ensure that a given executable is linked
 *        against the geomtools_bio DLL with new gcc 4.6 linking strategy 
 *        even if no explicit geomtools serialization code is explicitely 
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
} // end namespace geomtools

#endif // GEOMTOOLS_DETAIL_BIO_LINK_GUARD_H_


// -*- mode: c++; -*-
/* datatools::serialization::link_guard.h */
#ifndef DATATOOLS_SERIALIZATION_LINK_GUARD_H_
#define DATATOOLS_SERIALIZATION_LINK_GUARD_H_

namespace datatools {
namespace serialization {

class dynamic_link_guard {
 public:
  dynamic_link_guard();
  ~dynamic_link_guard();

  static dynamic_link_guard& instance();
 private:
  static bool _g_devel_;
};

} // end namespace serialization
} // end namespace datatools

#endif // DATATOOLS_SERIALIZATION_LINK_GUARD_H_


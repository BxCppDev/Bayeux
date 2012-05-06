// -*- mode: c++; -*-
/* genbb::serialization::link_guard.h */

#ifndef __genbb__serialization__link_guard_h
#define __genbb__serialization__link_guard_h 1

namespace genbb {
  namespace serialization {
    class dynamic_link_guard
    {
    public:
      dynamic_link_guard ();
      ~dynamic_link_guard ();
      static dynamic_link_guard & instance ();
    private:
      static bool _g_devel_;
    };
  } // end namespace serialization
} // end namespace genbb

#endif // __genbb__serialization__link_guard_h

/* end of genbb::serialization::link_guard.h */

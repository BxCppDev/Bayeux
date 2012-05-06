// -*- mode: c++; -*-
/* geomtools::serialization::link_guard.h */

#ifndef __geomtools__serialization__link_guard_h
#define __geomtools__serialization__link_guard_h 1

namespace geomtools {
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
} // end namespace geomtools

#endif // __geomtools__serialization__link_guard_h

/* end of geomtools::serialization::link_guard.h */

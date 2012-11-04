// -*- mode: c++; -*-
/* mygsl::serialization::link_guard.h */

#ifndef __mygsl__serialization__link_guard_h
#define __mygsl__serialization__link_guard_h 1

namespace mygsl {
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
} // end namespace snemo

#endif // __mygsl__serialization__link_guard_h

/* end of mygsl::serialization::link_guard.h */


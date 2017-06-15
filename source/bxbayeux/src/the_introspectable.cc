// the_introspectable.cc

// Ourselves:
#include <bayeux/bayeux_config.h>

// Introspection system relies on static init, and cannot
// guarantee this across translation units, so
// merge in sources here until a better solution is
// found.
// NB: this still relies on guarantee of static init order
// within single translation unit. This means:
// 1: Inclusions must be in dependency order least to most
//    dependent.

#include "../../bxdatatools/src/the_introspectable.cc"

#if BAYEUX_WITH_GEOMTOOLS == 1
#include "../../bxgeomtools/src/the_introspectable.cc"
#endif // BAYEUX_WITH_GEOMTOOLS == 1

#if BAYEUX_WITH_GENBB == 1
#include "../../bxgenbb_help/src/the_introspectable.cc"
#endif // BAYEUX_WITH_GENBB == 1

//#if BAYEUX_WITH_MCTOOLS == 1
//#include "../../bxmctools/src/the_introspectable.cc"
//#endif // BAYEUX_WITH_MCTOOLS == 1

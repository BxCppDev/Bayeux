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
// 2:
#include "../../bxdatatools/src/the_introspectable.cc"
#include "../../bxgeomtools/src/the_introspectable.cc"
#include "../../bxgenbb_help/src/the_introspectable.cc"
//#include "../../bxmctools/src/the_introspectable.cc"

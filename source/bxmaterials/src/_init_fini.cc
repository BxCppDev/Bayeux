/* materials/_init_fini.cc
 */
// Ourselves

// This package:
#include <materials/materials_config.h>
#include <materials/materials.h>

__attribute__((constructor))
static void materials_initializer()
{
  materials::initialize(0,0);
  return;
}

__attribute__((destructor))
static void materials_finalizer()
{
  materials::terminate();
  return;
}

// end of materials/_init_fini.cc

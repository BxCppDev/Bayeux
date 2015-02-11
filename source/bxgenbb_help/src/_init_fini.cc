// genbb_help/_init_fini.cc

// This package:
#include <genbb_help/genbb_help_config.h>
#include <genbb_help/genbb_help.h>

__attribute__((constructor))
static void genbb_help_initializer()
{
  genbb::initialize(0,0);
  return;
}

__attribute__((destructor))
static void genbb_help_finalizer()
{
  genbb::terminate();
  return;
}

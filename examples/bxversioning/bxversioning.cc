
// Standard Library
#include <iostream>

// Third Party
// - Bayeux
#include <bayeux/version.h>

int main(int argc, const char *argv[])
{
  std::cout << "[bxversionining] Bayeux version string = " 
            << bayeux::version::get_version() << std::endl;
  return 0;
}


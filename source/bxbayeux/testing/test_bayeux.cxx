// Standard library:
#include <iostream>

// This project:
#include <bayeux/bayeux_config.h>
#include <bayeux/bayeux.h>
#include <bayeux/reloc.h>
#include <bayeux/version.h>

int main(int /*argc_*/, char** /*argv_*/)
{
  BAYEUX_INIT();
  std::clog << "Hello, Bayeux !\n";
  std::clog << "  Version            : " << bayeux::version::get_version() << std::endl;
  std::clog << "  Prefix directory   : " << bayeux::get_prefix_dir() << std::endl;
  std::clog << "  Binary directory   : " << bayeux::get_binary_dir() << std::endl;
  std::clog << "  Library directory  : " << bayeux::get_library_dir() << std::endl;
  std::clog << "  Data directory     : " << bayeux::get_data_dir() << std::endl;
  std::clog << "  Resource directory : " << bayeux::get_resource_dir() << std::endl;
  std::clog << "  Geant4 module      : " << BAYEUX_WITH_GEANT4_MODULE << std::endl;
  std::clog << "  MCNP module        : " << BAYEUX_WITH_MCNP_MODULE << std::endl;
  std::clog << "  LaHague module     : " << BAYEUX_WITH_LAHAGUE << std::endl;
  std::clog << "  Qt GUI             : " << BAYEUX_WITH_QT_GUI << std::endl;

  BAYEUX_FINI();
  return 0;
}

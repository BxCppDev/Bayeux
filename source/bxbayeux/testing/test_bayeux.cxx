// Standard library:
#include <iostream>

// This project:
#include <bayeux/bayeux.h>
#include <bayeux/reloc.h>
#include <bayeux/version.h>

int main(void)
{
  BAYEUX_INIT();
  std::clog << "Hello, Bayeux !\n";
  std::clog << "  Version            : " << bayeux::version::get_version() << std::endl;
  std::clog << "  Prefix directory   : " << bayeux::get_prefix_dir() << std::endl;
  std::clog << "  Binary directory   : " << bayeux::get_binary_dir() << std::endl;
  std::clog << "  Library directory  : " << bayeux::get_library_dir() << std::endl;
  std::clog << "  Data directory     : " << bayeux::get_data_dir() << std::endl;
  std::clog << "  Resource directory : " << bayeux::get_resource_dir() << std::endl;
  std::clog << "  Documentation dir. : " << bayeux::get_doc_dir() << std::endl;
  // std::clog << "  Example directory  : " << bayeux::get_example_dir() << std::endl;
  BAYEUX_FINI();
  return 0;
}

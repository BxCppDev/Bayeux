// Standard library:
#include <iostream>
#include <cstdlib>

// This project:
#include <bayeux/bayeux_config.h>
#include <bayeux/bayeux.h>
#include <bayeux/reloc.h>
#include <bayeux/version.h>
#include <bayeux/resource.h>
#include <bayeux/datatools/kernel.h>
#include <bayeux/datatools/urn_query_service.h>
#include <bayeux/datatools/utils.h>

int main( int /*argc_*/, char** /*argv_*/ )
{
  std::clog << "[info] You may activate debug logging from your shell with: \n\n"
            << "\texport BAYEUX_SYS_LOGGING=\"trace\" \n"
            << "\texport DATATOOLS_SYS_LOGGING=\"trace\" \n"
            << '\n'
            << "[info] Back to mute with: \n\n"
            << "\texport BAYEUX_SYS_LOGGING=\"fatal\" \n"
            << "\texport DATATOOLS_SYS_LOGGING=\"fatal\" \n"
            << std::endl;
  int result = EXIT_SUCCESS;
  ::bayeux::initialize(0, nullptr, 0);

  try {

    {
      std::clog << "Print resource paths: " << std::endl;
      // datatools::kernel & dtk = ::datatools::kernel::instance();
      // dtk.tree_dump(std::clog, "Bayeux/datatools' kernel: ", "[info] ");
      std::clog << "  Version            : '" << bayeux::version::get_version() << "'" << std::endl;
      std::clog << "  Prefix directory   : '" << bayeux::get_prefix_dir() << "'" << std::endl;
      std::clog << "  Binary directory   : '" << bayeux::get_binary_dir() << "'" << std::endl;
      std::clog << "  Library directory  : '" << bayeux::get_library_dir() << "'" << std::endl;
      std::clog << "  Data directory     : '" << bayeux::get_data_dir() << "'" << std::endl;
      std::clog << "  Resource base directory     : \n"
                << "                       '" << bayeux::get_resource_dir() << "'" << std::endl;
      std::clog << "  Bayeux's resources directory : \n"
                << "                       '" << bayeux::get_bayeux_resource_dir() << "'" << std::endl;
      std::clog << std::endl;
    }

    {
      std::clog << "Print status of optional modules: " << std::endl;
      std::clog << "  Geant4 module      : " << BAYEUX_WITH_GEANT4_MODULE << std::endl;
      std::clog << "  MCNP module        : " << BAYEUX_WITH_MCNP_MODULE << std::endl;
      std::clog << "  LaHague module     : " << BAYEUX_WITH_LAHAGUE << std::endl;
      std::clog << "  Qt GUI             : " << BAYEUX_WITH_QT_GUI << std::endl;
      std::clog << std::endl;
    }

    {
      std::clog << "Resolve URN to path: " << std::endl;
      std::string logo_urn = "urn:bayeux:logo:small";
      std::string logo_category;
      std::string logo_path;
      std::string logo_mime;
      datatools::kernel & dtk = ::datatools::kernel::instance();
      if (dtk.has_urn_query()) {
        const datatools::urn_query_service & kUrnQuery = dtk.get_urn_query();
        if (kUrnQuery.resolve_urn_to_path(logo_urn, logo_category, logo_mime, logo_path)) {
          std::clog << "[info] URN => path resolver success : " << std::endl;
          std::clog << "  URN                : '" << logo_urn << "'" << std::endl;
          std::clog << "  Category           : '" << logo_category << "'" << std::endl;
          std::clog << "  MIME type          : '" << logo_mime << "'" << std::endl;
          std::clog << "  Resolved path      : '" << logo_path << "'" << std::endl;
        } else {
          std::clog << "[error] Cannot resolve path from URN = '" << logo_urn << "'" << std::endl;
        }
      } else {
        std::clog << "[error] Missing kernel's URN query service." << std::endl;
      }
      std::clog << std::endl;
    }

    {
      std::clog << "Fetch path from URN: " << std::endl;
      std::string logo_path = "urn:bayeux:logo:large";
      std::clog << "  Logo path (raw)      : '" << logo_path << "'" << std::endl;
      datatools::fetch_path_with_env(logo_path);
      std::clog << "  Logo path (resolved) : '" << logo_path << "'" << std::endl;
      std::clog << std::endl;
    }

    {
      std::clog << "Print kernel's URN query service: " << std::endl;
      datatools::kernel & dtk = ::datatools::kernel::instance();
      if (dtk.has_urn_query()) {
        dtk.get_urn_query().tree_dump(std::clog, "URN query:");
      }
      std::vector<std::string> paths;
      if (dtk.get_urn_query().find_urn_to_path(paths, "(.*)", "(.*)", "(.*)", "(.*)")) {
        std::clog << "[info] List of current registered URNs => path records:" << std::endl;
        for (const std::string & p : paths) {
          std::clog << "Path = '" << p << "'" << std::endl;
        }
      } else {
        std::cerr << "[error] " << "No path was found." << std::endl;
      }

      std::clog << std::endl;
    }

  } catch (std::exception & error) {
    std::cerr << "[fatal] " << error.what() << std::endl;
    result = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "[fatal] " << "An unexpected error occured!" << std::endl;
    result = EXIT_FAILURE;
  }

  bayeux::terminate();
  return ( result < 0xff ? result : 0xff );
}

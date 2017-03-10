// // Catch
// #define CATCH_CONFIG_RUNNER
// #include "catch.hpp"

// This project:
#include <bayeux/bayeux_config.h>
#include <bayeux/bayeux.h>
#include <bayeux/reloc.h>
#include <bayeux/version.h>
#include <bayeux/resource.h>
#include <bayeux/datatools/kernel.h>
#include <bayeux/datatools/urn_query_service.h>
#include <bayeux/datatools/utils.h>

// NB: These are true tests yet!
// TEST_CASE("Bayeux is setup", "[bayeux]") {
int main( int /*argc_*/, char** /*argv_*/ )
{
  int result = 0;
  ::bayeux::initialize();

  // int result = Catch::Session().run( argc_, argv_ );

  //SECTION("resource paths are valid") {
    {
    datatools::kernel & dtk = ::datatools::kernel::instance();
    dtk.tree_dump(std::clog, "Bayeux/datatools' kernel: ", "[info] ");
    std::clog << "  Version            : " << bayeux::version::get_version() << std::endl;
    std::clog << "  Prefix directory   : " << bayeux::get_prefix_dir() << std::endl;
    std::clog << "  Binary directory   : " << bayeux::get_binary_dir() << std::endl;
    std::clog << "  Library directory  : " << bayeux::get_library_dir() << std::endl;
    std::clog << "  Data directory     : " << bayeux::get_data_dir() << std::endl;
    std::clog << "  Resource base directory     : \n"
              << "                       " << bayeux::get_resource_dir() << std::endl;
    std::clog << "  Bayeux's resource directory : \n"
              << "                       " << bayeux::get_bayeux_resource_dir() << std::endl;
  }

  //SECTION("configuration is valid") {
  {
    std::clog << "  Geant4 module      : " << BAYEUX_WITH_GEANT4_MODULE << std::endl;
    std::clog << "  MCNP module        : " << BAYEUX_WITH_MCNP_MODULE << std::endl;
    std::clog << "  LaHague module     : " << BAYEUX_WITH_LAHAGUE << std::endl;
    std::clog << "  Qt GUI             : " << BAYEUX_WITH_QT_GUI << std::endl;
  }

  // SECTION("resolve URN")
  {
    std::string logo_urn = "urn:bayeux:logo:small";
    std::string logo_category;
    std::string logo_path;
    std::string logo_mime;
    datatools::kernel & dtk = ::datatools::kernel::instance();
    if (dtk.has_urn_query()) {
      const datatools::urn_query_service & kUrnQuery = dtk.get_urn_query();
      if (kUrnQuery.resolve_urn_as_path(logo_urn, logo_category, logo_path, logo_mime)) {
        std::clog << "  Logo               : " << logo_path << std::endl;
        std::clog << "  Category           : " << logo_category << std::endl;
        std::clog << "  MIME type          : " << logo_mime << std::endl;
      } else {
        std::clog << "[error] Cannot resolve URN = " << logo_urn << std::endl;
      }
    } else {
      std::clog << "[error] Missing kernel's URNquery service." << std::endl;
    }
  }

  // SECTION("fetch path from URN") {
  {
    std::string logo_path = "urn:bayeux:logo:large";
    datatools::fetch_path_with_env(logo_path);
    std::clog << "  Logo               : " << logo_path << std::endl;
  }

  bayeux::terminate();
  return ( result < 0xff ? result : 0xff );
}

// test_urn_to_path_resolver_service.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// This project:
#include <datatools/urn_to_path_resolver_service.h>

void trps0();

int main(int /* argc_ */, char ** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'datatools::urn_to_path_resolver_service' class."
              << std::endl;
    trps0();
    std::clog << "The end." << std::endl;
  } catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

void trps0()
{
  datatools::urn_to_path_resolver_service resourceResolverService;
  resourceResolverService.set_name("Urn2Resource");
  resourceResolverService.set_logging_priority(datatools::logger::PRIO_DEBUG);
  resourceResolverService.add_known_category("configuration");
  resourceResolverService.add_known_category("image");
  resourceResolverService.add_known_category("data");
  resourceResolverService.set_allow_overloading(true);

  resourceResolverService.add_entry("urn:bayeux:logo:small",
                                    "image",
                                    "@bayeux:images/logo/logo_bayeux_66x55_transparent.png",
                                    "image/png");

  resourceResolverService.add_entry("urn:bayeux:logo:medium",
                                    "image",
                                    "@bayeux:images/logo/logo_bayeux_240x200_transparent.png",
                                    "image/png");

  resourceResolverService.add_entry("urn:bayeux:logo:large",
                                    "image",
                                    "@bayeux:images/logo/logo_bayeux_300x250_transparent.png",
                                    "image/png");

  resourceResolverService.add_entry("urn:snemo:demonstrator:geometry:4.0",
                                    "configuration",
                                    "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf",
                                    "text/bayeux.datatools.properties");

  resourceResolverService.add_entry("urn:snemo:demonstrator:simulation:geant4:2.1",
                                    "configuration",
                                    "@falaise:config/snemo/demonstrator/simulation/geant4_control/2.1/manager.conf",
                                    "text/bayeux.datatools.multi_properties");

  resourceResolverService.add_entry("urn:snemo:demonstrator:simulation:vertexes:4.1",
                                    "configuration",
                                    "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/manager.conf",
                                    "text/bayeux.datatools.properties");

  resourceResolverService.add_entry("urn:snemo:demonstrator:simulation:decays:1.2",
                                    "configuration",
                                    "@falaise:config/snemo/demonstrator/simulation/primary_events/1.2/manager.conf",
                                    "text/bayeux.datatools.properties");

  // Overloading:
  resourceResolverService.add_entry("urn:snemo:demonstrator:simulation:decays:1.2",
                                    "configuration",
                                    "@falaise:config/snemo/demonstrator/simulation/primary_events/1.2.1/manager.conf",
                                    "text/bayeux.datatools.properties");

  resourceResolverService.add_entry("urn:snemo:demonstrator:reconstruction:mcudd:1.0",
                                    "configuration",
                                    "@falaise:config/snemo/demonstrator/reconstruction/mcudd/1.0.3/pipeline.conf",
                                    "text/bayeux.datatools.multi_properties");

  resourceResolverService.add_entry("urn:snemo:demonstrator:data:mc:run:1.0",
                                    "data",
                                    "${SNEMO_DATA_DIR}/run/simu/1.0/data/truth_hits.data.gz",
                                    "application/bayeux.datatools.io.binary+gzip");


  resourceResolverService.initialize_simple();

  std::clog << "\nList of registered URN/resource entries: " << resourceResolverService.size() << "\n";
  resourceResolverService.print_lookup_table(std::clog);
  std::clog << std::endl;

  return;
}

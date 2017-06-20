// -*- mode: c++; -*-
// test_urn_query_service.cxx

// Ourselves:
#include <datatools/urn_query_service.h>

// Standard Library:
#include <fstream>

// Third party:
#include <boost/lexical_cast.hpp>
#include <bayeux/bayeux.h>

// This project:
#include <datatools/urn_db_service.h>
#include <datatools/urn_to_path_resolver_service.h>
#include <datatools/kernel.h>
#include <datatools/dependency_graph.h>

void test_urn_query_service_0();

int main (int /* argc_ */, char ** /*argv_*/)
{
  bayeux::initialize(0,nullptr,0);
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test of the 'datatools::urn_query_service' class..." << std::endl;

    test_urn_query_service_0();

  } catch (std::exception & x) {
    std::clog << "[fatal] " << x.what () << std::endl;
    error_code =  EXIT_FAILURE;
  } catch (...) {
    std::clog << "[fatal] " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  bayeux::terminate();
  return error_code;
}

void test_urn_query_service_0()
{
  std::clog << "[info] " << "test_urn_query_service_0..." << std::endl;

  datatools::urn_db_service urnDbService1;
  urnDbService1.set_name("urndb1");
  urnDbService1.set_display_name("UrnDb1");
  urnDbService1.set_terse_description("URN Database Service #1");

  // Populate service with URN infos:
  for (int i = 0; i < 3; i++) {
    urnDbService1.add("urn:datatools:testing:data:" + boost::lexical_cast<std::string>(i),
                      "data");
  }
  urnDbService1.initialize_simple();
  {
    std::string urn = "urn:datatools:testing:params:1.0";
    urnDbService1.add(urn, "configuration");
    urnDbService1.add_link(urn, "urn:datatools:testing:data:2", "dependency");
  }
  urnDbService1.lock();
  urnDbService1.tree_dump(std::clog, "urndb1:", "[info] ");

  datatools::urn_db_service urnDbService2;
  urnDbService2.set_name("urndb2");
  urnDbService2.set_display_name("UrnDb2");
  urnDbService2.set_terse_description("URN Database Service #2");
  urnDbService2.set_allow_mounted(true);
  urnDbService2.connect_db(urnDbService1);
  urnDbService2.initialize_simple();

  // Populate service with URN infos:
  std::string urn0 = "urn:datatools:testing:config:0";
  urnDbService2.add(urn0, "configuration");
  for (int i = 1; i < 3; i++) {
    std::string urn = "urn:datatools:testing:config:" + boost::lexical_cast<std::string>(i);
    urnDbService2.add(urn, "configuration");
    urnDbService2.add_link(urn, urn0, "dependency");
  }
  std::string urn3 = "urn:datatools:testing:config:3";
  urnDbService2.add(urn3, "configuration");
  urnDbService2.add_mounted("urndb1", "urn:datatools:testing:data:1");
  urnDbService2.add_mounted("urndb1", "urn:datatools:testing:params:1.0");
  urnDbService2.add_link(urn3, "urn:datatools:testing:config:1", "dependency");
  urnDbService2.add_link(urn3, "urn:datatools:testing:config:2", "dependency");
  urnDbService2.add_link("urn:datatools:testing:config:2", "urn:datatools:testing:data:1", "dependency");
  urnDbService2.add_link("urn:datatools:testing:config:3", "urn:datatools:testing:params:1.0", "dependency");
  urnDbService2.lock();
  urnDbService2.tree_dump(std::clog, "urndb2:", "[info] ");

  datatools::urn_to_path_resolver_service urnResolverService1;
  urnResolverService1.set_name("urnresolv1");
  urnResolverService1.add_known_category("configuration");
  urnResolverService1.add_known_category("data");
  urnResolverService1.add_known_category("image");

  // Populate service with URN paths:
  for (int i = 0; i < 10; i++) {
    urnResolverService1.add_entry("urn:datatools:testing:data:" + boost::lexical_cast<std::string>(i),
                                  "data",
                                  "@datatools:testing/config/data_" + boost::lexical_cast<std::string>(i) + ".data",
                                  "application/bayeux.datatools.data");
  }
  urnResolverService1.add_entry("urn:datatools:variants:models:basic:1.0:utils",
                                "configuration",
                                "@datatools:variants/models/basic/1.0/utils.def",
                                "application/bayeux.datatools.multi_properties");
  urnResolverService1.add_entry("urn:bayeux:logo:medium",
                                "image",
                                "@bayeux:images/logo/logo_bayeux_240x200_transparent.png",
                                "image/png");
  urnResolverService1.add_entry("urn:bayeux:logo:large",
                                "image",
                                "@bayeux:images/logo/logo_bayeux_300x250_transparent.png",
                                "image/png");
  urnResolverService1.add_entry("urn:bayeux:logo:small",
                                "image",
                                "@bayeux:images/logo/logo_bayeux_66x55_transparent.png",
                                "image/png");

  // Populate service with URN path:
  for (int i = 0; i < 3; i++) {
    urnResolverService1.add_entry("urn:datatools:testing:config:" + boost::lexical_cast<std::string>(i),
                                  "configuration",
                                  "@datatools:testing/config/parameters_" + boost::lexical_cast<std::string>(i) + ".conf",
                                  "text/bayeux.datatools.properties");
  }

  // datatools::urn_query_service & kUrnQuery = datatools::kernel::instance().get_urn_query();
  datatools::urn_query_service urnQuery;
  urnQuery.set_logging_priority(datatools::logger::PRIO_DEBUG);
  urnQuery.add_db(urnDbService1);
  urnQuery.add_db(urnDbService2);
  urnQuery.add_path_resolver(urnResolverService1);
  urnQuery.tree_dump(std::clog, urnQuery.get_name(), "[info] ");
  urnQuery.initialize_simple();
  {
    std::clog << "\nBuild the dependency graph:" << std::endl;
    const datatools::dependency_graph & dg = urnQuery.get_dependency_graph();
    std::ofstream fexp("test_urn_query_service.dot");
    uint32_t xgv_options = 0;
    xgv_options |= datatools::dependency_graph::XGV_WITH_VERTEX_CATEGORY;
    xgv_options |= datatools::dependency_graph::XGV_WITH_VERTEX_INDEX;
    // xgv_options |= datatools::dependency_graph::XGV_WITH_EDGE_TOPIC;
    dg.export_graphviz(fexp, xgv_options);
  }
  std::clog << std::endl;

  // Queries:
  std::clog << "Queries: \n" << std::endl;

  {
    std::string urn = "urn:datatools:testing:data:8";
    std::string category = "data";
    std::string mime = "";
    std::string path;
    if (urnQuery.resolve_urn_to_path(urn, category, mime, path)) {
      std::clog << "[info] URN='" << urn << "' has associated path '" << path << "' "
                << "with category '" << category << "' and MIME type '" << mime << "'"
                << std::endl;
    } else {
      std::clog << "[info] URN='" << urn << "' has NO associated path '" << path << "' "
                << "with category '" << category << "' and MIME type '" << mime << "'"
                << std::endl;
    }
    std::clog << std::endl;
  }

  {
    std::string urn = "urn:datatools:testing:data:9";
    std::string category = "(dat)(.*)";
    std::string mime = "(application/)(.*)(\\.data)";
    std::string path;
    if (urnQuery.resolve_urn_to_path(urn, category, mime, path)) {
      std::clog << "[info] URN='" << urn << "' has associated path '" << path << "' "
                << "with category '" << category << "' and MIME type '" << mime << "'"
                << std::endl;
    } else {
      std::clog << "[info] URN='" << urn << "' has NO associated path "
                << "with category '" << category << "' and MIME type '" << mime << "'"
                << std::endl;
    }
    std::clog << std::endl;
  }

  {
    std::string urn = "urn:datatools:testing:config:2";
    std::string category = "(conf)(.*)";
    std::string mime = "text/bayeux.datatools.properties";
    std::string path;
    if (urnQuery.resolve_urn_to_path(urn, category, mime, path)) {
      std::clog << "[info] URN='" << urn << "' has associated path '" << path << "' "
                << "with category '" << category << "' and MIME type '" << mime << "'"
                << std::endl;
    } else {
      std::clog << "[info] URN='" << urn << "' has NO associated path "
                << "with category '" << category << "' and MIME type '" << mime << "'"
                << std::endl;
    }
    std::clog << std::endl;
  }

  // Find:
  std::clog << "Finding: \n" << std::endl;
  {
    std::vector<std::string> matching_urns;
    std::string urn_db_regex = "(.*)";
    std::string urn_regex = "(.*)";
    std::string category_regex = "(.*)";
    if (urnQuery.find_urn_info(matching_urns, urn_db_regex, urn_regex, category_regex, true)) {
      for (auto urn : matching_urns) {
        std::clog << "[info] URN = '" << urn << "'" << std::endl;
      }
    } else {
      std::clog << "[info] Cannot find URNs from DB '" << urn_db_regex << "' "
                << "with pattern '" << urn_regex << "' and "
                << "category '" << category_regex << "'!"
                << std::endl;
    }
    std::clog << std::endl;
  }

  {
    std::vector<std::string> matching_urns;
    std::string urn_db_regex = "(.*)";
    std::string urn_regex = "(.*)2";
    std::string category_regex = "(.*)";
    if (urnQuery.find_urn_info(matching_urns, urn_db_regex, urn_regex, category_regex, true)) {
      for (auto urn : matching_urns) {
        std::clog << "[info] URN = '" << urn << "'" << std::endl;
      }
    } else {
      std::clog << "[info] Cannot find URNs from DB '" << urn_db_regex << "' "
                << "with pattern '" << urn_regex << "' and "
                << "category '" << category_regex << "'!"
                << std::endl;
    }
    std::clog << std::endl;
  }

  datatools::properties setup;
  setup.store("conf.path", "urn:bayeux:logo:large");

  if (setup.has_key("conf.path")) {
    std::string conf_path = setup.fetch_string("conf.path");
    std::clog << "[info] Configuration path = '" << conf_path << "'" << std::endl;
    datatools::fetch_path_with_env(conf_path);
    std::clog << "[info] Resolved path = '" << conf_path << "'" << std::endl;
  }

  std::clog << std::endl;
  return;
}

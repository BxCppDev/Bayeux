// -*- mode: c++; -*-
// test_dependency_graph.cxx

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

// Ourselves:
#include <datatools/dependency_graph.h>

void test_dependency_graph_0();
void test_dependency_graph_1();

int main (int /* argc_ */, char ** /*argv_*/)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test of the 'datatools::dependency_graph' class..." << std::endl;

    test_dependency_graph_0();
    test_dependency_graph_1();

  } catch (std::exception & x) {
    std::clog << "[fatal] " << x.what () << std::endl;
    error_code =  EXIT_FAILURE;
  } catch (...) {
    std::clog << "[fatal] " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

void test_dependency_graph_0()
{
  std::clog << "[info] " << "test_dependency_graph_0..." << std::endl;

  datatools::dependency_graph dg;
  dg.add_vertex("Bayeux", "library");

  dg.add_vertex("datatools", "module");
  //dg.add_out_edge("Bayeux", "datatools", "dependency");

  dg.add_vertex("brio", "module");
  //dg.add_out_edge("Bayeux", "brio", "dependency");
  dg.add_out_edge("brio", "datatools", "dependency");

  dg.add_vertex("cuts", "module");
  //dg.add_out_edge("Bayeux", "cuts", "dependency");
  dg.add_out_edge("cuts", "datatools", "dependency");

  dg.add_vertex("mygsl", "module");
  //dg.add_out_edge("Bayeux", "mygsl", "dependency");
  dg.add_out_edge("mygsl", "datatools", "dependency");

  dg.add_vertex("materials", "module");
  //dg.add_out_edge("Bayeux", "materials", "dependency");
  dg.add_out_edge("materials", "datatools", "dependency");

  dg.add_vertex("dpp", "module");
  //dg.add_out_edge("Bayeux", "dpp", "dependency");
  dg.add_out_edge("dpp", "cuts", "dependency");
  dg.add_out_edge("dpp", "mygsl", "dependency");
  dg.add_out_edge("dpp", "brio", "dependency");

  dg.add_vertex("geomtools", "module");
  //dg.add_out_edge("Bayeux", "geomtools", "dependency");
  dg.add_out_edge("geomtools", "mygsl", "dependency");
  dg.add_out_edge("geomtools", "materials", "dependency");

  dg.add_vertex("genbb_help", "module");
  //dg.add_out_edge("Bayeux", "genbb_help", "dependency");
  dg.add_out_edge("genbb_help", "mygsl", "dependency");
  dg.add_out_edge("genbb_help", "geomtools", "dependency");

  dg.add_vertex("genvtx", "module");
  //dg.add_out_edge("Bayeux", "genvtx", "dependency");
  dg.add_out_edge("genvtx", "mygsl", "dependency");
  dg.add_out_edge("genvtx", "geomtools", "dependency");

  dg.add_vertex("emfield", "module");
  //dg.add_out_edge("Bayeux", "emfield", "dependency");
  dg.add_out_edge("emfield", "geomtools", "dependency");

  dg.add_vertex("mctools", "module");
  //dg.add_out_edge("Bayeux", "mctools", "dependency");
  dg.add_out_edge("mctools", "genbb_help", "dependency");
  dg.add_out_edge("mctools", "genvtx", "dependency");
  dg.add_out_edge("mctools", "geomtools", "dependency");
  dg.add_out_edge("mctools", "emfield", "dependency");
  dg.add_out_edge("mctools", "dpp", "dependency");

  dg.add_vertex("lahague", "module");
  //dg.add_out_edge("Bayeux", "lahague", "dependency");
  dg.add_out_edge("lahague", "mctools", "dependency");

  dg.add_vertex("Bayeux_mctools_geant4", "library");
  dg.add_out_edge("Bayeux_mctools_geant4", "Bayeux", "dependency");

  dg.add_vertex("Bayeux_mctools_mcnp", "library");
  dg.add_out_edge("Bayeux_mctools_mcnp", "Bayeux", "dependency");

  std::ofstream fexp("test_dependency_graph_Bayeux.dot");
  uint32_t xgv_options = 0;
  xgv_options |= datatools::dependency_graph::XGV_WITH_VERTEX_CATEGORY;
  // xgv_options |= datatools::dependency_graph::XGV_WITH_EDGE_TOPIC;
  dg.export_graphviz(fexp, xgv_options);

  return;
}

void test_dependency_graph_1()
{
  std::clog << "[info] " << "test_dependency_graph_1..." << std::endl;

  datatools::dependency_graph dg;

  std::string boost_sw_urn = "urn:sw:boost:1.63.0";
  dg.add_vertex(boost_sw_urn, "library");

  std::string gsl_sw_urn = "urn:sw:gsl:2.3";
  dg.add_vertex(gsl_sw_urn, "library");

  std::string camp_sw_urn = "urn:sw:camp:0.8";
  dg.add_vertex(camp_sw_urn, "library");

  std::string root_sw_urn = "urn:sw:root:6.8.6";
  dg.add_vertex(root_sw_urn, "library");

  std::string geant4_sw_urn = "urn:sw:geant4:9.6";
  dg.add_vertex(geant4_sw_urn, "library");

  std::string clhep_sw_urn = "urn:sw:clhep:2.1.3.1";
  dg.add_vertex(clhep_sw_urn, "library");

  std::string xercesc_sw_urn = "urn:sw:xercesc:3.1.4";
  dg.add_vertex(xercesc_sw_urn, "library");

  std::string bayeux_sw_urn = "urn:sw:bayeux:3.0.0";
  dg.add_vertex(bayeux_sw_urn, "library");
  dg.add_out_edge(bayeux_sw_urn, boost_sw_urn, "dependency");
  dg.add_out_edge(bayeux_sw_urn, gsl_sw_urn, "dependency");
  dg.add_out_edge(bayeux_sw_urn, root_sw_urn, "dependency");
  dg.add_out_edge(root_sw_urn, gsl_sw_urn, "dependency");
  dg.add_out_edge(bayeux_sw_urn, clhep_sw_urn, "dependency");
  dg.add_out_edge(bayeux_sw_urn, camp_sw_urn, "dependency");
  dg.add_out_edge(bayeux_sw_urn, geant4_sw_urn, "dependency");
  dg.add_out_edge(geant4_sw_urn, clhep_sw_urn, "dependency");
  dg.add_out_edge(geant4_sw_urn, xercesc_sw_urn, "dependency");

  std::string falaise_sw_urn = "urn:sw:falaise:3.0.0";
  dg.add_vertex(falaise_sw_urn, "library");
  dg.add_out_edge(falaise_sw_urn, bayeux_sw_urn, "dependency");

  std::string bxmaterials_basic_urn = "urn:bxmaterials:materials:basic:1.0";
  dg.add_vertex(bxmaterials_basic_urn, "data");
  // dg.add_out_edge(bayeux_sw_urn, bxmaterials_basic_urn, "dependency");

  std::string sndemo_exp_urn = "urn:snemo:demonstrator";
  dg.add_vertex(sndemo_exp_urn, "experiment");

  // std::string bipo3_exp_urn = "urn:bipo3";
  // dg.add_vertex(bipo3_exp_urn,  "experiment");

  // std::string bipo3_expsetup_urn = "urn:bipo3:detector:setup:1.0";
  // dg.add_vertex(bipo3_expsetup_urn, "expsetup");
  // dg.add_out_edge(bipo3_expsetup_urn, bipo3_exp_urn, "dependency");
  // dg.add_out_edge(falaise_sw_urn, bipo3_expsetup_urn, "composition");

  std::string sndemo_expsetup_urn = "urn:snemo:demonstrator:setup:1.0";
  dg.add_vertex(sndemo_expsetup_urn, "expsetup");
  dg.add_out_edge(sndemo_expsetup_urn, sndemo_exp_urn, "dependency");
  // dg.add_out_edge(falaise_sw_urn, sndemo_expsetup_urn, "dependency");

  // std::string sndemo_expsetup_services_urn = "urn:snemo:demonstrator:setup:1.0:services";
  // dg.add_vertex(sndemo_expsetup_services_urn, "services");
  // dg.add_out_edge(sndemo_expsetup_urn, sndemo_expsetup_services_urn, "composition");

  // std::string sndemo_expsetup_variants_urn = "urn:snemo:demonstrator:setup:1.0:variants";
  // dg.add_vertex(sndemo_expsetup_variants_urn, "variants");
  // dg.add_out_edge(sndemo_expsetup_variants_urn, sndemo_exp_urn, "dependency");
  // dg.add_out_edge(sndemo_expsetup_urn, sndemo_expsetup_variants_urn, "composition");

  std::string sndemo_geometry_urn = "urn:snemo:demonstrator:geometry:4.0";
  dg.add_vertex(sndemo_geometry_urn, "configuration");
  dg.add_out_edge(sndemo_geometry_urn, sndemo_exp_urn, "dependency");
  dg.add_out_edge(sndemo_expsetup_urn, sndemo_geometry_urn, "dependency");
  dg.add_out_edge(sndemo_geometry_urn, bxmaterials_basic_urn, "dependency");

  // std::string sndemo_geometry_service_urn = "urn:snemo:demonstrator:geometry:4.0:service";
  // dg.add_vertex(sndemo_geometry_service_urn, "configuration");
  // dg.add_out_edge(sndemo_expsetup_services_urn, sndemo_geometry_service_urn, "dependency");
  // dg.add_out_edge(sndemo_geometry_service_urn, sndemo_geometry_urn, "composition");

  // std::string sndemo_geometry_variants_urn = "urn:snemo:demonstrator:geometry:4.0:variants";
  // dg.add_vertex(sndemo_geometry_variants_urn, "variants");
  // dg.add_out_edge(sndemo_geometry_variants_urn, sndemo_exp_urn, "dependency");
  // dg.add_out_edge(sndemo_geometry_urn, sndemo_geometry_variants_urn, "composition");

  std::string sndemo_vtx_setup_urn = "urn:snemo:demonstrator:simulation:vertexes:4.1";
  dg.add_vertex(sndemo_vtx_setup_urn, "configuration");
  dg.add_out_edge(sndemo_vtx_setup_urn, sndemo_geometry_urn, "dependency");

  std::string sndemo_decay_setup_urn = "urn:snemo:demonstrator:simulation:decays:1.2";
  dg.add_vertex(sndemo_decay_setup_urn, "configuration");
  // dg.add_out_edge(sndemo_decay_setup_urn, sndemo_geometry_urn, "dependency");

  std::string sndemo_sim_setup_urn = "urn:snemo:demonstrator:simulation:2.1";
  dg.add_vertex(sndemo_sim_setup_urn, "simsetup");
  dg.add_out_edge(sndemo_sim_setup_urn, sndemo_expsetup_urn, "dependency");
  dg.add_out_edge(sndemo_sim_setup_urn, sndemo_geometry_urn, "dependency");
  dg.add_out_edge(sndemo_sim_setup_urn, sndemo_decay_setup_urn, "dependency");
  dg.add_out_edge(sndemo_sim_setup_urn, sndemo_vtx_setup_urn, "dependency");
  // dg.add_out_edge(falaise_sw_urn, sndemo_sim_setup_urn, "dependency");

  // std::string sndemo_sim_setup_variants_urn = "urn:snemo:demonstrator:simulation:2.1:variants";
  // dg.add_vertex(sndemo_sim_setup_variants_urn, "variants");
  // dg.add_out_edge(sndemo_sim_setup_urn, sndemo_sim_setup_variants_urn, "composition");
  // dg.add_out_edge(sndemo_vtx_setup_urn, sndemo_sim_setup_variants_urn, "dependency");
  // dg.add_out_edge(sndemo_decay_setup_urn, sndemo_sim_setup_variants_urn, "dependency");

  // std::string sndemo_sim_setup_services_urn = "urn:snemo:demonstrator:simulation:2.1:services";
  // dg.add_vertex(sndemo_sim_setup_services_urn, "services");
  // dg.add_out_edge(sndemo_sim_setup_urn, sndemo_sim_setup_services_urn, "composition");
  // dg.add_out_edge(sndemo_sim_setup_services_urn, sndemo_geometry_urn, "dependency");
  // dg.add_out_edge(sndemo_sim_setup_services_urn, sndemo_sim_setup_variants_urn, "dependency");

  std::string sndemo_rec_setup_urn = "urn:snemo:demonstrator:reconstruction:1.0";
  dg.add_vertex(sndemo_rec_setup_urn, "recsetup");
  dg.add_out_edge(sndemo_rec_setup_urn, sndemo_expsetup_urn, "dependency");
  // dg.add_out_edge(falaise_sw_urn, sndemo_rec_setup_urn, "dependency");

  // std::string simsetup_urn = "urn:snemo:demonstrator:simu:2.21";
  // std::string decaysetup_urn = "urn:snemo:demonstrator:simu:decays:1.23";
  // std::string g4setup_urn    = "urn:snemo:demonstrator:simu:geant4:1.23";
  // std::string recsetup_urn = "urn:snemo:demonstrator:rec:1.17";




  // dg.add_vertex(sndemo_shapes_urn, "data");
  // dg.add_vertex(materials_urn, "data");
  // dg.add_vertex(geometry_urn, "configuration");
  // dg.add_vertex(device_urn, "configuration");
  // dg.add_vertex(expsetupserv_urn, "services");
  // dg.add_vertex(simsetup_urn, "configuration");
  // dg.add_vertex(vtxsetup_urn, "configuration");
  // dg.add_vertex(decaysetup_urn, "configuration");
  // dg.add_vertex(g4setup_urn, "configuration");
  // dg.add_vertex(recsetup_urn, "configuration");

  // dg.add_out_edge(geometry_urn, materials_urn, "dependency");
  // dg.add_out_edge(geometry_urn, sndemo_shapes_urn,    "dependency");
  // dg.add_out_edge(geometry_urn, sndemo_exp_urn,       "dependency");
  // dg.add_out_edge(sndemo_expsetup_urn, geometry_urn,  "dependency");
  // dg.add_out_edge(device_urn, sndemo_exp_urn,    "dependency");
  // dg.add_out_edge(sndemo_expsetup_urn, device_urn,    "dependency");
  // dg.add_out_edge(expsetupserv_urn, geometry_urn,  "dependency");
  // dg.add_out_edge(expsetupserv_urn, device_urn,  "dependency");
  // dg.add_out_edge(simsetup_urn, sndemo_expsetup_urn,  "dependency");
  // dg.add_out_edge(vtxsetup_urn, geometry_urn,  "dependency");
  // dg.add_out_edge(g4setup_urn, geometry_urn,  "dependency");
  // dg.add_out_edge(g4setup_urn, vtxsetup_urn,  "dependency");
  // dg.add_out_edge(g4setup_urn, decaysetup_urn,  "dependency");
  // dg.add_out_edge(simsetup_urn, g4setup_urn,  "dependency");
  // dg.add_out_edge(recsetup_urn, sndemo_expsetup_urn,  "dependency");
  // dg.add_out_edge(decaysetup_urn, vtxsetup_urn,  "dependency");

  if (dg.has_vertex(sndemo_expsetup_urn)) {
    std::clog << "[info] dependency graph has vertex with ID '" << sndemo_expsetup_urn << "'" << std::endl;
  } else {
    std::clog << "[info] dependency graph has no vertex with ID '" << sndemo_expsetup_urn << "'" << std::endl;
  }
  if (dg.has_vertex(bxmaterials_basic_urn)) {
    std::clog << "[info] dependency graph has vertex with ID '" << bxmaterials_basic_urn << "'" << std::endl;
  } else {
    std::clog << "[info] dependency graph has no vertex with ID '" << bxmaterials_basic_urn << "'" << std::endl;
  }

  std::ofstream fexp("test_dependency_graph.dot");
  uint32_t xgv_options = 0;
  xgv_options |= datatools::dependency_graph::XGV_WITH_VERTEX_CATEGORY;
  // xgv_options |= datatools::dependency_graph::XGV_WITH_EDGE_TOPIC;
  dg.export_graphviz(fexp, xgv_options);

  std::clog << std::endl;

  return;
}

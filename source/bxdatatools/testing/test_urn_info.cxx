// -*- mode: c++; -*-
// test_urn_info.cxx

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
#include <limits>
#include <set>

// Third party:
#include <boost/serialization/map.hpp>

// Ourselves:
#include <datatools/urn_info.h>
#include <datatools/io_factory.h>

// This project:
#include <datatools/version_id.h>

void test_urn_info_0();
void test_urn_info_1();

int main (int /* argc_ */, char ** /*argv_*/)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test of the 'datatools::urn_info' class..." << std::endl;

    test_urn_info_0();
    test_urn_info_1();

  } catch (std::exception & x) {
    std::clog << "[fatal] " << x.what () << std::endl;
    error_code =  EXIT_FAILURE;
  } catch (...) {
    std::clog << "[fatal] " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

void test_urn_info_0()
{
  std::clog << "[info] " << "test_urn_info_0..." << std::endl;
  bool debug = false;

  std::map<std::string, datatools::urn_info> urns;

  datatools::urn_info uiBxmatBasicIso;
  uiBxmatBasicIso.set_urn("urn:datatools:materials:setup:basics:1.0:isotopes");
  uiBxmatBasicIso.set_category("data");
  uiBxmatBasicIso.set_description("Bayeux/materials basic isotopes definitions version 1.0");
  uiBxmatBasicIso.lock();
  if (debug) uiBxmatBasicIso.tree_dump(std::cerr, "URN info: ", "[debug] ");
  if (debug) std::cerr << std::endl;
  urns[uiBxmatBasicIso.get_urn()] = uiBxmatBasicIso;

  datatools::urn_info uiBxmatBasicElem;
  uiBxmatBasicElem.set_urn("urn:bayeux:datatools:materials:setup:basics:1.0:elements");
  uiBxmatBasicElem.set_category("data");
  uiBxmatBasicElem.set_description("Bayeux/materials basic elements definitions version 1.0");
  uiBxmatBasicElem.add_component(uiBxmatBasicIso.get_urn(), "isotopes");
  uiBxmatBasicElem.lock();
  if (debug) uiBxmatBasicElem.tree_dump(std::cerr, "URN info: ", "[debug] ");
  if (debug) std::cerr << std::endl;
  urns[uiBxmatBasicElem.get_urn()] = uiBxmatBasicElem;

  datatools::urn_info uiBxmatBasicMat;
  uiBxmatBasicMat.set_urn("urn:bayeux:datatools:materials:setup:basics:1.0:materials");
  uiBxmatBasicMat.set_category("data");
  uiBxmatBasicMat.set_description("Bayeux/materials basic materials definitions version 1.0");
  uiBxmatBasicMat.add_component(uiBxmatBasicElem.get_urn(), "elements");
  uiBxmatBasicMat.lock();
  if (debug) uiBxmatBasicMat.tree_dump(std::cerr, "URN info: ", "[debug] ");
  if (debug) std::cerr << std::endl;
  urns[uiBxmatBasicMat.get_urn()] = uiBxmatBasicMat;

  datatools::urn_info uiBxmatBasicMatAliases;
  uiBxmatBasicMatAliases.set_urn("urn:bayeux:datatools:materials:setup:basics:1.0:aliases");
  uiBxmatBasicMatAliases.set_category("data");
  uiBxmatBasicMatAliases.set_description("Bayeux/materials basic material aliases definitions version 1.0");
  uiBxmatBasicMatAliases.add_component(uiBxmatBasicMat.get_urn(), "materials");
  uiBxmatBasicMatAliases.lock();
  if (debug) uiBxmatBasicMatAliases.tree_dump(std::cerr, "URN info: ", "[debug] ");
  if (debug) std::cerr << std::endl;
  urns[uiBxmatBasicMatAliases.get_urn()] = uiBxmatBasicMatAliases;

  datatools::urn_info uiSnGeoMat;
  uiSnGeoMat.set_urn("urn:snemo:demonstrator:geometry:setup:4.0:materials");
  uiSnGeoMat.set_category("configuration");
  uiSnGeoMat.set_description("Materials Plugin of the SuperNEMO Demonstrator Geometry Model version 4.0");
  uiSnGeoMat.add_component("urn:bayeux:datatools:materials:setup:basics:1.0:isotopes",  "data");
  uiSnGeoMat.add_component("urn:bayeux:datatools:materials:setup:basics:1.0:elements",  "data");
  uiSnGeoMat.add_component("urn:bayeux:datatools:materials:setup:basics:1.0:materials", "data");
  uiSnGeoMat.add_component("urn:bayeux:datatools:materials:setup:basics:1.0:aliases",   "data");
  uiSnGeoMat.lock();
  if (debug) uiSnGeoMat.tree_dump(std::cerr, "URN info: ", "[debug] ");
  if (debug) std::cerr << std::endl;
  urns[uiSnGeoMat.get_urn()] = uiSnGeoMat;

  datatools::urn_info uiSnGeo;
  uiSnGeo.set_urn("urn:snemo:demonstrator:geometry:setup:4.0");
  uiSnGeo.set_category("configuration");
  uiSnGeo.set_description("SuperNEMO Demonstrator Geometry Model version 4.0");
  uiSnGeo.add_component("urn:snemo:demonstrator:geometry:setup:4.0:materials", "materials");
  uiSnGeo.lock();
  if (debug) uiSnGeo.tree_dump(std::cerr, "URN info: ", "[debug] ");
  if (debug) std::cerr << std::endl;
  urns[uiSnGeo.get_urn()] = uiSnGeo;

  datatools::urn_info uiSnSimuVtx;
  uiSnSimuVtx.set_urn("urn:snemo:demonstrator:simulation:vertexes:setup:4.1");
  uiSnSimuVtx.set_category("configuration");
  uiSnSimuVtx.set_description("SuperNEMO Demonstrator Simulation Vertex Generation version 4.1");
  uiSnSimuVtx.lock();
  if (debug) uiSnSimuVtx.tree_dump(std::cerr, "URN info: ", "[debug] ");
  if (debug) std::cerr << std::endl;
  urns[uiSnSimuVtx.get_urn()] = uiSnSimuVtx;

  datatools::urn_info uiSnSimuDecays;
  uiSnSimuDecays.set_urn("urn:snemo:demonstrator:simulation:decays:setup:2.1");
  uiSnSimuDecays.set_category("configuration");
  uiSnSimuDecays.set_description("SuperNEMO Demonstrator Simulation Decays Generation version 2.1");
  uiSnSimuDecays.lock();
  if (debug) uiSnSimuDecays.tree_dump(std::cerr, "URN info: ", "[debug] ");
  if (debug) std::cerr << std::endl;
  urns[uiSnSimuDecays.get_urn()] = uiSnSimuDecays;

  datatools::urn_info uiSnPhysEm;
  uiSnPhysEm.set_urn("urn:snemo:physics:1.3.1-beta:electromagnetic");
  uiSnPhysEm.set_category("data");
  uiSnPhysEm.set_description("Electromagnetic interaction physics model version 1.3.1-beta");
  uiSnPhysEm.lock();
  if (debug) uiSnPhysEm.tree_dump(std::cerr, "URN info: ", "[debug] ");
  if (debug) std::cerr << std::endl;
  urns[uiSnPhysEm.get_urn()] = uiSnPhysEm;

  datatools::urn_info uiSnPhysOpt;
  uiSnPhysOpt.set_urn("urn:snemo:physics:2.4.78:optics");
  uiSnPhysOpt.set_category("data");
  uiSnPhysOpt.set_description("Optics model version 2.4.78");
  uiSnPhysOpt.lock();
  if (debug) uiSnPhysOpt.tree_dump(std::cerr, "URN info: ", "[debug] ");
  urns[uiSnPhysOpt.get_urn()] = uiSnPhysOpt;
  if (debug) std::cerr << std::endl;

  datatools::urn_info uiSnSimu;
  uiSnSimu.set_urn("urn:snemo:demonstrator:simulation:geant4:setup:2.1");
  uiSnSimu.set_category("configuration");
  uiSnSimu.set_description("SuperNEMO Demonstrator Geant4 Simulation Setup version 2.1");
  uiSnSimu.add_component("urn:dummy:truc:1.0");
  uiSnSimu.add_component("urn:foo:chose:2.3-alpha", "foo");
  uiSnSimu.add_component("urn:bar:machin:pro", "foo");
  uiSnSimu.add_component("urn:snemo:demonstrator:geometry:setup:4.0", "geometry");
  if (debug) uiSnSimu.tree_dump(std::cerr, "URN info: ", "[debug] ");
  uiSnSimu.remove_component("urn:dummy:truc:1.0");
  uiSnSimu.add_component("urn:snemo:demonstrator:simulation:vertexes:setup:4.1", "vertexes");
  uiSnSimu.add_component("urn:snemo:demonstrator:simulation:decays:setup:2.1", "decays");
  uiSnSimu.add_component("urn:snemo:physics:1.3.1-beta:electromagnetic", "physics");
  uiSnSimu.add_component("urn:snemo:physics:2.4.78:optics", "physics");
  uiSnSimu.add_component("urn:baz:zaz");
  uiSnSimu.add_component("urn:people:johndoe", "shifter");
  uiSnSimu.lock();
  if (debug) uiSnSimu.tree_dump(std::cerr, "URN info: ", "[debug] ");
  if (debug) std::cerr << std::endl;
  urns[uiSnSimu.get_urn()] = uiSnSimu;

  for (const auto& ui : urns) {
    ui.second.tree_dump(std::clog, "URN info: " + ui.second.get_urn(), "[info] ");
    std::clog << std::endl;
  }
  std::clog << std::endl;

  {
    datatools::data_writer w("urn_info_dict.xml");
    w.store("std::map<std::string,datatools::urn_info>", urns);
  }
  urns.clear();
  {
    datatools::data_reader r("urn_info_dict.xml");
    r.load("std::map<std::string,datatools::urn_info>", urns);
  }
  for (const auto& ui : urns) {
    ui.second.tree_dump(std::clog, "Loaded URN info: " + ui.second.get_urn(), "[info] ");
    std::clog << std::endl;
  }
  std::clog << std::endl;

  return;
}


void test_urn_info_1()
{
  std::clog << "[info] " << "test_urn_info_1..." << std::endl;
  // bool debug = false;

  /* Scheme:
   *              ...........................
   *              : S   "collaboration"   P :
   *              :  o<----------------->o  :<----- S/P collaboration's perimeter
   *              :..^^.....................:       (S+P may be seen as an unique entity)
   *                 | \ "usage"
   *                 |  \
   *         "usage" |   \ D
   *                 |    o
   *                 |   /
   *                 |  / "dependency"
   *                 | /
   *               A |v
   *        .........o.........
   *        :       / \       :
   * "composition" /   \ "composition"
   *        :     /     \     :
   *        :    v       v    :
   *        :   o         o   :<----- A's perimeter
   *        :  B           C  :
   *        :.................:
   *
   */

  // Declare nodes (BGL: vertices)
  datatools::urn_info A("urn:datatools:testing:A", "bayeux::object",  "A item");
  datatools::urn_info B("urn:datatools:testing:B", "bayeux::object",  "B item");
  datatools::urn_info C("urn:datatools:testing:C", "bayeux::object",  "C item");
  datatools::urn_info D("urn:datatools:testing:D", "bayeux::object",  "D item");
  datatools::urn_info S("urn:datatools:testing:S", "bayeux::service", "S item");
  datatools::urn_info P("urn:datatools:testing:P", "bayeux::object",  "P item");

  // Describe components (BGL: edges)
  // B is a child/component of A (thus A cannot exist without B)
  A.add_component(B.get_urn(), "composition");
  // C is a child/component of A (thus A cannot exist without B)
  A.add_component(C.get_urn(), "composition");
  // A uses the S service (thus A cannot work without S)
  A.add_component(S.get_urn(), "usage");
  // D depends on A (D is the depender, A is the dependee, thus D cannot exists without A)
  D.add_component(A.get_urn(), "dependency");
  // D uses the S service (thus D cannot work without S)
  D.add_component(S.get_urn(), "usage");

  // Bi-directional components:
  // P collaborates with S service (thus P may not work without S)
  P.add_component(S.get_urn(), "collaboration");
  // S collaborates with P service (thus S may not work without P)
  S.add_component(P.get_urn(), "collaboration");

  std::map<std::string, datatools::urn_info> urns;
  urns[A.get_urn()] = A;
  urns[B.get_urn()] = B;
  urns[C.get_urn()] = C;
  urns[D.get_urn()] = D;
  urns[S.get_urn()] = S;
  urns[P.get_urn()] = P;
  for (auto & ui : urns) {
    ui.second.lock();
  }

  for (const auto & ui : urns) {
    ui.second.tree_dump(std::clog, "URN info: " + ui.second.get_urn(), "[info] ");
    std::clog << std::endl;
  }
  std::clog << std::endl;
  return;
}

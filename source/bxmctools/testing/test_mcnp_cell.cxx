// test_mcnp_surface.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/clhep_units.h>
// #include <datatools/utils.h>

// This project:
#include <mctools/mcnp/geometry/cell.h>
#include <mctools/mcnp/geometry/surface.h>
#include <mctools/mcnp/geometry/surface_type.h>
#include <mctools/mcnp/format.h>
#include <mctools/mcnp/units.h>

void test0();

int main(int /* argc_ */, char ** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;

  try {

    std::clog << "Test program for class 'mctools::mcnp::geometry::cell'!" << std::endl;

    test0();

  }
  catch(std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch(...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return(error_code);
}

void test0()
{
  namespace m2g = mctools::mcnp::geometry;

  std::map<int, m2g::surface> surfaces;
  for (int surf_id = 1; surf_id <= 6; surf_id++) {
    {
      m2g::surface a_surface;
      std::ostringstream surf_name;
      surf_name << "BoxBoundary" << surf_id;
      a_surface.set_name(surf_name.str());
      a_surface.set_id(surf_id);
      switch(surf_id) {
      case 1 :
        a_surface.add_parameter("D", 10.0 * CLHEP::cm, "length");
        a_surface.set_type(m2g::SURF_PLANE_X);
        break;
      case 2 :
        a_surface.add_parameter("D", 50.0 * CLHEP::cm, "length");
        a_surface.set_type(m2g::SURF_PLANE_X);
        break;
      case 3 :
        a_surface.add_parameter("D", +5.0 * CLHEP::cm, "length");
        a_surface.set_type(m2g::SURF_PLANE_Y);
        break;
      case 4 :
        a_surface.add_parameter("D", 10.0 * CLHEP::cm, "length");
        a_surface.set_type(m2g::SURF_PLANE_Y);
        break;
      case 5 :
        a_surface.add_parameter("D", -15.0 * CLHEP::cm, "length");
        a_surface.set_type(m2g::SURF_PLANE_Z);
        break;
      case 6 :
        a_surface.add_parameter("D", 15.0 * CLHEP::cm, "length");
        a_surface.set_type(m2g::SURF_PLANE_Z);
        break;
      }
      surfaces[surf_id] = a_surface;
    }
  }

  {
    int surf_id = 7;
    m2g::surface a_surface;
    std::ostringstream surf_name;
    surf_name << "Sphere" << surf_id;
    a_surface.set_name(surf_name.str());
    a_surface.set_id(surf_id);
    a_surface.set_type(m2g::SURF_SPHERE_O);
    a_surface.add_parameter("R", 20.0 * CLHEP::cm, "length");
    surfaces[surf_id] = a_surface;
  }

  std::clog << "List of surfaces : " << std::endl;
  for (std::map<int, m2g::surface>::const_iterator isurf = surfaces.begin();
       isurf != surfaces.end();
       isurf++) {
    isurf->second.print_card(std::clog, mctools::mcnp::format::CARD_COMMENT);
  }
  std::clog << std::endl;
  std::clog << "Surfaces done." << std::endl;

  std::clog << std::endl;
  std::clog << "A complex shape:" << std::endl;
  {
    m2g::cell a_cell;
    a_cell.set_name("AntiBoxSphereUnion");
    a_cell.set_id(1);
    a_cell.set_material_id(3);
    a_cell.set_material_density(3.25 * mctools::mcnp::units::density_unit());
    m2g::cell::surface_node_type & top_surface_node = a_cell.grab_surface_node();
    top_surface_node.set_branch(m2g::cell::SURF_OP_COMPLEMENT);
    m2g::cell::surface_node_type & comp = top_surface_node.add_branch(m2g::cell::SURF_OP_UNION);
    comp.set_branch(m2g::cell::SURF_OP_UNION);
    comp.add_leaf(7, m2g::cell::SURF_IE_EXTERIOR);
    {
      m2g::cell::surface_node_type & br_inter_node =
        comp.add_branch(m2g::cell::SURF_OP_INTERSECTION);
      br_inter_node.set_branch(m2g::cell::SURF_OP_INTERSECTION);
      br_inter_node.add_leaf(1, m2g::cell::SURF_IE_EXTERIOR);
      br_inter_node.add_leaf(2, m2g::cell::SURF_IE_EXTERIOR);
      br_inter_node.add_leaf(3, m2g::cell::SURF_IE_EXTERIOR);
      br_inter_node.add_leaf(4, m2g::cell::SURF_IE_EXTERIOR);
      br_inter_node.add_leaf(5, m2g::cell::SURF_IE_EXTERIOR);
      br_inter_node.add_leaf(6, m2g::cell::SURF_IE_EXTERIOR);
    }

    top_surface_node.tree_dump(std::clog, 0);
    std::clog << "Card:" << std::endl;
    a_cell.print_card(std::clog, mctools::mcnp::format::CARD_COMMENT);
  }

  std::clog << std::endl;
  std::clog << "A box:" << std::endl;
  {
    m2g::cell a_cell2;
    a_cell2.set_name("Box");
    a_cell2.set_id(2);
    a_cell2.set_material_id(8);
    a_cell2.set_material_density(0.17 * mctools::mcnp::units::density_unit());
    m2g::cell::surface_node_type & top_surface_node = a_cell2.grab_surface_node();
    top_surface_node.set_branch(m2g::cell::SURF_OP_INTERSECTION);
    top_surface_node.add_leaf(1, m2g::cell::SURF_IE_EXTERIOR);
    top_surface_node.add_leaf(2, m2g::cell::SURF_IE_EXTERIOR);
    top_surface_node.add_leaf(3, m2g::cell::SURF_IE_EXTERIOR);
    top_surface_node.add_leaf(4, m2g::cell::SURF_IE_EXTERIOR);
    top_surface_node.add_leaf(5, m2g::cell::SURF_IE_EXTERIOR);
    top_surface_node.add_leaf(6, m2g::cell::SURF_IE_EXTERIOR);
    top_surface_node.tree_dump(std::clog, 0);
    std::clog << "Card:" << std::endl;
    a_cell2.print_card(std::clog, mctools::mcnp::format::CARD_COMMENT);
  }
  return;
}

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

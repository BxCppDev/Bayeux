// test_mcnp_material.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <fstream>

// Third party:
// // - Bayeux/mygsl:
// #include <mygsl/rng.h>

// This project:
#include <mctools/mcnp/material/material.h>
#include <mctools/mcnp/material/material_store.h>
#include <mctools/mcnp/format.h>

int main(int /* argc_ */, char ** /* argv_ */)
{
  namespace m3 = mctools::mcnp::material;
  int error_code = EXIT_SUCCESS;
  try {

    std::clog << "Test program for class 'mctools::mcnp::material'!" << std::endl;

    {
      m3::material air;
      air.set_name("air");
      air.set_id(1);
      air.add_nuclide(8, 16, 0.2, m3::nuclide_entry::FRACTION_NATOMS);
      air.add_nuclide(7, 14, 0.8, m3::nuclide_entry::FRACTION_NATOMS, true);
      air.print_card(std::clog);

      m3::material water;
      water.set_name("water");
      water.set_id(2);
      water.add_nuclide(8, 16, 0.889, m3::nuclide_entry::FRACTION_MASS);
      water.add_nuclide(1,  1, 0.111, m3::nuclide_entry::FRACTION_MASS, true);
      water.print_card(std::clog);

      m3::material water2;
      water2.set_name("water2");
      water2.set_id(3);
      water2.add_nuclide(8, 16, 1, m3::nuclide_entry::FRACTION_NATOMS);
      water2.add_nuclide(1,  1, 2, m3::nuclide_entry::FRACTION_NATOMS, true);
      water2.print_card(std::clog);

      m3::material nat_germanium;
      nat_germanium.set_name("nat_germanium");
      nat_germanium.set_id(4);
      nat_germanium.make_natural_element(32);
      nat_germanium.print_card(std::clog);
      std::clog << std::endl;

    }

    {
      m3::material_store mat_store;

      m3::material & air = mat_store.add_material(1);
      air.set_name("air");
      air.add_nuclide(8, 16, 0.2, m3::nuclide_entry::FRACTION_NATOMS);
      air.add_nuclide(7, 14, 0.8, m3::nuclide_entry::FRACTION_NATOMS, true);

      m3::material & water = mat_store.add_material(2);
      water.set_name("water");
      water.add_nuclide(8, 16, 0.889, m3::nuclide_entry::FRACTION_MASS);
      water.add_nuclide(1,  1, 0.111, m3::nuclide_entry::FRACTION_MASS, true);

      m3::material & water2 = mat_store.add_material(3);
      water2.set_name("water2");
      water2.add_nuclide(8, 16, 1, m3::nuclide_entry::FRACTION_NATOMS);
      water2.add_nuclide(1,  1, 2, m3::nuclide_entry::FRACTION_NATOMS, true);

      m3::material & nat_germanium = mat_store.add_material(4);
      nat_germanium.set_name("nat_germanium");
      nat_germanium.make_natural_element(32);

      mat_store.print_card(std::clog);
      std::clog << std::endl;

    }

    {
      materials::manager mat_mgr;
      std::string mat_mgr_setup_file = "${MATERIALS_RESOURCE_DIR}/../testing/config/test_manager.conf";
      datatools::fetch_path_with_env(mat_mgr_setup_file);
      datatools::properties mat_mgr_setup;
      datatools::read_config(mat_mgr_setup_file, mat_mgr_setup);
      mat_mgr.initialize(mat_mgr_setup);
      mat_mgr.tree_dump(std::clog, "Material manager: ");

      m3::material_store mat_store;
      mat_store.build_from_bxmaterials(mat_mgr);

      std::ofstream mat_cards("test_mcnp_materials.inp");
      mctools::mcnp::format::add_comment_line(mat_cards, "Generated from Bayeux/mctools/ MCNP module:");
      mat_store.print_card(mat_cards, mctools::mcnp::format::CARD_COMMENT);
      mat_cards << std::endl;

    }

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

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

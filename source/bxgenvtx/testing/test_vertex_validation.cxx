// test_vertex_validation.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <iostream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/temporary_files.h>
#include <datatools/utils.h>
// - Bayeux/cuts:
#include <cuts/random_cut.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>
// - Bayeux/geomtools:
#include <geomtools/logical_volume.h>
#include <geomtools/physical_volume.h>
#include <geomtools/box.h>
#include <geomtools/cylinder.h>
#include <geomtools/tube.h>
#include <geomtools/placement.h>
#include <geomtools/display_data.h>
#include <geomtools/blur_spot.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_draw.h>
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

// This project:
#include <genvtx/vertex_validation.h>
#include <genvtx/box_vg.h>


void test1()
{
  std::clog << "\nRunning test1..." << std::endl;
  datatools::properties VV_config;
  VV_config.store("logging.priority", "trace");
  VV_config.store("maximum_number_of_tries", 300);

  genvtx::vertex_validation VV;

  datatools::properties RC_config;
  RC_config.store("seed", 12345);
  RC_config.store("accept_probability", 0.5);
  cuts::random_cut RC;
  RC.set_name("random");
  RC.initialize_standalone(RC_config);
  VV.set_validator(RC);
  VV.initialize(VV_config);
  VV.tree_dump(std::clog, "Vertex validation: ");

  for (int i = 0; i < 100; i++) {
    int ntries = 0;
    do {
      geomtools::vector_3d vtx(0.0, 0.0, 0.0);
      ntries++;
      // Minimal geometry context: the candidate vertex (in some local coordinate system)
      VV.grab_geometry_context().set_local_candidate_vertex(vtx);
      genvtx::vertex_validation::validate_status_type status = VV.validate();
      if (status == genvtx::vertex_validation::VS_ACCEPTED) {
        std::cout << vtx << std::endl;
        break;
      } else if (status == genvtx::vertex_validation::VS_MAXTRIES) {
        throw std::logic_error("Reached maximum number of tries!");
      } else {
        // Re-try !
      }
    } while (true);
  }
  std::cout << std::endl << std::endl;

  std::clog << "Total number of accepted vertexes : "
            << RC.get_number_of_accepted_entries() << std::endl;
  std::clog << "Total number of rejected vertexes : "
            << RC.get_number_of_rejected_entries() << std::endl;
  return;
}

void test2(bool draw_)
{
  std::clog << "\nRunning test2..." << std::endl;

  bool trace = false;

  geomtools::box b(10.0 * CLHEP::mm, 6.0 * CLHEP::mm, 1.0 * CLHEP::mm);
  b.lock();
  geomtools::logical_volume BoxLV;
  BoxLV.set_name("BoxLV");
  BoxLV.set_shape(b);
  BoxLV.set_material_ref("Copper");
  BoxLV.grab_parameters().store("visibility.color", "green");

  geomtools::cylinder       cyl_hole(0.75 * CLHEP::mm, 1.0 * CLHEP::mm);
  cyl_hole.lock();
  geomtools::logical_volume HoleLV;
  HoleLV.set_name("HoleLV");
  HoleLV.set_shape(cyl_hole);
  HoleLV.set_material_ref("Vacuum");
  HoleLV.grab_parameters().store("visibility.color", "orange");

  geomtools::box       box_hole(1.75 * CLHEP::mm, 2.0 * CLHEP::mm, 1.0 * CLHEP::mm);
  box_hole.lock();
  geomtools::logical_volume HoleLV2;
  HoleLV2.set_name("HoleLV2");
  HoleLV2.set_shape(box_hole);
  HoleLV2.set_material_ref("Air");
  HoleLV2.grab_parameters().store("visibility.color", "magenta");

  geomtools::physical_volume hole0_PV;
  geomtools::placement       hole0_PLCT;
  hole0_PV.set_logical(HoleLV);
  hole0_PV.set_placement(hole0_PLCT);
  BoxLV.add_physical(hole0_PV, "hole0");

  geomtools::physical_volume hole1_PV;
  geomtools::placement       hole1_PLCT;
  hole1_PLCT.set_translation(2.0 * CLHEP::mm, 0.0 * CLHEP::mm, 0.0 * CLHEP::mm);
  hole1_PV.set_logical(HoleLV);
  hole1_PV.set_placement(hole1_PLCT);
  BoxLV.add_physical(hole1_PV, "hole1");

  geomtools::physical_volume hole2_PV;
  geomtools::placement       hole2_PLCT;
  hole2_PLCT.set_translation(-3.5 * CLHEP::mm, -1.5 * CLHEP::mm, 0.0 * CLHEP::mm);
  hole2_PV.set_logical(HoleLV2);
  hole2_PV.set_placement(hole2_PLCT);
  BoxLV.add_physical(hole2_PV, "hole2");
  BoxLV.tree_dump(std::clog, "BoxLV: ");

  datatools::properties BVG_config;
  BVG_config.store_string("mode", "bulk");
  BVG_config.store_string("length_unit", "mm");
  BVG_config.store_real("skin_thickness", 0.25 * CLHEP::mm);
  BVG_config.set_explicit_unit("skin_thickness");
  BVG_config.store_real("skin_skip", 0.0 * CLHEP::mm);
  BVG_config.set_explicit_unit("skin_skip");
  BVG_config.store_boolean("validation.activation", true);
  if (trace) BVG_config.store_string("validation.logging.priority", "trace");
  BVG_config.store_integer("validation.maximum_number_of_tries", 200);
  BVG_config.store_string("validation.validator_id", "genvtx::not_in_daughters_vertex_validator");
  if (trace) BVG_config.store_string("validation.validator_params.logging.priority", "trace");
  BVG_config.store_string("validation.validator_params.cut.name", "no_daughter");
  BVG_config.store_string("validation.validator_params.cut.description", "Shoot in the box but the daughter volumes");
  BVG_config.store_real("validation.validator_params.tolerance", 0.1 * CLHEP::mm);
  BVG_config.set_explicit_unit("validation.validator_params.tolerance");
  BVG_config.store_real("validation.validator_params.daughter_tolerance", 0.1 * CLHEP::mm);
  BVG_config.set_explicit_unit("validation.validator_params.daughter_tolerance");

  genvtx::box_vg BVG;
  if (trace) BVG.set_logging_priority(datatools::logger::PRIO_TRACE);
  BVG.set_mode(genvtx::box_vg::MODE_BULK);
  BVG.set_logical(BoxLV);
  BVG.set_vertex_validation_support(true);
  BVG.initialize_standalone(BVG_config);
  BVG.tree_dump(std::clog, "BVG (no daughters): ");

  genvtx::box_vg BVG2;
  if (trace) BVG2.set_logging_priority(datatools::logger::PRIO_TRACE);
  BVG2.set_mode(genvtx::box_vg::MODE_BULK);
  BVG2.set_logical(BoxLV);
  BVG2.set_vertex_validation_support(true);
  BVG_config.store_boolean("validation.validator_params.reversed", true);
  BVG2.initialize_standalone(BVG_config);
  BVG2.tree_dump(std::clog, "BVG2 (only daughters): ");

  std::string       rng_id = "mt19937";
  unsigned long int rng_seed = 12345;
  mygsl::rng random(rng_id, rng_seed);

  // A display data object :
  geomtools::display_data DD;
  geomtools::blur_spot vtx_spot(geomtools::blur_spot::DIMENSION_THREE, 0.01 * CLHEP::mm);
  double vertex_dim = 0.05 * CLHEP::mm;
  vtx_spot.set_errors(vertex_dim, vertex_dim, vertex_dim);

  datatools::temp_file tmp_file;
  tmp_file.set_remove_at_destroy(false);
  tmp_file.create("/tmp", "test_vertex_validation_");

  int nvertexes = 5000;
  for (int i = 0; i < nvertexes; i++) {
    // Shoot the vertex:
    geomtools::vector_3d vtx;
    BVG.shoot_vertex(random, vtx);
    // std::cout << vtx << std::endl;
    geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), vtx, 4.0);
    if (draw_) {
      // Store the vertex in the  display data object:
      std::ostringstream vtx_name_ss;
      vtx_name_ss << "vtx_" << i;
      geomtools::display_data::display_item & vtx_spot_DI
        = DD.add_static_item(vtx_name_ss.str(), "vertex", "blue");
      geomtools::placement plcmt;
      plcmt.set_translation(vtx);
      vtx_spot.generate_wires(vtx_spot_DI.paths, plcmt);
    }
  }
  int nvertexes2 = 500;
  for (int i = 0; i < nvertexes2; i++) {
    // Shoot the vertex:
    geomtools::vector_3d vtx;
    BVG2.shoot_vertex(random, vtx);
    geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), vtx, 4.0);
    if (draw_) {
      // Store the vertex in the  display data object:
      std::ostringstream vtx_name_ss;
      vtx_name_ss << "vtx2_" << i;
      geomtools::display_data::display_item & vtx_spot_DI
        = DD.add_static_item(vtx_name_ss.str(), "vertex", "cyan");
      geomtools::placement plcmt;
      plcmt.set_translation(vtx);
      vtx_spot.generate_wires(vtx_spot_DI.paths, plcmt);
    }
  }
  BVG.tree_dump(std::clog, "BVG: ");
  BVG2.tree_dump(std::clog, "BVG2: ");

  if (trace) DD.tree_dump(std::clog, "Display data: ", "TRACE: ");
  if (draw_) {
    geomtools::placement identity;
    geomtools::gnuplot_drawer GD;
    geomtools::placement DD_placement;
    DD_placement.set_identity();
    GD.add_display_data(DD, DD_placement);
    GD.set_view (geomtools::gnuplot_drawer::view_3d());
    GD.set_mode (geomtools::gnuplot_drawer::mode_wired());
    GD.draw(BoxLV, identity, 4, "Vertex validation test (no daughter/only daughter) ", true);
  }

  return;
}

void test3(bool draw_)
{
  std::clog << "\nRunning test3..." << std::endl;

  bool trace = false;

  geomtools::box b(10.0 * CLHEP::mm, 6.0 * CLHEP::mm, 1.0 * CLHEP::mm);
  b.lock();
  geomtools::logical_volume BoxLV;
  BoxLV.set_name("BoxLV");
  BoxLV.set_shape(b);
  BoxLV.set_material_ref("Copper");
  BoxLV.grab_parameters().store("visibility.color", "green");

  geomtools::cylinder       cyl_hole(0.75 * CLHEP::mm, 1.0 * CLHEP::mm);
  cyl_hole.lock();
  geomtools::logical_volume HoleLV;
  HoleLV.set_name("HoleLV");
  HoleLV.set_shape(cyl_hole);
  HoleLV.set_material_ref("Vacuum");
  HoleLV.grab_parameters().store("visibility.color", "orange");

  geomtools::cylinder       cyl_block(0.5 * CLHEP::mm, 1.0 * CLHEP::mm);
  cyl_block.lock();
  geomtools::logical_volume BlockLV;
  BlockLV.set_name("BlockLV");
  BlockLV.set_shape(cyl_block);
  BlockLV.set_material_ref("Lead");
  BlockLV.grab_parameters().store("visibility.color", "red");

  geomtools::box rod(0.75 * CLHEP::mm, 0.75 * CLHEP::mm, 1.0 * CLHEP::mm);
  rod.lock();
  geomtools::logical_volume RodLV;
  RodLV.set_name("PipeLV");
  RodLV.set_shape(rod);
  RodLV.set_material_ref("Water");
  RodLV.grab_parameters().store("visibility.color", "cyan");
  geomtools::physical_volume rod_PV;
  geomtools::placement       rod_PLCT;
  rod_PV.set_logical(RodLV);
  rod_PV.set_placement(rod_PLCT);

  geomtools::cylinder       tube_pipe(0.80 * CLHEP::mm, 1.0 * CLHEP::mm);
  tube_pipe.lock();
  geomtools::logical_volume PipeLV;
  PipeLV.set_name("PipeLV");
  PipeLV.set_shape(tube_pipe);
  PipeLV.set_material_ref("Iron");
  PipeLV.add_physical(rod_PV, "rod");
  PipeLV.grab_parameters().store("visibility.color", "yellow");

  geomtools::physical_volume pipe_PV;
  pipe_PV.lock();
  geomtools::placement       pipe_PLCT;
  pipe_PLCT.set_translation(-0.1 * CLHEP::mm, 0.5 * CLHEP::mm, 0.0 * CLHEP::mm);
  pipe_PV.set_logical(PipeLV);
  pipe_PV.set_placement(pipe_PLCT);

  geomtools::box       box_hole(2.75 * CLHEP::mm, 3.5 * CLHEP::mm, 1.0 * CLHEP::mm);
  box_hole.lock();
  geomtools::logical_volume HoleLV2;
  HoleLV2.set_name("HoleLV2");
  HoleLV2.set_shape(box_hole);
  HoleLV2.set_material_ref("Air");
  HoleLV2.add_physical(pipe_PV, "pipe");
  HoleLV2.grab_parameters().store("visibility.color", "magenta");

  geomtools::physical_volume hole0_PV;
  geomtools::placement       hole0_PLCT;
  hole0_PLCT.set_translation(0.0 * CLHEP::mm, 1.5 * CLHEP::mm, 0.0 * CLHEP::mm);
  hole0_PV.set_logical(HoleLV);
  hole0_PV.set_placement(hole0_PLCT);
  BoxLV.add_physical(hole0_PV, "hole0");

  geomtools::physical_volume hole1_PV;
  geomtools::placement       hole1_PLCT;
  hole1_PLCT.set_translation(2.0 * CLHEP::mm, 0.0 * CLHEP::mm, 0.0 * CLHEP::mm);
  hole1_PV.set_logical(HoleLV);
  hole1_PV.set_placement(hole1_PLCT);
  BoxLV.add_physical(hole1_PV, "hole1");

  geomtools::physical_volume block_PV;
  geomtools::placement       block_PLCT;
  block_PLCT.set_translation(0.0 * CLHEP::mm, -1.5 * CLHEP::mm, 0.0 * CLHEP::mm);
  block_PV.set_logical(BlockLV);
  block_PV.set_placement(block_PLCT);
  BoxLV.add_physical(block_PV, "block");

  geomtools::physical_volume hole2_PV;
  geomtools::placement       hole2_PLCT;
  hole2_PLCT.set_translation(-2.5 * CLHEP::mm, -0.5 * CLHEP::mm, 0.0 * CLHEP::mm);
  hole2_PV.set_logical(HoleLV2);
  hole2_PV.set_placement(hole2_PLCT);
  BoxLV.add_physical(hole2_PV, "hole2");
  BoxLV.tree_dump(std::clog, "BoxLV: ");

  datatools::properties BVG_config;
  BVG_config.store_string("mode", "bulk");
  BVG_config.store_string("length_unit", "mm");
  BVG_config.store_real("skin_thickness", 0.25 * CLHEP::mm);
  BVG_config.set_explicit_unit("skin_thickness");
  BVG_config.store_real("skin_skip", 0.0 * CLHEP::mm);
  BVG_config.set_explicit_unit("skin_skip");
  BVG_config.store_boolean("validation.activation", true);
  if (trace) BVG_config.store_string("validation.logging.priority", "trace");
  BVG_config.store_integer("validation.maximum_number_of_tries", 200);
  BVG_config.store_string("validation.validator_id", "genvtx::in_materials_vertex_validator");

  if (trace) BVG_config.store_string("validation.validator_params.logging.priority", "trace");
  // BVG_config.store_string("validation.validator_params.logging.priority", "trace");
  BVG_config.store_string("validation.validator_params.cut.name", "in_metal");
  BVG_config.store_string("validation.validator_params.cut.description",
                          "Shoot from within the box bulk but only in metallic volumes");
  std::vector<std::string> materials;
  materials.push_back("Copper");
  materials.push_back("Iron");
  materials.push_back("Lead");
  BVG_config.store("validation.validator_params.materials.ref", materials);
  BVG_config.store_integer("validation.validator_params.max_depth", 4);
  BVG_config.tree_dump(std::clog, "BVG : ");

  genvtx::box_vg BVG;
  if (trace) BVG.set_logging_priority(datatools::logger::PRIO_TRACE);
  BVG.set_mode(genvtx::box_vg::MODE_BULK);
  BVG.set_logical(BoxLV);
  BVG.set_vertex_validation_support(true);
  BVG.initialize_standalone(BVG_config);
  BVG.tree_dump(std::clog, "BVG (in metal): ");

  genvtx::box_vg BVG2;
  if (trace) BVG2.set_logging_priority(datatools::logger::PRIO_TRACE);
  BVG2.set_mode(genvtx::box_vg::MODE_BULK);
  BVG2.set_logical(BoxLV);
  BVG2.set_vertex_validation_support(true);
  BVG_config.store_boolean("validation.validator_params.reversed", true);
  BVG2.initialize_standalone(BVG_config);
  BVG2.tree_dump(std::clog, "BVG2 (only daughters): ");

  std::string       rng_id = "mt19937";
  unsigned long int rng_seed = 12345;
  mygsl::rng        random(rng_id, rng_seed);

  // A display data object :
  geomtools::display_data DD;
  geomtools::blur_spot vtx_spot(geomtools::blur_spot::DIMENSION_THREE, 0.01 * CLHEP::mm);
  double vertex_dim = 0.05 * CLHEP::mm;
  vtx_spot.set_errors(vertex_dim, vertex_dim, vertex_dim);

  datatools::temp_file tmp_file;
  tmp_file.set_remove_at_destroy(false);
  tmp_file.create("/tmp", "test_vertex_validation_");

  int nvertexes = 5000;
  for (int i = 0; i < nvertexes; i++) {
    // Shoot the vertex:
    geomtools::vector_3d vtx;
    BVG.shoot_vertex(random, vtx);
    // std::cout << vtx << std::endl;
    geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), vtx, 4.0);
    if (draw_) {
      // Store the vertex in the  display data object:
      std::ostringstream vtx_name_ss;
      vtx_name_ss << "vtx_" << i;
      geomtools::display_data::display_item & vtx_spot_DI
        = DD.add_static_item(vtx_name_ss.str(), "vertex", "blue");
      geomtools::placement plcmt;
      plcmt.set_translation(vtx);
      vtx_spot.generate_wires(vtx_spot_DI.paths, plcmt);
    }
  }
  BVG.tree_dump(std::clog, "BVG: ");
  int nvertexes2 = 500;
  for (int i = 0; i < nvertexes2; i++) {
    // Shoot the vertex:
    geomtools::vector_3d vtx;
    BVG2.shoot_vertex(random, vtx);
    geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), vtx, 4.0);
    if (draw_) {
      // Store the vertex in the  display data object:
      std::ostringstream vtx_name_ss;
      vtx_name_ss << "vtx2_" << i;
      geomtools::display_data::display_item & vtx_spot_DI
        = DD.add_static_item(vtx_name_ss.str(), "vertex", "cyan");
      geomtools::placement plcmt;
      plcmt.set_translation(vtx);
      vtx_spot.generate_wires(vtx_spot_DI.paths, plcmt);
    }
  }
  BVG2.tree_dump(std::clog, "BVG2: ");

  if (trace) DD.tree_dump(std::clog, "Display data: ", "TRACE: ");
  if (draw_) {
    geomtools::placement identity;
    geomtools::gnuplot_drawer GD;
    geomtools::placement DD_placement;
    DD_placement.set_identity();
    GD.add_display_data(DD, DD_placement);
    GD.set_view (geomtools::gnuplot_drawer::view_3d());
    GD.set_mode (geomtools::gnuplot_drawer::mode_wired());
    GD.draw(BoxLV, identity, 4, "Vertex validation test (in metals/not in metals) ", true);
  }

  return;
}

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'genvtx::vertex_validation' program library." << std::endl;

    bool draw = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if (option == "--draw") {
          draw = true;
        } else {
          std::clog << "warning: ignoring option '" << option << "'!" << std::endl;
        }
      } else  {
        std::string argument = token;
        {
          std::clog << "warning: ignoring argument '" << argument << "'!" << std::endl;
        }
      }
      iarg++;
    }


    test1();

    test2(draw);

    test3(draw);

    std::clog << "The end." << std::endl;
  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

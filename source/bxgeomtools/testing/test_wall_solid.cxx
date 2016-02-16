// test_wall_solid.cxx

// Ourselves:
#include <geomtools/wall_solid.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/temporary_files.h>
#include <datatools/utils.h>

// This project:
#include <geomtools/geomtools_config.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/tessellation.h>
#include <geomtools/gdml_writer.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    // bool interactive = false;
    bool side = true;
    bool top = true;
    bool bottom = true;
    bool bulk = false;
    bool do_identity = false;
    bool do_locate = true;
    bool draw = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      // if (arg == "-r" || arg == "--interactive") interactive = true;
      if (arg == "-I" || arg == "--identity") do_identity = true;
      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-B" || arg == "--bulk") bulk = true;
      if (arg == "-l" || arg == "--locate") do_locate = false;
      if (arg == "-s" || arg == "--no-side") side = false;
      if (arg == "-b" || arg == "--no-bottom") bottom = false;
      if (arg == "-t" || arg == "--no-top") top = false;

      iarg++;
    }

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_wall_solid_");

    geomtools::wall_solid my_wall;
    my_wall.set_z(5.0);
    geomtools::simple_polygon & base = my_wall.grab_base();
    base.set_build_mode(geomtools::simple_polygon::BUILD_MODE_BY_VERTICES);
    base.add_vertex(0.0, 1.0);
    base.add_vertex(1.0, 3.0);
    base.add_vertex(3.0, 3.0);
    base.add_vertex(4.0, 1.0);
    base.add_vertex(2.5, 1.0);
    base.add_vertex(2.5,-2.0);
    base.add_vertex(0.0,-2.0);
    base.initialize_simple();
    my_wall.tree_dump(std::clog, "my_wall");

    geomtools::tessellated_solid my_wall_ts;
    my_wall.convert_to_tessellated(my_wall_ts);
    my_wall_ts.tree_dump(std::clog, "my_wall_ts");


    geomtools::gdml_writer writer;
    writer.add_wall("wall", my_wall, "mm");
    writer.save_file("test_wall_solid.gdml");

    std::clog << "Volume   = " << my_wall.get_volume() / CLHEP::m3 << " m3" << std::endl;

    std::clog << "Side surface = "
              << my_wall.get_surface(geomtools::wall_solid::FACE_SIDE) / CLHEP::mm2
              << " mm2" << std::endl;

    std::clog << "Top surface = "
              << my_wall.get_surface(geomtools::wall_solid::FACE_TOP) / CLHEP::mm2
              << " mm2" << std::endl;

    std::clog << "Bottom surface = " << my_wall.get_surface(geomtools::wall_solid::FACE_BOTTOM) / CLHEP::mm2
              << " mm2" << std::endl;

    std::clog << "Full surface = "
              << my_wall.get_surface(geomtools::wall_solid::FACE_ALL) / CLHEP::mm2
              << " mm2" << std::endl;

    my_wall.tree_dump(std::clog, "Wall_Solid: ", "INFO: ");

    geomtools::placement wall_placement(0.2, 0.15, 0.1, M_PI / 7.0, M_PI / 6.0, 0.0);
    if (do_identity) {
      wall_placement.set_identity();
    }
    geomtools::placement wall_placement_demo(-6, 0, 0, 0, 0, 0);

    {
      // Draw wall :
      tmp_file.out() << "# wall_solid (index 0):" << std::endl;
      geomtools::wires_type wall_wires;
      my_wall.generate_wires(wall_wires,
                             wall_placement,
                             geomtools::i_wires_3d_rendering::WR_NONE
                             // | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                             // | geomtools::i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY
                             // | geomtools::i_wires_3d_rendering::WR_BASE_BOUNDINGS
                             // | geomtools::wall_solid::WR_WALL_NO_BOTTOM_FACE
                             // | geomtools::wall_solid::WR_WALL_NO_TOP_FACE
                             // | geomtools::wall_solid::WR_WALL_NO_SIDE_FACE
                             );
      geomtools::gnuplot_draw::draw_wires(tmp_file.out(), wall_wires);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      int counts = 0;
      tmp_file.out() << "# locate (index 1)" << std::endl;
      size_t nshoots = 500000;
      for (int i = 0; i < (int) nshoots; i++) {
        double dim = 5. * CLHEP::mm;
        geomtools::vector_3d position (dim * ( -1 + 2 * drand48()),
                                       dim * ( -1 + 2 * drand48()),
                                       dim * ( -1 + 2 * drand48()));
        double skin = 0.20 * CLHEP::mm;
        geomtools::vector_3d position_c;
        wall_placement.mother_to_child(position, position_c);
        if (do_locate) {
          if (side && my_wall.is_on_surface(position_c,
                                            geomtools::wall_solid::FACE_SIDE, skin)) {
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 3.0);
            counts++;
          } else if (top && my_wall.is_on_surface(position_c,
                                                  geomtools::wall_solid::FACE_TOP, skin)) {
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 5.0);
            counts++;
          } else if (bottom && my_wall.is_on_surface(position_c,
                                                     geomtools::wall_solid::FACE_BOTTOM, skin)) {
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 6.0);
            counts++;
          } else if (bulk && my_wall.check_inside(position_c, skin)) {
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 2.0);
            counts++;
          }
        }
      }
      if (counts == 0) {
        tmp_file.out() << "0 0 0 -1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
    }

    {
      tmp_file.out() << "# wall (index 2): " << std::endl;
      geomtools::gnuplot_draw::draw_wall(tmp_file.out(),
                                         wall_placement_demo,
                                         my_wall,
                                         geomtools::i_wires_3d_rendering::WR_BASE_GRID
                                         | geomtools::i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY
                                         );
      tmp_file.out() << std::endl << std::endl;
    }

    {
      tmp_file.out() << "# wall (index 3): " << std::endl;
      geomtools::gnuplot_draw::draw_tessellated(tmp_file.out(),
                                                wall_placement_demo,
                                                my_wall_ts,
                                                geomtools::i_wires_3d_rendering::WR_NONE
                                                // | geomtools::tessellated_solid::WR_TESSELLA_ALL_SEGMENTS
                                                );
      tmp_file.out() << std::endl << std::endl;
    }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test geomtools::wall_solid' ");
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      g1.cmd("set xrange [-8:+6]");
      g1.cmd("set yrange [-6:+6]");
      g1.cmd("set zrange [-8:+8]");
      g1.cmd("set xyplane at -8");
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename() << "' index 0 title 'Wall solid' with lines ";
        plot_cmd << ", '' index 1 using 1:2:3:4 title 'Locate' with points pt 6 ps 0.3 linecolor variable ";
        plot_cmd << ", '' index 2 title 'Wall solid (bis)' with lines lt 2 ";
        plot_cmd << ", '' index 3 title 'Tessellated wall' with lines lt 4 lw 3 ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    }

  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

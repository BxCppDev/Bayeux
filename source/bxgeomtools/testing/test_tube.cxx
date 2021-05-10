// test_tube.cxx

// Ourselves:
#include <geomtools/tube.h>

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
#include <geomtools/tube.h>
#include <geomtools/cylinder.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    // bool interactive = false;
    bool debug = false;
    bool outer = true;
    bool inner = true;
    bool top = true;
    bool bottom = true;
    bool stop = true;
    bool start = true;
    bool bulk = false;
    bool do_identity = false;
    bool do_locate = true;
    bool do_sector = false;
    bool do_hole = false;
    bool do_envelope = false;
    bool draw = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      // if (arg == "-r" || arg == "--interactive") interactive = true;
      if (arg == "-I" || arg == "--identity") do_identity = true;
      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-B" || arg == "--bulk") bulk = true;
      if (arg == "-S" || arg == "--sector") do_sector = true;
      if (arg == "-H" || arg == "--hole") do_hole = true;
      if (arg == "-l" || arg == "--locate") do_locate = false;
      if (arg == "-i" || arg == "--no-inner") inner = false;
      if (arg == "-o" || arg == "--no-outer") outer = false;
      if (arg == "-b" || arg == "--no-bottom") bottom = false;
      if (arg == "-t" || arg == "--no-top") top = false;
      if (arg == "-s" || arg == "--no-start") start = false;
      if (arg == "-p" || arg == "--no-stop") stop = false;
      if (arg == "-e" || arg == "--envelope") do_envelope = true;

      iarg++;
    }

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy (true);
    tmp_file.create ("/tmp", "test_tube_");

    geomtools::tube my_tube;
    my_tube.set(3.5 * CLHEP::mm,
                5.2 * CLHEP::mm);
    if (do_hole) {
      my_tube.set_inner_r(2.0 * CLHEP::mm);
    }
    if (do_sector) {
      my_tube.set_phi(M_PI / 6, 0.75 * M_PI);
    }

    std::clog << "Tube     = " << my_tube << " " << std::endl;

    std::clog << "Volume   = " << my_tube.get_volume () / CLHEP::m3 << " m3" << std::endl;

    std::clog << "Outer side surface = "
              << my_tube.get_surface (geomtools::tube::FACE_OUTER_SIDE) / CLHEP::mm2
              << " mm2" << std::endl;

    std::clog << "Inner side surface = "
              << my_tube.get_surface (geomtools::tube::FACE_INNER_SIDE) / CLHEP::mm2
              << " mm2" << std::endl;

    std::clog << "Top surface = "
              << my_tube.get_surface (geomtools::tube::FACE_TOP) / CLHEP::mm2
              << " mm2" << std::endl;

    std::clog << "Bottom surface = " << my_tube.get_surface (geomtools::tube::FACE_BOTTOM) / CLHEP::mm2
              << " mm2" << std::endl;

    std::clog << "Full surface = "
              << my_tube.get_surface (geomtools::tube::FACE_ALL) / CLHEP::mm2
              << " mm2" << std::endl;
    my_tube.tree_dump(std::clog, "Tube: ", "INFO: ");

    geomtools::placement tube_placement(0.2, 0.15, 0.1, M_PI / 7.0, M_PI / 6.0, 0.0);
    if (do_identity) {
      tube_placement.set_identity();
    }

    {
      tmp_file.out() << "# tube (index 0): " << std::endl;
      geomtools::gnuplot_draw::draw_tube(tmp_file.out(), tube_placement, my_tube);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      int counts = 0;
      tmp_file.out() << "# locate (index 1)" << std::endl;
      size_t nshoots = 50000;
      for (int i = 0; i < (int) nshoots; i++) {
        double dim = 5. * CLHEP::mm;
        geomtools::vector_3d position (dim * ( -1 + 2 * drand48 ()),
                                       dim * ( -1 + 2 * drand48 ()),
                                       dim * ( -1 + 2 * drand48 ()));
        double skin = 0.20 * CLHEP::mm;
        geomtools::vector_3d position_c;
        tube_placement.mother_to_child(position, position_c);
        if (do_locate) {
          if (outer && my_tube.is_on_surface(position_c,
                                             geomtools::tube::FACE_OUTER_SIDE, skin)) {
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 3.0);
            counts++;
          } else if (inner && my_tube.is_on_surface(position_c,
                                                    geomtools::tube::FACE_INNER_SIDE, skin)) {
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 4.0);
            counts++;
          } else if (top && my_tube.is_on_surface(position_c,
                                                  geomtools::tube::FACE_TOP, skin)) {
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 5.0);
            counts++;
          } else if (bottom && my_tube.is_on_surface(position_c,
                                                     geomtools::tube::FACE_BOTTOM, skin)) {
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 6.0);
            counts++;
          } else if (bulk && my_tube.check_inside(position_c, skin)) {
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 2.0);
            counts++;
          } else if (start && my_tube.is_on_surface(position_c, geomtools::tube::FACE_START_ANGLE, skin)) {
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 7.0);
            counts++;
          } else if (stop && my_tube.is_on_surface(position_c, geomtools::tube::FACE_STOP_ANGLE, skin)) {
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 8.0);
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
      // Draw tube :
      tmp_file.out() << "# tube (index 2):" << std::endl;
      geomtools::wires_type tube_wires;
      my_tube.generate_wires(tube_wires,
                             tube_placement,
                             geomtools::i_wires_3d_rendering::WR_NONE
                             | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                             | geomtools::i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY
                             | geomtools::i_wires_3d_rendering::WR_BASE_BOUNDINGS
                             // | geomtools::i_wires_3d_rendering::WR_BASE_EXPLODE
                             // | geomtools::tube::WR_TUBE_NO_BOTTOM_FACE
                             | geomtools::tube::WR_TUBE_NO_TOP_FACE
                             // | geomtools::tube::WR_TUBE_NO_INNER_FACE
                             // | geomtools::tube::WR_TUBE_NO_OUTER_FACE
                             );
      geomtools::gnuplot_draw::draw_wires(tmp_file.out(), tube_wires);
      tmp_file.out() << std::endl << std::endl;
    }
   
    {
      std::clog << "test 3: Intercept..." << std::endl;

      size_t nshoots = 10000;
      int counts = 0;
      tmp_file.out() << "# intercept (index 3)" << std::endl;
      for (int i = 0; i < (int) nshoots; i++) {
        double dim = 4. * CLHEP::mm;
        geomtools::vector_3d pos (dim * drand48(),
                                  dim * drand48(),
                                  dim * drand48());
        geomtools::vector_3d dir;
        geomtools::randomize_direction (drand48, dir);

        geomtools::vector_3d pos_c;
        tube_placement.mother_to_child(pos, pos_c);
        geomtools::vector_3d dir_c;
        tube_placement.mother_to_child_direction(dir, dir_c);

        geomtools::face_intercept_info intercept_c;

        if (my_tube.find_intercept (pos_c, dir_c, intercept_c)) {
          counts++;
          geomtools::vector_3d impact;
          tube_placement.child_to_mother(intercept_c.get_impact(), impact);
          geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), impact);
          if (debug) std::clog << "test 3: Intercept face="
                               << intercept_c.get_face_id()
                               << " at impact=" << impact
                               << std::endl;
        } else {
          if (debug) std::clog << "test 3: No intercept." << std::endl;
        }
      }
      if (counts == 0) {
        tmp_file.out() << "0 0 0 -1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
    }
   
    if (do_envelope) {
      tmp_file.out() << "# tube envelope (index 4):" << std::endl;
      geomtools::cylinder my_tube_envelope;
      my_tube.compute_envelope(my_tube_envelope, 0.2, 0.2);
      // Draw envelope :
      geomtools::wires_type cylinder_wires;
      my_tube_envelope.generate_wires(cylinder_wires,
                             tube_placement,
                             geomtools::i_wires_3d_rendering::WR_NONE
                             | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                             | geomtools::i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY
                             //| geomtools::i_wires_3d_rendering::WR_BASE_BOUNDINGS
                             // | geomtools::i_wires_3d_rendering::WR_BASE_EXPLODE
                             // | geomtools::tube::WR_TUBE_NO_BOTTOM_FACE
                             // | geomtools::tube::WR_TUBE_NO_TOP_FACE
                             // | geomtools::tube::WR_TUBE_NO_INNER_FACE
                             // | geomtools::tube::WR_TUBE_NO_OUTER_FACE
                             );
      geomtools::gnuplot_draw::draw_wires(tmp_file.out(), cylinder_wires);
      tmp_file.out() << std::endl << std::endl;

    }
  

    // if (interactive) {
    //   std::clog << "Enter a tube (example: '{tube 900 1000 1000}'): ";
    //   std::cin >> std::ws >> my_tube;
    //   if (std::cin) {
    //     std::clog << "Tube = " << my_tube << " " << std::endl;
    //   } else {
    //     throw std::runtime_error ("Invalid input for tube!");
    //   }
    // }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd ("set title 'Test geomtools::tube' ");
      g1.cmd ("set grid");
      g1.cmd ("set size ratio -1");
      g1.cmd ("set view equal xyz");
      g1.cmd("set xrange [-6:+6]");
      g1.cmd("set yrange [-6:+6]");
      g1.cmd("set zrange [-8:+8]");
      g1.cmd("set xyplane at -8");
      // g1.cmd ("set xyplane at -10");
      g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 title 'Tube' with lines ";
        plot_cmd << ", '' index 1 using 1:2:3:4 title 'Locate' with points pt 6 ps 0.3 linecolor variable ";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 2 title 'Tube (grid)' with lines ";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 title 'Tube' with lines ";
        plot_cmd << " , '' index 3 title 'Intercepts' with dots ";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }
      
      if (do_envelope) {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 title 'Tube' with lines ";
        plot_cmd << " , '' index 4 title 'Envelope' with lines ";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }
 
#endif
    }

  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

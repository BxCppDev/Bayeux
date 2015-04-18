// test_box.cxx

// Ourselves:
#include <geomtools/box.h>

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
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
#include <geomtools/rectangle.h>

int main (int argc_, char ** argv_)
{
  // using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    bool debug = false;
    bool draw = false;
    long seed = 314159;
    bool test1 = true;
    bool test2 = true;
    bool test3 = true;

    bool do_identity = false;
    bool front  = true;
    bool back   = true;
    bool top    = true;
    bool bottom = true;
    bool left   = true;
    bool right  = true;
    bool bulk   = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];
      if (arg == "-d" || arg == "--debug") debug = true;
      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-I" || arg == "--identity") do_identity = true;
      // if (arg == "-1") test1 = false;
      // if (arg == "-2") test2 = false;
      // if (arg == "-3") test3 = false;
      if (arg == "-b") back   = false;
      if (arg == "-f") front  = false;
      if (arg == "-t") top    = false;
      if (arg == "-m") bottom = false;
      if (arg == "-l") left   = false;
      if (arg == "-r") right  = false;
      if (arg == "-B") bulk   = true;
      iarg++;
    }

    srand48 (seed);

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_box_");

    geomtools::placement box_placement(0.2, 0.15, 0.1, M_PI / 7.0, M_PI / 6.0, 0.0);
    if (do_identity) {
      box_placement.set_identity();
    }

    geomtools::box my_box(3.0 * CLHEP::mm, 2.0 * CLHEP::mm, 1.0 * CLHEP::mm);
    my_box.lock();
    my_box.tree_dump(std::clog, "my_box", "test 1: ");

    // test 1:
    if (test1) {
      std::clog << "test 1: Box #1  = " << my_box << " " << std::endl;

      std::clog << "test 1: Volume  = " << my_box.get_volume () / CLHEP::m3
           << " m3" << std::endl;

      std::clog << "test 1: Front surface = "
           << my_box.get_surface (geomtools::box::FACE_FRONT) / CLHEP::m2
           << " m2" << std::endl;

      std::clog << "test 1: Top surface = "
           << my_box.get_surface (geomtools::box::FACE_TOP) / CLHEP::m2
           << " m2" << std::endl;

      std::clog << "test 1: Right surface = "
           << my_box.get_surface (geomtools::box::FACE_RIGHT) / CLHEP::m2
           << " m2" << std::endl;

      std::clog << "test 1: Full surface = "
           << my_box.get_surface (geomtools::box::FACE_ALL) / CLHEP::m2
           << " m2" << std::endl;

      {
        bool test_in = false;
        if (test_in) {
          std::clog << "test 1: Enter a box (example: '{box 2 3 0.5}'): ";
          std::cin >> std::ws >> my_box;
          if (std::cin) {
            std::clog << "test 1: Box     = " << my_box << " " << std::endl;
          } else {
            throw std::runtime_error ("Invalid input for box!");
          }
        }
      }
      std::clog << "INFO: Basic done." << std::endl;
    }

    // test 2:
    if (test2) {
      std::clog << "test 2: Box   = " << my_box << " " << std::endl;

      geomtools::vector_3d pos(2.5 * CLHEP::mm,
                               3.5 * CLHEP::mm,
                               2.5 * CLHEP::mm);
      geomtools::vector_3d dir(-1, -2.5, -1.5);

      geomtools::vector_3d pos_c;
      box_placement.mother_to_child(pos, pos_c);
      geomtools::vector_3d dir_c;
      box_placement.mother_to_child_direction(dir, dir_c);

      geomtools::face_intercept_info intercept_c;
      geomtools::vector_3d impact;
      double tolerance = 0.05 * CLHEP::mm;
      if (my_box.find_intercept(pos_c, dir_c, intercept_c, tolerance)) {
        box_placement.child_to_mother(intercept_c.get_impact(), impact);
        std::clog << "test 2: Intercept face="
                  << intercept_c.get_face_id().get_face_bits()
                  << " at impact="
                  << impact
                  << std::endl;
      } else {
        std::clog << "test 2: No intercept." << std::endl;
      }

      tmp_file.out() << "# box (index 0): " << std::endl;
      geomtools::gnuplot_draw::draw_box(tmp_file.out(),
                                        box_placement,
                                        my_box);
      tmp_file.out() << std::endl << std::endl;

      tmp_file.out() << "# source (index 1): " << std::endl;
      geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), pos);
      tmp_file.out() << std::endl << std::endl;

      tmp_file.out() << "# impact (index 2): " << std::endl;
      geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), impact);
      tmp_file.out() << std::endl << std::endl;

      tmp_file.out() << "# track (index 3): " << std::endl;
      geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), pos);
      geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), impact);
      tmp_file.out() << std::endl << std::endl;
      std::clog << "INFO: Basic draw done." << std::endl;
    }

    // test 3:
    if (test3) {
      std::clog << "test 3: Box #3   = " << my_box << " " << std::endl;
      tmp_file.out() << "# intercepts (index 4): " << std::endl;
      size_t nshoots = 100000;
      int counts = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos(10.* drand48() * CLHEP::mm,
                                 10.* drand48() * CLHEP::mm,
                                 10.* drand48() * CLHEP::mm);
        geomtools::vector_3d dir;
        geomtools::randomize_direction (drand48, dir);

        geomtools::vector_3d pos_c;
        box_placement.mother_to_child(pos, pos_c);
        geomtools::vector_3d dir_c;
        box_placement.mother_to_child_direction(dir, dir_c);

        geomtools::face_intercept_info intercept_c;
        if (my_box.find_intercept(pos_c, dir_c, intercept_c)) {
          counts++;
          geomtools::vector_3d impact;
          box_placement.child_to_mother(intercept_c.get_impact(), impact);
          geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), impact);
          if (debug) std::clog << "test 3: Intercept face="
                               << intercept_c.get_face_id().get_face_bits()
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
      std::clog << "INFO: Intercept done." << std::endl;

      tmp_file.out() << "# locate (index 5): " << std::endl;
      nshoots = 1000000;
      counts = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        double dim = 3. * CLHEP::mm;
        geomtools::vector_3d position(dim * ( -1 + 2 * drand48 ()),
                                      dim * ( -1 + 2 * drand48 ()),
                                      dim * ( -1 + 2 * drand48 ()));
        geomtools::vector_3d position_c;
        box_placement.mother_to_child(position, position_c);

        double skin = 0.5 * CLHEP::mm;
        if (back && my_box.is_on_surface(position_c,
                                         geomtools::face_identifier(geomtools::box::FACE_BACK),
                                         skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 3.0);
          counts++;
        } else if (front && my_box.is_on_surface(position_c,
                                                 geomtools::face_identifier(geomtools::box::FACE_FRONT),
                                                 skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 4.0);
          counts++;
        } else if (top && my_box.is_on_surface(position_c,
                                               geomtools::face_identifier(geomtools::box::FACE_TOP),
                                               skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 5.0);
          counts++;
        } else if (bottom && my_box.is_on_surface(position_c,
                                                  geomtools::face_identifier(geomtools::box::FACE_BOTTOM),
                                                  skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 6.0);
          counts++;
        } else if (left && my_box.is_on_surface(position_c,
                                                geomtools::face_identifier(geomtools::box::FACE_LEFT),
                                                skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 7.0);
          counts++;
        } else if (right && my_box.is_on_surface(position_c,
                                                 geomtools::face_identifier(geomtools::box::FACE_RIGHT),
                                                 skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 8.0);
          counts++;
        } else if (bulk && my_box.check_inside(position_c, skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 2.0);
          counts++;
        }

      }
      if (counts == 0) {
        tmp_file.out() << "0 0 0 -1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
      std::clog << "INFO: Locate done." << std::endl;
    }

    {
      // Draw box :
      tmp_file.out() << "# box (index 6):" << std::endl;
      geomtools::wires_type box_wires;
      my_box.generate_wires(box_wires,
                            box_placement,
                            geomtools::i_wires_3d_rendering::WR_NONE
                            | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                            | geomtools::i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY
                            // | geomtools::box::WR_BASE_EXPLODE
                            // | geomtools::box::WR_BOX_NO_BACK_FACE
                            | geomtools::box::WR_BOX_NO_FRONT_FACE
                            // | geomtools::box::WR_BOX_NO_LEFT_FACE
                            // | geomtools::box::WR_BOX_NO_RIGHT_FACE
                            | geomtools::box::WR_BOX_NO_BOTTOM_FACE
                            // | geomtools::box::WR_BOX_NO_TOP_FACE
                            );
      geomtools::gnuplot_draw::draw_wires(tmp_file.out(), box_wires);
      tmp_file.out() << std::endl << std::endl;
      std::clog << "INFO: Draw box done." << std::endl;
    }

    {
      // Draw faces :
      tmp_file.out() << "# faces (index 7):" << std::endl;
      int face_counter = 0;
      for (geomtools::box::faces_mask_type face_id = geomtools::box::_FACE_BEGIN;
           face_id != geomtools::box::_FACE_END;
           face_id = static_cast<geomtools::box::faces_mask_type>(face_id << 1)) {
        // std::cerr << "DEVEL: face_id=" << face_id << std::endl;
        geomtools::rectangle face;
        geomtools::placement face_placement;
        my_box.compute_face(face_id, face, face_placement);
        geomtools::wires_type face_wires;
        face.generate_wires(face_wires, face_placement);
        geomtools::gnuplot_draw::draw_wires(tmp_file.out(), box_placement, face_wires);
        face_counter++;
        // Test:
        // if (face_counter == 5) break;
      }
      tmp_file.out() << std::endl << std::endl;
      // std::cerr << "INFO: face_counter=" << face_counter << std::endl;
      std::clog << "INFO: Draw faces done." << std::endl;
    }

    my_box.tree_dump(std::clog, "my_box", "Final status: ");

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test geomtools::box' ");
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      g1.cmd("set xrange [-4:+4]");
      g1.cmd("set yrange [-4:+4]");
      g1.cmd("set zrange [-4:+4]");
      g1.cmd("set xyplane at -4");
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename() << "' index 0 title 'Box' with lines ,";
        plot_cmd << " '' index 1 title 'Source' with points lt 3 pt 7 ps 1.0 ,";
        plot_cmd << " '' index 2 title 'Impact' with points lt 4 pt 7 ps 1.0 ,";
        plot_cmd << " '' index 3 title 'Track'  with lines lt 2 ";
        g1.cmd (plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename() << "' index 0 title 'Box' with lines ";
        plot_cmd << " , '' index 4 title 'Intercepts' with dots ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename() << "' index 0 title 'Box' with lines ";
        plot_cmd << " , '' index 5 using 1:2:3:4 title 'Locate' with points pt 6 ps 0.3 linecolor variable ";
        g1.cmd (plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep (200);
      }
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename() << "' index 6 title 'Box (grid)' with lines ";
        plot_cmd << " , '' index -1 title 'Box' with lines ";
        g1.cmd (plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep (200);
      }
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename() << "' index 7 title 'Box faces' with lines ";
        // plot_cmd << ", '" << tmp_file.get_filename() << "' index 0 title 'Box' with lines ,";
        g1.cmd (plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep (200);
      }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
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

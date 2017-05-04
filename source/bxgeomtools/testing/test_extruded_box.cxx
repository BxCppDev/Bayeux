// test_extruded_box.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

// This project:
#include <geomtools/geomtools_config.h>
#include <geomtools/extruded_box.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

// Third party:
// - Bayeux/datatools:
#include <datatools/temporary_files.h>
#include <datatools/utils.h>

int main (int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    bool draw  = false;
    bool debug  = false;
    long seed  = 314159;
    bool surface = true;
    bool inside = false;
    bool outside = false;
    bool top = true;
    bool bottom = true;
    bool do_identity = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-d" || arg == "--debug") debug = true;
      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-I" || arg == "--identity") do_identity = true;
      if (arg == "--inside") inside = true;
      if (arg == "--no-inside") inside = false;
      if (arg == "--outside") outside = true;
      if (arg == "--no-outside") outside = false;
      if (arg == "--surface") surface = true;
      if (arg == "--no-surface") surface = false;
      if (arg == "--no-top") top = false;
      if (arg == "--no-bottom") bottom = false;

      iarg++;
    }

    srand48(seed);

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_extruded_box_");

    geomtools::placement solid_placement(0.2, 0.15, 0.1, M_PI / 7.0, M_PI / 6.0, 0.0);
    if (do_identity) {
      solid_placement.set_identity();
    }

    geomtools::extruded_box my_xbox (8. * CLHEP::mm,
                                     8. * CLHEP::mm,
                                     6. * CLHEP::mm,
                                     1. * CLHEP::mm);
    if (!bottom) {
      my_xbox.remove_bottom();
    }
    if (!top) {
      my_xbox.remove_top();
    }
    my_xbox.tree_dump(std::clog, "my_xbox");

    {
      clog << "test 1: Extruded_Box #1         = " << my_xbox << " " << endl;

      clog << "test 1: Volume                  = " << my_xbox.get_volume () / CLHEP::m3
           << " m3" << endl;

      clog << "test 1: Top surface             = "
           << my_xbox.get_surface (geomtools::extruded_box::FACE_TOP) / CLHEP::m2
           << " m2" << endl;

      clog << "test 1: Bottom surface          = "
           << my_xbox.get_surface (geomtools::extruded_box::FACE_BOTTOM) / CLHEP::m2
           << " m2" << endl;

      clog << "test 1: Bottom surface (inside) = "
           << my_xbox.get_surface (geomtools::extruded_box::FACE_INSIDE_BOTTOM) / CLHEP::m2
           << " m2" << endl;

      clog << "test 1: Front surface           = "
           << my_xbox.get_surface (geomtools::extruded_box::FACE_FRONT) / CLHEP::m2
           << " m2" << endl;

      clog << "test 1: Front surface (inside)  = "
           << my_xbox.get_surface (geomtools::extruded_box::FACE_INSIDE_FRONT) / CLHEP::m2
           << " m2" << endl;

      clog << "test 1: Right surface           = "
           << my_xbox.get_surface (geomtools::extruded_box::FACE_RIGHT) / CLHEP::m2
           << " m2" << endl;

      clog << "test 1: Right surface (inside)  = "
           << my_xbox.get_surface (geomtools::extruded_box::FACE_INSIDE_RIGHT) / CLHEP::m2
           << " m2" << endl;

      clog << "test 1: Full surface            = "
           << my_xbox.get_surface (geomtools::extruded_box::FACE_ALL) / CLHEP::m2
           << " m2" << endl;

      {
        bool test_in = false;
        if (test_in) {
          clog << "test 1: Enter a extruded_box (example: '{extruded_box 1000 1000 1000}'): ";
          cin >> ws >> my_xbox;
          if (cin) {
            clog << "test 1: Extruded_Box     = " << my_xbox << " " << endl;
          } else {
            throw  std::runtime_error ("Invalid input for extruded_box!");
          }
        }
      }
    }

    {
      tmp_file.out() << "# draw extruded box (index 0): " << std::endl;
      geomtools::gnuplot_draw::draw_extruded_box (tmp_file.out(),
                                                  solid_placement,
                                                  my_xbox,
                                                  geomtools::i_wires_3d_rendering::WR_NONE
                                                  | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                                                  );
      tmp_file.out() << std::endl << std::endl;
      std::clog << "INFO: Draw box done." << std::endl;
    }

    {
      tmp_file.out() << "# locate (index 1): " << std::endl;
      size_t nshoots = 20000;
      int counts = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        double dim = 8. * CLHEP::mm;
        geomtools::vector_3d position (dim * (-1 + 2 * drand48 ()),
                                       dim * (-1 + 2 * drand48 ()),
                                       dim * (-1 + 2 * drand48 ()));
        geomtools::vector_3d position_c;
        solid_placement.mother_to_child(position, position_c);
        double skin = 0.4 * CLHEP::mm;
        if (outside && my_xbox.is_outside(position_c, skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 1.0);
          counts++;
        } else if (inside && my_xbox.is_inside(position_c, skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 2.0);
          counts++;
        } else if (surface && my_xbox.is_on_surface(position_c,
                                                    geomtools::face_identifier(geomtools::extruded_box::FACE_TOP),
                                                    skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 3.0);
          counts++;
        } else if (surface && my_xbox.is_on_surface(position_c,
                                                    geomtools::face_identifier(geomtools::extruded_box::FACE_INSIDE_RIGHT),
                                                    skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 4.0);
          counts++;
        } else if (surface && my_xbox.is_on_surface(position_c,
                                                    geomtools::face_identifier(geomtools::extruded_box::FACE_INSIDE_BOTTOM),
                                                    skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 6.0);
          counts++;
        } else if (surface && my_xbox.is_on_surface(position_c,
                                                    geomtools::face_identifier(geomtools::extruded_box::FACE_ALL),
                                                    skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 5.0);
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
      tmp_file.out() << "# intercepts (index 2): " << std::endl;
      size_t nshoots = 50000;
      int counts = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos(10. * (-1 + 2 *  drand48()) * CLHEP::mm,
                                 10. * (-1 + 2 *  drand48()) * CLHEP::mm,
                                 10. * (-1 + 2 *  drand48()) * CLHEP::mm);
        geomtools::vector_3d dir;
        geomtools::randomize_direction (drand48, dir);

        geomtools::vector_3d pos_c;
        solid_placement.mother_to_child(pos, pos_c);
        geomtools::vector_3d dir_c;
        solid_placement.mother_to_child_direction(dir, dir_c);

        geomtools::face_intercept_info intercept_c;
        double skin = 0.4;
        if (my_xbox.find_intercept(pos_c, dir_c, intercept_c, skin)) {
          counts++;
          geomtools::vector_3d impact;
          solid_placement.child_to_mother(intercept_c.get_impact(), impact);
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
    }

    {
      tmp_file.out() << "# draw extruded box (index 3): " << std::endl;
      geomtools::gnuplot_draw::draw_extruded_box (tmp_file.out(),
                                                  solid_placement,
                                                  my_xbox,
                                                  geomtools::i_wires_3d_rendering::WR_NONE
                                                  | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                                                  | geomtools::i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY
                                                  | geomtools::extruded_box::WR_EXTRBOX_NO_EXTERNAL_FACES
                                                  );
      tmp_file.out() << std::endl << std::endl;
      std::clog << "INFO: Draw box done." << std::endl;
    }

    my_xbox.tree_dump(std::clog, "my_xbox", "Final status: ");

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd ("set title 'Test geomtools::extruded_box' ");
      g1.cmd ("set grid");
      g1.cmd ("set size ratio -1");
      g1.cmd ("set view equal xyz");
      g1.cmd("set xrange [-10:+10]");
      g1.cmd("set yrange [-10:+10]");
      g1.cmd("set zrange [-10:+10]");
      g1.cmd("set xyplane at -10");
      g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename() << "' index 0 title 'Extruded box' with lines ,";
        g1.cmd (plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 title 'Extruded box' with lines";
        plot_cmd << ", '' index 1 using 1:2:3:4 title 'Locate' with points pt 6 ps 0.3 linecolor variable";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename() << "' index 0 title 'Extruded box' with lines ";
        plot_cmd << ", '' index 2 title 'Intercepts' with dots ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename() << "' index 3 title 'Extruded box (no external faces)' with lines";
        g1.cmd (plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    }

  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error!" <<std:: endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

// test_sphere.cxx

// Ourselves:
#include <geomtools/sphere.h>

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

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'geomtools::sphere'!" << std::endl;

    bool debug = false;
    bool draw = false;
    bool do_identity = false;
    bool do_sector = false;
    bool do_hole = false;
    long seed  = 314159;
    bool test1 = true;
    bool test2 = true;
    bool test3 = true;

    bool inner  = true;
    bool outer   = true;
    bool start_phi = true;
    bool stop_phi  = true;
    bool start_theta = true;
    bool stop_theta  = true;
    bool bulk   = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-d" || arg == "--debug") debug = true;
      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-I" || arg == "--identity") do_identity = true;
      if (arg == "-S" || arg == "--sector") do_sector = true;
      if (arg == "-H" || arg == "--hole") do_hole = true;
      if (arg == "--inner") inner = false;
      if (arg == "--outer") outer = false;
      if (arg == "--start-phi") start_phi = false;
      if (arg == "--stop-phi") stop_phi = false;
      if (arg == "--start-theta") start_theta = false;
      if (arg == "--stop-theta") stop_theta = false;
      if (arg == "--bulk")  bulk  = true;

      iarg++;
    }

    srand48(seed);

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_sphere_");

    geomtools::sphere my_sphere(3.0 * CLHEP::mm);
    if (do_hole) {
      my_sphere.set_r_min(2.0 * CLHEP::mm);
    }
    if (do_sector) {
      my_sphere.set_theta(0.25 * M_PI, 0.25 * M_PI);
      my_sphere.set_phi(M_PI/6, M_PI/7);
    }

    geomtools::placement sphere_placement(0.2, 0.15, 0.1, M_PI / 7.0, M_PI / 6.0, 0.0);
    if (do_identity) {
      sphere_placement.set_identity();
    }

    // test 1:
    if (test1) {

      std::clog << "test 1: Sphere #1 = " << my_sphere << " " << std::endl;

      std::clog << "test 1: Volume   = " << my_sphere.get_volume () / CLHEP::m3 << " m3" << std::endl;

      std::clog << "test 1: Side surface = "
           << my_sphere.get_surface (geomtools::sphere::FACE_SIDE) / CLHEP::m2
           << " m2" << std::endl;

      std::clog << "test 1: Full surface = "
           << my_sphere.get_surface (geomtools::sphere::FACE_ALL) / CLHEP::m2
           << " m2" << std::endl;

      {
        bool test_in = false;
        if (test_in) {
          std::clog << "test 1: Enter a sphere (example: '{sphere 1000}'): ";
          std::cin >> std::ws >> my_sphere;
          if (std::cin) {
            std::clog << "test 1: Sphere = " << my_sphere << " " << std::endl;
          } else {
            throw std::runtime_error("Invalid input for sphere!");
          }
        }
      }
    }

    // test 2:
    if (test2) {
      std::clog << "test 2: Sphere #2   = " << my_sphere << " " << std::endl;

      geomtools::vector_3d pos(4.0 * CLHEP::mm,
                               0.0 * CLHEP::mm,
                               0.0 * CLHEP::mm);
      geomtools::vector_3d dir(-1, 0, 0);

      geomtools::vector_3d pos_c;
      sphere_placement.mother_to_child(pos, pos_c);
      geomtools::vector_3d dir_c;
      sphere_placement.mother_to_child_direction(dir, dir_c);

      geomtools::face_intercept_info intercept_c;
      geomtools::vector_3d impact;
      if (my_sphere.find_intercept(pos_c, dir_c, intercept_c)) {
        sphere_placement.child_to_mother(intercept_c.get_impact(), impact);
        std::clog << "test 2: Intercept face=" << intercept_c.get_face_id ()
             << " at impact=" << impact
             << std::endl;
      } else {
        std::clog << "test 2: No intercept." << std::endl;
      }

      tmp_file.out() << "# sphere (index 0): " << std::endl;
      geomtools::gnuplot_draw::draw_sphere(tmp_file.out(),
                                           sphere_placement,
                                           my_sphere,
                                           geomtools::sphere::WR_NONE
                                           | geomtools::sphere::WR_BASE_GRID
                                           );
      tmp_file.out() << std::endl << std::endl;

      tmp_file.out() << "# source (index 1): " << std::endl;
      geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), pos);
      tmp_file.out() << std::endl << std::endl;

      tmp_file.out() << "# impact (index 2): " << std::endl;
      geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(),
                                                impact);
      tmp_file.out() << std::endl << std::endl;

      geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), pos);
      geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), impact);
      tmp_file.out() << std::endl << std::endl;
    }

    // test 3:
    if (test3) {
      std::clog << "test 3: Sphere #3   = " << my_sphere
                << " " << std::endl;
      tmp_file.out() << "# sphere (index 4): " << std::endl;
      geomtools::gnuplot_draw::draw_sphere(tmp_file.out(),
                                           sphere_placement,
                                           my_sphere,
                                           geomtools::sphere::WR_NONE
                                           | geomtools::sphere::WR_BASE_BOUNDINGS
                                           | geomtools::sphere::WR_BASE_GRID
                                           | geomtools::sphere::WR_BASE_LOW_ANGLE_SAMPLING
                                           );
      tmp_file.out() << std::endl << std::endl;

      {
        tmp_file.out() << "# intercepts (index 5): " << std::endl;
        size_t nshoots = 100000;
        int counts = 0;
        for (int i = 0; i < (int) nshoots; i++) {
          double dim = 4. * CLHEP::mm;
          geomtools::vector_3d pos(dim * drand48(),
                                   dim * drand48(),
                                   dim * drand48());
          geomtools::vector_3d dir;
          geomtools::randomize_direction(drand48, dir);

          geomtools::vector_3d pos_c;
          sphere_placement.mother_to_child(pos, pos_c);
          geomtools::vector_3d dir_c;
          sphere_placement.mother_to_child_direction(dir, dir_c);
          geomtools::face_intercept_info intercept_c;

          double tolerance = 0.1 * CLHEP::mm;
          if (my_sphere.find_intercept(pos_c, dir_c, intercept_c, tolerance)) {
            if (debug) std::clog << "test 3: Intercept face="
                                 << intercept_c.get_face_id ()
                                 << " at impact=" << intercept_c.get_impact ()
                                 << std::endl;
            counts++;
            geomtools::vector_3d impact;
            sphere_placement.child_to_mother(intercept_c.get_impact(), impact);
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), impact, 3.0);
          }
        } // for
        if (counts == 0) {
          tmp_file.out() << "0 0 0 -1" << std::endl;
        }
        tmp_file.out() << std::endl << std::endl;
      }
    }

    {
      tmp_file.out() << "# locate (index 6): " << std::endl;
      int nshoots = 100000;
      int counts = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        double dim = 4. * CLHEP::mm;
        geomtools::vector_3d position(dim * ( -1 + 2 * drand48 ()),
                                      dim * ( -1 + 2 * drand48 ()),
                                      dim * ( -1 + 2 * drand48 ()));
        geomtools::vector_3d position_c;
        sphere_placement.mother_to_child(position, position_c);

        double skin = 0.25 * CLHEP::mm;
        if (inner && my_sphere.is_on_surface(position_c,
                                             geomtools::face_identifier(geomtools::sphere::FACE_INNER_SIDE),
                                             skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 3.0);
          counts++;
        } else if (outer && my_sphere.is_on_surface(position_c,
                                                    geomtools::face_identifier(geomtools::sphere::FACE_OUTER_SIDE),
                                                    skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 4.0);
          counts++;
        } else if (start_theta && my_sphere.is_on_surface(position_c,
                                                          geomtools::face_identifier(geomtools::sphere::FACE_START_THETA_SIDE),
                                                          skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 5.0);
          counts++;
        } else if (stop_theta && my_sphere.is_on_surface(position_c,
                                                         geomtools::face_identifier(geomtools::sphere::FACE_STOP_THETA_SIDE),
                                                         skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 6.0);
          counts++;
        } else if (start_phi && my_sphere.is_on_surface(position_c,
                                                        geomtools::face_identifier(geomtools::sphere::FACE_START_PHI_SIDE),
                                                        skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 7.0);
          counts++;
        } else if (stop_phi && my_sphere.is_on_surface(position_c,
                                                       geomtools::face_identifier(geomtools::sphere::FACE_STOP_PHI_SIDE),
                                                       skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 8.0);
          counts++;
        } else if (bulk && my_sphere.check_inside(position_c, skin)) {
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

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test geomtools::sphere' ");
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      g1.cmd("set xrange [-6:+6]");
      g1.cmd("set yrange [-6:+6]");
      g1.cmd("set zrange [-6:+6]");
      g1.cmd("set xyplane at -6");
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 0 title 'Sphere' with lines ";
        plot_cmd << ", '' index 1 title 'Source' with points lt 3 pt 7 ps 1.0 ";
        plot_cmd << ", '' index 2 title 'Impact' with points lt 4 pt 7 ps 1.0 ";
        plot_cmd << ", '' index 3 title 'Track'  with lines lt 2 ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 4 title 'Sphere' with lines ";
        plot_cmd << " , '' index 5 title 'Intercepts' with dots ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename() << "' index 0 title 'Sphere' with lines ";
        plot_cmd << " , '' index 6 using 1:2:3:4 title 'Locate' with points pt 6 ps 0.70 linecolor variable ";
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

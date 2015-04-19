// test_sphere_vg.cxx

// Ourselves:
#include <genvtx/sphere_vg.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/temporary_files.h>
#include <datatools/utils.h>
#include <datatools/units.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>
// - Bayeux/geomtools:
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
    std::clog << "Test program for class 'genvtx::sphere_vg'!" <<std::endl;

    bool debug = false;
    bool draw = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];

      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        } else if (option == "-D" || option == "--draw") {
          draw = true;
        }
      }
      iarg++;
    }

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_sphere_vg_");

    std::string       rng_id = "mt19937";
    unsigned long int rng_seed = 12345;
    mygsl::rng random(rng_id, rng_seed);

    size_t nshoots = 50000;

    geomtools::sphere ball;
    ball.set_r_max(2.0);
    ball.set_r_min(1.0);
    ball.set_theta(20.0 * CLHEP::degree, 120.0 * CLHEP::degree);
    ball.set_phi(30.0 * CLHEP::degree, 70.0 * CLHEP::degree);

    {
      geomtools::vector_3d pos;
      geomtools::rotation_3d rot;
      geomtools::gnuplot_draw::draw_sphere(tmp_file.out(), pos, rot, ball);
      tmp_file.out() << std::endl << std::endl;
    }

    genvtx::sphere_vg vg;
    vg.set_debug (debug);

    bool surface = true;
    if (surface) {
      vg.set_sphere(ball);
      vg.set_mode(genvtx::sphere_vg::MODE_SURFACE);
      vg.set_surface_mask(geomtools::sphere::FACE_OUTER_SIDE |
                          geomtools::sphere::FACE_INNER_SIDE);
      std::clog << "warning: mode = " << vg.get_mode() << "\n";
      vg.set_skin_skip(0.025);
      vg.set_skin_thickness(0.05);
      vg.initialize_simple();
      vg.tree_dump(std::clog, "Sphere vertex generator (side, skipped)");
      geomtools::vector_3d vertex;
      for (int i = 0; i < (int) nshoots; i++) {
        vg.shoot_vertex(random, vertex);
        //if (vertex.x() < 0.0 && vertex.y() < 0.0) {
        geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), vertex, true);
        //}
      }
      tmp_file.out() << std::endl << std::endl;
    }

    vg.reset ();

    bool bulk = true;
    //bulk = false;
    if (bulk) {
      vg.set_sphere(ball);
      vg.set_mode(genvtx::sphere_vg::MODE_BULK);
      std::clog << "warning: mode = " << vg.get_mode() << "\n";
      vg.set_skin_skip(0.0);
      vg.set_skin_thickness(0.05);
      vg.initialize_simple();
      vg.tree_dump(std::clog, "Sphere vertex generator (bulk)");
      //nshoots = 50;
      geomtools::vector_3d vertex;
      for (int i = 0; i < (int) nshoots; i++) {
        vg.shoot_vertex(random, vertex);
        //if (vertex.y() > 0.0) { // && vertex.x() > 0.0) {
        geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), vertex, true);
        //}
      }
      tmp_file.out() << std::endl << std::endl;
    }

    {
      geomtools::vector_3d vertex;
      for (int i = 0; i < (int) (nshoots/10); i++) {
        genvtx::randomize_sphere(random,
                                 0.6, 1.4,
                                 M_PI/5, M_PI/3,
                                 -M_PI/2, -M_PI,
                                 vertex);
        // genvtx::randomize_sphere(random,
        //                          1.2, 1.25,
        //                          0.0, 0.5*M_PI,
        //                          0.0, M_PI,
        //                          vertex);
        geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), vertex, true);
      }
      tmp_file.out() << std::endl << std::endl;
    }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test genvtx::sphere_vg' ");
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      g1.cmd("set xrange [-2:+2]");
      g1.cmd("set yrange [-2:+2]");
      g1.cmd("set zrange [-2:+2]");
      g1.cmd("set xyplane at -2");
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 0 title 'Sphere' with lines ";
        g1.cmd(plot_cmd.str());
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
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

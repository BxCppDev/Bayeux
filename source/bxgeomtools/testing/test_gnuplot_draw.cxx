// test_gnuplot_draw.cxx

// Ourselves:
#include <geomtools/gnuplot_draw.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

// Third party:
// - CLHEP:
#include <CLHEP/Random/RanluxEngine.h>
#include <CLHEP/Random/RandFlat.h>
// - Bayeux/datatools:
#include <datatools/temporary_files.h>
#include <datatools/utils.h>

// This project:
#include <geomtools/geomtools_config.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
#include <geomtools/utils.h>
#include <geomtools/box.h>
#include <geomtools/cylinder.h>
#include <geomtools/circle.h>
#include <geomtools/rectangle.h>
#include <geomtools/tube.h>
#include <geomtools/line_3d.h>
#include <geomtools/placement.h>

class test_app
{
protected:

  long _seed;

public:

  test_app(long seed_)
  {
    _seed     = seed_;
    return;
  }

  void run(bool draw_)
  {

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_gnuplot_draw_");
    CLHEP::RanluxEngine engine(_seed, 3);
    CLHEP::RandFlat randflat(engine);

    std::ostream & output = tmp_file.out();

    geomtools::vector_3d pos;
    geomtools::rotation  rot;
    geomtools::create_rotation(rot, 0.0, 90. * CLHEP::degree, 0.0);

    // Head:
    pos.set(0., 0., 7.0);
    output << "# Head: \n";
    geomtools::gnuplot_draw::draw_circle(output, pos, rot, 1.);

    // Body:
    pos.set(0., 0., 4.5);
    geomtools::rectangle body(3., 2.);
    output << "# Body: \n";
    geomtools::gnuplot_draw::draw_rectangle(output, pos, rot, body);

    // Legs:
    geomtools::rectangle leg(3., 0.5);
    pos.set(0., -0.75, 1.5);
    output << "# Legs: \n";
    geomtools::gnuplot_draw::draw_rectangle(output, pos, rot, leg);
    pos.set(0., +0.75, 1.5);
    geomtools::gnuplot_draw::draw_rectangle(output, pos, rot, leg);

    // Arms:
    output << "# Arms: \n";
    geomtools::rectangle arm(0.5, 2.0);
    pos.set(0.,-2.0,5.75);
    geomtools::gnuplot_draw::draw_rectangle(output, pos, rot, arm);
    pos.set(0.,+2.0,5.75);
    geomtools::gnuplot_draw::draw_rectangle(output, pos, rot, arm);

    output << std::endl;

    // Holy hat:
    output << "# Holy hat: \n";
    geomtools::circle holy_hat(0.75);
    pos.set(0.,0.,9.0);
    geomtools::create_rotation(rot, 0.0, 150. * CLHEP::degree, 0.0);
    geomtools::gnuplot_draw::draw_circle(output, pos, rot, holy_hat);

    output << std::endl;

    // Saint's wand:
    output << "# Saint's wand: \n";
    pos.set(0., 0., 0.0);
    geomtools::create_rotation(rot, 0.0, 0. * CLHEP::degree, 0.0);
    geomtools::line_3d wand(0., 2., 0., 0., 3.5, 7.);
    geomtools::gnuplot_draw::draw_line(output, pos, rot, wand);

    output << std::endl;

    // Saint's box:
    output << "# Saint's box: \n";
    double x0 = 3.0;
    double y0 = 0.0;
    double z0 = 0.5;
    geomtools::box saint_box(2., 4., 1.);
    pos.set(x0, y0, z0);
    geomtools::create_rotation(rot, 0.0, 0. * CLHEP::degree, 0.0);
    geomtools::gnuplot_draw::draw_box(output, pos, rot, saint_box);
    pos.set(x0 + 1.707, y0, 1.707);
    geomtools::create_rotation(rot, 0.0, -45. * CLHEP::degree, 0.0);
    geomtools::rectangle saint_box_hat(2., 4.);
    geomtools::gnuplot_draw::draw_rectangle(output, pos, rot, saint_box_hat);

    output << std::endl;

    // Coins:
    output << "# Coins: \n";
    int nb_coins = randflat.shootInt(10, 30);
    for (int i = 0; i < nb_coins; i++) {
      pos.set(x0 + randflat.shoot(-0.5, +0.5),
              y0 + randflat.shoot(-0.5, +0.5),
              randflat.shoot(2,6));
      geomtools::create_rotation(rot,
                                 360. * CLHEP::degree * randflat.shoot(0.,1.),
                                 180. * CLHEP::degree * randflat.shoot(0., 1.),
                                 0.0);
      geomtools::cylinder coin(0.1, 0.02);
      geomtools::gnuplot_draw::draw_cylinder(output, pos, rot, coin);
    }

    // Holy tube:
    output << "# Holy tube: \n";
    geomtools::tube holy_tube(1.0, 1.3, 3.0);
    pos.set(1., -3., 2.0);
    geomtools::create_rotation(rot, 0.0, 10. * CLHEP::degree, 0.0);
    geomtools::gnuplot_draw::draw_tube(output, pos, rot, holy_tube);

    output << std::endl;

    geomtools::placement placement1(1.5, 3, 2.5,
                                    30. * CLHEP::degree,
                                    25. * CLHEP::degree,
                                    50. * CLHEP::degree);
    // A box:
    output << "# A box: \n";
    geomtools::box box1(2.0, 3.0, 0.5);
    geomtools::gnuplot_draw::draw_box(output, placement1, box1);

    geomtools::placement placement2(+1.5, -4, 4.5,
                                    10. * CLHEP::degree,
                                    65. * CLHEP::degree,
                                    0. * CLHEP::degree);

    // A cylinder:
    output << "# A cylinder: \n";
    // int mode = geomtools::gnuplot_draw::MODE_NULL;
    // mode = geomtools::gnuplot_draw::MODE_WIRED_CYLINDER;
    geomtools::cylinder cyl1(0.5, 3.0);
    geomtools::gnuplot_draw::draw_cylinder(output, placement2, cyl1);
    output << std::endl;

    // Frustrum:
    output << "# A frustrum: \n";
    pos.set(0, 0, 11);
    double phi1 = 0.35 * M_PI;
    double phi2 = 1.75 * M_PI;
    // phi1 = 0.0;
    // phi2 = 2 * M_PI;
    geomtools::create_rotation(rot, 0.0, 20. * CLHEP::degree, 30.0 * CLHEP::degree);
    geomtools::gnuplot_draw::draw_right_circular_conical_frustrum(output,
                                                                  pos, rot,
                                                                  0.5, 2.5,
                                                                  1.2, 1.7,
                                                                  2.0,
                                                                  phi1, phi2);

    if (draw_) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test geomtools::disk' ");
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      g1.cmd("set xrange [-12:+12]");
      g1.cmd("set yrange [-12:+12]");
      g1.cmd("set zrange [-2:+12]");
      g1.cmd("set xyplane at -2");
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");


      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 0 title '' with lines lw 1";
        plot_cmd << ", '' index 1 title '' with lines lw 1";
        plot_cmd << ", '' index 2 title '' with lines lw 1";
        plot_cmd << ", '' index 3 title '' with lines lw 1";
        plot_cmd << ", '' index 4 title '' with lines lw 1";
        plot_cmd << ", '' index 5 title '' with lines lw 1";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }



#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    }


    return;
  }

};

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    // bool debug = false;
    long seed = 12345;
    bool draw = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];
      // clog << "Argument=" << arg << std::endl;
      if (arg == "-D" || arg == "--draw") {
        draw = true;
      }
      if (arg == "-s" || arg == "--seed") {
        iarg++;
        if (iarg == argc_) {
          throw std::runtime_error("Missing seed value!");
        }
        std::istringstream seed_ss(argv_[iarg]);
        seed_ss >> seed;
        if (! seed_ss) {
          throw std::runtime_error("Invalid format for seed value!");
        }
        if (seed <= 0) {
          throw std::runtime_error("Invalid seed value!");
        }
      }
      iarg++;
    }

    test_app my_app(seed);
    my_app.run(draw);

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

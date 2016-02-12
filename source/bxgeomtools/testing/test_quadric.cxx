// test_quadric.cxx

// Ourselves:
#include <geomtools/quadric.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/temporary_files.h>
#include <datatools/utils.h>

// This project:
#include <geomtools/geomtools_config.h>
#include <geomtools/utils.h>
#include <geomtools/elliptical_cylinder.h>
#include <geomtools/point_on_quadric_finder.h>
#include <geomtools/foot_point_on_quadric_finder.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

void test0(bool draw_);

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'geomtools::plane' class." << std::endl;
    bool draw = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-D" || arg == "--draw") draw = true;

      iarg++;
    }

    test0(draw);

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

void test0(bool draw_)
{
  double a = 2.0;
  double b = 1.5;
  double h = 6.0;
  geomtools::elliptical_cylinder ec;
  ec.set_x_radius(a);
  ec.set_y_radius(b);
  ec.set_z(h);
  ec.initialize_simple();
  ec.tree_dump(std::clog, "Elliptic cylinder: ");

  geomtools::placement ec_placement;
  ec_placement.set_identity();

  geomtools::quadric q1;
  geomtools::quadric::make_elliptic_cylinder_along_z(q1, a, b);
  q1.tree_dump(std::clog, "Quadric: ");


  datatools::temp_file tmp_file;
  tmp_file.set_remove_at_destroy(true);
  tmp_file.create("/tmp", "test_quadric_");

  // Draw the volume:
  {
    tmp_file.out() << "# Draw solid (index 0) " << std::endl;
    geomtools::gnuplot_draw::draw_elliptical_cylinder(tmp_file.out(),
                                                      ec_placement,
                                                      ec,
                                                      geomtools::elliptical_cylinder::WR_NONE
                                                      | geomtools::elliptical_cylinder::WR_BASE_HUGE_ANGLE_SAMPLING
                                                      );
    tmp_file.out() << std::endl << std::endl;
  }
  std::clog << "Draw volume done." << std::endl;

  geomtools::vector_3d M(3.0, 5, 2.5);
  // M.set(0.001, 0, 0);
  std::clog << "M =  " << M << std::endl;
  geomtools::point_on_quadric_finder poqf(q1, 0.0001, 25, "test_quadric_S.data");
  geomtools::vector_3d S;
  if (poqf.find(M, S) != EXIT_SUCCESS) {
    std::cerr << "error: Cannot find point on quadric!" << std::endl;
  }
  std::clog << "S =  " << S << std::endl;

  geomtools::foot_point_on_quadric_finder fpoqf(q1, 0.0001, 25, 20.0, "test_quadric_H.data");
  geomtools::vector_3d H;
  if (fpoqf.find(M, H) != EXIT_SUCCESS) {
    std::cerr << "error: Cannot find foot point on quadric!" << std::endl;
  }
  std::clog << "H =  " << H << std::endl;

  tmp_file.out() << "# M: " << std::endl;
  geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), M);
  tmp_file.out() << std::endl << std::endl;

  tmp_file.out() << "# S: " << std::endl;
  geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), S);
  tmp_file.out() << std::endl << std::endl;

  tmp_file.out() << "# H: " << std::endl;
  geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), H);
  tmp_file.out() << std::endl << std::endl;

   /*
  double dist_q1M = q1.distance(M);
  std::clog << "Distance from M to q1 is : "
            << dist_q1M << std::endl;
  geomtools::vector_3d H = q1.orthogonal_projection(M);
  geomtools::vector_3d shoot_dir(0.0, 1.2, -0.5);
  geomtools::vector_3d I = q1.projection(M, shoot_dir);
  std::clog << "Distance from M to H is : "
            << (H - M).mag() << std::endl;
  std::clog << "Distance from M to I is : "
            << (I - M).mag() << std::endl;

  geomtools::face_intercept_info impact;
  geomtools::vector_3d shoot_dir2(0.0, -1.2, +0.25);
  double tolerance = 0.05;
  if (q1.find_intercept(M, shoot_dir2, impact, tolerance)) {
    std::clog << "Distance from M to impact is : "
              << (impact.get_impact() - M).mag() << std::endl;
  } else {
    std::clog << "No impact was found !" << std::endl;
  }
  */

  /*
  tmp_file.out() << "# projection & impact: " << std::endl;
  geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), H);
  geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), I);
  if (impact.is_valid()) {
    geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), impact.get_impact());
  }
  tmp_file.out() << std::endl << std::endl;

  tmp_file.out() << "# projection line: " << std::endl;
  geomtools::gnuplot_draw::draw_line(tmp_file.out(), M, H, true);
  geomtools::gnuplot_draw::draw_line(tmp_file.out(), M, I);
  if (impact.is_valid()) {
    geomtools::gnuplot_draw::draw_line(tmp_file.out(), M, impact.get_impact());
  }
  tmp_file.out() << std::endl << std::endl;

  {
    // Draw on plane (index 5):
    tmp_file.out() << "# points on surface: " << std::endl;
    size_t  nshoots = 100000;
    int counts = 0;
    for (int i = 0; i < (int) nshoots; i++) {
      geomtools::vector_3d pos((-2. + 4.* drand48()),
                               (-2. + 4.* drand48()),
                               (+3. + 4.* drand48()));
      double tolerance = 0.25;
      if (q1.is_on_surface(pos, tolerance)) {
        counts++;
        geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), pos, 3.0);
      }
    }
    if (counts == 0) {
      tmp_file.out() << "0 0 0 0.0" << std::endl;
    }
    tmp_file.out() << std::endl << std::endl;
  }

  {
    // Draw intercepts (index 6):
    tmp_file.out() << "# intercepts: " << std::endl;
    size_t nshoots = 10000;
    int counts = 0;
    for (int i = 0; i < (int) nshoots; i++) {
      geomtools::vector_3d pos((-5. + 10.* drand48()),
                               5.* drand48(),
                               (-5. + 10.* drand48()));
      geomtools::vector_3d dir;
      geomtools::randomize_direction(drand48, dir);

      double tolerance = 0.05;
      geomtools::face_intercept_info intercept;
      if (q1.find_intercept(pos, dir, intercept, tolerance)) {
        counts++;
        geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), intercept.get_impact(), 2.0);
        // Test: intercept segments
        // geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), pos);
        // geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), intercept.get_impact());
      }
    }
    if (counts == 0) {
      tmp_file.out() << "0 0 0 -1" << std::endl;
    }
    tmp_file.out() << std::endl << std::endl;
  }
  */

  if (draw_) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1

    Gnuplot g1("lines");
    g1.cmd("set title 'Test geomtools::quadric -- test0' ");
    g1.cmd("set grid");
    g1.cmd("set size ratio -1");
    g1.cmd("set view equal xyz");
    g1.cmd("set xyplane at -5");
    g1.set_xrange(-10, +10).set_yrange(-10, +10).set_zrange(-5, +10);
    g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");

    {
      std::ostringstream plot_cmd;
      plot_cmd << "splot '" << tmp_file.get_filename() << "' index 0 title 'Elliptical cylinder' with lines lt 1";
      plot_cmd << ", '' index 1 title 'M' with points lt 2 pt 7 ps 1.0 ";
      plot_cmd << ", '' index 2 title 'S' with points lt 3 pt 7 ps 1.0 ";
      plot_cmd << ", '' index 3 title 'H' with points lt 4 pt 7 ps 1.0 ";
      plot_cmd << ", 'test_quadric_S.data' index 0 title 'On surface march' with linespoints lt 5 pt 6 ps 1.0 ";
      plot_cmd << ", 'test_quadric_H.data' index 0 title 'Foot point march' with linespoints lt 6 pt 6 ps 1.0 ";
      // plot_cmd << ", '' index 2 title 'Projected points' with points lt 4 pt 7 ps 1.0 ";
      // plot_cmd << ", '' index 3 title 'Projection lines' with lines lt 5 ";
      g1.cmd(plot_cmd.str());
      g1.showonscreen(); // window output
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }

    {
      std::ostringstream plot_cmd;
      plot_cmd << "plot '" << tmp_file.get_filename() << "' index 0 title 'Elliptical cylinder' with lines lt 1";
      plot_cmd << ", '' index 1 title 'M' with points lt 2 pt 7 ps 1.0 ";
      plot_cmd << ", '' index 2 title 'S' with points lt 3 pt 7 ps 1.0 ";
      plot_cmd << ", '' index 3 title 'H' with points lt 4 pt 7 ps 1.0 ";
      plot_cmd << ", 'test_quadric_S.data' index 0 title 'On surface march' with linespoints lt 5 pt 6 ps 1.0 ";
      plot_cmd << ", 'test_quadric_H.data' index 0 title 'Foot point march' with linespoints lt 6 pt 6 ps 1.0 ";
      // plot_cmd << ", '' index 2 title 'Projected points' with points lt 4 pt 7 ps 1.0 ";
      // plot_cmd << ", '' index 3 title 'Projection lines' with lines lt 5 ";
      g1.cmd(plot_cmd.str());
      g1.showonscreen(); // window output
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }

    // {
    //   std::ostringstream plot_cmd;
    //   plot_cmd << "splot '" << tmp_file.get_filename()
    //            << "' index 0 title 'Plane p' with lines ";
    //   plot_cmd << ", '' index 4 using 1:2:3:4 title 'On surface' with points pt 6 ps 0.5 linecolor variable";
    //   g1.cmd(plot_cmd.str());
    //   g1.showonscreen(); // window output
    //   geomtools::gnuplot_drawer::wait_for_key();
    //   usleep(200);
    // }

    // bool yz = false;
    // if (yz) {
    //   Gnuplot g1("lines");
    //   g1.cmd("set title 'Test geomtools::plane -- test0' ");
    //   g1.cmd("set grid");
    //   g1.cmd("set size ratio -1");
    //   g1.set_xlabel("y").set_ylabel("z");
    //   g1.set_xrange(-10, +10).set_yrange(-10, +10).set_zrange(-10, +10);
    //   std::ostringstream plot_cmd;
    //   plot_cmd << "plot '" << tmp_file.get_filename() << "' index 0 using 2:3 title 'Plane p' with lines ,";
    //   plot_cmd << " '' index 1 using 2:3 notitle with points lt 3 pt 7 ps 1.0 ,";
    //   plot_cmd << " '' index 2 using 2:3 notitle with lines lt 2 ";
    //   g1.cmd(plot_cmd.str());
    //   //g1.plotfile_xyz (tmp_file.get_filename (), 1, 2, 3, "3D view");
    //   g1.showonscreen(); // window output
    //   geomtools::gnuplot_drawer::wait_for_key();
    //   usleep(200);
    // }

    // {
    //   std::ostringstream plot_cmd;
    //   plot_cmd << "splot '" << tmp_file.get_filename()
    //            << "' index 0 title 'Plane p' with lines ";
    //   plot_cmd << ", '' index 6 using 1:2:3:4 title 'Intercepts'     with points pt 6 ps 0.5 linecolor variable";
    //   g1.cmd(plot_cmd.str());
    //   g1.showonscreen(); // window output
    //   geomtools::gnuplot_drawer::wait_for_key();
    //   usleep(200);
    // }

#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
  }

  return;
}

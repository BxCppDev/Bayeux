// test_plane.cxx

// Ourselves:
#include <geomtools/plane.h>

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
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

void test0 ( bool draw_ )
{
  geomtools::plane my_plane;
  geomtools::vector_3d A(1, 2, 0.5);
  geomtools::vector_3d pN(0.0, 2.0, 1.0);
  my_plane.set(A, pN);

  geomtools::vector_3d B(3, 5, 2.5);
  double dist_pB = my_plane.distance(B);
  std::clog << "Distance from B to p is : "
            << dist_pB << std::endl;
  geomtools::vector_3d H = my_plane.orthogonal_projection(B);
  geomtools::vector_3d shoot_dir(0.0, 1.2, -0.5);
  geomtools::vector_3d I = my_plane.projection(B, shoot_dir);
  std::clog << "Distance from B to H is : "
            << (H - B).mag() << std::endl;
  std::clog << "Distance from B to I is : "
            << (I - B).mag() << std::endl;

  geomtools::face_intercept_info impact;
  geomtools::vector_3d shoot_dir2(0.0, -1.2, +0.25);
  double tolerance = 0.05;
  if (my_plane.find_intercept(B, shoot_dir2, impact, tolerance)) {
    std::clog << "Distance from B to impact is : "
              << (impact.get_impact() - B).mag() << std::endl;
  } else {
    std::clog << "No impact was found !" << std::endl;
  }

  datatools::temp_file tmp_file;
  tmp_file.set_remove_at_destroy(true);
  tmp_file.create("/tmp", "test_plane_");

  tmp_file.out() << "# grid: " << std::endl;
  my_plane.print_grid(tmp_file.out(), 1.0, 5, 5);

  tmp_file.out() << "# A: " << std::endl;
  geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), A);
  tmp_file.out() << std::endl << std::endl;

  tmp_file.out() << "# B: " << std::endl;
  geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), B);
  tmp_file.out() << std::endl << std::endl;

  tmp_file.out() << "# projection & impact: " << std::endl;
  geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), H);
  geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), I);
  if (impact.is_valid()) {
    geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), impact.get_impact());
  }
  tmp_file.out() << std::endl << std::endl;

  tmp_file.out() << "# projection line: " << std::endl;
  geomtools::gnuplot_draw::draw_line(tmp_file.out(), B, H, true);
  geomtools::gnuplot_draw::draw_line(tmp_file.out(), B, I);
  if (impact.is_valid()) {
    geomtools::gnuplot_draw::draw_line(tmp_file.out(), B, impact.get_impact());
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
      if (my_plane.is_on_surface(pos, tolerance)) {
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
      if (my_plane.find_intercept(pos, dir, intercept, tolerance)) {
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

  if (draw_) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1

    Gnuplot g1("lines");
    g1.cmd("set title 'Test geomtools::plane -- test0' ");
    g1.cmd("set grid");
    g1.cmd("set size ratio -1");
    g1.cmd("set view equal xyz");
    g1.cmd("set xyplane at -5");
    g1.set_xrange(-10, +10).set_yrange(-10, +10).set_zrange(-5, +10);
    g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");

    {
      std::ostringstream plot_cmd;
      plot_cmd << "splot '" << tmp_file.get_filename() << "' index 0 title 'Plane p' with lines ";
      plot_cmd << ", '' index 1 title 'A' with points lt 2 pt 7 ps 1.0 ";
      plot_cmd << ", '' index 2 title 'B' with points lt 3 pt 7 ps 1.0 ";
      plot_cmd << ", '' index 3 title 'Projected points' with points lt 4 pt 7 ps 1.0 ";
      plot_cmd << ", '' index 4 title 'Projection lines' with lines lt 5 ";
      g1.cmd(plot_cmd.str());
      g1.showonscreen(); // window output
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }

    {
      std::ostringstream plot_cmd;
      plot_cmd << "splot '" << tmp_file.get_filename()
               << "' index 0 title 'Plane p' with lines ";
      plot_cmd << ", '' index 5 using 1:2:3:4 title 'On surface' with points pt 6 ps 0.5 linecolor variable";
      g1.cmd(plot_cmd.str());
      g1.showonscreen(); // window output
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }

    bool yz = false;
    if (yz) {
      Gnuplot g1("lines");
      g1.cmd("set title 'Test geomtools::plane -- test0' ");
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.set_xlabel("y").set_ylabel("z");
      g1.set_xrange(-10, +10).set_yrange(-10, +10).set_zrange(-10, +10);
      std::ostringstream plot_cmd;
      plot_cmd << "plot '" << tmp_file.get_filename() << "' index 0 using 2:3 title 'Plane p' with lines ,";
      plot_cmd << " '' index 1 using 2:3 notitle with points lt 3 pt 7 ps 1.0 ,";
      plot_cmd << " '' index 2 using 2:3 notitle with lines lt 2 ";
      g1.cmd(plot_cmd.str());
      //g1.plotfile_xyz (tmp_file.get_filename (), 1, 2, 3, "3D view");
      g1.showonscreen(); // window output
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }

    {
      std::ostringstream plot_cmd;
      plot_cmd << "splot '" << tmp_file.get_filename()
               << "' index 0 title 'Plane p' with lines ";
      plot_cmd << ", '' index 6 using 1:2:3:4 title 'Intercepts'     with points pt 6 ps 0.5 linecolor variable";
      g1.cmd(plot_cmd.str());
      g1.showonscreen(); // window output
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }

#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
  }

  return;
}

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

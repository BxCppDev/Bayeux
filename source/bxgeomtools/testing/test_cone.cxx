// test_cone.cxx

// Ourselves:
#include <geomtools/cone.h>
#include <geomtools/right_circular_conical_nappe.h>
#include <geomtools/right_circular_conical_frustrum.h>

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
#include <geomtools/placement.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/circle.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

int main (int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'cone'!" << std::endl;

    bool draw = false;
    bool do_identity = false;
    bool do_sector = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-I" || arg == "--identity") do_identity = true;
      if (arg == "-S" || arg == "--sector") do_sector = true;

      iarg++;
    }

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_cone_");

    double z1 = -1.0;
    double z2 = +1.0;
    double r1 = 3.0;
    double r2 = 2.0;

    {
      std::clog << "Test primitives : " << std::endl;
      double surface = geomtools::cone::compute_frustrum_lateral_surface(z1, z2, r1, r2);
      std::clog << " - Conical frustrum surface : " << surface << std::endl;
      double volume = geomtools::cone::compute_frustrum_volume(z1, z2, r1, r2);
      std::clog << " - Conical frustrum volume  : " << volume << std::endl << std::endl;
    }

    geomtools::right_circular_conical_nappe my_rccn(r1, r2, z2 - z1);
    double br1 = 2.25;
    double br2 = r1;
    double tr1 = 1.5;
    double tr2 = r2;
    geomtools::right_circular_conical_frustrum my_rccf(br1, br2, tr1, tr2, z2 - z1);
    if (do_sector) {
      my_rccn.set_start_angle(M_PI / 4);
      my_rccn.set_delta_angle(M_PI);
      my_rccf.set_start_angle(M_PI / 4);
      my_rccf.set_delta_angle(M_PI);
    }
    my_rccn.tree_dump(std::clog, "RCCN:");
    double rccn_surface = my_rccn.get_surface();
    std::clog << "Conical nappe surface : " << rccn_surface << std::endl;
    my_rccf.tree_dump(std::clog, "RCCF:");
    double rccf_surface = my_rccf.get_surface();
    std::clog << "Conical frustrum surface : " << rccf_surface << std::endl;
    double rccf_volume = my_rccf.get_volume();
    std::clog << "Conical frustrum volume : " << rccf_volume << std::endl;

    geomtools::placement rccn_placement(0.2, 0.15, 0.1, M_PI / 7.0, M_PI / 6.0, 0.0);
    if (do_identity) {
      rccn_placement.set_identity();
    }

    {
      tmp_file.out() << "# conical nappe (index 0): " << std::endl;
      geomtools::gnuplot_draw::draw_right_circular_conical_nappe(tmp_file.out(),
                                                                 rccn_placement,
                                                                 my_rccn,
                                                                 geomtools::i_wires_3d_rendering::WR_NONE
                                                                 | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                                                                 | geomtools::right_circular_conical_nappe::WR_RCCN_NO_BOTTOM_EDGE
                                                                 // | geomtools::right_circular_conical_nappe::WR_RCCN_NO_TOP_EDGE
                                                                 // | geomtools::right_circular_conical_nappe::WR_RCCN_NO_START_ANGLE_EDGE
                                                                 | geomtools::right_circular_conical_nappe::WR_RCCN_NO_STOP_ANGLE_EDGE
                                                                 );
      tmp_file.out() << std::endl << std::endl;
    }

    {
      tmp_file.out() << "# conical nappe (index 1): " << std::endl;
      geomtools::gnuplot_draw::draw_right_circular_conical_nappe(tmp_file.out(),
                                                                 rccn_placement,
                                                                 my_rccn,
                                                                 geomtools::i_wires_3d_rendering::WR_NONE
                                                                 // | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                                                                 | geomtools::i_wires_3d_rendering::WR_BASE_LOW_ANGLE_SAMPLING
                                                                 // | geomtools::right_circular_conical_nappe::WR_RCCN_NO_BOTTOM_EDGE
                                                                 | geomtools::right_circular_conical_nappe::WR_RCCN_NO_TOP_EDGE
                                                                 // | geomtools::right_circular_conical_nappe::WR_RCCN_NO_START_ANGLE_EDGE
                                                                 // | geomtools::right_circular_conical_nappe::WR_RCCN_NO_STOP_ANGLE_EDGE
                                                                 );
      tmp_file.out() << std::endl << std::endl;
    }

    {
      tmp_file.out() << "# conical frustrum (index 2): " << std::endl;
      geomtools::gnuplot_draw::draw_right_circular_conical_frustrum(tmp_file.out(),
                                                                    rccn_placement,
                                                                    my_rccf,
                                                                    geomtools::i_wires_3d_rendering::WR_NONE
                                                                    | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                                                                    | geomtools::i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY
                                                                    // | geomtools::i_wires_3d_rendering::WR_BASE_LOW_ANGLE_SAMPLING
                                                                    // | geomtools::right_circular_conical_frustrum::WR_RCCF_NO_BOTTOM_FACE
                                                                    | geomtools::right_circular_conical_frustrum::WR_RCCF_NO_TOP_FACE
                                                                    | geomtools::right_circular_conical_frustrum::WR_RCCF_NO_INNER_FACE
                                                                    // | geomtools::right_circular_conical_frustrum::WR_RCCF_NO_OUTER_FACE
                                                                    // | geomtools::right_circular_conical_frustrum::WR_RCCF_NO_START_ANGLE_FACE
                                                                    // | geomtools::right_circular_conical_frustrum::WR_RCCF_NO_STOP_ANGLE_FACE
                                                                 );
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw intercepts (index 1):
      tmp_file.out() << "# intercepts " << std::endl;
      size_t nshoots = 10000;
      int counts = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos(2. * drand48() * CLHEP::mm,
                                 2. * drand48() * CLHEP::mm,
                                 -2. + 4. * drand48() * CLHEP::mm);
        geomtools::vector_3d dir;
        geomtools::randomize_direction(drand48, dir);
        geomtools::vector_3d pos_c;
        rccn_placement.mother_to_child(pos, pos_c);
        geomtools::vector_3d dir_c;
        rccn_placement.mother_to_child_direction(dir, dir_c);
        geomtools::face_intercept_info intercept_c;
        if (my_rccn.find_intercept(pos_c, dir_c, intercept_c)) {
          counts++;
          geomtools::vector_3d impact;
          rccn_placement.child_to_mother(intercept_c.get_impact(), impact);
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), impact, 3.0);
        }
      }
      if (counts == 0) {
        tmp_file.out() << "0 0 0 -1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
    }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test geomtools::cone' ");
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      g1.cmd("set xrange [-6:+6]");
      g1.cmd("set yrange [-6:+6]");
      g1.cmd("set zrange [-8:+8]");
      g1.cmd("set xyplane at -8");
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename() << "' index 0 title 'Conical nappe' with lines ";
        plot_cmd << ", '' index 3 using 1:2:3:4 title 'Intercepts' with points pt 6 ps 0.75 linecolor variable";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename() << "' index 1 title 'Conical nappe' with lines ";

        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename() << "' index 2 title 'Conical frustrum' with lines";
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

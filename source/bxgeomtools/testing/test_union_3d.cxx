// test_union_3d.cxx

// Ourselves:
#include <geomtools/union_3d.h>

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
#include <geomtools/box.h>
#include <geomtools/cylinder.h>
#include <geomtools/tube.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

int main (int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    bool debug = false;
    long seed = 314159;
    bool interactive = false;
    bool draw = false;
    bool zero = false;
    bool center2 = false;
    bool surf_first = false;
    bool surf_second = false;
    bool bulk = false;
    bool cyl1 = false;
    bool cyl2 = false;
    bool tub2 = false;

    int iarg = 1;
    while (iarg < argc_) {
      string arg = argv_[iarg];

      if (arg == "-d" || arg == "--debug") debug = true;
      if (arg == "-i" || arg == "--interactive") interactive = true;
      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-z" || arg == "--zero") zero = true;
      if (arg == "-C1") cyl1 = true;
      if (arg == "-c2" || arg == "--center2") center2 = true;

      if (arg == "-s1" || arg == "--surf1") surf_first = true;
      if (arg == "-s2" || arg == "--surf2") surf_second = true;
      if (arg == "-B" || arg == "--bulk") bulk = true;
      if (arg == "-C2") {
        cyl2 = true;
        tub2=false;
      }
      if (arg == "-T2") {
        tub2 = true;
        cyl2 = false;
      }
      iarg++;
    }

    srand48 (seed);

    using namespace geomtools;

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy (true);
    tmp_file.create ("/tmp", "test_union_3d_");

    box b1 (4.0 * CLHEP::mm,
            4.0 * CLHEP::mm,
            4.0 * CLHEP::mm);
    b1.lock();
    b1.tree_dump (std::clog, "Box 1:");
    placement p1 (vector_3d (0, 0, 0),
                  M_PI / 6.0, 0, 0);

    cylinder c1 (2.0 * CLHEP::mm,
                 4.0 * CLHEP::mm);
    c1.lock();

    box b2 (2.0 * CLHEP::mm,
            2.0 * CLHEP::mm,
            2.0 * CLHEP::mm);
    b2.lock();
    if (!cyl2) {
      b2.tree_dump (std::clog, "Box 2:");
    }

    cylinder c2 (2.0 * CLHEP::mm,
                 2.0 * CLHEP::mm);
    c2.lock();
    if (cyl2) {
      c2.tree_dump (clog, "Cylinder 2:");
    }

    tube t2 (1.0 * CLHEP::mm,
             2.0 * CLHEP::mm,
             2.0 * CLHEP::mm);
    t2.lock();
    if (tub2) {
      t2.tree_dump (clog, "Tube 2:");
    }
    placement p2 (vector_3d (2, 2, 2),
                  M_PI / 3., M_PI / 3., M_PI / 6.);

    union_3d u1;
    if (cyl1) {
      u1.set_shape1(c1, p1);
    } else {
      u1.set_shape1(b1, p1);
    }

    if (tub2) {
      u1.set_shape2(t2, p2);
    } else if (!cyl2) {
      u1.set_shape2(b2, p2);
    } else {
      u1.set_shape2(c2, p2);
    }
    u1.lock();
    u1.tree_dump (clog, "Union:");

    box ubb;
    placement ubbp;
    u1.get_bounding_data().compute_bounding_box(ubb, ubbp);

    if (interactive) {
      char c;
      cin >> c;
    }

    geomtools::vector_3d pos (4.0 * CLHEP::mm,
                              3.0 * CLHEP::mm,
                              6.0 * CLHEP::mm);
    geomtools::vector_3d dir (-1., -1., -1.);
    geomtools::face_intercept_info intercept;

    double tolerance = 0.2;
    if (!tub2) {
      if (u1.find_intercept (pos, dir, intercept, tolerance)) {
        clog << "test 1: Intercept face=" << intercept.get_face_id ()
             << " at impact=" << intercept.get_impact ()
             << std::endl;
      } else {
        clog << "test 1: No intercept." << std::endl;
      }
    }

    tmp_file.out() << "# Volumes (index 0):" << std::endl;
    if (!cyl1) {
      geomtools::gnuplot_draw::draw_box(tmp_file.out(),
                                        p1.get_translation(),
                                        p1.get_rotation(),
                                        b1.get_x(),
                                        b1.get_y(),
                                        b1.get_z());
    } else {
      geomtools::gnuplot_draw::draw_cylinder(tmp_file.out(),
                                             p1.get_translation(),
                                             p1.get_rotation(),
                                             c1.get_r(),
                                             c1.get_z());

    }
    if (tub2) {
      geomtools::gnuplot_draw::draw_tube(tmp_file.out(),
                                         p2.get_translation(),
                                         p2.get_rotation(),
                                         t2);
    } else if (!cyl2) {
      geomtools::gnuplot_draw::draw_box(tmp_file.out(),
                                        p2.get_translation(),
                                        p2.get_rotation(),
                                        b2.get_x(),
                                        b2.get_y(),
                                        b2.get_z());
    } else {
      geomtools::gnuplot_draw::draw_cylinder(tmp_file.out(),
                                             p2.get_translation(),
                                             p2.get_rotation(),
                                             c2.get_r(),
                                             c2.get_z());
    }
    tmp_file.out() << std::endl;
    // tmp_file.out() << std::endl << std::endl;

    tmp_file.out() << "# From (index 1):" << std::endl;
    geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), pos);
    tmp_file.out() << std::endl << std::endl;

    if (!tub2) {
      tmp_file.out() << "# To (index 2):" << std::endl;
      geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), intercept.get_impact());
      tmp_file.out() << std::endl << std::endl;

      tmp_file.out() << "# Track (index 3):" << std::endl;
      geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), pos);
      geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), intercept.get_impact());
      tmp_file.out() << std::endl << std::endl;
    } else {
      tmp_file.out() << "0 0 0 1" << std::endl;
      tmp_file.out() << std::endl << std::endl;
      tmp_file.out() << "0 0 0 1" << std::endl;
      tmp_file.out() << "0 0 0 1" << std::endl;
      tmp_file.out() << std::endl << std::endl;
    }

    clog << "test 1: End." << std::endl;

    tmp_file.out() <<"# Intercept (index 4):" << std::endl;
    size_t nshoots = 50000;
    for (int i = 0; i < (int) nshoots; i++) {
      if ((i%10000) == 0) clog << "Loop #" << i << std::endl;

      double dim = 6. * CLHEP::mm;
      geomtools::vector_3d local_pos(0, 0, 0);
      if (! zero) local_pos.set(dim * drand48(),
                          dim * drand48(),
                          dim * drand48());
      if (center2) local_pos.set(2. * CLHEP::mm, 2. * CLHEP::mm, 2. * CLHEP::mm);

      geomtools::vector_3d local_dir;
      geomtools::randomize_direction(drand48, local_dir);

      if (!tub2) {
        geomtools::face_intercept_info local_intercept;
        double local_tolerance = 0.2;
        if (u1.find_intercept(local_pos, local_dir, local_intercept, local_tolerance)) {
          if (debug) clog << "test 1: Intercept face="
                          << local_intercept.get_face_id()
                          << " at impact="
                          << local_intercept.get_impact()
                          << std::endl;
          geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(),
                                                    local_intercept.get_impact(),
                                                    false);
          int face = local_intercept.get_face_id().get_face_bits();
          tmp_file.out() << ' ' << face;
          tmp_file.out() << std::endl;
        } else {
          if (debug) clog << "test 1: No intercept." << std::endl;
        }
      } else {
        tmp_file.out() << "0 0 0 0" << std::endl;
      }
    }
    tmp_file.out() << std::endl << std::endl;

    tmp_file.out() <<"# Vertexes (index 5):" << std::endl;
    // nshoots = 10000000;
    nshoots = 1000000;
    int count = 0;
    for (int i = 0; i < (int) nshoots; i++)  {
      double dim = 6. * CLHEP::mm;
      geomtools::vector_3d position(0, 0, 0);
      position.set(dim * (-1 + 2 * drand48()),
                   dim * (-1 + 2 * drand48()),
                   dim * (-1 + 2 * drand48()));
      double skin = 0.1 * CLHEP::mm;
      geomtools::face_identifier fid_first;
      fid_first.prepend_part(geomtools::union_3d::FIRST_PART);
      fid_first.set_face_bits_any();
      geomtools::face_identifier fid_second;
      fid_second.prepend_part(geomtools::union_3d::SECOND_PART);
      fid_second.set_face_bits_any();
      if (surf_first && u1.is_on_surface(position, fid_first, skin)) {
        tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                       << 3
                       << std::endl;
        count++;
      } else if (surf_second && u1.is_on_surface(position, fid_second, skin)) {
        tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                       << 4
                       << std::endl;
        count++;
      } else if (bulk && u1.check_inside(position, skin)) {
        tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                       << 2
                       << std::endl;
        count++;
      }
    }
    if (!count) {
      tmp_file.out() << "0 0 0 1" << std::endl;
    }
    tmp_file.out() << std::endl << std::endl;

    ubb.tree_dump(std::clog, "Union BB: ", "NOTICE: " );
    ubbp.tree_dump(std::clog, "Union BB placement: ", "NOTICE: " );
    tmp_file.out() <<"# Bounding box (index 6):" << std::endl;
    geomtools::gnuplot_draw::draw_box(tmp_file.out(),
                                      ubbp.get_translation(),
                                      ubbp.get_rotation(),
                                      ubb.get_x(),
                                      ubb.get_y(),
                                      ubb.get_z());
    tmp_file.out() << std::endl << std::endl;

    tmp_file.out() <<"# Union (index 7):" << std::endl;
    geomtools::gnuplot_draw::draw(tmp_file.out(),
                                  geomtools::placement(0,0,0,0,0,0),
                                  u1,
                                  geomtools::union_3d::WR_NONE
                                  | geomtools::union_3d::WR_COMPOSITE_BOOST_SAMPLING
                                  );
    tmp_file.out() << std::endl << std::endl;

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test geomtools::union_3d' ");
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      // g1.cmd("set surface explicit");
      //g1.cmd("set xyplane at -10");
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 0 title 'Volumes' with lines ";
        plot_cmd << ", '' index 1 title 'Source' with points pt 6 ps 1 ";
        plot_cmd << ", '' index 2 title 'Impact' with points pt 6 ps 1 ";
        plot_cmd << ", '' index 3 title 'Track' with lines ";
        plot_cmd << ", '' index 6 title 'BB' with lines lt 0";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 0 title 'Volumes' with lines ";
        plot_cmd << ", '' index 4 title 'Impacts' with dots ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 7 title 'Union' with lines ";
        plot_cmd << ", '' index 5 title 'Vertexes' with point pt 6 ps 0.35 linecolor variable";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 7 title 'Union' with lines ";
        plot_cmd << ", '' index 6 title 'BB' with lines lt -1";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }
#endif
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

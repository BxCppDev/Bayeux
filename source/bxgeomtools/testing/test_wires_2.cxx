// test_wires_2.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <list>

// Third party:
// - Bayeux/datatools:
#include <datatools/temporary_files.h>
#include <datatools/utils.h>
#include <datatools/clhep_units.h>

// This project:
#include <geomtools/i_wires_3d_rendering.h>
#include <geomtools/polyline_3d.h>
#include <geomtools/placement.h>
#include <geomtools/utils.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/line_3d.h>
#include <geomtools/box.h>
#include <geomtools/cylinder.h>
#include <geomtools/rectangle.h>
#include <geomtools/circle.h>
#include <geomtools/regular_polygon.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'geomtools::i_wires_3d_rendering' tools." << std::endl;

    bool draw = false;
    bool delta_z = false;
    bool horiz = false;
    bool cyl = true;
    bool plot_in = false;
    bool plot_out = false;
    bool plot_surf = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];
      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-Z") delta_z = true;
      if (arg == "-H") horiz = true;
      if (arg == "-C") cyl = false;
      if (arg == "-pi") plot_in = true;
      if (arg == "-po") plot_out = true;
      if (arg == "-ps") plot_surf = true;
     iarg++;
    }

    geomtools::box b1;
    b1.set(2.0, 2.0, 2.0);
    b1.lock();

    geomtools::cylinder c1;
    c1.set(1.0, 2.0);
    c1.lock();

    geomtools::cylinder ci2;
    ci2.set(1.0, 2.0);
    ci2.lock();

    // geomtools::circle ci2(1.0);
    // geomtools::rectangle ci2(2.0, 2.0);
    // geomtools::regular_polygon ci2(8, 1.0);

    const geomtools::i_shape_3d * sh1 = &b1;
    if (cyl) {
      sh1 = &c1;
    }

    geomtools::placement up1;
    up1.set_identity();

    geomtools::placement up2(1.0, 0.5, 0.5, 0.0, 0.0, 0.0);

    geomtools::segment_type segment1;
    double z1 = 0.0;
    double z2 = 0.0;
    double eps = 0.002;
    eps = 0.0;
    if (horiz) {
      z1 += (1.0 - eps);
      z2 += (1.0 - eps);
    } else if (delta_z) {
      z1 += (1.0);
      z2 -= (1.0);
    }
    segment1.first = geomtools::vector_3d(-2.0, 1.0, z1);
    segment1.second = geomtools::vector_3d(+4.0, -0.5, z2);

    // A collection of polylines:
    geomtools::wires_type wires;
    uint32_t wopts = geomtools::i_wires_3d_rendering::WR_NONE
      | geomtools::i_wires_3d_rendering::WR_BASE_GRID
      // | geomtools::i_wires_3d_rendering::WR_BASE_LOW_ANGLE_SAMPLING
      | geomtools::i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY
      ;
    ci2.generate_wires(wires, up2, wopts);

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_wires_2_");

    // Boxes/cylinder:  (index 0):
    {
      tmp_file.out() << "# Box/cylinder (index 0):" << std::endl;
      geomtools::gnuplot_draw::basic_draw(tmp_file.out(), up1, *sh1);
      tmp_file.out() << std::endl;
    }

    // Segment: segment1 (index 1):
    tmp_file.out() << "# Segment (index 1):" << std::endl;
    geomtools::gnuplot_draw::draw_line(tmp_file.out(), segment1);
    tmp_file.out() << std::endl;

    std::list<geomtools::classified_segment> csegments;
    geomtools::classify_in_out_segment(segment1, *sh1, up1, 0.05, 0.0, csegments);
    std::clog << "Number of segments: " << csegments.size() << std::endl;

    // Out segments (index 2):
    int out_count = 0;
    tmp_file.out() << "# Out segments (index 2):" << std::endl;
    for (std::list<geomtools::classified_segment>::const_iterator i = csegments.begin();
         i != csegments.end();
         i++) {
      if (i->domain == geomtools::SHAPE_DOMAIN_OUTSIDE) {
        out_count++;
        geomtools::segment_type l;
        i->to_line(l);
        geomtools::gnuplot_draw::basic_draw_segment(tmp_file.out(), l, i == csegments.begin());
      }
    }
    if (out_count) tmp_file.out() << std::endl;

    // In segments (index 3):
    int in_count = 0;
    tmp_file.out() << "# In segments (index 3):" << std::endl;
    for (std::list<geomtools::classified_segment>::const_iterator i = csegments.begin();
         i != csegments.end();
         i++) {
      if (i->domain == geomtools::SHAPE_DOMAIN_INSIDE) {
        in_count++;
        geomtools::segment_type l;
        i->to_line(l);
        geomtools::gnuplot_draw::basic_draw_segment(tmp_file.out(), l, i == csegments.begin());
      }
    }
    if (in_count) tmp_file.out() << std::endl;

    // Surf segments (index 4):
    int surf_count = 0;
    tmp_file.out() << "# Surf segments (index 4):" << std::endl;
    for (std::list<geomtools::classified_segment>::const_iterator i = csegments.begin();
         i != csegments.end();
         i++) {
      if (i->domain == geomtools::SHAPE_DOMAIN_ON_SURFACE) {
        surf_count++;
        geomtools::segment_type l;
        i->to_line(l);
        geomtools::gnuplot_draw::basic_draw_segment(tmp_file.out(), l, i == csegments.begin());
      }
    }
    if (surf_count) tmp_file.out() << std::endl;

    // Cylinder 2 (index 5):
    {
      tmp_file.out() << "# Wires cylinder 2 (index 5):" << std::endl;
      geomtools::gnuplot_draw::basic_draw_wires(tmp_file.out(), wires);
      tmp_file.out() << std::endl;
    }

    geomtools::split_segment_wrt_shape sss2;
    sss2.configure(*sh1, up1, 0.01, 0.0001);
    sss2.add(wires);
    geomtools::wires_type in_wires;
    geomtools::wires_type out_wires;
    sss2.fetch_inside_wires(in_wires);
    sss2.fetch_outside_wires(out_wires);
    // In wires 2:
    {
      tmp_file.out() << "# In wires from cylinder 2 (index 5):" << std::endl;
      geomtools::gnuplot_draw::basic_draw_wires(tmp_file.out(), in_wires);
      tmp_file.out() << std::endl;
    }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test geomtools::test_wires_2' ");
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      g1.cmd("set xyplane at -1.5");
      g1.cmd("set xrange [-2:+5]");
      g1.cmd("set yrange [-5:+5]");
      g1.cmd("set zrange [-2:+5]");
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");

      {
        std::ostringstream plot_cmd;
        int index = 0;
        plot_cmd << "splot ";
        {
          plot_cmd << (index?",":"") << " '" << tmp_file.get_filename()
                   << "' index " << index << " title 'Box/Cylinder' with lines lt 5";
          index++;
        }
        {
          plot_cmd << (index?",":"") <<  " '" << tmp_file.get_filename()
                   << "' index " << index << " title 'Segment' with lines lt 6";
          index++;
        }
        if (out_count) {
          if (plot_out) {
            plot_cmd << (index?",":"")  << " '" << tmp_file.get_filename()
                     << "' index " << index << " title 'Out segments' with lines lt 1 lw 3";
          }
          index++;
        }
        if (in_count) {
          if (plot_in) {
            plot_cmd << (index?",":"")  << " '" << tmp_file.get_filename()
                     << "' index " << index << " title 'In segments' with lines lt 2 lw 3";
          }
          index++;
        }
        if (surf_count) {
          if (plot_surf) {
            plot_cmd << (index?",":"") << " '" << tmp_file.get_filename()
                     << "' index " << index << " title 'Surf segments' with lines lt 3 lw 3";
          }
          index++;
        }
        {

          plot_cmd << (index?",":"") << " '" << tmp_file.get_filename()
                   << "' index " << index << " title 'Wires cylinder 2' with lines lt 4";
          index++;

          plot_cmd << (index?",":"") << " '" << tmp_file.get_filename()
                   << "' index " << index << " title 'In wires' with lines lt 6 lw 1";
          index++;
        }
        std::cerr << "Gnuplot command = '" << plot_cmd.str() << "'" << std::endl;
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    }

    std::clog << "The end." << std::endl;
  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

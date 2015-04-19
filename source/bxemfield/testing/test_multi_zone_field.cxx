// test_multi_zone_field.cxx

// Ourselves:
#include <emfield/multi_zone_field.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/temporary_files.h>
#include <datatools/utils.h>
// - Bayeux/geomtools:
#include <geomtools/geomtools_config.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/box.h>
#include <geomtools/sphere.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

// This package:
#include <emfield/uniform_electric_field.h>

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'emfield::multi_zone_field' class." << std::endl;

    bool draw = false;

    {
      int iarg = 1;
      while (iarg < argc_) {
        std::string arg = argv_[iarg];
        if (arg == "-D" || arg == "--draw") draw = true;
        iarg++;
      }
    }

    emfield::base_electromagnetic_field::field_dict_type fields;


    emfield::base_electromagnetic_field::handle_type uef0_h(new emfield::uniform_electric_field);

    {
      emfield::uniform_electric_field & uef0
        = dynamic_cast<emfield::uniform_electric_field &>(uef0_h.grab());
      geomtools::vector_3d E0(0.0, 0.0, 2.0 * CLHEP::volt/CLHEP::mm);
      uef0.set_uniform_electric_field(E0);
      uef0.initialize_simple();
    }
    fields["E0"] = uef0_h;


    emfield::base_electromagnetic_field::handle_type uef1_h(new emfield::uniform_electric_field);
    {
      emfield::uniform_electric_field & uef1
        = dynamic_cast<emfield::uniform_electric_field &>(uef1_h.grab());
      geomtools::vector_3d E1(0.5 * CLHEP::volt/CLHEP::mm, 0.25 * CLHEP::volt/CLHEP::mm, 1.0 * CLHEP::volt/CLHEP::mm);
      uef1.set_uniform_electric_field(E1);
      uef1.initialize_simple();
    }
    fields["E1"] = uef1_h;


    emfield::base_electromagnetic_field::handle_type uef2_h(new emfield::uniform_electric_field);
    {
      emfield::uniform_electric_field & uef2
        = dynamic_cast<emfield::uniform_electric_field &>(uef2_h.grab());
      geomtools::vector_3d E2(1.0 * CLHEP::volt/CLHEP::mm, 0.0, 0.5 * CLHEP::volt/CLHEP::mm);
      uef2.set_uniform_electric_field(E2);
      uef2.initialize_simple();
    }
    fields["E2"] = uef2_h;


    emfield::multi_zone_field mzef;

    geomtools::placement z0_pos(-3.0 * CLHEP::mm, 0.0 * CLHEP::mm, 0.0,
                                 0.0, 0.0, 30.0 * CLHEP::degree);
    geomtools::sphere z0_shape(2.0 * CLHEP::mm);
    z0_shape.lock();
    double z0_skin = 0.01 * CLHEP::mm;
    mzef.add_zone_field("Z0", z0_pos, z0_shape, z0_skin, uef0_h.get(), true);

    geomtools::placement z1_pos(0.0 * CLHEP::mm, 1.0 * CLHEP::mm,
                                0.0, 0.0, 0.0, 20.0 * CLHEP::degree);
    geomtools::box z1_shape(5.0 * CLHEP::mm, 3.0 * CLHEP::mm, 3.0 * CLHEP::mm);
    z1_shape.lock();
    double z1_skin = 0.01 * CLHEP::mm;
    mzef.add_zone_field("Z1", z1_pos, z1_shape, z1_skin, uef1_h.get(), true);

    geomtools::placement z2_pos(+3.5 * CLHEP::mm, 2.0 * CLHEP::mm,
                                0.0, 0.0, 0.0, 0.0);
    geomtools::sphere z2_shape(2.5 * CLHEP::mm);
    z2_shape.lock();
    double z2_skin = 0.01 * CLHEP::mm;
    mzef.add_zone_field("Z2", z2_pos, z2_shape, z2_skin, uef2_h.get(), true);

    mzef.initialize_simple();
    mzef.tree_dump(std::clog, "MZEF: ");

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_emfield_multi_zone_field_");

    geomtools::gnuplot_draw::draw(tmp_file.out(), z0_pos, z0_shape, 0);
    tmp_file.out() << std::endl;
    tmp_file.out() << std::endl;

    geomtools::gnuplot_draw::draw(tmp_file.out(), z1_pos, z1_shape, 0);
    tmp_file.out() << std::endl;
    tmp_file.out() << std::endl;

    geomtools::gnuplot_draw::draw(tmp_file.out(), z2_pos, z2_shape, 0);
    tmp_file.out() << std::endl;
    tmp_file.out() << std::endl;

    double time = 0.0 * CLHEP::nanosecond;
    geomtools::vector_3d position;
    geomtools::vector_3d E;
    for (double x = -4 * CLHEP::mm; x <= +4 * CLHEP::mm; x += 0.5 * CLHEP::mm) {
      for (double y = -4 * CLHEP::mm; y <= +4 * CLHEP::mm; y += 0.5 * CLHEP::mm) {
        position.set(x, y, 0.0);
        mzef.compute_electric_field(position, time, E);
        tmp_file.out() << position.x() / CLHEP::mm << ' '
                       << position.y() / CLHEP::mm << ' '
                       << position.z() / CLHEP::mm << ' '
                       << E.x() / (CLHEP::volt/CLHEP::mm) << ' '
                       << E.y() / (CLHEP::volt/CLHEP::mm) << ' '
                       << E.z() / (CLHEP::volt/CLHEP::mm) << ' '
                       << std::endl;
      }
    }
    tmp_file.out() << std::endl;

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test emfield::multi_zone_field' ");
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      g1.cmd("set xrange [-4:+4]");
      g1.cmd("set yrange [-4:+4]");
      g1.cmd("set zrange [-4:+4]");
      g1.set_xlabel("x (mm)").set_ylabel("y (mm)").set_zlabel("E (V/mm)");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename() << "' "
                 << " i 3 u 1:2:3:4:5:6 notitle 'Multi zone electric field' with vectors lw 2 ";
        plot_cmd << ",  '" << tmp_file.get_filename() << "' "
                 << " i 0 title 'Zone 0' with lines";
        plot_cmd << ",  '" << tmp_file.get_filename() << "' "
                 << " i 1 title 'Zone 1' with lines";
        plot_cmd << ",  '" << tmp_file.get_filename() << "' "
                 << " i 2 title 'Zone 2' with lines";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
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

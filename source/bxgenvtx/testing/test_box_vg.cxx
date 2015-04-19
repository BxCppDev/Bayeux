// test_box_vg.cxx

// Ourselves:
#include <genvtx/box_vg.h>

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
#include <geomtools/gnuplot_draw.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>

// This project:
#include <geomtools/geomtools_config.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

int main(int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'box_vg'!" << std::endl;

    bool debug = false;
    bool draw = false;

    int iarg = 1;
    while (iarg < argc_)  {
      std::string token = argv_[iarg];

      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        } else if ((option == "-D") || (option == "--draw")) {
          draw = true;
        } else {
          std::clog << "warning: ignoring option '" << option << "'!" << std::endl;
        }
      } else {
        std::string argument = token;
        {
          std::clog << "warning: ignoring argument '" << argument << "'!" <<std:: endl;
        }
      }
      iarg++;
    }

    std::string       rng_id = "mt19937";
    unsigned long int rng_seed = 12345;
    mygsl::rng random(rng_id, rng_seed);

    size_t nshoots = 1000;

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_box_vg_");

    geomtools::box b(4., 2., 3.);

    {
      tmp_file.out() << "# Draw box (index 0) " << std::endl;
      geomtools::vector_3d pos;
      geomtools::rotation_3d rot;
      geomtools::gnuplot_draw::draw_box(tmp_file.out(), pos, rot, b);
      tmp_file.out() << std::endl << std::endl;
    }

    genvtx::box_vg vg;
    vg.set_debug(debug);

    bool surface = true;
    if (surface) {
      tmp_file.out() << "# Shoot on surface (index 1) " << std::endl;
      vg.set_box(b);
      vg.set_mode(genvtx::box_vg::MODE_SURFACE);
      int surface_mask = 0;
      //surface_mask |= geomtools::box::FACE_BACK ;
      //surface_mask |= geomtools::box::FACE_FRONT;
      //surface_mask |= geomtools::box::FACE_LEFT;
      surface_mask |= geomtools::box::FACE_RIGHT;
      surface_mask |= geomtools::box::FACE_BOTTOM;
      //surface_mask |= geomtools::box::FACE_TOP;
      vg.set_surface_mask(surface_mask);
      vg.set_skin_skip(0.40);
      vg.set_skin_thickness(0.20);
      vg.initialize_simple();
      vg.tree_dump(std::clog, "Box vertex generator (bottom+right surface, skipped)");
      geomtools::vector_3d vertex;
      for (int i = 0; i < (int) nshoots; i++) {
        vg.shoot_vertex(random, vertex);
        geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), vertex, true);
      }
      tmp_file.out() << std::endl << std::endl;
    }

    vg.reset();

    bool bulk = true;
    //bulk = false;
    if (bulk) {
      tmp_file.out() << "# Shoot in bulk (index 2) " << std::endl;
      vg.set_box(b);
      vg.set_mode(genvtx::box_vg::MODE_BULK);
      vg.set_skin_skip(0.0);
      vg.set_skin_thickness(0.30);
      vg.initialize_simple();
      vg.tree_dump(std::clog, "Box vertex generator (bulk)");
      geomtools::vector_3d vertex;
      for (int i = 0; i < (int) nshoots; i++) {
        vg.shoot_vertex(random, vertex);
        geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), vertex, true);
      }
      tmp_file.out() << std::endl << std::endl;
    }

    bool other = true;
    //other = false;
    if (other) {
      tmp_file.out() << "# Shoot other (index 3) " << std::endl;
      genvtx::i_vertex_generator * ivg = 0;
      datatools::properties config;
      config.store("mode", "surface");
      config.store("skin_skip",      0.05);
      config.store("skin_thickness", 0.1);
      std::vector<std::string> surfaces;
      surfaces.push_back("left");
      surfaces.push_back("back");
      surfaces.push_back("top");
      config.store("surfaces", surfaces);
      {
        config.store("box.x", b.get_x());
        config.store("box.y", b.get_y());
        config.store("box.z", b.get_z());
        config.store("length_unit", "mm");
      }
      config.tree_dump(std::clog, "Configuration for dynamic VG:");
      const genvtx::i_vertex_generator::factory_register_type & the_vg_factory
        = DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(genvtx::i_vertex_generator);

      ivg = the_vg_factory.get("genvtx::box_vg")();
      ivg->initialize_standalone(config);
      geomtools::vector_3d vertex;
      for (int i = 0; i < (int) nshoots; i++) {
        ivg->shoot_vertex(random, vertex);
        geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), vertex, true);
      }
      tmp_file.out() << std::endl << std::endl;
      if (ivg != 0) delete ivg;
    }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test genvtx::box_vg");
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      g1.cmd("set xrange [-5:+5]");
      g1.cmd("set yrange [-5:+5]");
      g1.cmd("set zrange [-5:+5]");
      g1.cmd("set xyplane at -5");
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename() << "' index 0 title 'Box' with lines ";
        plot_cmd << ", '' index 1 using 1:2:3:(2.0) title 'Shoot' with points pt 6 ps 0.3 linecolor variable ";
        plot_cmd << ", '' index 2 using 1:2:3:(3.0) title 'Bulk'  with points pt 6 ps 0.3 linecolor variable ";
        plot_cmd << ", '' index 3 using 1:2:3:(4.0) title 'other' with points pt 6 ps 0.3 linecolor variable ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
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
  return (error_code);
}

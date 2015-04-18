// test_blur_spot.cxx

// Ourselves:
#include <geomtools/blur_spot.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

// Third party:
// - Boost:
#include <boost/random/uniform_real.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>
// - Bayeux/datatools:
#include <datatools/io_factory.h>
#include <datatools/temporary_files.h>
#include <datatools/utils.h>
// Some pre-processor guard about Boost I/O usage and linkage :
#include <datatools/bio_guard.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>

// This project:
#include <geomtools/geomtools_config.h>
#include <geomtools/blur_spot.ipp>
#include <geomtools/placement.h>
#include <geomtools/units.h>
// Some pre-processor guard about Boost I/O usage and linkage :
#include <geomtools/bio_guard.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    long seed = 314159;
    bool draw = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];
      if (arg == "-D" || arg == "--draw") {
        draw = true;
      }
      iarg++;
    }

    typedef boost::mt19937 base_generator_type;
    base_generator_type generator(seed);
    boost::uniform_real<> my_prng_dist(0,1);
    boost::variate_generator<base_generator_type&,
                             boost::uniform_real<> > my_prng_boost (generator, my_prng_dist);

    mygsl::rng my_prng_mygsl (mygsl::rng::DEFAULT_RNG_TYPE, seed);

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy (true);
    tmp_file.create("/tmp", "test_blur_spot_");

    // std::ofstream fout ("test_blur_spot.data");

    {
      geomtools::blur_spot v0 (geomtools::blur_spot::dimension_zero);
      v0.set_hit_id(0);
      std::string s0 = "15 20 25 (mm)";
      geomtools::placement::from_string (s0, v0.grab_placement ());

      tmp_file.out() << "# index 0 " << std::endl;
      {
        geomtools::vector_3d vtx;
        v0.randomize_mygsl (my_prng_mygsl, vtx);
        geomtools::print_xyz(tmp_file.out(), vtx);
      }
      tmp_file.out() << std::endl << std::endl;

      geomtools::blur_spot v1 (geomtools::blur_spot::dimension_one);
      v1.set_hit_id(1);
      v1.set_z_error (2.4 * CLHEP::mm);
      std::string s1 = "10 -15 20 (mm) @ 45 30 0 (deg)";
      geomtools::placement::from_string (s1, v1.grab_placement ());
      tmp_file.out() << "# index 1 " << std::endl;
      for (int i = 0; i < 100; i++) {
        geomtools::vector_3d vtx;
        v1.randomize_boost (my_prng_boost, vtx, geomtools::blur_spot::mode_gaussian);
        geomtools::print_xyz(tmp_file.out(), vtx);
      }
      tmp_file.out() << std::endl << std::endl;

      geomtools::blur_spot v2 (geomtools::blur_spot::dimension_two);
      v2.set_hit_id(2);
      v2.set_x_error (4.4 * CLHEP::mm);
      v2.set_y_error (2.7 * CLHEP::mm);
      std::string s2 = " 40 -10 15 (mm) @ 25 25 0 (deg)";
      geomtools::placement::from_string (s2, v2.grab_placement ());
      tmp_file.out() << "# index 2 " << std::endl;
      for (int i = 0; i < 1000; i++) {
        geomtools::vector_3d vtx;
        v2.randomize_boost (my_prng_boost, vtx, geomtools::blur_spot::mode_gaussian);
        geomtools::print_xyz(tmp_file.out(), vtx);
      }
      tmp_file.out() << std::endl << std::endl;

      geomtools::blur_spot v3 (geomtools::blur_spot::dimension_three);
      v3.set_hit_id(3);
      v3.set_x_error (4.3 * CLHEP::mm);
      v3.set_y_error (3.1 * CLHEP::mm);
      v3.set_z_error (2.5 * CLHEP::mm);
      std::string s3 = " -30 10 20 (mm) @ 15 65 0 (deg)";
      geomtools::placement::from_string (s3, v3.grab_placement ());
      v3.invalidate_placement();
      tmp_file.out() << "# index 3 " << std::endl;
      for (int i = 0; i < 10000; i++) {
        geomtools::vector_3d vtx;
        v3.randomize_boost (my_prng_boost, vtx, geomtools::blur_spot::mode_interval);
        geomtools::print_xyz(tmp_file.out(), vtx);
      }
      tmp_file.out() << std::endl << std::endl;

      {
        tmp_file.out() << "# index 4 " << std::endl;
        boost::uniform_real<> x_prng_dist(-50 *  CLHEP::mm, +50 * CLHEP::mm);
        boost::uniform_real<> y_prng_dist(-50 *  CLHEP::mm, +50 * CLHEP::mm);
        boost::uniform_real<> z_prng_dist(-50 *  CLHEP::mm, +50 * CLHEP::mm);
        boost::variate_generator<base_generator_type&, boost::uniform_real<> > x_prng (generator, x_prng_dist);
        boost::variate_generator<base_generator_type&, boost::uniform_real<> > y_prng (generator, y_prng_dist);
        boost::variate_generator<base_generator_type&, boost::uniform_real<> > z_prng (generator, z_prng_dist);
        for (int i = 0; i < 100000; i++) {
          geomtools::vector_3d vtx;
          vtx.set (x_prng(), y_prng(), z_prng());
          bool keep = false;
          double nsigma = 2.0;
          double tolerance = 2.5 * CLHEP::mm;
          if (! keep && v0.match (vtx,
                                  geomtools::blur_spot::MODE_GAUSSIAN,
                                  tolerance)) keep = true;
          if (! keep && v1.match (vtx,
                                  geomtools::blur_spot::MODE_GAUSSIAN,
                                  nsigma,
                                  tolerance)) keep = true;
          if (! keep && v2.match (vtx,
                                  geomtools::blur_spot::MODE_GAUSSIAN,
                                  nsigma,
                                  nsigma,
                                  tolerance)) keep = true;
          if (! keep && v3.match (vtx,
                                  geomtools::blur_spot::MODE_GAUSSIAN,
                                  nsigma,
                                  nsigma,
                                  nsigma)) keep = true;
          if (keep) {
            geomtools::print_xyz(tmp_file.out(), vtx);
          }
        }
        tmp_file.out() << std::endl << std::endl;
      }

      {
        geomtools::wires_type v0_wires;
        v0.generate_wires_self(v0_wires);
        geomtools::gnuplot_draw::basic_draw_wires(tmp_file.out(), v0_wires);

        geomtools::wires_type v1_wires;
        v1.generate_wires_self(v1_wires);
        geomtools::gnuplot_draw::basic_draw_wires(tmp_file.out(), v1_wires);

        geomtools::wires_type v2_wires;
        v2.generate_wires_self(v2_wires);
        geomtools::gnuplot_draw::basic_draw_wires(tmp_file.out(), v2_wires);

        geomtools::wires_type v3_wires;
        v3.generate_wires_self(v3_wires);
        geomtools::gnuplot_draw::basic_draw_wires(tmp_file.out(), v3_wires);

        tmp_file.out() << std::endl << std::endl;
      }

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      if (draw) {

        {
          Gnuplot g0;
          std::ostringstream cmd;
          cmd << "set grid; ";
          cmd << "set xlabel 'x'; ";
          cmd << "set ylabel 'y'; ";
          cmd << "set zlabel 'z'; ";
          cmd << "splot ";
          cmd << "  '" << tmp_file.get_filename() << "' index 0 title '0D-vertex' with points";
          cmd << ", '" << tmp_file.get_filename() << "' index 1 title 'random 1D-vertex (gauss) ' with dots";
          cmd << ", '" << tmp_file.get_filename() << "' index 2 title 'random 2D-vertex (gauss)' with dots";
          cmd << ", '" << tmp_file.get_filename() << "' index 3 title 'random 3D-vertex (interval)' with dots";
          cmd << ", '" << tmp_file.get_filename() << "' index 4 title 'matching vertex' with points lt 7 pt 6 ps 0.75";
          g0.cmd (cmd.str());
          g0.showonscreen (); // window output
          geomtools::gnuplot_drawer::wait_for_key ();
          usleep (200);
        }

        {
          Gnuplot g1("lines");
          g1.cmd("set title 'Blur spots (3D view)'");
          g1.cmd("set grid");
          g1.cmd("set size ratio -1");
          g1.cmd("set view equal xyz");
          g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");
          std::ostringstream plot_cmd;
          plot_cmd << "splot ";
          plot_cmd << "'" << tmp_file.get_filename() << "' index 5 title 'Blur spots (wires)' with lines";
          g1.cmd (plot_cmd.str ());
          g1.showonscreen (); // window output
          geomtools::gnuplot_drawer::wait_for_key ();
          usleep (200);
        }
      }

#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

      {
        datatools::data_writer writer ("test_blur_spot_io.xml",
                                       datatools::using_multi_archives);
        writer.store (v0);
        writer.store (v1);
        writer.store (v2);
        writer.store (v3);
      }

      {
        datatools::data_reader reader ("test_blur_spot_io.xml",
                                       datatools::using_multi_archives);
        while (reader.has_record_tag ()) {
          std::clog << "notice: " << "Next..."  << std::endl;
          if (reader.record_tag_is (geomtools::blur_spot::SERIAL_TAG)) {
            geomtools::blur_spot bs;
            reader.load (bs);
            bs.tree_dump (std::clog, "Blur spot : ");
          } else {
            std::clog << "error: " << "Unsupported record tag : '"
                      << reader.get_record_tag () << "' ! " << std::endl;
            break;
          }
        }
      }
      unlink ("test_blur_spot_io.xml");

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

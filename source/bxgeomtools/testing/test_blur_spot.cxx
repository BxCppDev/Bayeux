// -*- mode: c++; -*- 
// test_blur_spot.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

#include <geomtools/blur_spot.h>
#include <geomtools/placement.h>
#include <geomtools/units.h>
#include <geomtools/gnuplot_i.h>
#include <boost/random/uniform_real.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>
#include <mygsl/rng.h>

#include <datatools/serialization/io_factory.h>

// Some pre-processor guard about Boost I/O usage and linkage :
#include <datatools/serialization/bio_guard.h>
#include <geomtools/serialization/bio_guard.h>

using namespace std;

void wait_for_key ();  // Programm halts until keypress

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      long seed = 314159;
      bool draw = false;
      typedef boost::mt19937 base_generator_type;
      base_generator_type generator(seed);
      boost::uniform_real<> my_prng_dist(0,1);
      boost::variate_generator<base_generator_type&, boost::uniform_real<> > my_prng_boost (generator, my_prng_dist);

      mygsl::rng my_prng_mygsl (mygsl::rng::DEFAULT_RNG_TYPE, seed);

      std::ofstream fout ("test_blur_spot.data");

      {
        geomtools::blur_spot v0 (geomtools::blur_spot::dimension_zero);
        string s0 = "15 20 25 (mm)";
        geomtools::placement::from_string (s0, v0.grab_placement ());

        fout << "# index 0 " << endl;
        {
          geomtools::vector_3d vtx;
          v0.randomize_mygsl (my_prng_mygsl, vtx);
          geomtools::print_xyz(fout, vtx);
        }
        fout << endl << endl;

        geomtools::blur_spot v1 (geomtools::blur_spot::dimension_one);
        v1.set_z_error (2.4 * CLHEP::mm);
        string s1 = "10 -15 20 (mm) @ 45 30 0 (deg)";
        geomtools::placement::from_string (s1, v1.grab_placement ());       
        fout << "# index 1 " << endl;
        for (int i = 0; i < 100; i++)
          {
            geomtools::vector_3d vtx;
            v1.randomize_boost (my_prng_boost, vtx, geomtools::blur_spot::mode_gaussian);
            geomtools::print_xyz(fout, vtx);           
          }
        fout << endl << endl;

        geomtools::blur_spot v2 (geomtools::blur_spot::dimension_two);
        v2.set_x_error (4.4 * CLHEP::mm);
        v2.set_y_error (2.7 * CLHEP::mm);
        string s2 = " 40 -10 15 (mm) @ 25 25 0 (deg)";
        geomtools::placement::from_string (s2, v2.grab_placement ());
        fout << "# index 2 " << endl;
        for (int i = 0; i < 1000; i++)
          {
            geomtools::vector_3d vtx;
            v2.randomize_boost (my_prng_boost, vtx, geomtools::blur_spot::mode_gaussian);
           geomtools::print_xyz(fout, vtx);           
          }
        fout << endl << endl;
       
        geomtools::blur_spot v3 (geomtools::blur_spot::dimension_three);
        v3.set_x_error (4.3 * CLHEP::mm);
        v3.set_y_error (3.1 * CLHEP::mm);
        v3.set_z_error (2.5 * CLHEP::mm);
        string s3 = " -30 10 20 (mm) @ 15 65 0 (deg)";
        geomtools::placement::from_string (s3, v3.grab_placement ());
        fout << "# index 3 " << endl;
        for (int i = 0; i < 10000; i++)
          {
            geomtools::vector_3d vtx;
            v3.randomize_boost (my_prng_boost, vtx, geomtools::blur_spot::mode_interval);
            geomtools::print_xyz(fout, vtx);           
          }
        fout << endl << endl;

        {
          fout << "# index 4 " << endl;
          boost::uniform_real<> x_prng_dist(-50 *  CLHEP::mm, +50 * CLHEP::mm);
          boost::uniform_real<> y_prng_dist(-50 *  CLHEP::mm, +50 * CLHEP::mm);
          boost::uniform_real<> z_prng_dist(-50 *  CLHEP::mm, +50 * CLHEP::mm);
          boost::variate_generator<base_generator_type&, boost::uniform_real<> > x_prng (generator, x_prng_dist);
          boost::variate_generator<base_generator_type&, boost::uniform_real<> > y_prng (generator, y_prng_dist);
          boost::variate_generator<base_generator_type&, boost::uniform_real<> > z_prng (generator, z_prng_dist);
          for (int i = 0; i < 100000; i++)
            {
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
              if (keep)
                {
                  geomtools::print_xyz(fout, vtx);           
                }
          }
          fout << endl << endl;
        }

        if (draw)
          {
            Gnuplot g0;
          ostringstream cmd;
          cmd << "set grid; ";
          cmd << "set xlabel 'x'; ";
          cmd << "set ylabel 'y'; ";
          cmd << "set zlabel 'z'; ";
          cmd << "splot ";
          cmd << "  '" << "test_blur_spot.data" << "' index 0 title '0D-vertex' with points";
          cmd << ", '" << "test_blur_spot.data" << "' index 1 title 'random 1D-vertex (gauss) ' with dots";
          cmd << ", '" << "test_blur_spot.data" << "' index 2 title 'random 2D-vertex (gauss)' with dots";
          cmd << ", '" << "test_blur_spot.data" << "' index 3 title 'random 3D-vertex (interval)' with dots";
          cmd << ", '" << "test_blur_spot.data" << "' index 4 title 'matching vertex' with points lt 7 pt 6 ps 0.75";
          g0.cmd (cmd.str());
          wait_for_key ();
        }

        {
          namespace ds = datatools::serialization;
          ds::data_writer writer ("test_blur_spot_io.xml",
                                  ds::using_multi_archives);
          writer.store (v0);
          writer.store (v1);
          writer.store (v2);
          writer.store (v3);
        }

        {
          namespace ds = datatools::serialization;
          ds::data_reader reader ("test_blur_spot_io.xml",
                                  ds::using_multi_archives);
          while (reader.has_record_tag ())
            {
              clog << "notice: " << "Next..."  << endl; 
             if (reader.record_tag_is (geomtools::blur_spot::SERIAL_TAG))
                {    
                  geomtools::blur_spot bs;
                  reader.load (bs);
                  bs.tree_dump (clog, "Blur spot : ");
                }
              else
                {
                  clog << "error: " << "Unsupported record tag : '" 
                       << reader.get_record_tag () << "' ! " << endl; 
                  break;
                }
            }
        }

      }

      unlink ("test_blur_spot.data");
      unlink ("test_blur_spot_io.xml");
    }
  catch (exception & x)
    {
      cerr << "error: " << x.what() << endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error!" << endl; 
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

void wait_for_key ()
{
#if defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    cout << endl << "Press ENTER to continue..." << endl;

    std::cin.clear();
    std::cin.ignore(std::cin.rdbuf()->in_avail());
    std::cin.get();
#endif
    return;
}

// end of test_blur_spot.cxx

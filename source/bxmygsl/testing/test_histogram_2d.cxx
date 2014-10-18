// test_histogram_2d.cxx
/*
 * Histogram 2D sample program
 *
 *
 */

// Standard library:
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>

// Third party:
// - Boost:
#include <boost/filesystem.hpp>
// - Bayeux/datatools:
#include <datatools/io_factory.h>

// Some pre-processor guard about Boost I/O usage and linkage :
#include <datatools/bio_guard.h>
#include <mygsl/bio_guard.h>

#include <mygsl/histogram_2d.h>

int main(int /* argc_ */ , char ** /* argv_ */)
{
  try
    {
      long   seed48   = 12345;
      size_t xbins    = 10;
      size_t ybins    = 10;
      size_t nshoots  = 10000;

      srand48(seed48);

      mygsl::histogram_2d h(xbins, 0.0, 1.0, ybins, 0.0, 1.0);
      /*
Gnuplot:

 set size square
 set title "test_histogram_2d"
 set xrange [0:1]
 set yrange [0:1]
 set samples 10,10
 set pm3d map
 set palette defined (0 "white", 0.01 "blue", 0.02 "green", 0.04 "red")
 splot 'test_histogram_2d.data' u (0.5*(($1)+($2))):(0.5*(($3)+($4))):5 notitle
 splot 'test_histogram_2d.data' u 1:3:5 notitle

       */
      {
        std::clog << "NOTICE: h fill" << std::endl;

        for (int i= 0; i < (int) nshoots; i++)
          {
            double x = -std::log(drand48());
            double y = -std::log(drand48());
            h.fill(x, y);
          }
        h.dump(std::clog);

        std::clog << "NOTICE: h print" << std::endl;

        std::string sname = "test_histogram_2d.data";
        std::ofstream ofhist(sname.c_str());
        h.print(ofhist);
        ofhist.close();

        h.tree_dump(std::clog, "Histogram 'h' : ");
        std::clog << std::endl;

        {
          std::clog << "INFO: "
                    << "Test serialization..."
                    << std::endl;
          std::string filename = "test_histogram_2d.xml";
          {
            std::clog << "INFO: "
                      << "Test serialization: writer..."
                      << std::endl;
            datatools::data_writer writer (filename);
            writer.store (h);
          }

          {
            std::clog << "INFO: "
                      << "Test serialization: reader..."
                      << std::endl;
            mygsl::histogram_2d hbis;
            datatools::data_reader reader (filename);
            if (reader.has_record_tag ())
              {
                if (reader.record_tag_is (mygsl::histogram_2d::SERIAL_TAG))
                  {
                    reader.load (hbis);
                    hbis.dump (std::clog);
                  }
                else
                  {
                    std::clog << "ERROR: "
                              << "Cannot load an histogram 2D object !"
                              << std::endl;
                  }
              }
            else
              {
                std::clog << "ERROR: "
                          << "No object in the Boost archive !"
                          << std::endl;
              }
          }
        }

      }

      std::clog << "NOTICE: The end." << std::endl;

    }
  catch(std::exception & x)
    {
      std::cerr << "ERROR: " << x.what() << std::endl;
      return (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

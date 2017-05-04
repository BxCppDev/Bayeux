/* gen_sample_data.cxx
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2013-02-16
 * Last modified : 2013-02-16
 *
 * Copyright (C) 2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * Description:
 *
 *  A program that generates some sample data records
 *  in a Boost/Serialization file to be used by test programs.
 *
 * Usage:
 *
 *  gen_sample_data [-10|-100] data_out[.brio|.trio|[.xml|.txt|.data][.gz]]
 *
 */

#include <cstdlib>
#include <iostream>
#include <string>
#include <list>
#include <stdexcept>

#include <dpp/dpp_config.h>
#include <boost/filesystem.hpp>

#if DPP_DATATOOLS_LEGACY == 1
#include <datatools/utils/ioutils.h>
#include <datatools/utils/properties.h>
#include <datatools/utils/things.h>
#include <datatools/serialization/bio_guard.h>
#else
#include <datatools/ioutils.h>
#include <datatools/properties.h>
#include <datatools/things.h>
#include <datatools/bio_guard.h>
#endif

#include <dpp/simple_data_sink.h>
#include <dpp/simple_brio_data_sink.h>

void gen_data_record (DPP_DU::things & data_record_)
{

  {
    DPP_DU::properties & header 
      = data_record_.add<DPP_DU::properties>("Header");
    header.store_flag ("simulated");
    header.store ("time", 1234000000 + (int) (1000000*drand48()));
    header.store ("pi", 3.14159);
    header.store ("length_unit", "mm");
    header.store ("time_unit", "ns");
  }

  {
    DPP_DU::properties & hits_data
      = data_record_.add<DPP_DU::properties>("Hits");
    int nhits = (int) (10*drand48());
    std::vector<int> ids;
    std::vector<double> x, y, z;
    std::vector<double> de;
    for (int i = 0; i < nhits; i++)
      {
        ids.push_back(i);
        x.push_back(drand48());
        y.push_back(drand48());
        z.push_back(drand48());
        de.push_back(drand48());
      }
    hits_data.store ("nhits", nhits);
    hits_data.store ("x", x);
    hits_data.store ("y", y);
    hits_data.store ("z", z);
    hits_data.store ("de", de);
  }

  data_record_.tree_dump(std::clog,"Data record : ");

  return;
}

/************************************************************/

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  std::string output_file;
  time_t now = time(0);
  long seed = (long) now;
  std::clog << DPP_DU::io::notice 
            << "Seed = " << seed << std::endl;
  srand48(seed);

  try
    {
      bool debug = false;
      int max_events = 1;

      int iarg = 1;
      while (iarg < argc_)
        {
          std::string token = argv_[iarg];
          if (token[0] == '-')
            {
              std::string option = token;
              if ((option == "-d") || (option == "--debug"))
                {
                  debug = true;
                }
              else if (option == "-10")
                {
                  max_events = 10;
                }
              else if (option == "-100")
                {
                  max_events = 100;
                }
              else
                {
                  std::clog << DPP_DU::io::warning 
                            << "ignoring option '" << option << "'!" << std::endl;
                }
            }
          else
            {
              std::string argument = token;
              if (output_file.empty ())
                {
                  output_file = argument;
                }
            }
          iarg++;
        }

      if (output_file.empty ())
        {
          throw std::logic_error ("Missing output sink !");
        }
      std::clog << DPP_DU::io::notice << "Output data source : "
                << "'" << output_file << "'" << std::endl;

      // Setup the data output sink :
      if (debug) std::clog << DPP_DU::io::notice
                           << "Using data sink '" << output_file << "'..." << std::endl;
      boost::scoped_ptr<dpp::i_data_sink> sink;
      boost::filesystem::path sink_path(output_file);
      std::clog << DPP_DU::io::notice
                << "Extension '" << sink_path.extension().string() << "'" << std::endl;
      if (sink_path.extension().string() == ".brio" ||
          sink_path.extension().string() == ".trio")
        {
          std::clog << DPP_DU::io::notice
                    << "Opening Brio sink... " << std::endl;
          sink.reset(new dpp::simple_brio_data_sink (output_file));
        }
      else
        {
          std::clog << DPP_DU::io::notice
                    << "Opening Boost sink... " << std::endl;
          sink.reset(new dpp::simple_data_sink (output_file));
        }

      // Loop on the event records from the data source file :
      int counts = 0;
      while (counts < max_events)
        {
          // Declare the data record :
          DPP_DU::things DR;

          // Generate the data record :
          gen_data_record (DR);

          // Storing the event record :
          sink.get()->store_next_record (DR);

          counts++;
        }
    }
  catch (std::exception & x)
    {
      std::cerr << "error: " << x.what () << std::endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << "error: " << "Unexpected error!" << std::endl;
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

// end of gen_sample_data.cxx
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

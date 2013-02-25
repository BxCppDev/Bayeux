// -*- mode: c++; -*- 
// test_genbb.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <boost/filesystem.hpp>

#include <datatools/utils.h>

#include <genbb_help/genbb.h>
#include <mygsl/rng.h>

using namespace std; 

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {  
      bool debug = false;
      bool test = false;
      bool dump = false;
      bool preserve_data = false;
      size_t max_count = 27000;

      int iarg = 1;
      while (iarg < argc_)
        {
          string arg = argv_[iarg];

          if (arg == "-d" || arg == "--debug") debug = true;
          if (arg == "-t" || arg == "--test") test = true;
          if (arg == "-D" || arg == "--dump") dump = true;
          if (arg == "-p") preserve_data = true;
          if (arg == "-10") max_count = 10;
          if (arg == "-100") max_count = 100;
          if (arg == "-1000") max_count = 1000;

          iarg++;
        }

      int seed = 314159;

      datatools::properties config;
      if (debug) config.store_flag ("debug");
      if (test) config.store_flag ("test");
      config.store ("seed", seed);
      if (preserve_data) config.store_flag ("preserve_data_files");
      config.store ("buffer_size", 10000);
      config.store ("decay_type", "DBD");
      config.store ("decay_isotope", "Se82");
      config.store ("decay_dbd_level", 0);
      config.store ("decay_dbd_mode", 4);
      config.store ("energy_min", 2.0);
      config.store ("energy_max", 4.3);
      if (debug) config.tree_dump (clog, "Configuration: ", "debug: ");
 
      mygsl::rng ran ("taus", seed);

      genbb::genbb GBB;
      GBB.set_debug (debug);
      GBB.set_external_random (ran);
      //GBB.set_delete_conf_file (false);
      //GBB.set_delete_log_files (false);
      //GBB.set_delete_data_files (false);
      //GBB.set_tmp_base_dir ("/tmp/${USER}");
      string tmp_dir = "/tmp/${USER}/genbb_work.d";
      datatools::fetch_path_with_env (tmp_dir);
      if (! boost::filesystem::is_directory (tmp_dir))
        {
          if (! boost::filesystem::create_directory (tmp_dir))
            {
              ostringstream message;
              message << "Cannot create temporary directory '" << tmp_dir << "' !"; 
              throw logic_error (message.str ());
            }
          
        }
      GBB.set_tmp_dir (tmp_dir);
      GBB.initialize_standalone (config);
      if (debug) clog << "debug: " << "Initialized !"<< endl;

      // working primary event:
      genbb::primary_event pe;

      for (int i = 0; i < max_count; i++)
        {
          if ((i % 1000) == 0 || (i == (max_count - 1))) 
            {
              clog << "Count : " << i << endl;
            }
          GBB.load_next (pe);
          if (dump) pe.dump ();
        }

      if (! tmp_dir.empty ()) 
        {
          clog << "Directory '" << tmp_dir << "' contains the intermediate generated data and log files." << endl;
        }
      GBB.reset ();
      if (debug) clog << "debug: " << "The end." << endl;
    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error!" << endl; 
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_genbb.cxx

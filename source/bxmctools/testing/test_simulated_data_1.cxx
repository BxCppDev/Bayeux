// -*- mode: c++ ; -*- 
// test_simulated_data_1.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <vector>

// Utilities :
#include <datatools/units.h>
#include <datatools/clhep_units.h>
#include <datatools/smart_ref.h>

// Simulated data model :
#include <mctools/simulated_data.h>

// Serialization :
#include <datatools/io_factory.h>
#include <mctools/simulated_data.ipp>

// Some pre-processor guard about Boost I/O usage and linkage :
#include <datatools/bio_guard.h>
#include <mygsl/bio_guard.h>
#include <geomtools/bio_guard.h>
#include <genbb_help/bio_guard.h>
#include <mctools/bio_guard.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'simulated_data'!" << endl; 
  
      bool debug = false;

      int iarg = 1;
      while (iarg < argc_)
        {
          string token = argv_[iarg];

          if (token[0] == '-')
            {
               string option = token; 
               if ((option == "-d") || (option == "--debug")) 
                 {
                   debug = true;
                 }
               else 
                 { 
                    clog << "warning: ignoring option '" << option << "'!" << endl; 
                 }
            }
          else
            {
              string argument = token; 
              { 
                clog << "warning: ignoring argument '" 
                     << argument << "'!" << endl; 
              }
            }
          iarg++;
      }
    
      clog << "Test 0:" << endl;
      {
        // A simulated data object :
        mctools::simulated_data SD;

        // Primary vertex :
        SD.grab_vertex ().set ( 2.0 * CLHEP::mm, 
                               -4.0 * CLHEP::mm, 
                               +7.0 * CLHEP::mm);
        // Primary event :
        {
          genbb::primary_event & the_primary_event = SD.grab_primary_event ();
          genbb::primary_particle p1, p2;
          p1.set_time (0.0 * CLHEP::ns);
          p1.set_type (genbb::primary_particle::ELECTRON);
          geomtools::vector_3d m1 (3.0 * CLHEP::MeV, 0.0, 0.0);
          p1.set_momentum (geomtools::vector_3d (3.0 * CLHEP::MeV, 0.0, 0.0));
          p2.set_time (1.3 * CLHEP::ns);
          p2.set_type (genbb::primary_particle::GAMMA);
          p2.set_momentum (geomtools::vector_3d (0.0, 2.0 * CLHEP::MeV, 0.0));
          the_primary_event.add_particle (p1);
          the_primary_event.add_particle (p2);
        }
        // Some properties :
        SD.grab_properties ().store_flag ("fake");
        SD.grab_properties ().store ("module_number", 0);
        SD.grab_properties ().store ("run_number", 666);
        SD.grab_properties ().store ("event_number", 7);

        // Add some list of MC hits :
        SD.add_step_hits ("calo", 10);
        SD.add_step_hit ("calo").set_hit_id (0);
        SD.add_step_hit ("calo").set_hit_id (1);
        SD.add_step_hit ("calo").set_hit_id (2);
        SD.add_step_hits ("xcalo", 5);
        SD.add_step_hits ("gveto", 5);
        SD.add_step_hit ("gveto").set_hit_id (0);
        SD.add_step_hits ("gg", 50);
        for (int igg =0; igg < 15; ++igg)
          {
            SD.add_step_hit ("gg").set_hit_id (igg);        
          }

        // Print :
        SD.tree_dump (clog, "Simulated data :");

        {
          // Serialization:
          datatools::data_writer DW ("test_simulated_data_1.xml", 
                                     datatools::using_multi_archives);
          DW.store (SD);
          clog << "The 'simulated_data' object has been stored in a Boost serialization XML file." << endl;
        }

      }
  
      clog << "Step 2:" << endl;
      {
        // Serialization:
        datatools::data_reader DR ("test_simulated_data_1.xml", 
                                   datatools::using_multi_archives);

        // A simulated data object :
        mctools::simulated_data SD;

        // Load a simulated data object from Boost serialization XML file:
        while (DR.has_record_tag ()) 
          {
            if (DR.record_tag_is (mctools::simulated_data::SERIAL_TAG)) 
              {
                DR.load (SD);
                SD.tree_dump (clog, "Simulated data (loaded from the reader) :");
              }
            else 
              {
                string bad_tag = DR.get_record_tag ();
                clog << "ERROR: unknown data tag '" 
                     << bad_tag << "'!" << endl; 
                break;
              }
          }
        clog << "The 'simulated_data' object has been loaded from a Boost serialization XML file." << endl;
      }

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
  return (error_code);
}

// end of test_simulated_data_1.cxx

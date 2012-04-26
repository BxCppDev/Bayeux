// -*- mode: c++ ; -*- 
// test_emfield_manager.cxx
 
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <emfield/emfield_config.h>
#include <emfield/electromagnetic_field_manager.h>
#include <datatools/utils/properties.h>
#include <datatools/services/service_manager.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for the 'emfield::electromagnetic_field_manager' class." << endl; 
  
      bool debug = false;
      bool verbose = false;

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
               else if ((option == "-v") || (option == "--verbose")) 
                 {
                   verbose = true;
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
                clog << "warning: ignoring argument '" << argument << "'!" << endl; 
              }
            } 
          iarg++; 
      }

      
      // This is a blank service manager :
      datatools::service::service_manager SRVCmgr;
      SRVCmgr.initialize ();

      // This is a EM field manager :
      emfield::electromagnetic_field_manager EMFmgr;

      // Attach the service manager :
      EMFmgr.set_service_manager (SRVCmgr);
     
      // Configuration :
      datatools::utils::properties EMFmgr_config;
      EMFmgr_config.set_flag ("debug");
      EMFmgr_config.set_flag ("needs_service_manager");

      // Load a specific set of EM field rules :
      EMFmgr.load ("${EMFIELD_DATA_DIR}/testing/config/test_emfields.conf");

      // Initialize :
      EMFmgr.initialize (EMFmgr_config);

      EMFmgr.tree_dump (std::clog, "EM field manager: ");
      
      for (emfield::base_electromagnetic_field::field_dict_type::const_iterator i = EMFmgr.get_fields().begin ();
           i != EMFmgr.get_fields().end ();
           i++)
        {
          std::string field_name = i->first;
          std::clog << "\n" << "Field '" << field_name << "' : " << std::endl;
          if (! EMFmgr.has_field (field_name))
            {
              std::ostringstream message;
              message << "No field named '" << field_name << "' !";
              throw std::logic_error (message.str ());
            }
          emfield::base_electromagnetic_field & the_field = EMFmgr.grab_field (field_name);
          
          geomtools::vector_3d position (2.0 * CLHEP::m, -1.0 * CLHEP::m, 0.5 * CLHEP::m);
          double time = 1.3 * CLHEP::second;
          
          if (the_field.is_electric_field ())
            {
              geomtools::vector_3d e_field;
              int status = the_field.compute_electric_field (position, time, e_field);
              if (status != 0)
                {
                  std::ostringstream message;
                  message << "Field named '" << field_name << "' cannot compute electric field value at " 
                          << position / CLHEP::m << " [m] / " << time / CLHEP::second << " [s] !";
                  throw std::logic_error (message.str ());        
                }
              else
                {
                  clog << "  Electric field at : " << position / CLHEP::m << " [m] / " 
                       << time / CLHEP::second << " [s] is : " 
                       << e_field / (CLHEP::volt / CLHEP::m) << " V/m" << std::endl;
                }
            }
          else
            {
              clog << "  Not an electric field." << std::endl;
            }
          
          if (the_field.is_magnetic_field ())
            {
              geomtools::vector_3d b_field;
              int status = the_field.compute_magnetic_field (position, time, b_field);
              if (status != 0)
                {
                  std::ostringstream message;
                  message << "Field named '" << field_name << "' cannot compute magnetic field value at " 
                          << position / CLHEP::m << " [m] / " << time / CLHEP::second << " [s] !";
                  throw std::logic_error (message.str ());        
                }
              else
                {
                  clog << "  Magnetic field at : " << position / CLHEP::m << " [m] / " 
                       << time / CLHEP::second << " [s] is : " 
                       << b_field / (CLHEP::gauss) << " gauss" << std::endl;
                }
            }
          else
            {
              clog << "  Not a magnetic field." << std::endl;
            }
        }

      clog << "The end." << endl;
    }
  catch (exception & x)
    { 
      cerr << "error: " << x.what () << endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error !" << endl; 
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

// end of test_emfield.cxx

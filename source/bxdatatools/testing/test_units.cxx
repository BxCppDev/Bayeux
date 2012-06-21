// -*- mode: c++ ; -*- 
// test_units.cxx

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include <datatools/utils/units.h>
#include <datatools/utils/clhep_units.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'datatools::units'!" << endl; 
  
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
                clog << "warning: ignoring argument '" << argument << "'!" << endl; 
              }
            }
          iarg++;
      }
    
      clog << "Known units : " << endl;
      const std::vector<std::string> & ulreg 
        = datatools::utils::units::get_unit_labels_registry ();
      for (int i = 0; i < ulreg.size (); i++)
        {
          clog << "Unit label = '" << ulreg[i] << "'" << endl;
        }
      clog << endl;

      string word = "0.1 mm";
      clog << "Parsing expression : '" << word << "'" << endl;

      istringstream iss (word);
      double length; 
      iss >> length;
      if (! iss)
        {
          ostringstream message;
          message << "Format error while reading a double value !";
          throw runtime_error (message.str ());
        }
      string sunit;
      iss >> sunit;
      if (! iss)
        {
          ostringstream message;
          message << "Format error while reading a word !";
          throw runtime_error (message.str ());
        }
      double lunit = datatools::utils::units::get_length_unit_from (sunit);

      cout << "CLHEP basic units: " << endl;
      cout << "  1 m    ==  " << CLHEP::m << endl;
      cout << "    1 cm   ==  " << CLHEP::cm << endl;
      cout << "    1 mm   ==  " << CLHEP::mm << endl;
      cout << "  1 kg     ==  " << CLHEP::kg << endl;
      cout << "    1 g  ==  " << CLHEP::g << endl;
      cout << "  1 s      ==  " << CLHEP::second << endl;
      cout << "    1 ms   ==  " << CLHEP::millisecond << endl;
      cout << "    1 ns   ==  " << CLHEP::nanosecond << endl;
      
      // apply the unit to the value:
      length *= lunit;

      cout << "Length is : " << length / CLHEP::um << " um" << endl;
      cout << "Length is : " << length / CLHEP::cm << " cm" << endl;
      cout << "Length is : " << length / CLHEP::parsec << " parsec" << endl;

      double lengthu = datatools::utils::units::get_value_with_unit ("8 inch");
      cout << "Length (2) is : " 
           << lengthu / CLHEP::cm << " cm" << endl;

      double temperature = datatools::utils::units::get_value_with_unit ("300 kelvin");
      cout << "Temperature is : " 
           << temperature / CLHEP::kelvin << " kelvin" << endl;

      double pressure = datatools::utils::units::get_value_with_unit ("1 millibar");
      cout << "Pressure is : " 
           << pressure / CLHEP::atmosphere << " atm" << endl;
 
      try
        {
          // this fails:
          double fail = datatools::utils::units::get_value_with_unit ("3.14159 dummy");
        }
      catch (exception & x)
        {
          clog << "As expected, the 'dummy' symbol is not a known unit !" << endl;
        }

      {
        double unit_value;
        string unit_label;
        if (datatools::utils::units::find_unit ("mBq/m3", unit_value, unit_label))
          {
            clog << "Unit label = '" << unit_label << "'\n";
            clog << "Unit value = " << unit_value << "\n";
          }
      }
      
      {
        double mass_activity_value;
        string unit_label;
        if (datatools::utils::units::find_value_with_unit ("10 uBq/kg", mass_activity_value, unit_label))
          {
            clog << "Mass activity value = " << mass_activity_value << "\n";
            clog << "Unit label = '" << unit_label << "'\n";
          }
        double mass_value;
        if (datatools::utils::units::find_value_with_unit ("2 kg", mass_value, unit_label))
          {
            clog << "Mass value = " << mass_value << "\n";
            clog << "Unit label = '" << unit_label << "'\n";
          }
        double activity = mass_activity_value * mass_value;
        double uBq_unit = 1.e-6 * CLHEP::becquerel;
        clog << "Activity value = " << activity / uBq_unit << " uBq\n";
        
      }

      
      {
        double frequency_value;
        string unit_label;
        if (datatools::utils::units::find_value_with_unit ("10.3 kHz", frequency_value, unit_label))
          {
            clog << "Frequency value = " << frequency_value << "\n";
            clog << "Unit label = '" << unit_label << "'\n";
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
      cerr << "error: " << "unexpected error!" << endl; 
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

// end of test_units.cxx

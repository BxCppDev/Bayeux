// -*- mode: c++ ; -*- 
// test_units.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

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

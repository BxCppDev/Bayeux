// -*- mode: c++; -*- 
// test_version_check.cxx

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>

#include <datatools/version_check.h>
#include <datatools/version_id.h>

using namespace std;

int main (int argc_ , char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      clog << "Test of the 'version_check' algos..." << endl; 
      bool debug = false;

      int iarg =  1;
      while (iarg < argc_) 
        {
          string arg = argv_[iarg];

          if ((arg == "-d") || (arg == "--debug")) debug = true;

          iarg++;
        }

      clog << endl;

      std::string rules; 
      std::string label = "datatools";
      rules = "foo (>=3.2) | bar (==6.7) | bar (==debug)";
      datatools::version_id v0 (3,1,4,"159");
      if (datatools::validate_version (label, v0, rules))
        {
          clog << "Label '" << label << "' with version ID '" << v0 << "' is validated by rule: \n  '"
               << rules << "'" << endl;
        }
      else
        {
          clog << "Label '" << label << "' with version ID '" << v0 << "' is NOT validated by rule: \n  '"
               << rules << "'" << endl;
        }
      clog << endl;

      std::ostringstream rules_iss;
      rules_iss << "foo";
      rules_iss << '|' << "datatools(>=3.0 & <=3.1.3 & !=3.0.1-buggy)";
      rules_iss << '|' << "datatools(=3.1.4-159)"; 
      rules_iss << '|' << "datatools(>4)"; 
      rules = rules_iss.str ();

      if (datatools::validate_version (label, v0, rules))
        {
          clog << "Label '" << label << "' with version ID '" << v0 << "' is validated by rule: \n  '"
               << rules << "'" << endl;
        }
      else
        {
          clog << "Label '" << label << "' with version ID '" << v0 << "' is NOT validated by rule: \n  '"
               << rules << "'" << endl;
        }
      clog << endl;

      datatools::version_id v1 (3,1,2);
      if (datatools::validate_version (label, v1, rules))
        {
          clog << "Label '" << label << "' with version ID '" << v1 << "' is validated by rule: \n  '"
               << rules << "'" << endl;
        }
      else
        {
          clog << "Label '" << label << "' with version ID '" << v1 << "' is NOT validated by rule: \n  '"
               << rules << "'" << endl;
        }
      clog << endl;

      label = "foo";
      datatools::version_id v2 (6,6,6);
      if (datatools::validate_version (label, v2, rules))
        {
          clog << "Label '" << label << "' with version ID '" << v2 << "' is validated by rule: \n  '"
               << rules << "'" << endl;
        }
      else
        {
          clog << "Label '" << label << "' with version ID '" << v2 << "' is NOT validated by rule: \n  '"
               << rules << "'" << endl;
        }
      clog << endl;

      label = "datatools";
      datatools::version_id v3 (3,5,2);
      if (datatools::validate_version (label, v3, rules))
        {
          clog << "Label '" << label << "' with version ID '" << v3 << "' is validated by rule: \n  '"
               << rules << "'" << endl;
        }
      else
        {
          clog << "Label '" << label << "' with version ID '" << v3 << "' is NOT validated by rule: \n  '"
               << rules << "'" << endl;
        }
      clog << endl;

      label = "datatools";
      datatools::version_id v4 (3,0,1,"buggy");
      if (datatools::validate_version (label, v4, rules))
        {
          clog << "Label '" << label << "' with version ID '" << v4 << "' is validated by rule: \n  '"
               << rules << "'" << endl;
        }
      else
        {
          clog << "Label '" << label << "' with version ID '" << v4 << "' is NOT validated by rule: \n  '"
               << rules << "'" << endl;
        }
      clog << endl;

      label = "datatools";
      datatools::version_id v5 (4,2,1);
      if (datatools::validate_version (label, v5, rules))
        {
          clog << "Label '" << label << "' with version ID '" << v5 << "' is validated by rule: \n  '"
               << rules << "'" << endl;
        }
      else
        {
          clog << "Label '" << label << "' with version ID '" << v5 << "' is NOT validated by rule: \n  '"
               << rules << "'" << endl;
        }
      clog << endl;

      label = "datatools";
      datatools::version_id v6 (4,2,1);
      rules_iss << '|' << "datatools(>4.0)"; 
      rules = rules_iss.str ();
      if (datatools::validate_version (label, v6, rules))
        {
          clog << "Label '" << label << "' with version ID '" << v6 << "' is validated by rule: \n  '"
               << rules << "'" << endl;
        }
      else
        {
          clog << "Label '" << label << "' with version ID '" << v6 << "' is NOT validated by rule: \n  '"
               << rules << "'" << endl;
        }
      clog << endl;

      rules = "datatools(!=5)";
      if (datatools::validate_version (label, v6, rules))
        {
          clog << "Label '" << label << "' with version ID '" << v6 << "' is validated by rule: \n  '"
               << rules << "'" << endl;
        }
      else
        {
          clog << "Label '" << label << "' with version ID '" << v6 << "' is NOT validated by rule: \n  '"
               << rules << "'" << endl;
        }
      clog << endl;

      rules = "datatools(!=4)";
      if (datatools::validate_version (label, v6, rules))
        {
          clog << "Label '" << label << "' with version ID '" << v6 << "' is validated by rule: \n  '"
               << rules << "'" << endl;
        }
      else
        {
          clog << "Label '" << label << "' with version ID '" << v6 << "' is NOT validated by rule: \n  '"
               << rules << "'" << endl;
        }
      clog << endl;

      rules = "datatools(!=4.0)";
      if (datatools::validate_version (label, v6, rules))
        {
          clog << "Label '" << label << "' with version ID '" << v6 << "' is validated by rule: \n  '"
               << rules << "'" << endl;
        }
      else
        {
          clog << "Label '" << label << "' with version ID '" << v6 << "' is NOT validated by rule: \n  '"
               << rules << "'" << endl;
        }
      clog << endl;

    }
  catch (exception & x)
    { 
      clog << "error: " << x.what () << endl; 
      error_code =  EXIT_FAILURE;
    }
  catch (...) 
    { 
      clog << "error: " << "unexpected error!" << endl;  
      error_code = EXIT_FAILURE; 
    } 
  return error_code;
} 

// end of test_version_check.cxx 
  

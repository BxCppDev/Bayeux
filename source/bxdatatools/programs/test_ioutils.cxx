// -*- mode: c++; -*- 
// test_ioutils.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <datatools/utils/ioutils.h>

using namespace std;

int main (int argc_ , char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      clog << "Test of the 'ioutils' class..." << endl; 
  
      bool debug = false;
      bool color = false;

      int iarg = 1;
      while (iarg <  argc_)  
	{
	  string arg = argv_[iarg]; 

	  if (arg == "-d" || arg == "--debug") debug =  true;

	  if (arg == "-c" || arg == "--color") color =  true;

	  iarg++;
	}
    
      datatools::utils::io::set_colored (color);

      using namespace datatools::utils;
      clog << io::indent << io::warning << "This is a warning print ! " 
	   << io::normal << endl;
      io::indent++;
      clog << io::indent << io::notice << "This is a notice print ! " 
	   << io::normal << endl;
      clog << io::indent << io::debug << "This is a debug print ! " 
	   << io::normal << endl;
      io::indent++;
      clog << io::indent << io::normal << io::notice << "More print ! " 
	   << io::normal << endl;
      clog << io::indent << io::normal << io::notice << "Yet another print ! " 
	   << io::normal << endl;
      io::indent--;
      clog << io::indent << io::normal << io::notice << "Back to margin ! " 
	   << io::normal << endl;
      io::indent (0);
      clog << io::indent << io::devel << "Done." 
	   << io::normal << endl;
      io::indent (6);  
      clog << io::indent << io::error << "Ooops !" 
	   << io::normal << endl;

      io::indenter ind;
      ind++;
      cout << ind << "Hello, World !" << endl;
      ind++;
      cout << ind << "How do you feel ?" << endl;
      ind (4);
      cout << ind << "Well, I'm fine." << endl;
      ind--;
      cout << ind << "Good ! Let's go !" << endl;

      uint32_t value = 54;
      cout << "value (dec) = " << value << endl;
      cout << "value (bin) = " << io::to_binary (value) << endl;

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

// end of test_ioutils.cxx

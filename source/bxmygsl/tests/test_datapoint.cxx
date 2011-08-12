// test_datapoint.cxx

#include <cstdlib>
#include <iostream>
#include <limits>

#include <mygsl/datapoint.h>

int main (int argc_ , char ** argv_)
{
  try  
    {
      mygsl::datapoint p1 (2.5, 3.4);
      mygsl::datapoint p2 (3.5, 2.24, 0.12);

      cout << "p1      == " << p1 << endl;
      cout << "p2      == " << p2 << endl;

      mygsl::datapoint p3;
      clog << "Enter a datapoint (example: `1 0 0' ) : ";
      cin >> p3;
      if (! cin)
	{
	  cerr << "Format error!" << endl;
	}
      else
	{
	  cout.precision (12);
	  cout << "p3       == " << p3 << endl;
	}
    }
  catch (std::exception & x)
    {
      std::cerr << "test_datapoint: " << x.what () << std::endl;
      return (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

// end of test_datapoint.cxx

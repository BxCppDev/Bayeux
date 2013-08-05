// test_permutation.cxx

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <cmath>

#include <mygsl/permutation.h>

int main(int argc_ , char ** argv_)
{
  using namespace std;
  try
    {
      mygsl::permutation p1 (5);

      cout << "p1           = "  << p1 << endl;

      p1.swap (1,4);
      cout << "p1 (swapped) = "  << p1 << endl;

      mygsl::permutation p2 (p1);
      p2.next ( );
      cout << "p2 (next)    = "  << p2 << endl;

      {
        mygsl::permutation p3 (3);
        int count = 0;
        do
          {
            cout << "p3 (#" << ++count << ")      = "  << p3 << endl;
          } while (p3.next ());
      }

      {
        mygsl::permutation p4 (3);
        cout << "p4          = "  << p4 << endl;
        mygsl::permutation p5;
        p4.linear_to_canonical (p5);
        cout << "p5          = "  << p5 << endl;
        mygsl::permutation p6;
        p5.canonical_to_linear (p6);
        cout << "p6          = "  << p6 << endl;

      }

      clog << "Enter a permutation: ";
      std::istringstream iss("(0, 1, 2, 4, 3)");
      iss >> p1;
      if (! iss)
        {
          clog << "Format error!" << endl;
        }
      else
        {
          cout << "p1 (input)   = "  << p1 << endl;
        }
    }
  catch (exception & x)
    {
      cerr << "PERMUTATION: " << x.what () << endl;
      return (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

// end of test_permutation.cxx

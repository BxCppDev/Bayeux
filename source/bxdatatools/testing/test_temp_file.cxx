// -*- mode: c++; -*-
// test_temp_file.cxx

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <exception>

// This Project:
#include <datatools/temporary_files.h>

using namespace std;

int main (int argc_ , char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test of the 'temp_file' class..." << endl;

      //bool debug = false;
      bool temp_file_delete = true;
      int iarg = 1;
      while (iarg <  argc_)
        {
          string arg = argv_[iarg];

          //if (arg == "-d" || arg == "--debug") debug = true;
          if (arg == "-D" || arg == "--no-delete") temp_file_delete = false;

          iarg++;
        }

      {
        datatools::temp_file ftmp;
        ftmp.set_remove_at_destroy (temp_file_delete);

        clog << "Create a temporary file: " << endl;
        ftmp.create ("/tmp", "temp_");
        clog << "Temporary filename is '"
             << ftmp.get_filename () << "' !" << endl;

        clog << "Write in the temporary file: " << endl;
        for (int i = 0; i < 10; i++)
          {
            ftmp.out () << "i=" << i << endl;
          }
        ftmp.close ();

        clog << "Read from the temporary file: " << endl;
        while (ftmp.in () && ! ftmp.in ().eof ())
          {
            string line;
            getline (ftmp.in (), line);
            cout << "Line is : '" << line << "'" << endl;
            ftmp.in () >> ws;
          }
      }

      {
        datatools::temp_file ftmp ("__missing", "temp_", false);
        ftmp.out () << "test: " << ftmp.get_filename() << endl;
      }
      {
        datatools::temp_file ftmp ("__new/sub", "temp_", false);
        ftmp.out () << "test: " << ftmp.get_filename() << endl;
      }
      try
        {
          datatools::temp_file ftmp ("/__forbidden", "temp_", false);
          ftmp.out () << "test: " << ftmp.get_filename() << endl;
        }
      catch (exception & x)
        {
          cerr << "WARNING: As expected, there was an error : "
               << x.what() << endl;
        }

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

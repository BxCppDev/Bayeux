// test_smart_filename.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <datatools/utils/smart_filename.h>
#include <datatools/utils/properties.h>
#include <datatools/utils/utils.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'datatools::smart_filename'!" << endl; 
  
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
              /* Here you may add more argument handlers... */
              { 
                clog << "warning: ignoring argument '" << argument << "'!" << endl; 
              }
            }
          iarg++;
        }
    
      using namespace datatools::utils;

      {
        cout << endl << "*** Single filename : " << endl;
        smart_filename sf1;
        smart_filename::make_single (sf1, "test.data");
        sf1.dump ();
        cout << "  Filename = '" << sf1[0] << "'" << endl;
        sf1.store_list_of_filenames ("test_smart_filename.lis", false);
        sf1.set ("test.log");
        sf1.dump ();
      }

      {
        cout << endl << "*** List of filenames : " << endl;
        smart_filename sf2;
        smart_filename::make_list (sf2);
        sf2.add ("test_a.data");
        sf2.add ("test_b.data");
        sf2.add ("test_c.data");
        sf2.add ("${HOME}/test_d.data");
        sf2.add ("${HOME}/test_e.data");
        sf2.add ("/tmp/test_f.data");
        sf2.add ("~/test_g.data");
        sf2.dump ();
        sf2.add ("~/test_h.data");
        sf2.add ("~/test_i.data");
        sf2.add ("~/test_j.data");
        for (int i = 0; i < sf2.size (); ++i)
          {
            cout << "  Filename = '" << sf2[i] << "'" << endl;
          }
        sf2.store_list_of_filenames ("test_smart_filename.lis", true);
      }

      {
        cout << endl << "*** Incremental filename : " << endl;
        smart_filename sf3;
        
        smart_filename::make_incremental (sf3, 
                                          "/tmp/${USER}",
                                          "file_",
                                          ".data",
                                          10,
                                          0,
                                          +1);
        
        sf3.dump ();
        for (int i = 0; i < sf3.size (); ++i)
          {
            cout << "  Filename = '" << sf3[i] << "'" << endl;
          }
        sf3.store_list_of_filenames ("test_smart_filename.lis", true);
      }

      {
        cout << endl << "*** List of filenames (2) : " << endl;
        smart_filename sf4;
        smart_filename::make_list (sf4, string ("test_smart_filename.lis"));
        sf4.dump ();
        for (int i = 0; i < sf4.size (); ++i)
          {
            cout << "  Filename = '" << sf4[i] << "'" << endl;
          }
      }

      {
        cout << endl << "*** Incremental filename (2) : " << endl;
        smart_filename sf5;
        
        smart_filename::make_incremental (sf5, 
                                          "/tmp/${USER}",
                                          "file2_",
                                          ".data",
                                          2,
                                          7,
                                          -1);
        
        sf5.dump ();
        for (int i = 0; i < sf5.size (); ++i)
          {
            cout << "  Filename = '" << sf5[i] << "'" << endl;
          }
      }

      {
        cout << endl << "*** Smart filename (from config file) : " << endl;
        smart_filename sf6;
        properties config6;
        string file6 = "${DATATOOLS_ROOT}/testing/config/test_smart_filename_1.conf";
        fetch_path_with_env (file6);
        properties::read_config (file6, config6);
        sf6.initialize (config6);
        sf6.dump ();
      }

      {
        cout << endl << "*** Smart filename (from config file) : " << endl;
        smart_filename sf7;
        properties config7;
        string file7 = "${DATATOOLS_ROOT}/testing/config/test_smart_filename_2.conf";
        fetch_path_with_env (file7);
        properties::read_config (file7, config7);
        sf7.initialize (config7);
        sf7.dump ();
      }

      {
        cout << endl << "*** Smart filename (from config file) : " << endl;
        smart_filename sf8;
        properties config8;
        string file8 = "${DATATOOLS_ROOT}/testing/config/test_smart_filename_3.conf";
        fetch_path_with_env (file8);
        properties::read_config (file8, config8);
        sf8.initialize (config8);
        sf8.dump ();
      }

      {
        cout << endl << "*** Smart filename (from config file) : " << endl;
        smart_filename sf9;
        properties config9;
        string file9 = "${DATATOOLS_ROOT}/testing/config/test_smart_filename_4.conf";
        fetch_path_with_env (file9);
        properties::read_config (file9, config9);
        sf9.initialize (config9);
        sf9.dump ();
      }

      {
        cout << endl << "*** Smart filename (from config file) : " << endl;
        smart_filename sf10;
        properties config10;
        string file10 = "${DATATOOLS_ROOT}/testing/config/test_smart_filename_5.conf";
        fetch_path_with_env (file10);
        properties::read_config (file10, config10);
        sf10.initialize (config10);
        sf10.dump ();
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

// end of test_smart_filename.cxx
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

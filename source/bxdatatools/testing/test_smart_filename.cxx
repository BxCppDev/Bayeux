// test_smart_filename.cxx

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// This Project:
#include <datatools/smart_filename.h>
#include <datatools/properties.h>
#include <datatools/utils.h>

using namespace std;

int main (/* int argc_, char ** argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'datatools::smart_filename'!" << endl;

      //bool debug = false;
      // int iarg = 1;
      // while (iarg < argc_)
      //   {
      //     string token = argv_[iarg];
      //     if (token[0] == '-')
      //       {
      //         string option = token;
      //         if ((option == "-d") || (option == "--debug"))
      //           {
      //             debug = true;
      //           }
      //         else
      //           {
      //             clog << "warning: ignoring option '" << option << "'!" << endl;
      //           }
      //       }
      //     else
      //       {
      //         string argument = token;
      //         /* Here you may add more argument handlers... */
      //         {
      //           clog << "warning: ignoring argument '" << argument << "'!" << endl;
      //         }
      //       }
      //     iarg++;
      //   }

      {
        cout << endl << "*** Single filename : " << endl;
        datatools::smart_filename sf1;
        datatools::smart_filename::make_single (sf1, "test.data");
        sf1.dump ();
        cout << "  Filename = '" << sf1[0] << "'" << endl;
        sf1.store_list_of_filenames ("test_smart_filename.lis", false);
        sf1.set_single ("test.log");
        sf1.dump ();

        if (sf1.has_filename ("test.log"))
          {
            clog << "Has filename '" << "test.log" << "'." << endl;
          }
      }

      {
        cout << endl << "*** List of filenames : " << endl;
        datatools::smart_filename sf2;
        datatools::smart_filename::make_list (sf2);
        sf2.add_to_list ("test_a.data");
        sf2.add_to_list ("test_b.data");
        sf2.add_to_list ("test_c.data");
        sf2.add_to_list ("${HOME}/test_d.data");
        sf2.add_to_list ("${HOME}/test_e.data");
        sf2.add_to_list ("/tmp/test_f.data");
        sf2.add_to_list ("~/test_g.data");
        sf2.dump ();
        sf2.add_to_list ("~/test_h.data");
        sf2.add_to_list ("~/test_i.data");
        sf2.add_to_list ("~/test_j.data");
        for (int i = 0; i < (int) sf2.size (); ++i)
          {
            cout << "  Filename = '" << sf2[i] << "'" << endl;
          }
        sf2.store_list_of_filenames ("test_smart_filename.lis", true);

        if (sf2.has_filename ("${HOME}/test_d.data"))
          {
            clog << "Has filename '" << "${HOME}/test_d.data" << "'." << endl;
          }
        }

      {
        cout << endl << "*** Incremental filename : " << endl;
        datatools::smart_filename sf3;
        datatools::smart_filename::make_incremental (sf3,
                                                     "/tmp/${USER}",
                                                     "file_",
                                                     ".data",
                                                     10,
                                                     0,
                                                     +1);

        sf3.dump ();
        for (int i = 0; i < (int) sf3.size (); ++i)
          {
            cout << "  Filename = '" << sf3[i] << "'" << endl;
          }
        sf3.store_list_of_filenames ("test_smart_filename.lis", true);
        if (sf3.has_filename ("/tmp/${USER}/file_3.data"))
          {
            clog << "Has filename '" << "/tmp/${USER}/file_3.data" << "'." << endl;
          }
      }

      {
        cout << endl << "*** List of filenames (2) : " << endl;
        datatools::smart_filename sf4;
        datatools::smart_filename::make_list (sf4, string ("test_smart_filename.lis"));
        sf4.dump ();
        for (int i = 0; i < (int) sf4.size (); ++i)
          {
            cout << "  Filename = '" << sf4[i] << "'" << endl;
          }
        if (sf4.has_filename ("/tmp/${USER}/file_6.data"))
          {
            clog << "Has filename '" << "/tmp/${USER}/file_6.data" << "'." << endl;
          }
       }

      {
        cout << endl << "*** Incremental filename (2) : " << endl;
        datatools::smart_filename sf5;

        datatools::smart_filename::make_incremental (sf5,
                                          "/tmp/${USER}",
                                          "file2_",
                                          ".data",
                                          2,
                                          7,
                                          -1);

        sf5.dump ();
        for (int i = 0; i < (int) sf5.size (); ++i)
          {
            cout << "  Filename = '" << sf5[i] << "'" << endl;
          }
        if (! sf5.has_filename ("/tmp/${USER}/file2_1.data"))
          {
            clog << "Has no filename '" << "/tmp/${USER}/file2_1.data" << "'." << endl;
          }
     }

      {
        cout << endl << "*** Smart filename (from config file) : " << endl;
        datatools::smart_filename sf6;
        datatools::properties config6;
        string file6 = "${DATATOOLS_TESTING_DIR}/config/test_smart_filename_1.conf";
        datatools::fetch_path_with_env (file6);
        datatools::read_config (file6, config6);
        sf6.initialize (config6);
        sf6.dump ();
        if (sf6.has_filename ("${DATATOOLS_TESTING_DIR}/test_smart_filename.cxx"))
          {
            clog << "Has filename '" << "${DATATOOLS_TESTING_DIR}/test_smart_filename.cxx" << "'." << endl;
          }
      }

      {
        cout << endl << "*** Smart filename (from config file) : " << endl;
        datatools::smart_filename sf7;
        datatools::properties config7;
        string file7 = "${DATATOOLS_TESTING_DIR}/config/test_smart_filename_2.conf";
        datatools::fetch_path_with_env (file7);
        datatools::read_config (file7, config7);
        sf7.initialize (config7);
        sf7.dump ();
        if (sf7.has_filename ("/tmp/mauger/file_3.data"))
          {
            clog << "Has filename '" << "/tmp/mauger/file_3.data" << "'." << endl;
          }
        if (! sf7.has_filename ("/tmp/mauger/file_11.data"))
          {
            clog << "Has no filename '" << "/tmp/mauger/file_11.data" << "'." << endl;
          }
       }

      {
        cout << endl << "*** Smart filename (from config file) : " << endl;
        datatools::smart_filename sf8;
        datatools::properties config8;
        string file8 = "${DATATOOLS_TESTING_DIR}/config/test_smart_filename_3.conf";
        datatools::fetch_path_with_env (file8);
        datatools::read_config (file8, config8);
        sf8.initialize (config8);
        sf8.dump ();
        if (sf8.has_filename ("test_a.data"))
          {
            clog << "Has filename '" << "test_a.data" << "'." << endl;
          }
      }

      {
        cout << endl << "*** Smart filename (from config file) : " << endl;
        datatools::smart_filename sf9;
        datatools::properties config9;
        string file9 = "${DATATOOLS_TESTING_DIR}/config/test_smart_filename_4.conf";
        datatools::fetch_path_with_env (file9);
        datatools::read_config (file9, config9);
        sf9.initialize (config9);
        sf9.dump ();
        if (! sf9.has_filename ("/tmp/${USER}/run_1.data"))
          {
            clog << "Has no filename '" << "/tmp/${USER}/run_1.data" << "'." << endl;
          }
        if (! sf9.has_filename ("/tmp/${USER}/run_2.data"))
          {
            clog << "Has no filename '" << "/tmp/${USER}/run_2.data" << "'." << endl;
          }
        if (sf9.has_filename ("/tmp/${USER}/run_3.data"))
          {
            clog << "Has filename '" << "/tmp/${USER}/run_3.data" << "'." << endl;
          }
        if (sf9.has_filename ("/tmp/${USER}/run_7.data"))
          {
            clog << "Has filename '" << "/tmp/${USER}/run_7.data" << "'." << endl;
          }
         if (! sf9.has_filename ("/tmp/${USER}/run_8.data"))
          {
            clog << "Has no filename '" << "/tmp/${USER}/run_8.data" << "'." << endl;
          }
     }

      {
        cout << endl << "*** Smart filename (from config file) : " << endl;
        datatools::smart_filename sf10;
        datatools::properties config10;
        string file10 = "${DATATOOLS_TESTING_DIR}/config/test_smart_filename_5.conf";
        datatools::fetch_path_with_env (file10);
        datatools::read_config (file10, config10);
        sf10.initialize (config10);
        sf10.dump ();
        if (! sf10.has_filename ("/tmp/${USER}/run_11.data"))
          {
            clog << "Has no filename '" << "/tmp/${USER}/run_11.data" << "'." << endl;
          }
        if (sf10.has_filename ("/tmp/${USER}/run_10.data"))
          {
            clog << "Has filename '" << "/tmp/${USER}/run_10.data" << "'." << endl;
          }
        if (sf10.has_filename ("/tmp/${USER}/run_0.data"))
          {
            clog << "Has filename '" << "/tmp/${USER}/run_0.data" << "'." << endl;
          }
       }

      {
        cout << endl << "*** Unranged incremental filename : " << endl;
        datatools::smart_filename sf11;

        datatools::smart_filename::make_unranged_incremental (sf11,
                                                   "/tmp/${USER}",
                                                   "head_",
                                                   ".data",
                                                   0,
                                                   +1,
                                                   "_tail",
                                                   4,
                                                   true);

        sf11.dump ();
        for (int i = 0; i < 25; ++i)
          {
            cout << "  Filename = '" << sf11[i] << "'" << endl;
          }
        sf11.store_list_of_filenames ("test_smart_filename.lis", true);
        if (! sf11.has_filename ("/tmp/${USER}/head_0_tail.data"))
          {
            clog << "Has no filename '" << "/tmp/${USER}/head_0_tail.data" << "'." << endl;
          }
        if (sf11.has_filename ("/tmp/${USER}/head_0000_tail.data"))
          {
            clog << "Has filename '" << "/tmp/${USER}/head_0000_tail.data" << "'." << endl;
          }
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

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

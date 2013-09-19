// -*- mode: c++; -*-
// test_genbb_writer.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <genbb_help/genbb_writer.h>
#include <genbb_help/primary_event.h>

using namespace std;

void usage (ostream & a_out)
{
  a_out << "\ntest_genbb_writer usage:" << endl;
  a_out << "\n To generate a GENBB files with tuned properties:" << endl;
  a_out << "   shell>$ test_genbb_writer [options]" << endl;
  a_out << "\nExample: " << endl;
  a_out << "   shell>$ test_genbb_writer -d -N 100000 --type e-"
        << endl;
  a_out << "                 --energy-min 0. --energy-max 5."
        << endl;
  a_out << "                 --phi-min 0. --phi-max 90."
        << endl;
  a_out << "                 --seed 314159"
        << endl;
  a_out << "                 --outfile /scratch/test_genbb_writer.data"
        << endl;
  a_out << endl;
  a_out << endl;
  return;
}


int main (int a_argc, char ** a_argv)
{
  int error_code = EXIT_SUCCESS;
  try {
    bool debug = false;

    int N = 3;
    int seed = 314159;
    string type = "e-";
    double Emin = 0.0, Emax = 4.0;
    double phi_min = 0., phi_max = 30.;
    string file_out = "test_genbb_writer.data";

    int iarg = 1;
    while (iarg < a_argc) {
      string arg = a_argv[iarg];
      if (arg[0] == '-') {
        if (arg == "-d" || arg == "--debug" ) {
          debug = true;
        } else if (arg == "-h" || arg == "--help" ) {
          usage ( clog );
          return 0;
        } else if ( arg == "-s" || arg == "--seed" )  {
          if ( ++iarg < a_argc )
            seed = atoi ( a_argv[iarg] );
        } else if ( arg == "-N")  {
          if ( ++iarg < a_argc )
            N = atoi ( a_argv[iarg] );
        } else if ( arg == "-t" || arg == "--type" ) {
          if (++iarg < a_argc)
            type = a_argv[iarg];
        } else if ( arg == "-Emin" || arg == "--energy-min") {
          if ( ++iarg < a_argc )
            Emin = atof ( a_argv[iarg] );
        } else if ( arg == "-Emax" || arg == "--energy-max") {
          if ( ++iarg < a_argc )
            Emax = atof ( a_argv[iarg] );
        } else if ( arg == "-phi_min" || arg == "--phi-min") {
          if ( ++iarg < a_argc )
            phi_min = atof ( a_argv[iarg] );
        } else if ( arg == "-phi_max" || arg == "--phi-max") {
          if ( ++iarg < a_argc )
            phi_max = atof ( a_argv[iarg] );
        } else if (arg == "-o" || arg == "--outfile") {
          if (++iarg < a_argc)
            file_out = a_argv[iarg];
        }
      }
      iarg++;
    }

    if ( N < 0 || Emin < 0 || Emax < 0 ||
         phi_min < 0 || phi_max > 180. ) {
      usage ( clog );
      return 1;
    }
    genbb::genbb_writer writer;

    writer.set_npart        ( N );
    writer.set_part_per_evt ( 1 );
    writer.set_seed         ( seed );
    writer.set_Emin         ( Emin );
    writer.set_Emax         ( Emax );
    writer.set_phi_min      ( phi_min * M_PI / 180. );
    writer.set_phi_max      ( phi_max * M_PI / 180. );
    writer.set_outfile      ( file_out );

    if ( type == "electron" || type == "e-" )
      writer.set_type ( 3 );
    else if ( type == "positron" || type == "e+" )
      writer.set_type ( 2 );
    else if ( type == "gamma" || type == "g" )
      writer.set_type ( 1 );
    else if ( type == "alpha" || type == "a" )
      writer.set_type ( 47 );
    else
      writer.set_type ( -1 );

    if ( debug )
      writer.dump( clog );

    writer.run();

  }
  catch (exception & x) {
    cerr << "error: " << x.what () << endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    cerr << "error: " << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

// end of test_genbb_writer.cxx

// -*- mode: c++; -*-
// test_bb0nu_cos_theta.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <boost/algorithm/string.hpp>

#include <datatools/ioutils.h>
#include <datatools/utils.h>

#include <genbb_help/genbb_mgr.h>

int main (int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Hello, this is a sample program for class 'genbb_mgr'!"
         << endl;

    bool debug = false;
    string input_file;

    int iarg = 1;
    while (iarg < argc_) {
      string arg = argv_[iarg];

      if (arg[0] == '-') {
        if (arg == "-d" || arg == "--debug") debug = true;
      } else {
        if (input_file.empty ()) {
          input_file = arg;
        }
      }
      iarg++;
    }
    if (input_file.empty ()) {
      clog << datatools::io::notice << "Using default input_file !" << endl;
      input_file = "${GENBB_HELP_DATA_DIR}/testing/data/se82_0nubb_mn.genbb";
    }
    datatools::fetch_path_with_env (input_file);
    clog << datatools::io::notice << "input_file = '" << input_file << "'" << endl;
    genbb::genbb_mgr mgr;

    if (boost::algorithm::iends_with (input_file, ".genbb")) {
      clog << datatools::io::notice << "Use GENBB input file format." << endl;
      mgr.set_format (genbb::genbb_mgr::FORMAT_GENBB);
    }

    mgr.set (input_file.c_str ());
    if (debug)  mgr.dump ();
    mgr.initialize_simple ();
    mgr.dump ();

    double me = CLHEP::electron_mass_c2;
    genbb::primary_event pe;
    while (mgr.has_next ()) {
      mgr.load_next (pe);
      if (debug) pe.dump ();
      double e_sum = 0.0;
      if (debug) clog << datatools::io::debug << "me="
                      << me / CLHEP::MeV
                      << " MeV" << endl;
      if (pe.get_particles ().size ()== 2) {
        genbb::primary_particle pp1 = pe.grab_particles ().front ();
        pe.grab_particles ().pop_front ();
        genbb::primary_particle pp2 = pe.grab_particles ().front ();
        pe.grab_particles ().pop_front ();
        if (! pp1.is_electron () && ! pp2.is_electron ()) {
          continue;
        }
        if (debug) {
          clog << datatools::io::debug << "p1=" << pp1.get_momentum () / CLHEP::MeV << endl;
          clog << datatools::io::debug << "p2=" << pp2.get_momentum () / CLHEP::MeV << endl;
        }
        double p1 = pp1.get_momentum ().mag ();
        double p2 = pp2.get_momentum ().mag ();
        if (debug) {
          clog << datatools::io::debug << "|p1|=" << p1 / CLHEP::MeV  << endl;
          clog << datatools::io::debug << "|p2|=" << p2 / CLHEP::MeV  << endl;
        }
        double p1_dot_p2 = pp1.get_momentum () * pp2.get_momentum ();
        double cos_theta = p1_dot_p2 / (p1 * p2);
        cout << cos_theta << endl;
      }
    }
    mgr.reset ();
    if (debug) mgr.dump ();

  }
  catch (exception & x) {
    cerr << datatools::io::error << x.what () << endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    cerr << datatools::io::error << "Unexpected error !" << endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

// end of test_bb0nu_cos_theta.cxx

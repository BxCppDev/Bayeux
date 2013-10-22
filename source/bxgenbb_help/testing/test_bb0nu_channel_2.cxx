// -*- mode: c++; -*-
// test_bb0nu_channel_2.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <cmath>

#include <boost/algorithm/string.hpp>

#include <mygsl/rng.h>

#include <datatools/ioutils.h>
#include <datatools/utils.h>

#include <genbb_help/genbb_mgr.h>
#include <genbb_help/bio_guard.h>

int main (int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    bool debug = false;
    string input_file;
    string rng_id              = "mt19937";
    unsigned long int rng_seed = 12345;
    double energy_resolution = 0.0;
    double energy_min        = 0.0;
    double energy_sum_min    = 0.0;
    double energy_sum_max    = 10000.0 * CLHEP::keV;

    int iarg = 1;
    while (iarg < argc_) {
      string arg = argv_[iarg];

      if (arg[0] == '-') {
        if (arg == "-d" || arg == "--debug") {
          debug = true;
        }

        if (arg == "-r" || arg == "--energy-resolution") {
          iarg++;
          string token = argv_[iarg];
          istringstream iss (token);
          iss >> energy_resolution;
          if (! iss) {
            ostringstream message;
            message << "Invalid format for option '--energy-resolution' !";
            throw runtime_error (message.str ());
          }
        }

        if (arg == "-s" || arg == "--seed") {
          iarg++;
          string token = argv_[iarg];
          istringstream iss (token);
          iss >> rng_seed;
          if (! iss) {
            ostringstream message;
            message << "Invalid format for option '--seed' !";
            throw runtime_error (message.str ());
          }
        }

        if (arg == "-m" || arg == "--energy-min") {
          iarg++;
          string token = argv_[iarg];
          istringstream iss (token);
          iss >> energy_min;
          if (! iss) {
            ostringstream message;
            message << "Invalid format for option '--energy-min' !";
            throw runtime_error (message.str ());
          }
          energy_min *= CLHEP::keV;
        }

        if (arg == "-sm" || arg == "--energy-sum-min") {
          iarg++;
          string token = argv_[iarg];
          istringstream iss (token);
          iss >> energy_sum_min;
          if (! iss) {
            ostringstream message;
            message << "Invalid format for option '--energy-sum-min' !";
            throw runtime_error (message.str ());
          }
          energy_sum_min *= CLHEP::keV;
        }

        if (arg == "-sM" || arg == "--energy-sum-max") {
          iarg++;
          string token = argv_[iarg];
          istringstream iss (token);
          iss >> energy_sum_max;
          if (! iss) {
            ostringstream message;
            message << "Invalid format for option '--energy-sum-max' !";
            throw runtime_error (message.str ());
          }
          energy_sum_max *= CLHEP::keV;
        }

      } else {
        if (input_file.empty ()) {
          input_file = arg;
        }
      }
      iarg++;
    }

    if (input_file.empty ()) {
      clog << datatools::io::notice << "Using default input_file !" << endl;
      input_file = "${GENBB_HELP_TESTING_DIR}/data/se82_0nubb_mn.genbb";
    }
    datatools::fetch_path_with_env (input_file);
    clog << datatools::io::notice << "input_file        = '" << input_file << "'" << endl;

    clog << datatools::io::notice << "energy_resolution = " << energy_resolution
         << "" << endl;
    clog << datatools::io::notice << "energy_min        = " << energy_min / CLHEP::keV
         << " keV" << endl;
    clog << datatools::io::notice << "energy_sum_min    = " << energy_sum_min / CLHEP::keV
         << " keV" << endl;
    clog << datatools::io::notice << "energy_sum_max    = " << energy_sum_max  / CLHEP::keV
         << " keV" << endl;
    clog << datatools::io::notice << "rng_seed          = " << rng_seed << endl;

    mygsl::rng r;
    r.init (rng_id, rng_seed);

    genbb::genbb_mgr mgr;

    if (boost::algorithm::iends_with (input_file, ".genbb")) {
      clog << datatools::io::notice << "Use GENBB input file format." << endl;
      mgr.set_format (genbb::genbb_mgr::FORMAT_GENBB);
    }

    mgr.set (input_file.c_str ());
    if (debug) mgr.dump ();
    mgr.initialize_simple ();
    mgr.dump ();

    double me = CLHEP::electron_mass_c2;
    genbb::primary_event pe;
    int count = 0;
    int selected_count = 0;
    double factor = 2. * sqrt (2. * log (2));
    clog << datatools::io::notice << "factor=" << factor << endl;
    while (mgr.has_next ()) {
      mgr.load_next (pe);
      if (debug) pe.dump ();
      double e_sum = 0.0;
      count++;
      if (debug) clog << datatools::io::debug << "me="
                      << me / CLHEP::MeV
                      << " MeV" << endl;
      if (pe.get_particles ().size () == 2) {
        genbb::primary_particle pp1 = pe.grab_particles ().front ();
        pe.grab_particles ().pop_front ();
        genbb::primary_particle pp2 = pe.grab_particles ().front ();
        pe.grab_particles ().pop_front ();
        if (! pp1.is_electron () && ! pp2.is_electron ()) {
          continue;
        }
        if (debug)  {
          clog << datatools::io::debug << "p1=" << pp1.get_momentum () / CLHEP::MeV << " MeV " << endl;
          clog << datatools::io::debug << "p2=" << pp2.get_momentum () / CLHEP::MeV << " MeV " << endl;
        }
        double p1 = pp1.get_momentum ().mag ();
        double p2 = pp2.get_momentum ().mag ();
        if (debug)  {
          clog << datatools::io::debug << "|p1|=" << p1 / CLHEP::MeV  << " MeV " << endl;
          clog << datatools::io::debug << "|p2|=" << p2 / CLHEP::MeV  << " MeV " << endl;
        }
        double p1_dot_p2 = pp1.get_momentum () * pp2.get_momentum ();
        double cos_theta = p1_dot_p2 / (p1 * p2);
        double e1 = pp1.get_kinetic_energy ();
        double e2 = pp2.get_kinetic_energy ();

        double fwhm1 = energy_resolution * sqrt (e1 / CLHEP::MeV) * CLHEP::MeV;
        double fwhm2 = energy_resolution * sqrt (e2 / CLHEP::MeV) * CLHEP::MeV;

        double sigma_e1 = fwhm1 / factor;
        double sigma_e2 = fwhm2 / factor;
        if (sigma_e1 > 0.0) {
          e1 = r.gaussian (e1, sigma_e1);
          if (e1 < 0.0) e1 = 0.0;
        }
        if (sigma_e2 > 0.0) {
          e2 = r.gaussian (e2, sigma_e2);
          if (e2 < 0.0) e2 = 0.0;
        }
        double esum = e1 + e2;
        if (debug)  {
          clog << datatools::io::debug << "e1             = " << e1 / CLHEP::MeV  << " MeV " << endl;
          clog << datatools::io::debug << "e2             = " << e2 / CLHEP::MeV  << " MeV " << endl;
          clog << datatools::io::debug << "energy_min     = " << energy_min / CLHEP::MeV << " MeV " << endl;
          clog << datatools::io::debug << "esum           = " << esum / CLHEP::MeV << " MeV " << endl;
          clog << datatools::io::debug << "energy_sum_min = " << energy_sum_min / CLHEP::MeV << " MeV " << endl;
          clog << datatools::io::debug << "energy_sum_max = " << energy_sum_max / CLHEP::MeV << " MeV " << endl;
        }
        if ((e1 > energy_min)
            && (e2 > energy_min)
            && (esum > energy_sum_min)
            && (esum < energy_sum_max)) {
          //clog << datatools::io::notice << "Selected !" << endl;
          selected_count++;
          cout << e1 / CLHEP::keV  << ' '
               << pp1.get_momentum ().x () / CLHEP::keV  << ' '
               << pp1.get_momentum ().y () / CLHEP::keV  << ' '
               << pp1.get_momentum ().z () / CLHEP::keV  << ' '
               << e2 / CLHEP::keV << ' '
               << pp2.get_momentum ().x () / CLHEP::keV  << ' '
               << pp2.get_momentum ().y () / CLHEP::keV  << ' '
               << pp2.get_momentum ().z () / CLHEP::keV  << ' '
               << esum / CLHEP::keV << ' '
               << cos_theta << ' '
               << endl;
        } else  {
          //clog << datatools::io::notice << "Not selected !" << endl;
        }
      }
    }
    mgr.reset ();
    double efficiency = selected_count * 1.0 / count;
    clog << datatools::io::notice << "Efficiency=" << efficiency << endl;
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

// end of test_bb0nu_channel_2.cxx

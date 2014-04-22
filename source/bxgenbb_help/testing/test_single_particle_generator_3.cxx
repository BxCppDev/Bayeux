// -*- mode: c++ ; -*-
// test_single_particle_generator_3.cxx
/*
 * Usage:
 *
 * shell> ./Linux-i686/bin/test_single_particle_generator_3 \
 *   | gsl-histogram 0 2 100 > ke.his
 *
 * gnuplot> plot './ke.his' using 1:3 with histep
 *
 */

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <genbb_help/single_particle_generator.h>
#include <genbb_help/primary_event.h>


int main (int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test program for class 'single_particle_generator'!" << endl;

    bool debug = false;
    int  many = 0;
    bool randomized_direction = false;
    // bool energy_range_mode = false;

    string spectrum_data_file = "";

    int iarg = 1;
    while (iarg < argc_) {
      string token = argv_[iarg];

      if (token[0] == '-') {
        string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        } else if ((option == "-m") || (option == "--many")) {
          many = 1;
        } else if ((option == "-M") || (option == "--many-many")) {
          many = 2;
        } else if ((option == "-S") || (option == "--so-many")) {
          many = 3;
        } else if ((option == "-H") || (option == "--huge-many")) {
          many = 4;
        } else if ((option == "-i") || (option == "--spectrum-data-file")) {
          spectrum_data_file = argv_[++iarg];
        } else if ((option == "-r") || (option == "--randomized-direction")) {
          randomized_direction = true;
        } else {
          clog << "warning: ignoring option '" << option << "'!" << endl;
        }
      } else {
        string argument = token;
        {
          clog << "warning: ignoring argument '" << argument << "'!" << endl;
        }
      }
      iarg++;
    }

    if (spectrum_data_file.empty ()) {
      // generate an histogram
      const size_t nbins = 100;
      const double xmin = 0.0;
      const double xmax = 10.0;
      mygsl::histogram h (nbins, xmin, xmax);

      const size_t nshoots = 1000;
      for (int i= 0; i < (int) nshoots; i++) {
        double x = -std::log(drand48());
        h.fill(x);
      }
      h.dump(std::cerr);

      spectrum_data_file = "test_histogram.data";
      std::ofstream ofhist (spectrum_data_file.c_str());
      ofhist << "# energy spectrum for the 'single_particle_generator'" << endl;
      ofhist << "#@energy_unit MeV" << endl;
      ofhist << "#@limits " << nbins << ' ' << xmin << ' ' << xmax << endl;
      h.print (ofhist);
      ofhist.close ();
    }

    genbb::single_particle_generator SPG;

    // Build a configuration properties container:
    datatools::properties config;
    if (debug) config.store_flag ("debug");

    /* The seed of the random number generator */
    config.store ("seed", 314159);

    /* Default momentum direction is along the Z axis */
    config.store ("randomized_direction", randomized_direction);

    /* Possible values are:
     *   "electron" or "e-"
     *   "positron" or "e+"
     *   "gamma"
     *   "alpha"
     *   "neutron"
     *   "protron"
     *   a few others...
     */
    config.store ("particle_name", "electron");

    /* Possible values are:
     *  - "gaussian_energy" and required properties are:
     *    - "mean_energy"
     *    - "sigma_energy"
     *  - "energy_range" and required properties are:
     *    - "min_energy"
     *    - "max_energy"
     *  - "monokinetic" and required propertie is:
     *    - "energy"
     */
    config.store ("mode", "spectrum");
    /* Possible values are:
     *  - "histogram_pdf" to get a random value
     *  from Alastair J. Walker lookup table
     *  - "tabulated_function" to get a random value
     *  using Von Neumann method
     */
    config.store ("spectrum.mode", "histogram_pdf");
    config.store ("spectrum.data_file", spectrum_data_file);

    /* Possible values are: "keV", "MeV"... */
    config.store ("energy_unit", "MeV");
    config.store ("spectrum.min_energy", 0.0);
    config.store ("spectrum.max_energy", 10.0);

    if (debug) config.tree_dump (clog, "Configuration: ", "debug: ");

    // Configure the event generator:
    SPG.initialize_standalone (config);

    // Shoot events:
    size_t max_count = 10;
    if (many == 1) max_count = 100;
    if (many == 2) max_count = 1000;
    if (many == 3) max_count = 10000;
    if (many == 4) max_count = 100000;
    genbb::primary_event pe; // working primary event
    for (int i = 0; i < (int) max_count; i++) {
      if (debug) clog << "Count : " << i << endl;
      SPG.load_next (pe);
      if (debug) pe.dump ();
      // Print the kinetic energy from the only particle in each event:
      cout << pe.get_particles ().front ().get_kinetic_energy () << endl;
    }

    if (debug) clog << "debug: " << "The end." << endl;

  }
  catch (exception & x) {
    cerr << "error: " << x.what () << endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    cerr << "error: " << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

// end of test_single_particle_generator_3.cxx

// -*- mode: c++; -*-
// test_wdecay0.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// This project:
#include <genbb_help/wdecay0.h>
#include <genbb_help/primary_event.h>
#include <mygsl/histogram.h>

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    bool debug = false;
    bool dump  = false;
    int  max_count = 10;
    bool high_energy_cut = false;
    bool zero_nu = false;
    int dbd_mode = 4;
    std::string isotope = "Se82";
    
    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];
      if (arg == "-d" || arg == "--debug") debug = true;
      if (arg == "-D" || arg == "--dump") dump = true;
      if (arg == "-L" || arg == "--lots") max_count = 100000;
      if (arg == "-H" || arg == "--high-energy-cut")  high_energy_cut = true;
      if (arg == "-Z" || arg == "--0nubb") zero_nu = true;
      if (arg == "-M" || arg == "--Mo100") isotope =  "Mo100";
      iarg++;
    }

    if (zero_nu) {
      dbd_mode = 1;
    }
    datatools::properties config;
    config.store("seed", 314159);
    config.store("decay_type", "DBD");
    config.store("decay_isotope", isotope);
    config.store("decay_dbd_level", 0);
    config.store("decay_dbd_mode", dbd_mode);
    if (high_energy_cut) {
      config.store("energy_min", 2.0);
      config.store("energy_max", 3.2);
      config.store("energy_unit", "MeV");
    }
    if (debug) config.store("logging.priority", "trace");
    if (debug) config.tree_dump(std::clog, "Configuration: ", "debug: ");

    genbb::wdecay0 WD0;
    if (debug) WD0.set_logging_priority(datatools::logger::PRIO_DEBUG);
    WD0.initialize_standalone(config);

    // Working primary event:
    genbb::primary_event pe;

    // Histograms:
    mygsl::histogram h_esum(320, 0.0, 3.2);
    mygsl::histogram h_e1(320, 0.0, 3.2);
    mygsl::histogram h_e2(320, 0.0, 3.2);
    mygsl::histogram h_cos12(50.0, -1.0, 1.0);
    
    for (int i = 0; i < (int) max_count; i++) {
      if (((i % 1000) == 0) || (i + 1) == max_count) {
        std::clog << "Count : " << i << std::endl;
      }
      WD0.load_next(pe);
      if (dump) pe.dump();
      double e1 = pe.get_particle(0).get_kinetic_energy();
      double e2 = pe.get_particle(1).get_kinetic_energy();
      double esum = e1 + e2;
      const auto & p1 =  pe.get_particle(0).get_momentum();
      const auto & p2 =  pe.get_particle(1).get_momentum();
      double cos12 = p1.dot(p2) / p1.mag() / p2.mag();
      h_esum.fill(esum / CLHEP::MeV);
      h_e1.fill(e1 / CLHEP::MeV);
      h_e2.fill(e2 / CLHEP::MeV);
      h_cos12.fill(cos12);
    }
    h_esum.print_ascii(std::cout);
    {
      std::string sname = "test_wdecay0-esum.his";
      std::ofstream ofhist(sname.c_str());
      h_esum.print(ofhist);
      ofhist.close();
    }
    {
      std::string sname = "test_wdecay0-e1.his";
      std::ofstream ofhist(sname.c_str());
      h_e1.print(ofhist);
      ofhist.close();
    }
    {
      std::string sname = "test_wdecay0-e2.his";
      std::ofstream ofhist(sname.c_str());
      h_e2.print(ofhist);
      ofhist.close();
    }
    {
      std::string sname = "test_wdecay0-cos12.his";
      std::ofstream ofhist(sname.c_str());
      h_cos12.print(ofhist);
      ofhist.close();
    }
    if (debug) std::clog << "debug: " << "The end." << std::endl;
  } catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

// end of test_wgenbb.cxx

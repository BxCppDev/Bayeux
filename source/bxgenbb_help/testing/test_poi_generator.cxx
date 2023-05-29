// -*- mode: c++; -*-
// test_poi_generator.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// This project:
#include <genbb_help/poi_generator.h>
#include <genbb_help/wdecay0.h>
#include <genbb_help/primary_event.h>
#include <mygsl/histogram.h>

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    bool debug = false;
    bool dump  = false;
    int max_count = 10;
    std::string isotope = "Bi207";
    
    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];
      if (arg == "-d" || arg == "--debug") debug = true;
      if (arg == "-D" || arg == "--dump") dump = true;
      if (arg == "-L" || arg == "--lots") max_count = 1000;
      iarg++;
    }
    datatools::properties config;
    config.store("seed", 314159);
    config.store("decay_type", "background");
    config.store("decay_isotope", isotope);
    if (debug) config.store("logging.priority", "trace");
    if (debug) config.tree_dump(std::clog, "Configuration: ", "debug: ");

    genbb::wdecay0 WD0;
    if (debug) WD0.set_logging_priority(datatools::logger::PRIO_DEBUG);
    WD0.initialize_standalone(config);

    genbb::poi_generator poiGen;
    poiGen.set_generator(WD0);
    datatools::properties poiConfig;
    poiConfig.store("poi.number_of_particules", 2);
    poiConfig.store("poi.particule_0.type", "electron");
    poiConfig.store_real_with_explicit_unit("poi.particule_0.min_energy",
					    500. * CLHEP::keV);
    poiConfig.set_unit_symbol("poi.particule_0.min_energy", "keV");
    poiConfig.store("poi.particule_1.type", "electron");
    poiConfig.store_real_with_explicit_unit("poi.particule_1.min_energy",
					    500. * CLHEP::keV);
    poiConfig.set_unit_symbol("poi.particule_1.min_energy", "keV");
    poiGen.initialize_standalone(poiConfig);

    // Working primary event:
    genbb::primary_event pe;

    // Histograms:
    mygsl::histogram h_e1(1500, 0.0, 1.5);
    mygsl::histogram h_e2(1500, 0.0, 1.5);
    mygsl::histogram h_p1(1500, 0.0, 1.5);
    mygsl::histogram h_p2(1500, 0.0, 1.5);
    mygsl::histogram h_g1(1500, 0.0, 1.5);
    mygsl::histogram h_g2(1500, 0.0, 1.5);
    
    for (int i = 0; i < (int) max_count; i++) {
      if (((i % 1000) == 0) || (i + 1) == max_count) {
        std::clog << "Count : " << i << std::endl;
      }
      if (not poiGen.has_next()) {
	std::clog << "No next event\n";
	break;
      } else {
	poiGen.load_next(pe);
	if (dump) {
	  boost::property_tree::ptree popts;
	  popts.put("title", "Generated primary event");
	  pe.print_tree(std::clog, popts);
	}
	const auto & E1 = pe.get_particle_of_type(genbb::primary_particle::ELECTRON, 0);
	const auto & E2 = pe.get_particle_of_type(genbb::primary_particle::ELECTRON, 1);
	const auto & P1 = pe.get_particle_of_type(genbb::primary_particle::POSITRON, 0);
	const auto & P2 = pe.get_particle_of_type(genbb::primary_particle::POSITRON, 1);
	const auto & G1 = pe.get_particle_of_type(genbb::primary_particle::GAMMA, 0);
	const auto & G2 = pe.get_particle_of_type(genbb::primary_particle::GAMMA, 1);
	if (E1) {
	  double ke1 = E1->get_kinetic_energy();
	  h_e1.fill(ke1 / CLHEP::MeV);
	}
	if (E2) {
	  double ke2 = E2->get_kinetic_energy();
	  h_e2.fill(ke2 / CLHEP::MeV);
	}
	if (P1) {
	  double kp1 = P1->get_kinetic_energy();
	  h_p1.fill(kp1 / CLHEP::MeV);
	}
	if (P2) {
	  double kp2 = P2->get_kinetic_energy();
	  h_p2.fill(kp2 / CLHEP::MeV);
	}
	if (G1) {
	  double kg1 = G1->get_kinetic_energy();
	  h_g1.fill(kg1 / CLHEP::MeV);
	}
	if (G2) {
	  double kg2 = G2->get_kinetic_energy();
	  h_g2.fill(kg2 / CLHEP::MeV);
	}
      }
    } // loop
    std::clog << "# original events = " << poiGen.get_original_event_counter() << '\n';
    std::clog << "# selected events = " << poiGen.get_event_counter() << '\n';
    
    {
      std::string sname = "test_poi_generator-e1.his";
      std::ofstream ofhist(sname.c_str());
      h_e1.print(ofhist);
      ofhist.close();
    }
    {
      std::string sname = "test_poi_generator-e2.his";
      std::ofstream ofhist(sname.c_str());
      h_e2.print(ofhist);
      ofhist.close();
    }
    {
      std::string sname = "test_poi_generator-p1.his";
      std::ofstream ofhist(sname.c_str());
      h_p1.print(ofhist);
      ofhist.close();
    }
    {
      std::string sname = "test_poi_generator-p2.his";
      std::ofstream ofhist(sname.c_str());
      h_p2.print(ofhist);
      ofhist.close();
    }
    {
      std::string sname = "test_poi_generator-g1.his";
      std::ofstream ofhist(sname.c_str());
      h_g1.print(ofhist);
      ofhist.close();
    }
    {
      std::string sname = "test_poi_generator-g2.his";
      std::ofstream ofhist(sname.c_str());
      h_g2.print(ofhist);
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

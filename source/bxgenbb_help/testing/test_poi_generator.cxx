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
    bool original = false;
    int max_count = 4000;
    std::string isotope = "Bi207";
    
    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];
      if (arg == "-d" || arg == "--debug") debug = true;
      if (arg == "-D" || arg == "--dump") dump = true;
      if (arg == "-L" || arg == "--lots") max_count = 1000;
      if (arg == "-O" || arg == "--original") original = true;
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
					    400. * CLHEP::keV);
    poiConfig.set_unit_symbol("poi.particule_0.min_energy", "keV");
    poiConfig.store("poi.particule_1.type", "electron");
    poiConfig.store_real_with_explicit_unit("poi.particule_1.min_energy",
					    400. * CLHEP::keV);
    poiConfig.set_unit_symbol("poi.particule_1.min_energy", "keV");
    poiGen.initialize_standalone(poiConfig);

    // Working primary event:
    genbb::primary_event pe;

    // Histograms:
    mygsl::histogram h_e1(1500, 0.0, 1.5);
    mygsl::histogram h_p1(1500, 0.0, 1.5);
    mygsl::histogram h_g1(1500, 0.0, 1.5);
    mygsl::histogram h_npart(10, 0.0, 10.0);
    mygsl::histogram h_ne(10, 0.0, 10.0);
    mygsl::histogram h_ng(10, 0.0, 10.0);
    mygsl::histogram h_ne_he(10, 0.0, 10.0);
 
    genbb::i_genbb * generator = &poiGen;
    if (original) {
      generator = &WD0;
    }
    
    for (int i = 0; i < (int) max_count; i++) {
      if (((i % 1000) == 0) || (i + 1) == max_count) {
        std::clog << "Count : " << i << std::endl;
      }
      if (not generator->has_next()) {
	std::clog << "No next event\n";
	break;
      } else {
	generator->load_next(pe);
	if (dump) {
	  boost::property_tree::ptree popts;
	  popts.put("title", "Generated primary event");
	  pe.print_tree(std::clog, popts);
	}
	int npart = pe.get_number_of_particles();
	int ng = 0;
	int ne = 0;
	int ne_he = 0;
	for (std::size_t iPart = 0; iPart < pe.get_number_of_particles(); iPart++) {
	  const auto & part = pe.get_particle(iPart);
	  if (part.is_electron()) {
	    ne++;
	    double ke1 = part.get_kinetic_energy();
	    if (ke1 > 200.0 * CLHEP::keV) ne_he++;
	    h_e1.fill(ke1 / CLHEP::MeV);
	  } else if (part.is_positron()) {
	    double kp1 = part.get_kinetic_energy();
	    h_p1.fill(kp1 / CLHEP::MeV);
	  } else if (part.is_gamma()) {
	    ng++;
	    double kg1 = part.get_kinetic_energy();
	    h_g1.fill(kg1 / CLHEP::MeV);
	  }
	}
	h_npart.fill(npart * 1.0);
	h_ne.fill(ne * 1.0);
	h_ng.fill(ng * 1.0);
	h_ne_he.fill(ne_he * 1.0);
      }
    } // loop
    std::clog << "max count = " << max_count << '\n';
    if (generator == &poiGen) {
      std::clog << "# original events = " << poiGen.get_original_event_counter() << '\n';
      std::clog << "# selected events = " << poiGen.get_event_counter() << '\n';
    }
    {
      std::string sname = "test_poi_generator-e1.his";
      std::ofstream ofhist(sname.c_str());
      h_e1.print(ofhist);
      ofhist.close();
    }
    {
      std::string sname = "test_poi_generator-p1.his";
      std::ofstream ofhist(sname.c_str());
      h_p1.print(ofhist);
      ofhist.close();
    }
    {
      std::string sname = "test_poi_generator-g1.his";
      std::ofstream ofhist(sname.c_str());
      h_g1.print(ofhist);
      ofhist.close();
    }
    {
      std::string sname = "test_poi_generator-ne.his";
      std::ofstream ofhist(sname.c_str());
      h_ne.print(ofhist);
      ofhist.close();
    }
    {
      std::string sname = "test_poi_generator-ng.his";
      std::ofstream ofhist(sname.c_str());
      h_ng.print(ofhist);
      ofhist.close();
    }
    {
      std::string sname = "test_poi_generator-ne_he.his";
      std::ofstream ofhist(sname.c_str());
      h_ne_he.print(ofhist);
      ofhist.close();
    }
    {
      std::string sname = "test_poi_generator-npart.his";
      std::ofstream ofhist(sname.c_str());
      h_npart.print(ofhist);
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

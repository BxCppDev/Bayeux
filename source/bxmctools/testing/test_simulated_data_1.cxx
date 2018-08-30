// -*- mode: c++ ; -*-
// test_simulated_data_1.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <vector>

#include <boost/scoped_ptr.hpp>

// Utilities :
#include <datatools/units.h>
#include <datatools/clhep_units.h>
#include <datatools/smart_ref.h>

// Simulated data model :
#include <mctools/simulated_data.h>
#include <mctools/simulated_data_reader.h>

// Serialization :
#include <datatools/io_factory.h>
#include <brio/writer.h>
#include <mctools/utils.h>
#include <mctools/simulated_data.ipp>
#include "dummy_mc_hit.h"
#include "dummy_mc_hit_2.h"

struct app_params {
  bool debug = false;
  bool hf    = false; // store header/footer
};
 
void test_1(const app_params &);
void test_2(const app_params &);

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'simulated_data'!" << std::endl;

    app_params params;

    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") || (option == "--debug")) {
          params.debug = true;
        } else if ((option == "-hf") ) {
          params.hf = true;
        } else {
          std::clog << "warning: ignoring option '" << option << "'!" << std::endl;
        }
      } else {
        std::string argument = token;
        {
          std::clog << "warning: ignoring argument '"
                    << argument << "'!" << std::endl;
        }
      }
      iarg++;
    }

    test_1(params);
    test_2(params);
    
    std::cerr << "Bye." << std::endl;
  } catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

void test_1(const app_params & params_)
{
  using namespace std;
  bool debug = params_.debug;
  
  clog << endl << endl;
  clog << "***********************************************************" << endl;
  clog << "Step 1: Generate a simulated data object and serialize it :" << endl;
  clog << "***********************************************************" << endl;
  {
    // datatools serialization:
    datatools::data_writer DW("test_simulated_data_1.xml.gz",
                              datatools::using_multi_archives);

    // brio serialization:
    brio::writer BW("test_simulated_data_1.brio");
    BW.add_store(mctools::io_utils::GENERAL_INFO_STORE,
                 datatools::properties::SERIAL_TAG);
    BW.add_store(mctools::io_utils::PLAIN_SIMULATED_DATA_STORE,
                 mctools::simulated_data::SERIAL_TAG);

    // A run header object:
    datatools::properties run_header;
    run_header.store_flag("test");
    run_header.store("library", "mctools");
    if (params_.hf) {
      DW.store(run_header);
      clog << "The run header has been stored in a Boost serialization XML file." << endl;
      BW.select_store(mctools::io_utils::GENERAL_INFO_STORE);
      BW.store(run_header);
      clog << "The run header has been stored in a Brio file." << endl;
    }

    BW.select_store(mctools::io_utils::PLAIN_SIMULATED_DATA_STORE);
    for (int i = 0; i < 3; i++) {
      // A simulated data object :
      mctools::simulated_data SD;

      // Primary vertex :
      SD.grab_vertex().set(2.0 * CLHEP::mm,
                           -4.0 * CLHEP::mm,
                           +7.0 * CLHEP::mm);
      // Primary event :
      {
        genbb::primary_event & the_primary_event = SD.grab_primary_event();
        the_primary_event.set_time(0.0);
        genbb::primary_particle p1, p2;
        p1.set_time(i * 1.0 * CLHEP::ns);
        p1.set_type(genbb::primary_particle::ELECTRON);
        p1.set_momentum(geomtools::vector_3d(3.0 * CLHEP::MeV, 0.0, 0.0));
        p2.set_time((1.3 + i) * CLHEP::ns);
        p2.set_type(genbb::primary_particle::GAMMA);
        p2.set_momentum(geomtools::vector_3d(0.0, 2.0 * CLHEP::MeV, 0.0));
        the_primary_event.add_particle(p1);
        the_primary_event.add_particle(p2);
      }
      // Some properties :
      SD.grab_properties().store_flag("fake");
      SD.grab_properties().store("module_number", 0);
      SD.grab_properties().store("run_number", 666);
      SD.grab_properties().store("event_number", i);

      // Add some list of MC hits :
      SD.add_step_hits("calo", 10);
      SD.add_step_hit("calo").set_hit_id(0);
      SD.add_step_hit("calo").set_hit_id(1);
      SD.add_step_hit("calo").set_hit_id(2);
      
      SD.add_step_hits("xcalo", 2);

      SD.add_step_hits("gveto", 3);
      SD.add_step_hit("gveto").set_hit_id(0);
      SD.add_step_hit("gveto").set_hit_id(1);
      
      const std::size_t ngg = 8;
      SD.add_step_hits("gg", ngg); 
      for (std::size_t igg = 0; igg < ngg; ++igg) {
        bool special = false;
        if (drand48() < 0.3) {
          special = true;
        }
        mctools::base_step_hit * new_hit = nullptr;
        if (special) {
          mctools::testing::dummy_mc_hit_2 & gghit2 = SD.add_hit<mctools::testing::dummy_mc_hit_2>("gg");
          new_hit = &gghit2;
          new_hit->grab_auxiliaries().store("comment", "I'm a mctools::testing::dummy_mc_hit_2 hit!");
          gghit2.set_foo(drand48());
          gghit2.set_bar(drand48());
        } else {
          mctools::base_step_hit & gghit = SD.add_step_hit("gg");
          new_hit = &gghit;
        }
        new_hit->set_hit_id(igg);
        geomtools::geom_id gid(1234, 0, 1, igg);
        new_hit->set_geom_id(gid);
        new_hit->grab_auxiliaries().store_flag("fake");
        new_hit->set_time_start(1.2 * CLHEP::microsecond);
        geomtools::vector_3d pos(0.0,
                                 drand48() * 22 * CLHEP::mm,
                                 150 * CLHEP::cm * (-1.0 + 2.0 * drand48()));
        new_hit->set_position_start(pos);
      }

      std::vector<std::string> hit_categories;
      SD.get_step_hits_categories(hit_categories);
      // List of hit categories known from this event:
      for (const std::string & hit_category : hit_categories) {
        std::clog << " - Category '" << hit_category << "'" << std::endl;
      }

      {
        // Print :
        boost::property_tree::ptree popts;
        popts.put(datatools::i_tree_dumpable::base_print_options::indent_key(), "");
        popts.put(datatools::i_tree_dumpable::base_print_options::title_key(), "Simulated data:");
        popts.put("list_hits", true);
        SD.print_tree(std::clog, popts);
      }
      DW.store(SD);
      clog << "The 'simulated_data' object has been stored in a Boost serialization XML file." << endl;
      BW.store(SD);
      clog << "The 'simulated_data' object has been stored in a Brio file." << endl;
    }

    // A run footer object:
    datatools::properties run_footer;
    run_footer.store("message", "The end");
    if (params_.hf) {
      DW.store(run_footer);
      clog << "The run footer has been stored in a Boost serialization XML file." << endl;
      BW.select_store(mctools::io_utils::GENERAL_INFO_STORE);
      BW.store(run_footer);
      clog << "The run footer has been stored in a Brio file." << endl;
    }
  }

  clog << endl << endl;
  clog << "*********************************************************" << endl;
  clog << "Step 2: Manual deserialization of simulated data objects:" << endl;
  clog << "*********************************************************" << endl;
  {
    // Serialization:
    datatools::data_reader DR("test_simulated_data_1.xml.gz",
                              datatools::using_multi_archives);

    // A simulated data object :
    mctools::simulated_data SD;
    boost::scoped_ptr<datatools::properties> run_header;
    boost::scoped_ptr<datatools::properties> run_footer;

    // Load a simulated data object from Boost serialization XML file:
    while (DR.has_record_tag()) {
      if (DR.record_tag_is(datatools::properties::SERIAL_TAG)) {
        if (!run_header) {
          run_header.reset(new datatools::properties);
          DR.load(*run_header.get());
          run_header.get()->tree_dump(std::clog, "Run header:");
        } else if (!run_footer) {
          run_footer.reset(new datatools::properties);
          DR.load(*run_footer.get());
          run_footer.get()->tree_dump(std::clog, "Run footer:");
        } else {
          string bad_tag = DR.get_record_tag();
          clog << "ERROR: Run header/footer have been already loaded !" << endl;
          break;
        }
      } else if (DR.record_tag_is(mctools::simulated_data::SERIAL_TAG)) {
        DR.load(SD);
        {
          // Print :
          boost::property_tree::ptree popts;
          popts.put(datatools::i_tree_dumpable::base_print_options::indent_key(), "");
          popts.put(datatools::i_tree_dumpable::base_print_options::title_key(), "Simulated data (loaded from the reader):");
          popts.put("list_hits", true);
          SD.print_tree(std::clog, popts);
        }
      } else {
        string bad_tag = DR.get_record_tag();
        clog << "ERROR: unknown data tag '"
             << bad_tag << "'!" << endl;
        break;
      }
    }
    clog << "The 'simulated_data' object has been loaded from a Boost serialization XML file." << endl;
  }

  clog << endl << endl;
  clog << "******************************************************************************" << endl;
  clog << "Step 3: Deserialization of simulated data objects through the dedicated reader" << endl;
  clog << "******************************************************************************" << endl;
  {
    mctools::simulated_data_reader reader;
    if (debug) reader.set_logging_priority(datatools::logger::PRIO_TRACE);
    // std::vector<std::string> files;
    // files.push_back("a.xml");
    // files.push_back("b.xml");
    // files.push_back("c.xml");
    // reader.initialize(files);
    reader.initialize("test_simulated_data_1.xml.gz");
    if (debug) reader.tree_dump(std::cerr, "Reader: ");
    if (reader.has_run_header()) {
      reader.get_run_header().tree_dump(std::clog, "Run header (loaded from the dedicated XML reader) :");
    }
    while (reader.has_next()) {
      mctools::simulated_data SD;
      reader.load_next(SD);
      {
        // Print :
        boost::property_tree::ptree popts;
        popts.put(datatools::i_tree_dumpable::base_print_options::indent_key(), "");
        popts.put(datatools::i_tree_dumpable::base_print_options::title_key(), "Simulated data (loaded from the XML reader):");
        popts.put("list_hits", true);
        SD.print_tree(std::clog, popts);
      }
      if (debug) reader.tree_dump(std::cerr, "Reader: ");
    }
    if (reader.has_run_footer()) {
      reader.get_run_footer().tree_dump(std::clog, "Run footer (loaded from the dedicated XML reader) :");
    }
    if (debug) reader.tree_dump(std::cerr, "Reader: ");
  }

  clog << endl << endl;
  clog << "******************************************************************************" << endl;
  clog << "Step 4: Deserialization of simulated data objects through the dedicated reader" << endl;
  clog << "******************************************************************************" << endl;
  {
    mctools::simulated_data_reader reader;
    if (debug) reader.set_logging_priority(datatools::logger::PRIO_TRACE);
    reader.initialize("test_simulated_data_1.brio");
    if (debug) reader.tree_dump(std::cerr, "Reader: ");
    if (reader.has_run_header()) {
      reader.get_run_header().tree_dump(std::clog,
                                        "Run header (loaded from the dedicated Brio reader) :");
    }
    int count = 0;
    while (reader.has_next()) {
      if (debug) std::cerr << "Entering new reader loop (Brio)..." << std::endl;
      mctools::simulated_data SD;
      int error = reader.load_next(SD);
      if (error != mctools::simulated_data_reader::LOAD_OK) {
        if (debug) std::cerr << "Load failed !" << std::endl;
        break;
      }
      count++;
      {
        // Print :
        boost::property_tree::ptree popts;
        popts.put(datatools::i_tree_dumpable::base_print_options::indent_key(), "");
        popts.put(datatools::i_tree_dumpable::base_print_options::title_key(), "Simulated data (loaded from the Brio reader):");
        popts.put("list_hits", true);
        SD.print_tree(std::clog, popts);
      }
      if (debug) reader.tree_dump(std::cerr, "Reader (Brio loop): ");
      if (debug) std::cerr << "Exiting reader loop (Brio)." << std::endl;
    }
    std::cerr << "Number of loaded simulated data records: " << count << std::endl;
    if (reader.has_run_footer()) {
      std::cerr << "Has footer." << std::endl;
      reader.get_run_footer().tree_dump(std::cerr,
                                        "Run footer (loaded from the Brio reader) :");
    }
  }
  return;
}

void test_2(const app_params & /* params_ */)
{
  using namespace std;
  // bool debug = params_.debug;
  
  clog << endl << endl;
  clog << "***********************************************************" << endl;
  clog << "Step 1: Generate a simulated data object and serialize it :" << endl;
  clog << "***********************************************************" << endl;

  {
    mctools::simulated_data SD;
    SD.set_time(0.0 * CLHEP::ns);
    geomtools::vector_3d vtx(0.0, 0.0, 10.0 * CLHEP::cm);
    SD.set_vertex(vtx);
    SD.add_step_hits("normal");
    SD.add_step_hits("spot");
    SD.add_step_hits("special");

    for (int i = 0; i < 3; i++) {
      mctools::base_step_hit & hit = SD.add_step_hit("normal");
      hit.set_hit_id(i);
      hit.set_energy_deposit(1.23 * CLHEP::keV);
    }

    for (int i = 0; i < 5; i++) {
      mctools::testing::dummy_mc_hit & mc_hit
        = SD.add_hit< mctools::testing::dummy_mc_hit>("spot");
      mc_hit.set_hit_id(i);
      mc_hit.set_energy_deposit(4.56 * CLHEP::keV);
      mc_hit.set_time_start(i * 0.1 * CLHEP::ns);
      if (drand48() < 0.5) mc_hit.set_foo(drand48());
    }

    for (int i = 0; i < 4; i++) {
      mctools::testing::dummy_mc_hit_2 & mc_hit
        = SD.add_hit< mctools::testing::dummy_mc_hit_2>("special");
      mc_hit.set_hit_id(i);
      mc_hit.set_energy_deposit(0.12 * CLHEP::keV);
      mc_hit.set_time_start(i * 2.35 * CLHEP::ns);
      if (drand48() < 0.5) mc_hit.set_foo(drand48());
      if (drand48() < 0.5) mc_hit.set_bar(drand48());
    }

    SD.tree_dump(std::clog, "Simulated data:"); 

    clog << endl << endl;
    clog << "*********************************************************" << endl;
    clog << "Step 2: Serialization:" << endl;
    clog << "*********************************************************" << endl;
    {
      // datatools serialization:
      datatools::data_writer DW("test_simulated_data_2.xml.gz",
                                datatools::using_multi_archives);
      DW.store(SD);
    }
  }

  {
    mctools::simulated_data SD;
    clog << endl << endl;
    clog << "*********************************************************" << endl;
    clog << "Step 3: Deserialization:" << endl;
    clog << "*********************************************************" << endl;
    {
      // datatools serialization:
      datatools::data_reader DR("test_simulated_data_2.xml.gz",
                                datatools::using_multi_archives);
      DR.load(SD);
    }
    {
      std::clog << "Loaded:" << std::endl;
      boost::property_tree::ptree popts;
      popts.put(datatools::i_tree_dumpable::base_print_options::indent_key(), "");
      popts.put("list_hits", true);
      SD.print_tree(std::clog, popts);
    }
  }

  return;
}

// end of test_simulated_data_1.cxx

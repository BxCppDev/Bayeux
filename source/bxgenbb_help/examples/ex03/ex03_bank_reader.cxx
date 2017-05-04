// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>

// Third party:
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/utils.h> // Utilities
#include <datatools/clhep_units.h> // Units
// - Bayeux/dpp:
#include <dpp/input_module.h> // Boost serialization file reader
// - Bayeux/genbb_help:
#include <genbb_help/manager.h>
#include <genbb_help/primary_event.h>
// #include <genbb_help/primary_event.ipp>

int main(int argc_, char ** argv_)
{
  datatools::logger::priority logging = datatools::logger::PRIO_NOTICE;
  try {
    std::string plain_input_file;
    if (argc_>1) {
      plain_input_file = argv_[1];
    }

    datatools::fetch_path_with_env(plain_input_file);
    DT_LOG_NOTICE(logging, "Loading data file '" << plain_input_file << "'...");

    // A data reader:
    dpp::input_module pe_bank_reader;
    pe_bank_reader.set_name("pe_bank_reader");
    // pe_bank_reader.set_logging_priority(datatools::logger::PRIO_TRACE);
    pe_bank_reader.set_single_input_file(plain_input_file);
    pe_bank_reader.set_clear_record(true);
    pe_bank_reader.initialize_simple();

    // Working event record data (with primary event bank):
    datatools::things ER;

    int pe_count = 0;
    double energy_mean = 0.0;
    while (! pe_bank_reader.is_terminated()) {
      if (pe_count % 10 == 0) {
        std::clog << "Processing records #" << pe_count << std::endl;
      }
      dpp::base_module::process_status status = pe_bank_reader.process(ER);
      if (status != dpp::base_module::PROCESS_SUCCESS) {
        std::clog << "Reader status=" << status << std::endl;
        break;
      }
      const genbb::primary_event & pe = ER.get<genbb::primary_event>("PE");
      energy_mean += pe.get_total_kinetic_energy();
      pe_count++;
      ER.clear();
    }
    pe_bank_reader.reset();
    std::clog << "Number of processed primary event records: " << pe_count << std::endl;
    std::clog << "Energy mean : " << energy_mean/pe_count/CLHEP::keV << " keV" << std::endl;

  }
  catch(std::exception & x) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, x.what());
  }
  catch(...) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, "Unexpected error !");
  }
  return 0;
}

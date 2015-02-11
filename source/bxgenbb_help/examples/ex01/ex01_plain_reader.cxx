// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>

// Third party:
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/io_factory.h> // Boost serialization file reader
#include <datatools/utils.h> // Utilities
#include <datatools/clhep_units.h> // Units
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
    datatools::data_reader pe_plain_reader;
    pe_plain_reader.init_multi(plain_input_file);
    pe_plain_reader.dump(std::clog);

    // Working primary event data:
    genbb::primary_event pe;

    int pe_count = 0;
    double energy_mean = 0.0;
    while (pe_plain_reader.has_record_tag()) {
      // DT_LOG_NOTICE(logging, "Found a record tag.");

      if (pe_plain_reader.record_tag_is(genbb::primary_event::SERIAL_TAG)) {
        // DT_LOG_NOTICE(logging, "Found a 'genbb::primary_event' record tag.");
        pe_plain_reader.load<genbb::primary_event>(pe);
        energy_mean += pe.get_total_kinetic_energy();
        pe_count++;
      } else {
        DT_THROW(std::runtime_error,
                 "Unexpected record tag '"
                 << pe_plain_reader.get_record_tag()
                 << "'! Reader is aborted!");
      }
    }
    std::clog << "Number of read primary events : " << pe_count << std::endl;
    std::clog << "Energy mean : " << energy_mean/pe_count/CLHEP::keV << " keV" << std::endl;

  }
  catch(std::exception & x) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL,x.what());
  }
  catch(...) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL,"Unexpected error !");
  }
  return 0;
}

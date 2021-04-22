// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>

// Third party:
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/io_factory.h> // Boost serialization file writer
#include <datatools/utils.h> // Utilities
// - Bayeux/genbb_help:
#include <genbb_help/manager.h>
#include <genbb_help/primary_event.h>
#include <genbb_help/primary_event.ipp> // Mandatory for BOOST_CLASS_VERSION

int main(int argc_, char ** argv_)
{
  datatools::logger::priority logging = datatools::logger::PRIO_NOTICE;
  try {
    int nevents = 10;

    if (argc_>1) {
      std::string opt = argv_[1];
      if (opt == "--debug") logging = datatools::logger::PRIO_DEBUG;
      else if (opt == "--warning") logging = datatools::logger::PRIO_WARNING;
    }

    // External PRNG :
    mygsl::rng PRNG;
    PRNG.initialize("taus2", 314159);

    // Declare a geometry manager :
    genbb::manager EGM;
    EGM.set_logging_priority(logging);

    // Parse its config file :
    datatools::properties EGM_config;
    std::string EGM_config_file = "config/manager.conf";
    datatools::read_config(EGM_config_file, EGM_config);

    // Initialize the geometry manager from the configuration parameters :
    EGM.set_external_prng(PRNG); // Attach the external PRNG to the manager
    EGM.initialize(EGM_config);

    if (logging >= datatools::logger::PRIO_NOTICE) {
      // Smart print :
      EGM.tree_dump(std::clog, "The event generator manager: ");
    }

    std::string eg_name = "Co60";
    DT_THROW_IF(! EGM.has(eg_name),
                std::logic_error,
                "No event generator named '" << eg_name << "' !");

    genbb::i_genbb & PG = EGM.grab(eg_name);

    // A data writer:
    std::string plain_input_file = "ex01.xml";
    datatools::data_writer pe_plain_writer;
    pe_plain_writer.init_multi(plain_input_file);

    int count = 0;
    while (PG.has_next()) {
      genbb::primary_event decay_event;
      PG.load_next(decay_event, false);
      decay_event.set_label (eg_name);
      decay_event.tree_dump (std::clog,
                             "Generated primary event: ",
                             "NOTICE: ");
      pe_plain_writer.store(decay_event);
      count++;
      if (count >= nevents) break;
    }

  }
  catch(std::exception & x) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, x.what());
  }
  catch(...) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, "Unexpected error !");
  }
  return 0;
}

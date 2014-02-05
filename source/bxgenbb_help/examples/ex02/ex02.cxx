// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>

// Third party:
// - Bayeux/datatools:
#include <datatools/logger.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>
// - Bayeux/genbb_help:
#include <genbb_help/primary_event.h>

// This project:
#include <crazy_gun.hh>

int main(int argc_, char ** argv_)
{
  datatools::logger::priority logging = datatools::logger::PRIO_NOTICE;
  try {
    // Number of generated events:
    int nevents = 10;

    // Command line arguments parsing:
    if (argc_>1) {
      std::string opt = argv_[1];
      if (opt == "--debug") logging = datatools::logger::PRIO_DEBUG;
      else if (opt == "--warning") logging = datatools::logger::PRIO_WARNING;
    }

    // External PRNG :
    mygsl::rng PRNG;
    PRNG.initialize("taus2", 314159);

    // The event generator:
    genbb::ex02::crazy_gun CG;
    CG.set_external_random(PRNG);
    CG.set_min_number_of_particles(3);
    CG.set_max_number_of_particles(7);
    CG.initialize_simple();
    CG.tree_dump(std::clog, "Crazy generator: ", "NOTICE: ");

    // Event loop:
    int count = 0;
    while (CG.has_next()) {
      genbb::primary_event decay_event;
      CG.load_next(decay_event, false);
      decay_event.set_label("genbb::ex02::crazy_gun");
      decay_event.tree_dump(std::clog,
                            "Generated primary event: ",
                            "NOTICE: ");
      count++;
      if (count >= nevents) break;
    }

  }
  catch(std::exception & x) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL,x.what());
  }
  catch(...) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL,"Unexpected error !");
  }
  return 0;
}

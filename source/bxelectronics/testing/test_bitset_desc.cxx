#include <iostream>

// Third party
// - Bayeux/datatools
#include <datatools/logger.h>
#include <datatools/exception.h>

// This project
#include <electronics/bitset_desc.h>

int main( int argc_, char * argv_[])
{
  int error_code = EXIT_SUCCESS;

  try {

    electronics::bitset_desc bsd;
    bsd.set_name("CALOBLKMULT");
    bsd.set_logging_priority(datatools::logger::PRIO_DEBUG);
    bsd.set_display_name("Calo. Block Mult.");
    bsd.set_terse_description("Calorimeter block multiplicity threshold");
    bsd.grab_auxiliaries().store_flag("test");
    bsd.grab_auxiliaries().store("version", "1.0");
    bsd.set_size(5);
    bsd.set_lsb_position(2);
    bsd.tree_dump(std::clog, "Bitset description: ");
    bsd.reset();
    bsd.tree_dump(std::clog, "Bitset description: ");

    datatools::properties bsd_cfg;
    bsd_cfg.store("name", "TRKBLKMASK");
    bsd_cfg.store("display_name", "Tracker Block Mask");
    bsd_cfg.store("description", "Mask for 18 cells in a tracker cell block");
    bsd_cfg.store("size", 18);
    bsd_cfg.store("lsb_position", 3);
    bsd.initialize(bsd_cfg);
    bsd.tree_dump(std::clog, "Another bitset description: ");

    boost::dynamic_bitset<> work;
    bsd.make(work);
    std::clog << "Bitset: " << work << " (default)" << std::endl;
    work[3] = 1;
    work[7] = 1;
    std::clog << "Bitset: " << work << " (modified)" << std::endl;
    boost::dynamic_bitset<> mask;
    bsd.make_mask(32, mask);
    std::clog << "Mask (32 bits): " << mask << std::endl;

  }
  catch (std::exception& error) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL,
                 error.what());
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL,
                 "Unexpected error!");
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

// test_base_event.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <vector>

// Boost:
#include <boost/lexical_cast.hpp>

// Bayeux:
#include <datatools/handle.h>

// This project:
#include <events/base_event.h>
#include <events/base_event.ipp>

int main (int /* argc_ */, char ** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {

    long int seed = 314159;
    srand48(seed);

    typedef datatools::handle<events::base_event> evhandle_type;
    typedef std::vector<evhandle_type> evcol_type;

    evcol_type evseq;

    std::size_t nevents = 8;
    double time = 0.0;
    for (std::size_t iev = 0; iev < nevents; iev++) {
      evhandle_type evh(new events::base_event);
      evseq.push_back(evh);
      events::base_event & BE = evseq.back().grab();
      BE.set_hit_id(iev);
      BE.set_geom_id(geomtools::geom_id(120, 0, 0));
      BE.grab_auxiliaries().store_flag("fake");
      BE.set_category(1);
      BE.set_position(geomtools::vector_3d(0.0, 0.0, 0.0));
      time += drand48() * 1.5 * CLHEP::ns;
      BE.set_time(time);
      BE.tree_dump(std::clog, "Event #" + boost::lexical_cast<std::string>(iev) + ":");
    }

  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

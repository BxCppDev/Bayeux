// test_random_event_sequence_model.cxx

// Ourselves:
#include <mctools/random_event_sequence_model.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Boost:
#include <boost/lexical_cast.hpp>
// - Bayeux/datatools:
#include <datatools/io_factory.h>
#include <datatools/units.h>
// - Bayeux/geomtools:
#include <geomtools/utils.h>

// This project:
// #include <mctools/xxx.h>

void test0();

int main (int /* argc_ */, char ** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'random_event_sequence_model'!" << std::endl;

    test0();

  } catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

void test0()
{

  int seed = 314159;
  mygsl::rng rand("taus", seed);
  mctools::random_event_sequence_model resm;
  /*int cat0 =*/ resm.add_source("Tl208", "source_foil", datatools::units::get_value_with_unit("2.0 Bq"));
  /*int cat1 =*/ resm.add_source("Bi214", "source_foil", datatools::units::get_value_with_unit("1.0 Bq"));
  /*int cat2 =*/ resm.add_source("K40",   "PMTs_glass",  datatools::units::get_value_with_unit("0.1 Bq"));
  resm.set_time_window(0.0, 1000.0 * CLHEP::second);
  resm.set_random(rand);
  // resm.set_relative_time(true);
  resm.initialize();
  resm.tree_dump(std::clog, "Model: ");

  std::list<events::base_event> events;
  resm.set_target(events);
  std::size_t sz = resm.generate_sequence();
  std::clog << "[info] Number of generated events: " << sz << std::endl;
  resm.reset();

  std::size_t n = 0;
  double tref = 0.0;
  for (auto ev : events) {
    // ev.tree_dump(std::clog, "Event #" + boost::lexical_cast<std::string>(n) + " :", "[debug] ");
    n++;
    double t = ev.get_time() - tref;
    std::cout << t / CLHEP::second << " " << ev.get_category() << std::endl;
    tref = ev.get_time();
  }
  return;
}

// test_angular_range.cxx

// Ourselves:
#include <geomtools/angular_range.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <exception>

// Third party:
// - Boost:
#include <boost/serialization/nvp.hpp>
// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/compiler_macros.h>
#include <datatools/io_factory.h>
#include <datatools/properties.h>

void test1();
void test2();
void test3();

int main (int DT_UNUSED(argc_), char ** DT_UNUSED(argv_) )
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'geomtools::angular_range'!" << std::endl;

    test1();
    test2();
    test3();

  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

void test1()
{
  std::clog << "Running test1... " << std::endl;
  // [0:360]° :
  geomtools::angular_range ar0("twopi");
  ar0.tree_dump(std::clog, "Angular range #0: ", "");
  for (double alpha = geomtools::angular_range::min_angle(ar0.get_type());
       alpha <= geomtools::angular_range::max_angle(ar0.get_type());
       alpha += 5.0 * CLHEP::degree) {
    if (!ar0.contains(alpha)) {
      std::clog << "Angle [" << alpha / CLHEP::degree << " degree] is not in range #0." << std::endl;
    }
  }

  {
    std::clog << "Angle sampling: " << std::endl;
    geomtools::angular_range::iterator iter(ar0, 6);
    iter.tree_dump(std::cerr, "Initial iterator #0: ",  "DEVEL: ");
    std::clog << std::endl;
    for (; !iter.is_finished(); iter.step()) {
      std::clog << "  Sampled angle [" << *iter / CLHEP::degree << " degree]" << std::endl;
    }
    std::clog << std::endl;
    iter.tree_dump(std::cerr, "Final iterator #0: ",  "DEVEL: ");
    std::clog << std::endl;
  }

  std::clog << "Angle sampling: " << std::endl;
  for (geomtools::angular_range::iterator iter(ar0); !iter.is_finished(); iter.step()) {
    std::clog << "  Sampled angle [" << *iter / CLHEP::degree << " degree]" << std::endl;
  }
  std::clog << std::endl;
  std::clog << std::endl;

  // [32.5:360]° :
  geomtools::angular_range ar1("twopi");
  ar1.set_start_angle(32.5 * CLHEP::degree);
  ar1.tree_dump(std::clog, "Angular range #1: ", "");
  for (double alpha = geomtools::angular_range::min_angle(ar1.get_type());
       alpha <= geomtools::angular_range::max_angle(ar1.get_type());
       alpha += 5.0 * CLHEP::degree) {
    if (!ar1.contains(alpha)) {
      std::clog << "Angle [" << alpha / CLHEP::degree << " degree] is not in range #1." << std::endl;
    }
  }
  std::clog << std::endl;

  std::clog << "Angle sampling: " << std::endl;
  for (geomtools::angular_range::iterator iter(ar1, 20); !iter.is_finished(); iter.step()) {
    std::clog << "  Sampled angle [" << *iter / CLHEP::degree << " degree]" << std::endl;
  }
  std::clog << std::endl;
  std::clog << std::endl;

  // [0:64.5]° :
  geomtools::angular_range ar2(geomtools::angular_range::RANGE_TYPE_PI);
  ar2.set_delta_angle(64.5 * CLHEP::degree);
  ar2.tree_dump(std::clog, "Angular range #2: ", "");
  for (double alpha = geomtools::angular_range::min_angle(ar2.get_type());
       alpha <= geomtools::angular_range::max_angle(ar2.get_type());
       alpha += 5.0 * CLHEP::degree) {
    if (!ar2.contains(alpha)) {
      std::clog << "Angle [" << alpha / CLHEP::degree << " degree] is not in range #2." << std::endl;
    }
  }
  std::clog << std::endl;

  // [12.5:290]° :
  geomtools::angular_range ar3(geomtools::angular_range::RANGE_TYPE_TWOPI);
  ar3.set_start_angle(12.5 * CLHEP::degree);
  ar3.set_delta_angle(277.5 * CLHEP::degree);
  ar3.tree_dump(std::clog, "Angular range #3: ", "");
  for (double alpha = geomtools::angular_range::min_angle(ar3.get_type());
       alpha <= geomtools::angular_range::max_angle(ar3.get_type());
       alpha += 5.0 * CLHEP::degree) {
    if (!ar3.contains(alpha)) {
      std::clog << "Angle [" << alpha / CLHEP::degree << " degree] is not in range #2." << std::endl;
    }
  }
  std::clog << std::endl;

  std::clog << "Angle sampling: " << std::endl;
  for (geomtools::angular_range::iterator iter(ar3, 12); !iter.is_finished(); iter.step()) {
    std::clog << "  Sampled angle [" << *iter / CLHEP::degree << " degree]" << std::endl;
  }
  std::clog << std::endl;
  std::clog << std::endl;

  std::clog << "Angle sampling: " << std::endl;
  for (geomtools::angular_range::iterator iter(ar3, 4); !iter; ++iter) {
    std::clog << "  Sampled angle [" << *iter / CLHEP::degree << " degree]" << std::endl;
  }
  std::clog << std::endl;
  std::clog << std::endl;
  return;
}

void test2()
{
  std::clog << "Running test2... " << std::endl;

  {
    {
      // [12.5:290]° :
      geomtools::angular_range ar0;
      ar0.set_type(geomtools::angular_range::RANGE_TYPE_TWOPI);
      ar0.set_start_angle(12.5 * CLHEP::degree);
      ar0.set_delta_angle(277.5 * CLHEP::degree);
      ar0.tree_dump(std::clog, "Angular range #0: ", "");

      datatools::data_writer the_writer("test_angular_range_0.xml", datatools::using_multi_archives);
      the_writer.store(ar0);
    }

    {
      geomtools::angular_range ar0;
      datatools::data_reader the_reader("test_angular_range_0.xml", datatools::using_multi_archives);
      the_reader.load(ar0);
      ar0.tree_dump(std::clog, "Reloaded angular range #0: ", "");
    }
    std::clog << std::endl;

  }
  {
    {
      // [0:360]° :
      geomtools::angular_range ar1;
      ar1.set_type(geomtools::angular_range::RANGE_TYPE_TWOPI);
      ar1.tree_dump(std::clog, "Angular range #1: ", "");

      datatools::data_writer the_writer("test_angular_range_1.xml", datatools::using_multi_archives);
      the_writer.store(ar1);
    }

    {
      geomtools::angular_range ar1;
      datatools::data_reader the_reader("test_angular_range_1.xml", datatools::using_multi_archives);
      the_reader.load(ar1);
      ar1.tree_dump(std::clog, "Reloaded angular range #1: ", "");
    }
    std::clog << std::endl;

  }
  std::clog << std::endl;
  return;
}

void test3()
{
  std::clog << "Running test3... " << std::endl;

  {
    geomtools::angular_range ar0("pi");
    datatools::properties ar_cfg;
    ar_cfg.store("type", "pi");
    ar_cfg.store_real_with_explicit_unit("start", 10.0 * CLHEP::degree);
    ar_cfg.store_real_with_explicit_unit("delta", 171.0 * CLHEP::degree);
    try {
      ar0.initialize(ar_cfg);
    } catch (std::exception & error) {
      std::cerr << "Caught: As expected : " << error.what() << std::endl;
    }
    std::clog << std::endl;
  }

  {
    geomtools::angular_range ar1("pi");
    datatools::properties ar_cfg;
    ar_cfg.store("type", "pi");
    ar_cfg.store_real_with_explicit_unit("start", 10.0 * CLHEP::degree);
    ar_cfg.store_real_with_explicit_unit("delta", 134.0 * CLHEP::degree);
    ar1.initialize(ar_cfg);
    ar1.tree_dump(std::clog, "Angular range #0: ", "");
    std::clog << std::endl;
  }

  {
    geomtools::angular_range ar1;
    datatools::properties ar_cfg;
    ar_cfg.store("type", "twopi");
    ar_cfg.store_real_with_explicit_unit("delta", 270.0 * CLHEP::degree);
    ar1.initialize(ar_cfg);
    ar1.tree_dump(std::clog, "Angular range #0: ", "");
    std::clog << std::endl;
  }

  std::clog << std::endl;
  return;
}

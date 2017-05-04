// Standard library
#include <iostream>
#include <exception>
#include <cstdlib>

// Third party
// - datatools
#include <datatools/clhep_units.h>
#include <datatools/io_factory.h>
#include <datatools/things.h>

// This project
#include <dpp/output_module.h>
#include <dpp/input_module.h>
#include <dpp_ex01/hit.h>
#include <dpp_ex01/raw_data.h>

void test0();
void test1();

int main() {
  try {
    test0();
    test1();
  } catch (std::exception & x) {
    std::cerr << "ERROR: " << x.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

void test0()
{
  dpp_ex01::hit h;
  h.set_id(3);
  h.set_energy(700.0 * CLHEP::keV);
  h.grab_auxiliaries().store_flag("test");
  h.grab_auxiliaries().store("time", 1.23 * CLHEP::ns);
  h.grab_auxiliaries().store("sigma_time", 176 * CLHEP::picosecond);
  h.dump(std::clog, "Hit:");

  dpp_ex01::raw_data RD;
  for (int i = 0; i < 3; i++) {
    dpp_ex01::hit dummy_hit;
    RD.grab_hits().push_back(dummy_hit);
    dpp_ex01::hit & h = RD.grab_hits().back();
    h.set_id(i);
    h.set_energy(i * 50.0 * CLHEP::keV);
    h.grab_auxiliaries().store_flag("fake");
  }
  RD.grab_auxiliaries().store_flag("test");
  RD.dump(std::clog, "Raw data:");

  {
    datatools::data_writer DW("test_dpp_ex01.xml",
                              datatools::using_multi_archives);
    DW.store(RD);
  }

  {
    dpp_ex01::raw_data RD2;
    datatools::data_reader DR("test_dpp_ex01.xml",
                              datatools::using_multi_archives);
    if (DR.has_record_tag()
        && DR.record_tag_is(dpp_ex01::raw_data::SERIAL_TAG)) {
      DR.load(RD2);
      RD2.dump(std::clog, "Deserialized raw data:");
    }
  }
  return;
}

void test1()
{
  datatools::things t;
  dpp_ex01::raw_data & RD = t.add<dpp_ex01::raw_data>("RD");
  for (int i = 0; i < 3; i++) {
    dpp_ex01::hit dummy_hit;
    RD.grab_hits().push_back(dummy_hit);
    dpp_ex01::hit & h = RD.grab_hits().back();
    h.set_id(i);
    h.set_energy(i * 50.0 * CLHEP::keV);
    h.grab_auxiliaries().store_flag("fake");
  }
  RD.grab_auxiliaries().store_flag("test");
  t.tree_dump(std::clog, "Things:");

  {
    datatools::data_writer DW("test_dpp_t_ex01.xml",
                              datatools::using_multi_archives);
    DW.store(t);
  }

  {
    datatools::things t2;
    datatools::data_reader DR("test_dpp_t_ex01.xml",
                              datatools::using_multi_archives);
    if (DR.has_record_tag()
        && DR.record_tag_is(datatools::things::SERIAL_TAG)) {
      DR.load(t2);
      t2.tree_dump(std::clog, "Deserialized things:");
    }
  }

  {
    dpp::output_module outmod;
    outmod.set_single_output_file("test_dpp_t2_ex01.xml");
    outmod.initialize_simple();
    outmod.process(t);
  }

  {
    datatools::things t2;
    dpp::input_module inmod;
    inmod.set_single_input_file("test_dpp_t2_ex01.xml");
    inmod.initialize_simple();
    inmod.process(t2);
    t2.tree_dump(std::clog, "Deserialized things:");
  }

  return;
}

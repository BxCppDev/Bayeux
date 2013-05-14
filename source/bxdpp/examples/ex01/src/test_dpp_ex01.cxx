#include <iostream>
#include <exception>

#include <datatools/clhep_units.h>
#include <datatools/io_factory.h>

#include <dpp_ex01/hit.h>
#include <dpp_ex01/raw_data.h>

// Explicit include the ``dpp_ex01`` serialization code:
//   #include <dpp_ex01/hit.ipp>
//   #include <dpp_ex01/raw_data.ipp>
// or explicitely link with the ``dpp_ex01_bio`` DLL.

int main() {
  try {

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
      h.set_energy(i *50.0 * CLHEP::keV);
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
      if (DR.has_record_tag ()
          && DR.record_tag_is(dpp_ex01::raw_data::SERIAL_TAG)) {
        DR.load(RD2);
        RD2.dump(std::clog, "Deserialized raw data:");
      }
    }
  }
  catch (std::exception & x) {
    std::cerr << "ERROR: " << x.what() << std::endl;
  }
}

// test_digitization_sampled_signal.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/temporary_files.h>
#include <datatools/utils.h>
#include <datatools/units.h>
#include <datatools/clhep_units.h>
#include <datatools/io_factory.h>
// - Bayeux/mygsl:
#include <mygsl/parameter_store.h>
// - Bayeux/geomtools:
#include <geomtools/geomtools_config.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

// This project:
#include <mctools/digitization/sampled_signal.h>

void test_ss_1(bool draw_ = false);

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'mctools::digitization::simple_linear_adc'!" << std::endl;

    bool draw = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];
      if (arg == "-D" || arg == "--draw") draw = true;
      iarg++;
    }

    srand48(314159);
    test_ss_1(draw);

    std::clog << "The end." << std::endl;
  } catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

void test_ss_1(bool draw_)
{

  {
    mctools::digitization::sampled_signal digi_sig;
    digi_sig.set_hit_id(42);
    geomtools::geom_id hitGid(1000, 1, 2);
    digi_sig.set_geom_id(hitGid);
    digi_sig.grab_auxiliaries().store("test", "foo");
    digi_sig.set_sampling_frequency(1.0 * datatools::units::get_frequency_unit_from("GHz"));
    digi_sig.set_number_of_samples(32);
    for (std::size_t isample = 0; isample < digi_sig.get_number_of_samples(); isample++) {
      if (isample == 4) {
        digi_sig.set_underflow_sample(isample);
      } else if (isample == 5) {
        digi_sig.set_overflow_sample(isample);
      } else if (isample == 6) {
        digi_sig.unset_sample(isample);
      } else {
        int32_t sample = 42 + (int32_t) (12 * drand48());
        digi_sig.set_sample(isample, sample);
      }
    }
    digi_sig.update();
    digi_sig.tree_dump(std::clog, "Digitized hit signal (1): ");
    std::clog << std::endl;

    digi_sig.set_sample(4, 37, true);
    digi_sig.tree_dump(std::clog, "Digitized hit signal (2): ");
    std::clog << std::endl;

    digi_sig.set_sample(5, 32, true);
    digi_sig.update();
    digi_sig.tree_dump(std::clog, "Digitized hit signal (3): ");
    std::clog << std::endl;

    digi_sig.set_sample(6, 67, true);
    digi_sig.tree_dump(std::clog, "Digitized hit signal (4): ");
    std::clog << std::endl;

    digi_sig.set_sample(6, 45, true);
    digi_sig.tree_dump(std::clog, "Digitized hit signal (5): ");
    std::clog << std::endl;

    digi_sig.unset_sample(10, true);
    digi_sig.tree_dump(std::clog, "Digitized hit signal (6): ");
    std::clog << std::endl;

    digi_sig.set_sample(10, 12, true);
    digi_sig.tree_dump(std::clog, "Digitized hit signal (7): ");
    std::clog << std::endl;

    mctools::digitization::sampled_signal digi_slice;
    uint32_t slicing_flags = mctools::digitization::sampled_signal::SLICING_ALL;
    digi_sig.slice_from_to(digi_slice, 14, 27, slicing_flags);
    digi_slice.tree_dump(std::clog, "Sliced digitized hit signal: ");
    std::clog << std::endl;

    {
      std::clog << "Serialization..." << std::endl;
      // Serialization:
      datatools::data_writer writer("test_digitization_sampled_signal.xml",
                                    datatools::using_multi_archives);
      writer.store(digi_sig);
    }

  }

  {
    mctools::digitization::sampled_signal digi_sig;
    {
      std::clog << "Deserialization..." << std::endl;
      // Deserialization:
      datatools::data_reader reader("test_digitization_sampled_signal.xml",
                                    datatools::using_multi_archives);
      reader.load(digi_sig);
    }
    digi_sig.tree_dump(std::clog, "Loaded sliced digitized hit signal: ");
    std::clog << std::endl;
  }

  return;
}

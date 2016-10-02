// test_digitization_simple_linear_adc.cxx

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
#include <mctools/digitization/simple_linear_adc.h>

void test_adc_1(bool draw_ = false);

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

    test_adc_1(draw);

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

void test_adc_1(bool draw_)
{

  std::clog << "INVALID_CHANNEL = "
            << std::hex << mctools::digitization::simple_linear_adc::INVALID_CHANNEL
            << std::endl;
  std::clog << "INVALID_CHANNEL = "
            << std::dec << mctools::digitization::simple_linear_adc::INVALID_CHANNEL
            << std::endl;

  mctools::digitization::simple_linear_adc adc;
  adc.set_nbits(3);
  adc.set_v_ref_low(-6.0 * CLHEP::volt);
  adc.set_v_ref_high(+1.0 * CLHEP::volt);
  adc.set_no_underflow(true);
  // adc.set_underflow_channel(-1);
  adc.set_overflow_channel(15);
  adc.initialize_simple();
  adc.tree_dump(std::clog, "ADC: ");

  /*
    $ ./BuildProducts/bin/bxtests/mctools-test_digitization_simple_linear_adc > a.data
    $ gnuplot
    gnuplot> set grid
    gnuplot> set key out
    gnuplot> set xrange [-10:+3]
    gnuplot> set xlabel "Voltage (volt)"
    gnuplot> set yrange [-2:16]
    gnuplot> set ylabel "Channel"
    gnuplot> plot 'a.data' i 1 u 1:2 w points pt 6 ps 0.5, \
                  ''  i 1 u 1:2 w histeps, \
                  ''  i 0 u 1:2 w points pt 4 ps 0.5

    gnuplot> set xrange [0:10]
    gnuplot> set xlabel "Time (ns)"
    gnuplot> set yrange [-10.5:2.5]
    gnuplot> set ylabel "Voltage (volt)"
    gnuplot> plot 'a.data' i 2 u 1:2 w l, '' i 2 u 1:4 w points pt 4 ps 0.5

   */
  std::cout.precision(15);
  double delta_v = 0.01 * CLHEP::volt;
  for (double voltage = -8.0 * CLHEP::volt;
       voltage < +3 * CLHEP::volt;
       voltage += delta_v) {
    int32_t channel = adc.quantize(voltage);
    std::cout << voltage / CLHEP::volt << ' ' << channel << std::endl;
  }
  std::cout << std::endl << std::endl;

  for (int ich = adc.get_min_channel(); ich <= adc.get_max_channel(); ich++) {
    std::cout << adc.compute_sampled_voltage(ich) / CLHEP::volt
              << ' ' << ich
              << std::endl;
  }
  std::cout << std::endl << std::endl;

  double v0 = 6.0 * CLHEP::volt;
  double dt =  50 * CLHEP::picosecond;
  double f = 0.15 / CLHEP::nanosecond;
  double omega = 2 * M_PI * f;
  for (double time = 0.0; time < 10. * CLHEP::ns; time += dt) {
    double voltage = -v0 * ( 1.0 + std::cos(omega * time)) + 2 * CLHEP::volt;
    // std::clog << "Voltage = " << voltage / CLHEP::volt << " V" << std::endl;
    int channel = adc.quantize(voltage);
    double sampled_voltage = adc.compute_sampled_voltage(channel);
    std::cout << time
              << ' ' << voltage / CLHEP::volt
              << ' ' << channel
              << ' ' << sampled_voltage / CLHEP::volt
              << std::endl;
  }
  std::cout << std::endl << std::endl;

  return;
}

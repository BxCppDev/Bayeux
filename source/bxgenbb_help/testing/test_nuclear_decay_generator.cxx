// test_nuclear_decay_generator.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/units.h>
// - Bayeux/geomtools:
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
// - Bayeux/mygsl:
#include <mygsl/histogram_1d.h>

// This project:
#include <genbb_help/nuclear_decay_generator.h>
#include <genbb_help/primary_event.h>

void test1(bool draw_, int many_);

int main(int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test program for class 'nuclear_decay_generator'!" << endl;

    int  many = 0;
    bool draw = false;

    int iarg = 1;
    while (iarg < argc_) {
      string token = argv_[iarg];

      if (token[0] == '-') {
        string option = token;
        if ((option == "-m") || (option == "--more")) {
          many += 1;
        } else if ((option == "--draw")) {
          draw = true;
        } else {
          clog << "warning: ignoring option '" << option << "'!" << endl;
        }
      } else {
        string argument = token;
        {
          clog << "warning: ignoring argument '" << argument << "'!" << endl;
        }
      }
      iarg++;
    }

    test1(draw, many);

  }
  catch (exception & x) {
    cerr << "error: " << x.what() << endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    cerr << "error: " << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}


void test1(bool draw_, int many_)
{
  std::cerr << "DEVEL: test1: Entering..." << std::endl;
  genbb::nuclear_decay_generator NDG;

  // Build a configuration properties container:
  datatools::properties config;
  config.store("logging", "trace");

  // The seed of the embedded random number generator:
  config.store ("seed", 314159);
  config.store ("manager.configuration",
                "${GENBB_HELP_TESTING_DIR}/config/test_nuclear_decay_manager_0.conf");
  config.store ("decaying.level", "He6_0");
  config.tree_dump (std::clog, "Configuration: ", "INFO: ");

  // Configure the event generator:
  NDG.initialize_standalone(config);

  // Shoot events:
  std::size_t max_count = 10;
  for (int i = 0; i < many_; i++) {
    max_count *= 10;
  }

  mygsl::histogram_1d h_ke(400, 0.0, 4000.0);
  mygsl::histogram_1d h_knu(400, 0.0, 4000.0);
  mygsl::histogram_1d h_krec(200, 0.0, 2.0);

  genbb::primary_event pe; // working primary event
  for (int i = 0; i <(int) max_count; i++) {
    std::clog << "Count : " << i << std::endl;
    NDG.load_next(pe);
    // pe.tree_dump(std::clog, "Nuclear decay:");
    for (int i = 0; i < (int) pe.get_number_of_particles(); i++) {
      const genbb::primary_particle & part = pe.get_particle(i);
      if (part.is_electron()) {
        double ke = part.get_kinetic_energy();
        h_ke.fill(ke / CLHEP::keV);
      } else if (part.is_neutrino()) {
        double knu = part.get_kinetic_energy();
        h_knu.fill(knu / CLHEP::keV);
      } else if (part.is_ion()) {
        double krec = part.get_kinetic_energy();
        h_krec.fill(krec / CLHEP::keV);
      }

    }
  }
  std::string histo_filename = "test_nuclear_decay_generator_hist.data";
  std::ofstream ofhist(histo_filename.c_str());
  h_ke.print(ofhist);
  ofhist << std::endl << std::endl;
  h_knu.print(ofhist);
  ofhist << std::endl << std::endl;
  h_krec.print(ofhist);
  ofhist << std::endl << std::endl;
  ofhist.close();

  if (draw_) {
    Gnuplot g1("line");
    g1.cmd("set title 'Test genbb::nuclear_decay_generator' ");
    g1.cmd("set grid");
    g1.cmd("set key out");
    {
      std::ostringstream plot_cmd;
      g1.cmd("set key out");
      g1.cmd("set xlabel 'Energy (keV)'");
      g1.cmd("set ylabel 'Counts'");
      plot_cmd << "plot '" << histo_filename << "' index 0 u (0.5*(column(1)+column(2))):3 title '' with histeps ";
      plot_cmd << ",    '" << histo_filename << "' index 1 u (0.5*(column(1)+column(2))):3 title '' with histeps ";
      g1.cmd(plot_cmd.str ());
      g1.showonscreen(); // window output
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }
    {
      std::ostringstream plot_cmd;
      g1.cmd("set key out");
      g1.cmd("set xlabel 'Energy (keV)'");
      g1.cmd("set ylabel 'Counts'");
      plot_cmd << "plot '" << histo_filename << "' index 2 u (0.5*(column(1)+column(2))):3 title '' with histeps ";
      g1.cmd(plot_cmd.str ());
      g1.showonscreen(); // window output
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }
  }

  unlink(histo_filename.c_str());
  std::cerr << "DEVEL: test1: Exiting." << std::endl;
  return;
}

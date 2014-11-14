// test_gamma_transition.cxx

// Ourselves
#include <genbb_help/gamma_transition.h>

// Standard library:
#include <iostream>
#include <cstdlib>
#include <exception>

// Third party:
// - CLHEP:
#include<CLHEP/Units/PhysicalConstants.h>
#include<CLHEP/Units/SystemOfUnits.h>
// - Bayeux/geomtools:
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>
#include <mygsl/histogram_1d.h>

void test_1(bool draw_);

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    bool draw = false;
    bool tensor = false;
    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "--draw") {
        draw = true;
      }
      iarg++;
    }

    test_1(draw);


  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

void test_1(bool draw_)
{
  mygsl::rng random("taus", 314159);

  // Describe the gamma transition of Li-6 (2186 keV) to Li-6 (0):
  genbb::gamma_transition GT;
  GT.set_energy(2186.0 * CLHEP::keV);
  GT.initialize_simple();
  GT.tree_dump(std::clog, "Li-6(2186 keV)->Li-6(0): ");
  std::clog << std::endl;

  std::string histo_filename = "test_gamma_transition_hist_e_phi_cost.data";
  std::clog << "Shoot events..." << std::endl;
  mygsl::histogram_1d h_e(300, 0.0, 3.0);
  mygsl::histogram_1d h_phi(100, 0.0, 2. * M_PI);
  mygsl::histogram_1d h_cost(100, -1.0, +1.0);
  int nshoots = 10000;
  for (int ishoot = 0; ishoot < nshoots; ishoot++) {
    double e, theta, phi;
    GT.fire_event_kinematics(random, e, theta, phi);
    h_e.fill(e / CLHEP::MeV);
    h_phi.fill(phi);
    h_cost.fill(std::cos(theta));
  }
  std::ofstream ofhist(histo_filename.c_str());
  h_e.print(ofhist);
  ofhist << std::endl << std::endl;
  h_phi.print(ofhist);
  ofhist << std::endl << std::endl;
  h_cost.print(ofhist);
  ofhist << std::endl << std::endl;
  ofhist.close();

  if (draw_) {
    Gnuplot g1("line");
    g1.cmd("set title 'Test genbb::gamma_transition' ");
    g1.cmd("set grid");
    g1.cmd("set key out");
    g1.cmd("set xlabel 'E (MeV)'");
    g1.cmd("set ylabel 'Counts'");
    {
      std::ostringstream plot_cmd;
      plot_cmd << "plot '" << histo_filename << "' index 0 u (0.5*(column(1)+column(2))):3 title 'Gamma energy' with histeps ";
      g1.cmd(plot_cmd.str ());
      g1.showonscreen(); // window output
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }
    {
      std::ostringstream plot_cmd;
      plot_cmd << "plot '" << histo_filename << "' index 1 u (0.5*(column(1)+column(2))):3 title 'Gamma phi' with histeps ";
      g1.cmd(plot_cmd.str ());
      g1.showonscreen(); // window output
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }
  }

  return;
}

// test_nuclear_transition.cxx

// Ourselves
#include <genbb_help/nuclear_transition.h>

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
  std::clog << "\nTest 1: \n";
  mygsl::rng random("taus", 314159);

  // Describe the gamma transition between level Rn-222 (186.2 keV) and Rn-222 (0):
  genbb::nuclear_transition NT;
  NT.set_Z(86);
  NT.set_A(222);
  NT.set_energy(186.211 * CLHEP::keV);
  NT.set_ic_k_shell_binding_energy(98.0 * CLHEP::keV);
  NT.set_ic_l_shell_binding_energy(17.0 * CLHEP::keV);
  NT.set_ic_m_shell_binding_energy( 4.0 * CLHEP::keV);
  NT.set_ic_k_shell_coefficient(0.190);
  NT.set_ic_l_shell_coefficient(0.360);
  NT.set_ic_m_shell_coefficient(0.0963);
  NT.set_pc_coefficient(0.0);

  NT.initialize_simple();
  NT.tree_dump(std::clog, "Rn-222(186.2 keV)->Rn-222(0): ");
  std::clog << std::endl;

  std::string histo_filename = "test_nuclear_transition_hist.data";
  std::clog << "Shoot events..." << std::endl;
  mygsl::histogram_1d h_g(200, 0.0, 200.0);
  mygsl::histogram_1d h_x(200, 0.0, 200.0);
  mygsl::histogram_1d h_e(200, 0.0, 200.0);
  int nshoots = 1000000;
  for (int ishoot = 0; ishoot < nshoots; ishoot++) {
    genbb::nuclear_transition::mode_type nc_mode;
    double eg, thetag, phig;
    double k1, theta1, phi1;
    double k2, theta2, phi2;
    NT.fire_event_kinematics(random, nc_mode, eg, thetag, phig, k1, theta1, phi1, k2, theta2, phi2);
    if (nc_mode == genbb::nuclear_transition::MODE_GAMMA) {
      h_g.fill(eg / CLHEP::keV);
    } else if (nc_mode < genbb::nuclear_transition::MODE_PC) {
      h_x.fill(eg / CLHEP::keV);
      h_e.fill(k1 / CLHEP::keV);
    }
  }
  std::ofstream ofhist(histo_filename.c_str());
  h_g.print(ofhist);
  ofhist << std::endl << std::endl;
  h_x.print(ofhist);
  ofhist << std::endl << std::endl;
  h_e.print(ofhist);
  ofhist << std::endl << std::endl;
  ofhist.close();

  if (draw_) {
    Gnuplot g1("line");
    g1.cmd("set title 'Test genbb::nuclear_transition' ");
    g1.cmd("set grid");
    g1.cmd("set key out");
    g1.cmd("set xlabel 'E (keV)'");
    g1.cmd("set ylabel 'Counts'");
    {
      std::ostringstream plot_cmd;
      plot_cmd << "plot '" << histo_filename << "' index 0 u (0.5*(column(1)+column(2))):3 title 'Gamma energy' with histeps ";
      plot_cmd << ", '" << histo_filename << "' index 1 u (0.5*(column(1)+column(2))):3 title 'X energy' with histeps ";
      plot_cmd << ", '" << histo_filename << "' index 2 u (0.5*(column(1)+column(2))):3 title 'conversion electron energy' with histeps ";
      g1.cmd(plot_cmd.str ());
      g1.showonscreen(); // window output
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }
  }

  return;
}

// test_alpha_decay.cxx

// Ourselves
#include <genbb_help/alpha_decay.h>

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
#include <mygsl/histogram_2d.h>

void test1(bool draw_);

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    bool draw = false;
    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "--draw") {
        draw = true;
      }
      iarg++;
    }

    test1(draw);

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

void test1(bool draw_)
{
  std::clog << "\nTest 1: \n";
  mygsl::rng random("taus", 314159);

  double mp = 938.27203 * CLHEP::MeV;
  double mn = 939.56536 * CLHEP::MeV;
  {
    // Describe the alpha decay of Ra-226:
    genbb::alpha_decay decay;
    decay.set_A(226);
    decay.set_Z(88);
    // From Ame2012:
    // 1N-Z    N    Z   A  EL    O     MASS EXCESS       BINDING ENERGY/A        BETA-DECAY ENERGY         ATOMIC MASS          V/S
    //                                   (keV)                (keV)                    (keV)                (micro-u)
    // 50  138   88  226 Ra        23669.571      2.308     7661.954    0.010 B-   -640.917    3.279 226 025410.330      2.477
    // 50  136   86  222 Rn        16374.031      2.321     7694.489    0.010 B-     24.269   21.120 222 017578.246      2.492
    //
    double m_Ra226 = decay.get_Z_parent() * mp + (decay.get_A_parent() - decay.get_Z_parent()) * mn - decay.get_A_parent() * 7661.954 * CLHEP::keV;
    double m_Rn222 = decay.get_Z_daughter() * mp + (decay.get_A_daughter() - decay.get_Z_daughter()) * mn - decay.get_A_daughter() * 7694.489 * CLHEP::keV;
    decay.set_mass_parent(m_Ra226);
    decay.set_mass_daughter(m_Rn222);
    decay.initialize_simple();
    decay.tree_dump(std::clog, "Ra226 alpha decay: ");
    std::clog << std::endl;

    std::string histo_filename = "test_beta_decay_Ra226_hist_ke_pr.data";
    {
      std::clog << "Shoot events..." << std::endl;
      mygsl::histogram_1d h_ka(1000, 0.0, decay.get_q_alpha());
      int nshoots = 1000;
      for (int ishoot = 0; ishoot < nshoots; ishoot++) {
        double ka, thetaa, phia, kr, thetar, phir;
        int err = decay.fire_event_kinematics(random, ka, thetaa, phia, kr, thetar, phir);
        if (err != 0) {
          //
        }
        h_ka.fill(ka);
      }
      std::ofstream ofhist(histo_filename.c_str());
      h_ka.print(ofhist);
      ofhist << std::endl << std::endl;
      ofhist.close();
    }

    if (draw_) {
      Gnuplot g1("line");
      g1.cmd("set title 'Test genbb::alpha_decay' ");
      g1.cmd("set grid");
      g1.cmd("set key out");
      g1.cmd("set xlabel 'pr (MeV)'");
      g1.cmd("set ylabel 'ke (MeV)'");

      {
        std::ostringstream plot_cmd;
        g1.cmd("set title 'Test genbb::alpha_decay: ka spectrum' ");
        g1.cmd("set grid");
        g1.cmd("set key out");
        g1.cmd("set yrange [0:1200]");
        g1.cmd("set xlabel 'ka (MeV)'");
        g1.cmd("set ylabel 'Counts'");
        plot_cmd << "plot '" << histo_filename << "' index 0 u (0.5*(column(1)+column(2))):3 title '' with histeps ";
        g1.cmd(plot_cmd.str ());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

    } // draw

    unlink(histo_filename.c_str());
  }

  return;
}

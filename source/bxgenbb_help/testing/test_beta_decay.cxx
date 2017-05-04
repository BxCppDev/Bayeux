// test_beta_decay.cxx

// Ourselves
#include <genbb_help/beta_decay.h>

// Standard library:
#include <iostream>
#include <cstdlib>
#include <exception>

// Third party:
// - CLHEP:
#include<CLHEP/Units/PhysicalConstants.h>
#include<CLHEP/Units/SystemOfUnits.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>
#include <mygsl/histogram_1d.h>
#include <mygsl/histogram_2d.h>
// - Bayeux/geomtools:
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>

void test1(bool draw_, bool tensor_);

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
      } else if (arg == "--tensor") {
        tensor = true;
      }
      iarg++;
    }

    test1(draw, tensor);

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

void test1(bool draw_, bool tensor_)
{
  std::clog << "\nTest 1: \n";
  mygsl::rng random("taus", 314159);

  double mp = 938.27203 * CLHEP::MeV;
  double mn = 939.56536 * CLHEP::MeV;
  {
    // Describe the beta+ decay of Ar-35 to Cl-35:
    genbb::beta_decay decay;
    decay.set_A(35);
    decay.set_Z_parent(18);
    decay.set_Z_daughter(17);
    decay.set_type(genbb::beta_decay::BETA_DECAY_PLUS);
    // From Ame2012:
    // 1N-Z    N    Z   A  EL    O     MASS EXCESS       BINDING ENERGY/A        BETA-DECAY ENERGY         ATOMIC MASS          V/S
    //                                   (keV)                (keV)                    (keV)                (micro-u)
    //  1   18   17   35 Cl       -29013.540      0.035     8520.279    0.001 B-  -5966.130    0.746  34 968852.682      0.037
    // -1   17   18   35 Ar    -  -23047.410      0.746     8327.465    0.021 B- -11874.519    0.905  34 975257.586      0.801
    //
    double m_Ar35 = decay.get_Z_parent() * mp + (decay.get_A() - decay.get_Z_parent()) * mn - decay.get_A() * 8327.465 * CLHEP::keV;
    double m_CL35 = decay.get_Z_daughter() * mp + (decay.get_A() - decay.get_Z_daughter()) * mn - decay.get_A() * 8520.279 * CLHEP::keV;
    decay.set_mass_parent(m_Ar35);
    decay.set_mass_daughter(m_CL35);
    // decay.set_q_beta(5966.1 * CLHEP::keV - 2. * CLHEP::electron_mass_c2);
    decay.set_coupling(genbb::beta_decay::COUPLING_USER);
    decay.set_a(+0.996);
    decay.initialize_simple();
    decay.tree_dump(std::clog, "Ar35 beta+ decay: ");
    std::clog << std::endl;
    if (draw_) {
      decay.display_pdf("Ar35 beta+ decay");
    }
  }

  {
    // Describe the beta- decay of He6:
    genbb::beta_decay decay;
    decay.set_A(6);
    decay.set_Z_parent(2);
    decay.set_Z_daughter(3);
    decay.set_type(genbb::beta_decay::BETA_DECAY_MINUS);
    // From Ame2012:
    // 1N-Z    N    Z   A  EL    O     MASS EXCESS       BINDING ENERGY/A        BETA-DECAY ENERGY         ATOMIC MASS          V/S
    //                                   (keV)                (keV)                    (keV)                (micro-u)
    //   2    4    2    6 He        17592.095      0.053     4878.519    0.009 B-   3505.216    0.053   6 018885.891      0.057
    //   0    3    3    6 Li        14086.87893    0.00144   5332.331    0.000 B-  -4288.155    5.448   6 015122.88742    0.00155
    //
    double m_He6 = decay.get_Z_parent() * mp + (decay.get_A() - decay.get_Z_parent()) * mn - decay.get_A() * 4878.519 * CLHEP::keV;
    double m_Li6 = decay.get_Z_daughter() * mp + (decay.get_A() - decay.get_Z_daughter()) * mn - decay.get_A() * 5332.331 * CLHEP::keV;
    decay.set_mass_parent(m_He6);
    decay.set_mass_daughter(m_Li6);
    // decay.set_q_beta(3505.216 * CLHEP::keV);
    decay.set_ke_cut(10.0 * CLHEP::keV);
    if (tensor_) {
      decay.set_coupling(genbb::beta_decay::COUPLING_TENSOR);
    } else {
      decay.set_coupling(genbb::beta_decay::COUPLING_AXIAL_VECTOR);
    }
    std::string log_filename = "test_beta_decay_log_He6.data";
    decay.set_log_filename(log_filename);
    decay.initialize_simple();
    decay.tree_dump(std::clog, "He6 beta- decay: ");
    std::clog << std::endl;

    std::string histo_filename = "test_beta_decay_He6_hist_ke_pr.data";
    {
      std::clog << "Shoot events..." << std::endl;
      mygsl::histogram_1d h_ke(100, 0.0, decay.get_q_beta());
      mygsl::histogram_1d h_pr(100, 0.0, decay.get_pr_max());
      mygsl::histogram_2d h_ke_pr(100, 0.0, decay.get_q_beta(),
                                  100, 0.0, decay.get_pr_max());
      int nshoots = 50000;
      for (int ishoot = 0; ishoot < nshoots; ishoot++) {
        double ke, pr, pnu, cos_er, cos_enu;
        int err = decay.fire_event_kinematics(random, ke, pr, pnu, cos_er, cos_enu);
        if (err != 0) {
          //
        }
        h_ke.fill(ke);
        h_pr.fill(pr);
        h_ke_pr.fill(ke, pr);
      }
      std::ofstream ofhist(histo_filename.c_str());
      h_ke.print(ofhist);
      ofhist << std::endl << std::endl;
      h_pr.print(ofhist);
      ofhist << std::endl << std::endl;
      h_ke_pr.print(ofhist);
      ofhist << std::endl << std::endl;
      ofhist.close();
    }

    if (draw_) {
      Gnuplot g1("line");
      g1.cmd("set title 'Test genbb::beta_decay' ");
      g1.cmd("set grid");
      g1.cmd("set key out");
      g1.cmd("set xlabel 'pr (MeV)'");
      g1.cmd("set ylabel 'ke (MeV)'");
      {
        std::ostringstream plot_cmd;
        plot_cmd << "plot '" << log_filename << "' index 0 u 1:2 title 'Phase space upper limit' with lines ";
        plot_cmd << ",    '" << log_filename << "' index 0 u 1:3 title 'Phase space lower limit' with lines ";
        plot_cmd << ",    '" << log_filename << "' index 1 u 1:2 title 'Max' with points pt 6 ";
        g1.cmd(plot_cmd.str ());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      decay.display_pdf("He6 beta- decay");

      {
        std::ostringstream plot_cmd;
        g1.cmd("set title 'Test genbb::beta_decay: ke spectrum' ");
        g1.cmd("set grid");
        g1.cmd("set key out");
        g1.cmd("set xlabel 'ke (MeV)'");
        g1.cmd("set ylabel 'Counts'");
        plot_cmd << "plot '" << histo_filename << "' index 0 u (0.5*(column(1)+column(2))):3 title '' with histeps ";
        g1.cmd(plot_cmd.str ());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        g1.cmd("set title 'Test genbb::beta_decay: pr spectrum' ");
        g1.cmd("set grid");
        g1.cmd("set key out");
        g1.cmd("set xlabel 'pr (MeV)'");
        g1.cmd("set ylabel 'Counts'");
        plot_cmd << "plot '" << histo_filename << "' index 1 u (0.5*(column(1)+column(2))):3 title '' with histeps ";
        g1.cmd(plot_cmd.str ());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        g1.cmd("set title 'Test genbb::beta_decay: ke-pr spectrum' ");
        g1.cmd("set grid");
        g1.cmd("set key out");
        g1.cmd("set xlabel 'ke (MeV)'");
        g1.cmd("set ylabel 'pr (MeV)'");
        g1.cmd("set zlabel 'Counts'");
        plot_cmd << "splot '" << histo_filename << "' index 2 u (0.5*(column(1)+column(2))):(0.5*(column(3)+column(4))):5 title '' with dots ";
        g1.cmd(plot_cmd.str ());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

    } // draw

    unlink(log_filename.c_str());
    unlink(histo_filename.c_str());
  }

  return;
}

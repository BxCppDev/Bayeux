// test_nuclear_decay_manager.cxx

// Ourselves:
#include <genbb_help/nuclear_decay_manager.h>

// Third party:
// - CLHEP:
#include <CLHEP/Units/PhysicalConstants.h>
#include <CLHEP/Units/SystemOfUnits.h>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/properties.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>
#include <mygsl/histogram_1d.h>
#include <mygsl/histogram_2d.h>
// - Bayeux/geomtools:
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>

// This project:
#include <genbb_help/nuclear_level.h>
#include <genbb_help/nuclear_decay.h>
#include <genbb_help/primary_event.h>
#include <genbb_help/base_decay_driver.h>
#include <genbb_help/beta_decay.h>
#include <genbb_help/nuclear_transition.h>

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
  std::cerr << "DEVEL: test1: Entering..." << std::endl;
  mygsl::rng random("taus", 314159);

  std::string decaying_level_name = "He6_0";
  // decaying_level_name = "Li6_3563";

  std::string filename = "${GENBB_HELP_TESTING_DIR}/config/test_nuclear_decay_manager_0.conf";
  datatools::fetch_path_with_env(filename);
  datatools::properties NDMconfig;
  datatools::read_config(filename, NDMconfig);
  NDMconfig.tree_dump(std::clog, "Nuclear decay manager configuration: ");

  genbb::nuclear_decay_manager NDM;
  NDM.initialize(NDMconfig);
  std::cerr << "DEVEL: test1: initialization done." << std::endl;
  NDM.tree_dump(std::clog, "Nuclear decay manager: ");

  const genbb::nuclear_level & level =
    NDM.get_levels().find(decaying_level_name)->second.get();
  level.tree_dump(std::clog, "Level : ");

  std::string histo_filename = "test_nuclear_decay_manager_hist.data";
  mygsl::histogram_1d h_ke(100, 0.0, 4.0 * CLHEP::MeV);
  mygsl::histogram_1d h_pr(100, 0.0, 4.0 * CLHEP::MeV);
  mygsl::histogram_2d h_ke_pr(100, 0.0, 4.0 * CLHEP::MeV,
                              100, 0.0, 4.0 * CLHEP::MeV);

  std::clog << "Shoot events..." << std::endl;
  genbb::beta_decay         * bd = 0;
  genbb::nuclear_transition * nt = 0;
  for (int idecay = 0; idecay < 100; idecay++) {
    genbb::primary_event event;
    double p = random.flat(0.0, 1.0);
    for (int i = 0; i < (int) level.get_number_of_decay_channels(); i++) {
      const genbb::nuclear_level::decay_channel & dc = level.get_decay_channel(i);
      if (p < dc.get_cumul_prob()) {
        genbb::nuclear_decay & decay =
          const_cast<genbb::nuclear_decay &>(dc.get_decay());
        genbb::base_decay_driver & dd = decay.grab_decay_driver();
        if (decaying_level_name == "He6_0") {
          if (!bd) {
            bd = dynamic_cast<genbb::beta_decay *>(&dd);
            if (draw_) {
              bd->tree_dump(std::cerr, "Beta decay: ", "DEVEL: ");
              bd->display_pdf("He6 beta- decay");
            }
          }
        } else {
          if (!nt) {
            nt = dynamic_cast<genbb::nuclear_transition *>(&dd);
            nt->tree_dump(std::cerr, "Nuclear transition: ", "DEVEL: ");
          }
        }
        dd.fill(random, event);
        std::size_t nb_particles = event.get_number_of_particles();
        if (decaying_level_name == "He6_0") {
          if (nb_particles == 2) {
            double ke = event.get_particle(0).get_kinetic_energy();
            double pr = event.get_particle(1).get_momentum().mag();
            // if (pr < 50.0 * CLHEP::keV || !datatools::is_valid(pr)) {
            //   std::cerr << "DEVEL: test1: pr = " << pr / CLHEP::keV << " keV" << std::endl;
            //   std::cerr << "DEVEL: test1: ke = " << ke / CLHEP::keV << " keV" << std::endl;
            //   event.get_particle(1).tree_dump(std::cerr, "Bad recoil: ", "DEVEL: ");
            // }
            h_ke.fill(ke);
            h_pr.fill(pr);
            h_ke_pr.fill(ke, pr);
          } else {
            event.tree_dump(std::cerr, "Bad event: ", "DEVEL: ");
          }
        } else {
          double kg = event.get_particle(0).get_kinetic_energy();
          double pr = 0.0;
          h_ke.fill(kg);
          h_pr.fill(pr);
          h_ke_pr.fill(kg, pr);
        }
      }
    }
  }
  std::ofstream ofhist(histo_filename.c_str());
  h_ke.print(ofhist);
  ofhist << std::endl << std::endl;
  h_pr.print(ofhist);
  ofhist << std::endl << std::endl;
  h_ke_pr.print(ofhist);
  ofhist << std::endl << std::endl;
  ofhist.close();

  if (draw_) {
    Gnuplot g1("line");
    g1.cmd("set title 'Test genbb::nuclear_decay_manager' ");
    g1.cmd("set grid");
    g1.cmd("set key out");

    {
      std::ostringstream plot_cmd;
      g1.cmd("set title 'Test genbb::nuclear_decay_manager: ke spectrum' ");
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
      g1.cmd("set title 'Test genbb::nuclear_decay_manager: pr spectrum' ");
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
      g1.cmd("set title 'Test genbb::nuclear_decay_manager: ke-pr spectrum' ");
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

  }
  unlink(histo_filename.c_str());
  std::cerr << "DEVEL: test1: Exiting." << std::endl;
  return;
}

// test_fermi_function_2.cxx

// Ourselves
#include <genbb_help/fermi_function.h>

// Standard library:
#include <iostream>
#include <cstdlib>
#include <exception>

// Third party:
// - CLHEP:
#include<CLHEP/Units/PhysicalConstants.h>
#include<CLHEP/Units/SystemOfUnits.h>
// - Bayeux/datatools:
#include <datatools/temporary_files.h>
// - Bayeux/materials:
#include <materials/isotope.h>
// - Bayeux/geomtools:
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>

// This project:
#include <genbb_help/beta_decay.h>

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    bool draw = false;
    bool shape_only = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "--draw") {
        draw = true;
      } else if (arg == "--shape-only") {
        shape_only = true;
      }
      iarg++;
    }

    {
      // Describe the beta- decay of He6:
      genbb::beta_decay decay;
      decay.set_A(6);
      decay.set_Z_parent(2);
      decay.set_Z_daughter(3);
      decay.set_type(genbb::beta_decay::BETA_DECAY_MINUS);
      decay.set_q_beta(3505 * CLHEP::keV);
      decay.set_mass_parent(materials::isotope::table_record_from_id(materials::isotope::id(2,6)).get_nucleus_mass());
      decay.set_mass_daughter(materials::isotope::table_record_from_id(materials::isotope::id(3,6)).get_nucleus_mass());
      decay.set_coupling(genbb::beta_decay::COUPLING_AXIAL_VECTOR);
      decay.initialize_simple();
      decay.tree_dump(std::clog, "He6 beta- decay: ");
      std::clog << std::endl;

      // Fermi function objects:
      double ke_cut = 0.5 * CLHEP::keV;

      unsigned int mode_NR = genbb::fermi_function::MODE_NON_RELATIVISTIC;
      if (shape_only) mode_NR |= genbb::fermi_function::MODE_SHAPE_ONLY;
      genbb::fermi_function FNR(decay.get_Z_daughter(),
                                decay.get_A(),
                                (decay.is_beta_minus() ?
                                 genbb::fermi_function::DT_BETA_MINUS :
                                 genbb::fermi_function::DT_BETA_PLUS),
                                mode_NR,
                                ke_cut);

      unsigned int mode_SS = genbb::fermi_function::MODE_SPHERICALLY_SYMMETRIC;
      if (shape_only) mode_SS |= genbb::fermi_function::MODE_SHAPE_ONLY;
      genbb::fermi_function FSS(decay.get_Z_daughter(),
                                decay.get_A(),
                                (decay.is_beta_minus() ?
                                 genbb::fermi_function::DT_BETA_MINUS :
                                 genbb::fermi_function::DT_BETA_PLUS),
                                mode_SS, ke_cut);

      datatools::temp_file tmp_file;
      tmp_file.set_remove_at_destroy (true);
      tmp_file.create ("/tmp", "test_fermi_function_2_");
      tmp_file.out().precision(15);

      double dke = 0.1 * CLHEP::keV;
      for (double ke = 1.0 * CLHEP::keV; ke < std::min(1.0 * CLHEP::MeV, decay.get_q_beta()); ke += dke) {
        tmp_file.out() << ke
                       << ' ' << FNR(ke)
                       << ' ' << FSS(ke)
                       << std::endl;
      }

      if (draw) {
        Gnuplot g1("line");
        g1.cmd("set title 'Test genbb::fermi_function' ");
        g1.cmd("set grid");
        g1.cmd("set key out");
        g1.cmd("set xlabel 'ke (MeV)'");
        g1.cmd("set ylabel 'F(ke)'");
        {
          std::ostringstream plot_cmd;
          plot_cmd << "plot ";
          plot_cmd << "  '" << tmp_file.get_filename() << "' index 0 u 1:2 title 'NR' with lines ";
          plot_cmd << ", '" << tmp_file.get_filename() << "' index 0 u 1:3 title 'SS' with lines ";
          g1.cmd (plot_cmd.str ());
          g1.showonscreen (); // window output
          geomtools::gnuplot_drawer::wait_for_key ();
          usleep (200);
        }
      }

    }

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

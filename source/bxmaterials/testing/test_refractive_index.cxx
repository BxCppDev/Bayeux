// Standard library:
#include <cstdlib>
#include <iostream>
#include <stdexcept>

// Bayeux:
#include <datatools/clhep_units.h>

// This project:
#include <materials/refractive_index.h>

int main(/* int argc_, char ** argv_ */) {
  try {

    double l1  =  300.0 * CLHEP::nm;
    double l2  = 2500.0 * CLHEP::nm;

    {
      materials::refractive_index r;
      r.make_constant(1.52);
      r.initialize_simple();
      r.write_ascii_file_with_units("test_refractive_index_N-BK7_n.data", l1, l2, 100, "um", "");
    }

    {
      materials::refractive_index rBK7;
      materials::refractive_index::coeff_dict_type rBK7_coeffs;
      rBK7_coeffs[1] = 0.0;
      rBK7_coeffs[2] = 1.03961212;
      rBK7_coeffs[3] = 0.00600069867;
      rBK7_coeffs[4] = 0.231792344;
      rBK7_coeffs[5] = 0.0200179144;
      rBK7_coeffs[6] = 1.01046945;
      rBK7_coeffs[7] = 103.560653;
      rBK7.set_lambda_range(l1, l2);
      rBK7.make_dispersion_rii(materials::refractive_index::FORMULA_DISPERSION_RII_SELLMEIER2,
                               rBK7_coeffs);
      rBK7.initialize_simple();

      // double lambda = 500.0 * CLHEP::nm;
      // double n = rBK7(lambda);
      // std::clog << "lambda = " << lambda / CLHEP::um << " [um] -> n=" << n << std::endl;

      rBK7.write_ascii_file_with_units("test_refractive_index_N-BK7_n_2.data", l1, l2, 100, "um", "");
    }

    {
      materials::refractive_index rBK7;
      rBK7.make_tabulated("${MATERIALS_RESOURCE_DIR}/data/optical/1.0/Glass/N-BK7/N-BK7_n.data",
                          "${MATERIALS_RESOURCE_DIR}/data/optical/1.0/Glass/N-BK7/N-BK7_k.data");
      rBK7.initialize_simple();
      rBK7.write_ascii_file_with_units("test_refractive_index_N-BK7_n_3.data", l1, l2, 100, "um", "");
      rBK7.set_eval_k();
      rBK7.write_ascii_file_with_units("test_refractive_index_N-BK7_k.data", l1, l2, 100, "um", "");
    }

    {
      materials::refractive_index rFQ;
      datatools::properties rFQ_config;
      std::string rFQ_config_file = "${MATERIALS_RESOURCE_DIR}/data/optical/1.0/Glass/FusedQuartz/FusedQuartz.conf";
      datatools::fetch_path_with_env(rFQ_config_file);
      datatools::properties::read_config(rFQ_config_file, rFQ_config);
      rFQ.initialize_standalone(rFQ_config);
      rFQ.write_ascii_file_with_units("test_refractive_index_FQ_n.data", l1, l2, 100, "um", "");
    }

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  } catch(...) {
    std::cerr << "error: " << "An unexpected error was caught!" << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

// test_nuclear_level.cxx

// Ourselves
#include <genbb_help/nuclear_level.h>

// Third party:
// - CLHEP:
#include<CLHEP/Units/PhysicalConstants.h>
#include<CLHEP/Units/SystemOfUnits.h>

void test1();
void test2();

int main (int /* argc_ */, char ** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {

    test1();

    test2();

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

void test1()
{
  std::clog << "\nTest 1: \n";
  genbb::nuclear_level He6_fundamental;
  He6_fundamental.set_A(6);
  He6_fundamental.set_Z(2);
  He6_fundamental.set_spin(0);
  He6_fundamental.set_parity(genbb::nuclear_level::PARITY_PLUS);
  He6_fundamental.set_half_life(0.801 * CLHEP::second);
  He6_fundamental.set_fundamental();
  He6_fundamental.tree_dump(std::clog, "He6 fundamental level:");

  genbb::nuclear_level Li6_fundamental;
  Li6_fundamental.set_A(6);
  Li6_fundamental.set_Z(3);
  Li6_fundamental.set_spin(1);
  Li6_fundamental.set_parity(genbb::nuclear_level::PARITY_PLUS);
  Li6_fundamental.set_stable();
  Li6_fundamental.set_fundamental();
  Li6_fundamental.tree_dump(std::clog, "Li6 fundamental level:");

  genbb::nuclear_level Li6_excited_1;
  Li6_excited_1.set_A(6);
  Li6_excited_1.set_Z(3);
  Li6_excited_1.set_spin(3);
  Li6_excited_1.set_parity(genbb::nuclear_level::PARITY_PLUS);
  Li6_excited_1.set_width(24 * CLHEP::keV);
  Li6_excited_1.set_energy(2186 * CLHEP::keV);
  Li6_excited_1.tree_dump(std::clog, "Li6 first excited state:");

  genbb::nuclear_level Li6_excited_2;
  Li6_excited_2.set_A(6);
  Li6_excited_2.set_Z(3);
  Li6_excited_2.set_spin(0);
  Li6_excited_2.set_parity(genbb::nuclear_level::PARITY_PLUS);
  Li6_excited_2.set_width(8.2 * CLHEP::eV);
  Li6_excited_2.set_energy(3562.9 * CLHEP::keV);
  Li6_excited_2.tree_dump(std::clog, "Li6 second excited state:");

  genbb::nuclear_level Li6_excited_3;
  Li6_excited_3.set_A(6);
  Li6_excited_3.set_Z(3);
  Li6_excited_3.set_spin(2);
  Li6_excited_3.set_parity(genbb::nuclear_level::PARITY_PLUS);
  Li6_excited_3.set_width(30.0 * CLHEP::MeV);
  Li6_excited_3.set_energy(4312 * CLHEP::keV);
  Li6_excited_3.tree_dump(std::clog, "Li6 third excited state:");

  return;
}

void test2()
{
  std::clog << "\nTest 2: \n";
  datatools::properties He6_0_config;
  He6_0_config.store("Z", 2);
  He6_0_config.store("A", 6);
  He6_0_config.store("spin", "3/2");
  He6_0_config.store("parity", "+");
  He6_0_config.store("half_life", 806.7 * CLHEP::ms);
  He6_0_config.set_explicit_unit("half_life", true);
  He6_0_config.set_unit_symbol("half_life", "ms");
  He6_0_config.store_flag("fundamental");
  He6_0_config.tree_dump(std::clog, "He6 fundamental level:");
  He6_0_config.store("aux.PyNE.Id", 20060000);
  He6_0_config.store("aux.test", "Hello World!");
  He6_0_config.store("aux.inhibit.beta", true);

  genbb::nuclear_level He6_0;
  He6_0.initialize(He6_0_config);
  He6_0.tree_dump(std::clog, "He6 fundamental:");

  return;
}

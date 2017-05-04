// test_nuclear_decay.cxx

// Ourselves
#include <genbb_help/nuclear_level.h>
#include <genbb_help/nuclear_decay.h>

// Third party:
// - CLHEP:
#include<CLHEP/Units/PhysicalConstants.h>
#include<CLHEP/Units/SystemOfUnits.h>

void test1();
void test2();
void test3();

int main (int /* argc_ */, char ** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {

    test1();

    test2();

    test3();

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
  He6_fundamental.set_half_life(0.8067 * CLHEP::second);
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

  genbb::nuclear_decay He6_Li6_decay;
  He6_Li6_decay.set_level_initial(He6_fundamental);
  He6_Li6_decay.set_level_final(Li6_fundamental);
  // He6_Li6_decay.set_logging(datatools::logger::PRIO_TRACE);

  datatools::properties He6_Li6_decay_config;
  He6_Li6_decay_config.store("decay_id", "genbb::beta_decay");
  He6_Li6_decay_config.store("driver.coupling", "A");
  He6_Li6_decay_config.store("driver.ke_cut", 10 * CLHEP::keV);
  He6_Li6_decay_config.set_explicit_unit("driver.ke_cut", true);
  He6_Li6_decay_config.set_unit_symbol("driver.ke_cut", "keV");

  He6_Li6_decay.initialize(He6_Li6_decay_config);
  He6_Li6_decay.tree_dump(std::clog, "He6->Li6 beta decay:");

  He6_fundamental.add_decay_channel(He6_Li6_decay, 1.0);
  He6_fundamental.tree_dump(std::clog, "He6 fundamental level:");

  return;
}

void test2()
{
  std::clog << "\nTest 2: \n";
  genbb::nuclear_level Mo100_fundamental;
  Mo100_fundamental.set_A(100);
  Mo100_fundamental.set_Z(42);
  Mo100_fundamental.set_spin(0);
  Mo100_fundamental.set_parity(genbb::nuclear_level::PARITY_PLUS);
  double year = 365.25 * 24 * 3600 * CLHEP::second;
  Mo100_fundamental.set_half_life(7.3e18 * year);
  Mo100_fundamental.set_fundamental();

  genbb::nuclear_level Ru100_fundamental;
  Ru100_fundamental.set_A(100);
  Ru100_fundamental.set_Z(44);
  Ru100_fundamental.set_spin(0);
  Ru100_fundamental.set_parity(genbb::nuclear_level::PARITY_PLUS);
  Ru100_fundamental.set_stable();
  Ru100_fundamental.set_fundamental();
  Ru100_fundamental.tree_dump(std::clog, "Ru100 fundamental level:");

  genbb::nuclear_decay Mo100_Ru100;
  Mo100_Ru100.set_level_initial(Mo100_fundamental);
  Mo100_Ru100.set_level_final(Ru100_fundamental);
  Mo100_Ru100.set_decay_id("DBD");
  Mo100_Ru100.tree_dump(std::clog, "Mo100->Ru100 double beta decay:");
  std::string error_message;
  if (!Mo100_Ru100.check_decay(error_message)) {
    std::clog << "Nuclear decay is not valid: " << error_message << std::endl;
  }

  Mo100_fundamental.add_decay_channel(Mo100_Ru100, 1.0);
  Mo100_fundamental.tree_dump(std::clog, "Mo-100 fundamental level:");

  return;
}

void test3()
{
  std::clog << "\nTest 3: \n";
  // The fundamental level of Ar-35:
  genbb::nuclear_level::handle_type hAr35_0(new genbb::nuclear_level);
  genbb::nuclear_level & Ar35_0 = hAr35_0.grab();
  Ar35_0.set_A(35);
  Ar35_0.set_Z(18);
  Ar35_0.set_spin(3./2);
  Ar35_0.set_parity(genbb::nuclear_level::PARITY_PLUS);
  Ar35_0.set_half_life(1.7756 * CLHEP::second);
  Ar35_0.set_fundamental();

  // The list of levels of Cl-35:
  std::vector<genbb::nuclear_level::handle_type> Cl35_levels;

  // The fundamental level of Cl-35:
  genbb::nuclear_level::handle_type hCl35_0(new genbb::nuclear_level);
  Cl35_levels.push_back(hCl35_0);
  genbb::nuclear_level & Cl35_0 = Cl35_levels.back().grab();
  Cl35_0.set_A(35);
  Cl35_0.set_Z(17);
  Cl35_0.set_spin(3./2);
  Cl35_0.set_parity(genbb::nuclear_level::PARITY_PLUS);
  Cl35_0.set_stable();
  Cl35_0.set_fundamental();
  Cl35_0.tree_dump(std::clog, "Cl35 fundamental level:");

  // The beta+ decay Ar-35 -> Cl-35(0):
  genbb::nuclear_decay::handle_type hAr35_Cl35_0(new genbb::nuclear_decay);
  genbb::nuclear_decay & Ar35_Cl35_0 = hAr35_Cl35_0.grab();
  Ar35_Cl35_0.set_decay_id("genbb::beta_decay");
  Ar35_Cl35_0.set_level_initial(Ar35_0);
  Ar35_Cl35_0.set_level_final(Cl35_0);
  Ar35_Cl35_0.grab_auxiliaries().store("beta.coupling", "T"); // Tensor coupling
  Ar35_Cl35_0.grab_auxiliaries().store("beta.Q", 4944.1 * CLHEP::keV); // Q-beta
  Ar35_Cl35_0.tree_dump(std::clog, "Ar35->Cl35 beta+ decay:");
  Ar35_0.add_decay_channel(Ar35_Cl35_0, 98.23 * CLHEP::perCent);

  // The first excited state of Cl-35:
  genbb::nuclear_level::handle_type hCl35_1219(new genbb::nuclear_level);
  Cl35_levels.push_back(hCl35_1219);
  genbb::nuclear_level & Cl35_1219 = Cl35_levels.back().grab();
  Cl35_1219.set_A(35);
  Cl35_1219.set_Z(17);
  Cl35_1219.set_spin(1./2);
  Cl35_1219.set_parity(genbb::nuclear_level::PARITY_PLUS);
  Cl35_1219.set_half_life(0.0);
  Cl35_1219.set_energy(1219.2 * CLHEP::keV);
  Cl35_1219.tree_dump(std::clog, "Cl35 excited state :");

  // The beta+ decay Ar-35 -> Cl-35(1219):
  genbb::nuclear_decay::handle_type hAr35_Cl35_1219(new genbb::nuclear_decay);
  genbb::nuclear_decay & Ar35_Cl35_1219 = hAr35_Cl35_1219.grab();
  Ar35_Cl35_1219.set_decay_id("genbb::beta_decay");
  Ar35_Cl35_1219.set_level_initial(Ar35_0);
  Ar35_Cl35_1219.set_level_final(Cl35_1219);
  Ar35_Cl35_1219.tree_dump(std::clog, "Ar35->Cl35 beta+ decay:");
  Ar35_0.add_decay_channel(Ar35_Cl35_1219, 1.23 * CLHEP::perCent);

  // The second excited state of Cl-35:
  genbb::nuclear_level::handle_type hCl35_1763(new genbb::nuclear_level);
  Cl35_levels.push_back(hCl35_1763);
  genbb::nuclear_level & Cl35_1763 = Cl35_levels.back().grab();
  Cl35_1763.set_A(35);
  Cl35_1763.set_Z(17);
  Cl35_1763.set_spin(5./2);
  Cl35_1763.set_parity(genbb::nuclear_level::PARITY_PLUS);
  Cl35_1763.set_half_life(0.0);
  Cl35_1763.set_energy(1763.1 * CLHEP::keV);
  Cl35_1763.tree_dump(std::clog, "Cl35 excited state :");

  // The beta+ decay Ar-35 -> Cl-35(1763):
  genbb::nuclear_decay::handle_type hAr35_Cl35_1763(new genbb::nuclear_decay);
  genbb::nuclear_decay & Ar35_Cl35_1763 = hAr35_Cl35_1763.grab();
  Ar35_Cl35_1763.set_decay_id("genbb::beta_decay");
  Ar35_Cl35_1763.set_level_initial(Ar35_0);
  Ar35_Cl35_1763.set_level_final(Cl35_1763);
  Ar35_Cl35_1763.tree_dump(std::clog, "Ar35->Cl35 beta+ decay:");
  Ar35_0.add_decay_channel(Ar35_Cl35_1763, 0.249 * CLHEP::perCent);

  // The third excited state of Cl-35:
  genbb::nuclear_level::handle_type hCl35_2645(new genbb::nuclear_level);
  Cl35_levels.push_back(hCl35_2645);
  genbb::nuclear_level & Cl35_2645 = Cl35_levels.back().grab();
  Cl35_2645.set_A(35);
  Cl35_2645.set_Z(17);
  Cl35_2645.set_spin(3./2);
  Cl35_2645.set_parity(genbb::nuclear_level::PARITY_PLUS);
  Cl35_2645.set_half_life(0.0);
  Cl35_2645.set_energy(2645.2 * CLHEP::keV);
  Cl35_2645.tree_dump(std::clog, "Cl35 excited state :");

  // The beta+ decay Ar-35 -> Cl-35(2645):
  genbb::nuclear_decay::handle_type hAr35_Cl35_2645(new genbb::nuclear_decay);
  genbb::nuclear_decay & Ar35_Cl35_2645 = hAr35_Cl35_2645.grab();
  Ar35_Cl35_2645.set_decay_id("genbb::beta_decay");
  Ar35_Cl35_2645.set_level_initial(Ar35_0);
  Ar35_Cl35_2645.set_level_final(Cl35_2645);
  Ar35_Cl35_2645.tree_dump(std::clog, "Ar35->Cl35 beta+ decay:");
  Ar35_0.add_decay_channel(Ar35_Cl35_2645, 0.015 * CLHEP::perCent);

  // The fourth excited state of Cl-35:
  genbb::nuclear_level::handle_type hCl35_2694(new genbb::nuclear_level);
  Cl35_levels.push_back(hCl35_2694);
  genbb::nuclear_level & Cl35_2694 = Cl35_levels.back().grab();
  Cl35_2694.set_A(35);
  Cl35_2694.set_Z(17);
  Cl35_2694.set_spin(3./2);
  Cl35_2694.set_parity(genbb::nuclear_level::PARITY_PLUS);
  Cl35_2694.set_half_life(0.0);
  Cl35_2694.set_energy(2693.9 * CLHEP::keV);
  Cl35_2694.tree_dump(std::clog, "Cl35 excited state :");

  // The beta+ decay Ar-35 -> Cl-35(2694):
  genbb::nuclear_decay::handle_type hAr35_Cl35_2694(new genbb::nuclear_decay);
  genbb::nuclear_decay & Ar35_Cl35_2694 = hAr35_Cl35_2694.grab();
  Ar35_Cl35_2694.set_decay_id("genbb::beta_decay");
  Ar35_Cl35_2694.set_level_initial(Ar35_0);
  Ar35_Cl35_2694.set_level_final(Cl35_2694);
  Ar35_Cl35_2694.tree_dump(std::clog, "Ar35->Cl35 beta+ decay:");
  Ar35_0.add_decay_channel(Ar35_Cl35_2694, 0.164 * CLHEP::perCent);

  // The fifth excited state of Cl-35:
  genbb::nuclear_level::handle_type hCl35_3002(new genbb::nuclear_level);
  Cl35_levels.push_back(hCl35_3002);
  genbb::nuclear_level & Cl35_3002 = Cl35_levels.back().grab();
  Cl35_3002.set_A(35);
  Cl35_3002.set_Z(17);
  Cl35_3002.set_spin(5./2);
  Cl35_3002.set_parity(genbb::nuclear_level::PARITY_PLUS);
  Cl35_3002.set_half_life(0.0);
  Cl35_3002.set_energy(3002.3 * CLHEP::keV);
  Cl35_3002.tree_dump(std::clog, "Cl35 excited state :");

  // The beta+ decay Ar-35 -> Cl-35(3002):
  genbb::nuclear_decay::handle_type hAr35_Cl35_3002(new genbb::nuclear_decay);
  genbb::nuclear_decay & Ar35_Cl35_3002 = hAr35_Cl35_3002.grab();
  Ar35_Cl35_3002.set_decay_id("genbb::beta_decay");
  Ar35_Cl35_3002.set_level_initial(Ar35_0);
  Ar35_Cl35_3002.set_level_final(Cl35_3002);
  Ar35_Cl35_3002.tree_dump(std::clog, "Ar35->Cl35 beta+ decay:");
  Ar35_0.add_decay_channel(Ar35_Cl35_3002, 0.088 * CLHEP::perCent);

  // The sixth excited state of Cl-35:
  genbb::nuclear_level::handle_type hCl35_3918(new genbb::nuclear_level);
  Cl35_levels.push_back(hCl35_3918);
  genbb::nuclear_level & Cl35_3918 = Cl35_levels.back().grab();
  Cl35_3918.set_A(35);
  Cl35_3918.set_Z(17);
  Cl35_3918.set_spin(3./2);
  Cl35_3918.set_parity(genbb::nuclear_level::PARITY_PLUS);
  Cl35_3918.set_half_life(0.0);
  Cl35_3918.set_energy(3918.3 * CLHEP::keV);
  Cl35_3918.tree_dump(std::clog, "Cl35 excited state :");

  // The seventh excited state of Cl-35:
  genbb::nuclear_level::handle_type hCl35_3967(new genbb::nuclear_level);
  Cl35_levels.push_back(hCl35_3967);
  genbb::nuclear_level & Cl35_3967 = Cl35_levels.back().grab();
  Cl35_3967.set_A(35);
  Cl35_3967.set_Z(17);
  Cl35_3967.set_spin(1./2);
  Cl35_3967.set_parity(genbb::nuclear_level::PARITY_PLUS);
  Cl35_3967.set_half_life(0.0);
  Cl35_3967.set_energy(3967.7 * CLHEP::keV);
  Cl35_3967.tree_dump(std::clog, "Cl35 excited state :");

  // The fundamental level of Ar-35:
  Ar35_0.tree_dump(std::clog, "Ar-35 fundamental level:");

  return;
}

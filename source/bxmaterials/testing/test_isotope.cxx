// test_isotope.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third Party:
// - Bayeux/datatools:
#include <datatools/clhep_units.h>

// This project:
#include <materials/isotope.h>

int main (int /*argc_*/, char ** /*argv_*/)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'materials::isotope'!" << std::endl;

    // My isotope 0 :
    materials::isotope my_isotope_0;
    std::clog << std::endl;
    my_isotope_0.tree_dump( std::clog, "My isotope 0: ");
    my_isotope_0.set_zai(1, 1);
    std::clog << std::endl;
    my_isotope_0.tree_dump( std::clog, "My isotope 0 (bis): ");
    my_isotope_0.set_name("hydrogen-1 (ground state)");
    my_isotope_0.find_mass_data();
    std::clog << std::endl;
    my_isotope_0.tree_dump( std::clog, "My isotope 0 (ter): ");

    // My isotope 1 :
    materials::isotope my_isotope_1(std::string("some hydrogen isotope"));
    std::clog << std::endl;
    my_isotope_1.tree_dump( std::clog, "My isotope 1: ");
    my_isotope_1.set_zai(2, 6, materials::isotope::GROUND_STATE);
    my_isotope_1.set_half_life(806.7 * CLHEP::ms, 0.1 * CLHEP::ms);
    my_isotope_1.set_mass_excess(17595.106 * CLHEP::keV, 0.755 * CLHEP::keV);
    std::clog << std::endl;
    my_isotope_1.tree_dump( std::clog, "My isotope 1: ");

    // My isotope 2 :
    materials::isotope my_isotope_2;
    my_isotope_2.set_zai(2, 6);
    std::clog << std::endl;
    my_isotope_2.tree_dump(std::clog, "My isotope 2: ");
    my_isotope_2.find_mass_data();
    my_isotope_2.find_decay_data();
    std::clog << std::endl;
    my_isotope_2.tree_dump(std::clog, "My isotope 2: ");

    // My isotope 3 :
    try {
      materials::isotope my_isotope_3("Sodium 22 (ground state)",
                                      11, 22,
                                      materials::isotope::GROUND_STATE);
      my_isotope_3.find_mass_data();
      my_isotope_3.find_decay_data();
      std::clog << std::endl;
      my_isotope_3.tree_dump(std::clog, "My isotope 3: ");
    }
    catch (std::exception & x) {
      std::clog << "NOTICE: As expected : " << x.what() << std::endl;
    }


    // My isotope 4 :
    std::clog << std::endl;
    materials::isotope my_isotope_4("Tungsten-184",
                                    "W", 184,
                                    materials::isotope::GROUND_STATE);
    my_isotope_4.grab_properties ().store ("class", "transition metal");
    my_isotope_4.find_mass_data();
    my_isotope_4.find_decay_data();
    my_isotope_4.tree_dump (std::clog, "My isotope 4: ");

    // My isotope 5 :
    std::clog << std::endl;
    materials::isotope my_isotope_5("Iode-120 metastable",
                                    "I", 120,
                                    materials::isotope::ISOMERIC_STATE);
    my_isotope_5.grab_properties ().store("color","solid");
    my_isotope_5.tree_dump (std::clog, "My isotope 5: ");

    // My isotope 6 :
    std::clog << std::endl;
    try {
      materials::isotope my_isotope_6("Unknown Sodium-45",
                                      11, 45,
                                      materials::isotope::GROUND_STATE);
    }
    catch (std::exception & x) {
      std::clog << "As expected, Sodium-45 is not valid !" << std::endl;
    }

  }
  catch (std::exception & x) {
    std::clog << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::clog << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

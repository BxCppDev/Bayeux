// test_material.cxx

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
#include <materials/element.h>
#include <materials/material.h>

int main (int /* argc */, char ** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'materials::material'!" << std::endl;

    // Definition of some isotopes :
    materials::isotope iso_H1("H1", "H", 1,
                              materials::isotope::GROUND_STATE,
                              materials::isotope::BF_LOCK);

    materials::isotope iso_H2("H2", "H", 2,
                              materials::isotope::GROUND_STATE,
                              materials::isotope::BF_LOCK);

    materials::isotope iso_O16("O16", "O",  16,
                               materials::isotope::GROUND_STATE,
                               materials::isotope::BF_LOCK);

    materials::isotope iso_O17("O17", "O",  17,
                               materials::isotope::GROUND_STATE,
                               materials::isotope::BF_LOCK);

    materials::isotope iso_O18("O18", "O",  18,
                               materials::isotope::GROUND_STATE,
                               materials::isotope::BF_LOCK);

    materials::isotope iso_B10("B10", "B",  10,
                               materials::isotope::GROUND_STATE,
                               materials::isotope::BF_LOCK);

    materials::isotope iso_B11("B11", "B",  11,
                               materials::isotope::GROUND_STATE,
                               materials::isotope::BF_LOCK);

    // Definition of some elements :
    materials::element elt_H("Hydrogen", 1);
    // Add locked isotopes by abundance:
    elt_H.add_isotope(iso_H1, 99.985);
    elt_H.add_isotope(iso_H2, 0.015);
    elt_H.build(materials::element::BF_LOCK);
    elt_H.tree_dump(std::clog, "my element Hydrogen");

    materials::element elt_O("Oxygen", 8);
    // Add locked isotopes by abundances:
    elt_O.add_isotope(iso_O16, 99.76);
    elt_O.add_isotope(iso_O17, 0.039);
    elt_O.add_isotope(iso_O18, 0.201);
    elt_O.build(materials::element::BF_LOCK);
    elt_O.tree_dump(std::clog, "my element Oxygen");

    materials::element elt_B("Bore", 5);
    // Add locked isotopes by abundances:
    elt_B.add_isotope(iso_B10, 19.9);
    elt_B.add_isotope(iso_B11, 80.1);
    elt_B.build(materials::element::BF_LOCK);
    elt_B.tree_dump(std::clog, "my element Bore");

    // Definition of Materials :
    materials::material mat_H2O;
    mat_H2O.set_name("water (H2O)");
    // Add locked elements by number of atoms:
    mat_H2O.add_element_by_nb_of_atoms(elt_H, 2);
    mat_H2O.add_element_by_nb_of_atoms(elt_O, 1);
    mat_H2O.set_state(materials::material::STATE_LIQUID);
    mat_H2O.set_density(1.0 * materials::material::g_per_cm3());
    mat_H2O.set_temperature(300.0 * CLHEP::kelvin);
    // Lock it !
    mat_H2O.build(materials::element::BF_LOCK);
    mat_H2O.tree_dump(std::clog, "First material (H2O) : ");

    materials::material mat_borated_water;
    double boron_mass_concentration = 1e-2;
    mat_borated_water.set_name("borated water");
    // Add locked compounds by fraction mass:
    mat_borated_water.add_element_by_mass(elt_B, boron_mass_concentration);
    mat_borated_water.add_material_by_mass(mat_H2O, 1. - boron_mass_concentration);
    mat_borated_water.set_state(materials::material::STATE_LIQUID);
    mat_borated_water.set_density(1.16 * materials::material::g_per_cm3());
    mat_borated_water.build(materials::material::BF_LOCK);
    mat_borated_water.tree_dump(std::clog, "2nd material Borated water : ");

    materials::material mat_diamond;
    mat_diamond.set_name("Diamond");
    // Defined by mean Z and A:
    mat_diamond.set_state(materials::material::STATE_SOLID);
    mat_diamond.set_mean_z_a(6.0, 12.0);
    mat_diamond.set_density(3.517 * materials::material::g_per_cm3());
    mat_diamond.set_pressure(1.0 * CLHEP::bar);
    mat_diamond.build(materials::material::BF_LOCK);
    mat_diamond.tree_dump(std::clog, "3rd material Diamond : ");

  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

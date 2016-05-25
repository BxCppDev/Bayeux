// test_element.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// This project
#include <materials/isotope.h>
#include <materials/element.h>

using namespace std;

int main (int /*argc_*/, char ** /*argv_*/)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'element'!" << std::endl;

    std::clog << "MATERIALS_RESOURCE_DIR='" << getenv("MATERIALS_RESOURCE_DIR") << "'" << std::endl;

    materials::isotope iso_H1(std::string("H"), 1,
                              materials::isotope::GROUND_STATE,
                              materials::isotope::BF_MASS_DATA);

    materials::isotope iso_H2(std::string("Hydrogen 2"),
                              std::string("H"), 2,
                              materials::isotope::GROUND_STATE);

    materials::isotope iso_H3("Hydrogen 3", "H", 3,
                              materials::isotope::GROUND_STATE);

    materials::isotope iso_H4("Hydrogen 4", "H", 4);

    materials::isotope iso_H5(std::string("Hydrogen 5"), 1, 5);

    materials::isotope iso_C12("Carbon 12", "C", 12,
                               materials::isotope::GROUND_STATE);
    iso_H1.lock();
    iso_H2.build(materials::isotope::BF_MASS_DATA);
    iso_H2.lock();
    iso_H3.build(materials::isotope::BF_MASS_DATA | materials::isotope::BF_LOCK);
    iso_H4.build(materials::isotope::BF_MASS_DATA | materials::isotope::BF_LOCK);
    // iso_H5.lock(); // Not locked for test

    iso_H1.tree_dump(std::clog, "Isotope Hydrogen 1");
    iso_H2.tree_dump(std::clog, "Isotope Hydrogen 2");
    iso_H3.tree_dump(std::clog, "Isotope Hydrogen 3");
    iso_H4.tree_dump(std::clog, "Isotope Hydrogen 4");
    iso_H5.tree_dump(std::clog, "Isotope Hydrogen 5");
    iso_C12.tree_dump(std::clog, "Isotope Carbon 12");

    materials::element elt_H(std::string("Hydrogen"),1);
    //elt_H.add_isotope(iso_C12, 0.1);
    elt_H.add_isotope(iso_H1, 0.2);
    elt_H.add_isotope(iso_H2, 0.5);
    elt_H.add_isotope(iso_H3, 0.1);
    //elt_H.add_isotope(iso_C12, 0.1);
    //elt_H.add_isotope(iso_H5, 0.1);
    elt_H.build();
    elt_H.tree_dump(std::clog, "my element Hydrogen");

    materials::isotope iso_Na = materials::isotope(std::string("Sodium"),
                                                   11, 23,
                                                   materials::isotope::GROUND_STATE,
                                                   materials::isotope::BF_LOCK);
    materials::element elt_Na(std::string("Sodium"), std::string("Na"));
    elt_Na.add_isotope(iso_Na);
    elt_Na.build();
    elt_Na.tree_dump(std::clog, "My element Sodium");

  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

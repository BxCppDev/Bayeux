// -*- mode: c++ ; -*-
// test_gdml_writer.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/utils.h>
#include <geomtools/gdml_writer.h>

using namespace std;

int main (int /* argc_ */, char ** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'gdml_writer'!" << endl;

      // bool debug = false;
      // int iarg = 1;
      // while (iarg < argc_)
      //   {
      //     string token = argv_[iarg];
      //     if (token[0] == '-')
      //       {
      //          string option = token;
      //          if ((option == "-d") || (option == "--debug"))
      //            {
      //              debug = true;
      //            }
      //          else
      //            {
      //               clog << "warning: ignoring option '" << option << "'!" << endl;
      //            }
      //       }
      //     else
      //       {
      //         string argument = token;
      //         {
      //           clog << "warning: ignoring argument '" << argument << "'!" << endl;
      //         }
      //       }
      //     iarg++;
      // }

      geomtools::gdml_writer writer;

      /*** Defines ***/

      writer.add_constant ("pi", M_PI);
      writer.add_constant ("e", exp (1.0));
      writer.add_quantity ("m_e",
                           "mass",
                           "MeV/c2",
                           0.511 * CLHEP::MeV / CLHEP::c_squared);
      writer.add_variable ("x", 3.0);
      writer.add_variable ("y", 4.0);
      writer.add_variable ("h", "sqrt(x*y)");

      writer.add_position ("pos1",
                           34. * CLHEP::mm,
                           -5. * CLHEP::mm,
                           12. * CLHEP::mm,
                           "mm");

      geomtools::vector_3d pos (1. * CLHEP::m,
                                -0.5 * CLHEP::m,
                                45 * CLHEP::cm);

      writer.add_position ("pos2", pos, "mm");
      writer.add_rotation ("rot1", "x", CLHEP::pi / 3 * CLHEP::radian, "degree");

      /*** Materials ***/

      writer.add_isotope ("U235", 92, 235, 235.01);
      writer.add_isotope ("U238", 92, 238, 235.03);
      writer.add_isotope ("U241", 92, 241, 238.14);

      writer.add_element ("Hydrogen", 1, "H", 1);
      writer.add_element ("Oxygen", 8, "O", 16);
      writer.add_element ("Carbon", 6, "C", 12);
      writer.add_element ("Nitrogen", 7, "N", 14);
      writer.add_element ("Lead", 82, "Pb", 1);
      writer.add_element ("enriched_uranium",
                          "U235", 0.9,
                          "U238", 0.1);
      try
        {
          writer.add_element ("enriched_uranium2",
                              "U235", 0.9,
                              "U238", 0.1,
                              "U241", 0.2);
        }
      catch (exception & x)
        {
          clog << "warning: " << x.what () << endl;
        }

      writer.add_material ("Al",
                           13.0,
                           2.70 * CLHEP::g / CLHEP::cm3,
                           26.98);

      map<string, double> Air_map;
      Air_map["Oxygen"] = 0.3;
      Air_map["Nitrogen"] = 0.7;
      writer.add_material ("Air",
                           "air",
                           1.29 * CLHEP::mg / CLHEP::cm3,
                           Air_map);

      map<string, size_t> H2O_map;
      H2O_map["Oxygen"] = 1;
      H2O_map["Hydrogen"] = 2;
      writer.add_material ("Water",
                           "H2O",
                           1.0 * CLHEP::g / CLHEP::cm3,
                           H2O_map);


      /*** Solids ***/

      /*
      datatools::utils::properties box_params;
      box_params.store ("length.x", 30. * CLHEP::cm);
      box_params.store ("length.y", 10. * CLHEP::cm);
      box_params.store ("length.z", 5. * CLHEP::cm);
      */

      geomtools::box b1 (30. * CLHEP::cm, 10. * CLHEP::cm, 5. * CLHEP::cm);
      writer.add_box ("box1", b1, "mm");

      geomtools::box b2 (100. * CLHEP::cm, 5. * CLHEP::cm, 5. * CLHEP::cm);
      writer.add_box ("box2", b2, "mm");

      geomtools::cylinder c1 (15. * CLHEP::cm, 50. * CLHEP::cm);
      writer.add_cylinder ("cyl1", c1, "mm");

      geomtools::tube t1 (11. * CLHEP::cm, 15. * CLHEP::cm, 25. * CLHEP::cm);
      writer.add_tube ("tub1", t1, "mm", "degree");

      writer.add_position ("u1_pos",
                           30. * CLHEP::cm,
                           10. * CLHEP::cm,
                           -20. * CLHEP::cm, "mm");
      writer.add_rotation ("u1_rot", "x", CLHEP::pi / 4 * CLHEP::radian, "degree");

      writer.add_gdml_union ("u1","box1", "tub1", "u1_pos", "u1_rot");
      writer.add_gdml_subtraction ("s1", "box1", "tub1", "u1_pos", "u1_rot");
      writer.add_gdml_intersection ("i1", "box1", "tub1", "u1_pos", "u1_rot");

      /*** Structures ***/

      writer.add_volume ("InVessel",
                         "Air",
                         "box2");

      list<geomtools::gdml_writer::physvol> physvols;
      physvols.push_back (geomtools::gdml_writer::physvol ("vol1", "pos1", "rot1"));
      physvols.push_back (geomtools::gdml_writer::physvol ("vol2", "pos2", "rot2"));
      physvols.push_back (geomtools::gdml_writer::physvol ("vol3", "pos3", "rot3"));
      writer.add_volume ("Vessel2",
                         "Air",
                         "box3",
                         physvols);

      geomtools::gdml_writer::replicavol RV;
      RV.volumeref = "vol2";
      RV.number = 3;
      RV.mode = geomtools::gdml_writer::replicavol::replicated_along_axis();
      RV.direction = "x";
      RV.width = 100.0 * CLHEP::mm;
      RV.offset = 0.0;
      writer.add_replica_volume ("Vessel3",
                                 "Air",
                                 "box2",
                                 RV,
                                 "mm");



      /*** Setups ***/

      writer.add_setup ("Setup",
                        "world");

      writer.dump (clog);

      writer.save_file ("test_gdml_writer.gdml");

    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error!" << endl;
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

// end of test_gdml_writer.cxx

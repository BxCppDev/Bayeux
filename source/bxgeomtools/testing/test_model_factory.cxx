// test_model_factory.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>

// This project:
#include <geomtools/geomtools_config.h>
#include <geomtools/model_factory.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
#include <geomtools/gdml_export.h>
#include <geomtools/placement.h>

// Additional registered test geometry models :
#include <geomtools_test_model_1.cc>
#include <geomtools_test_model_2.cc>
#include <geomtools_test_world_model.cc>

int main (int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'model_factory'!" << endl;

      bool debug = false;
      // debug = true;
      bool devel = false;
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      string drawer_view = geomtools::gnuplot_drawer::view_3d();
      bool draw = false;
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
      bool gdml = true;
      bool dump = true;
      string setup_filename;
      string model_name;
      bool interactive = false;

      int iarg = 1;
      while (iarg < argc_)
        {
          string token = argv_[iarg];

          if (token[0] == '-')
            {
               string option = token;
               if ((option == "-d") || (option == "--debug"))
                 {
                   debug = true;
                 }
               else if (option == "--interactive")
                 {
                   interactive = true;
                 }
               else if (option == "--devel")
                 {
                   devel = true;
                 }
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
               else if (option == "-D" || option == "--no-draw")
                 {
                   draw = false;
                 }
               else if ( option == "--draw")
                 {
                   draw = true;
                 }
               else if (option == "-G")
                 {
                   gdml = false;
                 }
               else if (option == "-T")
                 {
                   dump = false;
                 }
               else if (option == "-xy")
                 {
                   drawer_view = geomtools::gnuplot_drawer::view_2d_xy();
                 }
               else if (option == "-xz")
                 {
                   drawer_view = geomtools::gnuplot_drawer::view_2d_xz();
                 }
               else if (option == "-yz")
                 {
                   drawer_view = geomtools::gnuplot_drawer::view_2d_yz();
                 }
               else if (option == "-3d")
                 {
                   drawer_view = geomtools::gnuplot_drawer::view_3d();
                 }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
               else if (option == "-m")
                 {
                   model_name = argv_[++iarg];
                 }
               else
                 {
                    clog << "warning: ignoring option '" << option << "'!" << endl;
                 }
            }
          else
            {
              string argument = token;
              if (setup_filename.empty ())
                {
                  setup_filename = argument;
                }
              else
                {
                  clog << "warning: ignoring argument '" << argument << "'!" << endl;
                }
            }
          iarg++;
      }

      geomtools::model_factory factory;
      factory.set_debug (debug);
      factory.add_property_prefix ("visibility.");
      factory.add_property_prefix ("mapping.");
      factory.add_property_prefix ("material.");
      factory.add_property_prefix ("sensitive.");

      if (setup_filename.empty ())
        {
          setup_filename = "${GEOMTOOLS_TESTING_DIR}/config/test_geometry_models.conf";
        }
      datatools::fetch_path_with_env (setup_filename);
      factory.load (setup_filename);
      factory.lock ();
      if (dump) factory.tree_dump (clog, "Geometry model factory:");

      geomtools::placement p;
      p.set (0, 0, 0, 0 * CLHEP::degree, 0 * CLHEP::degree, 0);
      if (dump) p.tree_dump (clog, "Placement");

      clog << "Models: " << endl;
      int count = 0;
      for (geomtools::models_col_type::const_iterator i
             = factory.get_models ().begin ();
           i != factory.get_models ().end ();
           i++)
        {
          clog << "  " << i->second->get_name ();
          count++;
          if ((count % 5) == 0)  clog << endl;
        }
      clog << endl;

      if (model_name.empty ())
        {
          if (interactive) {
            clog << "Enter the name of the geometry model to be displayed: ";
            getline (cin, model_name);
          } else {
            model_name = "world";
          }
        }
      if (model_name.empty ())
        {
          model_name = "world";
        }
      clog << "Name of the model : " << model_name << endl;

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      if (draw)
        {
          geomtools::gnuplot_drawer GPD;
          GPD.set_view(drawer_view);
          GPD.set_mode(geomtools::gnuplot_drawer::mode_wired());
          GPD.draw_model(factory,
                         model_name,
                         p,
                         geomtools::gnuplot_drawer::display_level_no_limit());
        }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

      ostringstream ext_mat_oss;
      geomtools::gdml_writer writer;
      {
        writer.add_element ("Hydrogen", 1, "H", 1);
        writer.add_element ("Oxygen", 8, "O", 16);
        writer.add_element ("Carbon", 6, "C", 12);
        writer.add_element ("Nitrogen", 7, "N", 14);
        writer.add_element ("Lead", 82, "Pb", 1);

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
      }

      if (gdml)
      {
        geomtools::gdml_export GDML;
        if (devel) GDML.set_logging_priority (datatools::logger::PRIO_TRACE);
        GDML.add_replica_support (true);
        GDML.attach_external_materials (writer.get_stream (geomtools::gdml_writer::materials_section()));
        GDML.parameters ().store ("xml_version",
                                  geomtools::gdml_writer::default_xml_version());
        GDML.parameters ().store ("xml_encoding",
                                  geomtools::gdml_writer::default_xml_encoding());
        GDML.parameters ().store ("gdml_schema",
                                  geomtools::gdml_writer::default_gdml_schema());
        GDML.parameters ().store ("length_unit",
                                  geomtools::gdml_export::default_length_unit());
        GDML.parameters ().store ("angle_unit",
                                  geomtools::gdml_export::default_angle_unit());
        GDML.export_gdml ("test_model_factory.gdml", factory, model_name);
      }
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

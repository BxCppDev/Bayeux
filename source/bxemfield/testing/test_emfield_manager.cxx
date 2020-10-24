// test_emfield_manager.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

// Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/ioutils.h>
#include <datatools/service_manager.h>
// Bayeux/geomtools:
#include <geomtools/gnuplot_i.h>

// This project:
#include <emfield/emfield_config.h>
#include <emfield/electromagnetic_field_manager.h>
#include <emfield/oscillating_field.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for the 'emfield::electromagnetic_field_manager' class." << endl;

      bool   debug   = false;
      bool   plot    = false;
      bool   plot2   = false;
      double e_scale = 1.0;
      double b_scale = 1.0;

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
              else if ((option == "-p") || (option == "--plot"))
                {
                  plot = true;
                }
              else if ((option == "-p2") || (option == "--plot2"))
                {
                  plot2 = true;
                }
              else if ((option == "-E:10"))
                {
                  e_scale /= 10;
                }
              else if ((option == "-Ex10"))
                {
                  e_scale *= 10;
                }
              else if ((option == "-B:10"))
                {
                  b_scale /= 10;
                }
              else if ((option == "-Bx10"))
                {
                  b_scale *= 10;
                }
              else
                {
                  clog << "warning: ignoring option '" << option << "' !"
                       << endl;
                }
            }
          else
            {
              string argument = token;
              {
                clog << "warning: ignoring argument '" << argument << "' !"
                     << endl;
              }
            }
          iarg++;
        }

      // This is the service manager :
      datatools::service_manager SRVCmgr;
      datatools::properties SRVCmgrConfig;
      std::string SRVCmgrConfigFile = "${EMFIELD_TESTING_DIR}/config/test_service_manager.conf";
      if (debug) clog << datatools::io::debug << "Setup file = "
                      << SRVCmgrConfigFile << endl;
      datatools::fetch_path_with_env(SRVCmgrConfigFile);
      datatools::read_config(SRVCmgrConfigFile, SRVCmgrConfig);
      SRVCmgrConfig.tree_dump(std::cerr, "The service manager configuration : ");
      if (debug) clog << datatools::io::debug << "Initializing the service manager..." << endl;
      SRVCmgr.initialize(SRVCmgrConfig); // Initialize
      SRVCmgr.tree_dump(std::cerr, "The service manager : ");

      // This is the EM fields manager :
      emfield::electromagnetic_field_manager EMFmgr;
      if (debug) EMFmgr.set_logging_priority(datatools::logger::PRIO_DEBUG);
      EMFmgr.set_service_manager (SRVCmgr); // Attach the service manager
      std::string EMFmgrConfigFile = "${EMFIELD_TESTING_DIR}/config/test_manager.conf";
      datatools::properties EMFmgrConfig;
      datatools::fetch_path_with_env(EMFmgrConfigFile);
      datatools::read_config(EMFmgrConfigFile, EMFmgrConfig);
      EMFmgr.initialize (EMFmgrConfig); // Initialize
      EMFmgr.tree_dump (std::clog, "EM field manager: ");

      for (emfield::base_electromagnetic_field::field_dict_type::const_iterator i = EMFmgr.get_fields().begin ();
           i != EMFmgr.get_fields().end ();
           i++)
        {
          std::string field_name = i->first;
          std::clog << "\n" << "Field '" << field_name << "' : " << std::endl;
          if (! EMFmgr.has_field (field_name))
            {
              std::ostringstream message;
              message << "No field named '" << field_name << "' !";
              throw std::logic_error (message.str ());
            }
          const emfield::base_electromagnetic_field & the_field = EMFmgr.get_field (field_name);

          geomtools::vector_3d position (2.0 * CLHEP::m, -1.0 * CLHEP::m, 0.5 * CLHEP::m);
          double time = 1.3 * CLHEP::second;

          if (the_field.is_electric_field ())
            {
              geomtools::vector_3d e_field;
              int status = the_field.compute_electric_field (position, time, e_field);
              if (status != 0)
                {
                  std::ostringstream message;
                  message << "Field named '" << field_name << "' cannot compute electric field value at "
                          << position / CLHEP::m << " [m] / " << time / CLHEP::second << " [s] !";
                  throw std::logic_error (message.str ());
                }
              else
                {
                  clog << "  Electric field at : " << position / CLHEP::m << " [m] / "
                       << time / CLHEP::second << " [s] is : "
                       << e_field / (CLHEP::volt / CLHEP::m) << " V/m" << std::endl;
                }
            }
          else
            {
              clog << "  Not an electric field." << std::endl;
            }

          if (the_field.is_magnetic_field ())
            {
              geomtools::vector_3d b_field;
              int status = the_field.compute_magnetic_field (position, time, b_field);
              if (status != 0)
                {
                  std::ostringstream message;
                  message << "Field named '" << field_name << "' cannot compute magnetic field value at "
                          << position / CLHEP::m << " [m] / " << time / CLHEP::second << " [s] !";
                  throw std::logic_error (message.str ());
                }
              else
                {
                  clog << "  Magnetic field at : " << position / CLHEP::m << " [m] / "
                       << time / CLHEP::second << " [s] is : "
                       << b_field / (CLHEP::gauss) << " gauss" << std::endl;
                }
            }
          else
            {
              clog << "  Not a magnetic field." << std::endl;
            }
        }

      if (plot)
        {
          bool plot1 = true;
          if (plot1)
            {
              std::string datafile = "field1.data";
              std::ofstream fout (datafile.c_str ());
              std::string field_name = "E0+2*E1-3*B0";
              if (! EMFmgr.has_field (field_name))
                {
                  std::ostringstream message;
                  message << "No field named '" << field_name << "' !";
                  throw std::logic_error (message.str ());
                }
              const emfield::base_electromagnetic_field & the_field = EMFmgr.get_field (field_name);
              double dx = 0.4 * CLHEP::m;
              double dy = 0.4 * CLHEP::m;
              double dz = 0.4 * CLHEP::m;
              fout << "# x y z Ex Ey Ez Bx By Bz" << std::endl;
              fout.precision (15);
              for (double x = -1.0 * CLHEP::m; x <= +1.0 * CLHEP::m; x += dx)
                {
                  for (double y = -1.0 * CLHEP::m; y <= +1.0 * CLHEP::m; y += dy)
                    {
                      for (double z = -1.0 * CLHEP::m; z <= +1.0 * CLHEP::m; z += dz)
                        {
                          fout << x << ' ' << y << ' ' << z << ' ';
                          geomtools::vector_3d position (x, y, z);
                          geomtools::vector_3d b_field;
                          geomtools::vector_3d e_field;
                          double time = 0.0;
                          the_field.compute_magnetic_field (position, time, b_field);
                          the_field.compute_electric_field (position, time, e_field);
                          b_field /= CLHEP::gauss;
                          b_field *= b_scale;
                          e_field /= (CLHEP::volt / CLHEP::m);
                          e_field *= e_scale;
                          fout << e_field.x () << ' ' << e_field.y ()  << ' ' << e_field.z () << ' ';
                          fout << b_field.x () << ' ' << b_field.y ()  << ' ' << b_field.z () << ' ';
                          fout << std::endl;
                        }
                    }
                }
              Gnuplot g1;
              g1.set_title("Electromagnetic field");
              g1.set_grid();
              g1.set_xrange (-1.25 * CLHEP::m, +1.25 * CLHEP::m);
              g1.set_yrange (-1.25 * CLHEP::m, +1.25 * CLHEP::m);
              g1.set_zrange (-1.25 * CLHEP::m, +1.25 * CLHEP::m);
              g1.cmd ("set size ratio -1");
              g1.cmd ("set view equal xyz");
              {
                std::ostringstream cmd_oss;
                cmd_oss << "set xyplane at " << -1 * CLHEP::m;
                g1.cmd (cmd_oss.str ());
              }
              g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
              g1.cmd ("set xtics");
              g1.cmd ("set ytics");
              g1.cmd ("set ztics");

              {
                std::ostringstream cmd_oss;
                cmd_oss << "splot ";
                cmd_oss << " '" << datafile << "'" << " using 1:2:3:4:5:6 title \"E\" with vectors lt 3 ";
                cmd_oss << ", '" << datafile << "'" << " using 1:2:3:7:8:9 title \"B\" with vectors lt 1 ";
                g1.cmd (cmd_oss.str ());
              }

              g1.showonscreen ();
              {
                std::clog << std::endl << "Press [Enter] to continue..." << std::endl;
                std::string s;
                std::getline (std::cin, s);
                usleep (200);
              }

            }
         if (plot2)
            {
              std::string datafile = "field2.data";
              std::ofstream fout (datafile.c_str ());
              std::string field_name = "~E0+~E2"; //"~E1";
              if (! EMFmgr.has_field (field_name))
                {
                  std::ostringstream message;
                  message << "No field named '" << field_name << "' !";
                  throw std::logic_error (message.str ());
                }
              emfield::base_electromagnetic_field & the_field = EMFmgr.grab_field (field_name);
              double dx = 0.4 * CLHEP::m;
              double dy = 0.4 * CLHEP::m;
              double dz = 0.4 * CLHEP::m;
              fout << "# x y z Ex Ey Ez Bx By Bz" << std::endl;
              fout.precision (15);
              double delta_time = 0.01 * CLHEP::millisecond;
              double max_time = 1 * CLHEP::millisecond;
              // Time loop :
              int frame_counter = 0;
              for (double time = 0.0 * CLHEP::second;
                   time < max_time;
                   time += delta_time)
                {
                  for (double x = -1.0 * CLHEP::m; x <= +1.0 * CLHEP::m; x += dx)
                    {
                      for (double y = -1.0 * CLHEP::m; y <= +1.0 * CLHEP::m; y += dy)
                        {
                          for (double z = -1.0 * CLHEP::m; z <= +1.0 * CLHEP::m; z += dz)
                            {
                              fout << x << ' ' << y << ' ' << z << ' ';
                              geomtools::vector_3d position (x, y, z);
                              geomtools::vector_3d b_field;
                              geomtools::vector_3d e_field;
                              the_field.compute_magnetic_field (position, time, b_field);
                              the_field.compute_electric_field (position, time, e_field);
                              b_field /= CLHEP::gauss;
                              b_field *= b_scale;
                              e_field /= (CLHEP::volt / CLHEP::m);
                              e_field *= e_scale;
                              fout << e_field.x () << ' ' << e_field.y ()  << ' ' << e_field.z () << ' ';
                              fout << b_field.x () << ' ' << b_field.y ()  << ' ' << b_field.z () << ' ';
                              fout << std::endl;
                            }
                        }
                    }
                  fout << std::endl << std::endl;
                  frame_counter++;
                }
              Gnuplot g1;
              g1.set_title("Electromagnetic field");
              g1.set_grid();
              g1.set_xrange (-1.25 * CLHEP::m, +1.25 * CLHEP::m);
              g1.set_yrange (-1.25 * CLHEP::m, +1.25 * CLHEP::m);
              g1.set_zrange (-1.25 * CLHEP::m, +1.25 * CLHEP::m);
              g1.cmd ("set size ratio -1");
              g1.cmd ("set view equal xyz");
              {
                std::ostringstream cmd_oss;
                cmd_oss << "set xyplane at " << -1 * CLHEP::m;
                g1.cmd (cmd_oss.str ());
              }
              g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
              g1.cmd ("set xtics");
              g1.cmd ("set ytics");
              g1.cmd ("set ztics");

              for (int iframe = 0; iframe < frame_counter; iframe++)
                {
                  double time = iframe * delta_time;
                  std::ostringstream cmd_oss;
                  cmd_oss << "splot ";
                  cmd_oss << " '" << datafile << "'" << " index " << iframe
                          << " using 1:2:3:4:5:6 title \"E(t=" << time / CLHEP::millisecond << " ms)\" with vectors lt 3 ";
                  cmd_oss << ", '" << datafile << "'" << " index " << iframe
                          << " using 1:2:3:7:8:9 title \"B(t=" << time / CLHEP::millisecond << " ms)\" with vectors lt 1 ";
                  g1.cmd (cmd_oss.str ());
                  g1.showonscreen ();
                  usleep (50000);
                }

              {
                std::clog << std::endl << "Press [Enter] to continue..." << std::endl;
                std::string s;
                std::getline (std::cin, s);
                usleep (1000);
              }

            }
        }

      clog << "The end." << endl;
    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error !" << endl;
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

// end of test_emfield_manager.cxx

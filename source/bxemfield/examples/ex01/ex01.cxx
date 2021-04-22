// ex01.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/ioutils.h>
// - Bayeux/geomtools:
#include <geomtools/gnuplot_i.h>
// - Bayeux/emfield:
#include <emfield/emfield_config.h>
#include <emfield/electromagnetic_field_manager.h>
#include <emfield/oscillating_field.h>

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Example program ex01 from the 'emfield' library." << std::endl;

    bool   debug   = false;
    bool   plot    = false;
    bool   plot2   = false;
    double e_scale = 1.0;
    double b_scale = 1.0;

    int iarg = 1;
    while(iarg < argc_) {
      std::string token = argv_[iarg];

      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") ||(option == "--debug")) {
          debug = true;
        } else if ((option == "-p") ||(option == "--plot")) {
          plot = true;
        } else if ((option == "-p2") ||(option == "--plot2")) {
          plot2 = true;
        } else if ((option == "-E:10")) {
          e_scale /= 10;
        } else if ((option == "-Ex10")) {
          e_scale *= 10;
        } else if ((option == "-Ex5")) {
          e_scale *= 5;
        } else if ((option == "-Ex2")) {
          e_scale *= 2;
        } else if ((option == "-B:10")) {
          b_scale /= 10;
        } else if ((option == "-Bx10")) {
          b_scale *= 10;
        } else if ((option == "-Bx5")) {
          b_scale *= 5;
        } else if ((option == "-Bx2")) {
          b_scale *= 2;
        } else {
          std::clog << "warning: ignoring option '" << option << "' !"
                    << std::endl;
        }
      }
      iarg++;
    }

    // This is the EM fields manager :
    emfield::electromagnetic_field_manager EMFmgr;
    EMFmgr.set_logging_priority(datatools::logger::PRIO_DEBUG);
    std::string EMFmgrConfigFile = "${CONFIG_DIR}/fields/manager.conf";
    datatools::properties EMFmgrConfig;
    datatools::fetch_path_with_env(EMFmgrConfigFile);
    datatools::read_config(EMFmgrConfigFile, EMFmgrConfig);
    EMFmgr.initialize(EMFmgrConfig); // Initialize
    EMFmgr.tree_dump(std::clog, "EM field manager: ");

    std::clog << "\n" << "Number of fields = " << EMFmgr.get_fields().size() << std::endl;

    for (emfield::base_electromagnetic_field::field_dict_type::const_iterator ifield
           = EMFmgr.get_fields().begin();
         ifield != EMFmgr.get_fields().end();
         ifield++) {
      std::string field_name = ifield->first;
      std::clog << "\n" << "Field '" << field_name << "' : " << std::endl;
      if (! EMFmgr.has_field(field_name)) {
        std::ostringstream message;
        message << "No field named '" << field_name << "' !";
        throw std::logic_error(message.str());
      }
      const emfield::base_electromagnetic_field & the_field = EMFmgr.get_field(field_name);
      the_field.tree_dump(std::clog, "");

      // Check at a specific space/time value:
      geomtools::vector_3d position(2.0 * CLHEP::m, -1.0 * CLHEP::m, 0.5 * CLHEP::m);
      double time = 1.3 * CLHEP::second;

      if (the_field.is_electric_field()) {
        geomtools::vector_3d e_field;
        int status = the_field.compute_electric_field(position, time, e_field);
        if (status != 0) {
          DT_THROW_IF (status != 0, std::logic_error,
                       "Field named '" << field_name << "' cannot compute electric field value at "
                       << position / CLHEP::m << " [m] / " << time / CLHEP::second << " [s] !");
        } else {
          std::clog << "  Electric field at : " << position / CLHEP::m << " [m] / "
                    << time / CLHEP::second << " [s] is : "
                    << e_field /(CLHEP::volt / CLHEP::m) << " V/m" << std::endl;
        }
      } else {
        std::clog << "  Not an electric field." << std::endl;
      }

      if (the_field.is_magnetic_field()) {
        geomtools::vector_3d b_field;
        int status = the_field.compute_magnetic_field(position, time, b_field);
        if (status != 0) {
          DT_THROW_IF (status != 0, std::logic_error,
                       "Field named '" << field_name << "' cannot compute magnetic field value at "
                       << position / CLHEP::m << " [m] / " << time / CLHEP::second << " [s] !");
        } else {
          std::clog << "  Magnetic field at : " << position / CLHEP::m << " [m] / "
                    << time / CLHEP::second << " [s] is : "
                    << b_field /(CLHEP::gauss) << " gauss" << std::endl;
        }
      }
      else {
        std::clog << "  Not a magnetic field." << std::endl;
      }

    }

    if (plot) {
      bool plot1 = true;
      if (plot1) {
        std::string datafile = "field1.data";
        std::ofstream fout(datafile.c_str());
        std::string field_name = "B_multi_zone";
        if (! EMFmgr.has_field(field_name)) {
          std::ostringstream message;
          message << "No field named '" << field_name << "' !";
          throw std::logic_error(message.str());
        }
        const emfield::base_electromagnetic_field & the_field = EMFmgr.get_field(field_name);
        double spread = 1.75 * CLHEP::m;
        double step = 0.125 * CLHEP::m;
        double dx = step;
        double dy = step;
        double dz = step;
        fout << "# x y z Ex Ey Ez Bx By Bz" << std::endl;
        fout.precision(15);
        for (double x = -spread; x <= +spread; x += dx) {
          for (double y = -spread; y <= +spread; y += dy) {
            for (double z = -spread; z <= +spread; z += dz) {
              fout << x << ' ' << y << ' ' << z << ' ';
              geomtools::vector_3d position(x, y, z);
              geomtools::vector_3d b_field;
              geomtools::vector_3d e_field;
              double time = 0.0;
              the_field.compute_magnetic_field(position, time, b_field);
              the_field.compute_electric_field(position, time, e_field);
              b_field /= CLHEP::gauss;
              b_field *= b_scale;
              e_field /=(CLHEP::volt / CLHEP::m);
              e_field *= e_scale;
              if (e_field.mag() < 1.e-15) {
                fout << "NaN NaN NaN";
              } else {
                fout << e_field.x() << ' ' << e_field.y()  << ' ' << e_field.z() << ' ';
              }
              if (b_field.mag() < 1.e-15) {
                fout << "NaN NaN NaN";
              } else {
                fout << b_field.x() << ' ' << b_field.y()  << ' ' << b_field.z() << ' ';
              }
              fout << std::endl;
            }
          }
        }
        Gnuplot g1;
        g1.set_title(std::string("Electromagnetic field") + " '" + field_name + "'");
        g1.set_grid();
        g1.set_xrange(-1.1 * spread, +1.1 * spread);
        g1.set_yrange(-1.1 * spread, +1.1 * spread);
        g1.set_zrange(-1.1 * spread, +1.1 * spread);
        g1.cmd("set size ratio -1");
        g1.cmd("set view equal xyz");
        {
          std::ostringstream cmd_oss;
          cmd_oss << "set xyplane at " << -1.1 * spread;
          g1.cmd(cmd_oss.str());
        }
        g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");
        g1.cmd("set xtics");
        g1.cmd("set ytics");
        g1.cmd("set ztics");

        {
          std::ostringstream cmd_oss;
          cmd_oss << "splot ";
          cmd_oss << " '" << datafile << "'" << " using 1:2:3:4:5:6 title \"E\" with vectors lt 3 ";
          cmd_oss << ", '" << datafile << "'" << " using 1:2:3:7:8:9 title \"B\" with vectors lt 1 ";
          g1.cmd(cmd_oss.str());
        }

        g1.showonscreen();
        {
          std::clog << std::endl << "Press [Enter] to continue..." << std::endl;
          std::string s;
          std::getline(std::cin, s);
          usleep(200);
        }

      }

      if (plot2) {
        std::string datafile = "field2.data";
        std::ofstream fout(datafile.c_str());
        std::string field_name = "~E0+~E2"; //"~E1";
        if (! EMFmgr.has_field(field_name)) {
          std::ostringstream message;
          message << "No field named '" << field_name << "' !";
          throw std::logic_error(message.str());
        }
        emfield::base_electromagnetic_field & the_field = EMFmgr.grab_field(field_name);
        double dx = 0.4 * CLHEP::m;
        double dy = 0.4 * CLHEP::m;
        double dz = 0.4 * CLHEP::m;
        fout << "# x y z Ex Ey Ez Bx By Bz" << std::endl;
        fout.precision(15);
        double delta_time = 0.01 * CLHEP::millisecond;
        double max_time = 1 * CLHEP::millisecond;
        // Time loop :
        int frame_counter = 0;
        for (double time = 0.0 * CLHEP::second;
             time < max_time;
             time += delta_time) {
          for (double x = -1.0 * CLHEP::m; x <= +1.0 * CLHEP::m; x += dx) {
            for (double y = -1.0 * CLHEP::m; y <= +1.0 * CLHEP::m; y += dy) {
              for (double z = -1.0 * CLHEP::m; z <= +1.0 * CLHEP::m; z += dz) {
                fout << x << ' ' << y << ' ' << z << ' ';
                geomtools::vector_3d position(x, y, z);
                geomtools::vector_3d b_field;
                geomtools::vector_3d e_field;
                the_field.compute_magnetic_field(position, time, b_field);
                the_field.compute_electric_field(position, time, e_field);
                b_field /= CLHEP::gauss;
                b_field *= b_scale;
                e_field /=(CLHEP::volt / CLHEP::m);
                e_field *= e_scale;
                fout << e_field.x() << ' ' << e_field.y()  << ' ' << e_field.z() << ' ';
                fout << b_field.x() << ' ' << b_field.y()  << ' ' << b_field.z() << ' ';
                fout << std::endl;
              }
            }
          }
          fout << std::endl << std::endl;
          frame_counter++;
        }
        Gnuplot g1;
        g1.set_title(std::string("Electromagnetic field") + " '" + field_name + "'");
        g1.set_grid();
        g1.set_xrange(-1.25 * CLHEP::m, +1.25 * CLHEP::m);
        g1.set_yrange(-1.25 * CLHEP::m, +1.25 * CLHEP::m);
        g1.set_zrange(-1.25 * CLHEP::m, +1.25 * CLHEP::m);
        g1.cmd("set size ratio -1");
        g1.cmd("set view equal xyz");
        {
          std::ostringstream cmd_oss;
          cmd_oss << "set xyplane at " << -1 * CLHEP::m;
          g1.cmd(cmd_oss.str());
        }
        g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");
        g1.cmd("set xtics");
        g1.cmd("set ytics");
        g1.cmd("set ztics");

        for (int iframe = 0; iframe < frame_counter; iframe++) {
          double time = iframe * delta_time;
          std::ostringstream cmd_oss;
          cmd_oss << "splot ";
          cmd_oss << " '" << datafile << "'" << " index " << iframe
                  << " using 1:2:3:4:5:6 title \"E(t=" << time / CLHEP::millisecond << " ms)\" with vectors lt 3 ";
          cmd_oss << ", '" << datafile << "'" << " index " << iframe
                  << " using 1:2:3:7:8:9 title \"B(t=" << time / CLHEP::millisecond << " ms)\" with vectors lt 1 ";
          g1.cmd(cmd_oss.str());
          g1.showonscreen();
          usleep(50000);
        }

        {
          std::clog << std::endl << "Press [Enter] to continue..." << std::endl;
          std::string s;
          std::getline(std::cin, s);
          // if (s.length() && s[0] == 'q') break;
          usleep(1000);
        }

      }
    }

    std::clog << "The end." << std::endl;
  }
  catch(std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch(...) {
    std::cerr << "error: " << "Unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return(error_code);
}

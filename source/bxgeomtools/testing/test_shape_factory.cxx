// test_shape_factory.cxx

// Ourselves;
#include <geomtools/shape_factory.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/temporary_files.h>
#include <datatools/utils.h>
#include <datatools/clhep_units.h>

// This project;
#include <geomtools/geomtools_config.h>
#include <geomtools/cylinder.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

void test0(bool draw_, bool dump_);

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    bool draw = false;
    bool dump = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-d" || arg == "--dump") dump = true;

      iarg++;
    }

    test0(draw, dump);

  } catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

void test0(bool draw_, bool dump_)
{
  datatools::logger::priority logging = datatools::logger::PRIO_DEBUG;
  
  bool dump = dump_;
  bool dump2 = false;
  if (datatools::logger::is_debug(logging)) {
    dump = true;
    dump2 = true;
  }

  datatools::temp_file tmp_file;
  tmp_file.set_remove_at_destroy(true);
  tmp_file.create("/tmp", "test_shape_factory_");

  geomtools::shape_factory SF;
  DT_LOG_DEBUG(logging, "Initializing the shape factory...");
  SF.initialize_simple();
  std::string shapes_def = "${GEOMTOOLS_TESTING_DIR}/config/shapes.def";
  datatools::fetch_path_with_env(shapes_def);
  DT_LOG_DEBUG(logging, "Loading shapes' definitions...");
  SF.load(shapes_def);
  if (dump) {
    SF.tree_dump(std::cerr, " Shape factory: ", "[debug] ");
  }
  bool more = true;
  more = false;
  if (more) {
    DT_LOG_DEBUG(logging, "Creating more shapes...");
    {
      datatools::properties b0_cfg;
      b0_cfg.store("x", 3.4 * CLHEP::mm);
      b0_cfg.store("y", 5.6 * CLHEP::mm);
      b0_cfg.store("z", 1.2 * CLHEP::mm);
      const geomtools::i_object_3d & b0 = SF.create_reference("b0", "geomtools::box", b0_cfg);
      if (dump2) b0.tree_dump(std::clog, "Shape 'b0' : ");
    }

    {
      datatools::properties b1_cfg;
      b1_cfg.store("x", 1.2 * CLHEP::mm);
      b1_cfg.store("y", 3.4 * CLHEP::mm);
      b1_cfg.store("z", 5.6 * CLHEP::mm);
      const geomtools::i_object_3d & b1 = SF.create_reference("b1", "geomtools::box", b1_cfg);
      if (dump2) b1.tree_dump(std::clog, "Shape 'b1' : ");
    }

    {
      datatools::properties c3_cfg;
      c3_cfg.store("r", 0.5 * CLHEP::mm);
      c3_cfg.store("z", 2.2 * CLHEP::mm);
      const geomtools::i_object_3d & c3 = SF.create_reference("c3", "geomtools::cylinder", c3_cfg);
      if (dump2) c3.tree_dump(std::clog, "Shape 'c3' : ");
    }

    {
      datatools::properties u2_cfg;
      u2_cfg.store("shape_1.name", "b0");
      u2_cfg.store("shape_2.name", "b1");
      u2_cfg.store("shape_2.position", "2.3 1.2 0.5 (mm) / z 45 (degree)");
      const geomtools::i_object_3d & u2 = SF.create_reference("u2", "geomtools::union_3d", u2_cfg);
      if (dump2) u2.tree_dump(std::clog, "Shape 'u2' : ");
    }

    {
      datatools::properties s4_cfg;
      s4_cfg.store("shape_1.name", "b0");
      s4_cfg.store("shape_2.name", "b1");
      s4_cfg.store("shape_2.position", "2.3 1.2 0.5 (mm) / z 45 (degree)");
      const geomtools::i_object_3d & s4 = SF.create_reference("s4", "geomtools::subtraction_3d", s4_cfg);
      if (dump2) s4.tree_dump(std::clog, "Shape 's4' : ");
    }

  }
  if (dump) SF.tree_dump(std::clog, "Shape factory: ");

  DT_LOG_DEBUG(logging, "Building shape display map...");
  std::vector<std::string> names;
  SF.keys(names);
  std::map<int, std::string> display_map;
  {
    double r = 6.0 * CLHEP::mm;
    int index = 0;
    for (size_t i(0); i < names.size(); ++i) {
      const std::string & name = names[i];
      // if (name != "o106" && name != "o107") {
      //   continue;
      // }
      const geomtools::i_object_3d & obj = SF.get(name);
      if (dump) {
        obj.tree_dump(std::clog, name, "[dump] ");
      }
      geomtools::placement plcmt;
      double theta = i * 45.0 * CLHEP::degree;
      int nloops = (int) (theta / (360.0  * CLHEP::degree));
      double z = nloops * 5.0 * CLHEP::mm;
      plcmt.set(r * std::cos(theta), r * std::sin(theta), z, 0.0, 0.0, 0.0);
      tmp_file.out() << "# Shape: '" << name << "'" << '\n';
      uint32_t draw_options = 0;
      draw_options |= geomtools::i_wires_3d_rendering::WR_BASE_GRID;
      draw_options |= geomtools::i_wires_3d_rendering::WR_BASE_GRID_VERY_HIGH_DENSITY;
      draw_options |= geomtools::i_wires_3d_rendering::WR_BASE_HIGH_ANGLE_SAMPLING;
      geomtools::gnuplot_draw::draw(tmp_file.out(), plcmt, obj, draw_options);
      tmp_file.out() << "\n\n";
      display_map[index++] = name;
      DT_LOG_DEBUG(logging, "display_map size=" << display_map.size());
    }
    DT_LOG_DEBUG(logging, "End of shape loop.");
  }
  DT_LOG_DEBUG(logging, "Shape display map is built.");
  if (dump) SF.tree_dump(std::clog, "Shape factory: ");

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
  if (draw_) {
    Gnuplot g1;
    g1.cmd("set title 'Test geomtools::shape_factory' ");
    g1.cmd("set grid");
    g1.cmd("set size ratio -1");
    g1.cmd("set view equal xyz");
    g1.cmd("set xrange [-15:+15]");
    g1.cmd("set yrange [-15:+15]");
    g1.cmd("set zrange [-15:+15]");
    g1.cmd("set xyplane at -15");
    g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");
    {
      std::ostringstream plot_cmd;
      plot_cmd << "splot ";
      for (std::map<int, std::string>::const_iterator i = display_map.begin();
           i != display_map.end();
           i++) {
        if (i != display_map.begin()) {
          plot_cmd << " , ";
        }
        plot_cmd << "'" << tmp_file.get_filename ()
                 << "'    index " << i->first << " title '" << i->second << "' with lines ";
      }
      g1.cmd(plot_cmd.str());
      g1.showonscreen(); // window output
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }
  }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1

  return;
}

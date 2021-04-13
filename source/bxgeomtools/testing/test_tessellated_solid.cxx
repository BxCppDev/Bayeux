// test_tessellated_solid.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/temporary_files.h>
#include <datatools/utils.h>

// This project:
#include <geomtools/gnuplot_draw.h>
#include <geomtools/tessellation.h>
#include <geomtools/face_identifier.h>
#include <geomtools/gdml_writer.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

void test0();
void test1();
void test2();
void test3();
void test4();
void test5(bool draw_);
void test6(bool draw_);

int main (int argc_, char **argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Hello, this is a test program "
              << "for class 'tessellated_solid'!" << std::endl;

    bool draw = false;
    bool do_test6 = true;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-D" || arg == "--draw") {
        draw = true;
      } else if (arg == "-6" || arg == "--test6") {
        do_test6 = false;
      }

      iarg++;
    }

    test0();

    test1();

    test2();

    test3();

    test4();

    test5(draw);

    if (do_test6) test6(draw);

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

void test0()
{
  std::clog << "\n=== test0 ===" << std::endl;
  geomtools::vector_3d v0 (0, 0, 0);
  geomtools::vector_3d v1 (1, 0, 0);
  geomtools::vector_3d v2 (1.0, 1.0, 0);
  geomtools::vector_3d v3 (0, 1, 0);
  bool c = geomtools::facet34::check_quadrangle (v0, v1, v2, v3);
  if (!c) {
    std::clog << "test0: Invalid quadrangle !" << std::endl;
  } else {
    std::clog << "test0: Valid quadrangle !" << std::endl;
  }
  return;
}

void test1()
{
  std::clog << "\n=== test1 ===" << std::endl;
  geomtools::vector_3d v0 (0, 0, 0);
  geomtools::vector_3d v1 (1, 0, 0);
  geomtools::vector_3d v2 (0.5, 0.5, 0);
  geomtools::vector_3d v3 (0, 1, 0);
  bool c = geomtools::facet34::check_quadrangle (v0, v1, v2, v3);
  if (!c) {
    std::clog << "test1: Invalid quadrangle !" << std::endl;
  } else {
    std::clog << "test1: Valid quadrangle !" << std::endl;
  }
  return;
}

void test2()
{
  std::clog << "\n=== test2 ===" << std::endl;
  geomtools::vector_3d v0 (0, 0, 0);
  geomtools::vector_3d v1 (1,0, 0);
  geomtools::vector_3d v2 (0.25, 0.25, 0);
  geomtools::vector_3d v3 (0, 1, 0);
  bool c = geomtools::facet34::check_quadrangle (v0, v1, v2, v3);
  if (!c) {
    std::clog << "test2: Invalid quadrangle !" << std::endl;
  } else {
    std::clog << "test2: Valid quadrangle !" << std::endl;
  }
  return;
}

void test3()
{
  std::clog << "\n=== test3 ===" << std::endl;
  geomtools::vector_3d v0 (0, 0, 0);
  geomtools::vector_3d v1 (1,0, 0);
  geomtools::vector_3d v2 (0.25, 0.25, 0);
  bool c = geomtools::facet34::check_triangle (v0, v1, v2);
  if (!c) {
    std::clog << "test3: Invalid triangle !" << std::endl;
  } else {
    std::clog << "test3: Valid triangle !" << std::endl;
  }
  return;
}

void test4()
{
  std::clog << "\n=== test4 ===" << std::endl;
  geomtools::vector_3d v0 (0, 0, 0);
  geomtools::vector_3d v1 (1, 0, 0);
  geomtools::vector_3d v2 (2, 0, 0);
  bool c = geomtools::facet34::check_triangle (v0, v1, v2);
  if (!c) {
    std::clog << "test4: Invalid triangle !" << std::endl;
  } else {
    std::clog << "test4: Valid triangle !" << std::endl;
  }
  return;
}

void test5(bool draw_)
{
  std::clog << "\n=== test5 ===" << std::endl;
  geomtools::tessellated_solid TS1;

  int ivtx = 0;
  int P = TS1.add_vertex (ivtx++, 0, 0, 0);
  int Q = TS1.add_vertex (ivtx++, 1, 0, 0);
  int R = TS1.add_vertex (ivtx++, 1, 1, 0);
  int S = TS1.add_vertex (ivtx++, 0, 1, 0);
  int T = TS1.add_vertex (ivtx++, 0.5, 0.5, 1);

  int ifct = 0;
  TS1.add_facet4 (ifct++, P, S, R, Q);
  TS1.add_facet3 (ifct++, P, Q, T);
  TS1.add_facet3 (ifct++, Q, R, T);
  TS1.add_facet3 (ifct++, R, S, T);
  TS1.add_facet3 (ifct++, S, P, T);
  TS1.lock ();
  TS1.tree_dump (std::clog, "TC1: ");

  ivtx = 0;
  ifct = 0;
  geomtools::tessellated_solid TS2;
  double z0 = 2;
  int A1 = TS2.add_vertex (ivtx++, 0, 0, z0);
  int B1 = TS2.add_vertex (ivtx++, 2, 0, z0);
  int C1 = TS2.add_vertex (ivtx++, 2, 1, z0);
  int D1 = TS2.add_vertex (ivtx++, 0, 1, z0);
  int A2 = TS2.add_vertex (ivtx++, 0, 0, z0 + 0.5);
  int B2 = TS2.add_vertex (ivtx++, 2, 0, z0 + 0.5);
  int C2 = TS2.add_vertex (ivtx++, 2, 1, z0 + 0.5);
  int D2 = TS2.add_vertex (ivtx++, 0, 1, z0 + 0.5);

  TS2.add_facet4 (ifct++, A2, D2, C2, B2);
  TS2.add_facet4 (ifct++, A1, D1, C1, B1);
  TS2.add_facet4 (ifct++, A1, B1, B2, A2);
  TS2.add_facet4 (ifct++, B1, C1, C2, B2);
  TS2.add_facet4 (ifct++, C1, D1, D2, C2);
  TS2.add_facet4 (ifct++, D1, A1, A2, D2);

  TS2.lock ();
  TS2.tree_dump (std::clog, "TS2: ");
   
  std::clog << "[info] Store:" << '\n';
  datatools::temp_file store_tmp_file;
  store_tmp_file.set_remove_at_destroy(false);
  store_tmp_file.create("/tmp", "test_store_tessellated_solid");
  TS2.store(store_tmp_file.out());
  store_tmp_file.close();

  {
    geomtools::tessellated_solid TS2bis;
    std::ifstream fLoad(store_tmp_file.get_filename());
    TS2bis.load(fLoad);
    TS2bis.tree_dump (std::clog, "TS2 (reload): ");
  }

  datatools::temp_file tmp_file;
  tmp_file.set_remove_at_destroy(true);
  tmp_file.create("/tmp", "test_tessellated_solid");

  geomtools::placement tessel_placement;
  tessel_placement.set_identity();

  {
    tmp_file.out() << "# draw (index 0)" << std::endl;
    geomtools::gnuplot_draw::draw_tessellated(tmp_file.out(),
                                              tessel_placement,
                                              TS1,
                                              geomtools::i_wires_3d_rendering::WR_NONE
                                              | geomtools::i_wires_3d_rendering::WR_BASE_BOUNDINGS
                                              );
    geomtools::gnuplot_draw::draw_tessellated(tmp_file.out(),
                                              tessel_placement,
                                              TS2,
                                              geomtools::i_wires_3d_rendering::WR_NONE
                                              | geomtools::i_wires_3d_rendering::WR_BASE_BOUNDINGS
                                              );
    tmp_file.out() << std::endl << std::endl;
  }

  {
    bool do_bulk = true;
    bool do_surf = true;

    // do_bulk = false;
    // do_surf = false;

    int counts = 0;
    tmp_file.out() << "# locate (index 1)" << std::endl;
    size_t nshoots = 50000;
    for (int i = 0; i < (int) nshoots; i++) {
      double dim = 3. * CLHEP::mm;
      geomtools::vector_3d position (dim * ( 0 + drand48 ()),
                                     dim * ( 0 + drand48 ()),
                                     dim * ( -1 + 2 * drand48 ()));
      double skin = 0.05 * CLHEP::mm;
      geomtools::vector_3d position_c;
      tessel_placement.mother_to_child(position, position_c);
      if (do_surf && TS1.check_surface(position_c, skin)) {
        geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 3.0);
        counts++;
      } else if (do_surf && TS2.check_surface(position_c, skin)) {
        geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 2.0);
        counts++;
      } else if (do_bulk && TS1.check_inside(position_c, skin)) {
        geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 4.0);
        counts++;
      } else if (do_bulk && TS2.check_inside(position_c, skin)) {
        geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 5.0);
        counts++;
      }
    }
    if (counts == 0) {
      tmp_file.out() << "0 0 0 -1" << std::endl;
    }
    tmp_file.out() << std::endl << std::endl;
  }
  tmp_file.close();
  usleep(200);

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
  if (draw_) {

    {
      Gnuplot g1("lines");
      {
        std::ostringstream title;
        title << "set title '";
        title << "Tessellated solids";
        g1.cmd(title.str());
      }
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      g1.cmd("set xrange [-5:+5]");
      g1.cmd("set yrange [-5:+5]");
      g1.cmd("set zrange [-5:+5]");
      g1.cmd("set xyplane at -5");
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 title 'Tessellated solids' with lines ";
        plot_cmd << ", '' index 1 using 1:2:3:4 title 'Locate' with points pt 6 ps 0.3 linecolor variable ";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }

    }

    geomtools::gdml_writer writer;
    // Defines:
    writer.add_position ("null.pos",
                         0. * CLHEP::mm,
                         0. * CLHEP::mm,
                         0. * CLHEP::mm,
                         "mm");
    // Materials:
    writer.add_element("Hydrogen", 1, "H",  1);
    writer.add_element("Carbon",   6, "C", 12);
    writer.add_element("Nitrogen", 7, "N", 14);
    writer.add_element("Oxygen",   8, "O", 16);

    std::map<std::string, double> Air_map;
    Air_map["Oxygen"]   = 0.3;
    Air_map["Nitrogen"] = 0.7;
    writer.add_material("Air",
                        "air",
                        1.29 * CLHEP::mg / CLHEP::cm3,
                        Air_map);
    writer.add_material("Aluminium",
                        13.0,
                        2.70 * CLHEP::g / CLHEP::cm3,
                        26.98);

    // Solids:
    writer.add_gdml_box("lab.solid",
                        15.0 * CLHEP::mm, 15.0 * CLHEP::mm, 15.0 * CLHEP::mm,
                        "mm");
    writer.add_tessellated("tessella.solid", TS1, "mm");
    /*
    writer.add_gdml_box("tessella.solid",
                        5.0 * CLHEP::mm, 5.0 * CLHEP::mm, 5.0 * CLHEP::mm,
                        "mm");
    */

    // Structures:
    writer.add_volume("dummy.log",
                      "Aluminium",
                      "tessella.solid");

    std::list<geomtools::gdml_writer::physvol> world_physvols;
    world_physvols.push_back(geomtools::gdml_writer::physvol("dummy.log", "null.pos"));
    writer.add_volume("world.log",
                      "Air",
                      "lab.solid",
                      world_physvols);

    // Setup:
    writer.add_setup("Setup", "world.log");

    writer.dump(std::clog);

    std::string gdml_file = "test_tessellated_solid_1.gdml";
    writer.save_file(gdml_file,
                     geomtools::gdml_writer::default_xml_version(),
                     geomtools::gdml_writer::default_xml_encoding(),
                     geomtools::gdml_writer::default_gdml_schema(),
                     geomtools::gdml_writer::default_xsi()
                     );



  }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
  return;
}

void test6(bool draw_)
{
  std::clog << "\n=== test6 ===" << std::endl;
  geomtools::tessellated_solid TS;

  datatools::properties TS_config;
  TS_config.store_string("length_unit", "mm");
  TS_config.store_path("stl_file", "${GEOMTOOLS_TESTING_DIR}/data/test_stl_import.stl");
  TS.initialize(TS_config);

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
  if (draw_) {
    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_tessellated_solid");

    geomtools::vector_3d tessel_pos;
    geomtools::rotation_3d tessel_rot;
    geomtools::create_rotation(tessel_rot, 0.0, 0.0, 0.0);
    geomtools::gnuplot_draw::draw_tessellated(tmp_file.out(), tessel_pos, tessel_rot, TS);
    tmp_file.out() << std::endl;
    tmp_file.close();
    usleep(200);

    {
      Gnuplot g1 ("lines");
      {
        std::ostringstream title;
        title << "set title '";
        title << "Tessellated solid from CATIA/STL export";
        g1.cmd(title.str());
      }
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      g1.cmd("set xyplane at -30");
      g1.set_xrange(-30, +30).set_yrange(-30, +30).set_zrange(-30, +30);
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");

      g1.plotfile_xyz(tmp_file.get_filename (), 1, 2, 3, "3D view");
      g1.showonscreen(); // window output
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }

  }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
  return;
}

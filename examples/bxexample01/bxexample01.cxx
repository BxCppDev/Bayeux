//! \file  bxexample01.cxx
//! \brief A sample example program for Bayeux's libraries
//
// Copyright (c) 2013 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2013 by Université de Caen Basse-Normandie
//
// This file is part of Bayeux.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// Standard libraries
#include <iostream>
#include <string>
#include <cstdlib>

// Third Party
// - Boost
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem.hpp>

struct params_type {
  bool interactive;
  params_type () : interactive(false) {}
};

params_type & params()
{
  static boost::scoped_ptr<params_type > _params;
  if (! _params) {
    _params.reset(new params_type );
  }
  return *_params.get();
}

void title(std::string t_) {
  std::cout << std::endl;
  int n = t_.length();
  for (int i = 0; i < n+4; i++) {
    std::cout << '*';
  }
  std::cout << std::endl;
  std::cout << "* " << t_ << " *\n";
  for (int i = 0; i < n+4; i++) {
    std::cout << '*';
  }
  std::cout << std::endl;
  return;
}

// - Bayeux
#include <bayeux/bayeux_config.h>
#include <bayeux/version.h>

int example_bayeux()
{
  title("bayeux");

  std::clog << "Bayeux version is: " << bayeux::version::get_version() << std::endl;

  return 0;
}

// - Bayeux/datatools
#include <datatools/datatools_config.h>
#include <datatools/version.h>
#include <datatools/properties.h>
#include <datatools/kernel.h>
#include <datatools/library_info.h>

int example_datatools()
{
  title("datatools");

  std::clog << "datatools version is: " << datatools::version::get_version() << std::endl;

  if (datatools::kernel::is_instantiated()) {
    std::clog << "datatools kernel is instantiated." << std::endl;
    datatools::kernel & dt_kernel = datatools::kernel::instance();
    const datatools::library_info & lib_info_reg = dt_kernel.get_library_info_register();
    lib_info_reg.tree_dump(std::clog, "Library info register: ");
  }

  {
    std::clog << "datatools::properties object manipulation example: " << std::endl;
    datatools::properties config;
    config.store_flag("test");
    config.store("library.name", "datatools", "The datatools library name", true);
    config.store("library.version", datatools::version::get_version(),
                 "The datatools library version", true);
    config.store("debug.level", 3, "The debug level");
    config.tree_dump(std::clog, "Some configuration parameters:");
    std::string filename = "bxexample01_datatools.conf";
    config.write_configuration(filename);
    std::clog << "datatools configuration example file is: '" << filename << "'." << std::endl;
  }

  {
    std::string filename="@datatools:foo/bar.data";
    std::clog << "datatools dummy file path is: '"
              << filename << "'." << std::endl;
    try {
      datatools::fetch_path_with_env(filename);
      std::clog << "datatools expanded data file path is: '"
                << filename << "'." << std::endl;
    } catch (std::exception & error) {
      DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                    "As expected, we cannot parse this file path directive; reason is: "
                    << error.what());
    }
  }

  return 0;
}

// - Bayeux/cuts
#include <cuts/version.h>

int example_cuts()
{
  title("cuts");

  std::clog << "cuts version is: " << cuts::version::get_version() << std::endl;

  return 0;
}

// - Bayeux/materials
#include <materials/version.h>
#include <materials/resource.h>

int example_materials()
{
  title("materials");

  std::clog << "materials version is   : "
            << materials::version::get_version() << std::endl;
  std::clog << "materials resource dir : "
            << materials::get_resource_dir() << std::endl;

  {
    std::string filename="@materials:data/simple_elements.def";
    std::clog << "materials data file path is: '"
              << filename << "'." << std::endl;
    datatools::fetch_path_with_env(filename);
    std::clog << "materials expanded data file path is: '"
              << filename << "'." << std::endl;
  }

  {
    // Force env. var. :
    std::string env_name = "MATERIALS_DATA_DIR";
    std::string env_value = "/tmp/materials/alt.resources";
    if (! getenv(env_name.c_str())) {
      setenv(env_name.c_str(), env_value.c_str(), 1);
    }
    std::string filename="${MATERIALS_DATA_DIR}/data/simple_elements.def";
    std::clog << "materials data file alt. path is: '"
              << filename << "'." << std::endl;
    datatools::fetch_path_with_env(filename);
    std::clog << "materials expanded data file alt. path is: '"
              << filename << "'." << std::endl;
  }

  return 0;
}

// - Bayeux/mygsl
#include <mygsl/version.h>
#include <mygsl/histogram_1d.h>

int example_mygsl()
{
  title("mygsl");

  std::clog << "mygsl version is: "
            << mygsl::version::get_version() << std::endl;

  {
    std::clog << "mygsl::histogram object manipulation example: " << std::endl;
    mygsl::histogram h(15, -1.0, 2.0, mygsl::BIN_MODE_LINEAR);
    h.grab_auxiliaries().store("display.title", "A dummy histogram");
    h.grab_auxiliaries().store("display.x.label", "Energy");
    h.grab_auxiliaries().store("display.x.unit", "MeV");
    h.grab_auxiliaries().store("display.y.label", "dN/E");
    h.grab_auxiliaries().store("display.y.unit", "Count/0.2MeV");
    for (int i = 0; i < 1000; i++) {
      h.fill(drand48());
    }
    h.tree_dump(std::clog, "A 1D-histogram:");
    h.print_ascii(std::cout);
    h.reset();
  }

  return 0;
}

// - Bayeux/geomtools
#include <geomtools/version.h>
#include <geomtools/box.h>
#include <geomtools/logical_volume.h>
#include <geomtools/gnuplot_drawer.h>

int example_geomtools()
{
  title("geomtools");

  std::clog << "geomtools version is: "
            << geomtools::version::get_version() << std::endl;

  {
    std::clog << "geomtools::box object manipulation example: " << std::endl;
    geomtools::box b(3.0*CLHEP::cm, 2.0*CLHEP::cm, 0.5*CLHEP::cm);
    b.lock();
    b.tree_dump(std::clog, "A 3D-box:");
    std::clog << "Surface is: "
              << b.get_surface()/CLHEP::mm2 << " mm2" << std::endl;
    std::clog << "Top surface is: "
              << b.get_surface(geomtools::box::FACE_TOP)/CLHEP::mm2 << " mm2" << std::endl;
    std::clog << "Volume is: "
              << b.get_volume()/CLHEP::mm3 << " mm3" << std::endl;
    geomtools::logical_volume box_log("Box", b);
    box_log.set_material_ref("Inox");
    box_log.grab_parameters().store("material.mass_activity.Bi214",
                                    1.33e-3 * CLHEP::becquerel/CLHEP::kg);
    box_log.grab_parameters().set_explicit_unit("material.mass_activity.Bi214",
                                                true);
    box_log.grab_parameters().store("visibility.color", "red");
    box_log.tree_dump(std::clog, "Logical associated to the 3D-box:");
    geomtools::gnuplot_drawer drawer;
    geomtools::placement box_loc;
    if (params().interactive) drawer.draw_logical(box_log, box_loc, 100, "Display a 3D-box");

    b.reset();
  }

  return 0;
}

// - Bayeux/emfield
#include <emfield/version.h>
#include <emfield/uniform_magnetic_field.h>

int example_emfield()
{
  title("emfield");

  std::clog << "emfield version is: "
            << emfield::version::get_version() << std::endl;

  {
    std::clog << "emfield::uniform_magnetic_field object manipulation example: "
              << std::endl;
    emfield::uniform_magnetic_field b;
    b.set_uniform_magnetic_field(geomtools::vector_3d(0.0*CLHEP::gauss,
                                                      0.0*CLHEP::gauss,
                                                      25.0*CLHEP::gauss));
    b.initialize_simple();
    b.tree_dump(std::clog, "Uniform magnetic field:");
    const geomtools::vector_3d pos(12.0*CLHEP::cm,
                                   8.4*CLHEP::cm,
                                   -5.0*CLHEP::mm);
    double time = 2.34 * CLHEP::s;
    geomtools::vector_3d bvec;
    b.compute_magnetic_field(pos, time, bvec);
    std::clog << "Magnetic field amplitude is: "
              << bvec.mag()/CLHEP::gauss << " T" << std::endl;
    b.reset();
  }

  return 0;
}

// - Bayeux/genbb_help
#include <genbb_help/version.h>
#include <genbb_help/wdecay0.h>
#include <genbb_help/primary_event.h>

int example_genbb_help()
{
  title("genbb_help");

  std::clog << "genbb_help version is: "
            << genbb::version::get_version() << std::endl;

  {
    std::clog << "genbb::wdecay0 object manipulation example: "
              << std::endl;
    datatools::properties config;
    config.store("seed", 314159);
    config.store("decay_type", "DBD");
    config.store("decay_isotope", "Se82");
    config.store("decay_dbd_level", 0);
    config.store("decay_dbd_mode", 4);
    config.store("energy_min", 0.0);
    config.store("energy_max", 3.2);
    config.store("energy_unit", "MeV");
    config.tree_dump(std::clog, "wdecay0 configuration: ");

    genbb::wdecay0 wd0;
    wd0.initialize_standalone(config);
    for (int i =0; i < 4; i++) {
      genbb::primary_event pe;
      wd0.load_next(pe);
      pe.tree_dump(std::clog, "Generated primary event: ");
    }
    wd0.reset();
  }
  return 0;
}

// - Bayeux/genvtx
#include <genvtx/version.h>
#include <genvtx/box_vg.h>
#include <mygsl/rng.h>

int example_genvtx()
{
  title("genvtx");

  std::clog << "genvtx version is: "
            << genvtx::version::get_version() << std::endl;

  {
    std::clog << "genvtx::box_vg object manipulation example: "
              << std::endl;
    geomtools::box b(3.0*CLHEP::cm, 2.0*CLHEP::cm, 1.0*CLHEP::cm);
    b.lock();
    b.tree_dump(std::clog, "A 3D-box:");

    genvtx::box_vg bvg;
    bvg.set_box(b);
    bvg.set_bulk(0.0*CLHEP::cm);
    bvg.set_skin_skip(-1*CLHEP::mm);
    bvg.initialize_simple();
    bvg.tree_dump(std::clog, "A vertex generator for 3D-box:");

    mygsl::rng ran("taus", 314159);
    for (int i =0; i < 4; i++) {
     geomtools::vector_3d vertex;
     bvg.shoot_vertex(ran, vertex);
     std::clog << "Generated primary vertex: " << vertex/CLHEP::mm << std::endl;
    }
    bvg.reset();

  }
  return 0;
}

// - Bayeux/brio
#include <brio/version.h>
#include <brio/writer.h>
#include <brio/reader.h>

int example_brio()
{
  title("brio");

  std::clog << "brio version is: " << brio::version::get_version() << std::endl;

  std::clog << "Starting brio serialization example..." << std::endl;
  std::string filename = "bxexample01_brio.brio";
  {
    long int seed = 314159;
    srand48(seed);
    brio::writer out(filename);
    for (int i = 0; i < 4; i++) {
      datatools::properties config;
      config.store("function", "example_brio", "The test function for brio");
      config.store("library.name", "brio", "The brio library name");
      config.store("library.version", brio::version::get_version(),
                   "The brio library version");
      config.store("random", drand48(), "A random number (float)");
      out.store(config);
    }
    out.close ();
  }
  {
    brio::reader in(filename);
    while (in.has_next ()) {
       datatools::properties config;
       in.load_next(config);
    }
    in.close ();
  }
  std::clog << "brio serialization example file is: '" << filename << "'." << std::endl;

  std::clog << "brio serialization example is terminated." << std::endl;
  return 0;
}

// - Bayeux/dpp
#include <dpp/version.h>

int example_dpp()
{
  title("dpp");

  std::clog << "dpp version is: " << dpp::version::get_version() << std::endl;

  return 0;
}

// - Bayeux/mctools
#include <mctools/version.h>

int example_mctools()
{
  title("mctools");

  std::clog << "mctools version is: " << mctools::version::get_version() << std::endl;

  return 0;
}

#include <camp/class.hpp>
int example_reflection()
{
  std::clog << "Number of metaclasses = " << camp::classCount() << std::endl;
  for (int i = 0; i < camp::classCount(); i++) {
    const camp::Class & c = camp::classByIndex(i);
    std::clog << "Metaclass #" << i << " : " << c.name() << std::endl;
  }

  std::clog << "Number of metaenums = " << camp::enumCount() << std::endl;
  for (int i = 0; i < camp::enumCount(); i++) {
    const camp::Enum & e = camp::enumByIndex(i);
    std::clog << "Metaenum #" << i << " : " << e.name() << std::endl;
  }
  return 0;
}

#include <bayeux/bayeux.h>

int main(int argc, char *argv[]) {
  bayeux::initialize(argc, argv);

  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Welcome to Bayeux bxexample01 !" << std::endl;

    std::vector<std::string> args;
    for (int i =1; i < argc; i++) {
      args.push_back(std::string(argv[i]));
    }
    if (std::find(args.begin(), args.end(), "--interactive") != args.end()) {
      params().interactive = true;
    }

    example_datatools();

    example_cuts();

    example_materials();

    example_mygsl();

    example_geomtools();

    example_emfield();

    example_genbb_help();

    example_genvtx();

    example_brio();

    example_dpp();

    example_mctools();

    example_reflection();

    example_bayeux();

    std::clog << "The end." << std::endl;
  } catch (std::exception & error) {
    DT_LOG_ERROR(datatools::logger::PRIO_ERROR, error.what());
    error_code = EXIT_FAILURE;
  } catch (...) {
    DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }

  bayeux::terminate();
  return error_code;
}

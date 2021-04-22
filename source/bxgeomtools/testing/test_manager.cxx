// test_manager.cxx
/*
 * Copyright 2007-2013 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

// Ourselves:
#include <geomtools/manager.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Boost:
#include <boost/shared_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/utils.h>
#include <datatools/exception.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>

// This project:
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/gdml_export.h>
#include <geomtools/visibility.h>
#include <geomtools/manager_macros.h>
#include <geomtools/mapping.h>
#include <geomtools/materials_plugin.h>
#include <geomtools/materials_utils.h>
#include <geomtools/display_data.h>
#include <geomtools/box.h>
#include <geomtools/blur_spot.h>

/* A plugin for the geometry manager which instantiates
 * a 'mapping' object with its specific mapping rules.
 * It is a clone of the 'geomtools::mapping_plugin class'.
 */
class test_plugin : public geomtools::manager::base_plugin
{
public:
  test_plugin ();

  virtual ~test_plugin ();

  virtual int initialize(const datatools::properties & config_,
                         const geomtools::manager::plugins_dict_type & plugins_,
                         const datatools::service_dict_type & services_);

  /// Main reset method
  virtual int reset();

  /// Check if plugin is initialized
  virtual bool is_initialized() const;

  const geomtools::mapping & get_mapping () const;

protected:

  void _build_mapping (const datatools::properties & mapping_config_);

private:

  bool                _initialized_;
  geomtools::mapping  _mapping_;

  GEOMTOOLS_PLUGIN_REGISTRATION_INTERFACE(test_plugin)

};

GEOMTOOLS_PLUGIN_REGISTRATION_IMPLEMENT(test_plugin,"test_plugin")

const geomtools::mapping & test_plugin::get_mapping() const
{
  return _mapping_;
}

test_plugin::test_plugin()
{
  _initialized_ = false;
  return;
}

test_plugin::~test_plugin()
{
  if (is_initialized()) {
    reset ();
  }
  return;
}

bool test_plugin::is_initialized() const
{
  return _initialized_;
}

int test_plugin::initialize(const datatools::properties & config_,
                            const geomtools::manager::plugins_dict_type & /* plugins_ */,
                            const datatools::service_dict_type & /* services_ */)
{
  DT_THROW_IF(is_initialized(), std::logic_error, "Plugin is already initialized !");
  _build_mapping(config_);
  _initialized_ = true;
  return 0;
}

void test_plugin::_build_mapping (const datatools::properties & mapping_config_)
{
  _mapping_.set_id_manager (get_geo_manager().get_id_mgr ());
  _mapping_.initialize (mapping_config_);
  _mapping_.build_from (get_geo_manager().get_factory (),
                        get_geo_manager().get_world_name ());
  return;
}

int test_plugin::reset()
{
  _initialized_ = false;
  return 0;
}

/*****************************************************/
int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
      std::clog << "Test program for class 'geomtools::manager' !" << std::endl;

      bool   trace = false;
      bool   debug = false;
      bool   verbose = false;
      std::string manager_config_file;
      bool   dump = false;
      bool   visu = false;
      std::string visu_object_name = "";
      int    visu_depth  = geomtools::gnuplot_drawer::display_level_no_limit();
      std::string drawer_view = geomtools::gnuplot_drawer::view_3d();
      bool   gdml = false;
      bool   gdml_replica_support = false;
      bool   use_plugins = false;
      bool   force_show = false;
      bool   force_show_envelope = false;
      bool   force_show_children = false;
      bool   add_dd = false;
      int    nb_dd = 1;
      // parse command line argument:
      int iarg = 1;
      while (iarg < argc_)
        {
          std::string token = argv_[iarg];
          //std::cerr << "Token = " << token << '\n';
          if (token[0] == '-')
            {
              std::string option = token;
              if ((option == "-d") || (option == "--debug"))
                {
                  debug = true;
                }
              else if ((option == "-V") || (option == "--verbose"))
                {
                  verbose = true;
                }
              else if ((option == "-T") || (option == "--trace"))
                {
                  trace = true;
                }
              else if ((option == "-v") || (option == "--visu"))
                {
                  visu = true;
                }
              else if ((option == "-f") || (option == "--force-show"))
                {
                  force_show = true;
                }
              else if ((option == "-e") || (option == "--force-show-envelope"))
                {
                  force_show_envelope = true;
                }
              else if ((option == "-c") || (option == "--force-show-children"))
                {
                  force_show_children = true;
                }
              else if ((option == "-D") || (option == "--dump"))
                {
                  dump = true;
                }
              else if ((option == "-p") || (option == "--plugins"))
                {
                  use_plugins = true;
                }
              else if ((option == "-g") || (option == "--gdml"))
                {
                  gdml = true;
                }
              else if ((option == "-R") || (option == "--replica"))
                {
                  gdml_replica_support = true;
                }
              else if ((option == "-m") || (option == "--model"))
                {
                  std::string s = argv_[++iarg];
                  if (s.empty ())
                    {
                      throw std::logic_error ("Missing model name !");
                    }
                  visu_object_name = s;
                }
              else if ((option == "-D") || (option == "--visu-depth"))
                {
                  std::string s = argv_[++iarg];
                  if (s.empty ())
                    {
                      throw std::logic_error ("Missing visualization depth !");
                    }
                  std::istringstream iss (s);
                  iss >> visu_depth;
                  if (! iss)
                    {
                      std::cerr << "warning: invalid format for visualization depth !" << std::endl;
                    }
                  if (visu_depth < 0)
                    {
                      std::cerr << "warning: invalid value for visualization depth !" << std::endl;
                      visu_depth = 0;
                    }
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
              else if (option == "--add-display-data")
                {
                  add_dd = true;
                }
              else if (option == "--nb-display-data")
                {
                  add_dd = true;
                  nb_dd = atoi(argv_[++iarg]);
                }
              else
                {
                  std::clog << "warning: ignoring option '"
                            << option << "'!" << std::endl;
                }
            }
          else
            {
              std::string argument = token;
              if (manager_config_file.empty ())
                {
                  manager_config_file = argument;
                }
              else
                {
                  std::clog << "warning: ignoring argument '"
                            << argument << "'!" << std::endl;
                }
            }
          iarg++;
        }

      if (manager_config_file.empty ())
        {
          manager_config_file = "${GEOMTOOLS_TESTING_DIR}/config/test-1.0/test_manager.conf";
        }
      datatools::fetch_path_with_env (manager_config_file);
      std::clog << "Manager config. file : '" << manager_config_file << "'" << std::endl;

      // load properties from the configuration file:
      datatools::properties manager_config;
      datatools::read_config(manager_config_file,
                                         manager_config);

      DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(geomtools::manager::base_plugin).tree_dump (std::clog, "Geometry plugin system register: ", "INFO: ");

      geomtools::manager geo_mgr;
      if (verbose) geo_mgr.set_logging_priority (datatools::logger::PRIO_NOTICE);
      if (debug) geo_mgr.set_logging_priority (datatools::logger::PRIO_DEBUG);
      if (trace) geo_mgr.set_logging_priority (datatools::logger::PRIO_TRACE);
      geo_mgr.initialize (manager_config);
      if (dump)
        {
          geo_mgr.get_factory ().tree_dump (std::clog, "The embedded geometry model factory : ");
          geo_mgr.get_id_mgr ().tree_dump (std::clog, "The embedded GID manager : ");
          std::clog << "The embedded GID mapping : " << std::endl;
          geo_mgr.get_mapping ().dump_dictionnary (std::clog);
        }
      geo_mgr.tree_dump (std::clog, "The geometry manager : ");

      if (use_plugins)
        {
          std::clog << "NOTICE: " << "Accessing some plugins..." << std::endl;

          std::string mapping_plugin_name;

          // Access to a given plugin by name and type :
          mapping_plugin_name = "mapping_depth_2";
          if (geo_mgr.has_plugin (mapping_plugin_name)
              && geo_mgr.is_plugin_a<test_plugin>(mapping_plugin_name))
            {
              std::clog << "NOTICE: " << "Found plugin named '" << mapping_plugin_name
                        << "'" << std::endl;
              const test_plugin & tp = geo_mgr.get_plugin<test_plugin>(mapping_plugin_name);
              const geomtools::mapping & tm = tp.get_mapping();
              tm.dump_dictionnary(std::clog);
            }

          mapping_plugin_name = "mapping_only_scin";
          if (geo_mgr.has_plugin (mapping_plugin_name)
              && geo_mgr.is_plugin_a<test_plugin>(mapping_plugin_name))
            {
              std::clog << "NOTICE: " << "Found plugin named '" << mapping_plugin_name
                        << "'" << std::endl;
              const test_plugin & tp = geo_mgr.get_plugin<test_plugin>(mapping_plugin_name);
              const geomtools::mapping & tm = tp.get_mapping();
              tm.dump_dictionnary(std::clog);
            }

          mapping_plugin_name = "mapping_only_sources";
          if (geo_mgr.has_plugin (mapping_plugin_name)
              && geo_mgr.is_plugin_a<test_plugin>(mapping_plugin_name))
            {
              std::clog << "NOTICE: " << "Found plugin named '" << mapping_plugin_name
                        << "'" << std::endl;
              const test_plugin & tp = geo_mgr.get_plugin<test_plugin>(mapping_plugin_name);
              const geomtools::mapping & tm = tp.get_mapping();
              tm.dump_dictionnary(std::clog);
            }

          mapping_plugin_name = "mapping_no_electrodes";
          if (geo_mgr.has_plugin (mapping_plugin_name)
              && geo_mgr.is_plugin_a<test_plugin>(mapping_plugin_name))
            {
              std::clog << "NOTICE: " << "Found plugin named '" << mapping_plugin_name
                        << "'" << std::endl;
              const test_plugin & tp = geo_mgr.get_plugin<test_plugin>(mapping_plugin_name);
              const geomtools::mapping & tm = tp.get_mapping();
              tm.dump_dictionnary(std::clog);
            }

          mapping_plugin_name = "mapping_only_shield";
          if (geo_mgr.has_plugin (mapping_plugin_name)
              && geo_mgr.is_plugin_a<test_plugin>(mapping_plugin_name))
            {
              std::clog << "NOTICE: " << "Found plugin named '" << mapping_plugin_name
                        << "'" << std::endl;
              const test_plugin & tp = geo_mgr.get_plugin<test_plugin>(mapping_plugin_name);
              const geomtools::mapping & tm = tp.get_mapping();
              tm.dump_dictionnary(std::clog);
            }

        }
      else
        {
          std::clog << "NOTICE: " << "Don't use plugins..." << std::endl;
        }

      if (visu)
        {
          while (true)
            {
              std::clog << "Models: " << std::endl;
              int count = 0;
              for (geomtools::models_col_type::const_iterator i
                     = geo_mgr.get_factory ().get_models ().begin ();
                   i != geo_mgr.get_factory ().get_models ().end ();
                   i++)
                {
                  bool visible = true;
                  visible = geomtools::visibility::is_shown (i->second->get_logical ().get_parameters ());
                  if (force_show)
                    {
                      visible = true;
                    }
                  if (visible)
                    {
                      std::clog << "  " << datatools::io::left
                                << datatools::io::width (35) << i->second->get_name ();
                      count++;
                      if ((count % 2) == 0) std::clog << std::endl;
                    }
                }
              std::clog << std::endl;

              std::string option_view;
              if (visu_object_name.empty ())
                {
                  std::clog << "Enter the name of the geometry model to display (ex: '"
                            << geo_mgr.get_world_name () << "') \n";
                  std::clog << "or the GID of a geometry volume (ex: '[220:3]', '[2020:1.0]') [.q = quit]: ";
                  std::string line;
                  std::getline (std::cin, line);
                  std::istringstream iss (line);
                  iss >> visu_object_name >> std::ws >> option_view;
                  std::clog << std::endl;
                }
              if (visu_object_name == ".q")
                {
                  visu_object_name = "";
                  break;
                }
              if (! option_view.empty ())
                {
                  if (option_view == "-xy")
                    {
                      drawer_view = geomtools::gnuplot_drawer::view_2d_xy();
                    }
                  else if (option_view == "-xz")
                    {
                      drawer_view = geomtools::gnuplot_drawer::view_2d_xz();
                    }
                  else if (option_view == "-yz")
                    {
                      drawer_view = geomtools::gnuplot_drawer::view_2d_yz();
                    }
                  else if (option_view == "-3d")
                    {
                      drawer_view = geomtools::gnuplot_drawer::view_3d();
                    }
                }

              if (visu_object_name.empty ()) {
                  visu_object_name = "world";
                }
              if (debug) {
                  std::clog << "DEBUG: " << "visu_object_name : '" << visu_object_name << "'" << std::endl;
                }

              std::vector<boost::shared_ptr<geomtools::display_data> > dd_ptrs;

              geomtools::gnuplot_drawer GPD;
              GPD.grab_properties().store(geomtools::gnuplot_drawer::world_name_key(),
                                          geo_mgr.get_world_name ());
              if (add_dd) {
                  mygsl::rng prng("taus2", 314159);

                  geomtools::placement dd_pl;
                  dd_pl.set_translation (0.0, 40.0*CLHEP::cm, 0.0);


                  double x0, x1, y0, y1, z0, z1;
                  x0 = -40.0*CLHEP::cm;
                  x1 = +40.0*CLHEP::cm;
                  y0 =   0.0*CLHEP::cm;
                  y1 = +20.0*CLHEP::cm;
                  z0 =   0.0*CLHEP::cm;
                  z1 = +35.0*CLHEP::cm;

                  // x0 =   0.0*CLHEP::cm;
                  // x1 = +25.0*CLHEP::cm;
                  // y0 = -30.0*CLHEP::cm;
                  // y1 = -20.0*CLHEP::cm;
                  // z0 =   0.0*CLHEP::cm;
                  // z1 = +25.0*CLHEP::cm;

                  {
                    boost::shared_ptr<geomtools::display_data> p;
                    dd_ptrs.push_back(p);
                    boost::shared_ptr<geomtools::display_data> & dd_ptr = dd_ptrs[0];
                    geomtools::box spot_box(x1-x0, y1-y0, z1-z0);
                    geomtools::display_data * dd = new geomtools::display_data;
                    geomtools::display_data::display_item & spot_box_DI
                      = dd->add_static_item ("spot_box",
                                             "group::vertex_box",
                                             "magenta");
                    geomtools::placement plcmt;
                    plcmt.set_translation(0.5*(x1+x0), 0.5*(y1+y0), 0.5*(z1+z0));
                    spot_box.generate_wires (spot_box_DI.wires, plcmt);
                    dd_ptr.reset(dd);
                    GPD.add_display_data (*dd_ptr.get(), dd_pl);
                  }

                  int ndd = nb_dd;
                  for (int i = 0; i < ndd; i++) {
                    boost::shared_ptr<geomtools::display_data> p;
                    dd_ptrs.push_back(p);
                  }

                  for(int i = 1; i < (int) dd_ptrs.size(); i++) {
                      boost::shared_ptr<geomtools::display_data> & dd_ptr = dd_ptrs[i];
                      geomtools::blur_spot bs(3, 1 *CLHEP::mm);
                      bs.set_errors(1.*CLHEP::cm,1*CLHEP::cm,1.*CLHEP::cm);
                      geomtools::display_data * dd = new geomtools::display_data;
                      std::ostringstream dd_name_oss;
                      dd_name_oss << "vertex_" << (i-1);
                      geomtools::display_data::display_item & spot_DI
                        = dd->add_static_item (dd_name_oss.str(),
                                               "group::vertices",
                                               (prng.uniform() < 0.5 ? "orange" : "blue"));
                      geomtools::placement plcmt;
                      if (i == 0)
                        {
                          plcmt.set_translation (0.5*(x1+x0)+5.0*CLHEP::cm,
                                                 0.5*(y1+y0)+10.0*CLHEP::cm,
                                                 0.5*(z1+z0)+10.0*CLHEP::cm);
                          plcmt.set_orientation (15.*CLHEP::degree,
                                                 0.0,
                                                 0.0);
                        }
                      else
                        {
                          plcmt.set_translation (prng.flat(x0, x1),
                                                 prng.flat(y0, y1),
                                                 prng.flat(z0, z1));
                          plcmt.set_orientation (prng.flat(0.0, 360.*CLHEP::degree),
                                                 std::acos(prng.flat(-1.0, +1.0)),
                                                 0.0);
                        }
                      bs.generate_wires (spot_DI.wires, plcmt);
                      dd_ptr.reset(dd);
                      {
                        GPD.add_display_data (*dd_ptr.get(), dd_pl);
                      }
                      dd_ptr.get()->tree_dump (std::clog,
                                               dd_name_oss.str(),
                                               "Embedded display data : ");
                    }
                  std::clog << "Embedded display data : " << dd_ptrs.size() << std::endl;
                }
              if (force_show)
                {
                  GPD.grab_properties ().store (geomtools::gnuplot_drawer::force_show_property_name(), true);
                }
              if (force_show_envelope)
                {
                  GPD.grab_properties ().store (geomtools::gnuplot_drawer::force_show_envelope_property_name(), true);
                }
              if (force_show_children)
                {
                  GPD.grab_properties ().store (geomtools::gnuplot_drawer::force_show_children_property_name(), true);
                }
              GPD.set_view (drawer_view);
              GPD.set_mode (geomtools::gnuplot_drawer::mode_wired());
              int view_code = GPD.draw (geo_mgr,
                                        visu_object_name,
                                        visu_depth);
              if (view_code != 0) {
                  std::cerr << "ERROR: " << "Cannot display the object with label '"
                            << visu_object_name << "' !" << std::endl;
                }
              visu_object_name = "";
            } // visu
        } else {
          std::clog << "NOTICE: " << "No visu..." << std::endl;
        }

      if (gdml) {
          std::string world_model_name = geo_mgr.get_world_name ();

          geomtools::gdml_writer material_writer; // GDML writer for materials
          geomtools::gdml_export GDML;            // factory->GDML exporter
          if (debug) GDML.set_logging_priority (datatools::logger::PRIO_DEBUG);

          std::clog << "NOTICE: " << "Accessing the materials driver plugin..." << std::endl;

          const materials::manager * mat_mgr_ref = 0;
          // Access to a given plugin by name and type :
          std::string materials_plugin_name = "materials_driver";
          if (geo_mgr.has_plugin (materials_plugin_name)
              && geo_mgr.is_plugin_a<geomtools::materials_plugin>(materials_plugin_name)) {
              std::clog << "NOTICE: " << "Found materials plugin named '" << materials_plugin_name
                        << "'" << std::endl;
              const geomtools::materials_plugin & mgp
                = geo_mgr.get_plugin<geomtools::materials_plugin>(materials_plugin_name);
              const materials::manager & mat_mgr = mgp.get_manager();
              mat_mgr_ref = &mat_mgr;
            }

          if (mat_mgr_ref != 0) {
              std::clog << "NOTICE: "
                        << "Export GDML materials from the materials driver plugin: "<< std::endl;
              geomtools::export_gdml (*mat_mgr_ref, material_writer);
              GDML.attach_external_materials (material_writer.get_stream (geomtools::gdml_writer::materials_section()));
            }

          GDML.add_auxiliary_support (false);
          GDML.add_replica_support (gdml_replica_support);
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
          GDML.export_gdml ("${GEOMTOOLS_TMP_DIR}/test_manager.gdml",
                            geo_mgr.get_factory (),
                            world_model_name);
          std::clog << "NOTICE: " << "GDML file '${GEOMTOOLS_TMP_DIR}/test_manager.gdml' has been generated." << std::endl;
          // GDML
        } else {
          std::clog << "NOTICE: " << "No GDML..." << std::endl;
        }

      std::clog << "NOTICE: " << "The end." << std::endl;
    }
  catch (std::exception & x) {
      std::cerr << "ERROR: " << x.what () << std::endl;
      error_code = EXIT_FAILURE;
    }
  catch (...) {
      std::cerr << "ERROR: " << "Unexpected error!" << std::endl;
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

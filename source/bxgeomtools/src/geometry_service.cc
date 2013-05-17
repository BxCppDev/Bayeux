/* geometry_service.cc
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#include <geomtools/geometry_service.h>
#include <datatools/properties.h>
#include <datatools/exception.h>

// Geometry manager :
#include <geomtools/manager.h>

namespace geomtools {

  /** Auto-registration of this service class in a central service Db */
  DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(geometry_service, "geomtools::geometry_service")

  const geomtools::manager & geometry_service::get_geom_manager () const
  {
    DT_THROW_IF (! is_initialized (),
                 std::logic_error,
                 "Service '" << get_name () << "' is not initialized ! ");
    return *_geom_manager_;
  }

  bool geometry_service::is_initialized () const
  {
    return _initialized_;
  }

  int geometry_service::initialize (const datatools::properties & a_config,
                                    datatools::service_dict_type & a_service_dict)
  {
    DT_THROW_IF (is_initialized (),
                 std::logic_error,
                 "Service '" << get_name () << "' is already initialized ! ");
    bool debug = false;
    if (a_config.has_flag ("debug")) {
      debug = true;
    }

    std::string geometry_manager_configuration_file;
    if (a_config.has_key ("manager.configuration_file")) {
      geometry_manager_configuration_file
        = a_config.fetch_string ("manager.configuration_file");
    } else {
      DT_THROW_IF(true,
                  std::logic_error,
                  "Missing '" << "manager.configuration_file"
                  << "' property for service '" << get_name () << "' !");
    }

    datatools::fetch_path_with_env (geometry_manager_configuration_file);
    datatools::properties geometry_manager_config;
    datatools::properties::read_config (geometry_manager_configuration_file,
                                        geometry_manager_config);
    bool manager_build_mapping = false;
    if (a_config.has_flag ("manager.build_mapping")) {
      manager_build_mapping = true;
    }
    if (manager_build_mapping) {
      geometry_manager_config.update ("build_mapping", true);
    }

    bool manager_no_excluded_categories = false;
    if (a_config.has_flag ("manager.no_excluded_categories")) {
      manager_no_excluded_categories = true;
    }
    if (manager_no_excluded_categories) {
      if (geometry_manager_config.has_key ("mapping.excluded_categories")) {
        geometry_manager_config.erase ("mapping.excluded_categories");
      }
    }

    _geom_manager_ = new geomtools::manager;
    _geom_manager_->initialize (geometry_manager_config);

    _initialized_ = true;
    return EXIT_SUCCESS;
  }

  int geometry_service::reset ()
  {
    DT_THROW_IF (! is_initialized (),
                 std::logic_error,
                 "Geometry service '" << get_name () << "' is not initialized ! ");
    if (_geom_manager_ != 0) {
      delete _geom_manager_;
      _geom_manager_ = 0;
    }
    _initialized_ = false;
    return EXIT_SUCCESS;
  }

  geometry_service::geometry_service ()
    : datatools::base_service ("geomtools::geometry_service")
  {
    _initialized_ = false;
    _geom_manager_ = 0;
    return;
  }

  // dtor:
  geometry_service::~geometry_service ()
  {
    if (this->geometry_service::is_initialized ()) {
      this->geometry_service::reset ();
    }
    return;
  }

  void geometry_service::tree_dump (std::ostream & a_out ,
                                    const std::string & a_title,
                                    const std::string & a_indent,
                                    bool a_inherit) const
  {
    this->base_service::tree_dump (a_out, a_title, a_indent, true);
    a_out << a_indent << datatools::i_tree_dumpable::inherit_tag (a_inherit)
          << "Geometry manager : " << _geom_manager_ << " " << std::endl;
    return;
  }

}  // end of namespace geomtools


// end of geometry_service.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

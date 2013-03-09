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

// Geometry manager :
#include <geomtools/manager.h>

namespace geomtools {

      using namespace std;

      /** Auto-registration of this service class in a central service Db */
      DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(geometry_service, "geomtools::geometry_service")

      const geomtools::manager & geometry_service::get_geom_manager () const
      {
        if (! is_initialized ())
          {
            ostringstream message;
            message << "geomtools::geometry_service::get_geom_manager: "
                    << "Service '" << get_name () << "' is not initialized ! ";
            throw logic_error (message.str ());
          }
        return *_geom_manager_;
      }

      bool geometry_service::is_initialized () const
      {
        return _initialized_;
      }

      int geometry_service::initialize (const datatools::properties & a_config,
                                        datatools::service_dict_type & a_service_dict)
      {
        if (is_initialized ())
          {
            ostringstream message;
            message << "geomtools::geometry_service::initialize: "
                    << "Service '" << get_name () << "' is already initialized ! ";
            throw logic_error (message.str ());
          }

        bool debug = false;
        if (a_config.has_flag ("debug"))
          {
            debug = true;
          }

        string geometry_manager_configuration_file;
        if (a_config.has_key ("manager.configuration_file"))
          {
            geometry_manager_configuration_file
              = a_config.fetch_string ("manager.configuration_file");
          }
        else
          {
            ostringstream message;
            message << "geomtools::geometry_service::initialize: "
                    << "Missing '" << "manager.configuration_file"
                    << "' property for service '" << get_name () << "' ! ";
            throw logic_error (message.str ());
          }

        datatools::fetch_path_with_env (geometry_manager_configuration_file);
        datatools::properties geometry_manager_config;
        datatools::properties::read_config (geometry_manager_configuration_file,
                                                   geometry_manager_config);
        bool manager_build_mapping = false;
        if (a_config.has_flag ("manager.build_mapping"))
          {
            manager_build_mapping = true;
          }
        if (manager_build_mapping)
          {
            geometry_manager_config.update ("build_mapping", true);
          }

        bool manager_no_excluded_categories = false;
        if (a_config.has_flag ("manager.no_excluded_categories"))
          {
            manager_no_excluded_categories = true;
          }
        if (manager_no_excluded_categories)
          {
            if (geometry_manager_config.has_key ("mapping.excluded_categories"))
              {
                geometry_manager_config.erase ("mapping.excluded_categories");
              }
          }

        _geom_manager_ = new geomtools::manager;
        _geom_manager_->set_debug (debug);
        _geom_manager_->initialize (geometry_manager_config);

        _initialized_ = true;
        return EXIT_SUCCESS;
      }

      int geometry_service::reset ()
      {
        if (! is_initialized ())
          {
            ostringstream message;
            message << "geomtools::geometry_service::reset: "
                    << "Service '" << get_name () << "' is not initialized ! ";
            throw logic_error (message.str ());
          }
        if (_geom_manager_ != 0)
          {
            delete _geom_manager_;
            _geom_manager_ = 0;
          }
        _initialized_ = false;
        return EXIT_SUCCESS;
      }

      // ctor:
      geometry_service::geometry_service ()
        : datatools::base_service ("geomtools::geometry",
                                   "Geometry service")
      {
        _initialized_ = false;
        _geom_manager_ = 0;
        return;
      }

      // dtor:
      geometry_service::~geometry_service ()
      {
        if (this->geometry_service::is_initialized ())
          {
            this->geometry_service::reset ();
          }
        return;
      }

      void geometry_service::tree_dump (ostream & a_out ,
                                        const string & a_title,
                                        const string & a_indent,
                                        bool a_inherit) const
      {
        namespace du = datatools;
        this->base_service::tree_dump (a_out, a_title, a_indent, true);

        a_out << a_indent << du::i_tree_dumpable::inherit_tag (a_inherit)
              << "Geometry manager : " << _geom_manager_ << " " << endl;

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

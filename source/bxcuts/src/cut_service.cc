/* cut_service.cc
 *
 * Copyright (C) 2011-2015 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#include <cuts/cut_service.h>

// Standard library:
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/exception.h>

// This project:
#include <cuts/cut_manager.h>

namespace cuts {

  /// Auto-registration of this service class in a central service Db
  DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(cut_service, "cuts::cut_service")

  bool cut_service::owns_cut_manager() const
  {
    return _cut_manager_ != 0 && _owns_manager_;
  }

  cut_manager & cut_service::grab_cut_manager()
  {
    DT_THROW_IF(_cut_manager_ == 0,
                std::logic_error,
                "No embedded cut manager is defined !");
    return *_cut_manager_;
  }

  const cut_manager & cut_service::get_cut_manager() const
  {
    DT_THROW_IF(_cut_manager_ == 0,
                std::logic_error,
                "No embedded cut manager is defined !");
    return *_cut_manager_;
  }

  void cut_service::set_cut_manager(const cut_manager & a_cut_manager)
  {
    DT_THROW_IF(&a_cut_manager == _cut_manager_,
                std::logic_error,
                "Self-referenced manager is not allowed !");
    if(_cut_manager_ != 0 && _owns_manager_) {
      delete _cut_manager_;
    }
    _cut_manager_ = 0;
    _owns_manager_ = false;
    _cut_manager_ = const_cast<cut_manager*>(&a_cut_manager);
    return;
  }

  bool cut_service::is_initialized() const
  {
    return _cut_manager_ != 0;
  }

  int cut_service::initialize(const datatools::properties & a_config,
                              datatools::service_dict_type& /*a_service_dict*/)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Service '" << get_name() << "' is already initialized !");

    datatools::base_service::_common_initialize(a_config);

    if(_cut_manager_ == 0) {
      DT_THROW_IF(! a_config.has_key("cut_manager.config"),
                  std::logic_error,
                  "Missing '" << "cut_manager.config"<< "' property !");
      std::string config_filename = a_config.fetch_string("cut_manager.config");
      datatools::properties cut_manager_config;
      datatools::fetch_path_with_env(config_filename);

      datatools::read_config(config_filename,
                                         cut_manager_config);
      _cut_manager_ = new cut_manager;
      _owns_manager_ = true;
      _cut_manager_->initialize(cut_manager_config);
    }

    return EXIT_SUCCESS;
  }

  int cut_service::reset()
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "Cut service '" << get_name() << "' is not initialized ! ");
    if(_owns_manager_ && _cut_manager_ != 0) {
      if(_cut_manager_->is_initialized()) _cut_manager_->reset();
      delete _cut_manager_;
      _cut_manager_ = 0;
    }
    _owns_manager_ = false;
    return EXIT_SUCCESS;
  }

  cut_service::cut_service()
  {
    _owns_manager_ = false;
    _cut_manager_ = 0;
    return;
  }

  cut_service::~cut_service()
  {
    if(cut_service::is_initialized()) {
      this->cut_service::reset();
    }
    return;
  }

  void cut_service::tree_dump(std::ostream & a_out ,
                              const std::string & a_title,
                              const std::string & a_indent,
                              bool a_inherit) const
  {
    this->base_service::tree_dump(a_out, a_title, a_indent, true);
    a_out << a_indent << datatools::i_tree_dumpable::tag
          << "Owns manager : '" << _owns_manager_ << "'" << std::endl;
    a_out << a_indent << datatools::i_tree_dumpable::inherit_tag(a_inherit)
          << "Cut manager  :  " << _cut_manager_ << std::endl;
    return;
  }

}  // end of namespace cuts


/***************
 * OCD support *
 ***************/

#include <datatools/ocd_macros.h>

// OCD support for class '::cuts::cut_service' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::cuts::cut_service,ocd_)
{
  ocd_.set_class_name("cuts::cut_service");
  ocd_.set_class_description("A cut service");
  ocd_.set_class_library("cuts");
  ocd_.set_class_documentation("The ``cut_service`` object inherits the ``datatools::base_service``\n"
                               "interface and is thus usable from a ``datatools::service_manager``\n"
                               "object.                                                           \n"
                               "A cut service object embeds a ``cut_manager`` object and makes it \n"
                               "available through the Bayeux's datatools services support.        \n"
                               );

  ::datatools::base_service::common_ocd(ocd_);

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("cut_manager.config")
      .set_terse_description("The name of the configuration file for the embedded cut manager")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_mandatory(false)
      .set_long_description("This is the name of the main configuration file for   \n"
                            "the embedded cut manager.                             \n"
                            "This property is only used if no external cut manager \n"
                            "has been defined through a prior call to the          \n"
                            "``cuts::cut_service::set_cut_manager`` method.        \n"
                            "The filename may contain some environment variables.  \n"
                            )
      .add_example("Specify the manager configuration file: ::                     \n"
                   "                                                               \n"
                   "    cut_manager.config : string as path = \"cut_manager.conf\" \n"
                   "                                                               \n"
                   )
      ;
  }

  ocd_.set_configuration_hints("The cut service uses a 'datatools::properties' object                \n"
                               "to initialize its behaviour and contents.                            \n"
                               "                                                                     \n"
                               "Example::                                                            \n"
                               "                                                                     \n"
                               "   logging.priority   : string = \"warning\"                         \n"
                               "   cut_manager.config : string as path = \"my_cut_mgr.conf\"         \n"
                               "                                                                     \n"
                               "See dedicated OCD support for the ``cuts::cut_manager`` class        \n"
                               "and ``cuts::XXX_cut`` classes for a description of the syntax        \n"
                               "used to instantiate cuts objects from a cut manager.                 \n"
                               "                                                                     \n"
                               "From a service manager (``datatools::service_manager`` class)        \n"
                               "one uses the following syntax from a ``datatools::multi_properties`` \n"
                               "config file::                                                        \n"
                               "                                                                     \n"
                               "   #@key_label   \"name\"                                            \n"
                               "   #@meta_label  \"type\"                                            \n"
                               "                                                                     \n"
                               "   [name=\"my_cuts\" type=\"cuts::cut_service\"]                     \n"
                               "   logging.priority   : string = \"warning\"                         \n"
                               "   cut_manager.config : string as path = \"my_cut_mgr.conf\"         \n"
                               "                                                                     \n"
                               );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()

DOCD_CLASS_SYSTEM_REGISTRATION(::cuts::cut_service,"cuts::cut_service")

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

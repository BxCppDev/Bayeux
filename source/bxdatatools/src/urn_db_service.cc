// datatools/urn_db_service.cc
//
// Copyright (c) 2017 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of datatools.
//
// datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools. If not, see <http://www.gnu.org/licenses/>.

// Ourselves:
#include <datatools/urn_db_service.h>

// Standard library:
#include <sstream>
#include <fstream>

// This project:
#include <datatools/exception.h>
#include <datatools/multi_properties.h>
#include <datatools/utils.h>
#include <datatools/urn.h>
#include <datatools/urn_query_service.h>
#include <datatools/kernel.h>

namespace datatools {

  /** Auto-registration of this service class in a central service Db */
  DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(urn_db_service, "datatools::urn_db_service")

  urn_db_service::urn_db_service()
  {
    return;
  }

  urn_db_service::~urn_db_service()
  {
    if (this->is_initialized()) this->urn_db_service::reset();
    return;
  }

  bool urn_db_service::is_initialized() const
  {
    return _initialized_;
  }

  int urn_db_service::initialize(const datatools::properties & config_,
                                 datatools::service_dict_type& /*services_*/)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Service is already initialized!");
    base_service::common_initialize(config_);

    {
      std::vector<std::string> leaves_csv_filenames;
      if (config_.has_key("urn_infos.csv_leaves")) {
        config_.fetch("urn_infos.csv_leaves", leaves_csv_filenames);
      }
      for (const std::string & filename : leaves_csv_filenames) {
        std::string file = filename;
        datatools::fetch_path_with_env(file);
        load_csv_leaves(file);
      }
    }

    {
      std::vector<std::string> urn_infos_def_filenames;
      if (config_.has_key("urn_infos.definitions")) {
        config_.fetch("urn_infos.definitions", urn_infos_def_filenames);
      }
      for (const std::string & filename : urn_infos_def_filenames) {
        std::string file = filename;
        datatools::fetch_path_with_env(file);
        load(file);
      }
    }

    _init_();
    _initialized_ = true;
    // Publish the service in the Kernel's URN system singleton:
    if (config_.has_flag("kernel.push")) {
      std::string name;
      if (config_.has_key("kernel.push.name")) {
        name = config_.fetch_string("kernel.push.name");
      }
      kernel_push(name);
    }
    return datatools::SUCCESS;
  }

  int urn_db_service::reset()
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Service is not initialized!");
    kernel_pop();
    _initialized_ = false;
    _urn_infos_.clear();
    return datatools::SUCCESS;
  }

  void urn_db_service::tree_dump(std::ostream& out_,
                                 const std::string& title_,
                                 const std::string& indent_,
                                 bool inherit_) const
  {
    this->base_service::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "URN infos  : "
         << _urn_infos_.size() << std::endl;
    for (urn_info_dict_type::const_iterator i = _urn_infos_.begin();
         i != _urn_infos_.end();
         i++) {
      out_ << indent_ << i_tree_dumpable::inherit_skip_tag(inherit_);
      std::ostringstream indent2;
      indent2 << indent_ << i_tree_dumpable::inherit_skip_tag(inherit_);
      // indent2 << i_tree_dumpable::last_skip_tag;
      urn_info_dict_type::const_iterator j = i;
      j++;
      if (j == _urn_infos_.end()) {
        out_ << i_tree_dumpable::last_tag;
        indent2 << i_tree_dumpable::last_skip_tag;
      } else {
        out_ << i_tree_dumpable::tag;
        indent2 << i_tree_dumpable::skip_tag;
      }
      out_ << "URN '" << i->first << "' : " << std::endl;
      i->second.tree_dump(out_, "", indent2.str());
    }

    return;
  }

  void urn_db_service::load(const std::string & filename_)
  {
    datatools::multi_properties mp("urn", "category");
    DT_LOG_DEBUG(get_logging_priority(), "Loading file '" << filename_ << "' with items' URN infos...");
    mp.read(filename_);
    std::vector<std::string> urns;
    mp.ordered_keys(urns);
    for (const std::string & urn : urns) {
      const datatools::multi_properties::entry & mpe = mp.get(urn);
      const std::string & category = mpe.get_meta();
      const datatools::properties & props = mpe.get_properties();
      urn_info ui;
      ui.set_urn(urn);
      ui.set_category(category);
      ui.initialize(props);
      add(ui);
    }
    return;
  }

  void urn_db_service::load_csv_leaves(const std::string & filename_)
  {
    std::string csvfile = filename_;
    datatools::fetch_path_with_env(csvfile);
    DT_LOG_DEBUG(get_logging_priority(), "Loading CSV file '" << csvfile << "' with leaf items' URN infos...");
    std::ifstream fcsv(csvfile.c_str());
    fcsv >> std::ws;
    std::string default_category;
    while (fcsv && !fcsv.eof()) {
      std::string line;
      std::getline(fcsv, line);
      {
        // Skip blank lines and lines starting with '#':
        std::istringstream testline(line);
        std::string word;
        testline >> word;
        if (word.empty()) continue;
        if (word[0] == '#') {
          if (word == "#@category") {
            std::string ignore;
            testline >> default_category >> std::ws;
            if (!testline.eof()) {
              std::getline(testline, ignore);
            }
          }
          continue;
        }
      }
      std::string urn_key;
      std::string urn_category;
      std::string urn_description;
      std::istringstream linein(line);
      linein >> urn_key >> std::ws;
      if (! linein.eof()) {
        linein >> urn_category >> std::ws;
      } else {
        urn_category = default_category;
      }
      std::getline(linein, urn_description);
      urn_info leaf_ui(urn_key, urn_category, urn_description);
      add(leaf_ui);
    }
    return;
  }

  void urn_db_service::_init_()
  {
    // nothing special...
    return;
  }

  bool urn_db_service::has(const std::string & urn_) const
  {
    return _urn_infos_.count(urn_) == 1;
  }

  void urn_db_service::add(const urn_info & uinfo_)
  {
    _urn_infos_[uinfo_.get_urn()] = uinfo_;
    return;
  }

  const urn_info & urn_db_service::get(const std::string & urn_) const
  {
    urn_info_dict_type::const_iterator found = _urn_infos_.find(urn_);
    DT_THROW_IF(found == _urn_infos_.end(), std::logic_error,
                "URN database has no URN '" << urn_ << "'!");
    return found->second;
  }

  std::size_t urn_db_service::get_number_of_urn_infos() const
  {
    return _urn_infos_.size();
  }

  const urn_db_service::urn_info_dict_type & urn_db_service::get_urn_infos() const
  {
    return _urn_infos_;
  }

  void urn_db_service::kernel_push(const std::string & name_)
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Cannot register in the library's URN query service if not initialized!");
    datatools::kernel::instance().grab_urn_query().add_db(*this, name_);
    return;
  }

  void urn_db_service::kernel_pop()
  {
    if (datatools::kernel::instance().get_urn_query().has_db(*this)) {
      datatools::kernel::instance().grab_urn_query().remove_db(*this);
    }
    return;
  }

} // end of namespace datatools

/***************
 * OCD support *
 ***************/

#include <datatools/ocd_macros.h>

// OCD support for class '::datatools::urn_db_service' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::datatools::urn_db_service, ocd_)
{
  ocd_.set_class_name("datatools::urn_db_service");
  ocd_.set_class_description("URN information database service");
  ocd_.set_class_library("datatools");
  ocd_.set_class_documentation("A *service* which host a dictionary of items addressed  \n"
                               "though their unique URNs and classified in *categories* \n");


  // Invoke OCD support from parent class :
  ::datatools::base_service::common_ocd(ocd_);

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("urn_infos.csv_leaves")
      .set_terse_description("A  list of CSV formatted files which contains leaf URN definitions")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_path(true)
      .set_mandatory(false)
      .set_long_description("Each file use a CSV ASCII format:  \n"
                            "                                                                 \n"
                            "Example:                                                         \n"
                            "                                                                 \n"
                            "#@category data                                                  \n"
                            "urn:fooexp:data:run:0                                            \n"
                            "urn:fooexp:data:run:1                                            \n"
                            "urn:fooexp:data:run:2                                            \n"
                            "urn:fooexp:data:run:3                                            \n"
                            "urn:fooexp:data:run:4                                            \n"
                            "                                                                 \n"
                            "urn:fooexp:data:metadata:1.0 metadata Metadata associated to the runs\n"
                            "urn:fooexp:data:run:5                                            \n"
                            "urn:fooexp:data:run:6                                            \n"
                            "urn:fooexp:data:run:7                                            \n"
                            "urn:fooexp:data:run:7                                            \n"
                            "                                                                 \n"
                            )
      .add_example("Use an explicit list of leaf URNs CSV formatted files::      \n"
                   "                                                             \n"
                   "    urn_infos.csv_leaves : string[2] as path = \\            \n"
                   "      \"${DATA_REPOSITORY_DIR}/fooexp/phase1/runs.lis\" \\   \n"
                   "      \"${DATA_REPOSITORY_DIR}/fooexp/phase2/runs.lis\"      \n"
                   "                                                             \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("urn_infos.defintions")
      .set_terse_description("A  list of files which contains definitions of URN informations")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_path(true)
      .set_mandatory(false)
      .set_long_description("Each file use the ``datatools::multi_properties`` format:        \n"
                            "                                                                 \n"
                            "Example:                                                         \n"
                            "                                                                 \n"
                            "#@description Definition list for URN infos                      \n"
                            "#@key_label   \"urn\"                                            \n"
                            "#@meta_label  \"category\"                                       \n"
                            "                                                                 \n"
                            "[urn=\"urn:bayeux:images:logo:small\" category=\"graphics\"]     \n"
                            "description : string = \"Bayeux logo (small)\"                   \n"
                            "                                                                 \n"
                            "[urn=\"urn:bayeux:images:logo:large\" category=\"graphics\"]     \n"
                            "description : string = \"Bayeux logo (large)\"                   \n"
                            "                                                                 \n"
                            "                                                                 \n"
                            )
      .add_example("Use an explicit list of URNs definition files::                            \n"
                            "                                                                  \n"
                            "    urn_infos.definitions : string[2] as path = \\                \n"
                            "      \"${DATA_REPOSITORY_DIR}/fooexp/config/simulation.defs\" \\ \n"
                            "      \"${DATA_REPOSITORY_DIR}/fooexp/config/calibration.defs\"   \n"
                            "                                                                  \n")
                            ;
  }

  ocd_.set_configuration_hints("A geometry manager is configured through a configuration file that  \n"
                               "uses the format of 'datatools::properties' setup file.              \n"
                               "Example: ::         .                                               \n"
                               "  logging.priority : string = \"warning\"                           \n"
                               "  name : string = \"Foo\"                                           \n"
                               "    urn_infos.csv_leaves : string[2] as path = \\                   \n"
                               "      \"${DATA_REPOSITORY_DIR}/fooexp/phase1/runs.lis\" \\          \n"
                               "      \"${DATA_REPOSITORY_DIR}/fooexp/phase2/runs.lis\"             \n"
                               "    urn_infos.defintions : string[2] as path = \\                   \n"
                               "      \"${DATA_REPOSITORY_DIR}/fooexp/config/simulation.defs\" \\   \n"
                               "      \"${DATA_REPOSITORY_DIR}/fooexp/config/calibration.defs\"     \n"
                             );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}

DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::datatools::urn_db_service,"datatools::urn_db_service")

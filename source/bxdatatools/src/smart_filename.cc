/* smart_filename.cc
 *
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *
 *
 */
// Ourselves
#include <datatools/smart_filename.h>

// Standard Library
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

// Third Party
// - Boost
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

// This Project
#include <datatools/ioutils.h>
#include <datatools/utils.h>
#include <datatools/exception.h>
#include <datatools/logger.h>

namespace datatools {

using namespace std;

const std::string & smart_filename::labels::mode_single()
{
  static std::string label;
  if (label.empty()) label = "single";
  return label;
}


const std::string & smart_filename::labels::mode_list()
{
  static std::string label;
  if (label.empty()) label = "list";
  return label;
}


const std::string & smart_filename::labels::mode_incremental()
{
  static std::string label;
  if (label.empty()) label = "incremental";
  return label;
}


bool smart_filename::is_debug() const {
  return debug_;
}


void smart_filename::set_debug(bool a_new_value) {
  debug_ = a_new_value;
}


int smart_filename::get_mode() const {
  return mode_;
}


bool smart_filename::is_initialized() const {
  return (mode_ != MODE_INVALID);
}


bool smart_filename::is_expand_path() const {
  return expand_path_;
}


bool smart_filename::is_single() const {
  return (mode_ == MODE_SINGLE);
}


bool smart_filename::is_list() const {
  return (mode_ == MODE_LIST);
}


bool smart_filename::is_incremental() const {
  return (mode_ == MODE_INCREMENTAL);
}


bool smart_filename::is_ranged() const {
  return ranged_;
}


bool smart_filename::is_valid() const {
  if (!this->is_initialized()) return false;
  if (this->is_ranged()) {
    return list_.size() > 0;
  }
  return true;
}


size_t smart_filename::current_size() const {
  return list_.size();
}


bool smart_filename::has_filename(const std::string& filename_,
                                  bool expand_path_) const {
  std::string fname = filename_;
  if (expand_path_) {
    datatools::fetch_path_with_env(fname);
  }
  if (!this->is_initialized()) {
    return false;
  }
  if (this->is_ranged() || list_.size () > 0) {
    return find(list_.begin(), list_.end(), fname) != list_.end();
  }
  return false;
}


size_t smart_filename::size() const {
  DT_THROW_IF (!this->is_ranged(),
               std::logic_error,
               "List of filenames is not ranged ! Size is not known in advance !");
  return list_.size();
}


smart_filename::const_iterator smart_filename::begin() const {
  return list_.begin();
}


smart_filename::const_iterator smart_filename::end() const {
  DT_THROW_IF (!ranged_,
               std::logic_error,
               "List of filenames is not ranged (incremental mode with no stopping index) !");
  return list_.end();
}


const std::string& smart_filename::operator[](int a_index) const {
  return this->get_filename(a_index);
}


const std::string& smart_filename::get_filename(int a_index) const {
  DT_THROW_IF (!this->is_valid(), std::logic_error, "Not valid !");
  DT_THROW_IF (a_index < 0, std::range_error, "Index " << a_index << " is not valid !");
  if (this->is_incremental()
      && ! this->is_ranged()
      && (a_index >= (int)list_.size())) {
    smart_filename* mutable_this = const_cast<smart_filename*>(this);
    for (int i = list_.size(); i <= a_index; ++i) {
      string filename;
      int file_number = incremental_starting_index_ +  incremental_increment_ * a_index;
      this->build_incremental_filename(file_number, filename);
      mutable_this->add_list(filename);
    }
  } else {
    DT_THROW_IF (a_index < 0 || a_index > (int)list_.size(),
                 std::range_error,
                 "Index " << a_index<< " is not valid !");
  }
  return list_[a_index];
}


void smart_filename::set_mode(int a_new_value) {
  mode_ = a_new_value;
  if (mode_ == MODE_SINGLE) {
    ranged_ = true;
  } else if (mode_ == MODE_LIST) {
    ranged_ = true;
  } else if (mode_ == MODE_INCREMENTAL) {
    ranged_ = false;
    if (incremental_stopping_index_ > MODE_INCREMENTAL_UNRANGED) {
      ranged_ = true;
    }
  } else {
    ranged_ = false;
  }
}


void smart_filename::set(const string& a_new_value) {
  DT_THROW_IF (!this->is_single(),std::logic_error,"Not using 'single' mode !");
  list_.clear();
  this->add_list(a_new_value);
}


void smart_filename::set_single(const std::string& a_filename) {
  this->set(a_filename);
}


void smart_filename::reset() {
  mode_ = MODE_INVALID;
  ranged_ = false;
  expand_path_ = true;
  incremental_starting_index_ = 0;
  incremental_stopping_index_ = MODE_INCREMENTAL_UNRANGED;
  incremental_increment_      = MODE_INCREMENTAL_DEFAULT_INCREMENT;
  incremental_prefix_ = "";
  incremental_suffix_ = "";
  incremental_extension_ = "";
  incremental_path_ = "";
  incremental_index_ndigit_ = 0;
  list_.clear();
  list_allow_duplication_ = false;
}


void smart_filename::set_list_allow_duplication(bool a_new_value) {
  list_allow_duplication_ = a_new_value;
}


void smart_filename::add_list(const string& a_filename) {
  DT_THROW_IF (a_filename.empty(),logic_error,"Missing filename !");
  DT_THROW_IF (this->is_single() && (list_.size() > 0),
               logic_error,
               "Cannot add a filename ('single' mode) !");
  string filename = a_filename;
  if (this->is_expand_path()) {
    datatools::fetch_path_with_env(filename);
  }
  if ((list_.size() > 0) && !list_allow_duplication_) {
    DT_THROW_IF (std::find(list_.begin(), list_.end(), filename) != list_.end(),
                 logic_error,
                 "Duplication error: filename '" << filename << "' is already in the list !");
  }
  list_.push_back(filename);
}


void smart_filename::add(const string& a_filename) {
  DT_THROW_IF (!this->is_list(),
               logic_error,
               "Not using 'list' mode !");
  this->add_list(a_filename);
}


void smart_filename::add_to_list(const std::string& a_filename) {
  this->add(a_filename);
}


// Constructor:
smart_filename::smart_filename() {
  debug_ = false;
  this->reset();
}


// Destructor:
smart_filename::~smart_filename() {
  this->reset();
}


// static
void smart_filename::make_single(smart_filename& a_smart_filename,
                                 const string& a_filename,
                                 bool a_expand_path) {
  a_smart_filename.reset();
  a_smart_filename.ranged_ = true;
  a_smart_filename.set_mode(MODE_SINGLE);
  a_smart_filename.expand_path_ = a_expand_path;
  a_smart_filename.add_list(a_filename);
}


void smart_filename::make_list(smart_filename& a_smart_filename,
                                bool a_allow_duplication,
                                bool a_expand_path) {
  a_smart_filename.reset();
  a_smart_filename.ranged_ = true;
  a_smart_filename.set_mode(MODE_LIST);
  a_smart_filename.set_list_allow_duplication (a_allow_duplication);
  a_smart_filename.expand_path_ = a_expand_path;
}


void smart_filename::make_list(smart_filename& a_smart_filename,
                               const string& a_list_filename,
                               bool a_allow_duplication,
                               bool a_expand_path) {
  a_smart_filename.reset();
  a_smart_filename.ranged_ = true;
  a_smart_filename.set_mode(MODE_LIST);
  a_smart_filename.set_list_allow_duplication(a_allow_duplication);
  a_smart_filename.expand_path_ = a_expand_path;
  string list_filename = a_list_filename;
  datatools::fetch_path_with_env(list_filename);
  DT_THROW_IF (!boost::filesystem::exists(list_filename),
               logic_error,
               "File '" << a_list_filename << "' does not exists !");
  DT_THROW_IF (boost::filesystem::is_directory(list_filename),
               logic_error,
               "Path '" << list_filename << "' is a directory !");
  ostringstream message;
  message << "datatools::smart_filename::make_list: "
          << "Reading file '"
          << list_filename << "'...";
  DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,message.str());
  ifstream inlist(list_filename.c_str());
  while (inlist) {
    string line;
    getline(inlist, line);
    if (!line.empty()) {
      istringstream iss(line);
      string word;
      iss >> word;
      // skipping blank lines :
      if (!word.empty()) {
        // comment line :
        if (word[0] == '#'){
          continue;
        } else {
          // add a filename in the list :
          a_smart_filename.add(word);
        }
      }
    }
    if (inlist.eof()) break;
  }
}


void smart_filename::make_unranged_incremental(smart_filename& a_smart_filename,
                                                const string& a_path,
                                                const string& a_prefix,
                                                const string& a_extension,
                                                int a_starting_index,
                                                int a_increment_index,
                                                const string & a_suffix,
                                                int a_incremental_index_ndigit,
                                                bool a_expand_path) {
  make_incremental(a_smart_filename, a_path, a_prefix, a_extension,
                    MODE_INCREMENTAL_UNRANGED, a_starting_index,
                    a_increment_index, a_suffix,
                    a_incremental_index_ndigit, a_expand_path);
}


void smart_filename::make_incremental(smart_filename & a_smart_filename,
                                      const string & a_path,
                                      const string & a_prefix,
                                      const string & a_extension,
                                      int a_stopping_index,
                                      int a_starting_index,
                                      int a_increment_index,
                                      const string & a_suffix,
                                      int a_incremental_index_ndigit,
                                      bool a_expand_path) {
  a_smart_filename.reset();
  a_smart_filename.set_mode(MODE_INCREMENTAL);
  a_smart_filename.ranged_ = false;
  a_smart_filename.expand_path_ = a_expand_path;
  DT_THROW_IF (a_prefix.empty(),
               logic_error,
               "Missing prefix !");
  a_smart_filename.incremental_path_ = a_path;
  a_smart_filename.incremental_prefix_ = a_prefix;
  a_smart_filename.incremental_extension_ = a_extension;
  a_smart_filename.incremental_suffix_ = a_suffix;
  a_smart_filename.incremental_index_ndigit_ = a_incremental_index_ndigit;
  DT_THROW_IF (a_increment_index == 0,logic_error,"Invalid null increment !");
  string path;
  if (!a_path.empty()) {
    path = a_smart_filename.incremental_path_;
    if (a_smart_filename.is_expand_path()) {
      datatools::fetch_path_with_env(path);
      a_smart_filename.incremental_path_ = path;
    }
  }
  a_smart_filename.incremental_starting_index_ = a_starting_index;
  a_smart_filename.incremental_stopping_index_ = a_stopping_index;
  a_smart_filename.incremental_increment_ = a_increment_index;
  a_smart_filename.ranged_ = false;
  if (a_smart_filename.incremental_stopping_index_
      <= MODE_INCREMENTAL_UNRANGED) {
    a_smart_filename.incremental_stopping_index_ = MODE_INCREMENTAL_UNRANGED;
    if (a_smart_filename.incremental_increment_ < 0) {
      a_smart_filename.incremental_stopping_index_ = 0;
      a_smart_filename.ranged_ = true;
    }
  } else {
    a_smart_filename.ranged_ = true;
  }

  if (a_smart_filename.incremental_stopping_index_
      > MODE_INCREMENTAL_UNRANGED) {
    if (a_smart_filename.incremental_stopping_index_
        < a_smart_filename.incremental_starting_index_) {
      DT_THROW_IF (a_smart_filename.incremental_increment_ >= 0,
                   logic_error,
                   "Invalid increment rule (start="
                   << a_smart_filename.incremental_starting_index_
                   << ",stop="
                   << a_smart_filename.incremental_stopping_index_
                   << ",increment="
                   << a_smart_filename.incremental_increment_ << ") !");
    }
  }

  int index = a_smart_filename.incremental_starting_index_;
  int direction = +1;
  if (a_smart_filename.incremental_increment_ < 0) direction = -1;
  if (a_smart_filename.is_ranged()) {
    while ((direction * index)
           <= (direction * a_smart_filename.incremental_stopping_index_)) {
      std::string filename;
      a_smart_filename.build_incremental_filename(index, filename);
      a_smart_filename.add_list(filename);
      // increment:
      index += a_smart_filename.incremental_increment_;
    }
  }
  //a_smart_filename.dump(std::clog);
}


void smart_filename::build_incremental_filename(
    int increment_index_,
    std::string& filename_) const {
  ostringstream filename_ss;
  if (!incremental_path_.empty()) {
    filename_ss << incremental_path_;
    if (incremental_path_[incremental_path_.length() - 1] != '/') {
      filename_ss << '/';
    }
  }
  filename_ss << incremental_prefix_;
  if (incremental_index_ndigit_ > 0) {
    filename_ss << std::dec << std::setfill('0')
                << std::setw(incremental_index_ndigit_);
  }
  filename_ss << increment_index_;
  if (!incremental_suffix_.empty()) {
    filename_ss << incremental_suffix_;
  }
  if (!incremental_extension_.empty()) {
    if (incremental_extension_[0] != '.') {
      filename_ss << '.';
    }
    filename_ss << incremental_extension_;
  }
  filename_ = filename_ss.str();
}


void smart_filename::print_list_of_filenames(std::ostream& /*a_out*/) const {
  DT_THROW_IF (!this->is_initialized(),logic_error,"Smart file is not initialized !");
}


void smart_filename::store_list_of_filenames(const string& a_list_filename,
                                             bool a_append) const {
  std::ofstream fout;
  std::string list_filename = a_list_filename;
  datatools::fetch_path_with_env(list_filename);
  if (a_append) {
    fout.open(list_filename.c_str(), std::ios::app);
  } else {
    fout.open(list_filename.c_str());
  }
  DT_THROW_IF (!fout, std::runtime_error,"Cannot open file '" << list_filename << "' !");
  if (!a_append) {
    fout << "# a list of filenames : " << std::endl;
  }
  this->print_list_of_filenames(fout);
}

void smart_filename::dump(std::ostream& a_out) const {
  a_out << "smart_filename::dump : " << std::endl;
  a_out << "|-- Debug       : " << debug_ << std::endl;
  if (!this->is_initialized()) {
    a_out << "`-- Initialized : '" << "No" << "'" << std::endl;
  }
  else {
    a_out << "|-- Initialized : '" << "Yes" << "'" << std::endl;
  }
  std::string mode_str = "";
  if (this->is_single()) mode_str = labels::mode_single();
  if (this->is_list()) mode_str = labels::mode_list();
  if (this->is_incremental()) mode_str = labels::mode_incremental();
  a_out << "|-- Mode        : '" << mode_str << "'" << std::endl;
  a_out << "|-- Ranged      : " << ranged_ << std::endl;
  if (!this->is_valid()) {
    a_out << "`-- Valid       : '" << "No" << "'" << std::endl;
  } else {
    a_out << "|-- Valid       : '" << "Yes" << "'" << std::endl;
    a_out << "`-- Current list [" << list_.size() << ']' << std::endl;
    for (int i = 0; i < (int)list_.size(); ++i) {
      a_out << "    ";
      int j = i;
      j++;
      if (j != (int)list_.size ()) {
        a_out << "|-- ";
      } else {
        a_out << "`-- ";
      }
      a_out << "Filename '" << list_[i] << "'" << std::endl;
    }
  }
}


void smart_filename::initialize(const properties& a_config) {
  if (a_config.has_flag("debug")) {
    this->set_debug(true);
  }

  bool expand_path = true;
  if (a_config.has_flag("no_expand_path")) {
    expand_path = false;
  }

  DT_THROW_IF (!a_config.has_key("mode"), std::logic_error, "Missing 'mode' key !");

  std::string mode_str = a_config.fetch_string("mode");

  if (mode_str == labels::mode_single()) {
    DT_THROW_IF (!a_config.has_key("single.filename"),std::logic_error,"Missing 'single.filename' key !");
    std::string single_filename = a_config.fetch_string("single.filename");
    smart_filename::make_single(*this, single_filename, expand_path);
  } else if (mode_str == labels::mode_list()) {
    bool allow_duplicate = false;
    if (a_config.has_flag("list.duplicate")) {
      allow_duplicate = true;
    }
    if (a_config.has_key("list.file")) {
      std::string list_file = a_config.fetch_string("list.file");
      smart_filename::make_list(*this, list_file, allow_duplicate, expand_path);
    } else {
      std::vector<string> list_vec;
      DT_THROW_IF (!a_config.has_key("list.filenames"), std::logic_error, "Missing 'list.filenames' key !");
      smart_filename::make_list(*this, allow_duplicate, expand_path);
      a_config.fetch("list.filenames", list_vec);
      for (int i = 0; i < (int)list_vec.size(); ++i) {
        this->add(list_vec[i]);
      }
    }
  } else if (mode_str == labels::mode_incremental()) {
    std::string incremental_prefix = a_config.fetch_string("incremental.prefix");
    std::string incremental_suffix = "";
    std::string incremental_path;
    std::string incremental_extension;
    int incremental_stopping = MODE_INCREMENTAL_UNRANGED;
    int incremental_starting = MODE_INCREMENTAL_DEFAULT_START;
    int incremental_increment = MODE_INCREMENTAL_DEFAULT_INCREMENT;
    uint32_t incremental_index_ndigit = 0;
    DT_THROW_IF (!a_config.has_key("incremental.prefix"),std::logic_error,"Missing 'incremental.prefix' key !");
    if (a_config.has_key("incremental.suffix")) {
      incremental_suffix = a_config.fetch_string("incremental.suffix");
    }
    if (a_config.has_key("incremental.path")) {
      incremental_path = a_config.fetch_string("incremental.path");
    }
    if (a_config.has_key("incremental.extension")) {
      incremental_extension = a_config.fetch_string("incremental.extension");
    }
    if (a_config.has_key("incremental.increment")) {
      incremental_increment = a_config.fetch_integer("incremental.increment");
      DT_THROW_IF (incremental_increment == 0,std::logic_error,"Invalid null increment value !");
    }
    if (a_config.has_key("incremental.stop")) {
      incremental_stopping = a_config.fetch_integer("incremental.stop");
      DT_THROW_IF (incremental_stopping < -1,std::logic_error,"Invalid negative stopping value !");
    } else {
      // 2012-05-02 FM : allow unranged list of incremented filenames.
      // DT_THROW_IF (incremental_increment > 0,
      //              logic_error,
      //              " Missing 'incremental.stop' key !");
    }
    if (a_config.has_key("incremental.start")) {
      incremental_starting = a_config.fetch_integer("incremental.start");
      DT_THROW_IF (incremental_starting < 0,std::logic_error,"Invalid negative starting value !");
    } else {
      DT_THROW_IF (incremental_increment < 0,std::logic_error,"Missing 'incremental.start' key !");
    }
    smart_filename::make_incremental(*this, incremental_path,
                                      incremental_prefix,
                                      incremental_extension,
                                      incremental_stopping,
                                      incremental_starting,
                                      incremental_increment,
                                      incremental_suffix,
                                      incremental_index_ndigit,
                                      expand_path);
  } else {
    DT_THROW_IF(true,std::logic_error,"Invalid 'mode' key ('" << mode_str << "') !");
  }
}

}  // end of namespace datatools


/***************
 * OCD support *
 ***************/

#include <datatools/ocd_macros.h>

// OCD support for class '::datatools::smart_filename' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::datatools::smart_filename,ocd_)
{
  ocd_.set_class_name ("datatools::smart_filename");
  ocd_.set_class_description ("Smart automated list of filenames");
  ocd_.set_class_library ("datatools");
  ocd_.set_class_documentation ("A ``datatools::smart_filename`` object stores a sorted      \n"
                                "list of filenames.                           \n"
                                "There are three different modes to build     \n"
                                "such a list of filenames :                   \n"
                                "                                             \n"
                                "a. the *single* mode manages one and         \n"
                                "   only one filename explicitely provided by \n"
                                "   the user.                                 \n"
                                "b. the *list* mode manages several filenames \n"
                                "   obtained from an explicit list provided by\n"
                                "   the user.                                 \n"
                                "c. the *incremental* mode manages several    \n"
                                "   filenames that are automatically built    \n"
                                "   from dedicated rules provided by the user.\n"
                                "                                             \n"
                                "All filenames may contains environment variables.\n"
                                "Once initialized, a ``datatools::smart_filename`` object can be \n"
                                "traversed to extract all, or part of, the list of\n"
                                "filenames it contains.                           \n"
                                )
    ;

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("debug")
      .set_terse_description("Flag to activate debugging output")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("no_expand_path")
      .set_terse_description("Flag to inhibit the path shell expansion within filenames")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Allow the path shell expansion: ::                   \n"
                   "                                                     \n"
                   "  no_expand_path : boolean = 0                       \n"
                   "                                                     \n"
                   )
       ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("mode")
      .set_terse_description("The filenames building mode")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("Supported values for the ``mode`` property are :     \n"
                            "                                                     \n"
                            "* ``single``      : handle a unique filename.        \n"
                            "* ``list``        : handle several filenames from    \n"
                            "  a list of filenames.                               \n"
                            "* ``incremental`` : generate a list of filenames     \n"
                            "  with an integer increment.                         \n"
                            "                                                     \n")
      .add_example("Use the single file mode: ::                         \n"
                   "                                                     \n"
                   "   mode : string = \"single\"                        \n"
                   "                                                     \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("single.filename")
      .set_terse_description("The name of the unique file addressed by the *single* mode")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_mandatory(true)
      .set_triggered_by_label("mode", "single")
      .set_long_description("The name of the unique filename in *single* mode.   \n"
                            "The filename may contain some environment variable. \n")
      .add_example(
                   "Set the name of an unique file : ::                          \n"
                   "                                                             \n"
                   "   mode            : string = \"single\"                     \n"
                   "   single.filename : string as path = \"${TMP_DIR}/test.tmp\"\n"
                   "                                                             \n"
                   )
       ;
  }


  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("list.duplicate")
      .set_terse_description("Flag to allow filename duplication")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "list")
      .set_default_value_boolean(false)
      .set_long_description("If set, this flag allows the duplication of a given filename \n"
                            "in the list of filenames.                                    \n"
                            "This behaviour is disabled by default.                       \n")
      .add_example(
                   "Forbid the duplication of filenames: ::                      \n"
                   "                                                             \n"
                   "   mode           : string = \"list\"                        \n"
                   "   list.duplicate : boolean = 0                              \n"
                   "                                                             \n"
                   )
       ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("list.file")
      .set_terse_description("The name of a plain text file that contains an explicit list of filenames")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "list")
      .set_long_description("The specified file stores one filename per line.    \n")
      .add_example("Set the name of the list file ::                             \n"
                   "                                                             \n"
                   "   mode      : string = \"list\"                             \n"
                   "   list.file : string as path = \"my_files.lis\"             \n"
                   "                                                             \n"
                   "The format of the ``my_files.lis`` is: ::                    \n"
                   "                                                             \n"
                   "   # My list of files (a header comment) :                   \n"
                   "   run_1.data                                                \n"
                   "   run_2.data                                                \n"
                   "   run_3a.data                                               \n"
                   "   # run_3b.data # this is a commented line                  \n"
                   "   ${DATA_DIR}/run_4.data                                    \n"
                   "   ${DATA_DIR}/run_5.data                                    \n"
                   "                                                             \n"
                   "   run_6.data                                                \n"
                   "   run.log                                                   \n"
                   "   # the end of the list.                                    \n"
                   "                                                             \n"
                   "                                                             \n"
                   "The filenames may contain some environment variable(s).      \n"
                   "Blank lines are ignored as well as lines starting with ``#``.\n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("list.filenames")
      .set_terse_description("An array of filenames")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_path(true)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "list")
      .set_long_description("The names of the several filenames in *list* mode.      \n"
                            "The filenames may contain some environment variable(s). \n")
      .add_example("Declare three filenames in *list* mode: ::             \n"
                   "                                                           \n"
                   "   mode           : string = \"list\"                      \n"
                   "   list.filenames : string[3] as path = \\                 \n"
                   "     \"${DATA_DIR}/run_0.data\" \\                         \n"
                   "     \"${DATA_DIR}/run_1.data\" \\                         \n"
                   "     \"${DATA_DIR}/run_2.data\"                            \n"
                   "                                                           \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("incremental.prefix")
      .set_terse_description("The filename prefix")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_triggered_by_label("mode", "incremental")
      .set_long_description("The filenames prefix in *incremental* mode.\n")
      .add_example("The prefix of the incremented filenames: ::                \n"
                   "                                                           \n"
                   "   mode               : string = \"incremental\"           \n"
                   "   incremental.prefix : string = \"run_\"                  \n"
                   "                                                           \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("incremental.suffix")
      .set_terse_description("The filename suffix")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "incremental")
      .set_long_description("The filenames suffix in *incremental* mode.       \n"
                            "The default value is empty.                       \n")
     .add_example("The suffix of the incremented filenames: ::                \n"
                  "                                                           \n"
                  "   mode               : string = \"incremental\"           \n"
                  "   incremental.suffix : string = \"_special\"              \n"
                  "                                                           \n"
                  )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("incremental.path")
      .set_terse_description("The files' path")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "incremental")
      .set_long_description("The path for filenames in *incremental* mode.                 \n"
                            "The path may contain some environment variable(s).            \n"
                            "The default value is empty (current working directory).       \n")
      .add_example("The path of the incremented filenames: ::                    \n"
                   "                                                             \n"
                   "   mode             : string = \"incremental\"               \n"
                   "   incremental.path : string as path = \"${TMP_DIR}/data\"   \n"
                   "                                                             \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("incremental.extension")
      .set_terse_description("The filenames' extension")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "incremental")
      .set_long_description("The extension of filenames in *incremental* mode.   \n"
                            "The default value is empty.                         \n")
      .add_example("The extension of the incremented filenames: ::               \n"
                   "                                                             \n"
                   "   mode                  : string = \"incremental\"          \n"
                   "   incremental.extension : string = \".data\"                \n"
                   "                                                             \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("incremental.increment")
      .set_terse_description("The filenames' increment")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "incremental")
      .set_default_value_integer(1)
      .set_long_description("The increment for automatic building of filenames in          \n"
                            "*incremental* mode.                                           \n"
                            "                                                              \n"
                            "Value ``0`` is illegal.                                       \n")
      .add_example("The increment of the incremented filenames: ::                \n"
                   "                                                              \n"
                   "   mode                  : string  = \"incremental\"          \n"
                   "   incremental.increment : integer = 1                        \n"
                   "                                                              \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("incremental.start")
      .set_terse_description("The first incremental index")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "incremental")
      .set_long_description("The first incremental index for building filenames in        \n"
                            "*incremental* mode.                                          \n"
                            "The default value is ``0`` if increment is positive.         \n")
      .add_example("The starting index of the incremented filenames: ::          \n"
                   "                                                             \n"
                   "   mode                  : string  = \"incremental\"         \n"
                   "   incremental.start     : integer = 0                       \n"
                   "   incremental.increment : integer = 1                       \n"
                   "                                                             \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("incremental.stop")
      .set_terse_description("The last incremental index")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "incremental")
      .set_long_description("The last incremental index for building filenames in         \n"
                            "*incremental* mode.                                          \n"
                            "The default value is ``0`` if increment is negative.         \n"
                            )
      .add_example("The stopping index of the incremented filenames: ::          \n"
                   "                                                             \n"
                   "   mode                  : string  = \"incremental\"         \n"
                   "   incremental.increment : integer = 1                       \n"
                   "   incremental.start     : integer = 0                       \n"
                   "   incremental.stop      : integer = 9                       \n"
                   "                                                             \n"
                   )
      .add_example("The stopping index of the incremented filenames: ::          \n"
                   "                                                             \n"
                   "   mode                  : string  = \"incremental\"         \n"
                   "   incremental.increment : integer = -1                      \n"
                   "   incremental.start     : integer =  9                      \n"
                   "   incremental.stop      : integer =  0                      \n"
                   "                                                             \n"
                   )
      ;
  }

  ocd_.set_configuration_hints ("Configuration examples for a ``datatools::smart_filename`` object :\n"
                                "                                                             \n"
                                "***Example 1:***                                             \n"
                                "                                                             \n"
                                "A unique filename: ::                                        \n"
                                "                                                             \n"
                                "   mode            : string = \"single\"                     \n"
                                "   single.filename : string as path = \"${TMP_DIR}/test.tmp\"\n"
                                "                                                             \n"
                                "                                                             \n"
                                "***Example 2:***                                             \n"
                                "                                                             \n"
                                "A list of filenames: ::                                      \n"
                                "                                                             \n"
                                "   mode            : string = \"list\"                       \n"
                                "   list.duplicate  : boolean = 0                             \n"
                                "   list.filenames  : string[2] as path = \\                  \n"
                                "     \"${DATA_DIR}/run_0.data\" \\                           \n"
                                "     \"${DATA_DIR}/run_1.data\" \\                           \n"
                                "     \"${DATA_DIR}/run_2.data\"                              \n"
                                "                                                             \n"
                                "                                                             \n"
                                "***Example 3:***                                             \n"
                                "                                                             \n"
                                "Incremented filenames: ::                                    \n"
                                "                                                             \n"
                                "   mode                  : string = \"incremental\"          \n"
                                "   incremental.path      : string = \"/tmp/${USER}\"         \n"
                                "   incremental.prefix    : string = \"run_\"                 \n"
                                "   incremental.extension : string = \".data\"                \n"
                                "   incremental.increment : integer = 1                       \n"
                                "   incremental.start     : integer = 0                       \n"
                                "   incremental.stop      : integer = 49                      \n"
                                "                                                             \n"
                                "***Example 4:***                                             \n"
                                "                                                             \n"
                                "Unranged incremented filenames (no ``incremental.stop`` index property): ::\n"
                                "                                                             \n"
                                "   mode                  : string = \"incremental\"          \n"
                                "   incremental.path      : string = \"/tmp/${USER}\"         \n"
                                "   incremental.prefix    : string = \"run_\"                 \n"
                                "   incremental.extension : string = \".data\"                \n"
                                "   incremental.increment : integer = 1                       \n"
                                "   incremental.start     : integer = 0                       \n"
                                "                                                             \n"
                                "***Example 5:***                                             \n"
                                "                                                             \n"
                                "Decremented filenames: ::                                    \n"
                                "                                                             \n"
                                "   mode                  : string = \"incremental\"          \n"
                                "   incremental.path      : string = \"/tmp/${USER}\"         \n"
                                "   incremental.prefix    : string = \"run_\"                 \n"
                                "   incremental.extension : string = \".data\"                \n"
                                "   incremental.increment : integer = -1                      \n"
                                "   incremental.start     : integer = 6                       \n"
                                "   incremental.stop      : integer = 0                       \n"
                                "                                                             \n"
                                );
  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()

DOCD_CLASS_SYSTEM_REGISTRATION(datatools::smart_filename, "datatools::smart_filename")

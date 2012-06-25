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
#include <datatools/utils/smart_filename.h>

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
#include <datatools/utils/ioutils.h>
#include <datatools/utils/utils.h>

namespace datatools {

namespace utils {

using namespace std;

const string smart_filename::MODE_SINGLE_LABEL      =  "single";
const string smart_filename::MODE_LIST_LABEL        =  "list";
const string smart_filename::MODE_INCREMENTAL_LABEL =  "incremental";

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
    datatools::utils::fetch_path_with_env(fname);
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
  if (!this->is_ranged()) {
    throw logic_error("datatools::utils::smart_filename::size: List of filenames is not ranged ! Size is not known in advance !");         
  }
  return list_.size();
}


smart_filename::const_iterator smart_filename::begin() const {
  return list_.begin();
}


smart_filename::const_iterator smart_filename::end() const {
  if (!ranged_) {
    throw logic_error("datatools::utils::smart_filename::end: List of filenames is not ranged (incremental mode with no stopping index) !");
  }
  return list_.end();
}


const std::string& smart_filename::operator[](int a_index) const {
  return this->get_filename(a_index);
}


const std::string& smart_filename::get_filename(int a_index) const {
  if (!this->is_valid()) {
    throw logic_error("datatools::utils::smart_filename::get_filename: Not valid !");
  }
  if (a_index < 0) {
    throw range_error("datatools::utils::smart_filename::get_filename: Index is not valid !");
  }
  if (this->is_incremental() 
      && ! this->is_ranged() 
      && (a_index >= list_.size())) {
    smart_filename* mutable_this = const_cast<smart_filename*>(this);
    for (int i = list_.size(); i <= a_index; ++i) {
      string filename;
      int file_number = incremental_starting_index_ +  incremental_increment_ * a_index;
      this->build_incremental_filename(file_number, filename);
      mutable_this->add_list(filename);
    }
  } else {
    if (a_index < 0 || a_index > list_.size()) {
      throw range_error("datatools::utils::smart_filename::get_filename: Index is not valid !");
    }
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
  if (!this->is_single()) {
    throw logic_error("datatools::utils::smart_filename::set: Not using 'single' mode !");
  }
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
  if (a_filename.empty()) {
    throw logic_error("datatools::utils::smart_filename::_add_list: Missing filename !");
  }
  if (this->is_single() && (list_.size() > 0)) {
    throw logic_error("datatools::utils::smart_filename::_add_list: Cannot add a filename ('single' mode) !");
  }
  string filename = a_filename;
  if (this->is_expand_path()) {
    datatools::utils::fetch_path_with_env(filename);
  }
  if ((list_.size() > 0) && !list_allow_duplication_) { 
    if (std::find(list_.begin(), list_.end(), filename) != list_.end()) {
      ostringstream message;
      message << "datatools::utils::smart_filename::_add_list: "
              << "Duplication error: filename '" 
              << filename << "' is already in the list !";
      throw logic_error(message.str());              
    }
  }
  list_.push_back(filename);
}


void smart_filename::add(const string& a_filename) {
  if (!this->is_list()) {
    ostringstream message;
    message << "datatools::utils::smart_filename::add: "
            << "Not using 'list' mode !";
    throw logic_error(message.str());                      
  }
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
  datatools::utils::fetch_path_with_env(list_filename);
  if (!boost::filesystem::exists(list_filename)) {
    ostringstream message;
    message << "datatools::utils::smart_filename::make_list: "
            << "File '" 
            << a_list_filename << "' does not exists !";
    throw logic_error(message.str());                      
  }
  if (boost::filesystem::is_directory(list_filename)) {
    ostringstream message;
    message << "datatools::utils::smart_filename::make_list: "
            << "Path '" 
            << list_filename << "' is a directory !";
    throw logic_error(message.str());                      
  }
  ostringstream message;
  message << "datatools::utils::smart_filename::make_list: "
          << "Reading file '" 
          << list_filename << "'...";
  clog << datatools::utils::io::notice << message.str() << endl; 
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
  if (a_prefix.empty()) {
    ostringstream message;
    message << "datatools::utils::smart_filename::make_incremental: "
            << "Missing prefix !";
    throw logic_error(message.str());              
  }
  a_smart_filename.incremental_path_ = a_path;
  a_smart_filename.incremental_prefix_ = a_prefix;
  a_smart_filename.incremental_extension_ = a_extension;
  a_smart_filename.incremental_suffix_ = a_suffix;
  a_smart_filename.incremental_index_ndigit_ = a_incremental_index_ndigit;
  if (a_increment_index == 0) {
    ostringstream message;
    message << "datatools::utils::smart_filename::make_incremental: "
            << "Invalid null increment !";
    throw logic_error(message.str());              
  }
  string path;
  if (!a_path.empty()) {
    path = a_smart_filename.incremental_path_;
    if (a_smart_filename.is_expand_path()) {
      datatools::utils::fetch_path_with_env(path);
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
      if (a_smart_filename.incremental_increment_ >= 0) {
        ostringstream message;
        message << "datatools::utils::smart_filename::make_incremental: "
                << "Invalid increment rule (start=" 
                << a_smart_filename.incremental_starting_index_
                << ",stop=" 
                << a_smart_filename.incremental_stopping_index_ 
                << ",increment=" 
                << a_smart_filename.incremental_increment_ << ") !";
        throw logic_error(message.str());              
      }
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
  a_smart_filename.dump(std::clog);
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


void smart_filename::print_list_of_filenames(std::ostream& a_out) const {
  if (!this->is_initialized()) {
    std::ostringstream message;
    message << "datatools::utils::smart_filename::print_list_of_filenames: "
            << "Smart file is not initialized !";
    throw std::logic_error(message.str());                      
  }
  for (std::vector<string>::const_iterator i = list_.begin();
       i != list_.end();
       ++i) {
    a_out << *i << std::endl;
  }
}


void smart_filename::store_list_of_filenames(const string& a_list_filename, 
                                              bool a_append) const {
  std::ofstream fout;
  std::string list_filename = a_list_filename;
  datatools::utils::fetch_path_with_env(list_filename);
  if (a_append) {
    fout.open(list_filename.c_str(), std::ios::app);
  } else {
    fout.open(list_filename.c_str());
  }
  if (!fout) {
    std::ostringstream message;
    message << "datatools::utils::smart_filename::store_list_of_filenames: "
            << "Cannot open file '" << list_filename << "' !";
    throw std::logic_error(message.str());                      
  }
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
  if (this->is_single()) mode_str = MODE_SINGLE_LABEL;
  if (this->is_list()) mode_str = MODE_LIST_LABEL;
  if (this->is_incremental()) mode_str = MODE_INCREMENTAL_LABEL;
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
      if (j != list_.size ()) {
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

  if (!a_config.has_key("mode")) {
    throw std::logic_error("datatools::utils::smart_filename::initialize: Missing 'mode' key !");
  }

  std::string mode_str = a_config.fetch_string("mode");

  if (mode_str == MODE_SINGLE_LABEL) {
    if (!a_config.has_key("single.filename")) {
      throw std::logic_error("datatools::utils::smart_filename::initialize: Missing 'single.filename' key !");
    }
    std::string single_filename = a_config.fetch_string("single.filename");
    smart_filename::make_single(*this, single_filename, expand_path);
  } else if (mode_str == MODE_LIST_LABEL) {
    bool allow_duplicate = false;
    if (a_config.has_flag("list.duplicate")) {
      allow_duplicate = true;
    }
    if (a_config.has_key("list.file")) {
      std::string list_file = a_config.fetch_string("list.file");
      smart_filename::make_list(*this, list_file, allow_duplicate, expand_path);
    } else {
      std::vector<string> list_vec;
      if (!a_config.has_key("list.filenames")) {
        throw std::logic_error("datatools::utils::smart_filename::initialize: Missing 'list.filenames' key !");
      }
      smart_filename::make_list(*this, allow_duplicate, expand_path);
      a_config.fetch("list.filenames", list_vec);
      for (int i = 0; i < (int)list_vec.size(); ++i) {
        this->add(list_vec[i]);
      }
    }
  } else if (mode_str == MODE_INCREMENTAL_LABEL) {
    std::string incremental_prefix = a_config.fetch_string("incremental.prefix");
    std::string incremental_suffix = "";
    std::string incremental_path;
    std::string incremental_extension;
    int incremental_stopping = MODE_INCREMENTAL_UNRANGED; 
    int incremental_starting = MODE_INCREMENTAL_DEFAULT_START; 
    int incremental_increment = MODE_INCREMENTAL_DEFAULT_INCREMENT; 
    uint32_t incremental_index_ndigit = 0; 

    if (!a_config.has_key("incremental.prefix")) {
      throw std::logic_error("datatools::utils::smart_filename::initialize: Missing 'incremental.prefix' key !");
    }
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
      if (incremental_increment == 0) {
        throw std::logic_error("datatools::utils::smart_filename::initialize: Invalid null increment value !");
      }
    }
    if (a_config.has_key("incremental.stop")) {
      incremental_stopping = a_config.fetch_integer("incremental.stop");
      if (incremental_stopping < -1) {
        throw std::logic_error("datatools::utils::smart_filename::initialize: Invalid negative stopping value !");
      }
    } else {
      // 2012-05-02 FM : allow unranged list of incremented filenames.
      // if (incremental_increment > 0)
      //   {
      //     throw logic_error ("datatools::utils::smart_filename::initialize: Missing 'incremental.stop' key !");
      //   }
    }
    if (a_config.has_key("incremental.start")) {
      incremental_starting = a_config.fetch_integer("incremental.start");
      if (incremental_starting < 0) {
        throw std::logic_error("datatools::utils::smart_filename::initialize: Invalid negative starting value !");
      }
    } else {
      if (incremental_increment < 0) {
        throw std::logic_error("datatools::utils::smart_filename::initialize: Missing 'incremental.start' key !");
      }
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
    std::ostringstream message;
    message << "datatools::utils::smart_filename::initialize: "
            << "Invalid 'mode' key ('" << mode_str << "') !";
    throw std::logic_error(message.str());              
  }
}

}  // end of namespace utils
}  // end of namespace datatools


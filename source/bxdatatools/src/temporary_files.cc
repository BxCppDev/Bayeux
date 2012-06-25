// -*- mode: c++; -*- 
// temporary_files.cc
// Ourselves
#include <datatools/utils/temporary_files.h>

// Standard Library
#include <cstdlib>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <stdexcept>

// Third Party
// - A
#include <boost/filesystem.hpp>

// This Project
#include <datatools/utils/utils.h>

namespace datatools {
namespace utils {

const std::string temp_file::DEFAULT_PATTERN = "tmp_";
bool temp_file::g_devel = false;


// ctor:
temp_file::temp_file() {
  this->set_defaults();
}


// ctor:
temp_file::temp_file(const char* pattern, bool remove_at_destroy) {
  this->set_defaults();
  this->set_remove_at_destroy(remove_at_destroy);
  std::string p = pattern;
  this->create ("", p);
}


// ctor:
temp_file::temp_file(std::string pattern, bool remove_at_destroy) {
  this->set_defaults();
  this->set_remove_at_destroy(remove_at_destroy);
  this->create("", pattern);
}


// ctor:
temp_file::temp_file(std::string path_dir, const char* pattern, 
                     bool remove_at_destroy) {
  this->set_defaults();
  this->set_remove_at_destroy(remove_at_destroy);
  std::string p = pattern;
  this->create(path_dir, p);
}


// ctor:
temp_file::temp_file(const char* path_dir, const char* pattern, 
                     bool remove_at_destroy) {
  this->set_defaults();
  this->set_remove_at_destroy(remove_at_destroy);
  std::string p = pattern;
  std::string dir = path_dir;
  this->create(dir, p);
}


// ctor:
temp_file::temp_file(std::string path_dir, std::string pattern, 
                     bool remove_at_destroy) {
  this->set_defaults();
  this->set_remove_at_destroy(remove_at_destroy);
  this->create(path_dir, pattern);
}


// dtor:
temp_file::~temp_file() {
  if (remove_at_destroy_) {
    if (g_devel) {
      std::clog << "DEVEL: datatools::utils::temp_file::dtor: "
                << "Deleting the file with name '"
                << filename_ 
                << "'" << std::endl;
    }
    this->remove();
  }
  if (template_ != 0) {
    delete template_;
    template_ = 0;
  }
}


const std::string& temp_file::get_filename() const {
  return filename_;
}


bool temp_file::is_valid() const {
  return template_ != 0;
}


bool temp_file::is_read_open() const {
  return read_open_;
}


bool temp_file::is_write_open() const {
  return write_open_;
}


void temp_file::set_remove_at_destroy(bool r) {
  remove_at_destroy_ = r;
}


std::ofstream& temp_file::out() {
  if (g_devel) {
    std::clog << "DEVEL: datatools::utils::temp_file::out: Entering..." 
              << std::endl;
  }
  if (!this->is_valid()) {
    std::ostringstream message;
    message << "datatools::utils::temp_file::out: "
            << "Temporary file has not been created yet !";
    throw std::logic_error(message.str());     
  }
  if (read_open_) {
    std::ostringstream message;
    message << "datatools::utils::temp_file::out: "
            << "Temporary file '" 
            << filename_ 
            << "' is already open in read mode !";
    throw std::logic_error(message.str());     
  }
  if (!write_open_) {
    out_.open(filename_.c_str());
    if (!out_) {
      std::ostringstream message;
      message << "datatools::utils::temp_file::out: "
              << "Cannot open temporary file '" 
              << filename_ 
              << "' in write mode !";
      throw std::runtime_error(message.str());                   
    }
    write_open_ = true;
  }
  if (g_devel) {
    std::clog << "DEVEL: datatools::utils::temp_file::out: Exiting." 
              << std::endl;
  }
  return out_;
}


std::ifstream& temp_file::in() {
  if (g_devel) {
    std::clog << "DEVEL: datatools::utils::temp_file::in: Entering..." 
              << std::endl;
  }
  if (!this->is_valid()) {
    std::ostringstream message;
    message << "datatools::utils::temp_file::in: "
            << "Temporary file has not been created yet !";
    throw std::logic_error(message.str());
  }
  if (write_open_) {
    std::ostringstream message;
    message << "datatools::utils::temp_file::in: "
            << "Temporary file '" 
            << filename_ 
            << "' is already open in write mode !";
    throw std::logic_error(message.str());     
  }
  if (!read_open_) {
    in_.open(filename_.c_str());
    if (!out_) {
      std::ostringstream message;
      message << "datatools::utils::temp_file::in: "
              << "Cannot open temporary file '" 
              << filename_ 
              << "' in read mode !";
      throw std::runtime_error(message.str());                   
    }
    read_open_ = true;
  }
  if (g_devel) {
    std::clog << "DEVEL: datatools::utils::temp_file::in: Exiting." 
              << std::endl;
  }
  return in_;
}


void temp_file::create(std::string a_path_dir, std::string a_pattern) {
  bool devel = g_devel;
  if (this->is_valid()) {
    std::ostringstream message;
    message << "datatools::utils::temp_file::create: "
            << "Temporary file with name '"
            << filename_ 
            << "' has already been created !";
    throw std::logic_error(message.str());
  } 
  if (a_path_dir.empty()) {
    path_dir_ = ".";
  } else {
    path_dir_ = a_path_dir;
    fetch_path_with_env(path_dir_);
  }
  if (!boost::filesystem::is_directory(path_dir_)) {
    std::ostringstream message;
    message << "datatools::utils::temp_file::create: "
            << "Directory with name '"
            << path_dir_ 
            << "' does not exist !";
    throw std::runtime_error(message.str());   
  }

  if (a_pattern.empty()) {
    pattern_ = DEFAULT_PATTERN;
  } else {
    fetch_path_with_env(a_pattern);
    if (a_pattern.find_first_of('/') != a_pattern.npos) {
      std::ostringstream message;
      message << "datatools::utils::temp_file::create: "
              << "Invalid pattern for filename ! Pattern '"
              << a_pattern 
              << "' contains invalid character(s) !";
      throw std::logic_error(message.str());         
    }
    pattern_ = a_pattern;
  }
  std::ostringstream filename_oss;
  filename_oss << path_dir_ << '/' << pattern_ << "XXXXXX";
  full_pattern_ = filename_oss.str();
  if (devel) {
    std::clog << "DEVEL: datatools::utils::temp_file::create: full_pattern is '" << full_pattern_ << "' !" << std::endl;
  }
  template_ = new char[full_pattern_.size() + 10];
  template_[0] = '\0';
  if (devel) {
    std::clog << "DEVEL: datatools::utils::temp_file::create: buffer is allocated !" << std::endl;
  }
  for (size_t i = 0; i < full_pattern_.size(); ++i) {
    template_[i] = full_pattern_[i];
  }
  template_[full_pattern_.size()] = '\0';
  if (devel) {
    std::clog << "DEVEL: datatools::utils::temp_file::create: template= '" 
              << template_ << "'" << std::endl;
  }
  int err = mkstemp(template_);
  if (err == -1) {
    std::ostringstream message;
    message << "datatools::utils::temp_file::create: "
            << "Cannot create temporary file using pattern '"
            << full_pattern_ << "' !";
    if (template_ != 0) {
      delete template_;
    }
    template_ = 0;
    throw std::runtime_error(message.str());
  } 
  if (devel) {
    std::clog << "DEVEL: datatools::utils::temp_file::create: Template is '" << template_ << "' !" << std::endl;
  }
  filename_ = template_;
  if (devel) {
    std::clog << "DEVEL: datatools::utils::temp_file::create: Filename is '"
              << filename_ << "' !" << std::endl;
  }
}


void temp_file::close() {
  if (this->is_read_open()) {
    if (g_devel) {
      std::clog << "DEVEL: datatools::utils::temp_file::close: closing read file mode !" << std::endl;
    }
    in_.close();
    read_open_ = false;
  }      
  if (this->is_write_open()) {
    if (g_devel) {
      std::clog << "DEVEL: datatools::utils::temp_file::close: closing write file mode !" << std::endl;
    }
    out_.close();
    write_open_ = false;
  }
}


void temp_file::remove() {
  if (g_devel) {
    std::clog << "DEVEL: datatools::utils::temp_file::remove: Entering..." 
              << std::endl;
  }
  this->close();
  if (!filename_.empty()) {
    int err = unlink(filename_.c_str());
    if (err == -1) {
      std::ostringstream message;
      message << "datatools::utils::temp_file::remove: "
              << "Cannot delete temporary file '"
              << filename_ << "' !";
      throw std::runtime_error(message.str());
    } 
  }
  if (template_ != 0) {
    delete template_;
    template_ = 0;
  }     
  this->set_defaults();
  if (g_devel) {
    std::clog << "DEVEL: datatools::utils::temp_file::remove: Exiting." 
              << std::endl;
  }
}


void temp_file::set_defaults() {
  remove_at_destroy_ = true;
  template_ = 0;
  read_open_ = false;
  write_open_ = false;
}

} // namespace utils
} // namespace datatools 

// end of temporary_files.cc

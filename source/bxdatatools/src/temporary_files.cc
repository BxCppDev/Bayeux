// -*- mode: c++; -*-
// temporary_files.cc
// Ourselves
#include <datatools/temporary_files.h>

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
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/exception.h>
#include <datatools/logger.h>

namespace datatools {

const std::string temp_file::DEFAULT_PATTERN = "tmp_";

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
    std::clog << "NOTICE: datatools::temp_file::dtor: "
              << "Deleting the temporary file with name '"
              << filename_
              << "'" << std::endl;
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
  DT_THROW_IF (!this->is_valid(),
               std::logic_error,
               "Temporary file has not been created yet !");
  DT_THROW_IF (read_open_,
               std::logic_error,
               "Temporary file '" << filename_ << "' is already open in read mode !");
  if (!write_open_) {
    out_.open(filename_.c_str());
    DT_THROW_IF (!out_,
                 std::runtime_error,
                 "Cannot open temporary file '" << filename_ << "' in write mode !");
    write_open_ = true;
  }
  return out_;
}


std::ifstream& temp_file::in() {
  DT_THROW_IF (!this->is_valid(),
               std::logic_error,
               "Temporary file has not been created yet !");
  DT_THROW_IF (write_open_,
               std::logic_error,
               "Temporary file '" << filename_ << "' is already open in write mode !");
  if (!read_open_) {
    in_.open(filename_.c_str());
    DT_THROW_IF (!out_,
                 std::runtime_error,
                 "Cannot open temporary file '" << filename_ << "' in read mode !");
    read_open_ = true;
  }
  return in_;
}


void temp_file::create(std::string a_path_dir, std::string a_pattern) {
  bool devel = false;
  DT_THROW_IF (this->is_valid(),
               std::logic_error,
               "Temporary file has already been created !");
  if (a_path_dir.empty()) {
    path_dir_ = ".";
  } else {
    path_dir_ = a_path_dir;
    fetch_path_with_env(path_dir_);
  }
  boost::filesystem::path dir_path(path_dir_.c_str ());
  DT_THROW_IF ( boost::filesystem::exists (dir_path)
                && ! boost::filesystem::is_directory (dir_path),
                std::runtime_error,
                "Path '" << dir_path.string () << "' is not a directory !");
  if (!boost::filesystem::exists(dir_path)) {
    DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                  "Creating base directory '" << dir_path.string () << "' for temporary file.");
    DT_THROW_IF (! boost::filesystem::create_directories(dir_path),
                 std::runtime_error,
                 "Couldn't create directory with name '" << dir_path.string () << "' !");
  }

  if (a_pattern.empty()) {
    pattern_ = DEFAULT_PATTERN;
  } else {
    fetch_path_with_env(a_pattern);
    DT_THROW_IF (a_pattern.find_first_of('/') != a_pattern.npos,
                 std::logic_error,
                 "Invalid pattern for filename ! Pattern '" << a_pattern << "' contains invalid character(s) !");
    pattern_ = a_pattern;
  }
  std::ostringstream filename_oss;
  filename_oss << path_dir_ << '/' << pattern_ << "XXXXXX";
  full_pattern_ = filename_oss.str();
  template_ = new char[full_pattern_.size() + 10];
  template_[0] = '\0';
  for (size_t i = 0; i < full_pattern_.size(); ++i) {
    template_[i] = full_pattern_[i];
  }
  template_[full_pattern_.size()] = '\0';
  int err = mkstemp(template_);
  if (err == -1) {
    if (template_ != 0) {
      delete template_;
    }
    template_ = 0;
    std::ostringstream message;
    message << "datatools::temp_file::create: "
            << "Cannot create temporary file using pattern '"
            << full_pattern_ << "' !";
    DT_THROW_IF(true, std::runtime_error, message.str());
  }
  filename_ = template_;
}


void temp_file::close() {
  if (this->is_read_open()) {
    in_.close();
    read_open_ = false;
  }
  if (this->is_write_open()) {
    out_.close();
    write_open_ = false;
  }
}


void temp_file::remove() {
  this->close();
  if (!filename_.empty()) {
    int err = unlink(filename_.c_str());
    DT_THROW_IF (err == -1,
                 std::runtime_error,
                 "Cannot delete temporary file '" << filename_ << "' !");
  }
  if (template_ != 0) {
    delete template_;
    template_ = 0;
  }
  this->set_defaults();
}


void temp_file::set_defaults() {
  remove_at_destroy_ = true;
  template_ = 0;
  read_open_ = false;
  write_open_ = false;
}

} // namespace datatools

// end of temporary_files.cc

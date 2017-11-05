// temporary_files.cc

// Ourselves:
#include <datatools/temporary_files.h>

// Standard Library:
#include <cstdlib>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <stdexcept>

// Third Party:
#include <boost/filesystem.hpp>

// This Project:
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/exception.h>
#include <datatools/logger.h>

namespace datatools {

  const std::string & temp_file::default_pattern()
  {
    static std::string pattern;
    if (pattern.empty()) pattern = "tmp_";
    return pattern;
  }

  temp_file::temp_file()
  {
    this->set_defaults();
    return;
  }

  temp_file::temp_file(const char* pattern, bool remove_at_destroy)
  {
    this->set_defaults();
    this->set_remove_at_destroy(remove_at_destroy);
    std::string p = pattern;
    this->create ("", p);
    return;
  }

  temp_file::temp_file(std::string pattern, bool remove_at_destroy)
  {
    this->set_defaults();
    this->set_remove_at_destroy(remove_at_destroy);
    this->create("", pattern);
    return;
  }

  temp_file::temp_file(std::string path_dir, const char* pattern,
                       bool remove_at_destroy)
  {
    this->set_defaults();
    this->set_remove_at_destroy(remove_at_destroy);
    std::string p = pattern;
    this->create(path_dir, p);
    return;
  }

  temp_file::temp_file(const char* path_dir, const char* pattern,
                       bool remove_at_destroy)
  {
    this->set_defaults();
    this->set_remove_at_destroy(remove_at_destroy);
    std::string p = pattern;
    std::string dir = path_dir;
    this->create(dir, p);
    return;
  }

  temp_file::temp_file(std::string path_dir, std::string pattern,
                       bool remove_at_destroy)
  {
    this->set_defaults();
    this->set_remove_at_destroy(remove_at_destroy);
    this->create(path_dir, pattern);
    return;
  }

  temp_file::~temp_file()
  {
    if (_remove_at_destroy_) {
      if(_verbose_) {
        DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                      "Deleting the temporary file with name '"
                      << _filename_
                      << "'");
      }
      this->remove();
    }
    if (_template_ != 0) {
      delete _template_;
      _template_ = 0;
    }
    return;
  }

  const std::string& temp_file::get_filename() const
  {
    return _filename_;
  }

  bool temp_file::is_valid() const
  {
    return _template_ != 0;
  }

  bool temp_file::is_read_open() const
  {
    return _read_open_;
  }

  bool temp_file::is_write_open() const
  {
    return _write_open_;
  }

  void temp_file::set_remove_at_destroy(bool r)
  {
    _remove_at_destroy_ = r;
    return;
  }

  void temp_file::set_verbose(bool v)
  {
    _verbose_ = v;
    return;
  }

  std::ofstream& temp_file::out()
  {
    DT_THROW_IF (!this->is_valid(),
                 std::logic_error,
                 "Temporary file has not been created yet !");
    DT_THROW_IF (_read_open_,
                 std::logic_error,
                 "Temporary file '" << _filename_ << "' is already open in read mode !");
    if (!_write_open_) {
      _out_.open(_filename_.c_str());
      DT_THROW_IF (!_out_,
                   std::runtime_error,
                   "Cannot open temporary file '" << _filename_ << "' in write mode !");
      _write_open_ = true;
    }
    return _out_;
  }

  std::ifstream& temp_file::in()
  {
    DT_THROW_IF (!this->is_valid(),
                 std::logic_error,
                 "Temporary file has not been created yet !");
    DT_THROW_IF (_write_open_,
                 std::logic_error,
                 "Temporary file '" << _filename_ << "' is already open in write mode !");
    if (!_read_open_) {
      _in_.open(_filename_.c_str());
      DT_THROW_IF (!_in_,
                   std::runtime_error,
                   "Cannot open temporary file '" << _filename_ << "' in read mode !");
      _read_open_ = true;
    }
    return _in_;
  }

  void temp_file::create(std::string a_path_dir, std::string a_pattern)
  {
    DT_THROW_IF (this->is_valid(),
                 std::logic_error,
                 "Temporary file has already been created !");
    if (a_path_dir.empty()) {
      _path_dir_ = ".";
    } else {
      _path_dir_ = a_path_dir;
      fetch_path_with_env(_path_dir_);
    }
    boost::filesystem::path dir_path(_path_dir_.c_str ());
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
      _pattern_ = default_pattern();
    } else {
      fetch_path_with_env(a_pattern);
      DT_THROW_IF (a_pattern.find_first_of('/') != a_pattern.npos,
                   std::logic_error,
                   "Invalid pattern for filename ! Pattern '" << a_pattern << "' contains invalid character(s) !");
      _pattern_ = a_pattern;
    }
    std::ostringstream filename_oss;
    filename_oss << _path_dir_ << '/' << _pattern_ << "XXXXXX";
    _full_pattern_ = filename_oss.str();
    _template_ = new char[_full_pattern_.size() + 10];
    _template_[0] = '\0';
    for (size_t i = 0; i < _full_pattern_.size(); ++i) {
      _template_[i] = _full_pattern_[i];
    }
    _template_[_full_pattern_.size()] = '\0';
    int err = mkstemp(_template_);
    if (err == -1) {
      if (_template_ != 0) {
        delete _template_;
      }
      _template_ = 0;
      std::ostringstream message;
      message << "datatools::temp_file::create: "
              << "Cannot create temporary file using pattern '"
              << _full_pattern_ << "' !";
      DT_THROW_IF(true, std::runtime_error, message.str());
    }
    _filename_ = _template_;
    return;
  }

  void temp_file::close()
  {
    if (this->is_read_open()) {
      _in_.close();
      _read_open_ = false;
    }
    if (this->is_write_open()) {
      _out_.close();
      _write_open_ = false;
    }
    return;
  }

  void temp_file::remove()
  {
    this->close();
    if (!_filename_.empty()) {
      int err = unlink(_filename_.c_str());
      DT_THROW_IF (err == -1,
                   std::runtime_error,
                   "Cannot delete temporary file '" << _filename_ << "' !");
    }
    if (_template_ != 0) {
      delete _template_;
      _template_ = 0;
    }
    this->set_defaults();
    return;
  }

  void temp_file::set_defaults()
  {
    _verbose_ = false;
    _remove_at_destroy_ = true;
    _template_ = 0;
    _read_open_ = false;
    _write_open_ = false;
    return;
  }

} // namespace datatools

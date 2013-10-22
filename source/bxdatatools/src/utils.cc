// -*- mode: c++; -*-
// utils.cc
// Ourselves
#include <datatools/utils.h>

// Standard Library
#include <cstdlib>
#include <cmath>
#include <unistd.h>
#include <wordexp.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <vector>

// Third Party
// - Boost
#include <boost/tokenizer.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem.hpp>

// This Project
#include <datatools/ioutils.h>
#include <datatools/library_info.h>
#include <datatools/properties.h>
#include <datatools/kernel.h>

namespace datatools {

void invalidate(double& x) {
  x = std::numeric_limits<double>::quiet_NaN();
}

bool is_valid(double x) {
  return x == x;
}

bool is_infinity(double x) {
  return std::isinf(x);
}

bool is_normal(double x) {
  return ! is_infinity(x) && is_valid(x);
}


void plus_infinity(double& x) {
  x = std::numeric_limits<double>::infinity();
}


void minus_infinity(double& x) {
  x = -std::numeric_limits<double>::infinity();
}


void infinity(double& x) {
  plus_infinity(x);
}


void invalidate(float& x) {
  x = std::numeric_limits<float>::quiet_NaN();
}


bool is_valid(float x) {
  return x == x;
}


void plus_infinity(float& x) {
  x = std::numeric_limits<float>::infinity();
}


void minus_infinity(float& x) {
  x = -std::numeric_limits<float>::infinity();
}


void infinity(float& x) {
  plus_infinity(x);
}

struct _gp {
  enum action_type {
    ACTION_GET = 1,
    ACTION_SET = 2,
    ACTION_RESET = 3
  };
  static const std::string & global_path(int action_,
                                         const std::string & gpath_ = "");
};

bool has_global_path()
{
  return ! _gp::global_path(_gp::ACTION_GET).empty();
}

void set_global_path(const std::string & gpath_)
{
  _gp::global_path(_gp::ACTION_SET, gpath_);
}

void reset_global_path()
{
  _gp::global_path(_gp::ACTION_RESET);
}

const std::string & get_global_path()
{
  return _gp::global_path(_gp::ACTION_GET);
}

/**************************************************/
class fetch_path_processor {
 public:

  enum use_type {
    USE_NOTHING        = 0x0,
    USE_TRACE          = bit_mask::bit01,
    USE_GLOBAL_PATH    = bit_mask::bit02,
    USE_ENVIRON        = bit_mask::bit03,
    USE_KERNEL_LIBINFO = bit_mask::bit04
  };

  fetch_path_processor(std::string parent_path_ = "",
                       bool use_global_path_ = false,
                       bool use_env_ = true,
                       bool use_kernel_libinfo_ = true);

  fetch_path_processor(uint32_t use_mode,
                       const std::string & parent_path_ = "");

  bool process(std::string&);

  void set_use_global_path(bool);

  bool use_global_path() const;

  void set_use_kernel_libinfo(bool);

  bool use_kernel_libinfo() const;

 private:
  void process_impl(std::string &);
  bool _trace_;
  bool _use_global_path_;
  bool _use_env_;
  bool _use_kernel_libinfo_;
  std::string _parent_path_;
};


const std::string & _gp::global_path(int action_,
                                     const std::string & gpath_)
{
  static boost::scoped_ptr<std::string> g_path(0);
  if (g_path.get() == 0) {
    g_path.reset(new std::string);
  }
  std::string & gpath = *g_path.get();
  {
    char * egp = ::getenv("DATATOOLS_GLOBAL_PATH");
    if (gpath.empty() && egp != 0) {
      std::clog << datatools::io::notice
                << "datatools::_gp::global_path: "
                << "Set the global path from the 'DATATOOLS_GLOBAL_PATH' environment variable."
                << std::endl;
      gpath = egp;
    }
  }
  if (action_ == ACTION_RESET) {
    gpath.clear();
  } else if (action_ == ACTION_GET) {
  } else if (action_ == ACTION_SET) {
    if (! gpath_.empty()) {
      fetch_path_processor fpp;
      fpp.set_use_global_path(false);
      std::string p = gpath_;
      fpp.process(p);
      gpath = p;
    }
  }
  return gpath;
}

void fetch_path_processor::set_use_global_path(bool ugp_)
{
  _use_global_path_ = ugp_;
}

bool fetch_path_processor::use_global_path() const
{
  return _use_global_path_;
}

void fetch_path_processor::set_use_kernel_libinfo(bool ukl_)
{
  _use_kernel_libinfo_ = ukl_;
  return;
}

bool fetch_path_processor::use_kernel_libinfo() const
{
  return _use_kernel_libinfo_;
}

fetch_path_processor::fetch_path_processor(uint32_t use_mode_,
                                           const std::string & parent_path_)
{
  _trace_ = false;
  _use_global_path_ = use_mode_ & USE_GLOBAL_PATH;
  _use_env_ = use_mode_ & USE_ENVIRON;
  _use_kernel_libinfo_ = use_mode_ & USE_KERNEL_LIBINFO;
  _parent_path_ = parent_path_;
  return;
}


fetch_path_processor::fetch_path_processor(std::string parent_path_,
                                           bool use_global_path_,
                                           bool use_env_,
                                           bool use_kernel_libinfo_) {
  _use_global_path_ = use_global_path_;
  _parent_path_ = parent_path_;
  _trace_ = false;
  _use_env_ = use_env_;
  _use_kernel_libinfo_ = use_kernel_libinfo_;
}

bool fetch_path_processor::process(std::string& path) {
  try {
    this->process_impl(path);
  } catch (std::exception & error) {
    DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                 "Cannot process path '" << path << "' : "
                 << error.what());
    return false;
  }
  return true;
}

// Hmmm probably this method needs refactoring... (with Boost/Spirit?)
void fetch_path_processor::process_impl(std::string& path) {
  bool trace = _trace_;
  //trace = true;

  // working buffer:
  std::string text = path;
  bool registered_lib_resource = false;
  if (text[0] == '@') {
    DT_THROW_IF(! datatools::kernel::is_instantiated(),
                std::runtime_error,
                "The datatools kernel has not been instantiated !"
                << "No support for '@foo:bar.txt' syntax !");
    const datatools::kernel & dtk = datatools::kernel::instance();
    DT_THROW_IF(! dtk.has_library_info_register(),
                std::runtime_error,
                "The datatools kernel library info register has not been activated !"
                << "No support for '@foo:bar/blah.txt' syntax !");
    const datatools::library_info & lib_info_reg =
      datatools::kernel::instance().get_library_info_register();
    int pos = text.find(':');
    DT_THROW_IF(pos == text.npos,
                std::logic_error,
                "Invalid syntax for library location !");
    std::string library_name = text.substr(1, pos-1);
    if (trace) {
      DT_LOG_TRACE(datatools::logger::PRIO_TRACE,
                   "Found resource directive for library '" << library_name
                   << "' in path '" << path << ".");
    }
    DT_THROW_IF(! lib_info_reg.has(library_name),
                std::logic_error,
                "Unregistered resource path for library '" << library_name << "' !");
    if (trace) {
      lib_info_reg.print(library_name, std::cerr);
      DT_LOG_TRACE(datatools::logger::PRIO_TRACE,
                   "INSTALL_RESOURCE_DIR key is = '"
                   << datatools::library_info::keys::install_resource_dir() << "'");
    }

    const datatools::properties & lib_infos = lib_info_reg.get(library_name);

    // Search for resource directory path from the register:
    std::string resource_dir_str;

    // From the registered environment variable name (if any):
    if (resource_dir_str.empty() && lib_infos.has_key(datatools::library_info::keys::env_resource_dir())) {
      std::string env_resource_dir = lib_infos.fetch_string(datatools::library_info::keys::env_resource_dir());
      if (! env_resource_dir.empty()) {
        const char *env_value = getenv(env_resource_dir.c_str());
        if (env_value != 0) {
          resource_dir_str = std::string(env_value);
        }
      }
    }

    // From the registered installation path (if any):
    if (resource_dir_str.empty() && lib_infos.has_key(datatools::library_info::keys::install_resource_dir())) {
       resource_dir_str = lib_infos.fetch_string(datatools::library_info::keys::install_resource_dir());
    }

    if (! resource_dir_str.empty()) {
      boost::filesystem::path resource_dir = resource_dir_str;
      if (trace) {
        DT_LOG_TRACE(datatools::logger::PRIO_TRACE,
                     "Resource installation path for library '" << library_name
                     << "' in path '" << path << " is '" << resource_dir << "'.");
      }
      boost::filesystem::path resource_relative_path = text.substr(pos+1);
      if (trace) {
        DT_LOG_TRACE(datatools::logger::PRIO_TRACE,
                     "Resource relative path for library '" << library_name
                     << "' in path '" << path << " is '" << resource_relative_path << "'.");
      }
      boost::filesystem::path resource_full_path = resource_dir / resource_relative_path;
      text = resource_full_path.string();
      if (trace) {
        DT_LOG_TRACE(datatools::logger::PRIO_TRACE,
                     "Resource absolute path for library '" << library_name
                     << "' in path '" << path << " is '" << text << "'.");
      }
      registered_lib_resource = true;
    } else {
      DT_THROW_IF(true, std::logic_error,
                 "No resource installation directory for library '" << library_name
                 << "' in path '" << path << "' !");
    }
  }

  DT_THROW_IF(text.find('?') != text.npos || text.find('*') != text.npos,
              std::logic_error,
              "Wildcard characters found in path = " << text);

  if (_use_env_) {
    // Activate the parsing of embedded environment variables:
    std::ostringstream s;
    wordexp_t p;
    int we_error = wordexp( text.c_str(), &p, WRDE_NOCMD|WRDE_SHOWERR|WRDE_UNDEF);

    DT_THROW_IF(we_error != 0,
                std::logic_error,
                "wordexp error, code = " << we_error << ", input = '" << path << "'");

    if (p.we_wordc == 0) {
      DT_THROW_IF(true, std::logic_error, "Nothing to expand !");
    }

    char** w = p.we_wordv;
    if (p.we_wordc > 1) {
      std::ostringstream message;
      message << "wordexp expands to many tokens : ";
      for (size_t i = 0; i < p.we_wordc; i++) {
        message << " '" << w[i]<< "'";
      }
      wordfree( &p );
      DT_THROW_IF(true, std::logic_error, message.str());
    }
    s << w[0];
    wordfree( &p );
    text = s.str();
  }
  path = text;

  if (! registered_lib_resource) {
    // If the path does not start with a "@libname:" prefix
    // we can search for an explicit parent path :
    // Check for an explicit parent path :
    std::string parent_path = _parent_path_;
    if (parent_path.empty()) {
      // Check for an implicit parent path :
      if (use_global_path() && datatools::has_global_path())  {
        parent_path = get_global_path();
      }
    }
    if (! parent_path.empty()
        && (path.substr(0, 1) != "/")
        && (path != ".")
        && (path.substr(0, 2) != "./")
        && (path.find(":/") == std::string::npos)) {
      // Process relative path with prepend parent path :
      text = parent_path + '/' + path;
    }
  }
  path = text;
  return;
}
/**************************************************/

std::string fetch_path(const std::string& word)
{
  std::string w = word; // Working buffer
  fetch_path_processor fpp("", true, true, true);
  bool r = fpp.process(w);
  DT_THROW_IF(!r, std::logic_error, "Cannot fetch path from '" << word << "' !");
  return w;
}


bool fetch_path_without_env(std::string& path) {
  fetch_path_processor fpp("", false, false, true);
  return fpp.process(path);
}


bool fetch_path_with_env(std::string& path) {
  fetch_path_processor fpp("", false, true, true);
  return fpp.process(path);
}


bool fetch_path_with_env_p(std::string& path,
                           const std::string & parent_path_) {
  fetch_path_processor fpp(parent_path_, false, true, true);
  return fpp.process(path);
}


bool fetch_path_with_env_g(std::string& path) {
  fetch_path_processor fpp("", true);
  return fpp.process(path);
}

bool fetch_path_with_env_pg(std::string& path,
                           const std::string & parent_path_) {
  fetch_path_processor fpp(parent_path_, true);
  return fpp.process(path);
}


std::string expand_path(const std::string& path) {
  std::string res("");
  if (path.empty()) return (res);

  const std::string allowed =
      ":~./_-${}0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int found = path.find_first_not_of(allowed);

  DT_THROW_IF(found > 0, std::runtime_error, "invalid character found");

  std::ostringstream sh_cmd;
  char dummy[256];
  sprintf(dummy, "tmp.XXXXXX");

  int tmpfile = mkstemp(dummy);
  DT_THROW_IF(tmpfile == -1, std::runtime_error, "Failed to create tmp file");

  sh_cmd << "echo \"" << path << "\" > " << dummy;

  int ret = system(sh_cmd.str().c_str());
  DT_THROW_IF(ret != 0, std::runtime_error, "Shell command " << sh_cmd.str() << " failed");

  std::ifstream input(dummy);
  DT_THROW_IF(!input, std::runtime_error, "Cannot open file " << dummy);

  std::string line_get;
  std::getline(input, line_get);
  DT_THROW_IF(!input, std::runtime_error, "Cannot read file " << dummy);

  input.close();
  close(tmpfile);
  ret = unlink(dummy);

  if (ret != 0) {
    std::cerr << "expand_path: warning: "
              << "Cannot delete temporary file '"
              << dummy << "'" << std::endl;
  }
  return (line_get);
}


/* From:
   Data Structures in C++ Using the STL
   by Timothy A. Budd
   published by Addison Wesley Longman
   ISBN 0-201-31659-5
http://web.engr.oregonstate.edu/~budd/Books/stl/info/ReadMe.html
*/
void split_string(const std::string& text, const std::string& separators,
                  std::list<std::string>& words) {
// split a string into a list of words
// 'text_' and 'separators_' are input,
// list of 'words_' is output
  int text_len = text.length();

  // find first non-separator character
  int start = text.find_first_not_of(separators, 0);

  // loop as long as we have a non-separator character
  while ((start >= 0) && (start < text_len)) {
    // find end of current word
    int stop = text.find_first_of(separators, start);

    if ((stop < 0) || (stop > text_len)) stop = text_len;

    // add word to list of words
    words.push_back(text.substr(start, stop - start));

    // find start of next word
    start = text.find_first_not_of(separators, stop+1);
  }
}

} // namespace datatools

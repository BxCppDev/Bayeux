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
//#include <boost/tokenizer.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

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

bool is_quoted(const std::string & text_, char q_)
{
  DT_THROW_IF(q_ != '\'' && q_ != '"',
              std::range_error,
              "Unsupported quoting character '" << q_ << "'!");
  char c = q_;
  if (text_.size() > 1) {
    return (text_[0] == c) && (text_[text_.size() - 1] == c);
  }
  return false;
}

void add_quotes(const std::string & from_, std::string & to_, char q_)
{
  DT_THROW_IF(q_ != '\'' && q_ != '"',
              std::range_error,
              "Unsupported quoting character '" << q_ << "'!");
  if (! is_quoted(from_, q_)) {
    std::ostringstream oss;
    oss << q_ << from_ << q_;
    to_ = oss.str();
  } else {
    to_ = from_;
  }
  return;
}

void add_quotes(std::string & text_, char q_)
{
  std::string t;
  add_quotes(text_, t, q_);
  text_ = t;
  return;
}

void remove_quotes(const std::string & from_, std::string & to_, char q_)
{
  DT_THROW_IF(q_ != '\'' && q_ != '"',
              std::range_error,
              "Unsupported quoting character '" << q_ << "'!");
  if (is_quoted(from_, q_)) {
    to_ = from_.substr(1,from_.length()-2);
  } else {
    to_ = from_;
  }
  return;
}

void remove_quotes(std::string & text_, char q_)
{
  std::string t;
  remove_quotes(text_, t, q_);
  text_ = t;
  return;
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

  struct lib_info_keys_entry_type
  {
    std::string install_path_key;
    std::string environ_path_key;
  };
  typedef std::map<std::string, lib_info_keys_entry_type> lib_info_keys_dict_type;

  static const lib_info_keys_dict_type & lib_info_keys();

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

bool resolve_library_info_path_keys(const std::string & library_topic_,
                                    std::string & install_path_key_,
                                    std::string & environ_path_key_)
{
  typedef fetch_path_processor::lib_info_keys_dict_type dict_type;
  const dict_type & lik = fetch_path_processor::lib_info_keys();
  dict_type::const_iterator found = lik.find(library_topic_);
  if (found == lik.end()) {
    return false;
  }
  bool valid = false;
  if (! found->second.install_path_key.empty()) {
    install_path_key_ = found->second.install_path_key;
    valid = true;
  }
  if (! found->second.environ_path_key.empty()) {
    environ_path_key_ = found->second.environ_path_key;
    valid = true;
  }
  return valid;
}


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

// static
const fetch_path_processor::lib_info_keys_dict_type & fetch_path_processor::lib_info_keys()
{
  static boost::scoped_ptr<lib_info_keys_dict_type> _lik;
  if (! _lik) {
    _lik.reset(new lib_info_keys_dict_type);
    lib_info_keys_dict_type & keys = *_lik;

    {
      lib_info_keys_entry_type e;
      e.install_path_key = library_info::keys().install_prefix();
      e.environ_path_key = library_info::keys().env_prefix();
      keys["prefix"] = e;
    }

    {
      lib_info_keys_entry_type e;
      e.install_path_key = library_info::keys().install_resource_dir();
      e.environ_path_key = library_info::keys().env_resource_dir();
      keys["resources"] = e;
      keys[""] = e;
    }

    {
      lib_info_keys_entry_type e;
      e.install_path_key = library_info::keys().install_lib_dir();
      e.environ_path_key = library_info::keys().env_lib_dir();
      keys["libraries"] = e;
    }

    {
      lib_info_keys_entry_type e;
      e.install_path_key = library_info::keys().install_bin_dir();
      e.environ_path_key = library_info::keys().env_bin_dir();
      keys["binaries"] = e;
    }

    {
      lib_info_keys_entry_type e;
      e.install_path_key = library_info::keys().install_plugin_lib_dir();
      e.environ_path_key = library_info::keys().env_plugin_lib_dir();
      keys["plugins"] = e;
    }

    {
      lib_info_keys_entry_type e;
      e.install_path_key = library_info::keys().install_doc_dir();
      keys["docs"] = e;
    }

  }
  return *_lik;
}

// Hmmm probably this method needs refactoring... (with Boost/Spirit?)
// Well, Boost Spirit is probably expensive for such a simple parsing
// (compilation time and size of code explodes with many levels of templatization...)
void fetch_path_processor::process_impl(std::string& path) {
  bool trace = _trace_;
  {
    // Special environment variable to trace the resolution of path
    char * env = getenv("DATATOOLS_FETCH_PATH_TRACE");
    if (env != NULL) {
      std::string str_env = env;
      if (str_env == "1") {
        trace = true;
        DT_LOG_TRACE(datatools::logger::PRIO_TRACE,
                     "Activating the TRACE logging level thanks to the 'DATATOOLS_FETCH_PATH_TRACE' environment variable for path '" << path << "'...");
      }
    }
  }

  if (trace) {
    DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Entering...");
  }
  // working buffer:
  std::string text = path;
  bool registered_lib_topic = false;
  if (text[0] == '@') {
    if (trace) {
      DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Entering '@' mode...");
    }
    DT_THROW_IF(! datatools::kernel::is_instantiated(),
                std::runtime_error,
                "The datatools kernel has not been instantiated !"
                << "No support for '@foo:bar.txt' syntax !");
    const datatools::kernel & dtk = datatools::kernel::instance();
    DT_THROW_IF(! dtk.has_library_info_register(),
                std::runtime_error,
                "The datatools kernel library info register has not been activated !"
                << "No support for '@foo[.topic]:bar/blah.txt' syntax !");
    const datatools::library_info & lib_info_reg =
      datatools::kernel::instance().get_library_info_register();
    size_t pos = text.find(':');
    DT_THROW_IF(pos == text.npos,
                std::logic_error,
                "Invalid syntax for library location !");
    std::string library_directive = text.substr(1, pos-1);
    std::vector<std::string> lib_tokens;
    boost::split(lib_tokens, library_directive, boost::is_any_of("."));
    // if (trace) {
    //   for (int i = 0; i < (int) lib_tokens.size(); i++) {
    //     std::cerr << "DEVEL: tok = " << lib_tokens[i] << std::endl;
    //   }
    // }

    DT_THROW_IF(lib_tokens.size() == 0, std::logic_error,
                "Missing library directive !");
    std::string library_name = lib_tokens[0];
    std::string library_topic;
    if (lib_tokens.size() > 1) {
      library_topic = lib_tokens[1];
      if (trace) {
        DT_LOG_TRACE(datatools::logger::PRIO_TRACE,
                     "Found library topic '" << library_topic
                     << "'.");
      }
    }

    std::string install_path_key;
    std::string environ_path_key;
    if (! resolve_library_info_path_keys(library_topic, install_path_key, environ_path_key)) {
      DT_THROW_IF(true,
                  std::logic_error,
                  "Topic '" << library_topic << "' is not registered !");
    }

    if (trace) {
      DT_LOG_TRACE(datatools::logger::PRIO_TRACE,
                   "Found '" << library_topic << "' directive for library '" << library_name
                   << "' in path '" << path << ".");
      DT_LOG_TRACE(datatools::logger::PRIO_TRACE,
                   "with install_path_key='" << install_path_key << "'");
      DT_LOG_TRACE(datatools::logger::PRIO_TRACE,
                   "and  environ_path_key='" << environ_path_key << "'");
    }
    DT_THROW_IF(! lib_info_reg.has(library_name),
                std::logic_error,
                "Unregistered library '" << library_name << "' in the kernel library register!");

    const datatools::properties & lib_infos = lib_info_reg.get(library_name);

    // Search for topic directory path from the register:
    std::string topic_dir_str;

    // From the registered environment variable name (if any):
    if (!environ_path_key.empty()) {
      if (trace) {
        DT_LOG_TRACE(datatools::logger::PRIO_TRACE,
                     "Search to resolve the path from an environment variable...");
      }
      if (topic_dir_str.empty() && lib_infos.has_key(environ_path_key)) {
        std::string env_topic_dir = lib_infos.fetch_string(environ_path_key);
        if (! env_topic_dir.empty()) {
          const char *env_value = getenv(env_topic_dir.c_str());
          if (env_value != 0) {
            if (trace) {
              DT_LOG_TRACE(datatools::logger::PRIO_TRACE,
                           "Found the '" << env_topic_dir << "' environment variable.");
            }
            topic_dir_str = std::string(env_value);
          }
        }
      }
    }

    // From the registered installation path (if any):
    if (!install_path_key.empty()) {
      if (topic_dir_str.empty() && lib_infos.has_key(install_path_key)) {
        topic_dir_str = lib_infos.fetch_string(install_path_key);
      }
    }

    if (! topic_dir_str.empty()) {
      boost::filesystem::path topic_dir = topic_dir_str;
      if (trace) {
        DT_LOG_TRACE(datatools::logger::PRIO_TRACE,
                     "Topic installation path for library '" << library_name
                     << "' in path '" << path << " is '" << topic_dir << "'.");
      }
      boost::filesystem::path topic_relative_path = text.substr(pos+1);
      if (trace) {
        DT_LOG_TRACE(datatools::logger::PRIO_TRACE,
                     "Topic relative path for library '" << library_name
                       << "' in path '" << path << " is '" << topic_relative_path << "'.");
      }
      boost::filesystem::path topic_full_path = topic_dir / topic_relative_path;
      text = topic_full_path.string();
        if (trace) {
          DT_LOG_TRACE(datatools::logger::PRIO_TRACE,
                       "Topic absolute path for library '" << library_name
                       << "' in path '" << path << " is '" << text << "'.");
        }
        registered_lib_topic = true;
    } else {
      DT_THROW_IF(true, std::logic_error,
                  "No '" << library_topic << "' directory for library '" << library_name
                  << "' as been resolved from path '" << path << "' !");
    }

  } // end if (text[0] == '@')

  DT_THROW_IF(text.find('?') != text.npos || text.find('*') != text.npos,
              std::logic_error,
              "Wildcard characters found in path = " << text);

  if (_use_env_) {
    // Activate the parsing of embedded environment variables:
    wordexp_t p;
    // bool freed = false;
    std::string error_message;
    const char * word = text.c_str();
    try {
      int we_error = wordexp( word, &p, WRDE_NOCMD|WRDE_SHOWERR|WRDE_UNDEF);
      switch (we_error) {
      case WRDE_BADCHAR:
        //std::cerr << "ERROR ************* WRDE_BADCHAR" << std::endl;
        DT_THROW_IF(true,
                    std::logic_error,
                    "wordexp error, code = " << we_error << " : Unquoted characters for input = '" << path << "'");
      case WRDE_BADVAL:
        //std::cerr << "ERROR ************* WRDE_BADVAL" << std::endl;
        DT_THROW_IF(true,
                    std::logic_error,
                    "wordexp error, code = " << we_error << " : Undefined shell variable for input = '" << path << "'");
      case WRDE_CMDSUB:
        //std::cerr << "ERROR ************* WRDE_CMDSUB" << std::endl;
        DT_THROW_IF(true,
                    std::logic_error,
                    "wordexp error, code = " << we_error << " : Unauthorized command substitution for input = '" << path << "'");
      case WRDE_NOSPACE:
        //std::cerr << "ERROR ************* WRDE_NOSPACE" << std::endl;
        // 2014-04-29 FM: Calling wordfree here at least (following the GNU example)
        wordfree( &p );
        // freed = true;
        DT_THROW_IF(true,
                    std::logic_error,
                    "wordexp error, code = " << we_error << " : Attempt to allocate memory failed for input = '" << path << "'");
      case WRDE_SYNTAX:
        //std::cerr << "ERROR ************* WRDE_SYNTAX" << std::endl;
        DT_THROW_IF(true,
                    std::logic_error,
                    "wordexp error, code = " << we_error << " : Shell syntax error for input = '" << path << "'");
      default:
        if (p.we_wordc == 0) {
          DT_THROW_IF(true, std::logic_error, "Nothing to expand !");
        } else {
          char** w = p.we_wordv;
          if (p.we_wordc > 1) {
            std::ostringstream message;
            message << "wordexp expands to many tokens : ";
            for (size_t i = 0; i < p.we_wordc; i++) {
              message << " '" << w[i]<< "'";
            }
            DT_THROW_IF(true, std::logic_error, message.str());
          } else {
            // std::ostringstream s;
            // s << w[0];
            text = w[0]; //s.str();
          }
        }
      } // switch (we_error)
    }
    catch (std::exception & we_x) {
      error_message = we_x.what();
    }
    // 2014-04-29 FM: Calling wordfree segfaults!!! (the manual is not clear about the need for wordfree)
    // if (! freed) {
    //   std::cerr << "FATAL ************* " << std::endl;
    //   usleep(100000);
    //   wordfree( &p );
    // }
    if (!error_message.empty()) {
      DT_THROW_IF(true,
                  std::logic_error,
                  "Shell expansion error: " << error_message);
    }
  } // if (_use_env_)
  path = text;

  if (! registered_lib_topic) {
    // If the path does not start with a "@libname[.XXX]:" prefix
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

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

// This Project
#include <datatools/ioutils.h>

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
  fetch_path_processor(std::string parent_path_ = "",
                       bool use_global_path_ = false);

  bool process(std::string&);

  void set_use_global_path(bool);

  bool use_global_path() const;

 private:
  bool process_impl(std::string &);
  bool _use_global_path_;
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


fetch_path_processor::fetch_path_processor(std::string parent_path_,
                                           bool use_global_path_) {
  _use_global_path_ = use_global_path_;
  _parent_path_ = parent_path_;
}


bool fetch_path_processor::process(std::string& path) {
  return this->process_impl(path);
}


bool fetch_path_processor::process_impl(std::string& path) {
  std::string::size_type dollar;
  std::string text = path;
  if (path.find('?') != path.npos ||
      path.find('*') != path.npos) {
      std::ostringstream message;
      message << "datatools::fetch_path_processor::process_impl: "
              << "Wildcard characters are not supported in path='" << path << "' !";
      throw std::logic_error(message.str());
   }
  {
    std::ostringstream s;
    wordexp_t p;
    int we_error = wordexp( text.c_str(), &p, WRDE_NOCMD|WRDE_SHOWERR|WRDE_UNDEF);
    if (we_error != 0) {
      std::ostringstream message;
      message << "datatools::fetch_path_processor::process_impl: "
              << "wordexp failed with code : " << we_error << "; broken path is='" << path << "' !";
      throw std::logic_error(message.str());
    }
    if (p.we_wordc == 0) {
      return false;
    }
    char** w = p.we_wordv;
    //std::cerr << "************ WORDEXP=";
    if (p.we_wordc > 1) {
      std::ostringstream message;
      message << "datatools::fetch_path_processor::process_impl: "
              << "wordexp expands to many tokens : ";
      for (size_t i = 0; i < p.we_wordc; i++) {
        //std::cerr << w[i];
        message << " '" << w[i]<< "'";
      }
      throw std::logic_error(message.str());
    }
    s << w[0];
    wordfree( &p );
    text = s.str();
  }
  path = text;

  // Old implementation :
  /*
  if (text.substr(0, 2) == "~/") {
    // Process special HOME dir :
    std::string tmp = text.substr(2);
    text = "${HOME}/" + tmp;
  } else if (text == "~") {
    // Process special HOME dir :
    text = "${HOME}";
  }
  */

  /*
  while ((dollar = text.find ('$')) != std::string::npos) {
    std::string::size_type slash = text.find('/', dollar + 1);
    std::string::size_type back_slash = text.find('\\', dollar + 1);
    std::string::size_type pos = std::string::npos;

    if (slash != std::string::npos) {
      if (back_slash != std::string::npos) {
        pos = std::min(slash, back_slash);
      } else {
        pos = slash;
      }
    } else {
      if (back_slash != std::string::npos) {
        pos = back_slash;
      } else {
        pos = std::string::npos;
      }
    }
    std::string env;
    if (pos == std::string::npos) {
      env = text.substr(dollar + 1, text.length() - (dollar + 1));
    } else {
      //     abc$xxx/ef
      //     0  3   7 9
      env = text.substr(dollar + 1, pos - (dollar + 1));
    }
    std::string env_candidate = env;
    if (!env.empty()) {
      if (env[0] == '{') {
        if (env[env.size() - 1] != '}') {
          return false;
        } else {
          env_candidate = env.substr(1, env.size() - 2);
        }
      }
    } else {
      return false;
    }
    if (env_candidate.empty()) {
      return false;
    }
    char* val = ::getenv(env_candidate.c_str());
    if (val) {
      std::string value = text.substr(0, dollar);
      value += val;
      if(pos != std::string::npos) {
        value += text.substr (pos, text.length() - pos);
      }
      text = value;
    }
    if (!val) return false; // Stop looping if env variable not found.
  }
  path = text;
  */

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
  path = text;
  return true;
}
/**************************************************/


bool fetch_path_with_env(std::string& path) {
  fetch_path_processor fpp("",false);
  return fpp.process(path);
}


bool fetch_path_with_env_p(std::string& path,
                           const std::string & parent_path_) {
  fetch_path_processor fpp(parent_path_, false);
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

  if (found > 0) {
    throw std::runtime_error("expand_path: found a not allowed character!");
  }

  std::ostringstream sh_cmd;
  char dummy[256];
  sprintf(dummy, "tmp.XXXXXX");
  int tmpfile = mkstemp(dummy);
  if (tmpfile == -1) {
    throw std::runtime_error("expand_path: Cannot create temporary file!");
  }
  sh_cmd << "echo \"" << path << "\" > " << dummy;
  int ret = system(sh_cmd.str().c_str());
  if (ret != 0) {
    throw std::runtime_error("expand_path: Cannot execute shell command!");
  }
  std::ifstream input(dummy);
  if (!input) {
    throw std::runtime_error("expand_path: Cannot open temporary file!");
  }
  std::string line_get;
  std::getline(input, line_get);
  if (!input) {
    throw std::runtime_error("expand_path: Cannot read temporary file!");
  }

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


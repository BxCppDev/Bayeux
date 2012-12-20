// -*- mode: c++; -*-
// utils.cc
// Ourselves 
#include <datatools/utils.h>

// Standard Library
#include <cstdlib>
#include <cmath>
#include <unistd.h>

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

namespace datatools {

void invalidate(double& x) {
  x = std::numeric_limits<double>::quiet_NaN();
}

bool is_valid(double x) {
  return x == x;
}

bool is_infinity(double x) {
  return isinf (x);
}

bool is_normal(double x) {
  return ! is_infinity (x) && is_valid (x);
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

/**************************************************/
class fetch_path_processor {
 public:
  fetch_path_processor();

  bool process(std::string&);

 private:
  bool process_impl(std::string &);
};


fetch_path_processor::fetch_path_processor() {}


bool fetch_path_processor::process(std::string& path) {
  return this->process_impl(path);
}


bool fetch_path_processor::process_impl(std::string& path) {
  std::string::size_type dollar;
  std::string text = path;
  if (text.substr(0, 2) == "~/") {
    std::string tmp = text.substr(2);
    text = "${HOME}/" + tmp;
  } else if (text == "~") {
    text = "${HOME}";
  }
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
  return true;
}
/**************************************************/


bool fetch_path_with_env(std::string& path) {
  fetch_path_processor fpp;
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


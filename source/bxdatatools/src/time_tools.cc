// -*- mode: c++; -*- 
// time_tools.cc
// Ourselves
#include <datatools/utils/time_tools.h>

// Standard Library
#include <cstdlib>
#include <cmath>
#include <sys/time.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <limits>

// Third Party
// - A

// This Project
#include <datatools/utils/clhep_units.h>
#include <datatools/utils/units.h>
#include <datatools/utils/utils.h>

namespace datatools {
namespace utils {

double computing_time::g_system_dead_time = 
    std::numeric_limits<double>::quiet_NaN();


// ctor:
computing_time::computing_time() {
  if (!datatools::utils::is_valid(g_system_dead_time)) {
    g_compute_system_dead_time();
  }
  this->reset();
}


// dtor:
computing_time::~computing_time() {}


bool computing_time::is_stopped() const {
  return ((start_.tv_sec == 0) && (start_.tv_usec == 0));
}


bool computing_time::is_started() const {
  return !this->is_stopped();
}


size_t computing_time::get_counts() const {
  return counts_;
}


double computing_time::get_last_elapsed_time() const {
  return last_elapsed_time_;
}


double computing_time::get_total_time() const {
  return this->get_sum_time() - 2 * this->get_counts()*g_system_dead_time;
}


double computing_time::get_sum_time() const {
  return sum_time_;
}


double computing_time::get_sum2_time() const {
  return sum2_time_;
}


double computing_time::get_min_time() const {
  return min_time_;
}


double computing_time::get_max_time() const {
  return max_time_;
}


double computing_time::get_mean_time() const {
  if (counts_ == 0) {
    return std::numeric_limits<double>::quiet_NaN();
  }
  return sum_time_ / counts_;
}


double computing_time::get_sigma_time() const {
  if (counts_ < 2) {
    return std::numeric_limits<double>::quiet_NaN();
  }
  double m = this->get_mean_time();
  double m2 = sum2_time_ / counts_;
  return sqrt(m2 - (m * m));
}


void computing_time::start() {
  this->resume();
}


void computing_time::stop() {
  this->pause();
  counts_++;
}


void computing_time::pause() {
  if((gettimeofday(&stop_, NULL)) == -1) {
    std::ostringstream message;
    message << "datatools::utils::computing_time::start: "
            << "gettimeofday failed !";
    throw std::runtime_error(message.str());
  }
  double elapsed_time = 0.0;
  timeval diff;
  g_timeval_subtract(stop_, start_, diff);
  elapsed_time = diff.tv_sec * CLHEP::second 
      + diff.tv_usec * CLHEP::microsecond;
  sum_time_ += elapsed_time;
  sum2_time_ += (elapsed_time * elapsed_time);

  if (!datatools::utils::is_valid(min_time_)) {
    min_time_ = elapsed_time;
  } else if (elapsed_time < min_time_) {
    min_time_ = elapsed_time;
  }

  if (!datatools::utils::is_valid(max_time_)) {
    max_time_ = elapsed_time;
  } else if (elapsed_time > max_time_) {
    max_time_ = elapsed_time;
  }

  last_elapsed_time_ = elapsed_time;
  start_.tv_sec = 0;
  start_.tv_usec = 0;
  stop_.tv_sec = 0;
  stop_.tv_usec = 0;
}


void computing_time::resume() {
  if((gettimeofday(&start_, NULL)) == -1) {
    std::ostringstream message;
    message << "datatools::utils::computing_time::resume: "
            << "gettimeofday failed !";
    throw std::runtime_error(message.str());
  }
}


void computing_time::reset() {
  start_.tv_sec = 0;
  start_.tv_usec = 0;
  stop_.tv_sec = 0;
  stop_.tv_usec = 0;
  counts_ = 0;
  sum_time_ = 0.0;
  sum2_time_ = 0.0;
  datatools::utils::invalidate(min_time_);
  datatools::utils::invalidate(max_time_);
  datatools::utils::invalidate(last_elapsed_time_);
}


void computing_time::tree_dump(std::ostream& a_out, 
                               const std::string& a_title,
                               const std::string& a_indent,
                               bool a_inherit) const {
  namespace du = datatools::utils;
  std::string indent;
  if (!a_indent.empty()) indent = a_indent;
  if (!a_title.empty()) a_out << indent << a_title << std::endl;

  a_out << indent << du::i_tree_dumpable::tag
        << "System dead time   : " 
        << g_system_dead_time / CLHEP::microsecond << " us" << std::endl;   

  if (counts_ == 0) {
    a_out << indent << du::i_tree_dumpable::inherit_tag(a_inherit)  
          << "No statistics" << std::endl;   
  } else {
    a_out << indent << du::i_tree_dumpable::tag 
          << "Count(s)      : " << counts_ << std::endl;
    a_out << indent << du::i_tree_dumpable::tag 
          << "Sum time      : " 
          << sum_time_ / CLHEP::second << " s" << std::endl;
    a_out << indent << du::i_tree_dumpable::tag 
          << "Min time      : " 
          << min_time_ / CLHEP::second << " s" << std::endl;
    a_out << indent << du::i_tree_dumpable::tag 
          << "Max time      : " 
          << max_time_ / CLHEP::second << " s" << std::endl;
    a_out << indent << du::i_tree_dumpable::tag 
          << "Sum squ. time : " 
          << sum2_time_ / (CLHEP::second * CLHEP::second) 
          << " s²" << std::endl;
    a_out << indent << du::i_tree_dumpable::tag 
          << "Total time    : " 
          << this->get_total_time() / CLHEP::second << " s" << std::endl;
    a_out << indent << du::i_tree_dumpable::tag 
          << "Mean time     : " 
          << this->get_mean_time() / CLHEP::second << " s" << std::endl;
    a_out << indent <<  du::i_tree_dumpable::tag  
          << "Sigma time    : ";
    if (std::isnan(this->get_sigma_time())) {
      a_out << '-';
    } else {
      a_out << this->get_sigma_time() / CLHEP::second << " s";
    }

    a_out << std::endl;
    a_out << indent <<  du::i_tree_dumpable::inherit_tag(a_inherit)  
          << "Last elapsed time : " 
          << last_elapsed_time_ / CLHEP::second << " s" << std::endl;
  }
}


bool computing_time::g_timeval_subtract(const timeval& a_start, 
                                        const timeval& a_stop,
                                        timeval& a_result) {
  timeval x = a_start;
  timeval y = a_stop;
  /* Perform the carry for the later subtraction by updating y. */
  if (x.tv_usec < y.tv_usec) {
    int nsec = (y.tv_usec - x.tv_usec) / 1000000 + 1;
    y.tv_usec -= 1000000 * nsec;
    y.tv_sec += nsec;
  }
  if (x.tv_usec - y.tv_usec > 1000000) {
    int nsec = (x.tv_usec - y.tv_usec) / 1000000;
    y.tv_usec += 1000000 * nsec;
    y.tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  a_result.tv_sec = x.tv_sec - y.tv_sec;
  a_result.tv_usec = x.tv_usec - y.tv_usec;

  /* Return 1 if result is negative. */
  if (x.tv_sec < y.tv_sec) {
    return false;
  }
  return true;
}


void computing_time::g_compute_system_dead_time() {
  // preliminary:
  g_system_dead_time = 0.0 * CLHEP::second;
  timeval start, stop;
  gettimeofday(&start, NULL);
  size_t n = 10;

  for (int i = 0; i < (n - 1); i++) {
    gettimeofday(&stop, NULL);      
  }

  double elapsed_time = 0.0;
  timeval diff;
  g_timeval_subtract(stop, start, diff);
  elapsed_time = diff.tv_sec * CLHEP::second 
      + diff.tv_usec * CLHEP::microsecond;
  g_system_dead_time = elapsed_time / n;

  std::clog << "NOTICE: " 
            << "computing_time::g_compute_system_dead_time: "
            << "System dead time = " 
            << g_system_dead_time / CLHEP::second  << " s"
            << std::endl;
}

} // namespace utils
} // namespace datatools 


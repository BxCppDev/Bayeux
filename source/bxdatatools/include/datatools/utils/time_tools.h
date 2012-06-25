// time_tools.h
#ifndef DATATOOLS_UTILS_TIME_TOOLS_H_
#define DATATOOLS_UTILS_TIME_TOOLS_H_
// Standard Library
#include <iostream>
#include <sys/time.h>

// Third Party
// - A

// This Project
#include <datatools/utils/i_tree_dump.h>

namespace datatools {
namespace utils {

class computing_time : public i_tree_dumpable {
 public:
  static double g_system_dead_time;

 public:
  // ctor:
  computing_time();

  // dtor:
  virtual ~computing_time();

  bool is_stopped() const;

  bool is_started() const;

  size_t get_counts() const;

  double get_last_elapsed_time() const;

  double get_total_time() const;

  double get_sum_time() const;

  double get_sum2_time() const;

  double get_min_time() const;

  double get_max_time() const;

  double get_mean_time() const;

  double get_sigma_time() const;

  void start();

  void stop();

  void pause();

  void resume();

  void reset();

  virtual void tree_dump(std::ostream& a_out = std::clog, 
                         const std::string& a_title = "",
                         const std::string& a_indent = "",
                         bool a_inherit = false) const;

  static bool g_timeval_subtract(const timeval& a_stop, 
                                 const timeval& a_start,
                                 timeval& a_result);

  static void g_compute_system_dead_time();


 private:
  timeval start_;
  timeval stop_;
  size_t  counts_;
  double  sum_time_;
  double  min_time_;
  double  max_time_;
  double  sum2_time_;
  double  last_elapsed_time_;
};

} // namespace utils
} // namespace datatools 

#endif // DATATOOLS_UTILS_TIME_TOOLS_H_


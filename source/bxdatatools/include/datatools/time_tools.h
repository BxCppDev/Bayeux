/// \file datatools/time_tools.h

#ifndef DATATOOLS_TIME_TOOLS_H
#define DATATOOLS_TIME_TOOLS_H

// Standard Library:
#include <iostream>
#include <sys/time.h>

// This Project:
#include <datatools/i_tree_dump.h>

namespace datatools {

  /**!  \struct computing_time
     \brief A class that compute time statistics
  */
  class computing_time
    : public i_tree_dumpable
  {
  public:

    static double system_dead_time();

    /// Constructor
    computing_time();

    /// Destructor
    ~computing_time() override;

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

    void tree_dump(std::ostream & out_ = std::clog,
         const std::string & title_ = "",
         const std::string & indent_ = "",
         bool inherit_ = false) const override;

    static bool g_timeval_subtract(const timeval & stop_,
           const timeval & start_,
           timeval & result_);

    static double compute_system_dead_time();

  private:

    timeval _start_;
    timeval _stop_;
    size_t  _counts_;
    double  _system_dead_time_;
    double  _sum_time_;
    double  _min_time_;
    double  _max_time_;
    double  _sum2_time_;
    double  _last_elapsed_time_;

  };

} // namespace datatools

#endif // DATATOOLS_TIME_TOOLS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

// -*- mode: c++; -*- 
// time_tools.cc

#include <cstdlib>
#include <cmath>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <sys/time.h>
#include <limits>

#include <datatools/utils/time_tools.h>
#include <datatools/utils/clhep_units.h>

#include <datatools/utils/utils.h>
#include <datatools/utils/units.h>

namespace datatools {

  namespace utils {

    using namespace std;

    double computing_time::g_system_dead_time = numeric_limits<double>::quiet_NaN ();

    size_t computing_time::get_counts () const
    {
      return _counts_;
    }

    double computing_time::get_last_elapsed_time () const
    {
      return _last_elapsed_time_;
    }

    double computing_time::get_total_time () const
    {
      return get_sum_time () - 2 * get_counts () *  g_system_dead_time;
    }

    double computing_time::get_sum_time () const
    {
      return _sum_time_;
    }

    double computing_time::get_sum2_time () const
    {
      return _sum2_time_;
    }

    double computing_time::get_min_time () const
    {
      return _min_time_;
    }

    double computing_time::get_max_time () const
    {
      return _max_time_;
    }
 
    double computing_time::get_mean_time () const
    {
      if (_counts_ == 0) 
        {
          return numeric_limits<double>::quiet_NaN ();
        }
      return _sum_time_ / _counts_;
    }

    double computing_time::get_sigma_time () const
    {
      if (_counts_ < 2) 
        {
          return numeric_limits<double>::quiet_NaN ();
        }
      double m = get_mean_time ();
      double m2 = _sum2_time_ / _counts_;
      return sqrt (m2 - (m * m));
    }

    // ctor:
    computing_time::computing_time ()
    {
      if (! datatools::utils::is_valid (g_system_dead_time))
        {
          g_compute_system_dead_time ();
        }
      reset ();
      return;
    }
    
    // dtor:
    computing_time::~computing_time ()
    {
      return;
    }

    void computing_time::resume ()
    {
      if((gettimeofday(&_start_, NULL)) == -1)
         {
           ostringstream message;
           message << "datatools::utils::computing_time::resume: "
                   << "gettimeofday failed !";
           throw runtime_error (message.str ());
         }
       return;
     }
    
    void computing_time::start ()
    {
      resume();
      return;
    }
 
    void computing_time::stop ()
    {
      pause ();
      _counts_++;
      return;
    }
    
    void computing_time::pause ()
    {
      if((gettimeofday(&_stop_, NULL)) == -1)
         {
           ostringstream message;
           message << "datatools::utils::computing_time::start: "
                   << "gettimeofday failed !";
           throw runtime_error (message.str ());
         }
      double elapsed_time = 0.0;
      timeval diff;
      g_timeval_subtract (_stop_, _start_, diff);
      elapsed_time = diff.tv_sec * CLHEP::second 
        + diff.tv_usec * CLHEP::microsecond;
      _sum_time_ += elapsed_time;
      _sum2_time_ += (elapsed_time * elapsed_time);
      if (! datatools::utils::is_valid (_min_time_))
        {
          _min_time_ = elapsed_time;
        }
      else if (elapsed_time < _min_time_)
        {
          _min_time_ = elapsed_time;
        }
      if (! datatools::utils::is_valid (_max_time_))
        {
          _max_time_ = elapsed_time;
        }
      else if (elapsed_time > _max_time_)
        {
          _max_time_ = elapsed_time;
        }
      _last_elapsed_time_ = elapsed_time;
      _start_.tv_sec = 0;
      _start_.tv_usec = 0;
      _stop_.tv_sec = 0;
      _stop_.tv_usec = 0;
      return;
    }

    void computing_time::reset ()
    {
      _start_.tv_sec = 0;
      _start_.tv_usec = 0;
      _stop_.tv_sec = 0;
      _stop_.tv_usec = 0;
      _counts_ = 0;
      _sum_time_ = 0.0;
      _sum2_time_ = 0.0;
      datatools::utils::invalidate (_min_time_);
      datatools::utils::invalidate (_max_time_);
      datatools::utils::invalidate (_last_elapsed_time_);
      return;
    }

    bool computing_time::is_stopped () const
    {
      return ((_start_.tv_sec == 0) && (_start_.tv_usec == 0));
    }

    bool computing_time::is_started () const
    {
      return ! is_stopped ();
    }

    bool computing_time::g_timeval_subtract (const timeval & a_start, 
                                             const timeval & a_stop,
                                             timeval & a_result)
    {
      timeval x = a_start;
      timeval y = a_stop;
      /* Perform the carry for the later subtraction by updating y. */
      if (x.tv_usec < y.tv_usec) 
        {
          int nsec = (y.tv_usec - x.tv_usec) / 1000000 + 1;
          y.tv_usec -= 1000000 * nsec;
          y.tv_sec += nsec;
        }
      if (x.tv_usec - y.tv_usec > 1000000) 
        {
          int nsec = (x.tv_usec - y.tv_usec) / 1000000;
          y.tv_usec += 1000000 * nsec;
          y.tv_sec -= nsec;
        }
      
      /* Compute the time remaining to wait.
         tv_usec is certainly positive. */
      a_result.tv_sec = x.tv_sec - y.tv_sec;
      a_result.tv_usec = x.tv_usec - y.tv_usec;

      /* Return 1 if result is negative. */
      if ( x.tv_sec < y.tv_sec)
        {
          return false;
        }
      return true;
    }
 
    void computing_time::g_compute_system_dead_time ()
    {
      // preliminary:
      g_system_dead_time = 0.0 * CLHEP::second;
      timeval start, stop;
      gettimeofday(&start, NULL);
      size_t n = 10;
      for (int i = 0; i < (n - 1); i++)
        {
          gettimeofday(&stop, NULL);      
        }  
      double elapsed_time = 0.0;
      timeval diff;
      g_timeval_subtract (stop, start, diff);
      elapsed_time = diff.tv_sec * CLHEP::second 
        + diff.tv_usec * CLHEP::microsecond;
      g_system_dead_time = elapsed_time / n;
      clog << "NOTICE: " 
           << "computing_time::g_compute_system_dead_time: "
           << "System dead time = " << g_system_dead_time / CLHEP::second  << " s"
           << endl;
      return;
    }

    void computing_time::tree_dump (std::ostream & a_out, 
                                    const std::string & a_title,
                                    const std::string & a_indent,
                                    bool a_inherit) const
    {
      namespace du = datatools::utils;
      std::string indent;
      if (! a_indent.empty () ) indent = a_indent;
      if (! a_title.empty ()) 
        {
          a_out << indent << a_title << std::endl;
        }
      
      a_out << indent << du::i_tree_dumpable::tag
           << "System dead time   : " << g_system_dead_time / CLHEP::microsecond << " us" << std::endl;   

      if (_counts_ == 0)
        {
          a_out << indent << du::i_tree_dumpable::inherit_tag (a_inherit)  
               << "No statistics" << std::endl;   
        }
      else
        {
          a_out << indent << du::i_tree_dumpable::tag 
               << "Count(s)      : " << _counts_ << std::endl;
          a_out << indent << du::i_tree_dumpable::tag 
               << "Sum time      : " << _sum_time_ / CLHEP::second << " s" << std::endl;
          
          a_out << indent << du::i_tree_dumpable::tag 
               << "Min time      : " << _min_time_ / CLHEP::second << " s" << std::endl;
          
          a_out << indent << du::i_tree_dumpable::tag 
               << "Max time      : " <<  _max_time_ / CLHEP::second << " s" << std::endl;
          
          a_out << indent << du::i_tree_dumpable::tag 
               << "Sum squ. time : " 
               << _sum2_time_ / (CLHEP::second * CLHEP::second) << " s²" << std::endl;

          a_out << indent << du::i_tree_dumpable::tag 
               << "Total time    : " <<  get_total_time () / CLHEP::second << " s" << std::endl;
          
          a_out << indent << du::i_tree_dumpable::tag 
               << "Mean time     : " <<  get_mean_time () / CLHEP::second << " s" << std::endl;

          a_out << indent <<  du::i_tree_dumpable::tag  
               << "Sigma time    : ";
          if (isnan (get_sigma_time ())) a_out << '-';
          else a_out << get_sigma_time () / CLHEP::second << " s";
          a_out << std::endl;

          a_out << indent <<  du::i_tree_dumpable::inherit_tag (a_inherit)  
               << "Last elapsed time : " <<  _last_elapsed_time_ / CLHEP::second << " s" << std::endl;
        }

      return;
    }
    

  } // namespace utils

} // namespace datatools 

// end of time_tools.cc

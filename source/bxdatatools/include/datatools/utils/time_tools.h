// time_tools.h

#ifndef __datatools__utils__time_tools_h
#define __datatools__utils__time_tools_h 1

#include <iostream>
#include <sys/time.h>

#include <datatools/utils/i_tree_dump.h>

namespace datatools {
  
  namespace utils {

    class computing_time : public i_tree_dumpable
    {
    public:

      static double g_system_dead_time;

    public:

      bool is_stopped () const;

      bool is_started () const;

      size_t get_counts () const;

      double get_last_elapsed_time () const;

      double get_total_time () const;

      double get_sum_time () const;

      double get_sum2_time () const;

      double get_min_time () const;

      double get_max_time () const;

      double get_mean_time () const;

      double get_sigma_time () const;

      // ctor:
      computing_time ();

      // dtor:
      virtual ~computing_time ();

      void start ();

      void stop ();

      void pause ();

      void resume ();

      void reset ();

      virtual void tree_dump (std::ostream & a_out = std::clog, 
			      const std::string & a_title = "",
			      const std::string & a_indent = "",
			      bool a_inherit = false) const;

      static bool g_timeval_subtract (const timeval & a_stop, 
				      const timeval & a_start,
				      timeval & a_result);

      static void g_compute_system_dead_time ();

    private:
      
      timeval _start_;
      timeval _stop_;
      size_t  _counts_;
      double  _sum_time_;
      double  _min_time_;
      double  _max_time_;
      double  _sum2_time_;
      double  _last_elapsed_time_;

    };

    
  } // namespace utils

} // namespace datatools 

#endif // __datatools__utils__time_tools_h

// end of time_tools.h

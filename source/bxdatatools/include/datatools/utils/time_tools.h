// time_tools.h

#ifndef __datatools__utils__time_tools_h
#define __datatools__utils__time_tools_h 1


#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <sys/time.h>
#include <limits>

#include <datatools/utils/utils.h>
#include <datatools/utils/units.h>
#include <datatools/utils/i_tree_dump.h>

namespace datatools {
  
  namespace utils {

    class computing_time : public i_tree_dumpable
    {
    public:

      static double g_system_dead_time;

    private:
      
      timeval __start;
      timeval __stop;
      size_t  __counts;
      double  __sum_time;
      double  __min_time;
      double  __max_time;
      double  __sum2_time;
      double  __last_elapsed_time;

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

      void reset ();

      virtual void tree_dump (std::ostream & out_ = std::clog, 
			      const std::string & title_ = "",
			      const std::string & indent_ = "",
			      bool inherit_ = false) const;


    public:

      static bool g_timeval_subtract (const timeval & stop_, 
				      const timeval & start_,
				      timeval & result_);

      static void g_compute_system_dead_time ();

    };

    
  } // namespace utils

} // namespace datatools 

#endif // __datatools__utils__time_tools_h

// end of time_tools.h

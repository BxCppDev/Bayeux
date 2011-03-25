// -*- mode: c++; -*- 
// time_tools.cc

#include <datatools/utils/time_tools.h>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

namespace datatools {

  namespace utils {

    using namespace std;

    double computing_time::g_system_dead_time = numeric_limits<double>::quiet_NaN ();

    size_t computing_time::get_counts () const
    {
      return __counts;
    }

    double computing_time::get_last_elapsed_time () const
    {
      return __last_elapsed_time;
    }

    double computing_time::get_total_time () const
    {
      return get_sum_time () - 2 * get_counts () *  g_system_dead_time;
    }

    double computing_time::get_sum_time () const
    {
      return __sum_time;
    }

    double computing_time::get_sum2_time () const
    {
      return __sum2_time;
    }

    double computing_time::get_min_time () const
    {
      return __min_time;
    }

    double computing_time::get_max_time () const
    {
      return __max_time;
    }
 
    double computing_time::get_mean_time () const
    {
      if (__counts == 0) 
	{
	  return numeric_limits<double>::quiet_NaN ();
	}
      return __sum_time / __counts;
    }

    double computing_time::get_sigma_time () const
    {
      if (__counts < 2) 
	{
	  return numeric_limits<double>::quiet_NaN ();
	}
      double m = get_mean_time ();
      double m2 = __sum2_time / __counts;
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
    }
    
    // dtor:
    computing_time::~computing_time ()
    {
    }
    
    void computing_time::start ()
    {
       if((gettimeofday(&__start, NULL)) == -1)
	 {
	   ostringstream message;
	   message << "datatools::utils::computing_time::start: "
		   << "gettimeofday failed !";
	   throw runtime_error (message.str ());
	 }
       return;
    }
    
    void computing_time::stop ()
    {
      if((gettimeofday(&__stop, NULL)) == -1)
	 {
	   ostringstream message;
	   message << "datatools::utils::computing_time::start: "
		   << "gettimeofday failed !";
	   throw runtime_error (message.str ());
	 }
      double elapsed_time = 0.0;
      timeval diff;
      g_timeval_subtract (__stop, __start, diff);
      elapsed_time = diff.tv_sec * CLHEP::second 
	+ diff.tv_usec * CLHEP::microsecond;
      __sum_time += elapsed_time;
      __sum2_time += (elapsed_time * elapsed_time);
      __counts++;
      if (! datatools::utils::is_valid (__min_time))
	{
	  __min_time = elapsed_time;
	}
      else if (elapsed_time < __min_time)
	{
	  __min_time = elapsed_time;
	}
      if (! datatools::utils::is_valid (__max_time))
	{
	  __max_time = elapsed_time;
	}
      else if (elapsed_time > __max_time)
	{
	  __max_time = elapsed_time;
	}
      __last_elapsed_time = elapsed_time;
      __start.tv_sec = 0;
      __start.tv_usec = 0;
      __stop.tv_sec = 0;
      __stop.tv_usec = 0;
      return;
    }

    void computing_time::reset ()
    {
      __start.tv_sec = 0;
      __start.tv_usec = 0;
      __stop.tv_sec = 0;
      __stop.tv_usec = 0;
      __counts = 0;
      __sum_time = 0.0;
      __sum2_time = 0.0;
      datatools::utils::invalidate (__min_time);
      datatools::utils::invalidate (__max_time);
      datatools::utils::invalidate (__last_elapsed_time);
      return;
    }

    bool computing_time::is_stopped () const
    {
      return ((__start.tv_sec == 0) && (__start.tv_usec == 0));
    }

    bool computing_time::is_started () const
    {
      return ! is_stopped ();
    }

    bool computing_time::g_timeval_subtract (const timeval & x_, 
					     const timeval & y_,
					     timeval & result_)
    {
      timeval x = x_;
      timeval y = y_;
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
      result_.tv_sec = x.tv_sec - y.tv_sec;
      result_.tv_usec = x.tv_usec - y.tv_usec;

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

    void computing_time::tree_dump (std::ostream & out_, 
				    const std::string & title_,
				    const std::string & indent_,
				    bool inherit_) const
    {
      namespace du = datatools::utils;
      std::string indent;
      if (! indent_.empty () ) indent = indent_;
      if (! title_.empty ()) 
	{
	  out_ << indent << title_ << std::endl;
	}
      
      out_ << indent << du::i_tree_dumpable::tag
	   << "System dead time   : " << g_system_dead_time / CLHEP::microsecond << " us" << std::endl;	  

      if (__counts == 0)
	{
	  out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)  
	       << "No statistics" << std::endl;	  
	}
      else
	{
	  out_ << indent << du::i_tree_dumpable::tag 
	       << "Counts        : " << __counts << std::endl;
	  out_ << indent << du::i_tree_dumpable::tag 
	       << "Sum time      : " << __sum_time / CLHEP::second << " s" << std::endl;
	  
	  out_ << indent << du::i_tree_dumpable::tag 
	       << "Min time      : " << __min_time / CLHEP::second << " s" << std::endl;
	  
	  out_ << indent << du::i_tree_dumpable::tag 
	       << "Max time      : " <<  __max_time / CLHEP::second << " s" << std::endl;
	  
	  out_ << indent << du::i_tree_dumpable::tag 
	       << "Sum squ. time : " 
	       << __sum2_time / (CLHEP::second * CLHEP::second) << " s²" << std::endl;

	  out_ << indent << du::i_tree_dumpable::tag 
	       << "Total time    : " <<  get_total_time () / CLHEP::second << " s" << std::endl;
	  
	  out_ << indent << du::i_tree_dumpable::tag 
	       << "Mean time     : " <<  get_mean_time () / CLHEP::second << " s" << std::endl;

	  out_ << indent <<  du::i_tree_dumpable::tag  
	       << "Sigma time    : " <<  get_sigma_time () / CLHEP::second << " s" << std::endl;

	  out_ << indent <<  du::i_tree_dumpable::inherit_tag (inherit_)  
	       << "Last elapsed time : " <<  __last_elapsed_time / CLHEP::second << " s" << std::endl;
	}

      return;
    }
    

  } // namespace utils

} // namespace datatools 

// end of time_tools.cc

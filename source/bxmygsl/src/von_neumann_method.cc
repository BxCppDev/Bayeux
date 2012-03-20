// mygsl::von_neumann_method.cc

#include <mygsl/von_neumann_method.h>

#include <cmath>
#include <sstream>
#include <stdexcept>
#include <limits>
 
namespace mygsl {

  using namespace std;

  bool von_neumann_method::g_debug = false;

  const double  von_neumann_method::AUTO_FMAX = 0.0;

  bool von_neumann_method::is_initialized () const
  {
    return _func_ != 0;
  }

  double von_neumann_method::get_xmin () const
  {
    return _xmin_;
  }

  double von_neumann_method::get_xmax () const
  {
    return _xmax_;
  }

  double von_neumann_method::get_fmax () const
  {
    return _fmax_;
  }

  size_t von_neumann_method::get_max_counts () const
  {
    return _max_counts_;
  }


  von_neumann_method::von_neumann_method ()
  {
    _xmin_ = std::numeric_limits<double>::quiet_NaN ();
    _xmax_ = std::numeric_limits<double>::quiet_NaN ();
    _fmax_ = std::numeric_limits<double>::quiet_NaN ();
    _func_ = 0;
    _max_counts_ = DEFAULT_MAX_COUNTS;
    return;
  }

  von_neumann_method::von_neumann_method (double xmin_, 
                                          double xmax_, 
                                          unary_eval & func_, 
                                          double fmax_,
                                          size_t nsamples_,
                                          size_t max_counts_)
  {
    _xmin_ = std::numeric_limits<double>::quiet_NaN ();
    _xmax_ = std::numeric_limits<double>::quiet_NaN ();
    _fmax_ = std::numeric_limits<double>::quiet_NaN ();
    _func_ = 0;
    _max_counts_ = DEFAULT_MAX_COUNTS;
    init (xmin_, xmax_, func_, fmax_, nsamples_, max_counts_);
    return;
  }

  void von_neumann_method::dump (std::ostream & out_) const
  {
    out_ << "von_neumann_method:" << std::endl;
    out_ << "|-- x(min)      : " << _xmin_ << std::endl;
    out_ << "|-- x(max)      : " << _xmax_ << std::endl;
    out_ << "|-- f(max)      : " << _fmax_ << std::endl;
    out_ << "|-- func        : " << std::hex << _func_ 
         << std::dec << std::endl;
    out_ << "`-- counts(max) : " << _max_counts_ << std::endl;
    return;
  }

  void von_neumann_method::reset ()
  {
    if (! is_initialized ()) return;
    _xmin_ = std::numeric_limits<double>::quiet_NaN ();
    _xmax_ = std::numeric_limits<double>::quiet_NaN ();
    _fmax_ = std::numeric_limits<double>::quiet_NaN ();
    _func_ = 0;
    _max_counts_ = DEFAULT_MAX_COUNTS;
    return;
  }

  void von_neumann_method::init (double xmin_, 
                                 double xmax_, 
                                 unary_eval & func_, 
                                 double fmax_,
                                 size_t nsamples_,
                                 size_t max_counts_)
  {
    using namespace std;
    if (is_initialized ()) 
      {
        ostringstream message;
        message << "von_neumann_method::init: "
                << "Method is already initalized !";
        throw runtime_error (message.str ());
      }
    if (g_debug)
      {
        clog << "DEBUG: von_neumann_method::init: Entering..." 
             << endl;
      }
    if (xmin_ > xmax_)
      {
        throw runtime_error ("von_neumann_method::init: invalid range!");
      }
    _xmin_ = xmin_;
    _xmax_ = xmax_;
    if (max_counts_ < 10)
      {
        throw runtime_error ("von_neumann_method::init: invalid maximum number of tries!");
      }
    _max_counts_ = max_counts_;
    _func_ = &func_;

    if (fmax_ > AUTO_FMAX)
      {
        _fmax_ = fmax_;
      }
    else 
      {
        if (g_debug)
          {
            clog << "DEBUG: von_neumann_method::init: searching for 'f(max)'..." 
                 << endl;
          }
        double fmax = -1.0;
        double f1 = -1.0;
        double f2 = -1.0;
        double dfmax = -1.0;
        if (nsamples_ < 2)
          {
            throw runtime_error ("von_neumann_method::init: Invalid sampling!");
          }
        double dx = (_xmax_ - _xmin_) / nsamples_;
        for (double x = _xmin_;
             x <  (_xmax_ + 0.5 *dx);
             x += dx)
          {
            double f = (*_func_) (x);
            if (f1 < 0.0)
              {
                f1 = f;
              }
            else 
              {
                f1 = f2;
              }
            f2 = f;
            if ( abs (f) > fmax)
              {
                fmax = abs (f);
                double df = (f2 - f1) / dx;
                if (abs (df) > dfmax) dfmax = abs (df);
              }
          }
        _fmax_ = fmax + dfmax * 2. * dx;
        if (g_debug)
          {
            clog << "DEBUG: von_neumann_method::init: found 'f(max)' = "
                 << _fmax_ << endl;
          }
      }
    if (g_debug)
      {
        clog << "DEBUG: von_neumann_method::init: Exiting." 
             << endl;
      }
    return;
  }

  double von_neumann_method::shoot (rng & ran_)
  {
    double res;
    size_t max_counts = _max_counts_;
    size_t counts = 0;
    while (true)
      {
        double x = ran_.flat (_xmin_, _xmax_);
        double y = ran_.flat (0.0, _fmax_);
        counts++;
        double f = (*_func_) (x);
        if (y < f)
          {
            res = x;
            break;
          } 
        if ((max_counts > 0) && (counts > max_counts))
          {
            throw runtime_error ("von_neumann_method::shoot: maximum number of tries has been reached!");
          }
      } 
    return res;
  }

}

// end of mygsl::von_neumann_method.cc


// ioutils.cc

#include <mygsl/ioutils.h>

#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <limits>

namespace mygsl
{

  using namespace std;

  const string ioutils::NAN_STRING     = "{NaN}";
  const string ioutils::INF_POS_STRING = "{+Inf}";
  const string ioutils::INF_NEG_STRING = "{-Inf}";

  ostream & ostream_odouble (ostream & out_, const double & x_)
    {
      double x = x_;
      if (isnan (x))
        {
          out_ << ioutils::NAN_STRING;
        }
      else if (isinf (x))
        {
          if (x > 0.0) out_ << ioutils::INF_POS_STRING;
          else  out_ << ioutils::INF_NEG_STRING;
        }
      else
        {
          out_ << x;
        }
      return out_;
    }

  OstreamManipulator<double> odouble (const double & x_) 
  {
    return OstreamManipulator<double> (&ostream_odouble, x_);
  }
  
  istream & istream_idouble (istream & in_, double & x_) 
  {
    double * x = &x_;
    char c = 0;
    in_.get (c);
    if (!in_)
      {
        return in_;
      }
    double val = numeric_limits<double>::quiet_NaN ();
    if (c != '{')
      {
        in_.putback (c);
        in_ >> val;
        if (! in_)
          {
            return in_;
          }
        else
          {
            *x = val;
          }
      }
    else
      {
        char tmp [5];
        tmp[4] = '\0';
        in_.get (tmp, 5, '}');
        if (! in_)
          {
            return in_;
          }
        string s = tmp;
        if (s == ioutils::NAN_STRING.substr(1, 3))
          {
            val = numeric_limits<double>::quiet_NaN ();
          }
        else if (s == ioutils::INF_POS_STRING.substr(1, 4))
          {
            val = numeric_limits<double>::infinity ();
          }
        else if (s == ioutils::INF_NEG_STRING.substr(1, 4))
          {
            val = -numeric_limits<double>::infinity ();
          }
        else
          {
            in_.setstate (ios::failbit);
            return in_;
          }
        in_.get (c);
        if (! in_)
          {
            return in_;
          }
        if (c != '}')
          {
            in_.setstate (ios::failbit);
            return in_;
          }
        else
          {
            *x = val;
          }
      }
    return in_;
  }

  IstreamManipulatorRef<double> idouble (double & x_) 
  {
    return IstreamManipulatorRef<double> (&istream_idouble, x_);
  }

} // namespace mygsl

// end of ioutils.cc

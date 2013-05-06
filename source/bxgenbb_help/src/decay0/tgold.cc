// tgold.cc

#include <genbb_help/decay0/tgold.h>
#include <sstream>
#include <stdexcept>

namespace genbb {
  namespace decay0 {
    
    // From : http://en.wikipedia.org/wiki/Golden_section_search 
    // a and c are the current bounds; the minimum is between them.
    // b is a center point
    // f(x,params) is some mathematical function elsewhere defined
    // a corresponds to x1; b corresponds to x2; c corresponds to x3
    // x corresponds to x4
    // minmax = 1 -> Search for minimum
    // minmax = 2 -> Search for maximum
    void decay0_tgold(double a, double b, double c, func_type f, 
                      double eps, int minmax,
                      double & xextr, double & fextr,
                      void * params)
    {
      double sign = 1.0;
      if (minmax == 2) sign = -1.0;
      static double phi = (1 + std::sqrt(5)) / 2;
      static double resphi = 2 - phi;
      double x;
      if (c - b > b - a)
        {
          x = b + resphi * (c - b);
        }
      else
        {
          x = b - resphi * (b - a);
        }
      if (std::abs(c - a) < eps * (std::abs(b) + std::abs(x))) 
        {
          xextr= (c + a) / 2;
          fextr=f(xextr,params);
          return ;
        }
      if (f(x,params) == f(b,params))
        {
          std::ostringstream message;
          message << "genbb::decay0::decay0_tgold: "
                  << "Invalid case : f(x,params) == f(b,params) !";
          throw std::logic_error(message.str());          
        }
      if (sign*f(x,params) < sign*f(b,params)) {
        if (c - b > b - a) 
          {
            decay0_tgold(b, x, c, f, eps, minmax, xextr, fextr, params);
            return;
          }
        else 
          {
            decay0_tgold(a, x, b, f, eps, minmax, xextr, fextr, params);
            return;
          }
      }
      else {
        if (c - b > b - a) 
          {
            decay0_tgold(a, b, x, f, eps, minmax, xextr, fextr, params);
            return;
          }
        else 
          {
            decay0_tgold(x, b, c, f, eps, minmax, xextr, fextr, params);
            return;
          }
      }
      return;
    }

  } // end of namespace decay0 
} // end of namespace genbb 
// 
// end of tgold.cc
// 
// Local Variables: --
// mode: c++ --
// End: --

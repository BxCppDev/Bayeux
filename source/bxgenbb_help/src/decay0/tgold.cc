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
    void decay0_tgold_o(double a, double b, double c, func_type f,
                      double eps, int minmax,
                      double & xextr, double & fextr,
                      void * params)
    {
      double sign = 1.0;
      if (minmax == 2) sign = -1.0;
      static double phi = (1 + std::sqrt(5)) / 2;
      static double resphi = 2 - phi;
      double x;
      if (c - b > b - a) {
        x = b + resphi * (c - b);
      } else {
        x = b - resphi * (b - a);
      }
      if (std::abs(c - a) < eps * (std::abs(b) + std::abs(x))) {
        xextr= (c + a) / 2;
        fextr=f(xextr,params);
        return ;
      }
      if (f(x,params) == f(b,params)) {
        std::ostringstream message;
        message << "genbb::decay0::decay0_tgold: "
                << "Invalid case : f(x,params) == f(b,params) !";
        throw std::logic_error(message.str());
      }
      if (sign*f(x,params) < sign*f(b,params)) {
        if (c - b > b - a) {
          decay0_tgold(b, x, c, f, eps, minmax, xextr, fextr, params);
          return;
        } else {
          decay0_tgold(a, x, b, f, eps, minmax, xextr, fextr, params);
          return;
        }
      } else {
        if (c - b > b - a) {
          decay0_tgold(a, b, x, f, eps, minmax, xextr, fextr, params);
          return;
        } else {
          decay0_tgold(x, b, c, f, eps, minmax, xextr, fextr, params);
          return;
        }
      }
      return;
    }


    void decay0_tgold(double a, double /*b*/, double c, func_type f,
                        double eps, int minmax,
                        double & xextr, double &fextr,
                        void * params)
    {
      /*
       * Subroutine tgold determines maximum or minimum of the function f(x) in
       * the interval [a,c] by the gold section method.
       * Call :   external f
       *          call tgold(a,b,f,eps,minmax,xextr,fextr)
       * Input:   a,c    - beginning and the end of the interval [a,c];
       *          b      - dummy value
       *          f      - name of the external function; should be as
       *                        function f(x), and additional parameters have
       *                        to be transmitted with help of common blocks;
       *          eps    - desired uncertainty of xextr determination;
       *          minmax - if minmax = 1 minimum of f(x) is searched,
       *                             = 2 maximum of f(x) is searched.
       * Output:  xextr  - x-point of extremum;
       *          fextr  - f(xextr).
       * V.Tretyak, 25.09.1985.
       */
      double qc = 0.61803395;
      double al = a;
      double bl = c;
      double xp = al+(bl-al)*qc;
      double xl = bl-(bl-al)*qc;
      double yp = f(xp,params);
      double yl=f(xl,params);
    label_3:
      if (minmax == 1) goto label_4;
      else if (minmax == 2) goto label_5;
    label_4:
      if(yp < yl) goto label_1;
      goto label_6;
    label_5:
      if (yp > yl) goto label_1;
    label_6:
      bl=xp;
      xp=xl;
      yp=yl;
      xl=bl-(bl-al)*qc;
      yl=f(xl,params);
      goto label_2;
    label_1:
      al=xl;
      xl=xp;
      yl=yp;
      xp=al+(bl-al)*qc;
      yp=f(xp,params);
    label_2:
      if ((bl-al)>eps) goto label_3;
      xextr=0.5*(al+bl);
      fextr=f(xextr,params);
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

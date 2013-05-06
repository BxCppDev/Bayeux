// tsimpr.cc

#include <genbb_help/decay0/tsimpr.h>

namespace genbb {
  namespace decay0 {
    
    double decay0_tsimpr(func_type f, double a, double b, double h, 
                         void * params)
    {
      int n=(int) ((b-a)/h) -1;
      double fi=f(a,params)-f(b,params);
      double x1=a;
      double x2=a;
      int ni=1;
      do {
        x1=x2+h;
        x2=x1+h;
        fi=fi+4.*f(x1,params)+2.*f(x2,params);
        ni=ni+2;
      }
      while (ni < n);
      return fi*h/3.;     
    }

  } // end of namespace decay0 
} // end of namespace genbb 
// 
// end of tsimpr.cc
// 
// Local Variables: --
// mode: c++ --
// End: --

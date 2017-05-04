
#include <cmath>

#include <gsl/gsl_math.h>

#include <mygsl/rng.h>

#include <genbb_help/primary_event.h>

#include <genbb_help/decay0/gdrot.h>

namespace genbb {
  namespace decay0 {
  
    void decay0_gdrot(double * p,
                      double costh,double sinth,
                      double cosph,double sinph)
    {
      double p1 = p[0];
      double p2 = p[1];
      double p3 = p[2];
      p[0] =  p1*costh*cosph - p2*sinph + p3*sinth*cosph;
      p[1] =  p1*costh*sinph + p2*cosph + p3*sinth*sinph;
      p[2] = -p1*sinth                  + p3*costh;
      return;
    }
    
  } // end of namespace decay0 
} // end of namespace genbb 

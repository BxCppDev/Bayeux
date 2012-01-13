#include <iostream>
#include <sstream>
#include <cmath>
#include <stdexcept>
#include <gsl/gsl_interp.h>
#include <gsl/gsl_spline.h>

#ifndef __genbb_help__divdif_h
#define __genbb_help__divdif_h 1

extern "C"
{

  float divdif_wrap (float * f_, float * a_, int32_t * n_, 
		     float * x_, int32_t * degree_);

}

#endif // __genbb_help__divdif_h 1

// end of divdif.h

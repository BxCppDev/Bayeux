// gauss.cc

#include <cmath>
#include <sstream>
#include <stdexcept>

#include <mygsl/rng.h>

#include <genbb_help/decay0/gauss.h>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_errno.h>

namespace genbb {
  namespace decay0 {
    
    double decay0_gauss(func_type f_, double min_, double max_, double relerr_, 
                        void * params)
    {
      double result, abserr;
      size_t neval;
      gsl_function F;
      F.function = f_;
      F.params   = params;
      int status = gsl_integration_qng(&F, min_, max_, 0.0, relerr_,
                                       &result, &abserr, &neval);
      if (status != 0)
        {
          std::ostringstream message;
          message << "genbb::decay0::decay0_gauss: "
                  << "GSL QNG integration error '"
                  << gsl_strerror(status)
                  << "' !";
          throw std::runtime_error(message.str());
        }
      return result;
    }

  } // end of namespace decay0 
} // end of namespace genbb 
// 
// end of gauss.cc
// 
// Local Variables: --
// mode: c++ --
// End: --

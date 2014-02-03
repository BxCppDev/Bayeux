// gauss.cc

// Standard library:
#include <cmath>
#include <sstream>
#include <stdexcept>

// Third party:
// - GSL:
#include <gsl/gsl_errno.h>
#include <gsl/gsl_integration.h>
// - Bayeux/datatools:
#include <datatools/logger.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>
// - Bayeux/genbb_help:
#include <genbb_help/decay0/gauss.h>

namespace genbb {
  namespace decay0 {

    double decay0_gauss(func_type f_, double min_, double max_, double epsrel_,
                        void * params)
    {
      bool devel = false;
      if (devel) {
        std::cerr << "DEVEL: genbb::decay0::decay0_gauss: Entering..." << std::endl;
      }
      double result, abserr;
      size_t neval;
      gsl_function F;
      F.function = f_;
      F.params   = params;
      double epsabs = 1e-9;
      double epsrel = epsrel_;
      epsabs = 0.0;
      /*
      if (devel) {
        double s = 0.0;
        double dx =  0.1 * (max_ - min_);
        for (double x = min_ + 0.5 *dx; x < max_; x += dx) {
          double f = f_(x, params);
          s += f;
          //std::cerr << "DEVEL: genbb::decay0::decay0_gauss: x=" << x << " f=" << f << std::endl;
        }
        s *= dx;
        //std::cerr << "DEVEL: genbb::decay0::decay0_gauss: s=" << s << std::endl;
      }
      */
      /*
      double dx = (max_ - min_) / 6.0;
      double x1 = min_ + dx;
      double x3 = min_ + 3* dx;
      double x5 = max_ - dx;
      double f1 = f_(x1, params);
      double f3 = f_(x3, params);
      double f5 = f_(x5, params);
      if (devel) {
        std::cerr << "DEVEL: genbb::decay0::decay0_gauss: f1=" << f1 << std::endl;
        std::cerr << "DEVEL: genbb::decay0::decay0_gauss: f3=" << f3 << std::endl;
        std::cerr << "DEVEL: genbb::decay0::decay0_gauss: f5=" << f5 << std::endl;
      }
      */
      int count = 0;
      int status = 0;
      gsl_error_handler_t * gsl_eh = gsl_set_error_handler_off();
      while (true) {
        status = gsl_integration_qng(&F, min_, max_, epsabs, epsrel,
                                         &result, &abserr, &neval);
        // if (devel) {
        //   std::cerr << "DEVEL: genbb::decay0::decay0_gauss: status = " << status << std::endl;
        //   std::cerr << "DEVEL: genbb::decay0::decay0_gauss: result = " << result << std::endl;
        //   std::cerr << "DEVEL: genbb::decay0::decay0_gauss: abserr = " << abserr << std::endl;
        //   std::cerr << "DEVEL: genbb::decay0::decay0_gauss: neval  = " << neval << std::endl;
        // }
        if (status == 0) {
          break;
        }
        if (status == GSL_ETOL) {
          epsrel *= 10.0;
        }
        count++;
        if (count >= 2) {
          break;
        }
        // if (devel) {
        //   std::cerr << "DEVEL: genbb::decay0::decay0_gauss: GSL_ETOL = " << "retrying..." << std::endl;
        // }
      }
      gsl_set_error_handler(gsl_eh);
      if (status != 0) {
        std::ostringstream message;
        message << "genbb::decay0::decay0_gauss: "
                << "GSL QNG integration error '"
                << gsl_strerror(status)
                << "' !";
        //std::cerr << "ERROR: " << message.str() << std::endl;
        DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                       message.str());
        // throw std::runtime_error(message.str());
      }
      if (devel) {
        std::cerr << "DEVEL: genbb::decay0::decay0_gauss: Exiting." << std::endl;
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

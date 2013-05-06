
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/primary_event.h>

#include <genbb_help/decay0/beta.h>
#include <genbb_help/decay0/particle.h>
#include <genbb_help/decay0/tgold.h>
#include <genbb_help/decay0/funbeta.h>

namespace genbb {
  namespace decay0 {

    void decay0_beta(mygsl::rng & prng_, primary_event & event_,
                     double Qbeta_, double Zdtr_,
                     double tcnuc_, double thnuc_, double & tdnuc_)
    {
      parbeta pars;
      pars.Zdtr  = Zdtr_;
      pars.Qbeta = Qbeta_;
      decay0_beta(prng_, event_,tcnuc_, thnuc_,tdnuc_, &pars);
      return;
    }

    void decay0_beta(mygsl::rng & prng_, primary_event & event_, 
                     double tcnuc_, double thnuc_, double &tdnuc_, 
                     void * params_)
    {
      // Parameters for function 'decay0_funbeta'
      const parbeta * pars = static_cast<const parbeta *>(params_);
      const double & Zdtr  = pars->Zdtr;
      const double & Qbeta = pars->Qbeta;

      // std::clog << "DEBUG: " << "genbb::decay0::decay0_beta: "
      //           << "Zdtr=" << Zdtr
      //           << std::endl;
      // std::clog << "DEBUG: " << "genbb::decay0::decay0_beta: "
      //           << "Qbeta=" << Qbeta
      //           << std::endl;

      double em, fm;
      decay0_tgold(50.e-6, 0.5 * Qbeta, Qbeta ,
                   decay0_funbeta, 0.001 * Qbeta, 2,
                   em, fm, params_);
      // std::clog << "DEBUG: " << "genbb::decay0::decay0_beta: "
      //           << "fm=" << fm << " MeV"
      //           << std::endl;
      // Rejection method:
      double f, fe, E;
      do {
        E = 50.e-6 + (Qbeta - 50.e-6) * prng_();
        // std::clog << "DEBUG: " << "genbb::decay0::decay0_beta: "
        //           << "E=" << E << " MeV"
        //           << std::endl;
        fe = decay0_funbeta(E, params_);
        f = fm * prng_();
      } while (f > fe);
      int np;
      if (Zdtr >= 0.) np = ELECTRON;
      if (Zdtr < 0.) np = POSITRON;
      // isotropical emission of beta particle is supposed
      decay0_particle(prng_, event_, 
                      np, E, E, 0., M_PI, 0., 2.*M_PI,
                      tcnuc_,thnuc_,tdnuc_);
      return;
    }

  } // end of namespace decay0 
} // end of namespace genbb 

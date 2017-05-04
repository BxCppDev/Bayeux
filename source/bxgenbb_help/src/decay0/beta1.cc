
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/primary_event.h>

#include <genbb_help/decay0/beta1.h>
#include <genbb_help/decay0/particle.h>
#include <genbb_help/decay0/tgold.h>
#include <genbb_help/decay0/funbeta1.h>

namespace genbb {
  namespace decay0 {

    void decay0_beta1(mygsl::rng & prng_, primary_event & event_,
                      double Qbeta_, double Zdtr_,
                      double tcnuc_, double thnuc_, double & tdnuc_,
                      double c1_, double c2_, double c3_, double c4_)
    {
      parbeta1 pars1;
      pars1.Zdtr  = Zdtr_;
      pars1.Qbeta = Qbeta_;
      pars1.c1    = c1_;
      pars1.c2    = c2_;
      pars1.c3    = c3_;
      pars1.c4    = c4_;
      decay0_beta1(prng_, event_, tcnuc_, thnuc_, tdnuc_, &pars1);
      return;
    }
  
    void decay0_beta1(mygsl::rng & prng_, primary_event & event_, 
                      double tcnuc_,double thnuc_,double & tdnuc_, 
                      void * params_)
    {
      // Parameters for function 'decay0_funbeta1'
      const parbeta1 * pars1 = static_cast<const parbeta1 *>(params_);
      const double & Zdtr  = pars1->Zdtr;
      const double & Qbeta = pars1->Qbeta;

      // parbeta1 pars1;
      // pars1.Zdtr  = pars->Zdtr;
      // pars1.Qbeta = pars->Qbeta;
      // pars1.c1    = pars->c1;
      // pars1.c2    = pars->c2;
      // pars1.c3    = pars->c3;
      // pars1.c4    = pars->c4;

      double em, fm;
      decay0_tgold(50.e-6, 0.5 * Qbeta, Qbeta,
                   decay0_funbeta1, 0.001 * Qbeta, 2, 
                   em, fm, params_);
      // Rejection method:
      double f, fe, E;
      do {
        E  = 50.e-6 + (Qbeta - 50.e-6) * prng_();
        fe = decay0_funbeta1(E, params_);
        f  = fm * prng_();
      } while (f > fe);
      int np;
      if (Zdtr >= 0.) np = ELECTRON;
      if (Zdtr < 0.) np  = POSITRON;
      // isotropical emission of beta particle is supposed:
      decay0_particle(prng_, event_, np, E, E, 
                      0., M_PI, 0., 2.*M_PI,
                      tcnuc_, thnuc_, tdnuc_);
      return;
    }

  } // end of namespace decay0 
} // end of namespace genbb 

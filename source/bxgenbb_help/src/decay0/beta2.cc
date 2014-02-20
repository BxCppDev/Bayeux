
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/primary_event.h>

#include <genbb_help/decay0/beta2.h>
#include <genbb_help/decay0/particle.h>
#include <genbb_help/decay0/tgold.h>
#include <genbb_help/decay0/funbeta2.h>

namespace genbb {
  namespace decay0 {
    
    void decay0_beta2(mygsl::rng & prng_, primary_event & event_,
                      double Qbeta_, double Zdtr_,
                      double tcnuc_, double thnuc_, double & tdnuc_,
                      int kf_,
                      double c1_, double c2_, double c3_, double c4_)
    {
      parbeta2 pars2;
      pars2.Zdtr  = Zdtr_;
      pars2.Qbeta = Qbeta_;
      pars2.c1    = c1_;
      pars2.c2    = c2_;
      pars2.c3    = c3_;
      pars2.c4    = c4_;
      pars2.kf    = kf_;
      decay0_beta2(prng_, event_, tcnuc_, thnuc_, tdnuc_, &pars2);
      return;
    }
  
    void decay0_beta2(mygsl::rng & prng_, primary_event & event_, 
                      double tcnuc_, double thnuc_, double & tdnuc_, 
                      void * params_)
    {
      const parbeta2 * pars2 = static_cast<const parbeta2 *>(params_);
      const double & Zdtr  = pars2->Zdtr;
      const double & Qbeta = pars2->Qbeta;
      //const double & c1    = pars2->c1;
      //const double & c2    = pars2->c2;
      //const double & c3    = pars2->c3;
      //const double & c4    = pars2->c4;
      //const double & kf    = pars2->kf;
      double em, fm;
      decay0_tgold(50.e-6, 0.5 * Qbeta, Qbeta,
                   decay0_funbeta2, 0.001 * Qbeta, 2, 
                   em, fm, params_);
      // Rejection method:
      double f, fe, E;
      do {      
        E  = 50.e-6 + (Qbeta - 50.e-6) * prng_();
        fe = decay0_funbeta2(E, params_);
        f  = fm * prng_();
      } while (f > fe);
      int np;
      if (Zdtr >= 0.) np = ELECTRON;
      if (Zdtr < 0.)  np = POSITRON;
      // isotropical emission of beta particle is supposed:
      decay0_particle(prng_, event_, np,
                      E, E, 0., M_PI, 0., 2.*M_PI,
                      tcnuc_,thnuc_,tdnuc_);
      return;
    }

  } // end of namespace decay0 
} // end of namespace genbb 


#include <cmath>

#include <gsl/gsl_math.h>

#include <mygsl/rng.h>

#include <genbb_help/primary_event.h>

#include <genbb_help/decay0/moller.h>
#include <genbb_help/decay0/moller1.h>
#include <genbb_help/decay0/particle.h>

namespace genbb {
  namespace decay0 {
  
    void decay0_moller(mygsl::rng & prng, primary_event & event, 
                       double E1, double E2,
                       double teta1, double teta2,
                       double phi1, double phi2,
                       double dcute)
    {
      static double emass = decay0_emass();
      double pe0[3], pe1[3], pe2[3];
      double phi=phi1+(phi2-phi1)*prng();
      double ctet1=1.;
      if (teta1 != 0.) ctet1 = std::cos(teta1);
      double ctet2=-1.;
      if (teta2 != M_PI) ctet2 = std::cos(teta2);
      double ctet=ctet1+(ctet2-ctet1)*prng();
      double stet=std::sqrt(1.-ctet*ctet);
      double E=E1;
      if (E1 != E2) E=E1+(E2-E1)*prng();
      double p=std::sqrt(E*(E+2.*emass));
      pe0[0]=p*stet*std::cos(phi);
      pe0[1]=p*stet*std::sin(phi);
      pe0[2]=p*ctet;
      decay0_moller1(prng,dcute,pe0,pe1,pe2);

      // Push particles in the preimary event:
      primary_particle part;
      geomtools::vector_3d momentum;
      geomtools::vector_3d vertex;
      geomtools::invalidate(vertex);
      
      // Positron:
      part.set_type(ELECTRON);
      part.set_time (0. * CLHEP::second);
      momentum.set(pe1[0], pe1[1], pe1[2]);
      part.set_momentum (momentum* CLHEP::MeV);
      part.set_vertex (vertex);
      event.grab_particles ().push_back(part);
     
      // Electron:
      part.set_type(ELECTRON);
      part.set_time (0. * CLHEP::second);
      momentum.set(pe2[0], pe2[1], pe2[2]);
      part.set_momentum (momentum * CLHEP::MeV);
      part.set_vertex (vertex);
      event.grab_particles ().push_back(part);

      return;
    }
    
  } // end of namespace decay0 
} // end of namespace genbb 

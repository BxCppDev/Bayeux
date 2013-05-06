
#include <cmath>

#include <gsl/gsl_math.h>

#include <datatools/units.h>
#include <mygsl/rng.h>
#include <genbb_help/primary_event.h>

#include <genbb_help/decay0/compton.h>
#include <genbb_help/decay0/particle.h>
#include <genbb_help/decay0/compton1.h>

namespace genbb {
  namespace decay0 {
  
    void decay0_compton(mygsl::rng & prng, primary_event & event, 
                        double E1,double E2,
                        double teta1,double teta2,
                        double phi1,double phi2)
    {
      static double emass = decay0_emass();
      double pg0[3], pg1[3], pe1[3];
      double phi=phi1+(phi2-phi1)*prng();
      double ctet1=1.;
      if (teta1 != 0.) ctet1=std::cos(teta1);
      double  ctet2=-1.;
      if (teta2 != M_PI) ctet2=std::cos(teta2);
      double ctet=ctet1+(ctet2-ctet1)*prng();
      double stet=sqrt(1.-ctet*ctet);
      double E=E1;
      if (E1 != E2) E=E1+(E2-E1)*prng();
      double p=E;
      pg0[0]=p*stet*std::cos(phi);
      pg0[1]=p*stet*std::sin(phi);
      pg0[2]=p*ctet;
      decay0_compton1(prng,pg0,pg1,pe1);

      primary_particle part;
      geomtools::vector_3d momentum;
      geomtools::vector_3d vertex;
      geomtools::invalidate(vertex);
      part.set_time (0. * CLHEP::second);
      part.set_vertex (vertex);

      // Gamma:
      part.set_type(GAMMA);
      momentum.set(pg1[0], pg1[1], pg1[2]);
      part.set_momentum (momentum* CLHEP::MeV);
      event.grab_particles ().push_back(part);

      // Electron:
      part.set_type(ELECTRON);
      momentum.set(pe1[0], pe1[1], pe1[2]);
      part.set_momentum (momentum* CLHEP::MeV);
      event.grab_particles ().push_back(part);

      return;
    }
    
  } // end of namespace decay0 
} // end of namespace genbb 

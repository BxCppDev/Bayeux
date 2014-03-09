// pairext.cc

#include <cmath>

#include <gsl/gsl_math.h>

#include <datatools/units.h>
#include <mygsl/rng.h>

#include <genbb_help/decay0/pairext.h>
#include <genbb_help/decay0/pairext1.h>
#include <genbb_help/decay0/particle.h>

namespace genbb {
  namespace decay0 {

    void decay0_pairext(mygsl::rng & prng, primary_event & event,
                        double E1,double E2,
                        double teta1,double teta2,
                        double phi1,double phi2,double Z)
    {
      double pg0[3], pe1[3], pe2[3];
      double phi=phi1+(phi2-phi1)*prng();
      double ctet1=1.;
      if (teta1 != 0.) ctet1=std::cos(teta1);
      double ctet2=-1.;
      if (teta2 != M_PI) ctet2=std::cos(teta2);
      double ctet=ctet1+(ctet2-ctet1)*prng();
      double stet=std::sqrt(1.-ctet*ctet);
      double E=E1;
      if (E1 != E2) E=E1+(E2-E1)*prng();
      double p=E;
      pg0[0]=p*stet*std::cos(phi);
      pg0[1]=p*stet*std::sin(phi);
      pg0[2]=p*ctet;
      decay0_pairext1(prng, Z,pg0,pe1,pe2);
      // to fix GEANT numbers of emitted particles but to select the momentum
      // randomly
      if(prng() < 0.5) {
        double x1=pe1[0];
        double x2=pe1[1];
        double x3=pe1[2];
        pe1[0]=pe2[0];
        pe1[1]=pe2[1];
        pe1[2]=pe2[2];
        pe2[0]=x1;
        pe2[1]=x2;
        pe2[2]=x3;
      }
      primary_particle part;
      geomtools::vector_3d momentum;
      geomtools::vector_3d vertex;
      geomtools::invalidate(vertex);
      //// double pmass;

      // Positron:
      //// pmass = decay0_particle_mass (POSITRON);
      part.set_type(POSITRON);
      part.set_time (0. * CLHEP::second);
      momentum.set(pe1[0], pe1[1], pe1[2]);
      part.set_momentum (momentum* CLHEP::MeV);
      part.set_vertex (vertex);
      event.grab_particles ().push_back(part);

      // Electron:
      //// pmass = decay0_particle_mass (ELECTRON);
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
//
// end of pairext.cc
//
// Local Variables: --
// mode: c++ --
// End: --


#include <cmath>
#include <limits>
#include <stdexcept>

#include <datatools/units.h>
#include <mygsl/rng.h>
#include <genbb_help/decay0/particle.h>
#include <genbb_help/primary_event.h>

namespace genbb {
  namespace decay0 {

    /// Return the name of the particle with given ID (MeV)
    const std::string decay0_geant3_particle_name(int np)
    {
     switch(np) {
        case GAMMA: return std::string("gamma");
        case POSITRON: return std::string("positron");
        case ELECTRON: return std::string("electron");
        case NEUTRINO: return std::string("neutrino");
        case MUONPLUS: return std::string("muon+");
        case MUONMINUS: return std::string("muon-");
        case PION0: return std::string("pion0");
        case PIONPLUS: return std::string("pion+");
        case PIONMINUS: return std::string("pion-");
        case KAON0LONG: return std::string("kaon0 long");
        case KAONPLUS: return std::string("kaon+");
        case KAONMINUS: return std::string("kaon-");
        case NEUTRON: return std::string("neutron");
        case PROTON: return std::string("proton");
        case ANTIPROTON: return std::string("antiproton");
        case KAON0SHORT: return std::string("kaon0 short");
        case ETA: return std::string("eta");
        case LAMBDA: return std::string("lambda");
        case SIGMAPLUS: return std::string("sigma+");
        case SIGMA0: return std::string("sigma0");
        case SIGMAMINUS: return std::string("sigma-");
        case XI0: return std::string("xi0");
        case XIMINUS: return std::string("xi-");
        case OMEGA: return std::string("omega");
        case ANTINEUTRON: return std::string("antineutron");
        case ANTILAMBDA: return std::string("antilambda");
        case ANTISIGMAMINUS: return std::string("antisigma-");
        case ANTISIGMA0: return std::string("antisigma0");
        case ANTISIGMAPLUS: return std::string("antisigma+");
        case ANTIXI0: return std::string("antixi0");
        case ANTIXIPLUS: return std::string("antixi+");
        case ANTIOMEGAPLUS: return std::string("antiomega+");
        case DEUTERON: return std::string("deuteron");
        case TRITIUM: return std::string("tritium");
        case ALPHA: return std::string("alpha");
        case GEANTINO: return std::string("geantino");
        case HE3: return std::string("He3");
        case CERENKOV: return std::string("Cerenkov");
        }
     return std::string("");
    }

    double decay0_particle_mass(int np)
    {
      switch(np) {
        case GAMMA: return 0.;
        case POSITRON: return 0.51099906;
        case ELECTRON: return 0.51099906;
        case NEUTRINO: return 0.;
        case MUONPLUS: return 105.658389;
        case MUONMINUS: return 105.658389;
        case PION0: return 134.9764;
        case PIONPLUS: return 139.5700;
        case PIONMINUS: return 139.5700;
        case KAON0LONG: return 497.672;
        case KAONPLUS: return 493.677;
        case KAONMINUS: return 493.677;
        case NEUTRON: return 939.56563;
        case PROTON: return 938.27231;
        case ANTIPROTON: return 938.27231;
        case KAON0SHORT: return 497.672;
        case ETA: return 547.45;
        case LAMBDA: return 1115.684;
        case SIGMAPLUS: return 1189.37;
        case SIGMA0: return 1192.55;
        case SIGMAMINUS: return 1197.436;
        case XI0: return 1314.9;
        case XIMINUS: return 1321.32;
        case OMEGA: return 1672.45;
        case ANTINEUTRON: return 939.56563;
        case ANTILAMBDA: return 1115.684;
        case ANTISIGMAMINUS: return 1189.37;
        case ANTISIGMA0: return 1192.55;
        case ANTISIGMAPLUS: return 1197.436;
        case ANTIXI0: return 1314.9;
        case ANTIXIPLUS: return 1321.32;
        case ANTIOMEGAPLUS: return 1672.45;
        case DEUTERON: return 1875.613;
        case TRITIUM: return 2809.25;
        case ALPHA: return 3727.417;
        case GEANTINO: return 0.;
        case HE3: return 2809.23;
        case CERENKOV: return 0.;
        }
      return std::numeric_limits<double>::quiet_NaN();
    }

    double decay0_emass()
    {
      return decay0_particle_mass(ELECTRON);
    }

    void decay0_particle(mygsl::rng & prng_, primary_event & event_,
                         int np, double E1, double E2,
                         double teta1, double teta2,
                         double phi1, double phi2,
                         double tclev, double thlev, double & tdlev)
    {
      double last_time = 0.0 * CLHEP::second;
      if (event_.get_particles ().size() > 0) {
        const primary_particle & last_part = event_.get_particles ().back();
        last_time = last_part.get_time();
      }

      primary_particle part;
      double pmass = decay0_particle_mass (np);
      if (isnan(pmass)) {
        std::ostringstream message;
        message << "genbb::decay0::decay0_particle: "
                << "Unknown mass for particle with ID=" << np << " !";
        throw std::logic_error(message.str());
      }
      double phi=phi1+(phi2-phi1)*prng_();
      double ctet1=1.;
      if (teta1 != 0.) ctet1=std::cos(teta1);
      double ctet2=-1.;
      if (teta2 != M_PI) ctet2=std::cos(teta2);
      double ctet=ctet1+(ctet2-ctet1)*prng_();
      double stet=std::sqrt(1.-ctet*ctet);
      double E=E1;
      if (E1 != E2) E=E1+(E2-E1)*prng_();
      // --- randomly blur the particle energy
      // double dE=0.000025;
      // E=E+dE*(-1+2*prng_());
      double p=std::sqrt(E*(E+2.*pmass));
      geomtools::vector_3d momentum(p*stet*std::cos(phi),
                                    p*stet*std::sin(phi),
                                    p*ctet);
      geomtools::vector_3d vertex;
      geomtools::invalidate(vertex);

      tdlev=tclev;
      if (thlev > 0.) tdlev = tclev - thlev / std::log(2.) * std::log(prng_());

      part.set_type(np);
      part.set_time (last_time + tdlev * CLHEP::second);
      part.set_momentum (momentum * CLHEP::MeV);
      part.set_vertex (vertex);
      event_.grab_particles ().push_back(part);

      return;
    }

  } // end of namespace decay0
} // end of namespace genbb

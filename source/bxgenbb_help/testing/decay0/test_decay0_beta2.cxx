#include <iostream>
#include <cmath>
#include <datatools/units.h>
#include <genbb_help/decay0/beta2.h>
#include <genbb_help/decay0/funbeta2.h>
#include <genbb_help/primary_event.h>
#include <mygsl/rng.h>

int main(void)
{
  mygsl::rng prng;
  prng.init("taus", 314159);

  genbb::primary_event event;

  genbb::decay0::parbeta2 beta2_params;
  // K40 (1.311,+20.,0.,0.,t,3,7.,7.,1.,0.)

  beta2_params.Zdtr  = 20.;
  beta2_params.Qbeta = 1.311;
  beta2_params.c1 = 7.;
  beta2_params.c2 = 7.;
  beta2_params.c3 = 1.;
  beta2_params.c4 = 0.;
  beta2_params.kf = 3;
  beta2_params.c1 = 0.;
  beta2_params.c2 = 0.;
  beta2_params.c3 = 0.;
  beta2_params.c4 = 0.;
  beta2_params.kf = 0;

  double creation_time = 0.0; // sec
  double halflife      = 10.0; // sec
  double decay_time; // sec

  for (int i = 0; i < 10000; i++)
    {
      event.reset();
      event.set_time (0.0);
      event.set_label ("test_beta2 (decay0 C++ port)");
      genbb::decay0::decay0_beta2(prng, event,
                                  creation_time,
                                  halflife,
                                  decay_time,
                                  &beta2_params);
      const genbb::primary_particle & beta
        = event.get_particles().front();
      if (! beta.is_electron())
        {
          std::cerr << "ERROR: Not an electron !" << '\n';
          return (1);
        }
      double ke = beta.get_kinetic_energy ();
      std::cout << "beta2- "  << ke / CLHEP::MeV << '\n';
      if (i == 0) event.tree_dump(std::clog, "Beta2 decay: ");
    }
  std::cout << '\n' << '\n';

  return 0;
}

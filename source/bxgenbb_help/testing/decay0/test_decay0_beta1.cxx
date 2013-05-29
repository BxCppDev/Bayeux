#include <iostream>
#include <cmath>
#include <datatools/units.h>
#include <genbb_help/decay0/beta1.h>
#include <genbb_help/decay0/funbeta1.h>
#include <genbb_help/primary_event.h>
#include <mygsl/rng.h>

int main(void)
{
  mygsl::rng prng;
  prng.init("taus", 314159);

  genbb::primary_event event;

  genbb::decay0::parbeta1 beta1_params;
  // C14
  beta1_params.Zdtr  = 7.;
  beta1_params.Qbeta = 0.156;
  beta1_params.c1 = 0.;
  beta1_params.c2 = 0.19418;
  beta1_params.c3 = 0.;
  beta1_params.c4 = 0.;

  double creation_time = 0.0; // sec
  double halflife      = 10.0; // sec
  double decay_time; // sec

  for (int i = 0; i < 100000; i++)
    {
      event.reset();
      event.set_time (0.0);
      event.set_label ("test_beta1 (decay0 C++ port)");
      genbb::decay0::decay0_beta1(prng, event,
                                  creation_time, 
                                  halflife,
                                  decay_time, 
                                  &beta1_params);
      const genbb::primary_particle & beta 
        = event.get_particles().front();
      if (! beta.is_electron())
        {
          std::cerr << "ERROR: Not an electron !" << '\n';
          return (1);
        }
      double ke = beta.get_kinetic_energy ();
      std::cout << "beta1- "  << ke / CLHEP::MeV << '\n';
      if (i == 0) event.tree_dump(std::clog, "Beta1 decay: ");
    }
  std::cout << '\n' << '\n';

  return 0;
}

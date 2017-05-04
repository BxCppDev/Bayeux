#include <iostream>
#include <cmath>
#include <datatools/units.h>
#include <genbb_help/decay0/beta.h>
#include <genbb_help/decay0/funbeta.h>
#include <genbb_help/primary_event.h>
#include <mygsl/rng.h>

int main(void)
{
  mygsl::rng prng;
  prng.init("taus", 314159);

  genbb::primary_event event;

  genbb::decay0::parbeta beta_params;
  beta_params.Zdtr  = 84;
  beta_params.Qbeta = 1.423;

  double creation_time = 0.0; // sec
  double halflife      = 0.0; // sec
  double decay_time; // sec

  for (int i = 0; i < 10000; i++)
    {
      event.reset();
      event.set_time (0.0);
      event.set_label ("test_beta (decay0 C++ port)");
      genbb::decay0::decay0_beta(prng, event,
                                 creation_time,
                                 halflife,
                                 decay_time,
                                 &beta_params);
      const genbb::primary_particle & beta = event.get_particles().front();
      if (! beta.is_electron())
        {
          std::cerr << "ERROR: Not an electron !" << '\n';
          return (1);
        }
      double ke = beta.get_kinetic_energy ();
      std::cout << "beta- "  << ke / CLHEP::MeV << '\n';
      if (i == 0) event.tree_dump(std::clog, "Beta decay: ");
    }
  std::cout << '\n' << '\n';

  beta_params.Zdtr = -84;
  for (int i = 0; i < 10000; i++)
    {
      event.reset();
      event.set_time (0.0);
      event.set_label ("test_beta (decay0 C++ port)");
      genbb::decay0::decay0_beta(prng, event,
                                 creation_time,
                                 halflife,
                                 decay_time,
                                 &beta_params);
      const genbb::primary_particle & beta = event.get_particles().front();
      if (! beta.is_positron())
        {
          std::cerr << "ERROR: Not an positron !" << '\n';
          return (1);
        }
      double ke = beta.get_kinetic_energy ();
      std::cout << "beta+ " << ke / CLHEP::MeV << '\n';
      if (i == 0) event.tree_dump(std::clog, "Beta decay: ");
    }
  std::cout << '\n' << '\n';

  return 0;
}

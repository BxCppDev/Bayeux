#include <iostream>
#include <cmath>

#include <datatools/units.h>

#include <genbb_help/primary_event.h>
#include <genbb_help/decay0/beta_1fu.h>
#include <genbb_help/decay0/funbeta_1fu.h>

#include <mygsl/rng.h>

int main(void)
{
  mygsl::rng prng;
  prng.initialize("taus", 314159);

  genbb::primary_event event;

  genbb::decay0::bj69sl2 sl2_params;
  // Cs137 (0.514,56.,0.,0.,t,0.,0.,0.,0.)
  sl2_params.Zdtr  = 56.;
  sl2_params.Qbeta = 0.514;
  sl2_params.c1 = 0.;
  sl2_params.c2 = 0.;
  sl2_params.c3 = 0.;
  sl2_params.c4 = 0.;

  double creation_time = 0.0; // sec
  double halflife      = 10.0; // sec
  double decay_time; // sec

  for (int i = 0; i < 10000; i++) {
    event.reset();
    event.set_time (0.0);
    event.set_label ("test_decay0_beta_1fu (decay0 C++ port)");
    genbb::decay0::decay0_beta_1fu(prng, event,
                                   creation_time,
                                   halflife,
                                   decay_time,
                                   &sl2_params);
    const genbb::primary_particle & beta
      = event.get_particles().front();
    if (! beta.is_electron()) {
      std::cerr << "ERROR: Not an electron !" << '\n';
      return (1);
    }
    double ke = beta.get_kinetic_energy ();
    std::cout << "sl2- "  << ke / CLHEP::MeV << '\n';
    if (i == 0) event.tree_dump(std::clog, "Sl2 decay: ");
  }
  std::cout << '\n' << '\n';

  return 0;
}

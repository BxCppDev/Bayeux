#include <iostream>
#include <cmath>
#include <datatools/units.h>
#include <genbb_help/decay0/bb.h>
#include <genbb_help/primary_event.h>
#include <mygsl/rng.h>

int main(void)
{
  mygsl::rng prng;
  prng.init("taus", 314159);

  genbb::primary_event event;

  genbb::decay0::bbpars bb_params;
  bb_params.modebb   = genbb::decay0::MODEBB_1;
  bb_params.Qbb      = 3.034;
  bb_params.Edlevel  = 0.0;
  bb_params.EK       = 0.0;
  bb_params.Zdbb     = 44.0;
  bb_params.Adbb     = 100.0;
  bb_params.istartbb = 0;
  for (int i = 0; i < (int) genbb::decay0::bbpars::SPSIZE; i++)
    {
      bb_params.spthe1[i] = 0.0;
      bb_params.spthe2[i] = 0.0;
    }
  bb_params.spmax = -1.0;

  for (int i = 0; i < 100000; i++)
    {
      event.reset();
      event.set_time (0.0);
      event.set_label ("test_bb (decay0 C++ port)");
      genbb::decay0::decay0_bb(prng, event,
                               &bb_params);
      const genbb::primary_particle & beta1 = event.get_particles().front();
      const genbb::primary_particle & beta2 = event.get_particles().back();
      if (! beta1.is_electron())
        {
          std::cerr << "ERROR: Not an electron !" << '\n';
          return (1);
        }
      if (! beta2.is_electron())
        {
          std::cerr << "ERROR: Not an electron !" << '\n';
          return (1);
        }
      double ke1 = beta1.get_kinetic_energy ();
      double ke2 = beta2.get_kinetic_energy ();
      double kesum = ke1 + ke2;
      std::cout << ke1 / CLHEP::MeV
                << ' ' << ke2 / CLHEP::MeV
                << ' ' << kesum / CLHEP::MeV
                << '\n';
      if (i == 0) event.tree_dump(std::clog, "Double beta decay: ");
    }
  std::cout << '\n' << '\n';

  return 0;
}

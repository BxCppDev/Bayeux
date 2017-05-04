#include <iostream>
#include <cmath>
#include <genbb_help/decay0/electron.h>
#include <genbb_help/primary_event.h>
#include <mygsl/rng.h>

int main(void)
{
  mygsl::rng prng;
  prng.init("taus", 314159);

  genbb::primary_event event;

  double E = 1.3; // MeV
  double level_creation_time = 0.0; // sec
  double level_halflife      = 0.0; // sec
  double level_decay_time;
  event.set_time (0.0);
  event.set_label ("test_electron (decay0 C++ port)");
  genbb::decay0::decay0_electron(prng, event,
                                 E,
                                 level_creation_time, 
                                 level_halflife,
                                 level_decay_time);

  event.tree_dump(std::clog, "Electron: ");

  return 0;
}

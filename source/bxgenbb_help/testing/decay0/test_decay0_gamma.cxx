#include <iostream>
#include <cmath>
#include <genbb_help/decay0/gamma.h>
#include <genbb_help/primary_event.h>
#include <mygsl/rng.h>

int main(void)
{
  mygsl::rng prng;
  prng.init("taus", 314159);

  genbb::primary_event event;

  double E = 1.4; // MeV
  double level_creation_time = 245.67e-9; // sec
  double level_halflife      = 245.67e-17; // sec
  double level_decay_time;
  event.set_time (0.0);
  event.set_label ("test_gamma (decay0 C++ port)");
  genbb::decay0::decay0_gamma(prng, event,
                              E,
                              level_creation_time, 
                              level_halflife,
                              level_decay_time);
 
  event.tree_dump(std::clog, "Gamma: ");

  return 0;
}

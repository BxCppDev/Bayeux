#ifndef GENBB_DECAY0_GE70LOW_H_
#define GENBB_DECAY0_GE70LOW_H_ 1

namespace mygsl {
  class rng;
}
namespace genbb {
  class primary_event;
  namespace decay0 {

    void Ge70low(mygsl::rng & prng_, primary_event & event_, int levelkev);


  } // end of namespace decay0
} // end of namespace genbb

#endif // GENBB_DECAY0_GE70LOW_H_
// Local Variables: --
// mode: c++ --
// End: --

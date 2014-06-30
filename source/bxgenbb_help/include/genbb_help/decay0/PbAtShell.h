#ifndef GENBB_DECAY0_PBATSHELL_H_
#define GENBB_DECAY0_PBATSHELL_H_ 1

namespace mygsl
{
  class rng;
}
namespace genbb
{
  class primary_event;
  namespace decay0
  {

    void PbAtShell (mygsl::rng & prng_, primary_event & event_, int KLMenergy,
                    double tclev, double thlev, double &tdlev);


  } // end of namespace decay0
} // end of namespace genbb

#endif // GENBB_DECAY0_PBATSHELL_H_
// Local Variables: --
// mode: c++ --
// End: --

#ifndef GENBB_DECAY0_FUNBETA1_H_
#define GENBB_DECAY0_FUNBETA1_H_ 1

#include <genbb_help/decay0/funbeta.h>

namespace genbb {

  namespace decay0 {
    
    struct parbeta1 : public parbeta
    {
      double c1;
      double c2;
      double c3;
      double c4;
    };
    
    double decay0_funbeta1(double E_, void * params_);
    
  } // end of namespace decay0 
} // end of namespace genbb 

#endif // GENBB_DECAY0_FUNBETA1_H_
// 
// Local Variables: --
// mode: c++ --
// End: --

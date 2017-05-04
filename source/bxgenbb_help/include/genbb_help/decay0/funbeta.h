#ifndef GENBB_DECAY0_FUNBETA_H_
#define GENBB_DECAY0_FUNBETA_H_ 1

namespace genbb {

  namespace decay0 {
     
    struct parbeta
    {
      double Zdtr;
      double Qbeta;
    };
   
    double decay0_funbeta(double E, void * params);

  } // end of namespace decay0 
} // end of namespace genbb 

#endif // GENBB_DECAY0_FUNBETA_H_
// 
// Local Variables: --
// mode: c++ --
// End: --

#ifndef GENBB_DECAY0_DIVDIF_H_
#define GENBB_DECAY0_DIVDIF_H_ 1

namespace genbb {
  namespace decay0 {

    /// \brief Tabular interpolation using symmetrically placed argument points.
    double decay0_divdif(const double * F_, 
                         const double * A_, 
                         int NN_,
                         double X_, 
                         int MM_);

  } // end of namespace decay0 
} // end of namespace genbb 

#endif // GENBB_DECAY0_DIVDIF_H_
// 
// Local Variables: --
// mode: c++ --
// End: --



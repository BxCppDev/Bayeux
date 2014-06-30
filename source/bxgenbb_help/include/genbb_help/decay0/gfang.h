#ifndef GENBB_DECAY0_GFANG_H_
#define GENBB_DECAY0_GFANG_H_ 1

namespace genbb {
  namespace decay0 {

    /// \brief  Finds theta-phi angles for a particle with momentum P.
    // The logical flag ROTATE indicates whether the director
    // cosines are along the Z axis (.FALSE.) or not (.TRUE.).
    // F.Carminati (GEANT).
    void decay0_gfang(const double * p,
                      double &costh, double &sinth,
                      double &cosph, double &sinph,
                      bool &rotate);

  } // end of namespace decay0
} // end of namespace genbb

#endif // GENBB_DECAY0_GFANG_H_
//
// Local Variables: --
// mode: c++ --
// End: --

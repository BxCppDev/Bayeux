#ifndef GENBB_DECAY0_PLOG69_H_
#define GENBB_DECAY0_PLOG69_H_ 1

//#include <boost/scoped_ptr.hpp>

// namespace mygsl {
//   class tabulated_function;
// }

namespace genbb {

  namespace decay0 {

    // Values of natural logarithm of the standard values of momentum
    // (in units m_e*c) from:
    // H.Behrens, J.Janecke, "Numerical tables for beta-decay and
    // electron capture", Berlin, Springer-Verlag, 1969.
    // Range of momenta correspond to kinetic energy range from 2.55 keV to 25.0 MeV.
    struct BJ69 {
      static const double plog69[48];
    };

  } // end of namespace decay0
} // end of namespace genbb

#endif // GENBB_DECAY0_PLOG69_H_
//
// Local Variables: --
// mode: c++ --
// End: --

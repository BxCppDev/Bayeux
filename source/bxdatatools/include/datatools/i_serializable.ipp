/// \file datatools/i_serializable.ipp
#ifndef DATATOOLS_I_SERIALIZABLE_IPP
#define DATATOOLS_I_SERIALIZABLE_IPP

// Ourselves:
#include <datatools/i_serializable.h>

// Third Party:
// - Boost:
#include <boost/serialization/export.hpp>

namespace datatools {

  /// Boost serialization template method
  DATATOOLS_SERIALIZATION_SERIALIZE_IMPLEMENT_HEADER(i_serializable, /*archive*/, /*version*/)
  {
  }

} // end of namespace datatools

#endif // DATATOOLS_I_SERIALIZABLE_IPP

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

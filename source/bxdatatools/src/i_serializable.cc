// i_serializable.cc

// Ourselves:
#include <datatools/i_serializable.h>

namespace datatools {

  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION (i_serializable,
                                                    "datatools::i_serializable/__system__")

  i_serializable::i_serializable()
  {
    return;
  }

  i_serializable::~i_serializable()
  {
    return;
  }

} // end of namespace datatools

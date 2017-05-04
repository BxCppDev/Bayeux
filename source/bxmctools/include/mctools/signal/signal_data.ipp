/// \file mctools/signal/signal_data.ipp
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2016-09-28
 * Last modified : 2016-09-28
 */

#ifndef MCTOOLS_SIGNAL_SIGNAL_DATA_IPP
#define MCTOOLS_SIGNAL_SIGNAL_DATA_IPP

// Ourselves:
#include <mctools/signal/signal_data.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/map.hpp>
// - Bayeux/datatools :
#include <datatools/i_serializable.ipp>
#include <datatools/properties.ipp>

namespace mctools {

  namespace signal {

    template<class Archive>
    void signal_data::serialize(Archive & ar_, const unsigned int /* version_ */)
    {
      ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      ar_ & boost::serialization::make_nvp("auxiliaries",  _auxiliaries_);
      ar_ & boost::serialization::make_nvp("signals_dict", _signals_dict_);
      return;
    }

  } // end of namespace signal

} // end of namespace mctools

#endif // MCTOOLS_SIGNAL_SIGNAL_DATA_IPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

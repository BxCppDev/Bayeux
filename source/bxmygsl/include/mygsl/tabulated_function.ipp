/// \file mygsl/tabulated_function.ipp

#ifndef MYGSL_TABULATED_FUNCTION_IPP
#define MYGSL_TABULATED_FUNCTION_IPP 1

// Ourselves:
#include <mygsl/tabulated_function.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.ipp>

namespace mygsl {

  template<class Archive>
  void tabulated_function::serialize(Archive & ar_,
                                     const unsigned int /*version*/)
  {
    if (Archive::is_saving::value) {
      std::string interpolation_name = this->interpolator_name();
      bool table_locked = this->is_table_locked();
      std::size_t npoints = this->size();
      std::vector<double> tx;
      std::vector<double> tf;
      export_to_vectors(tx, tf);
      ar_ & boost::serialization::make_nvp("interpolation_name", interpolation_name);
      ar_ & boost::serialization::make_nvp("table_locked", table_locked);
      
      ar_ & boost::serialization::make_nvp("points.n", npoints);
      ar_ & boost::serialization::make_nvp("points.x", tx);
      ar_ & boost::serialization::make_nvp("points.f", tf);
    } else {
      this->reset();
      std::string interpolation_name;
      bool table_locked = false;
      std::size_t npoints = 0;
      std::vector<double> tx;
      std::vector<double> tf;
      ar_ & boost::serialization::make_nvp("interpolation_name", interpolation_name);
      ar_ & boost::serialization::make_nvp("table_locked", table_locked);
      ar_ & boost::serialization::make_nvp("points.n", npoints);
      ar_ & boost::serialization::make_nvp("points.x", tx);
      ar_ & boost::serialization::make_nvp("points.f", tf);
      for (int i = 0; i < (int) npoints; i++) {
        this->add_point(tx[i], tf[i]);
      }
      if (table_locked) {
        if (interpolation_name.empty()) {
          interpolation_name = tabulated_function::default_interp_name();
        }
        this->lock_table(interpolation_name);
      }
    }
    return;
  }

} // end of namespace mygsl

#endif // MYGSL_TABULATED_FUNCTION_IPP

// Local Variables:
// mode: c++
// coding: utf-8
// End:

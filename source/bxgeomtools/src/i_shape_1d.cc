// i_shape_1d.cc

// Ourselves:
#include <geomtools/i_shape_1d.h>

namespace geomtools {

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(i_shape_1d,
                                                    "geomtools::i_shape_1d");

  bool i_shape_1d::has_number_of_paths() const
  {
    return get_number_of_paths() > 0;
  }

  unsigned int i_shape_1d::get_number_of_paths() const
  {
    return 0;
  }

  bool i_shape_1d::has_length(uint32_t flags_) const
  {
    return datatools::is_valid(get_length(flags_));
  }

  double i_shape_1d::get_length(uint32_t /*flags_*/) const
  {
    return std::numeric_limits<double>::quiet_NaN();
  }

  int i_shape_1d::get_dimensional() const
  {
    return DIMENSIONAL_1;
  }

  i_shape_1d::i_shape_1d()
    : i_object_3d()
  {
    return;
  }

  i_shape_1d::i_shape_1d(double tolerance_)
    : i_object_3d(tolerance_)
  {
    return;
  }

  i_shape_1d::i_shape_1d(double tolerance_,
                         double angular_tolerance_)
    : i_object_3d(tolerance_, angular_tolerance_)
  {
    return;
  }

  i_shape_1d::~i_shape_1d()
  {
    return;
  }

  // static
  void i_shape_1d::init_ocd(datatools::object_configuration_description & ocd_)
  {
    i_object_3d::init_ocd(ocd_);
    return;
  }

} // end of namespace geomtools

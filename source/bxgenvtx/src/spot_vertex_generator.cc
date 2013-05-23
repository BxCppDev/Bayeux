// -*- mode: c++ ; -*-
/* spot_vertex_generator.cc
 */

#include <genvtx/spot_vertex_generator.h>

#include <stdexcept>

#include <cstdlib>

#include <datatools/units.h>
#include <geomtools/utils.h>

#include <datatools/exception.h>

namespace genvtx {

  using namespace std;

  GENVTX_VG_REGISTRATION_IMPLEMENT(spot_vertex_generator,"genvtx::spot_vertex_generator");

  const geomtools::vector_3d & spot_vertex_generator::get_spot () const
  {
    return _spot_;
  }

  void spot_vertex_generator::set_spot (const geomtools::vector_3d & new_value_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _spot_ = new_value_;
    return;
  }

  void spot_vertex_generator::set_spot (double x_, double y_, double z_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _spot_.set (x_, y_, z_);
    return;
  }

  GENVTX_VG_IS_INITIALIZED_IMPLEMENT_HEAD(spot_vertex_generator)
  {
    return geomtools::is_valid (_spot_);
  }

  GENVTX_VG_INITIALIZE_IMPLEMENT_HEAD(spot_vertex_generator,
                                      configuration_,
                                      service_manager_,
                                      vgens_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");

   // parameters of the spot vertex generator:
    double x, y, z;
    x = y = z = 0.0;
    double lunit = CLHEP::millimeter;
    string lunit_str;

    if (configuration_.has_key ("spot.x"))
      {
        x = configuration_.fetch_real ("spot.x");
      }

    if (configuration_.has_key ("spot.y"))
      {
        y = configuration_.fetch_real ("spot.y");
      }

    if (configuration_.has_key ("spot.z"))
      {
        z = configuration_.fetch_real ("spot.z");
      }

    if (configuration_.has_key ("spot.length_unit"))
      {
        lunit_str = configuration_.fetch_string ("spot.length_unit");
        if (lunit_str == "none")
          {
            lunit = 1.0;
          }
        else
          {
            lunit = datatools::units::get_length_unit_from (lunit_str);
          }
      }

    if (! lunit_str.empty ())
      {
        x *= lunit;
        y *= lunit;
        z *= lunit;
      }

    set_spot (x, y, z);

    return;
  }

  GENVTX_VG_RESET_IMPLEMENT_HEAD(spot_vertex_generator)
  {
    geomtools::invalidate (_spot_);
    return;
  }

  // Constructor:
  GENVTX_VG_CONSTRUCTOR_IMPLEMENT_HEAD(spot_vertex_generator)
  {
    geomtools::invalidate (_spot_);
    return;
  }

  // Destructor :
  GENVTX_VG_DEFAULT_DESTRUCTOR_IMPLEMENT(spot_vertex_generator)

  GENVTX_VG_SHOOT_VERTEX_IMPLEMENT_HEAD(spot_vertex_generator,random_,vertex_)
  {
    DT_THROW_IF (! is_initialized(), std::logic_error, "Not initialized !");
    vertex_ = _spot_;
    return;
  }

  // Useful constructors :

  spot_vertex_generator::spot_vertex_generator (double x_, double y_, double z_)
  {
    _spot_.set (x_, y_, z_);
    return;
  }

  spot_vertex_generator::spot_vertex_generator (const geomtools::vector_3d & spot_)
  {
    _spot_ = spot_;
    return;
  }

} // end of namespace genvtx

// end of spot_vertex_generator.cc

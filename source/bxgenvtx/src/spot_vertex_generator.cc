/* spot_vertex_generator.cc
 */

// Ourselves:
#include <genvtx/spot_vertex_generator.h>

// Standard library:
#include <stdexcept>
#include <cstdlib>

// Third party:
// - Bayeux/datatools
#include <datatools/units.h>
#include <datatools/exception.h>
// - Bayeux/geomtools
#include <geomtools/utils.h>

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

  bool spot_vertex_generator::is_initialized () const
  {
    return geomtools::is_valid (_spot_);
  }

  void spot_vertex_generator::initialize (const ::datatools::properties & configuration_,
                                          ::datatools::service_manager & /*service_manager_*/,
                                          ::genvtx::vg_dict_type & /*vgens_*/)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");

    double lunit = CLHEP::mm;
    if (configuration_.has_key ("spot.length_unit"))
      {
        const std::string lunit_str = configuration_.fetch_string ("spot.length_unit");
        lunit = datatools::units::get_length_unit_from (lunit_str);
      }

    DT_THROW_IF (! configuration_.has_key ("spot.x"), std::logic_error,
                 "Missing 'spot.x' property in spot vertex model '" << get_name () << "' !");
    double x = configuration_.fetch_real ("spot.x");
    if (! configuration_.has_explicit_unit ("spot.x")) x *= lunit;

    DT_THROW_IF (! configuration_.has_key ("spot.y"), std::logic_error,
                 "Missing 'spot.y' property in spot vertex model '" << get_name () << "' !");
    double y = configuration_.fetch_real ("spot.y");
    if (! configuration_.has_explicit_unit ("spot.y")) y *= lunit;

    DT_THROW_IF (! configuration_.has_key ("spot.z"), std::logic_error,
                 "Missing 'spot.z' property in spot vertex model '" << get_name () << "' !");
    double z = configuration_.fetch_real ("spot.z");
    if (! configuration_.has_explicit_unit ("spot.z")) z *= lunit;

    set_spot (x, y, z);
    return;
  }

  void spot_vertex_generator::reset ()
   {
    geomtools::invalidate (_spot_);
    return;
  }

  spot_vertex_generator::spot_vertex_generator() : genvtx::i_vertex_generator()
  {
    geomtools::invalidate (_spot_);
    return;
  }

  spot_vertex_generator::~spot_vertex_generator()
  {
    if (is_initialized ()) reset ();
    return;
  }

  void spot_vertex_generator::_shoot_vertex(::mygsl::rng & /*random_*/,
                                            ::geomtools::vector_3d & vertex_)
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

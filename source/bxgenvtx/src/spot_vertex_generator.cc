// spot_vertex_generator.cc

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

/***************
 * OCD support *
 ***************/

#include <datatools/ocd_macros.h>

// OCD support :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::genvtx::spot_vertex_generator,ocd_)
{
  ocd_.set_class_name("genvtx::spot_vertex_generator");
  ocd_.set_class_description("A generator that generates vertexes at an unique given point");
  ocd_.set_class_library("genvtx");
  ocd_.set_class_documentation("This generator needs to be given an unique (x,y,z) triplet\n"
                               "from which it builds non randomized vertexes.             \n"
                               );

  ::genvtx::i_vertex_generator::ocd_support(ocd_);

  {
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("spot.length_unit")
      .set_from("genvtx::spot_vertex_generator")
      .set_terse_description("Set the default length unit used by the spot vertex generator")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_default_value_string("mm")
      .add_example("Set an arbitrary length unit: ::       \n"
                   "                                       \n"
                   "  spot.length_unit : string = \"mm\"   \n"
                   "                                       \n"
                   );

    ;
  }

  {
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("spot.x")
      .set_from("genvtx::spot_vertex_generator")
      .set_terse_description("Set the X position of the spot vertex")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .add_example("Set an arbitrary X position: ::        \n"
                   "                                       \n"
                   "  spot.x : real as length = 0 mm       \n"
                   "                                       \n"
                   );

    ;
  }

  {
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("spot.y")
      .set_from("genvtx::spot_vertex_generator")
      .set_terse_description("Set the Y position of the spot vertex")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .add_example("Set an arbitrary Y position: ::        \n"
                   "                                       \n"
                   "  spot.y : real as length = 0 mm       \n"
                   "                                       \n"
                   );

    ;
  }

  {
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("spot.z")
      .set_from("genvtx::spot_vertex_generator")
      .set_terse_description("Set the Z position of the spot vertex")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .add_example("Set an arbitrary Z position: ::        \n"
                   "                                       \n"
                   "  spot.z : real as length = 0 mm       \n"
                   "                                       \n"
                   );

    ;
  }

  //ocd_.set_configuration_hints ("Nothing special.");
  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(genvtx::spot_vertex_generator,"genvtx::spot_vertex_generator")

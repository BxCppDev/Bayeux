/// \file geomtools/tube.cc

// Ourselves:
#include <geomtools/tube.h>

// Standard library:
#include <cmath>
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/units.h>

// This project:
#include <geomtools/cylinder.h>

namespace geomtools {

  using namespace std;

  const std::string & tube::tube_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "tube";
    }
    return label;
  }

  double tube::get_xmin () const
  {
    return -_outer_r_;
  }

  double tube::get_xmax () const
  {
    return +_outer_r_;
  }

  double tube::get_ymin () const
  {
    return -_outer_r_;
  }

  double tube::get_ymax () const
  {
    return +_outer_r_;
  }

  double tube::get_zmin () const
  {
    return -0.5*_z_;
  }

  double tube::get_zmax () const
  {
    return +0.5*_z_;
  }

  double tube::get_z () const
  {
    return _z_;
  }

  void tube::set_z (double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0,std::logic_error ,
                 "Invalid '" << new_value_ << "' Z value !");
    _z_ = new_value_;
    return;
  }

  double tube::get_half_z () const
  {
    return 0.5 * _z_;
  }

  void tube::set_half_z (double new_value_)
  {
    set_z (new_value_ + new_value_);
    return;
  }

  bool tube::is_extruded () const
  {
    return _inner_r_ > 0.0;
  }

  double tube::get_inner_r () const
  {
    return _inner_r_;
  }

  double tube::get_outer_r () const
  {
    return _outer_r_;
  }

  void tube::set_radii (double inner_r_, double outer_r_)
  {
    DT_THROW_IF (inner_r_ < 0.0, std::logic_error, "Invalid inner radius '" << inner_r_ << "' !");
    _inner_r_ = inner_r_;
    DT_THROW_IF (outer_r_ < 0.0, std::logic_error, "Invalid outer radius '" << outer_r_ << "' !");
    _outer_r_ = outer_r_;
    DT_THROW_IF (_outer_r_ <= _inner_r_,
                 std::logic_error,
                 "Outer radius '"
                 << _outer_r_ << "' <= inner radius '"
                 << _inner_r_ << "'!");
    return;
  }

  void tube::set (double ir_, double or_, double z_)
  {
    set_radii (ir_ ,or_);
    set_z (z_);
    return;
  }

  void tube::set_half (double ir_, double or_, double hz_)
  {
    set_radii (ir_, or_);
    set_half_z (hz_);
    return;
  }

  tube::tube ()
  {
    reset ();
    return;
  }

  tube::tube (double ir_, double or_, double z_)
  {
    set (ir_, or_, z_);
    return;
  }

   tube::~tube ()
  {
    return;
  }

  double tube::get_surface (uint32_t mask_) const
  {
    double s = 0.0;
    if (mask_ & FACE_INNER_SIDE)
      {
        s += 2.0 * M_PI * _inner_r_ * _z_;
      }
    if ( mask_ & FACE_OUTER_SIDE )
      {
        s += 2.0 * M_PI * _outer_r_ * _z_;
      }
    if ( mask_ & FACE_BOTTOM )
      {
        s += M_PI * (_outer_r_ * _outer_r_ - _inner_r_ * _inner_r_);
      }
    if ( mask_ & FACE_TOP )
      {
        s += M_PI * (_outer_r_ * _outer_r_ - _inner_r_ * _inner_r_);
      }
    return s;
  }

  double tube::get_volume (uint32_t /*flags*/) const
  {
    return M_PI * (_outer_r_ * _outer_r_ - _inner_r_ * _inner_r_ ) * _z_;
  }

  bool tube::is_valid () const
  {
    return ((_inner_r_ > 0.0) && (_outer_r_ > _inner_r_) && _z_ > 0.0);
  }

  void tube::reset ()
  {
    _inner_r_ = -1.0;
    _outer_r_ = -1.0;
    _z_       = -1.0;
  }

  std::string tube::get_shape_name () const
  {
    return tube::tube_label();
  }

  bool tube::is_inside ( const vector_3d & point_ , double skin_ ) const
  {
    double skin = get_skin (skin_);
    double hskin = 0.5 * skin;
    double r = hypot(point_.x(), point_.y());
    if ( (r <= (_outer_r_ - hskin))
         && (r >= (_inner_r_ + hskin))
         && (std::abs(point_.z()) <= (0.5 * _z_ - hskin))
         ) return true;
    return false;
  }

  bool tube::is_outside ( const vector_3d & point_ , double skin_ ) const
  {
    double skin = get_skin (skin_);
    double hskin = 0.5 * skin;
    double r = hypot(point_.x(), point_.y());
    if ( (r >= (_outer_r_ + hskin))
         || (r <= (_inner_r_ - hskin))
         || (std::abs(point_.z()) >= (0.5 * _z_ + hskin))
         ) return true;
    return false;
  }

  double tube::get_parameter ( const std::string & flag_ ) const
  {
    if ( flag_ == "inner.r" ) return get_inner_r ();
    if ( flag_ == "outer.r" ) return get_outer_r ();
    if ( flag_ == "z" ) return get_z ();
    if ( flag_ == "volume" ) return get_volume ();
    if ( flag_ == "surface.top" ) return get_surface (FACE_TOP);
    if ( flag_ == "surface.bottom" ) return get_surface (FACE_BOTTOM);
    if ( flag_ == "surface.inner.side" ) return get_surface (FACE_INNER_SIDE);
    if ( flag_ == "surface.outer.side" ) return get_surface (FACE_OUTER_SIDE);
    if ( flag_ == "surface" ) return get_surface (FACE_ALL);
    DT_THROW_IF(true, std::runtime_error, "Unknown flag !");
  }

  vector_3d tube::get_normal_on_surface (const vector_3d & position_) const
  {
    vector_3d normal;
    invalidate (normal);
    if (is_on_surface (position_, FACE_OUTER_SIDE))
      {
        double phi = position_.phi ();
        normal.set (std::cos (phi), std::sin (phi), 0.0);
      }
    else if (is_on_surface (position_, FACE_INNER_SIDE))
      {
        double phi = position_.phi ();
        normal.set (std::cos (phi), std::sin (phi), 0.0);
        normal *= -1.;
      }
    else if (is_on_surface (position_, FACE_BOTTOM)) normal.set (0.0, 0.0, -1.0);
    else if (is_on_surface (position_, FACE_TOP)) normal.set (0.0, 0.0, +1.0);
    return (normal);
  }

  bool tube::is_on_surface (const vector_3d & point_,
                            int mask_,
                            double skin_) const
  {
    double skin = get_skin ();
    if (skin_ > USING_PROPER_SKIN) skin = skin_;

    int mask = mask_;
    if (mask_ == (int) ALL_SURFACES) mask = FACE_ALL;

    double r = hypot (point_.x (), point_.y ());
    if (mask & FACE_BOTTOM)
      {
        if ( (std::abs (point_.z () + 0.5 * _z_) < 0.5 * skin)
             && (r < _outer_r_ && r > _inner_r_)) return true;
      }
    if (mask & FACE_TOP)
      {
        if ( ( std::abs (point_.z () - 0.5 * _z_) < 0.5 * skin)
             && (r < _outer_r_ && r > _inner_r_)) return true;
      }
    if (mask & FACE_OUTER_SIDE)
      {
        if ( (std::abs (point_.z ()) < 0.5 * _z_)
             && (std::abs (r - _outer_r_) < 0.5 * skin)) return true;
      }
    if (mask & FACE_INNER_SIDE)
      {
        if ( (std::abs (point_.z ()) < 0.5 * _z_)
             && (std::abs (r - _inner_r_) < 0.5 * skin)) return true;
      }
    return false;
  }

  bool tube::find_intercept (const vector_3d & /*from_*/,
                             const vector_3d & /*direction_*/,
                             intercept_t & intercept_,
                             double /*skin_*/) const
  {
    DT_THROW_IF(true, std::runtime_error, "Not implemented !");
    intercept_.reset ();
    return intercept_.is_ok ();
  }

  std::ostream & operator<< (std::ostream & out_, const tube & t_)
  {
    out_ << '{' << tube::tube_label() << ' '
         << t_._inner_r_ << ' '
         << t_._outer_r_ << ' '
         << t_._z_ << '}';
    return out_;
  }

  std::istream & operator>> (std::istream & in_, tube & t_)
  {
    t_.reset ();
    char c = 0;
    in_.get (c);
    if (c != '{')
      {
        in_.clear (std::ios_base::failbit);
        return in_;
      }
    std::string name;
    in_ >> name;
    if (name != tube::tube_label())
      {
        in_.clear (std::ios_base::failbit);
        return in_;
      }
    double in_r, out_r, z;
    in_ >> in_r >> out_r >> z;
    if (! in_)
      {
        in_.clear (std::ios_base::failbit);
        return in_;
      }
    c = 0;
    in_.get (c);
    if (c != '}')
      {
        in_.clear (std::ios_base::failbit);
        return in_;
      }
    try
      {
        t_.set (in_r, out_r, z);
      }
    catch (...)
      {
        t_.reset ();
        in_.clear (std::ios_base::failbit);
      }
    return in_;
  }

  void tube::compute_inner_cylinder (cylinder & ic_)
  {
    ic_.reset ();
    DT_THROW_IF (! is_valid (),logic_error, "Tube is not valid !");
    ic_.set (get_inner_r (), get_z ());
    return;
  }

  void tube::compute_outer_cylinder (cylinder & oc_)
  {
    oc_.reset ();
    DT_THROW_IF (! is_valid (),logic_error, "Tube is not valid !");
    oc_.set (get_outer_r (), get_z ());
    return;
  }

  void tube::tree_dump (ostream & out_,
                        const string & title_,
                        const string & indent_,
                        bool inherit_) const
  {
    string indent;
    if (! indent_.empty ()) indent = indent_;
    i_object_3d::tree_dump (out_, title_, indent_, true);

    out_ << indent << datatools::i_tree_dumpable::tag
         << "R(internal) : " << get_inner_r () / CLHEP::mm << " mm" << endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "R(external) : " << get_outer_r () / CLHEP::mm << " mm" << endl;

    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Z : " << get_z () / CLHEP::mm << " mm" << endl;
    return;
  }

  void tube::generate_wires (std::list<polyline_3d> & lpl_,
                             const placement & p_,
                             uint32_t /*options_*/) const
  {
    const int nsamples = 36;
    double r[2];
    r[0]=get_inner_r ();
    r[1]=get_outer_r ();
    for (int k = 0; k < 2; k++)
      {
        for (int j = 0; j < 2; j++)
          {
            vector_3d vertex[nsamples];
            for (int i = 0; i < nsamples; i++)
              {
                double thetai = i * 2. * M_PI/nsamples;
                vertex[i].set (r[k] * std::cos (thetai),
                               r[k] * std::sin (thetai),
                               -0.5 * get_z () + j * get_z ());
              }
            {
              polyline_3d dummy;
              lpl_.push_back (dummy);
            }
            polyline_3d & pl = lpl_.back ();
            pl.set_closed (true);
            for (int i = 0; i < 36; i++)
              {
                vector_3d v;
                p_.child_to_mother (vertex[i], v);
                pl.add (v);
              }
          }

        for (int i = 0; i < nsamples; i++)
          {
            vector_3d vertex[2];
            double thetai = i * 2. * M_PI/nsamples;
            double x = r[k] * std::cos (thetai);
            double y = r[k] * std::sin (thetai);
            vertex[0].set (x, y, -0.5 * get_z ());
            vertex[1].set (x, y, +0.5 * get_z ());
            {
              polyline_3d dummy;
              lpl_.push_back (dummy);
              polyline_3d & pl = lpl_.back ();
              pl.set_closed (false);
              for (int i = 0; i < 2; i++)
                {
                  vector_3d v;
                  p_.child_to_mother (vertex[i], v);
                  pl.add (v);
                }
            }
          }
      }
    return;
  }

  void tube::initialize(const datatools::properties & config_)
  {
    reset();
    double lunit = CLHEP::mm;
    if (config_.has_key("length_unit")) {
      const std::string lunit_str = config_.fetch_string("length_unit");
      lunit = datatools::units::get_length_unit_from(lunit_str);
    }

    // double aunit = CLHEP::degree;
    // if (config_.has_key("angle_unit")) {
    //   const std::string aunit_str = config_.fetch_string("angle_unit");
    //   aunit = datatools::units::get_angle_unit_from(aunit_str);
    // }

    double inner_r;
    datatools::invalidate (inner_r);
    if (config_.has_key ("inner_r")) {
      inner_r = config_.fetch_real ("inner_r");
      if (! config_.has_explicit_unit ("inner_r")) {
        inner_r *= lunit;
      }
    } else if (config_.has_key ("inner_radius")) {
      inner_r = config_.fetch_real ("inner_radius");
      if (! config_.has_explicit_unit ("inner_radius")) {
        inner_r *= lunit;
      }
    } else if (config_.has_key ("inner_diameter")) {
      inner_r = 0.5 * config_.fetch_real ("inner_diameter");
      if (! config_.has_explicit_unit ("inner_diameter")) {
        inner_r *= lunit;
      }
    }
    if (! datatools::is_valid(inner_r)) {
      // DT_LOG_WARNING (get_logging_priority (),
      //                 "Missing tube 'inner_r' property ! Using 0-default inner radius !");
      inner_r = 0.0;
    }

    double outer_r;
    datatools::invalidate (outer_r);
    if (config_.has_key ("outer_r")) {
      outer_r = config_.fetch_real ("outer_r");
      if (! config_.has_explicit_unit ("outer_r")) {
        outer_r *= lunit;
      }
    } else if (config_.has_key ("outer_radius")) {
      outer_r = config_.fetch_real ("outer_radius");
      if (! config_.has_explicit_unit ("outer_radius")) {
        outer_r *= lunit;
      }
    } else if (config_.has_key ("outer_diameter")) {
      outer_r = 0.5 * config_.fetch_real ("outer_diameter");
      if (! config_.has_explicit_unit ("outer_diameter")) {
        outer_r *= lunit;
      }
    }
    DT_THROW_IF (! datatools::is_valid (outer_r), std::logic_error,
                 "Missing tube 'outer_r' property !");


    DT_THROW_IF (! config_.has_key("z"), std::logic_error,
                 "Missing tube 'z' property !");
    double z = config_.fetch_real("z");
    if (! config_.has_explicit_unit("z")) {
      z *= lunit;
    }

    set_radii(inner_r, outer_r);
    set_z(z);

    return;
  }

  // static
  void tube::init_ocd(datatools::object_configuration_description & ocd_)
  {
    i_shape_3d::init_ocd(ocd_);

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("length_unit")
        .set_from("geomtools::tube")
        .set_terse_description("The unit symbol used for length unit")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_default_value_string("mm")
        .set_long_description("A character string that represents the default    \n"
                              "length unit for length parameters of the tube.\n"
                              )
        .add_example("Set the default length unit::       \n"
                     "                                    \n"
                     "  length_unit : string = \"mm\"     \n"
                     "                                    \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("inner_r")
        .set_from("geomtools::tube")
        .set_terse_description("The inner radius of the tube")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_default_value_real(0.0)
        .add_example("Set the inner radius::               \n"
                     "                                     \n"
                     "  inner_r : real as length = 2.0 cm  \n"
                     "                                     \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("inner_radius")
        .set_from("geomtools::cylinder")
        .set_terse_description("The inner radius of the cylinder")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_long_description("Alias of the ``inner_r`` property.\n"
                              "Not used if the ``inner_r`` property is set."
                              )
        .add_example("Set the inner radius::                    \n"
                     "                                          \n"
                     "  inner_radius : real as length = 2.0 cm  \n"
                     "                                          \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("inner_diameter")
        .set_from("geomtools::cylinder")
        .set_terse_description("The inner diameter of the cylinder")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_long_description("Not used if the ``inner_r`` or ``inner_radius`` property is set.")
        .add_example("Set the inner diameter::                    \n"
                     "                                            \n"
                     "  inner_diameter : real as length = 4.0 cm  \n"
                     "                                            \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("outer_r")
        .set_from("geomtools::tube")
        .set_terse_description("The outer radius of the tube")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(true)
        .add_example("Set the outer radius::               \n"
                     "                                     \n"
                     "  outer_r : real as length = 5.0 cm  \n"
                     "                                     \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("outer_radius")
        .set_from("geomtools::cylinder")
        .set_terse_description("The outer radius of the cylinder")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_long_description("Not used if the ``outer_r`` property is set.")
        .add_example("Set the outer radius::                   \n"
                     "                                         \n"
                     "  outer_radius : real as length = 5.0 cm \n"
                     "                                         \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("outer_diameter")
        .set_from("geomtools::cylinder")
        .set_terse_description("The outer diameter of the cylinder")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_long_description("Not used if the ``outer_r`` or ``outer_radius`` property is set.")
        .add_example("Set the outer diameter::                    \n"
                     "                                            \n"
                     "  outer_diameter : real as length = 10.0 cm \n"
                     "                                            \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("z")
        .set_from("geomtools::tube")
        .set_terse_description("The dimension of the tube along the Z axis")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(true)
        .add_example("Set the Z dimension::          \n"
                     "                               \n"
                     "  z : real as length = 5.0 mm  \n"
                     "                               \n"
                     )
        ;
    }

    return;
  }

} // end of namespace geomtools

/** Opening macro for implementation
 *  @arg geomtools::tube the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized(passed by mutable reference).
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(geomtools::tube, ocd_)
{
  // The class name :
  ocd_.set_class_name("geomtools::tube");

  // The class terse description :
  ocd_.set_class_description("A class representing a 3D tube solid shape");

  // The library the class belongs to :
  ocd_.set_class_library("geomtools");

  // The class detailed documentation :
  ocd_.set_class_documentation("The tube solid 3D-shape is defined by its three  \n"
                               "dimensions.                                       \n"
                               );

  // Populate the specific OCD :
  geomtools::tube::init_ocd(ocd_);

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the      \n"
                               "``datatools::properties`` ASCII format::         \n"
                               "                                                 \n"
                               "  tolerance.length : real = 1.0e-6 um            \n"
                               "  length_unit      : string = \"mm\"             \n"
                               "  inner_r : real as length = 15.0 mm             \n"
                               "  outer_r : real as length = 25.0 mm             \n"
                               "  z       : real           = 25.3 cm             \n"
                               "                                                 \n"
                               );

  /* Set the validation support flag :
   *  we activate this if the description of all configuration
   *  properties has been provides(see above). This enables the
   *  OCD tools to check the syntax and validaty of a given
   *  configuration file.
   */
  ocd_.set_validation_support(true);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'geomtools::tube' :
DOCD_CLASS_SYSTEM_REGISTRATION(geomtools::tube, "geomtools::tube")

/** geomtools/ellipsoid.cc */

// Ourselves:
#include <geomtools/ellipsoid.h>

// Standard library:
#include <cmath>
#include <stdexcept>
#include <sstream>

// Third party:
// - GSL:
#include <gsl/gsl_math.h>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/exception.h>
#include <datatools/units.h>
#include <datatools/properties.h>

namespace geomtools {

  using namespace std;

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(ellipsoid, "geomtools::ellipsoid");

  const std::string & ellipsoid::ellipsoid_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "ellipsoid";
    }
    return label;
  }

  void ellipsoid::_build_bounding_data()
  {
    _grab_bounding_data().make_box(get_xmin(), get_xmax(),
                                   get_ymin(), get_ymax(),
                                   get_zmin(), get_zmax());
    return;
  }

  double
  ellipsoid::get_xmin() const
  {
    return -get_x_radius();
  }

  double
  ellipsoid::get_xmax() const
  {
    return get_x_radius();
  }

  double
  ellipsoid::get_ymin() const
  {
    return -get_y_radius();
  }

  double
  ellipsoid::get_ymax() const
  {
    return get_y_radius();
  }

  double
  ellipsoid::get_zmin() const
  {
    return -(get_z_radius()-get_bottom_z_cut());
  }

  double
  ellipsoid::get_zmax() const
  {
    return get_z_radius()-get_top_z_cut();
  }

  double
  ellipsoid::get_x_radius() const
  {
    return _x_radius_;
  }

  void
  ellipsoid::set_x_radius (double r_)
  {
    DT_THROW_IF(r_ <= 0.0, std::domain_error,
                "Invalid radius (" << r_ << ") !");
    _x_radius_ = r_;
  }

  double
  ellipsoid::get_y_radius() const
  {
    return _y_radius_;
  }

  void
  ellipsoid::set_y_radius (double r_)
  {
    DT_THROW_IF(r_ <= 0.0, std::domain_error,
                "Invalid radius (" << r_ << ") !");
    _y_radius_ = r_;
  }

  double
  ellipsoid::get_z_radius() const
  {
    return _z_radius_;
  }

  void
  ellipsoid::set_z_radius (double r_)
  {
    DT_THROW_IF(r_ <= 0.0, std::domain_error,
                "Invalid radius (" << r_ << ") !");
    _z_radius_ = r_;
  }

  double ellipsoid::get_bottom_z_cut() const
  {
    return _bottom_z_cut_;
  }

  void ellipsoid::set_bottom_z_cut(double cut_)
  {
    _bottom_z_cut_ = cut_;
  }

  double ellipsoid::get_top_z_cut() const
  {
    return _top_z_cut_;
  }

  void ellipsoid::set_top_z_cut(double cut_)
  {
    _top_z_cut_ = cut_;
  }

  void
  ellipsoid::reset()
  {
    unlock();

    _set_default();

    this->i_shape_3d::reset();
    return;
  }

  void ellipsoid::_set_default()
  {
    datatools::invalidate(_x_radius_);
    datatools::invalidate(_y_radius_);
    datatools::invalidate(_z_radius_);
    _bottom_z_cut_ = 0.;
    _top_z_cut_ = 0.;

    return;
  }

  void ellipsoid::set (double rx_, double ry_, double rz_)
  {
    set_x_radius(rx_);
    set_y_radius(ry_);
    set_z_radius(rz_);
    return;
  }

  void ellipsoid::set (double rx_, double ry_, double rz_,
                       double zm_, double zp_)
  {
    set_x_radius(rx_);
    set_y_radius(ry_);
    set_z_radius(rz_);
    set_bottom_z_cut (zm_);
    set_top_z_cut (zp_);
    return;
  }

  ellipsoid::ellipsoid()
  {
    _set_default();
    return;
  }

  ellipsoid::ellipsoid (double rx_, double ry_, double rz_)
  {
    _set_default();
    set (rx_, ry_, rz_);
    return;
  }

  ellipsoid::ellipsoid (double rx_, double ry_, double rz_,
                        double zm_, double zp_)
  {
    _set_default();
    set (rx_, ry_, rz_, zm_, zp_);
    return;
  }

  ellipsoid::~ellipsoid()
  {
  }

  void ellipsoid::initialize(const datatools::properties & config_,
                             const handle_dict_type *)
  {
    reset();
    double lunit = CLHEP::mm;
    if (config_.has_key ("length_unit")) {
      const std::string lunit_str = config_.fetch_string ("length_unit");
      lunit = datatools::units::get_length_unit_from (lunit_str);
    }

    double x_radius;
    datatools::invalidate (x_radius);
    if (config_.has_key ("x_radius")) {
      x_radius = config_.fetch_real ("x_radius");
      if (! config_.has_explicit_unit ("x_radius")) {
        x_radius *= lunit;
      }
    } else if (config_.has_key ("x_diameter")) {
      x_radius = 0.5 * config_.fetch_real ("x_diameter");
      if (! config_.has_explicit_unit ("x_diameter")) {
        x_radius *= lunit;
      }
    }
    DT_THROW_IF (! datatools::is_valid (x_radius), std::logic_error,
                 "Missing elliptical_tube 'x_radius' property !");

    double y_radius;
    datatools::invalidate (y_radius);
    if (config_.has_key ("y_radius")) {
      y_radius = config_.fetch_real ("y_radius");
      if (! config_.has_explicit_unit ("y_radius")) {
        y_radius *= lunit;
      }
    } else if (config_.has_key ("y_diameter")) {
      y_radius = 0.5 * config_.fetch_real ("y_diameter");
      if (! config_.has_explicit_unit ("y_diameter")) {
        y_radius *= lunit;
      }
    }
    DT_THROW_IF (! datatools::is_valid (y_radius), std::logic_error,
                 "Missing elliptical_tube 'y_radius' property !");

    double z_radius;
    datatools::invalidate (z_radius);
    if (config_.has_key ("z_radius")) {
      z_radius = config_.fetch_real ("z_radius");
      if (! config_.has_explicit_unit ("z_radius")) {
        z_radius *= lunit;
      }
    } else if (config_.has_key ("x_diameter")) {
      z_radius = 0.5 * config_.fetch_real ("x_diameter");
      if (! config_.has_explicit_unit ("x_diameter")) {
        z_radius *= lunit;
      }
    }
    DT_THROW_IF (! datatools::is_valid (z_radius), std::logic_error,
                 "Missing elliptical_tube 'z_radius' property !");

    double bottom_z;
    bottom_z = 0.;
    if (config_.has_key ("bottom_z_cut")) {
      bottom_z = config_.fetch_real ("bottom_z_cut");
      if (! config_.has_explicit_unit ("bottom_z_cut")) {
        bottom_z *= lunit;
      }
    }

    double top_z;
    top_z = 0.;
    if (config_.has_key ("top_z_cut")) {
      top_z = config_.fetch_real ("top_z_cut");
      if (! config_.has_explicit_unit ("top_z_cut")) {
        top_z *= lunit;
      }
    }

    set(x_radius, y_radius, z_radius, bottom_z, top_z);
    return;
  }

  double
  ellipsoid::get_surface (uint32_t /*mask_*/) const
  {
    double s = 0.0;
    DT_THROW_IF(true, std::runtime_error, "Not implemented !");
    return s;
  }

  double
  ellipsoid::get_volume (uint32_t /*flags*/) const
  {
    double v = 0.0;
    DT_THROW_IF(true, std::runtime_error, "Not implemented !");
    return v;
  }

  bool
  ellipsoid::is_valid() const
  {
    return ( datatools::is_valid(_x_radius_) && (_x_radius_ > 0.0)
             && datatools::is_valid(_y_radius_) && (_y_radius_ > 0.0)
             && datatools::is_valid(_z_radius_) && (_z_radius_ > 0.0)
             && datatools::is_valid(_top_z_cut_) && (_top_z_cut_ >= 0.0)
             && datatools::is_valid(_bottom_z_cut_) && (_bottom_z_cut_ >= 0.0)
             && (_top_z_cut_+_bottom_z_cut_ < 2.*_z_radius_) );
      }

  std::string
  ellipsoid::get_shape_name() const
  {
    return ellipsoid_label();
  }

  bool
  ellipsoid::is_inside (const vector_3d & /*point_*/, double /*skin_*/) const
  {
    DT_THROW_IF(true, std::runtime_error, "Not implemented !");
    return false;
  }

  bool
  ellipsoid::is_outside (const vector_3d & /*point_*/, double /*skin_*/) const
  {
    DT_THROW_IF(true, std::runtime_error, "Not implemented !");
    return false;
  }

  double
  ellipsoid::get_parameter (const std::string & flag_) const
  {
    if  (flag_ == "x_radius")     return get_x_radius();
    if  (flag_ == "y_radius")     return get_y_radius();
    if  (flag_ == "z_radius")     return get_z_radius();
    if  (flag_ == "bottom_z_cut") return get_bottom_z_cut();
    if  (flag_ == "top_z_cut")    return get_top_z_cut();
    if  (flag_ == "surface")      return get_surface (FACE_ALL);
    if  (flag_ == "volume")       return get_volume();
    DT_THROW_IF(true, std::logic_error,"Unknown flag '" << flag_ << "' !");
  }

  vector_3d
  ellipsoid::get_normal_on_surface (const vector_3d & /*position_*/) const
  {
    vector_3d normal;
    invalidate (normal);
    DT_THROW_IF(true, std::runtime_error, "Not implemented !");
    return (normal);
  }

  bool
  ellipsoid::is_on_surface (const vector_3d & /*point_*/,
                            int /*mask_*/,
                            double /*skin_*/) const
  {
    DT_THROW_IF(true, std::runtime_error, "Not implemented !");
    return false;
  }

  bool
  ellipsoid::find_intercept (const vector_3d & /*from_*/,
                             const vector_3d & /*direction_*/,
                             intercept_t & intercept_,
                             double /*skin_*/) const
  {
    DT_THROW_IF(true, std::runtime_error, "Not implemented !");
    intercept_.reset ();
    return intercept_.is_ok();
  }

  std::ostream &
  operator<< (std::ostream & out_, const ellipsoid & e_)
  {
    out_ << '{' << ellipsoid::ellipsoid_label() << ' '
         << e_._x_radius_ << ' '
         << e_._y_radius_ << ' '
         << e_._z_radius_ << ' '
         << e_._bottom_z_cut_ << ' '
         << e_._top_z_cut_ << '}';
    return out_;
  }

  std::istream &
  operator>> (std::istream & in_, ellipsoid & e_)
  {
    e_.reset();
    char c = 0;
    in_.get (c);
    if (c != '{') {
      in_.clear (std::ios_base::failbit);
      return in_;
    }
    std::string name;
    in_ >> name;
    if (name != ellipsoid::ellipsoid_label()) {
      in_.clear (std::ios_base::failbit);
      return in_;
    }
    double rx, ry, rz;
    in_ >> rx >> ry >> rz;
    if (! in_) {
      in_.clear (std::ios_base::failbit);
      return in_;
    }
    double zbot, ztop;
    in_ >> zbot >> ztop;
    if (! in_) {
      in_.clear (std::ios_base::failbit);
      return in_;
    }
    c = 0;
    in_.get (c);
    if (c != '}')  {
      in_.clear (std::ios_base::failbit);
      return in_;
    }
    try {
      e_.set(rx, ry, rz, zbot, ztop);
    }
    catch (...) {
      e_.reset();
      in_.clear (std::ios_base::failbit);
    }
    return in_;
  }

  void ellipsoid::tree_dump (ostream & out_,
                             const string & title_,
                             const string & indent_,
                             bool inherit_) const
  {
    string indent;
    if (! indent_.empty()) indent = indent_;
    i_object_3d::tree_dump (out_, title_, indent_, true);

    out_ << indent << datatools::i_tree_dumpable::tag
         << "X radius : " << get_x_radius () / CLHEP::mm << " mm" << endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Y radius : " << get_y_radius () / CLHEP::mm << " mm" << endl;
    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Z radius : " << get_z_radius () / CLHEP::mm << " mm" << endl;

    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Z bottom-cut : " << get_bottom_z_cut () / CLHEP::mm << " mm" << endl;
    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Z top-cut    : " << get_top_z_cut () / CLHEP::mm << " mm" << endl;

    return;
  }

  // static
  void ellipsoid::init_ocd(datatools::object_configuration_description & ocd_)
  {
    i_shape_3d::init_ocd(ocd_);

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("length_unit")
        .set_from("geomtools::ellipsoid")
        .set_terse_description("The unit symbol used for length unit")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_default_value_string("mm")
        .set_long_description("A character string that represents the default \n"
                              "length unit for length parameters of the box.  \n"
                              )
        .add_example("Set the default length unit:        \n"
                     "                                    \n"
                     "  length_unit : string = \"mm\"     \n"
                     "                                    \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("x_radius")
        .set_from("geomtools::ellipsoid")
        .set_terse_description("The x radius of the ellipsoid")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(true)
        .add_example("Set the x radius:                     \n"
                     "                                      \n"
                     "  x_radius : real as length = 2.0 cm  \n"
                     "                                      \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("y_radius")
        .set_from("geomtools::ellipsoid")
        .set_terse_description("The y radius of the ellipsoid")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(true)
        .add_example("Set the y radius:                     \n"
                     "                                      \n"
                     "  y_radius : real as length = 4.0 cm  \n"
                     "                                      \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("z_radius")
        .set_from("geomtools::ellipsoid")
        .set_terse_description("The z radius of the ellipsoid")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(true)
        .add_example("Set the z radius:                     \n"
                     "                                      \n"
                     "  z_radius : real as length = 6.0 cm  \n"
                     "                                      \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("z_top_cut")
        .set_from("geomtools::ellipsoid")
        .set_terse_description("The z cut from top")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .add_example("Set the z cut from top:                   \n"
                     "                                          \n"
                     "  top_z_cut : real as length = 1.0 cm     \n"
                     "                                          \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("z_bottom_cut")
        .set_from("geomtools::ellipsoid")
        .set_terse_description("The z cut from bottom")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .add_example("Set the z cut from bottom:                \n"
                     "                                          \n"
                     "  bottom_z_cut : real as length = 1.5 cm  \n"
                     "                                          \n"
                     )
        ;
    }

    return;
  }

} // end of namespace geomtools

/** Opening macro for implementation
 *  @arg geomtools::ellipsoid the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized (passed by mutable reference).
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(geomtools::ellipsoid, ocd_)
{
  // The class name :
  ocd_.set_class_name ("geomtools::ellipsoid");

  // The class terse description :
  ocd_.set_class_description ("A class representing a 3D ellipsoid solid shape");

  // The library the class belongs to :
  ocd_.set_class_library ("geomtools");

  // The class detailed documentation :
  ocd_.set_class_documentation ("The ellipsoid solid 3D-shape");

  // Populate the specific OCD :
  geomtools::ellipsoid::init_ocd(ocd_);

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the  \n"
                               "``datatools::properties`` ASCII format::     \n"
                               "                                             \n"
                               "  length_unit      : string = \"mm\"         \n"
                               "  tolerance.length : real   = 1.0e-6 um      \n"
                               "  x_radius : real as length = 200.0 mm       \n"
                               "  y_radius : real as length = 100.0 mm       \n"
                               "  z_radius : real as length = 600.0 mm       \n"
                               "  z_top_cut        : real   =  0.0 cm        \n"
                               "  z_bototm_cut     : real   = 30.0 cm        \n"
                               "                                             \n"
                               );

  /** Set the validation support flag :
   *  we activate this if the description of all configuration
   *  properties has been provides (see above). This enables the
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

// Registration macro for class 'geomtools::ellipsoid' :
DOCD_CLASS_SYSTEM_REGISTRATION(geomtools::ellipsoid, "geomtools::ellipsoid")

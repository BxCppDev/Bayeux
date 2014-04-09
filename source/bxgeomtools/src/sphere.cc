/* geomtools/sphere.cc */

// Ourselves:
#include <geomtools/sphere.h>

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

  const std::string & sphere::sphere_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "sphere";
    }
    return label;
  }

  double sphere::get_xmin () const
  {
    return -_r_;
  }

  double sphere::get_xmax () const
  {
    return +_r_;
  }

  double sphere::get_ymin () const
  {
    return -_r_;
  }

  double sphere::get_ymax () const
  {
    return +_r_;
  }

  double sphere::get_zmin () const
  {
    return std::min(_r_ * std::cos(_start_theta_ + _delta_theta_),
                    _r_min_ * std::cos(_start_theta_ + _delta_theta_)
                    );
  }

  double sphere::get_zmax () const
  {
    return std::max(_r_ * std::cos(_start_theta_),
                    _r_min_ * std::cos(_start_theta_)
                    );
  }

  double
  sphere::get_r() const
  {
    return _r_;
  }

  double
  sphere::get_r_max() const
  {
    return _r_;
  }

  double
  sphere::get_r_min () const
  {
    return _r_min_;
  }

  double
  sphere::get_radius () const
  {
    return get_r();
  }

  void
  sphere::set_r (double new_value_)
  {
    DT_THROW_IF  (new_value_ < 0.0,std::domain_error,
                  "Invalid radius (" << new_value_ << ") !");
    _r_ = new_value_;
    if (_r_ < _r_min_) {
      reset_r_min ();
    }
  }

  void
  sphere::set_r_max (double new_value_)
  {
    set_r(new_value_);
  }

  void
  sphere::set_r_min (double new_value_)
  {
    _r_min_ = new_value_;
    DT_THROW_IF(new_value_ < 0.0, std::domain_error,
                "Invalid inner radius (" << new_value_ << ") !");
    if (_r_min_ > _r_) {
      set_r_max(_r_min_);
    }
  }

  void
  sphere::reset_r_max ()
  {
    datatools::invalidate(_r_);
  }

  void
  sphere::reset_r_min ()
  {
    _r_min_ = 0.0;
  }

  void
  sphere::set_radius (double new_value_)
  {
    set_r (new_value_);
  }

  void
  sphere::set (double r_)
  {
    reset_r_min();
    set_r_max(r_);
  }

  void
  sphere::set (double r_min_, double r_max_)
  {
    set_r_min(r_min_);
    set_r_max(r_max_);
  }

  void sphere::set_phi(double start_phi_, double delta_phi_)
  {
    _start_phi_ = start_phi_;
    _delta_phi_ = delta_phi_;
  }

  double sphere::get_start_phi() const
  {
    return _start_phi_;
  }

  double sphere::get_delta_phi() const
  {
    return _delta_phi_;
  }

  void sphere::set_theta(double start_theta_, double delta_theta_)
  {
    _start_theta_ = start_theta_;
    _delta_theta_ = delta_theta_;
  }

  double sphere::get_start_theta() const
  {
    return _start_theta_;
  }

  double sphere::get_delta_theta() const
  {
    return _delta_theta_;
  }

  void sphere::_set_default()
  {
    datatools::invalidate(_r_);
    _r_min_ = 0.0;
    _start_phi_ = 0.0;
    _delta_phi_ = 2 * M_PI;
    _start_theta_ = 0.0;
    _delta_theta_ = M_PI;
  }

  sphere::sphere ()
  {
    _set_default();
  }

  sphere::sphere (double r_)
  {
    _set_default();
    set_r_max(r_);
  }

  sphere::sphere (double radius_min_, double radius_max_)
  {
    _set_default();
    set_r_min(radius_min_);
    set_r_max(radius_max_);
  }

  sphere::~sphere ()
  {
  }

  void sphere::initialize(const datatools::properties & config_)
  {
    reset();
    double lunit = CLHEP::mm;
    if (config_.has_key ("length_unit")) {
      const std::string lunit_str = config_.fetch_string ("length_unit");
      lunit = datatools::units::get_length_unit_from (lunit_str);
    }

    double aunit = CLHEP::degree;
    if (config_.has_key ("angle_unit")) {
      const std::string aunit_str = config_.fetch_string ("angle_unit");
      aunit = datatools::units::get_angle_unit_from (aunit_str);
    }

    double rmin, rmax;
    rmin = 0.0;
    if (config_.has_key ("r_max")) {
      rmax = config_.fetch_real ("r_max");
      if (! config_.has_explicit_unit ("r_max")) {
        rmax *= lunit;
      }
    } else {
      DT_THROW_IF (! config_.has_key ("r"), std::logic_error,
                   "Missing sphere 'r' property !");
      rmax = config_.fetch_real ("r");
      if (! config_.has_explicit_unit ("r")) {
        rmax *= lunit;
      }
    }
    if (config_.has_key ("r_min")) {
      rmin = config_.fetch_real ("r_min");
      if (! config_.has_explicit_unit ("r_min")) {
        rmin *= lunit;
      }
    }

    double start_theta = 0.0;
    double delta_theta = 2 * M_PI * CLHEP::radian;
    bool not_full_theta = false;
    if (config_.has_key ("start_theta")) {
      start_theta = config_.fetch_real ("start_theta");
      if (! config_.has_explicit_unit ("start_theta")) {
        start_theta *= aunit;
      }
      not_full_theta = true;
    }
    if (config_.has_key ("delta_theta")) {
      delta_theta = config_.fetch_real ("delta_theta");
      if (! config_.has_explicit_unit ("delta_theta")) {
        delta_theta *= aunit;
      }
      not_full_theta = true;
    }

    double start_phi = 0.0;
    double delta_phi = M_PI * CLHEP::radian;
    bool not_full_phi = false;
    if (config_.has_key ("start_phi")) {
      start_phi = config_.fetch_real ("start_phi");
      if (! config_.has_explicit_unit ("start_phi")) {
        start_phi *= aunit;
      }
      not_full_phi = true;
    }
    if (config_.has_key ("delta_phi")) {
      delta_phi = config_.fetch_real ("delta_phi");
      if (! config_.has_explicit_unit ("delta_phi")) {
        delta_phi *= aunit;
      }
      not_full_phi = true;
    }
    if (rmin > 0.0) {
      set_r_min(rmin);
    }
    set_r_max(rmax);
    if (not_full_theta) {
      set_theta(start_theta, delta_theta);
    }
    if (not_full_phi) {
      set_phi(start_phi, delta_phi);
    }
    return;
  }

  double
  sphere::get_surface (uint32_t mask_) const
  {
    double s = 0.0;
    if  (mask_ & FACE_SIDE) {
      s += 4.0 * M_PI * _r_ * _r_;
    }
    if  (mask_ & FACE_INNER_SIDE) {
      s += 4.0 * M_PI * _r_min_ * _r_min_;
    }
    return s;
  }

  double
  sphere::get_volume (uint32_t /*flags*/) const
  {
    double v = 4.0 * M_PI * gsl_pow_3(_r_) / 3.0;
    if (datatools::is_valid(_r_min_)) {
      v -=  4.0 * M_PI * gsl_pow_3(_r_min_) / 3.0;
    }
    return v;
  }

  bool
  sphere::is_valid () const
  {
    if (datatools::is_valid(_r_min_)) {
      if (_r_min_ < 0.0) return false;
    }
    return datatools::is_valid(_r_)
      && (_r_ > 0.0);
  }

  void
  sphere::reset ()
  {
    _set_default();
  }

  bool sphere::is_orb() const
  {
    if (! datatools::is_valid(_r_)) return false;
    if (_r_min_ > 0.0) return false;
    if (_start_phi_ > 0.0) return false;
    if (_delta_phi_ < 2 * M_PI) return false;
    if (_start_theta_ > 0.0) return false;
    if (_delta_theta_ < M_PI) return false;
    return true;
  }

  std::string
  sphere::get_shape_name () const
  {
    return sphere_label();
  }

  bool
  sphere::is_inside (const vector_3d & point_, double skin_) const
  {
    double skin = get_skin ();
    if  (skin_ > USING_PROPER_SKIN) skin = skin_;
    double r = point_.mag ();
    if  (r > _r_ + 0.5 * skin) return false;
    if  (r < _r_min_ - 0.5 * skin) return false;
    return true;
  }

  double
  sphere::get_parameter (const std::string & flag_) const
  {
    if  (flag_ == "r") return get_r ();
    if  (flag_ == "radius") return get_r ();
    if  (flag_ == "outer_radius") return get_r_max();
    if  (flag_ == "inner_radius") return get_r_min();
    if  (flag_ == "outer_diameter") return 2 * get_r_max ();
    if  (flag_ == "diameter") return 2 * get_r ();
    if  (flag_ == "inner_diameter") return 2 * get_r_min ();
    if  (flag_ == "volume") return get_volume ();
    if  (flag_ == "surface.side") return get_surface (FACE_SIDE);
    if  (flag_ == "surface.inner_side") return get_surface (FACE_INNER_SIDE);
    if  (flag_ == "surface") return get_surface (FACE_ALL);
    DT_THROW_IF(true, std::logic_error,"Unknown flag '" << flag_ << "' !");
  }

  vector_3d
  sphere::get_normal_on_surface (const vector_3d & position_) const
  {
    vector_3d normal;
    invalidate (normal);
    if (is_on_surface (position_, FACE_SIDE)) {
      normal = position_;
      normal.unit ();
    }
    if (is_on_surface (position_, FACE_INNER_SIDE)) {
      normal = -position_;
      normal.unit ();
    }
    return (normal);
  }

  bool
  sphere::is_on_surface (const vector_3d & point_,
                         int mask_    ,
                         double skin_) const
  {
    // bool debug = false;
    //debug = true;
    double skin = get_skin ();
    if (skin_ > USING_PROPER_SKIN) skin = skin_;

    int mask = mask_;
    if (mask_ == (int) ALL_SURFACES) mask = FACE_ALL;

    double hskin = 0.5 * skin;
    double r = point_.mag ();
    // if (debug)
    //   {
    //     std::clog << "DEVEL: sphere::is_on_surface: r= "
    //               << r
    //               << std::endl;
    //     std::clog << "DEVEL: sphere::is_on_surface: R= "
    //               << _r_
    //               << std::endl;
    //     std::clog << "DEVEL: sphere::is_on_surface: hskin= "
    //               << hskin
    //               << std::endl;
    //   }
    if (mask & FACE_SIDE) {
      if (std::abs(r - _r_) < hskin) return true;
    }
    if (mask & FACE_INNER_SIDE) {
      if (std::abs(r - _r_min_) < hskin) return true;
    }
    double phi = point_.phi();
    if (phi >= _start_phi_ && phi <= _start_phi_ + _delta_phi_) return true;
    double theta = point_.theta();
    if (theta >= _start_theta_ && theta <= _start_theta_ + _delta_theta_) return true;
    return false;
  }

  bool
  sphere::find_intercept (const vector_3d & from_,
                          const vector_3d & direction_,
                          intercept_t & intercept_,
                          double skin_) const
  {
    // bool debug = false;
    //debug = true;
    const unsigned int NFACES = 1;
    double t[NFACES];
    const int SPH_SIDE = 0;
    t[SPH_SIDE] = -1.0; // side
    double a, b, c;
    double x0 = from_.x ();
    double y0 = from_.y ();
    double z0 = from_.z ();
    double dx = direction_.x ();
    double dy = direction_.y ();
    double dz = direction_.z ();
    a = dx * dx + dy * dy + dz * dz;
    b = 2. * (dx * x0 + dy * y0 + dz * z0);
    c = x0 * x0 + y0 * y0 + z0 * z0 - _r_ * _r_;
    double delta = b * b - 4. * a * c;
    double ts[2];
    // if (debug)
    //   {
    //     std::clog << "DEVEL: sphere::find_intercept: from= "
    //               << from_
    //               << std::endl;
    //     std::clog << "DEVEL: sphere::find_intercept: direction= "
    //               << direction_
    //               << std::endl;
    //     std::clog << "DEVEL: sphere::find_intercept: a= "
    //               << a
    //               << std::endl;
    //     std::clog << "DEVEL: sphere::find_intercept: b= "
    //               << b
    //               << std::endl;
    //     std::clog << "DEVEL: sphere::find_intercept: c= "
    //               << c
    //               << std::endl;
    //     std::clog << "DEVEL: sphere::find_intercept: delta= "
    //               << delta
    //               << std::endl;
    //   }
    if (delta >= 0.0){
      double q = std::sqrt (delta);
      double n = a + a;
      ts[0] = (- b + q) / n;
      ts[1] = (- b - q) / n;
    }
    // if (debug)
    //   {
    //     std::clog << "DEVEL: sphere::find_intercept: ts[" << 0 << "]= "
    //               << ts[0]
    //               << std::endl;
    //     std::clog << "DEVEL: sphere::find_intercept: ts[" << 1 << "]= "
    //               << ts[1]
    //               << std::endl;
    //   }
    for (int i = 0; i < 2; i++) {
      double tsi = ts[i];
      if (std::isnormal (tsi) && (tsi > 0.0)) {
        if (t[SPH_SIDE] < 0) {
          t[SPH_SIDE] = tsi;
        } else {
          if (tsi < t[SPH_SIDE]) {
            t[SPH_SIDE] = tsi;
          }
        }
      }
    }

    double t_min = -1.0;
    int face_min = 0;
    for (int i = 0; i < (int) NFACES; i++) {
      double ti = t[i];
      // if (debug)
      //   {
      //     std::clog << "DEVEL: sphere::find_intercept: t[" << i << "]= "
      //               << ti << " t_min=" << t_min
      //               << " face_min=" << face_min
      //               << std::endl;
      //   }
      if (std::isnormal(ti) && (ti > 0.0)) {
        int face_bit = (0x1 << i); // face mask
        vector_3d intercept = from_ + direction_ * ti;
        if (is_on_surface(intercept, face_bit, skin_))  {
          if ((t_min < 0.0) || (ti < t_min)) {
            t_min = ti;
            face_min = face_bit;
          }
        }
      }
      // if (debug)
      //   {
      //     std::clog << "DEVEL: sphere::find_intercept: t_min=" << t_min
      //               << " face_min=" << face_min
      //               << std::endl;
      //   }
    }
    intercept_.reset ();
    if (face_min > 0) {
      intercept_.set (0, face_min, from_ + direction_ * t_min);
    }
    return intercept_.is_ok ();
  }

  std::ostream &
  operator<< (std::ostream & out_, const sphere & s_)
  {
    out_ << '{' << sphere::sphere_label() << ' '
         << s_._r_
         << ' ' << s_._r_min_
         << '}';
    return out_;
  }

  std::istream &
  operator>> (std::istream & in_, sphere & s_)
  {
    s_.reset ();
    char c = 0;
    in_.get (c);
    if (c != '{') {
      in_.clear (std::ios_base::failbit);
      return in_;
    }
    std::string name;
    in_ >> name;
    if (name != sphere::sphere_label()) {
      in_.clear (std::ios_base::failbit);
      return in_;
    }
    double r, rmin;
    rmin = 0.0;
    in_ >> r >> rmin;
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
      s_.set(r);
      s_.set_r_min(rmin);
    }
    catch (...) {
      s_.reset ();
      in_.clear (std::ios_base::failbit);
    }
    return in_;
  }

  void sphere::tree_dump (ostream & out_,
                          const string & title_,
                          const string & indent_,
                          bool inherit_) const
  {
    string indent;
    if (! indent_.empty ()) indent = indent_;
    i_object_3d::tree_dump (out_, title_, indent_, true);

    out_ << indent << datatools::i_tree_dumpable::tag
         << "R : " << get_r () / CLHEP::mm << " mm" << endl;
    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "R(min) : " << get_r_min () / CLHEP::mm << " mm" << endl;
    return;
  }

  // static
  void sphere::init_ocd(datatools::object_configuration_description & ocd_)
  {
    i_shape_3d::init_ocd(ocd_);

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("length_unit")
        .set_from("geomtools::sphere")
        .set_terse_description("The unit symbol used for length unit")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_default_value_string("mm")
        .set_long_description("A character string that represents the default \n"
                              "length unit for length parameters of the box.  \n"
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
      cpd.set_name_pattern("angle_unit")
        .set_from("geomtools::sphere")
        .set_terse_description("The unit symbol used for angle unit")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_default_value_string("degree")
        .set_long_description("A character string that represents the default \n"
                              "angle unit for angle parameters of the sphere.  \n"
                              )
        .add_example("Set the default angle unit::        \n"
                     "                                    \n"
                     "  angle_unit : string = \"mm\"      \n"
                     "                                    \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("r_max")
        .set_from("geomtools::sphere")
        .set_terse_description("The outer radius of the sphere")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(true)
        .add_example("Set the outer radius::             \n"
                     "                                   \n"
                     "  r_max : real as length = 5.0 cm  \n"
                     "                                   \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("r_min")
        .set_from("geomtools::sphere")
        .set_terse_description("The inner radius of the sphere")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_default_value_real(0.0)
        .add_example("Set the inner radius::             \n"
                     "                                   \n"
                     "  r_min : real as length = 1.0 cm  \n"
                     "                                   \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("start_phi")
        .set_from("geomtools::sphere")
        .set_terse_description("The starting phi angle")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_default_value_real(0.0)
        .add_example("Set the starting phi angle::              \n"
                     "                                          \n"
                     "  start_phi : real as angle = 30.0 degree \n"
                     "                                          \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("delta_phi")
        .set_from("geomtools::sphere")
        .set_terse_description("The delta phi angle")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_default_value_real(2 * M_PI * CLHEP::radian)
        .add_example("Set the delta phi angle::                 \n"
                     "                                          \n"
                     "  delta_phi : real as angle = 60.0 degree \n"
                     "                                          \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("start_theta")
        .set_from("geomtools::sphere")
        .set_terse_description("The starting theta angle")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_default_value_real(0.0)
        .add_example("Set the starting theta angle::              \n"
                     "                                            \n"
                     "  start_theta : real as angle = 0.0 degree  \n"
                     "                                            \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("delta_theta")
        .set_from("geomtools::sphere")
        .set_terse_description("The delta theta angle")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_default_value_real(M_PI * CLHEP::radian)
        .add_example("Set the delta theta angle::                 \n"
                     "                                            \n"
                     "  delta_theta : real as angle = 90.0 degree \n"
                     "                                            \n"
                     )
        ;
    }

    return;
  }

} // end of namespace geomtools

/** Opening macro for implementation
 *  @arg geomtools::sphere the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized (passed by mutable reference).
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(geomtools::sphere, ocd_)
{
  // The class name :
  ocd_.set_class_name ("geomtools::sphere");

  // The class terse description :
  ocd_.set_class_description ("A class representing a 3D sphere solid shape");

  // The library the class belongs to :
  ocd_.set_class_library ("geomtools");

  // The class detailed documentation :
  ocd_.set_class_documentation ("The sphere solid 3D-shape");

  // Populate the specific OCD :
  geomtools::sphere::init_ocd(ocd_);

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the       \n"
                               "``datatools::properties`` ASCII format::          \n"
                               "                                                  \n"
                               "  tolerance.length : real as length = 1.0e-6 um   \n"
                               "  tolerance.angle  : real as angle = 1.0e-6 radian\n"
                               "  length_unit : string = \"mm\"                   \n"
                               "  angle_unit : string = \"degree\"                \n"
                               "  r_max : real as length = 15.0 mm                \n"
                               "  r_min : real as length = 10.0 mm                \n"
                               "                                                  \n"
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

// Registration macro for class 'geomtools::sphere' :
DOCD_CLASS_SYSTEM_REGISTRATION(geomtools::sphere, "geomtools::sphere")

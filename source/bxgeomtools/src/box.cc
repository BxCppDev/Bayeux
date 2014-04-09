/** \file geomtools/box.cc */

// Ourselves:
#include <geomtools/box.h>

// Standard library:
#include <datatools/utils.h>
#include <datatools/exception.h>
#include <datatools/units.h>
#include <datatools/properties.h>

namespace geomtools {

  const std::string & box::box_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "box";
    }
    return label;
  }

  double box::get_xmin() const
  {
    return -0.5 * _x_;
  }

  double box::get_xmax() const
  {
    return +0.5 * _x_;
  }

  double box::get_ymin() const
  {
    return -0.5 * _y_;
  }

  double box::get_ymax() const
  {
    return +0.5 * _y_;
  }

  double box::get_zmin() const
  {
    return -0.5 * _z_;
  }

  double box::get_zmax() const
  {
    return +0.5 * _z_;
  }

  double
  box::get_x() const
  {
    return _x_;
  }

  void
  box::set_x(double a_x)
  {
    DT_THROW_IF (a_x < 0.0,std::logic_error,
                 "Invalid '" << a_x << "' X value !");
    _x_ = a_x;
    return;
  }

  double
  box::get_half_x() const
  {
    return _x_ * 0.5;
  }

  void
  box::set_half_x(double a_half_x)
  {
    set_x(a_half_x + a_half_x);
    return;
  }

  double
  box::get_y() const
  {
    return _y_;
  }

  double
  box::get_half_y() const
  {
    return _y_ * 0.5;
  }

  void
  box::set_half_y(double a_half_y)
  {
    set_y(a_half_y + a_half_y);
    return;
  }

  void
  box::set_y( double a_y )
  {
    DT_THROW_IF (a_y < 0.0,std::logic_error,
                 "Invalid '" << a_y << "' Y value !");
    _y_ = a_y;
    return;
  }

  double
  box::get_z() const
  {
    return _z_;
  }

  void
  box::set_z(double a_z)
  {
    DT_THROW_IF (a_z < 0.0,std::logic_error,
                 "Invalid '" << a_z << "' Z value !");
    _z_ = a_z;
    return;
  }

  double
  box::get_half_z() const
  {
    return _z_ * 0.5;
  }

  void
  box::set_half_z(double a_half_z)
  {
    set_z(a_half_z + a_half_z);
    return;
  }

  void
  box::set(double a_x, double a_y, double a_z)
  {
    set_x(a_x);
    set_y(a_y);
    set_z(a_z);
    return;
  }

  void
  box::set_half(double a_half_x, double a_half_y, double a_half_z)
  {
    set_half_x(a_half_x);
    set_half_y(a_half_y);
    set_half_z(a_half_z);
    return;
  }

  box::box() : i_shape_3d(DEFAULT_SKIN)
  {
    _x_ = -1.0;
    _y_ = -1.0;
    _z_ = -1.0;
    return;
  }

  box::box(double a_x, double a_y,double a_z ) : i_shape_3d(DEFAULT_SKIN)
  {
    set_x(a_x);
    set_y(a_y);
    set_z(a_z);
    return;
  }

  box::~box()
  {
  }

  std::string
  box::get_shape_name() const
  {
    return box_label();
  }

  double
  box::get_surface(uint32_t a_mask) const
  {
    double s = 0.0;
    uint32_t mask = a_mask;
    //std::cerr << "DEVEL *** box::get_surface : mask  = " <<(uint32_t)(mask) << std::endl;
    //std::cerr << "DEVEL *** box::get_surface :   ALL_SURFACES  = " <<(uint32_t)(ALL_SURFACES) << std::endl;
    if (a_mask ==(uint32_t) ALL_SURFACES) mask = FACE_ALL;
    //std::cerr << "DEVEL *** box::get_surface :   effective mask  = " <<(uint32_t)(mask) << std::endl;

    if (mask & FACE_BACK) {
      s += _y_ * _z_;
    }
    if (mask & FACE_FRONT) {
      s += _y_ * _z_;
    }
    if (mask & FACE_LEFT) {
      s += _x_ * _z_;
    }
    if (mask & FACE_RIGHT) {
      s += _x_ * _z_;
    }
    if (mask & FACE_BOTTOM) {
      s += _x_ * _y_;
    }
    if (mask & FACE_TOP) {
      s += _x_ * _y_;
    }
    return s;
  }

  double
  box::get_parameter(const std::string & a_flag) const
  {
    if (a_flag == "x") return get_x();
    if (a_flag == "y") return get_y();
    if (a_flag == "z") return get_z();
    if (a_flag == "volume")         return get_volume();
    if (a_flag == "surface.top")    return get_surface(FACE_TOP);
    if (a_flag == "surface.bottom") return get_surface(FACE_BOTTOM);
    if (a_flag == "surface.back")   return get_surface(FACE_BACK);
    if (a_flag == "surface.front")  return get_surface(FACE_FRONT);
    if (a_flag == "surface.left")   return get_surface(FACE_LEFT);
    if (a_flag == "surface.right")  return get_surface(FACE_RIGHT);
    if (a_flag == "surface")        return get_surface(FACE_ALL);

    DT_THROW_IF (true, std::logic_error, "Unknown flag '" << a_flag << "' !");
  }

  double
  box::get_volume(uint32_t /*flags*/) const
  {
    return _x_ * _y_ * _z_;
  }

  bool
  box::is_valid() const
  {
    return(_x_ > 0.0 && _y_ > 0.0 && _z_ > 0.0);
  }

  void box::initialize(const datatools::properties & config_)
  {
    reset();
    double lunit = CLHEP::mm;
    if (config_.has_key("length_unit")) {
      const std::string lunit_str = config_.fetch_string("length_unit");
      lunit = datatools::units::get_length_unit_from(lunit_str);
    }

    DT_THROW_IF (! config_.has_key("x"), std::logic_error, "Missing box 'x' property !");
    double x = config_.fetch_real("x");
    if (! config_.has_explicit_unit("x")) {
      x *= lunit;
    }
    DT_THROW_IF (! config_.has_key("y"), std::logic_error, "Missing box 'y' property !");
    double y = config_.fetch_real("y");
    if (! config_.has_explicit_unit("y")) {
      y *= lunit;
    }
    DT_THROW_IF (! config_.has_key("z"), std::logic_error, "Missing box 'z' property !");
    double z = config_.fetch_real("z");
    if (! config_.has_explicit_unit("z")) {
      z *= lunit;
    }

    set(x, y, z);

    return;
  }

  void
  box::reset()
  {
    _x_ = -1.0;
    _y_ = -1.0;
    _z_ = -1.0;
    i_object_3d::reset();
  }

  bool
  box::is_inside(const vector_3d & a_position, double a_skin) const
  {
    double skin = get_skin();
    if (a_skin > USING_PROPER_SKIN) skin = a_skin;

    if (std::abs(a_position.x()) > 0.5 * _x_ + 0.5 * skin) return false;
    if (std::abs(a_position.y()) > 0.5 * _y_ + 0.5 * skin) return false;
    if (std::abs(a_position.z()) > 0.5 * _z_ + 0.5 * skin) return false;
    return true;
  }

  vector_3d
  box::get_normal_on_surface(const vector_3d & a_position) const
  {
    vector_3d normal;
    geomtools::invalidate(normal);
    if (is_on_surface(a_position, FACE_BACK)) normal.set(-1.0, 0.0, 0.0);
    else if (is_on_surface(a_position, FACE_FRONT)) normal.set(+1.0, 0.0, 0.0);
    else if (is_on_surface(a_position, FACE_LEFT)) normal.set(0.0, -1.0, 0.0);
    else if (is_on_surface(a_position, FACE_RIGHT)) normal.set(0.0, +1.0, 0.0);
    else if (is_on_surface(a_position, FACE_BOTTOM)) normal.set(0.0, 0.0, -1.0);
    else if (is_on_surface(a_position, FACE_TOP)) normal.set(0.0, 0.0, +1.0);
    return(normal);
  }

  bool
  box::is_on_surface(const vector_3d & a_position ,
                     int    a_mask ,
                     double a_skin) const
  {
    //bool debug = false;
    double skin = get_skin();
    if (a_skin > USING_PROPER_SKIN) skin = 2 * a_skin;

    int mask = a_mask;
    if (a_mask ==(int) ALL_SURFACES) mask = FACE_ALL;

    double hskin = 0.5 * skin;
    if (mask & FACE_BACK) {
      if ((std::abs(a_position.x() + 0.5 * _x_) < hskin)
          &&(std::abs(a_position.y()) < 0.5 * _y_)
          &&(std::abs(a_position.z()) < 0.5 * _z_)) return true;
    }
    if (mask & FACE_FRONT) {
      if ((std::abs(a_position.x() - 0.5 * _x_) < hskin)
          &&(std::abs(a_position.y()) < 0.5 * _y_)
          &&(std::abs(a_position.z()) < 0.5 * _z_)) return true;
    }
    if (mask & FACE_LEFT) {
      if ((std::abs(a_position.y() + 0.5 * _y_) < hskin)
          &&(std::abs(a_position.x()) < 0.5 * _x_)
          &&(std::abs(a_position.z()) < 0.5 * _z_)) return true;
    }
    if (mask & FACE_RIGHT) {
      // if (debug) {
      //     std::clog << "DEVEL: box::is_on_surface: FACE_RIGHT" << std::endl;
      //     std::clog << "DEVEL: box::is_on_surface: hskin=" << hskin << std::endl;
      //     std::clog << "DEVEL: box::is_on_surface: point=" << a_position << std::endl;
      //     std::clog << "DEVEL: box::is_on_surface: dim x=" << 0.5 * _x_ << std::endl;
      //     std::clog << "DEVEL: box::is_on_surface: dim y=" << 0.5 * _y_ << std::endl;
      //     std::clog << "DEVEL: box::is_on_surface: dim z=" << 0.5 * _z_ << std::endl;
      //   }
      if ((std::abs(a_position.y() - 0.5 * _y_) < hskin)
          &&(std::abs(a_position.x()) < 0.5 * _x_)
          &&(std::abs(a_position.z()) < 0.5 * _z_)) return true;
    }
    if (mask & FACE_BOTTOM) {
      if ((std::abs(a_position.z() + 0.5 * _z_) < hskin)
          &&(std::abs(a_position.x()) < 0.5 * _x_)
          &&(std::abs(a_position.y()) < 0.5 * _y_)) return true;
    }
    if (mask & FACE_TOP) {
      if ((std::abs(a_position.z() - 0.5 * _z_) < hskin)
          &&(std::abs(a_position.x()) < 0.5 * _x_)
          &&(std::abs(a_position.y()) < 0.5 * _y_)) return true;
    }
    return false;
  }

  bool
  box::find_intercept(const vector_3d & a_from,
                      const vector_3d & a_direction,
                      intercept_t & a_intercept,
                      double a_skin) const
  {
    bool debug = false;
    const unsigned int NFACES = 6;
    double t[NFACES];
    t[BACK]   = -(get_half_x() + a_from[vector_3d::X])
      / a_direction[vector_3d::X];
    t[FRONT]  = +(get_half_x() - a_from[vector_3d::X])
      / a_direction[vector_3d::X];
    t[LEFT]   = -(get_half_y() + a_from[vector_3d::Y])
      / a_direction[vector_3d::Y];
    t[RIGHT]  = +(get_half_y() - a_from[vector_3d::Y])
      / a_direction[vector_3d::Y];
    t[BOTTOM] = -(get_half_z() + a_from[vector_3d::Z])
      / a_direction[vector_3d::Z];
    t[TOP]    = +(get_half_z() - a_from[vector_3d::Z])
      / a_direction[vector_3d::Z];

    double t_min = -1.0;
    int face_min = 0;
    for (int i = 0; i <(int) NFACES; i++) {
      double ti = t[i];
      if (debug) {
        std::clog << "DEVEL: box::find_intercept: t[" << i << "]= "
                  << ti << " t_min=" << t_min
                  << " face_min=" << face_min
                  << std::endl;
      }
      if (std::isnormal(ti) &&(ti > 0.0)) {
        int face_bit =(0x1 << i); // face mask
        vector_3d intercept = a_from + a_direction * ti;
        if (is_on_surface(intercept, face_bit, a_skin)) {
          if ((t_min < 0.0) ||(ti < t_min)) {
            t_min = ti;
            face_min = face_bit;
          }
        }
      }
    }
    a_intercept.reset();
    if (face_min > 0) {
      a_intercept.set(0, face_min, a_from + a_direction * t_min);
    }
    return a_intercept.is_ok();
  }

  std::ostream &
  operator<<(std::ostream & a_out, const box & a_box)
  {
    a_out << '{' << box::box_label() << ' '
          << a_box._x_ << ' '
          << a_box._y_ << ' '
          << a_box._z_ << '}';
    return a_out;
  }

  std::istream &
  operator>>(std::istream & a_in, box & a_box)
  {
    a_box.reset();
    char c = 0;
    a_in.get(c);
    if (c != '{') {
      a_in.clear(std::ios_base::failbit);
      return a_in;
    }
    std::string name;
    a_in >> name;
    if (name != box::box_label()) {
      a_in.clear(std::ios_base::failbit);
      return a_in;
    }
    double x, y, z;
    a_in >> x >> y >> z;
    if (! a_in) {
      a_in.clear(std::ios_base::failbit);
      return a_in;
    }
    c = 0;
    a_in.get(c);
    if (c != '}') {
      a_in.clear(std::ios_base::failbit);
      return a_in;
    }
    try {
      a_box.set(x,y,z);
    } catch (...) {
      a_box.reset();
      a_in.clear(std::ios_base::failbit);
    }
    return a_in;
  }


  void box::tree_dump(std::ostream & a_out,
                      const std::string & a_title,
                      const std::string & a_indent,
                      bool a_inherit) const
  {
    std::string indent;
    if (! a_indent.empty()) indent = a_indent;
    i_object_3d::tree_dump(a_out, a_title, a_indent, true);

    a_out << indent << datatools::i_tree_dumpable::tag
          << "X : " << get_x() / CLHEP::mm << " mm" << std::endl;
    a_out << indent << datatools::i_tree_dumpable::tag
          << "Y : " << get_y() / CLHEP::mm << " mm" << std::endl;
    a_out << indent << datatools::i_tree_dumpable::inherit_tag(a_inherit)
          << "Z : " << get_z() / CLHEP::mm << " mm" << std::endl;
    return;
  }

  void box::generate_wires(std::list<polyline_3d> & lpl_,
                           const placement & p_,
                           uint32_t /*options_*/) const
  {
    double dim[3];
    dim[0] = 0.5*get_x();
    dim[1] = 0.5*get_y();
    dim[2] = 0.5*get_z();

    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 2; j++) {
        vector_3d vertex[4];
        if (i ==0) {
          vertex[0].set((1 - 2 *j) * dim[0],   dim[1],  dim[2]);
          vertex[1].set((1 - 2 *j) * dim[0],   dim[1], -dim[2]);
          vertex[2].set((1 - 2 *j) * dim[0],  -dim[1], -dim[2]);
          vertex[3].set((1 - 2 *j) * dim[0],  -dim[1],  dim[2]);
        }
        if (i == 1) {
          vertex[0].set( dim[0],(1 - 2 *j) * dim[1],   dim[2]);
          vertex[1].set(-dim[0],(1 - 2 *j) * dim[1],   dim[2]);
          vertex[2].set(-dim[0],(1 - 2 *j) * dim[1],  -dim[2]);
          vertex[3].set( dim[0],(1 - 2 *j) * dim[1],  -dim[2]);
        }
        if (i == 2) {
          vertex[0].set( dim[0],  dim[1],(1 - 2 *j) * dim[2]);
          vertex[1].set(-dim[0],  dim[1],(1 - 2 *j) * dim[2]);
          vertex[2].set(-dim[0], -dim[1],(1 - 2 *j) * dim[2]);
          vertex[3].set( dim[0], -dim[1],(1 - 2 *j) * dim[2]);
        }
        {
          polyline_3d dummy;
          lpl_.push_back(dummy);
        }
        polyline_3d & pl = lpl_.back();
        pl.set_closed(true);
        for (int i = 0; i < 4; i++) {
          vector_3d v;
          p_.child_to_mother(vertex[i], v);
          pl.add(v);
        }
      }
    }
    return;
  }

  // static
  void box::init_ocd(datatools::object_configuration_description & ocd_)
  {
    i_shape_3d::init_ocd(ocd_);

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("length_unit")
        .set_from("geomtools::box")
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
      cpd.set_name_pattern("x")
        .set_from("geomtools::box")
        .set_terse_description("The dimension of the box along the X axis")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(true)
        .add_example("Set the X dimension::          \n"
                     "                               \n"
                     "  x : real as length = 5.0 cm  \n"
                     "                               \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("y")
        .set_from("geomtools::box")
        .set_terse_description("The dimension of the box along the X axis")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(true)
        .add_example("Set the Y dimension::          \n"
                     "                               \n"
                     "  y : real as length = 10.0 mm \n"
                     "                               \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("z")
        .set_from("geomtools::box")
        .set_terse_description("The dimension of the box along the Z axis")
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
 *  @arg geomtools::box the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized(passed by mutable reference).
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(geomtools::box, ocd_)
{
  // The class name :
  ocd_.set_class_name("geomtools::box");

  // The class terse description :
  ocd_.set_class_description("A class representing a 3D box solid shape");

  // The library the class belongs to :
  ocd_.set_class_library("geomtools");

  // The class detailed documentation :
  ocd_.set_class_documentation("The box solid 3D-shape is defined by its three dimensions\n"
                               "along the X, Y and Z axis.                               \n"
                               );

  // Populate the specific OCD :
  geomtools::box::init_ocd(ocd_);

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the      \n"
                               "``datatools::properties`` ASCII format::         \n"
                               "                                                 \n"
                               "  tolerance.length : real = 1.0e-6 um            \n"
                               "  length_unit : string = \"mm\"                  \n"
                               "  x : real as length = 25.0 cm                   \n"
                               "  y : real           = 12.0                      \n"
                               "  z : real           =  2.5                      \n"
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

// Registration macro for class 'geomtools::box' :
DOCD_CLASS_SYSTEM_REGISTRATION(geomtools::box, "geomtools::box")

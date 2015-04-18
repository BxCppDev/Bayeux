/// utils.cc

// Ourselves:
#include <geomtools/utils.h>

// Standard library:
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cmath>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/units.h>

namespace geomtools {

  using namespace std;

  std::string shape_domain_flag_label(shape_domain_flags_type flag_)
  {
    if (flag_ == SHAPE_DOMAIN_INSIDE) return std::string("inside");
    if (flag_ == SHAPE_DOMAIN_OUTSIDE) return std::string("outside");
    if (flag_ == SHAPE_DOMAIN_ON_SURFACE) return std::string("on_surface");
    if (flag_ == SHAPE_DOMAIN_INSIDE_DAUGHTER) return std::string("inside_daughter");
    if (flag_ == SHAPE_DOMAIN_ON_DAUGHTER_SURFACE) return std::string("on_daughter_surface");
    return std::string("");
  }

  bool position_is_in(double position_,
                      double start_position_, double delta_position_,
                      double tolerance_,
                      bool bounds_excluded_)
  {
    DT_THROW_IF(delta_position_ < 0, std::range_error,
                "Invalid negative delta position!");
    bool in = false;
    double stop_position = start_position_ + delta_position_;
    double p = position_;
    if (bounds_excluded_) {
      if (p > (start_position_ - tolerance_)
          && (p < stop_position + tolerance_)) {
        in = true;
      }
    } else {
      if (p >= (start_position_ - tolerance_)
          && (p <= stop_position + tolerance_)) {
        in = true;
      }
    }
    return in;
  }

  bool angle_is_in(double angle_,
                   double start_angle_, double delta_angle_,
                   double angular_tolerance_,
                   bool bounds_excluded_)
  {
    static const double two_pi = 2. * M_PI;
    bool in = false;
    if (delta_angle_ >= two_pi) {
      in = true;
    } else {
      double stop_angle = start_angle_ + delta_angle_;
      double a = angle_;
      if (bounds_excluded_) {
        if (a > (start_angle_ - angular_tolerance_) && (a < stop_angle + angular_tolerance_)) {
          in = true;
        } else {
          double n = ceil((start_angle_ - a) / two_pi);
          a = angle_ + n * two_pi;
          if (a > (start_angle_ - angular_tolerance_) && (a < stop_angle + angular_tolerance_)) {
            in = true;
          }
        }
      } else {
        if (a >= (start_angle_ - angular_tolerance_) && (a <= stop_angle + angular_tolerance_)) {
          in = true;
        } else {
          double n = ceil((start_angle_ - a) / two_pi);
          a = angle_ + n * two_pi;
          if (a >= (start_angle_ - angular_tolerance_) && (a <= stop_angle + angular_tolerance_)) {
            in = true;
          }
        }
      }
    }
    return in;
  }

  const double constants::DEFAULT_TOLERANCE      = constants::get_default_tolerance ();
  const double constants::DEFAULT_ANGULAR_TOLERANCE = constants::get_default_angular_tolerance ();
  const double constants::ZERO_TOLERANCE         = constants::get_zero_tolerance ();
  const double constants::USING_PROPER_TOLERANCE = constants::get_proper_tolerance ();

  // static
  double constants::get_default_angular_tolerance ()
  {
    return GEOMTOOLS_DEFAULT_ANGULAR_TOLERANCE;
  }

  // static
  double constants::get_default_tolerance ()
  {
    return GEOMTOOLS_DEFAULT_TOLERANCE;
  }

  // static
  double constants::get_proper_tolerance ()
  {
    return GEOMTOOLS_PROPER_TOLERANCE;
  }

  // static
  double constants::get_zero_tolerance ()
  {
    return GEOMTOOLS_ZERO_TOLERANCE;
  }

  const string & io::vector_2d_serial_tag()
  {
    static std::string tag;
    if (tag.empty()) {
      tag = "__geomtools::vector_2d__";
    }
    return tag;
  }

  const string & io::vector_3d_serial_tag()
  {
    static std::string tag;
    if (tag.empty()) {
      tag = "__geomtools::vector_3d__";
    }
    return tag;
  }

  const string & io::rotation_3d_serial_tag()
  {
    static std::string tag;
    if (tag.empty()) {
      tag = "__geomtools::rotation_3d__";
    }
    return tag;
  }

  const string & io::position_suffix()
  {
    static std::string tag;
    if (tag.empty()) {
      tag = ".pos";
    }
    return tag;
  }

  const string & io::rotation_suffix()
  {
    static std::string tag;
    if (tag.empty()) {
      tag = ".rot";
    }
    return tag;
  }

  string to_xy (const vector_2d & p_)
  {
    ostringstream out;
    out << p_.x () << ' ' <<  p_.y ();
    return out.str ();
  }

  string vector_2d_to_xy (const vector_2d & p_)
  {
    ostringstream out;
    out << p_.x () << ' ' <<  p_.y ();
    return out.str ();
  }

  void print_xy (ostream & out_,
                 const vector_2d & p_,
                 bool endl_)
  {
    out_ << p_.x () << ' ' <<  p_.y ();
    if (endl_) out_ << endl;
    return;
  }

  void print_xy_stdout (const vector_2d & p_)
  {
    print_xy (cout, p_, true);
    return;
  }

  void print_xy_stderr (const vector_2d & p_)
  {
    print_xy (cerr, p_, true);
    return;
  }

  void invalidate (vector_2d & vec_)
  {
    double qnan = numeric_limits<double>::quiet_NaN();
    vec_.set (qnan, qnan);
    return;
  }

  void set_xy (vector_2d & vec_, double x_, double y_)
  {
    vec_.set (x_, y_);
    return;
  }

  void set_rphi (vector_2d & vec_, double r_, double phi_)
  {
    vec_.setPolar (r_, phi_);
    return;
  }

  void invalidate_vector_2d (vector_2d & vec_)
  {
    invalidate (vec_);
    return;
  }

  bool is_valid (const vector_2d & vec_)
  {
    return ((vec_.x () == vec_.x ()) && (vec_.y () == vec_.y ()));
  }

  bool is_valid_vector_2d (const vector_2d & vec_)
  {
    return is_valid (vec_);
  }

  void vector_2d_to_vector_3d (const vector_2d & v2d_, vector_3d & v3d_)
  {
    v3d_.set (v2d_.x(), v2d_.y(), 0.0);
    return;
  }

  void vector_3d_to_vector_2d (const vector_3d & v3d_, vector_2d & v2d_)
  {
    v2d_.set (v3d_.x(), v3d_.y());
    return;
  }

  void make_phi_theta (vector_3d & vec_, double phi_, double theta_)
  {
    double sin_theta = std::sin(theta_);
    double x = sin_theta * std::cos(phi_);
    double y = sin_theta * std::sin(phi_);
    double z = std::cos(theta_);
    vec_.set(x, y, z);
    return;
  }

  std::string to_xyz (const vector_3d & p_)
  {
    std::ostringstream out;
    out << p_.x () << ' ' <<  p_.y () << ' ' << p_.z ();
    return out.str ();
  }

  std::string vector_3d_to_xyz (const vector_3d & p_)
  {
    std::ostringstream out;
    out << p_.x () << ' ' <<  p_.y () << ' ' << p_.z ();
    return out.str ();
  }

  void print_xyz (std::ostream & out_,
                  const vector_3d & p_,
                  bool endl_)
  {
    out_ << p_.x () << ' ' <<  p_.y () << ' ' << p_.z ();
    if (endl_) out_ << endl;
    return;
  }

  void print (std::ostream & out_, const vector_3d & p_)
  {
    out_ << p_;
    return;
  }

  void print_xyz_stdout (const vector_3d & p_)
  {
    print_xyz (cout, p_);
    return;
  }

  void print_xyz_stderr (const vector_3d & p_)
  {
    print_xyz (cerr, p_);
    return;
  }

  void create (vector_3d & v_,
               double x_,
               double y_,
               double z_)
  {
    create_xyz (v_, x_, y_, z_);
    return;
  }

  void create_xyz (vector_3d & v_,
                   double x_,
                   double y_,
                   double z_)
  {
    v_.setX (x_);
    v_.setY (y_);
    v_.setZ (z_);
    return;
  }

  void create_polar (vector_3d & v_,
                     double r_,
                     double theta_,
                     double z_)
  {
    create_xyz (v_,
                r_ * cos (theta_),
                r_ * sin (theta_),
                z_);
    return;
  }

  void create_spherical (vector_3d & v_,
                         double r_,
                         double phi_,
                         double theta_)
  {
    create_xyz (v_,
                r_ * sin (theta_) * cos (phi_),
                r_ * sin (theta_) * sin (phi_),
                r_ * cos (theta_));
    return;
  }

  void set (vector_3d & vec_, double x_, double y_, double z_)
  {
    vec_.set (x_, y_, z_);
    return;
  }

  void set_r_theta_phi (vector_3d & vec_,
                        double r_, double theta_, double phi_)
  {
    vec_.setRThetaPhi (r_, theta_, phi_);
    return;
  }

  void set_rho_phi_z (vector_3d & vec_, double rho_, double phi_, double z_)
  {
    vec_.setRhoPhiZ (rho_, phi_, z_);
    return;
  }

  void invalidate (vector_3d & vec_)
  {
    double qnan = numeric_limits<double>::quiet_NaN();
    vec_.set (qnan, qnan, qnan);
    return;
  }

  void invalidate_vector_3d (vector_3d & vec_)
  {
    invalidate (vec_);
    return;
  }

  bool is_valid (const vector_3d & vec_)
  {
    return (vec_.x () == vec_.x ()) && (vec_.y () == vec_.y () ) && (vec_.z () == vec_.z ());
  }

  bool is_valid_vector_3d (const vector_3d & vec_)
  {
    return is_valid (vec_);
  }

  bool are_near (const vector_3d & vec1_,
                 const vector_3d & vec2_,
                 double tolerance_)
  {
    return vec1_.isNear (vec2_, tolerance_); // from CLHEP
  }

  bool are_near_vector_3d (const vector_3d & vec1_,
                           const vector_3d & vec2_,
                           double tolerance_)
  {
    return are_near (vec1_, vec2_, tolerance_);
  }

  bool parse(const std::string & token_, vector_3d & position_)
  {
    std::istringstream iss(token_);
    bool ok = parse(iss, position_, true);
    return ok;
  }

  bool parse(std::istream & in_, vector_3d & position_, bool nothing_more_)
  {
    invalidate_vector_3d(position_);
    double x(0.0), y(0.0), z(0.0);
    double length_unit = CLHEP::mm;
    in_ >> std::ws;
    in_ >> x >> y >> z >> std::ws;
    if (!in_) {
      return false;
    }
    // Extract length unit:
    if (! in_.eof ()) {
      char open = in_.peek();
      if (open == '(') {
        std::string length_unit_str;
        in_.get();
        std::getline(in_, length_unit_str, ')');
        if (! in_) {
          return false;
        }
        length_unit = datatools::units::get_length_unit_from(length_unit_str);
      }
      in_ >> ws;
    }
    x *= length_unit;
    y *= length_unit;
    z *= length_unit;
    if (nothing_more_) {
      // Check if the stream is empty as we want no more token
      std::string token;
      in_ >> token;
      if (token.length() > 0) {
        return false;
      }
    }
    position_.set(x, y, z);
    return true;
  }


  void print_xy (ostream & out_,
                 const basic_polyline_2d & p_,
                 bool endl_)
  {
    for (basic_polyline_2d::const_iterator i = p_.begin ();
         i != p_.end ();
         i++) {
      print_xy (out_, *i, true);
    }
    if (endl_) out_ << endl;
    return;
  }

  string to_xy (const basic_polyline_2d & p_)
  {
    ostringstream out;
    print_xy (out, p_, true);
    return out.str ();
  }

  string basic_polyline_2d_to_xy (const basic_polyline_2d & p_)
  {
    ostringstream out;
    print_xy (out, p_, true);
    return out.str ();
  }

  void print_xy_stdout (const basic_polyline_2d & p_)
  {
    print_xy (cout, p_, true);
    return;
  }

  void print_xy_stderr (const basic_polyline_2d & p_)
  {
    print_xy (cerr, p_, true);
    return;
  }

  void print_xyz (ostream & out_,
                  const basic_polyline_3d & p_,
                  bool endl_)
  {
    for (basic_polyline_3d::const_iterator i = p_.begin ();
         i != p_.end ();
         i++) {
      print_xyz (out_, *i, true);
    }
    if (endl_) out_ << endl;
    return;
  }

  string to_xyz (const basic_polyline_3d & p_)
  {
    ostringstream out;
    print_xyz (out, p_, true);
    return out.str ();
  }

  string basic_polyline_3d_to_xyz (const basic_polyline_3d & p_)
  {
    ostringstream out;
    print_xyz (out, p_, true);
    return out.str ();
  }

  void print_xyz_stdout (const basic_polyline_3d & p_)
  {
    print_xyz (cout, p_, true);
    return;
  }

  void print_xyz_stderr (const basic_polyline_3d & p_)
  {
    print_xyz (cerr, p_, true);
    return;
  }

  void invalidate (rotation_3d & rot_)
  {
    rot_ = geomtools::rotation_3d ();
    double qnan = numeric_limits<double>::quiet_NaN();
    double * xx_addr = static_cast<double *>(static_cast<void *> (&rot_));
    double * last_addr = xx_addr + 9;
    for (double * p = xx_addr; p < last_addr; p++) {
      *p = 0.0;
    }
    *xx_addr = qnan;
    xx_addr += 4;
    *xx_addr = qnan;
    xx_addr += 4;
    *xx_addr = qnan;
    return;
  }

  void rectify (rotation_3d & rot_)
  {
    if (! is_valid (rot_)) return;
    double * xx_addr = static_cast<double *>(static_cast<void *> (&rot_));
    double * last_addr = xx_addr + 9;
    double epsilon = 16 * numeric_limits<double>::epsilon ();
    for (double * p = xx_addr; p < last_addr; p++) {
      if (std::abs (*p) < epsilon) {
        *p = 0.0;
      }
    }
    return;
  }

  void invalidate_rotation_3d (rotation_3d & rot_)
  {
    invalidate (rot_);
    return;
  }

  bool is_valid (const rotation_3d & rot_)
  {
    return (rot_.xx() == rot_.xx());
  }

  bool is_valid_rotation_3d (const rotation_3d & rot_)
  {
    return is_valid (rot_);
  }

  void create(rotation_3d & rot_,
              double angle0_,
              double angle1_,
              double angle2_,
              euler_angles_type et_)
  {
    rotation_3d r1, r2, r3;
    switch (et_) {
    case EULER_ANGLES_ZXZ:
      r1.rotateZ(-angle0_);
      r2.rotateX(-angle1_);
      r3.rotateZ(-angle2_);
      break;
    case EULER_ANGLES_XYX:
      r1.rotateX(-angle0_);
      r2.rotateY(-angle1_);
      r3.rotateX(-angle2_);
      break;
    case EULER_ANGLES_YZY:
      r1.rotateY(-angle0_);
      r2.rotateZ(-angle1_);
      r3.rotateY(-angle2_);
      break;
    case EULER_ANGLES_ZYZ:
      r1.rotateZ(-angle0_);
      r2.rotateY(-angle1_);
      r3.rotateZ(-angle2_);
      break;
    case EULER_ANGLES_XZX:
      r1.rotateX(-angle0_);
      r2.rotateZ(-angle1_);
      r3.rotateX(-angle2_);
      break;
    case EULER_ANGLES_YXY:
      r1.rotateY(-angle0_);
      r2.rotateX(-angle1_);
      r3.rotateY(-angle2_);
      break;
    case EULER_ANGLES_XYZ:
      r1.rotateX(-angle0_);
      r2.rotateY(-angle1_);
      r3.rotateZ(-angle2_);
      break;
    case EULER_ANGLES_YZX:
      r1.rotateY(-angle0_);
      r2.rotateZ(-angle1_);
      r3.rotateX(-angle2_);
      break;
    case EULER_ANGLES_ZXY:
      r1.rotateZ(-angle0_);
      r2.rotateX(-angle1_);
      r3.rotateY(-angle2_);
      break;
    case EULER_ANGLES_XZY:
      r1.rotateX(-angle0_);
      r2.rotateZ(-angle1_);
      r3.rotateY(-angle2_);
      break;
    case EULER_ANGLES_ZYX:
      r1.rotateZ(-angle0_);
      r2.rotateY(-angle1_);
      r3.rotateX(-angle2_);
      break;
    case EULER_ANGLES_YXZ:
      r1.rotateY(-angle0_);
      r2.rotateX(-angle1_);
      r3.rotateZ(-angle2_);
      break;
    }
    rot_ = r3 * r2 * r1;
    return;
  }

  void create_zyz (rotation_3d & rot_,
                   double phi_,
                   double theta_,
                   double delta_)
  {
    // create(rot_, phi, theta_, delta_, EULER_ANGLES_ZYZ);
    rotation_3d r1, r2, r3;
    r1.rotateZ (-phi_);
    r2.rotateY (-theta_);
    r3.rotateZ (-delta_);
    rot_ = r3 * r2 * r1;
    return;
  }

  void create (rotation_3d & rot_,
               double phi_,
               double theta_,
               double delta_)
  {
    create_zyz (rot_, phi_, theta_, delta_);
    return;
  }

  void create_zxz (rotation_3d & rot_,
                   double phi_,
                   double theta_,
                   double psi_)
  {
    // create(rot_, phi, theta_, psi_, EULER_ANGLES_ZXZ);
    rotation_3d r1, r2, r3;
    r1.rotateZ (-phi_);
    r2.rotateX (-theta_);
    r3.rotateZ (-psi_);
    rot_ = r3 * r2 * r1;
    return;
  }

  void create_xyz (rotation_3d & rot_,
                   double phi_,
                   double theta_,
                   double psi_)
  {
    // create(rot_, phi, theta_, psi_, EULER_ANGLES_XYZ);
    rotation_3d r1, r2, r3;
    r1.rotateX (-phi_);
    r2.rotateY (-theta_);
    r3.rotateZ (-psi_);
    rot_ = r3 * r2 * r1;
    return;
  }

  void create_rotation_3d (rotation_3d & rot_,
                           double phi_,
                           double theta_,
                           double delta_)
  {
    create (rot_, phi_, theta_, delta_);
    return;
  }

  void create_rotation_from_zyz_euler_angles (rotation_3d & rot_,
                                              double phi_,
                                              double theta_,
                                              double delta_)
  {
    create (rot_, phi_, theta_, delta_);
    return;
  }

  void create_rotation (rotation_3d & rot_,
                        double phi_,
                        double theta_,
                        double delta_)
  {
    create (rot_, phi_, theta_, delta_);
    return;
  }

  void reset (rotation_3d & rot_)
  {
    rot_ = rotation_3d ();
    return;
  }

  double get_special_rotation_angle (int flag_angle_)
  {
    switch (flag_angle_) {
    case ROTATION_ANGLE_0: return 0.0 * CLHEP::degree;
    case ROTATION_ANGLE_90: return 90.0 * CLHEP::degree;
    case ROTATION_ANGLE_180: return 180.0 * CLHEP::degree;
    case ROTATION_ANGLE_270: return 270.0 * CLHEP::degree;
    }
    return numeric_limits<double>::quiet_NaN();
  }

  int get_special_rotation_angle_from_label (const string & s_)
  {
    if (s_ == "0") return ROTATION_ANGLE_0;
    if (s_ == "90") return ROTATION_ANGLE_90;
    if (s_ == "180") return ROTATION_ANGLE_180;
    if (s_ == "270") return ROTATION_ANGLE_270;
    return ROTATION_ANGLE_INVALID;
  }

  bool check_special_rotation_angle (int flag_angle_)
  {
    return (flag_angle_ >= ROTATION_ANGLE_0) && (flag_angle_ <= ROTATION_ANGLE_270);
  }

  string get_special_rotation_angle_label (int flag_angle_)
  {
    switch (flag_angle_) {
    case ROTATION_ANGLE_0: return "0";
    case ROTATION_ANGLE_90: return "90";
    case ROTATION_ANGLE_180: return "180";
    case ROTATION_ANGLE_270: return "270";
    }
    return "";
  }

  bool check_rotation_axis (int axis_)
  {
    return (axis_ >= ROTATION_AXIS_X) && (axis_ <= ROTATION_AXIS_Z);
  }

  int get_rotation_axis_from_label (const string & s_)
  {
    if (s_ == "x") return ROTATION_AXIS_X;
    if (s_ == "y") return ROTATION_AXIS_Y;
    if (s_ == "z") return ROTATION_AXIS_Z;
    return ROTATION_AXIS_INVALID;
  }

  string get_rotation_label (int axis_)
  {
    switch (axis_) {
    case ROTATION_AXIS_X: return "x";
    case ROTATION_AXIS_Y: return "y";
    case ROTATION_AXIS_Z: return "z";
    }
    return "";
  }

  void create_rotation_from_axis (rotation_3d & rot_,
                                  int axis_,
                                  double angle_)
  {
    /*
      clog << "DEVEL: create_rotation_from_axis: "
      << "axis=" << axis_ << " angle=" << angle_ / CLHEP::degree << " °" << endl;
    */
    DT_THROW_IF (! check_rotation_axis (axis_), logic_error, "Invalid rotation axis !");
    rotation_3d r;
    if (axis_ == ROTATION_AXIS_X) {
      r.rotateX (-angle_);
    }
    if (axis_ == ROTATION_AXIS_Y) {
      r.rotateY (-angle_);
    }
    if (axis_ == ROTATION_AXIS_Z) {
      r.rotateZ (-angle_);
    }
    rot_ = r;
    return;
  }

  void create_rotation (rotation_3d & rot_,
                        int axis_,
                        double angle_)
  {
    create_rotation_from_axis (rot_, axis_, angle_);
  }

  void create_rotation (rotation_3d & rot_,
                        int axis_,
                        int special_angle_)
  {
    DT_THROW_IF (! check_special_rotation_angle (special_angle_),
                 logic_error, "Invalid special angle !");
    double angle = get_special_rotation_angle (special_angle_);
    create_rotation (rot_, axis_, angle);
    return;
  }

  void create_rotation_from (rotation_3d & /*rot_*/,
                             const string & /*s_*/)
  {
    DT_THROW_IF(true, runtime_error, "Not implemented yet !");
    /*
      istringstream iss (s_);
      string token;
      getline (iss, token, '(');
      if (! iss)
      {
      invalidate_rotation_3d (rot_);
      return;
      }
      {
      if (token != "rotation")
      {
      }
      }
    */
  }

  void extract_xyz_euler_angle_from_rotation (const rotation_3d & rot_,
                                              double & a_,
                                              double & b_,
                                              double & c_)
  {
    // http://www.geometrictools.com/Documentation/EulerAngles.pdf
    rotation_3d rot = rot_.inverse ();
    const double r00 = rot.xx ();
    const double r01 = rot.xy ();
    const double r02 = rot.xz ();
    const double r10 = rot.yx ();
    const double r11 = rot.yy ();
    const double r12 = rot.yz ();
    //const double r20 = rot.zx ();
    //const double r21 = rot.zy ();
    const double r22 = rot.zz ();
    double thetaX  = numeric_limits<double>::quiet_NaN ();
    double thetaY  = numeric_limits<double>::quiet_NaN ();
    double thetaZ  = numeric_limits<double>::quiet_NaN ();
    double epsilon = 16 * numeric_limits<double>::epsilon ();

    if (r02 < (+1 - epsilon))
      {
        if (r02 > (-1 + epsilon))
          {
            thetaY = asin (r02);
            thetaX = atan2 (-r12, r22);
            thetaZ = atan2 (-r01, r00);
          }
        else // r02 = -1
          {
            // Not a unique solution: thetaZ - thetaX = atan2(r10,r11)
            thetaY = -0.5 * M_PI;
            thetaX = -atan2 (r10, r11);
            thetaZ = 0;
          }
      }
    else // r02 = +1
      {
        // Not a unique solution: thetaZ + thetaX = atan2(r10,r11)
        thetaY = +0.5 * M_PI;
        thetaX = atan2 (r10, r11);
        thetaZ = 0;
      }
    thetaX *= CLHEP::radian;
    thetaY *= CLHEP::radian;
    thetaZ *= CLHEP::radian;
    a_ = thetaX;
    b_ = thetaY;
    c_ = thetaZ;
    if (std::abs (a_) < epsilon) a_ = 0.0;
    if (std::abs (b_) < epsilon) b_ = 0.0;
    if (std::abs (c_) < epsilon) c_ = 0.0;
    if (a_ < -M_PI * CLHEP::radian) a_ += 360. * CLHEP::degree;
    if (b_ < -M_PI * CLHEP::radian) b_ += 360. * CLHEP::degree;
    if (c_ < -M_PI * CLHEP::radian) c_ += 360. * CLHEP::degree;
    return;
  }

  /*
    void extract_xyz_euler_angle_from_rotation (const rotation_3d & rot_,
    double & a_,
    double & b_,
    double & c_)
    {

    // [ xx  xy  xz ]
    // [ yx  yy  yz ]
    // [ zx  zy  zz ]
    double xx = rot_.xx ();
    double yx = rot_.yx ();
    double yy = rot_.yy ();
    double yz = rot_.yz ();
    double zx = rot_.zx ();
    double zy = rot_.zy ();
    double zz = rot_.zz ();
    // from a sample by Evgueni Tcherniaev:
    //  see also: http://en.wikipedia.org/wiki/Euler_angles -> Table of matrices
    double cosb = sqrt (xx * xx + yx * yx);
    if (cosb > 16 * numeric_limits<double>::epsilon ())
    {
    a_ = -atan2 ( zy, zz) * CLHEP::radian;
    b_ = -atan2 (-zx, cosb) * CLHEP::radian;
    c_ = -atan2 ( yx, xx) * CLHEP::radian;
    }
    else
    {
    a_ = -atan2 (-yz, yy) * CLHEP::radian;
    b_ = -atan2 (-zx, cosb) * CLHEP::radian;
    c_ = 0. * CLHEP::radian;
    }
    if (a_ < 0.0) a_ += 360. * CLHEP::degree;
    if (b_ < 0.0) b_ += 360. * CLHEP::degree;
    if (c_ < 0.0) c_ += 360. * CLHEP::degree;
    if (std::abs (a_) < 16 * numeric_limits<double>::epsilon()) a_ = 0.0;
    if (std::abs (b_) < 16 * numeric_limits<double>::epsilon()) b_ = 0.0;
    if (std::abs (c_) < 16 * numeric_limits<double>::epsilon()) c_ = 0.0;
    return;
    }
  */

  void extract_zyz_euler_angle_from_rotation (const rotation_3d & rot_,
                                              double & a_,
                                              double & b_,
                                              double & c_)
  {
    /* Rotation matrix elements :
     *  [ xx  xy  xz ]
     *  [ yx  yy  yz ]
     *  [ zx  zy  zz ]
     *
     * with:
     *
     *   xx =  c1 c2 c3 - s1 s3
     *   xy =  s1 c2 c3 + c1 s3
     *   xz = -s2 c3
     *   yx = -c1 c2 s3  - s1 c3
     *   yy = -s1 c2 s3  + c1 c3
     *   yz =  s2 s3
     *   zx =  c1 s2
     *   zy =  s1 s2
     *   zz =  c2
     *
     * and :
     *
     *  c1 = cos (a_) ,  s1 = sin (a_)
     *  c2 = cos (b_) ,  s2 = sin (b_)
     *  c3 = cos (c_) ,  s3 = sin (c_)
     *
     */

    // http://www.geometrictools.com/Documentation/EulerAngles.pdf
    rotation_3d rot = rot_.inverse ();
    //const double r00 = rot.xx ();
    //const double r01 = rot.xy ();
    const double r02 = rot.xz ();
    const double r10 = rot.yx ();
    const double r11 = rot.yy ();
    const double r12 = rot.yz ();
    const double r20 = rot.zx ();
    const double r21 = rot.zy ();
    const double r22 = rot.zz ();
    double thetaZ0 = numeric_limits<double>::quiet_NaN ();
    double thetaY  = numeric_limits<double>::quiet_NaN ();
    double thetaZ1 = numeric_limits<double>::quiet_NaN ();
    double epsilon = 16 * numeric_limits<double>::epsilon ();

    if (r22 < (1 - epsilon))
      {
        if (r22 > (-1 + epsilon))
          {
            thetaY = acos (r22);
            thetaZ0 = atan2 (r12, r02);
            thetaZ1 = atan2 (r21, -r20);
          }
        else // r22 = -1
          {
            // Not a unique solution: thetaZ1 - thetaZ0 = atan2(r10,r11)
            thetaY = M_PI;
            thetaZ0 = -atan2 (r10, r11);
            thetaZ1 = 0;
          }
      }
    else // r22 = +1
      {
        // Not a unique solution: thetaZ1 + thetaZ0 = atan2(r10,r11)
        thetaY = 0;
        thetaZ0 = atan2 (r10, r11);
        thetaZ1 = 0;
      }

    thetaY  *= CLHEP::radian;
    thetaZ0 *= CLHEP::radian;
    thetaZ1 *= CLHEP::radian;
    a_ = thetaZ0;
    b_ = thetaY;
    c_ = thetaZ1;
    if (std::abs (a_) < epsilon) a_ = 0.0;
    if (std::abs (b_) < epsilon) b_ = 0.0;
    if (std::abs (c_) < epsilon) c_ = 0.0;
    if (a_ < -M_PI * CLHEP::radian) a_ += 360. * CLHEP::degree;
    if (b_ < -M_PI * CLHEP::radian) b_ += 360. * CLHEP::degree;
    if (c_ < -M_PI * CLHEP::radian) c_ += 360. * CLHEP::degree;

    return;
  }

  /*
    double xx = rot_.xx ();
    double xy = rot_.xy ();
    double xz = rot_.xz ();
    double yx = rot_.yx ();
    double yy = rot_.yy ();
    double yz = rot_.yz ();
    double zx = rot_.zx ();
    double zy = rot_.zy ();
    double zz = rot_.zz ();
    a_ = numeric_limits<double>::quiet_NaN ();
    b_ = numeric_limits<double>::quiet_NaN ();
    c_ = numeric_limits<double>::quiet_NaN ();

    double abs_s2 = sqrt (zx * zx + zy * zy);
    if (abs_s2 > epsilon)
    {
    cerr << "DEVEL: |s2| != 0" << endl;
    a_ = atan2 (zy, zx) * CLHEP::radian;
    c_ = atan2 (yz, -xz) * CLHEP::radian;
    double s1 = sin (a_);
    if (std::abs (s1) > epsilon)
    {
    b_ = atan2 (zy, s1 * zz) * CLHEP::radian;
    }
    else
    {
    double s3 = sin (c_);
    if (std::abs (s3) > epsilon)
    {
    b_ = atan2 (yz, s3 * zz) * CLHEP::radian;
    }
    else
    {
    b_ = atan2 (zx, cos(a_) * zz) * CLHEP::radian;
    }
    }
    }
    else
    {
    cerr << "DEVEL: |s2| == 0" << endl;
    double c2 = zz;
    if (c2 > 0.5)
    {
    b_ = 0.0;
    c_ = 0.0;
    a_ = atan2 (xy, xx) * CLHEP::radian;
    }
    else
    {
    b_ = 180.0 * CLHEP::degree;
    c_ = 0.0;
    a_ = atan2 (-xy, -xx) * CLHEP::radian;
    }
    }
    if (std::abs (a_) < 16 * numeric_limits<double>::epsilon ()) a_ = 0.0;
    if (std::abs (b_) < 16 * numeric_limits<double>::epsilon ()) b_ = 0.0;
    if (std::abs (c_) < 16 * numeric_limits<double>::epsilon ()) c_ = 0.0;
    if (a_ < 0.0) a_ += 360. * CLHEP::degree;
    if (b_ < 0.0) b_ += 360. * CLHEP::degree;
    if (c_ < 0.0) c_ += 360. * CLHEP::degree;
    return;
    }
  */

  /*
    void extract_zyz_euler_angle_from_rotation (const rotation_3d & rot_,
    double & a_,
    double & b_,
    double & c_)
    {

    // [ xx  xy  xz ]
    // [ yx  yy  yz ]
    // [ zx  zy  zz ]

    double xx = rot_.xx ();
    double xy = rot_.xy ();
    double xz = rot_.xz ();
    double yx = rot_.yx ();
    double yy = rot_.yy ();
    double yz = rot_.yz ();
    double zx = rot_.zx ();
    double zy = rot_.zy ();
    double zz = rot_.zz ();
    // from a sample by Evgueni Tcherniaev:
    //  see also: http://en.wikipedia.org/wiki/Euler_angles -> Table of matrices
    double sinb = sqrt (zx * zx + zy * zy);
    double cosb = zz;
    if (sinb > 16 * numeric_limits<double>::epsilon ())
    {
    cerr << "DEVEL: sinb != 0" << endl;
    // a_ = atan2 ( zy, -zx) * CLHEP::radian;
    // b_ = atan2 ( sinb, zz) * CLHEP::radian;
    // c_ = atan2 ( yz, xz) * CLHEP::radian;
    a_ = atan2 ( zy, zx) * CLHEP::radian;
    c_ = atan2 ( yz, -xz) * CLHEP::radian;
    double c1 = cos (a_);
    double s3 = sin (c_);
    if ((std::abs (c1) > 16 * numeric_limits<double>::epsilon ()))
    {
    cerr << "DEVEL: cosa != 0" << endl;
    if (std::abs (s3) > 16 * numeric_limits<double>::epsilon ())
    {
    b_ = atan2 ( c1*yz, c1*s3*zz) * CLHEP::radian;
    }
    else
    {
    if (xx * s3 > 0.0)
    {
    b_ = atan2 ( -zy, zz) * CLHEP::radian;
    }
    else
    {
    b_ = atan2 ( zy, zz) * CLHEP::radian;
    }
    }
    }
    else
    {
    cerr << "DEVEL: cosa == 0" << endl;
    }
    }
    else
    {
    cerr << "DEVEL: sinb == 0" << endl;
    if (cosb < 0)
    {
    a_ = (atan2 ( yx, xx) * CLHEP::radian - 180. * CLHEP::degree);
    cerr << "DEVEL: cosb < 0 " << endl;
    b_ = 180. * CLHEP::degree;
    c_ = 0.0;
    }
    else
    {
    cerr << "DEVEL: cosb > 0 " << endl;
    a_ = atan2 ( yx, xx) * CLHEP::radian;
    b_ = 0. * CLHEP::radian;
    c_ = 0. * CLHEP::radian;
    }
    }
    if (std::abs (a_) < 16 * numeric_limits<double>::epsilon ()) a_ = 0.0;
    if (std::abs (b_) < 16 * numeric_limits<double>::epsilon ()) b_ = 0.0;
    if (std::abs (c_) < 16 * numeric_limits<double>::epsilon ()) c_ = 0.0;
    // a_ *= -1.0;
    // b_ *= -1.0;
    // c_ *= -1.0;
    if (a_ < 0.0) a_ += 360. * CLHEP::degree;
    if (b_ < 0.0) b_ += 360. * CLHEP::degree;
    if (c_ < 0.0) c_ += 360. * CLHEP::degree;
    return;
    }
  */

  bool is_identity (const rotation_3d & rot_)
  {
    return rot_.isIdentity ();
  }

  void reset_rotation_3d (rotation_3d & rot_)
  {
    reset (rot_);
    return;
  }

  void tree_dump (const rotation_3d & rot_,
                  ostream & out_ ,
                  const string & title_,
                  const string & indent_)
  {
    string title = title_;
    string indent = indent_;
    if (title.empty ()) title = title_;
    out_ << title << endl;
    string last_tag  = "`-- ";
    string last_tagc = "    ";
    if (is_valid (rot_)) {
      out_ << indent << last_tag << "[ ( " <<
        setw (11) << setprecision (6) << rot_.xx () << "   " <<
        setw (11) << setprecision (6) << rot_.xy () << "   " <<
        setw (11) << setprecision (6) << rot_.xz () << ")" << endl;
      out_ << indent << last_tagc << "  ( " <<
        setw (11) << setprecision (6) << rot_.yx () << "   " <<
        setw (11) << setprecision (6) << rot_.yy () << "   " <<
        setw (11) << setprecision (6) << rot_.yz () << ")" << endl;
      out_ << indent << last_tagc << "  ( " <<
        setw (11) << setprecision (6) << rot_.zx () << "   " <<
        setw (11) << setprecision (6) << rot_.zy () << "   " <<
        setw (11) << setprecision (6) << rot_.zz () << ") ]" << endl;
    } else {
      out_ << indent << last_tag << "[" << "invalid" << "]" << endl;
    }
    return;
  }

  const std::string & filled_utils::filled_none_label()
  {
    static std::string label;
    if (label.empty()) label = "none";
    return label;
  }

  const std::string & filled_utils::filled_by_envelope_label()
  {
    static std::string label;
    if (label.empty()) label = "by_envelope";
    return label;
  }

  const std::string & filled_utils::filled_by_extrusion_label()
  {
    static std::string label;
    if (label.empty()) label = "by_extrusion";
    return label;
  }

  string filled_utils::get_filled_mode_label(filled_utils::filled_type t)
  {
    if (t == FILLED_NONE) return filled_none_label();
    if (t == FILLED_BY_ENVELOPE) return filled_by_envelope_label();
    if (t == FILLED_BY_EXTRUSION) return filled_by_extrusion_label();
    return "";
  }

  filled_utils::filled_type filled_utils::get_filled_mode(const std::string & filled_label)
  {
    if ( filled_label == filled_none_label()) return FILLED_NONE;
    if ( filled_label == filled_by_envelope_label()) return FILLED_BY_ENVELOPE;
    if ( filled_label == filled_by_extrusion_label()) return FILLED_BY_EXTRUSION;
    return FILLED_UNDEFINED;
  }

  // Functions to compute geometric barycenter :

  void compute_barycenter (const std::vector<vector_3d> & points_,
                           vector_3d & barycenter_)
  {

    if (points_.size () == 0) {
      invalidate (barycenter_);
      return;
    }
    barycenter_.set (0.0, 0.0, 0.0);
    for (std::vector<vector_3d>::const_iterator i = points_.begin ();
         i != points_.end ();
         i++) {
      barycenter_ += *i;
    }
    barycenter_ /= points_.size ();
    return;
  }


  vector_3d compute_barycenter (const std::vector<vector_3d> & points_)
  {
    vector_3d barycenter;
    compute_barycenter (points_, barycenter);
    return barycenter;
  }

  void compute_weighted_barycenter (const std::vector<vector_3d> & points_,
                                    const std::vector<double> & weights_,
                                    vector_3d & weighted_barycenter_)
  {
    if (points_.size () != weights_.size ()) {
      invalidate (weighted_barycenter_);
      DT_THROW_IF(true, std::logic_error, "Unmatching vectors of points vs weights !");
      return;
    }
    if (points_.size () == 0) {
      invalidate (weighted_barycenter_);
      return;
    }
    double wsum = 0.0;
    weighted_barycenter_.set (0.0, 0.0, 0.0);
    for (size_t i = 0; i < points_.size (); i++) {
      double wi = weights_[i];
      wsum += wi;
      weighted_barycenter_ += (points_[i] * wi);
    }
    DT_THROW_IF (wsum == 0.0, std::logic_error, "Invalid (zero) sum of weights !");
    weighted_barycenter_ /= wsum;
    return;
  }

  vector_3d compute_weighted_barycenter (const std::vector<vector_3d> & points_,
                                         const std::vector<double> & weights_)
  {
    vector_3d weighted_barycenter;
    compute_weighted_barycenter (points_, weights_, weighted_barycenter);
    return weighted_barycenter;
  }

} // end of namespace geomtools

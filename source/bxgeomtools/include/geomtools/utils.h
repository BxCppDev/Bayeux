/// \file geomtools/utils.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2015-02-18
 *
 * License:
 *
 * Description:
 *
 *  Utilities.
 *
 */

#ifndef GEOMTOOLS_UTILS_H
#define GEOMTOOLS_UTILS_H 1

// Standard library:
#include <string>
#include <iostream>
#include <list>
#include <vector>

// Third party:
// - Bayeux/datatools:
#include <datatools/bit_mask.h>
#include <datatools/reflection_interface.h>

// This project:
#include <geomtools/geomtools_config.h>
#include <geomtools/clhep.h>

namespace geomtools {

  //! \brief Check if a position is inside a position interval
  bool position_is_in(double position_,
                      double start_position_, double delta_position_,
                      double tolerance_ = 0.0,
                      bool bounds_excluded_ = false);

  //! \brief Check if an angle is inside an angular interval
  bool angle_is_in(double angle_,
                   double start_angle_, double delta_angle_,
                   double angular_tolerance_ = 0.0,
                   bool bounds_excluded_ = false);

  //! Alias for a list of 2D vertice as a 2D-polyline
  typedef std::list<vector_2d> basic_polyline_2d;

  //! Aliases for a segment made of two 3D vertice
  typedef std::pair<vector_3d, vector_3d> basic_segment_3d;
  typedef basic_segment_3d segment_type;

  //! Aliases for a list of 3D vertice as a 3D-polyline
  typedef std::list<vector_3d> basic_polyline_3d;
  typedef std::list<vector_3d> polyline_type;

  //! Aliases for an ordered collection of 3D vertice
  typedef std::vector<vector_3d> vertex_col_type;

  //! Alias for a list of 3D-polylines
  typedef std::list<polyline_type> wires_type;

  void print_xy(std::ostream & out_,
                const vector_2d & p_,
                bool endl_ = true);

  std::string to_xy(const vector_2d & p_);

  std::string vector_2d_to_xy(const vector_2d & p_);

  void print_xy_stdout(const vector_2d & p_);

  void print_xy_stderr(const vector_2d & p_);

  void print_xyz(std::ostream & out_,
                 const vector_3d & p_,
                 bool endl_ = true);

  void print(std::ostream & out_, const vector_3d & p_);

  std::string to_xyz(const vector_3d & p_);

  std::string vector_3d_to_xyz(const vector_3d & p_);

  void print_xyz_stdout(const vector_3d & p_);

  void print_xyz_stderr(const vector_3d & p_);

  void set_vector_2d(const std::vector<double> & source_, vector_2d & target_);

  void set_vector_3d(const std::vector<double> & source_, vector_3d & target_);

  //! \brief Parse a vector 3D object from an input stream
  //!
  //! Supported format is:
  //! \code
  //! x y z [(length unit)]
  //! \endcode
  //! where x, y and z are the cartesian coordinates in an arbitrary reference frame and the \a lenght \a unit
  //! is passed optionally as a character string representing the unit symbol.
  //! Examples:
  //! \code
  //!   1.2 3.4 5.6
  //!   1.2 3.4 5.6 (mm)
  //! \endcode
  bool parse(std::istream & in_, vector_3d & position_, bool nothing_more_ = false);

  //! \brief Parse a vector 3D object from an input string
  bool parse(const std::string & token_, vector_3d & position_);

  void print_xy(std::ostream & out_,
                const basic_polyline_2d & p_,
                bool endl_ = true);

  std::string to_xy(const basic_polyline_2d & p_);

  std::string basic_polyline_2d_to_xy(const basic_polyline_2d & p_);

  void print_xy_stdout(const basic_polyline_2d & p_);

  void print_xy_stderr(const basic_polyline_2d & p_);

  void print_xyz(std::ostream & out_,
                 const basic_polyline_3d & p_,
                 bool endl_ = true);

  std::string to_xyz(const basic_polyline_3d & p_);

  std::string basic_polyline_3d_to_xyz(const basic_polyline_3d & p_);

  void print_xyz_stdout(const basic_polyline_3d & p_);

  void print_xyz_stderr(const basic_polyline_3d & p_);

  //! Orientation constants
  enum orientation_type {
    ORIENTATION_INVALID    = -1, ///< Invalid orientation
    ORIENTATION_VERTICAL   =  0, ///< Vertical orientation
    VERTICAL               =  0, ///< Vertical orientation
    ORIENTATION_HORIZONTAL =  1, ///< Horizontal orientation
    HORIZONTAL             =  1  ///< Horizontal orientation
  };

  //! Direction flags
  enum direction_flags_type {
    DIRECTION_FLAGS_NONE   = 0x0,
    DIRECTION_FLAGS_BACK   = datatools::bit_mask::bit00, ///< -x direction
    DIRECTION_FLAGS_FRONT  = datatools::bit_mask::bit01, ///< +x direction
    DIRECTION_FLAGS_LEFT   = datatools::bit_mask::bit02, ///< -y direction
    DIRECTION_FLAGS_RIGHT  = datatools::bit_mask::bit03, ///< +y direction
    DIRECTION_FLAGS_BOTTOM = datatools::bit_mask::bit04, ///< -z direction
    DIRECTION_FLAGS_TOP    = datatools::bit_mask::bit05  ///< +z direction
  };

  //! Direction type
  enum direction_type {
    DIRECTION_INVALID = -1,                ///< Invalid direction
    DIRECTION_BACK    =  0,                ///< -x direction
    DIRECTION_XMINUS  =  DIRECTION_BACK,   ///< -x direction
    DIRECTION_FRONT   =  1,                ///< +x direction
    DIRECTION_XPLUS   =  DIRECTION_FRONT,  ///< +x direction
    DIRECTION_LEFT    =  2,                ///< -y direction
    DIRECTION_YMINUS  =  DIRECTION_LEFT,   ///< -y direction
    DIRECTION_RIGHT   =  3,                ///< +y direction
    DIRECTION_YPLUS   =  DIRECTION_RIGHT,  ///< +y direction
    DIRECTION_BOTTOM  =  4,                ///< -y direction
    DIRECTION_ZMINUS  =  DIRECTION_BOTTOM, ///< -y direction
    DIRECTION_TOP     =  5,                ///< +z direction
    DIRECTION_ZPLUS   =  DIRECTION_TOP     ///< +z direction
  };

  direction_type get_direction_from_label(const std::string &);

  std::string get_direction_label(direction_type);

  enum vertex_1d_type {
    VERTEX_NONE     = 0x0,
    VERTEX_ALL_BITS = datatools::bit_mask::nbits31,
    VERTEX_ALL      = VERTEX_ALL_BITS
  };

  enum path_1d_type {
    PATH_NONE     = 0x0,
    PATH_ALL_BITS = datatools::bit_mask::nbits31,
    PATH_ALL      = PATH_ALL_BITS
  };

  enum side_2d_type {
    SIDE_NONE     = 0x0,
    EDGE_NONE     = SIDE_NONE,
    SIDE_ALL_BITS = datatools::bit_mask::nbits31,
    EDGE_ALL_BITS = SIDE_ALL_BITS,
    SIDE_ALL      = SIDE_ALL_BITS,
    EDGE_ALL      = SIDE_ALL
  };
  typedef side_2d_type edge_2d_type;

  //! Flags determining the position of a point with respect to a 3D shape
  enum shape_domain_flags_type {
    SHAPE_DOMAIN_NONE                = 0x0,
    SHAPE_DOMAIN_INSIDE              = datatools::bit_mask::bit00,
    SHAPE_DOMAIN_OUTSIDE             = datatools::bit_mask::bit01,
    SHAPE_DOMAIN_ON_SURFACE          = datatools::bit_mask::bit02,
    SHAPE_DOMAIN_INSIDE_DAUGHTER     = datatools::bit_mask::bit03,
    SHAPE_DOMAIN_ON_DAUGHTER_SURFACE = datatools::bit_mask::bit04
  };

  std::string shape_domain_flag_label(shape_domain_flags_type flag_);

  //! Some constants
  struct constants {
    static const int    NO_INTERCEPT = -1;
    static const double DEFAULT_TOLERANCE;
    static const double DEFAULT_ANGULAR_TOLERANCE;
    static const double ZERO_TOLERANCE;
    static const double USING_PROPER_TOLERANCE;

    static double get_default_tolerance();
    static double get_default_angular_tolerance();
    static double get_zero_tolerance();
    static double get_proper_tolerance();
  };

  class filled_utils
  {
  public:

    enum filled_type {
      FILLED_UNDEFINED    = -1,
      FILLED_NONE         =  0,
      FILLED_BY_ENVELOPE  =  1,
      FILLED_BY_EXTRUSION =  2
    };

    static const std::string & filled_none_label();
    static const std::string & filled_by_envelope_label();
    static const std::string & filled_by_extrusion_label();

    static std::string get_filled_mode_label(filled_utils::filled_type);

    static filled_type get_filled_mode(const std::string & filled_label);

  };

  /* Initialize a rotation matrix for
   * "World coordinates system->Local coordinates system":
   */

  /// \brief Type of Euler angles
  enum euler_angles_type {
    EULER_ANGLES_INVALID = -1, //!< Invalid Euler angles
    EULER_ANGLES_ZXZ     =  0, //!< ZXZ classic Euler angles
    EULER_ANGLES_XYX     =  1, //!< XYX classic Euler angles
    EULER_ANGLES_YZY     =  2, //!< YZY classic Euler angles
    EULER_ANGLES_ZYZ     =  3, //!< ZYZ classic Euler angles
    EULER_ANGLES_XZX     =  4, //!< XZX classic Euler angles
    EULER_ANGLES_YXY     =  5, //!< YXY classic Euler angles
    EULER_ANGLES_XYZ     =  6, //!< XYZ Tait-Bryan angles
    EULER_ANGLES_YZX     =  7, //!< YZX Tait-Bryan angles
    EULER_ANGLES_ZXY     =  8, //!< ZXY Tait-Bryan angles
    EULER_ANGLES_XZY     =  9, //!< XZY Tait-Bryan angles
    EULER_ANGLES_ZYX     = 10, //!< ZYX Tait-Bryan angles
    EULER_ANGLES_YXZ     = 11  //!< YXZ Tait-Bryan angles
  };

  //! Create a rotation using Euler angles
  //!
  //! @param rot_ the rotation matrix to be initialized
  //! @param angle0_ the first Euler angle
  //! @param angle1_ the second Euler angle
  //! @param angle2_ the third Euler angle
  //! @param et_     the type of Euler angles
  void create(rotation_3d & rot_,
              double angle0_,
              double angle1_,
              double angle2_,
              euler_angles_type et_);

  //! Create a rotation using ZYZ Euler angles
  //!
  //! @param rot_ the rotation matrix to be initialized
  //! @param phi_ the azimuthal angle
  //! @param theta_ the polar/zenith angle
  //! @param delta_ the tilt angle
  void create_zyz(rotation_3d & rot_,
                  double phi_,
                  double theta_,
                  double delta_);

  //! Create a rotation (default is using ZYZ Euler angles)
  //!
  //! @param rot_ the rotation matrix to be initialized
  //! @param phi_ the azimuthal angle
  //! @param theta_ the polar/zenith angle
  //! @param delta_ the tilt angle
  void create(rotation_3d & rot_,
              double phi_,
              double theta_,
              double delta_);

  //! Create a rotation using ZXZ Euler angles
  void create_zxz(rotation_3d & rot_,
                  double phi_,
                  double theta_,
                  double psi_);

  //! Create a rotation using XYZ Euler angles
  void create_xyz(rotation_3d & rot_,
                  double phi_,
                  double theta_,
                  double psi_);

  //! Create a rotation(default is using ZYZ Euler angles)
  //!
  //! @param rot_ the rotation matrix to be initialized
  //! @param phi_ the azimuthal angle
  //! @param theta_ the polar/zenith angle
  //! @param delta_ the tilt angle
  void create_rotation_3d(rotation_3d & rot_,
                          double phi_,
                          double theta_,
                          double delta_);

  //! Create a rotation (default is using ZYZ Euler angles)
  //!
  //! @param rot_ the rotation matrix to be initialized
  //! @param phi_ the azimuthal angle
  //! @param theta_ the polar/zenith angle
  //! @param delta_ the tilt angle
  void create_rotation_from_zyz_euler_angles(rotation_3d & rot_,
                                             double phi_,
                                             double theta_,
                                             double delta_);

  //! Create a rotation (default is using ZYZ Euler angles)
  //!
  //! @param rot_ the rotation matrix to be initialized
  //! @param phi_ the azimuthal angle
  //! @param theta_ the polar/zenith angle
  //! @param delta_ the tilt angle
  void create_rotation(rotation_3d & rot_,
                       double phi_,
                       double theta_,
                       double delta_);

  //! Check if a rotation is the identity
  bool is_identity(const rotation_3d & rot_);

  //! Extract the XYZ Euler angles from a rotation
  void extract_xyz_euler_angle_from_rotation(const rotation_3d & rot_,
                                             double & a_,
                                             double & b_,
                                             double & c_);

  //! Extract the ZYZ Euler angles from a rotation
  // http://www.geometrictools.com/Documentation/EulerAngles.pdf
  void extract_zyz_euler_angle_from_rotation(const rotation_3d & rot_,
                                             double & a_,
                                             double & b_,
                                             double & c_);

  // http://www.cgafaq.info/wiki/Euler_angles_from_matrix
  // struct extract_euler_angle
  // {
  //   int i, neg, alt, rev;
  //   extract_euler_angle()  {}
  // };

  //! Special rotation angle values
  enum special_rotation_angle_type {
    ROTATION_ANGLE_INVALID = -1, //!< Invalid special rotation angle
    ROTATION_ANGLE_0   = 0, //!< Null rotation angle
    ROTATION_ANGLE_90  = 1, //!< 90° rotation angle
    ROTATION_ANGLE_180 = 2, //!< 180° rotation angle
    ROTATION_ANGLE_270 = 3  //!< 270° rotation angle
  };

  bool check_special_rotation_angle(int);

  double get_special_rotation_angle(int);

  int get_special_rotation_angle_from_label(const std::string & );

  std::string get_special_rotation_angle_label(int);

  //! Axis type
  enum axis_type {
    AXIS_INVALID = -1, //!< Invalid axis
    AXIS_X = 0, //!< X axis
    AXIS_Y = 1, //!< Y axis
    AXIS_Z = 2  //!< Z axis
  };

  //! Rotation axis type
  enum rotation_axis_type {
    ROTATION_AXIS_INVALID = -1, //!< Invalid rotation axis
    ROTATION_AXIS_X = AXIS_X, //!< X rotation axis
    ROTATION_AXIS_Y = AXIS_Y, //!< Y rotation axis
    ROTATION_AXIS_Z = AXIS_Z  //!< Z rotation axis
  };

  bool check_rotation_axis(int);

  int get_rotation_axis_from_label(const std::string & );

  std::string get_rotation_label(int);

  void create_rotation_from_axis(rotation_3d & rot_,
                                 int axis_,
                                 double angle_);

  void create_rotation(rotation_3d & rot_,
                       int axis_,
                       double angle_);

  void create_rotation(rotation_3d & rot_,
                       int axis_,
                       int special_angle_);

  void create_rotation_from(rotation_3d & rot_,
                            const std::string &);

  //! Invalidate a rotation 3D object
  void reset(rotation_3d & rot_);

  //! Invalidate a rotation 3D object
  void reset_rotation_3d(rotation_3d & rot_);

  //! Smart print for rotation 3D object
  void tree_dump(const rotation_3d & rot_,
                 std::ostream & out_,
                 const std::string & title_ = "",
                 const std::string & indent_ = "");

  //! Invalidate a rotation 3D object
  void invalidate(rotation_3d & rot_);

  //! Rectify a rotation 3D object
  void rectify(rotation_3d & rot_);

  //! Invalidate a rotation 3D object
  void invalidate_rotation_3d(rotation_3d & rot_);

  //! Check if a rotation 3D object is valid
  bool is_valid(const rotation_3d & rot_);

  //! Check if a rotation 3D object is valid
  bool is_valid_rotation_3d(const rotation_3d & rot_);

  //! Create/set coordinates of a vector 3D object (cartesian)
  void create(vector_3d &,
              double x_,
              double y_,
              double z_);

  //! Create/set coordinates of a vector 3D object (cartesian)
  void create_xyz(vector_3d &,
                  double x_,
                  double y_,
                  double z_);

  //! Create/set coordinates of a vector 3D object (polar)
  void create_polar(vector_3d &,
                    double r_,
                    double theta_,
                    double z_);

  //! Create/set coordinates of a vector 3D object (spherical)
  void create_spherical(vector_3d &,
                        double r_,
                        double phi_,
                        double theta_);

  //! Set coordinates of a vector 3D object (cartesian)
  void set(vector_3d & vec_, double x_, double y_, double z_);

  //! Set coordinates of a vector 3D object (spherical)
  void set_r_theta_phi(vector_3d & vec_, double r_, double theta_, double phi_);

  //! Set coordinates of a vector 3D object (polar)
  void set_rho_phi_z(vector_3d & vec_, double rho_, double phi_, double z_);

  //! Zero a vector 3D object
  void zero(vector_3d & vec_);

  //! Invalidate a vector 3D object
  void invalidate(vector_3d & vec_);

  //! Invalidate a vector 3D object
  void invalidate_vector_3d(vector_3d & vec_);

  //! Check if a vector 3D object is valid
  bool is_valid(const vector_3d & vec_);

  //! Check if a vector 3D object is valid
  bool is_valid_vector_3d(const vector_3d & vec_);

  //! Check if two vector 3D objects are close to each other given a tolerance
  bool are_near(const vector_3d & vec1_,
                const vector_3d & vec2_,
                double tolerance_ = constants::DEFAULT_TOLERANCE);

  //! Check if two vector 3D objects are close to each other given a tolerance
  bool are_near_vector_3d(const vector_3d & vec1_,
                          const vector_3d & vec2_,
                          double tolerance_);

  //! Set coordinates of a vector 2D object (cartesian)
  void set(vector_2d & vec_, double x_, double y_);

  //! Set coordinates of a vector 2D object (polar)
  void set_r_phi(vector_2d & vec_, double r_, double phi_);

  //! Zero a vector 2D object
  void zero(vector_2d & vec_);

  //! Invalidate a vector 2D object
  void invalidate(vector_2d & vec_);

  //! Invalidate a vector 2D object
  void invalidate_vector_2d(vector_2d & vec_);

  //! Check if a vector 2D object is valid
  bool is_valid(const vector_2d & vec_);

  //! Check if a vector 2D object is valid
  bool is_valid_vector_2d(const vector_2d & vec_);

  //! Convert a vector 2D object to a vector 3D object (export x and y coordinates, setting z to zero)
  void vector_2d_to_vector_3d(const vector_2d & v2d_, vector_3d & v3d_);

  //! Convert a vector 3D object to a vector 2D object (export only x and y coordinates)
  void vector_3d_to_vector_2d(const vector_3d & v3d_, vector_2d & v2d_);

  //! Create a vector 3D object from spherical angular coordinates (magnitude is set to 1)
  void make_phi_theta(vector_3d & vec_, double phi_, double theta_);

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnoexcept-type"
#endif
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnoexcept-type"
#endif
  //! \brief Isotropically randomize the direction of an unit vector
  //!
  //! @arg ran_ a function or object-function (functor) with the following operator defined:
  //! \code
  //!   double operator()();
  //! \endcode
  //! that must return an uniform deviated random number in the [0,1) range
  //! like drand48 does.
  //! @arg ran_dir_ the unit vector to be randomized
  template <class ran_func>
  void randomize_direction(ran_func & ran_, vector_3d & ran_dir_) 
  {
    double phi = 2. * M_PI * ran_();
    double cos_theta = -1 + 2 * ran_();
    double sin_theta = std::sqrt(1. - cos_theta * cos_theta);
    double x = sin_theta * std::cos(phi);
    double y = sin_theta * std::sin(phi);
    double z = cos_theta;
    ran_dir_.set(x, y, z);
  }
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#ifdef __clang__
#pragma clang diagnostic pop
#endif

  //! \brief Isotropically randomize the direction of an unit vector
  //!
  //! @arg ran_ a function or object-function (functor) with the following operator defined:
  //! \code
  //!   double operator()();
  //! \endcode
  //! that must return an uniform deviated random number in the [0,1) range
  //! like drand48 does.
  //! @return the unit vector to be randomized
  template <class ran_func>
  vector_3d randomize_direction(ran_func & ran_)
  {
    vector_3d dir;
    randomize_direction(ran_, dir);
    return dir;
  }

  //! \brief Isotropically randomize the direction of an unit vector orthogonal to a given direction
  //!
  //! @arg ran_ a function or object-function (functor) with the following operator defined:
  //! \code
  //!   double operator()();
  //! \endcode
  //! that must return an uniform deviated random number in the [0,1) range
  //! like drand48 does.
  //! @arg dir_ the direction the randomized unit vector must be orthogonal to
  //! @arg ran_dir_ the unit vector to be randomized
  template <class ran_func>
  void randomize_orthogonal_direction(ran_func & ran_,
                                      const vector_3d & dir_,
                                      vector_3d & ran_dir_)
  {
    double theta = 2. * M_PI * ran_();
    double dx = cos(theta);
    double dy = sin(theta);
    double dz = 0.0;
    double dir_theta = dir_.theta();
    double dir_phi = dir_.phi();
    rotation_3d dir_rot;
    create_rotation_3d(dir_rot, dir_phi, dir_theta, 0.0);
    rotation_3d dir_inverse_rot;
    dir_inverse_rot = dir_rot.inverse();
    vector_3d v(dx, dy, dz);
    ran_dir_ = v.transform(dir_inverse_rot);
    return;
  }

  //! \brief Isotropically randomize the direction of an unit vector orthogonal to a given direction
  //!
  //! @arg ran_ a function or object-function (functor) with the following operator defined:
  //! \code
  //!   double operator()();
  //! \endcode
  //! that must return an uniform deviated random number in the [0,1) range
  //! like drand48 does.
  //! @arg dir_ the direction the randomized unit vector must be orthogonal to
  //! @return the unit vector to be randomized
  template <class ran_func>
  vector_3d randomize_orthogonal_direction(ran_func & ran_,
                                           const vector_3d & ref_dir_)
  {
    vector_3d dir;
    randomize_direction(ran_, ref_dir_, dir);
    return dir;
  }

  //! Compute the barycenter of a collection of 3D points
  //! @arg points_ the input collection of 3D points
  //! @arg barycenter_ the output computed barycenter
  void compute_barycenter(const std::vector<vector_3d> & points_,
                          vector_3d & barycenter_);

  //! Compute the barycenter of a collection of 3D points
  //! @arg points_ the input collection of 3D points
  //! @return the computed barycenter
  vector_3d compute_barycenter(const std::vector<vector_3d> & points_);

  //! Compute the weighted barycenter of a collection of 3D points
  //! @arg points_ the input collection of 3D points
  //! @arg weights_ the input collection of weights associated to 3D points
  //! @arg weighted_barycenter_ the output computed weighted barycenter
  void compute_weighted_barycenter(const std::vector<vector_3d> & points_,
                                   const std::vector<double> & weights_,
                                   vector_3d & weighted_barycenter_);

  //! Compute the weighted barycenter of a collection of 3D points
  //! @arg points_ the input collection of 3D points
  //! @arg weights_ the input collection of weights associated to 3D points
  //! @return the computed weighted  barycenter
  vector_3d compute_weighted_barycenter(const std::vector<vector_3d> & points_,
                                        const std::vector<double> & weights_);

  //! Wrapper for rotation object
  class rotation_wrapper_type : public rotation_3d
  {
  public:
    rotation_wrapper_type(double mxx_, double mxy_, double mxz_,
                          double myx_, double myy_, double myz_,
                          double mzx_, double mzy_, double mzz_)
      : rotation_3d(mxx_, mxy_, mxz_,
                    myx_, myy_, myz_,
                    mzx_, mzy_, mzz_) {}
  };

  //! Type alias
  typedef rotation_wrapper_type rotation_wrapper_t;

  //! \brief I/O constants for the serialization of vector(2D/3D) and rotation objects
  struct io
  {
    static const std::string & vector_2d_serial_tag();
    static const std::string & vector_3d_serial_tag();
    static const std::string & rotation_3d_serial_tag();
    static const std::string & position_suffix();
    static const std::string & rotation_suffix();
  };

} // end namespace geomtools

//! Serialization stuff for CLHEP 'vector_3d'
namespace boost {

  namespace serialization {

    template<class Archive>
    void save(Archive & a_ar ,
              const geomtools::vector_3d & v_,
              const unsigned int a_version);

    template<class Archive>
    void load(Archive & a_ar ,
              geomtools::vector_3d & v_,
              const unsigned int a_version);

    template<class Archive>
    void serialize(Archive & a_ar,
                   geomtools::vector_3d  & v_,
                   const unsigned int a_version);

  } // namespace serialization

} // namespace boost

//! Serialization stuff for CLHEP 'vector_2d'
namespace boost {

  namespace serialization {

    //! Boost serialization save method for vector 2D objects
    template<class Archive>
    void save(Archive & a_ar ,
              const geomtools::vector_2d & v_,
              const unsigned int a_version);

    //! Boost serialization load method for vector 2D objects
    template<class Archive>
    void load(Archive & a_ar ,
              geomtools::vector_2d & v_,
              const unsigned int a_version);

    //! Boost serialization general method for vector 2D objects
    template<class Archive>
    void serialize(Archive & a_ar,
                   geomtools::vector_2d  & v_,
                   const unsigned int a_version);

  } // namespace serialization

} // namespace boost

//! Serialization stuff for CLHEP 'rotation_3d'
namespace boost {

  namespace serialization {

    //! Boost serialization save method for vector 3D objects
    template<class Archive>
    void save(Archive & a_ar ,
              const geomtools::rotation_3d & r_,
              const unsigned int a_version);

    //! Boost serialization load method for vector 3D objects
    template<class Archive>
    void load(Archive & a_ar ,
              geomtools::rotation_3d & a_rotation,
              const unsigned int a_version);

    //! Boost serialization general method for vector 3D objects
    template<class Archive>
    void serialize(Archive & a_ar,
                   geomtools::rotation_3d  & a_rotation,
                   const unsigned int a_version);

  } // namespace serialization

} // namespace boost

// Activate reflection layer for some 'geomtools::XXX' enums:
DR_TYPE_INIT(::geomtools::orientation_type)
DR_TYPE_INIT(::geomtools::direction_type)
DR_TYPE_INIT(::geomtools::axis_type)
DR_TYPE_INIT(::geomtools::rotation_axis_type)

#endif // GEOMTOOLS_UTILS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

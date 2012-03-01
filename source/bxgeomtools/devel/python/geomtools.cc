/* datatools.cc */

#include <boost/python.hpp>

#include <geomtools/utils.h>
#include <geomtools/line_3d.h>
#include <geomtools/helix_3d.h>

using namespace boost::python;

// overloads:
double (geomtools::vector_3d::*angle_1) () const
  = &geomtools::vector_3d::angle;

double (geomtools::vector_3d::*angle_2) (const geomtools::vector_3d &) const
  = &geomtools::vector_3d::angle;

// overloads:
double (geomtools::vector_3d::*cosTheta_1) () const
  = &geomtools::vector_3d::cosTheta;

double (geomtools::vector_3d::*cosTheta_2) (const geomtools::vector_3d &) const
  = &geomtools::vector_3d::cosTheta;

// overloads:
double (geomtools::vector_3d::*cos2Theta_1) () const
  = &geomtools::vector_3d::cos2Theta;

double (geomtools::vector_3d::*cos2Theta_2) (const geomtools::vector_3d &) const
  = &geomtools::vector_3d::cos2Theta;

// overloads:
geomtools::vector_3d (geomtools::vector_3d::*perpPart_1) () const
  = &geomtools::vector_3d::perpPart;

geomtools::vector_3d (geomtools::vector_3d::*perpPart_2) (const geomtools::vector_3d &) const
  = &geomtools::vector_3d::perpPart;

// overloads:
geomtools::vector_3d (geomtools::vector_3d::*project_1) () const
  = &geomtools::vector_3d::project;

geomtools::vector_3d (geomtools::vector_3d::*project_2) (const geomtools::vector_3d &) const
  = &geomtools::vector_3d::project;

// overloads:
double (geomtools::vector_3d::*rapidity_1) () const
  = &geomtools::vector_3d::rapidity;

double (geomtools::vector_3d::*rapidity_2) (const geomtools::vector_3d &) const
  = &geomtools::vector_3d::rapidity;

// overloads:
double (geomtools::vector_3d::*perp2_1) () const
  = &geomtools::vector_3d::perp2;

double (geomtools::vector_3d::*perp2_2) (const geomtools::vector_3d &) const
  = &geomtools::vector_3d::perp2;

// overloads:
double (geomtools::vector_3d::*polarAngle_1) (const geomtools::vector_3d &) const
  = &geomtools::vector_3d::polarAngle;

double (geomtools::vector_3d::*polarAngle_2) (const geomtools::vector_3d &,
					      const geomtools::vector_3d &) const
  = &geomtools::vector_3d::polarAngle;

// overloads:
double (geomtools::vector_3d::*azimAngle_1) (const geomtools::vector_3d &) const
  = &geomtools::vector_3d::azimAngle;

double (geomtools::vector_3d::*azimAngle_2) (const geomtools::vector_3d &,
					      const geomtools::vector_3d &) const
  = &geomtools::vector_3d::azimAngle;

// rotate:
geomtools::vector_3d & (geomtools::vector_3d::*rotate_1) 
  (const geomtools::vector_3d &, double)
  = &geomtools::vector_3d::rotate;

geomtools::vector_3d & (geomtools::vector_3d::*rotate_2) 
  (double, const geomtools::vector_3d &)
  = &geomtools::vector_3d::rotate;

geomtools::vector_3d & (geomtools::vector_3d::*rotate_3) 
  (double, double, double)
  = &geomtools::vector_3d::rotate;

geomtools::vector_3d & (geomtools::vector_3d::*rotate_4)
  (const CLHEP::HepAxisAngle &)
  = &geomtools::vector_3d::rotate;

geomtools::vector_3d & (geomtools::vector_3d::*rotate_5)
  (const CLHEP::HepEulerAngles &)
  = &geomtools::vector_3d::rotate;

/************/

/*
using namespace geomtools;

std::string (*to_xy_1) 
  (const geomtools::vector_2d &)
  = &geomtools::to_xy;

std::string (*to_xy_2) (const geomtools::basic_polyline_2d &)
  = &geomtools::to_xy;

std::string (*to_xyz_1) (const geomtools::vector_3d &)
  = &geomtools::to_xyz;

std::string (*to_xyz_2) (const geomtools::basic_polyline_3d &)
  = &geomtools::to_xyz;
*/


BOOST_PYTHON_MODULE (geomtools)
{
  using namespace boost::python;

  /****************/

  def ("invalidate_vector_2d", geomtools::invalidate_vector_2d);

  def ("is_valid_vector_2d", geomtools::is_valid_vector_2d);

  def ("vector_2d_to_xy",geomtools::vector_2d_to_xy);

  def ("basic_polyline_2d_to_xy", geomtools::basic_polyline_2d_to_xy);

  def ("invalidate_vector_3d", geomtools::invalidate_vector_3d);

  def ("is_valid_vector_3d", geomtools::is_valid_vector_3d);

  def ("are_near_vector_3d", geomtools::are_near_vector_3d);

  def ("vector_3d_to_xyz", geomtools::vector_3d_to_xyz);

  def ("basic_polyline_3d_to_xyz", geomtools::basic_polyline_3d_to_xyz);

  /****************/

  class_<geomtools::vector_2d> ("vector_2d", init<> ())

    .def(init<double, double>())

    .def(init<const geomtools::vector_2d &>())

    .def("x",     &geomtools::vector_2d::x)

    .def("y",     &geomtools::vector_2d::y)

    .def("set_x",     &geomtools::vector_2d::setX)

    .def("set_y",     &geomtools::vector_2d::setY)

    .def("set",     &geomtools::vector_2d::set)

    .def("get_mag_sqr", &geomtools::vector_2d::mag2)

    .def("get_mag",     &geomtools::vector_2d::mag)

    .def("get_phi",     &geomtools::vector_2d::phi)

    .def("get_r",     &geomtools::vector_2d::r)

    .def("set_r",     &geomtools::vector_2d::setR)

    .def("set_phi",     &geomtools::vector_2d::setPhi)

    .def("set_mag",     &geomtools::vector_2d::setMag)

    .def("set_polar",     &geomtools::vector_2d::setPolar)

    .def("compare",     &geomtools::vector_2d::compare)

    .def("unit",              &geomtools::vector_2d::unit)

    .def("orthogonal",        &geomtools::vector_2d::orthogonal)

    .def("dot",        &geomtools::vector_2d::dot)

    .def("get_angle_with",        &geomtools::vector_2d::angle)

    .def("rotate",        &geomtools::vector_2d::rotate)

    .def(-self)    

    .def(self + self)

    .def(self - self)

    .def(self * float())

    .def(float() * self)

    .def(self / float())

    .def(self += self)

    .def(self -= self)

    .def(self *= float())

    //.def(self /= float())

    .def(self < self)

    .def(self <= self)

    .def(self > self)

    .def(self >= self)

    .def(self_ns::str (self))

    ;

  /****************/

  class_<geomtools::vector_3d> ("vector_3d", init<> ())

    .def(init<double, double, double>())

    .def(init<const geomtools::vector_3d &>())

    .def("x",     &geomtools::vector_3d::x)

    .def("y",     &geomtools::vector_3d::y)

    .def("z",     &geomtools::vector_3d::z)

    .def("get_x", &geomtools::vector_3d::x)

    .def("get_y", &geomtools::vector_3d::y)

    .def("get_z", &geomtools::vector_3d::z)

    .def("set_x", &geomtools::vector_3d::setX)

    .def("set_y", &geomtools::vector_3d::setY)

    .def("set_z", &geomtools::vector_3d::setZ)

    .def("set",   &geomtools::vector_3d::set)

    .def("get_cos_theta",          cosTheta_1)

    .def("get_cos_theta_with",     cosTheta_2)

    .def("get_cos_sqr_theta",      cos2Theta_1)

    .def("get_cos_sqr_theta_with", cos2Theta_2)

    .def("get_mag_sqr", &geomtools::vector_3d::mag2)

    .def("get_mag",     &geomtools::vector_3d::mag)

    .def("set_phi",     &geomtools::vector_3d::setPhi)

    .def("set_theta",   &geomtools::vector_3d::setTheta)

    .def("set_mag",     &geomtools::vector_3d::setMag)

    .def("get_perp_sqr",      perp2_1)

    .def("get_perp_sqr_with", perp2_2)

    .def("get_perp",          perpPart_1)

    .def("get_perp_with",     perpPart_2)

    .def("get_project",       project_1)

    .def("get_project_with",  project_2)

    .def("set_perp",          &geomtools::vector_3d::setPerp)

    .def("set_cyl_theta",     &geomtools::vector_3d::setCylTheta)

    .def("unit",              &geomtools::vector_3d::unit)

    .def("orthogonal",        &geomtools::vector_3d::orthogonal)

    .def("get_dot",             &geomtools::vector_3d::dot)

    .def("get_cross",           &geomtools::vector_3d::cross)

    .def("get_angle",           angle_1)

    .def("get_angle_with",      angle_2)

    .def("get_pseudo_rapidity", &geomtools::vector_3d::pseudoRapidity)

    .def("get_r",     &geomtools::vector_3d::getR)

    .def("set_r",     &geomtools::vector_3d::setR)

    .def("get_theta", &geomtools::vector_3d::getTheta)

    .def("get_phi",   &geomtools::vector_3d::getPhi)

    .def("get_rho",   &geomtools::vector_3d::getRho)

    .def("set_rho",   &geomtools::vector_3d::setRho)

    .def("compare",   &geomtools::vector_3d::compare)

    .def("diff_sqr",  &geomtools::vector_3d::diff2)

    .def("is_parallel",    &geomtools::vector_3d::isParallel)

    .def("is_orthogonal",  &geomtools::vector_3d::isOrthogonal)

    .def("how_parallel",   &geomtools::vector_3d::howParallel)

    .def("how_orthogonal", &geomtools::vector_3d::howOrthogonal)

    .def("get_beta",       &geomtools::vector_3d::beta)

    .def("get_gamma",      &geomtools::vector_3d::gamma)

    .def("get_colinear_rapidity",    &geomtools::vector_3d::coLinearRapidity)

    .def("get_rapidity",             rapidity_1)

    .def("get_rapidity_with",        rapidity_2)

    .def("get_polar_angle_with",     polarAngle_1)

    .def("get_polar_angle_with_ref", polarAngle_2)

    .def("get_azim_angle_with",      azimAngle_1)

    .def("get_azim_angle_with_ref",  azimAngle_2)

    .def("get_delta_phi_with",       &geomtools::vector_3d::deltaPhi)

    .def("rotate", rotate_1, return_internal_reference<> ())

    .def("rotate", rotate_2, return_internal_reference<> ())

    .def(-self)    

    .def(self + self)

    .def(self - self)

    .def(self * float())

    .def(float() * self)

    .def(self / float())

    .def(self += self)

    .def(self -= self)

    .def(self *= float())

    .def(self /= float())

    .def(self < self)

    .def(self <= self)

    .def(self > self)

    .def(self >= self)

    .def(self_ns::str (self))

    ;

  /****************/

  def ("create_rotation_3d", geomtools::create_rotation_3d);

  def ("create_rotation_from_axis", 
       geomtools::create_rotation_from_axis);

  def ("create_rotation_from_zyz_euler_angles", 
       geomtools::create_rotation_from_zyz_euler_angles);

  def ("reset_rotation_3d", geomtools::reset_rotation_3d);

  def ("invalidate_rotation_3d", geomtools::invalidate_rotation_3d);

  def ("is_valid_rotation_3d", geomtools::is_valid_rotation_3d);

  /****************/

  class_<geomtools::rotation_3d> ("rotation_3d", init<> ())

    .def(init<const geomtools::rotation_3d &>())

    //.def("inverse", geomtools::rotation_3d::inverse)
    //.def("invert", geomtools::rotation_3d::invert, return_internal_reference<> ())

    .def(self * self)

    .def(self * other<geomtools::vector_3d>())

    .def(self_ns::str (self))

    ;

  /****************/

  class_<geomtools::line_3d> ("line_3d", init<> ())

    .def(init<const geomtools::vector_3d &, const geomtools::vector_3d &> ())

    .def("is_valid", 
	 &geomtools::line_3d::is_valid)

    .def("invalidate", 
	 &geomtools::line_3d::invalidate)

    .def("get_point", 
	 &geomtools::line_3d::get_point)

    .def("get_first", 
	 &geomtools::line_3d::get_first,
	 return_value_policy<copy_const_reference> ())

    .def("set_first", 
	 &geomtools::line_3d::set_first)

    .def("get_last", 
	 &geomtools::line_3d::get_last,
	 return_value_policy<copy_const_reference> ())

    .def("set_last", 
	 &geomtools::line_3d::set_last)

    .def("get_length", 
	 &geomtools::line_3d::get_length)

    .def("dump", 
	 &geomtools::line_3d::dump)
    ;


  /****************/
 
  class_<geomtools::helix_3d> ("helix_3d", init<> ())

    .def("is_valid", 
	 &geomtools::helix_3d::is_valid)

    .def("invalidate", 
	 &geomtools::helix_3d::invalidate)

    .def("get_point", 
	 &geomtools::helix_3d::get_point)

    .def("get_center_const", 
	 &geomtools::helix_3d::get_center_const,
	 return_value_policy<copy_const_reference> ())

    .def("set_center", 
	 &geomtools::helix_3d::set_center)

    .def("get_radius", 
	 &geomtools::helix_3d::get_radius)

    .def("set_radius", 
	 &geomtools::helix_3d::set_radius)

    .def("get_step", 
	 &geomtools::helix_3d::get_step)

    .def("set_step", 
	 &geomtools::helix_3d::set_step)

    .def("get_t1", 
	 &geomtools::helix_3d::get_t1)

    .def("set_t1", 
	 &geomtools::helix_3d::set_t1)

    .def("get_t2", 
	 &geomtools::helix_3d::get_t2)

    .def("set_t2", 
	 &geomtools::helix_3d::set_t2)

    .def("get_angle1", 
	 &geomtools::helix_3d::get_angle1)

    .def("set_angle1", 
	 &geomtools::helix_3d::set_angle1)

    .def("get_angle2", 
	 &geomtools::helix_3d::get_angle2)

    .def("set_angle2", 
	 &geomtools::helix_3d::set_angle2)

    .def("get_curvilinear_position", 
	 &geomtools::helix_3d::get_curvilinear_position)

    .def("t_is_valid", 
	 &geomtools::helix_3d::t_is_valid)

    .def("get_theta_from_xy", 
	 &geomtools::helix_3d::get_theta_from_xy)

    .def("get_t_from_xyz", 
	 &geomtools::helix_3d::get_t_from_xyz)

    .def("get_t_from_z", 
	 &geomtools::helix_3d::get_t_from_z)

    .def("get_z_from_t", 
	 &geomtools::helix_3d::get_z_from_t)

    .def("get_point_xy", 
	 &geomtools::helix_3d::get_point_xy)

    .def("invalidate_angles", 
	 &geomtools::helix_3d::invalidate_angles)

    .def("angles_are_valid", 
	 &geomtools::helix_3d::angles_are_valid)

    .def("get_first", 
	 &geomtools::helix_3d::get_first)

    .def("get_last", 
	 &geomtools::helix_3d::get_last)

    .def("dump", 
	 &geomtools::helix_3d::dump)


    ;

}

/* end of datatools.cc */


// -*- mode: c++; -*-
/* placement.cc
 */

#include <geomtools/placement.h>

namespace geomtools {

  using namespace std;

  bool placement::is_replica () const
  {
    return false;
  }

  bool placement::has_only_one_rotation () const
  {
    return true;
  }

  bool placement::is_valid () const
  {
    return
      geomtools::is_valid (__translation)
      && geomtools::is_valid (__rotation)
      && geomtools::is_valid (__inverse_rotation);
  }

  void placement::invalidate ()
  {
    geomtools::invalidate (__translation);
    geomtools::invalidate (__rotation);
    geomtools::invalidate (__inverse_rotation);
    __rotation_axis = ROTATION_AXIS_INVALID;
    __rotation_angle = std::numeric_limits<double>::quiet_NaN ();
    __phi = __theta = __delta = std::numeric_limits<double>::quiet_NaN ();
    return;
  }

  const vector_3d &
  placement::get_translation () const
  {
    return __translation;
  }

  void
  placement::set_translation (double x_, double y_, double z_)
  {
    set_translation (vector_3d (x_, y_, z_));
  }

  void
  placement::set_translation (const vector_3d & new_value_)
  {
    __translation = new_value_;
  }

  const rotation_3d &
  placement::get_rotation () const
  {
    return __rotation;
  }

  bool placement::is_simple_rotation () const
  {
    return is_rotation_x () || is_rotation_y () || is_rotation_z ();
  }

  bool placement::is_zyz_rotation () const
  {
    return ! is_simple_rotation ();
  }

  bool placement::is_rotation_x () const
  {
    return __rotation_axis == ROTATION_AXIS_X;
  }

  bool placement::is_rotation_y () const
  {
    return __rotation_axis == ROTATION_AXIS_Y;
  }

  bool placement::is_rotation_z () const
  {
    return __rotation_axis == ROTATION_AXIS_Z;
  }

  int placement::get_rotation_axis () const
  {
    return __rotation_axis;
  }

  double placement::get_rotation_angle () const
  {
    return __rotation_angle;
  }

  double placement::get_phi () const
  {
    return __phi;
  }

  double placement::get_theta () const
  {
    return __theta;
  }

  double placement::get_delta () const
  {
    return __delta;
  }

  void placement::__compute_orientation ()
  {
    if (is_simple_rotation ())
      {
	// X, Y or Z rotations:
	geomtools::create_rotation_from_axis (__rotation, __rotation_axis, __rotation_angle);
      }
    else
      {
	// rotation by (Z,Y,Z) Euler angles:
	geomtools::create_rotation_from_zyz_euler_angles (__rotation, __phi, __theta, __delta);
      }
    geomtools::rectify (__rotation);
    __inverse_rotation = __rotation.inverse ();
    return;
  }

  void placement::__compute_orientation_xyz ()
  {
    if (is_simple_rotation ())
      {
	// X, Y or Z rotations:
	geomtools::create_rotation_from_axis (__rotation,
					      __rotation_axis,
					      __rotation_angle);
      }
    else
      {
	// rotation by (X,Y,Z) Euler angles:
	geomtools::create_xyz (__rotation, __phi, __theta, __delta);
      }
    geomtools::rectify (__rotation);
    __inverse_rotation = __rotation.inverse ();
    return;
  }

  bool placement::has_angles () const
  {
    if (is_simple_rotation ())
      {
	return __rotation_angle == __rotation_angle;
      }
    // else:
    return ((__phi == __phi) && (__theta == __theta) && (__delta == __delta));
  }

  void placement::set (double x_, double y_, double z_,
		       double phi_, double theta_, double delta_)
  {
    set_translation (x_, y_, z_);
    set_orientation (phi_, theta_, delta_);
    return;
  }

  void placement::set (const vector_3d & t_,
		       double phi_, double theta_, double delta_)
  {
    set_translation (t_);
    set_orientation (phi_, theta_, delta_);
    return;
  }

  void placement::set (double x_, double y_, double z_,
		       int axis_, double angle_)
  {
    set_translation (x_, y_, z_);
    set_orientation (axis_, angle_);
    return;
  }

  void placement::set (const vector_3d & t_,
		       int axis_, double angle_)
  {
    set_translation (t_);
    set_orientation (axis_, angle_);
    return;
  }

  void placement::set_orientation (int axis_, double angle_)
  {
    if (! check_rotation_axis (axis_))
      {
	throw runtime_error ("placement::set_orientation: Invalid rotation axis !");
      }
    __rotation_axis = axis_;
    __rotation_angle = angle_;
    __phi = std::numeric_limits<double>::quiet_NaN ();
    __theta = std::numeric_limits<double>::quiet_NaN ();
    __delta = std::numeric_limits<double>::quiet_NaN ();
    __compute_orientation ();
    return;
  }

  void placement::set_orientation (double phi_, double theta_, double delta_)
  {
    __rotation_axis = ROTATION_AXIS_INVALID;
    __rotation_angle = std::numeric_limits<double>::quiet_NaN ();
    __phi = phi_;
    __theta = theta_;
    __delta = delta_;
    __compute_orientation ();
    return;
  }

  // Not recommended at all:
  void placement::set_orientation (const rotation_3d & r_)
  {
    __rotation_axis == ROTATION_AXIS_INVALID;
    __rotation_angle = std::numeric_limits<double>::quiet_NaN ();
    __phi   = std::numeric_limits<double>::quiet_NaN ();
    __theta = std::numeric_limits<double>::quiet_NaN ();
    __delta = std::numeric_limits<double>::quiet_NaN ();
    __rotation = r_;
    //__rotation.rectify ();
    rectify (__rotation);
    __inverse_rotation = __rotation.inverse ();
    extract_zyz_euler_angle_from_rotation (__rotation,
					   __phi,
					   __theta,
					   __delta);
    return;
  }

  void placement::set_orientation_xyz (double phi_, double theta_, double delta_)
  {
    __rotation_axis = ROTATION_AXIS_INVALID;
    __rotation_angle = std::numeric_limits<double>::quiet_NaN ();
    __phi = phi_;
    __theta = theta_;
    __delta = delta_;
    __compute_orientation_xyz ();
    extract_zyz_euler_angle_from_rotation (__rotation,
					   __phi,
					   __theta,
					   __delta);
    return;
  }

  const rotation_3d & placement::get_inverse_rotation () const
  {
    return __inverse_rotation;
  }

  // i_placement interface:
  size_t placement::get_number_of_items () const
  {
    return 1;
  }

  size_t placement::compute_index_map (vector<uint32_t> & map_,
				       int item_) const
  {
    if ((item_ < 0) || (item_ >= get_number_of_items ()))
      {
 	ostringstream message;
	message << "placement::get_index_map: "
		<< "Invalid item index '" << item_ << "' !" << endl;
	throw runtime_error (message.str ());
      }
    map_.clear ();
    map_.push_back (item_);
    return map_.size ();
  }

  void placement::get_placement (int item_, placement & p_) const
  {
    if (item_ != 0)
      {
	ostringstream message;
	message << "placement::get_placement: Invalid item index '" << item_ << "' !" << endl;
	throw runtime_error (message.str ());
      }
    p_ = *this;
  }

  // ctor:
  placement::placement () : i_placement ()
  {
    __rotation_axis = ROTATION_AXIS_INVALID;
    __rotation_angle = std::numeric_limits<double>::quiet_NaN ();
    __phi = __theta = __delta = 0.0;
    return;
  }

  // ctor:
  placement::placement (const vector_3d & translation_,
			double phi_,
			double theta_,
			double delta_) : i_placement ()
  {
    __rotation_axis = ROTATION_AXIS_INVALID;
    __rotation_angle = std::numeric_limits<double>::quiet_NaN ();
    __phi = __theta = __delta = std::numeric_limits<double>::quiet_NaN ();
    set_translation (translation_);
    set_orientation (phi_, theta_, delta_);
    return;
  }

  // ctor:
  placement::placement (const vector_3d & translation_,
			int axis_,
			double angle_) : i_placement ()
  {
    __rotation_axis = ROTATION_AXIS_INVALID;
    __rotation_angle = std::numeric_limits<double>::quiet_NaN ();
    __phi = __theta = __delta = 0.0;
    set_translation (translation_);
    set_orientation (axis_, angle_);
    return;
  }

  // ctor:
  placement::placement (double x_,
			double y_,
			double z_,
			double phi_,
			double theta_,
			double delta_) : i_placement ()
  {
    __rotation_axis = ROTATION_AXIS_INVALID;
    __rotation_angle = std::numeric_limits<double>::quiet_NaN ();
    __phi = __theta = __delta = 0.0;
    set_translation (x_, y_, z_);
    set_orientation (phi_, theta_, delta_);
    return;
  }

  // ctor:
  placement::placement (double x_,
			double y_,
			double z_,
			int axis_,
			double angle_) : i_placement ()
  {
    __rotation_axis = ROTATION_AXIS_INVALID;
    __rotation_angle = std::numeric_limits<double>::quiet_NaN ();
    __phi = __theta = __delta = std::numeric_limits<double>::quiet_NaN ();
    set_translation (x_, y_, z_);
    set_orientation (axis_, angle_);
    return;
  }

  // dtor:
  placement::~placement ()
  {
    return;
  }

  void placement::reset ()
  {
    set_translation (0., 0., 0.);
    set_orientation (0., 0., 0.);
    return;
  }

  // transformation methods:

  void placement::mother_to_child (const vector_3d & mother_pos_,
				   vector_3d & child_pos_) const
  {
    vector_3d v = mother_pos_ - __translation;
    child_pos_ = v.transform (__rotation);
    return;
  }

  vector_3d placement::mother_to_child (const vector_3d & mother_pos_) const
  {
    vector_3d v;
    mother_to_child (mother_pos_, v);
    return v;
  }

  void placement::child_to_mother (const vector_3d & child_pos_ ,
				   vector_3d & mother_pos_) const
  {
    vector_3d v (child_pos_);
    mother_pos_ = v.transform (__inverse_rotation) + __translation;
    return;
  }

  vector_3d placement::child_to_mother (const vector_3d & child_pos_) const
  {
    vector_3d v;
    child_to_mother (child_pos_, v);
    return v;
  }

  void placement::mother_to_child_direction (const vector_3d & mother_dir_,
					     vector_3d & child_dir_) const
  {
    vector_3d a;
    mother_to_child (vector_3d ( 0., 0., 0.), a);
    vector_3d b;
    mother_to_child (mother_dir_, b);
    child_dir_= b - a;
    return;
  }

  vector_3d placement::mother_to_child_direction (const vector_3d & mother_dir_) const
  {
    vector_3d v;
    mother_to_child_direction (mother_dir_,v);
    return v;
  }

  void placement::child_to_mother_direction (const vector_3d & child_dir_ ,
					     vector_3d & mother_dir_) const
  {
    vector_3d a;
    child_to_mother (vector_3d (0., 0., 0.), a);
    vector_3d b;
    child_to_mother (child_dir_, b);
    mother_dir_= b - a;
    return;
  }

  vector_3d placement::child_to_mother_direction (const vector_3d & child_dir_) const
  {
    vector_3d v;
    child_to_mother_direction (child_dir_,v);
    return v;
  }

  /*
uti  void placement::mother_to_child (const placement & pm_, placement & pc_) const
  {
    mother_to_child (pm_.__translation, pc_.__translation);
    pc_.__phi = pc_.__theta = pc_.__delta = std::numeric_limits<double>::quiet_NaN ();
    pm_.__rotation = pc_.__rotation * pc_.__rotation;
    pc_.__inverse_rotation = pc_.__rotation.inverse ();
  }
  */

  void placement::child_to_mother (const placement & pc_, placement & pm_) const
  {
    child_to_mother (pc_.__translation, pm_.__translation);
    pm_.__phi = pm_.__theta = pm_.__delta = std::numeric_limits<double>::quiet_NaN ();
    pm_.__inverse_rotation = __inverse_rotation * pc_.__inverse_rotation;
    pm_.__rotation = pm_.__inverse_rotation.inverse ();

    // reconvert angles in ZYZ Euler representation from __rotation:
    extract_zyz_euler_angle_from_rotation (pm_.__rotation,
					   pm_.__phi,
					   pm_.__theta,
					   pm_.__delta);
    return;
  }

  void placement::relocate (const placement & wp2_, placement & lp2_) const
  {
    const placement & wp1 = *this;
    //clog << "DEVEL: placement::relocate: T1      = " <<  wp1.get_translation () << endl;
    //clog << "DEVEL: placement::relocate: T2      = " <<  wp2_.get_translation () << endl;
    vector_3d ref = wp2_.get_translation ();
    //clog << "DEVEL: placement::relocate: ref     = " << ref << endl;
    vector_3d new_ref;
    wp1.mother_to_child (ref, new_ref);
    //clog << "DEVEL: placement::relocate: new_ref = " << new_ref << endl;
    lp2_.set_translation (new_ref);
    lp2_.__phi = lp2_.__theta = lp2_.__delta = std::numeric_limits<double>::quiet_NaN ();

    lp2_.__rotation         = wp2_.__rotation * wp1.__inverse_rotation;
    lp2_.__inverse_rotation = lp2_.__rotation.inverse ();

    // reconvert angles in ZYZ Euler representation from __rotation:
    extract_zyz_euler_angle_from_rotation (lp2_.__rotation,
					   lp2_.__phi,
					   lp2_.__theta,
					   lp2_.__delta);
    return;
  }

  void placement::tree_dump (ostream & out_,
			     const string & title_,
			     const string & indent_,
			     bool inherit_) const
  {
    namespace du = datatools::utils;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    this->i_placement::tree_dump (out_, title_, indent, true);

    out_ << indent << i_tree_dumpable::tag << "Valid : "
	 << is_valid ()  << endl;

    out_ << indent << i_tree_dumpable::tag << "Translation : "
	 << __translation  << endl;

    if (is_zyz_rotation ())
      {
	out_ << indent << i_tree_dumpable::tag << "Euler Angles (ZYZ): "
	     << (has_angles ()? "Yes": "No") << endl;
	if (has_angles ())
	  {
	    out_ << indent << i_tree_dumpable::skip_tag
		 << i_tree_dumpable::tag << "Phi :   "
		 << __phi / CLHEP::degree << " degree" << endl;
	    out_ << indent << i_tree_dumpable::skip_tag
		 << i_tree_dumpable::tag << "Theta : "
		 << __theta / CLHEP::degree << " degree"  << endl;
	    out_ << indent << i_tree_dumpable::skip_tag
		 << i_tree_dumpable::last_tag << "Delta : "
		 << __delta / CLHEP::degree << " degree"  << endl;
	  }
      }
    else
      {
	out_ << indent << i_tree_dumpable::tag << "Rotation axis : "
	     << __rotation_axis << endl;
	out_ << indent << i_tree_dumpable::tag << "Rotation angle : "
	     << __rotation_angle / CLHEP::degree << " degree" << endl;
      }

    {
      ostringstream oss_title;
      oss_title << indent << i_tree_dumpable::tag << "Rotation :";
      ostringstream oss_indent;
      oss_indent << indent << i_tree_dumpable::skip_tag;
      geomtools::tree_dump (__rotation,
			    out_,
			    oss_title.str (),
			    oss_indent.str ());
    }

    {
      ostringstream oss_title;
      oss_title << indent << i_tree_dumpable::inherit_tag (inherit_) << "Inverse rotation :";
      ostringstream oss_indent;
      oss_indent << indent << i_tree_dumpable::inherit_skip_tag (inherit_);
      geomtools::tree_dump (__inverse_rotation,
			    out_,
			    oss_title.str (),
			    oss_indent.str ());
    }

    return;
   }

  void
  placement::dump (ostream      & out_ ,
		   const string & title_,
		   const string & indent_) const
  {
    tree_dump (out_, title_, indent_);
    /*
    using namespace std;
    string indent = indent_;
    string title = title_;
    if ( title.empty () ) title = title_;
    out_ << title << endl;
    string tag       = "|-- ";
    string tagc      = "|   ";
    string last_tag  = "`-- ";
    string last_tagc = "    ";

    out_ << indent << tag << "Translation : "
	 << __translation  << endl;
    */
    /*
    tree_dump (__rotation, out_, tag + "Rotation :", indent + tagc);
    tree_dump (__inverse_rotation, out_,
	       last_tag + "Inverse rotation :", indent + last_tagc);
    */
  }

  void placement::to_string (string & str_, const placement & pl_)
  {
    ostringstream oss;
    double lunit = CLHEP::mm;
    double aunit = CLHEP::degree;
    oss << pl_.get_translation ().x () / lunit;
    oss << ' ' << pl_.get_translation ().y () / lunit;
    oss << ' ' << pl_.get_translation ().z () / lunit;
    oss << " (mm)";
    if (pl_.is_simple_rotation ())
      {
	oss << " / ";
	oss << pl_.get_rotation_axis ();
	oss << ' ' << pl_. get_rotation_angle () / aunit;
     }
    else
      {
	oss << " @ ";
	oss << pl_.get_phi () / aunit;
	oss << ' ' << pl_.get_theta () / aunit;
	oss << ' ' << pl_.get_delta () / aunit;
      }
    oss << " (deg)";
    str_ = oss.str ();
    return;
  }

  ostream & operator<< (ostream & out_, const placement & pl_)
  {
    string s;
    placement::to_string (s, pl_);
    out_ << s;
    return out_;
  }

  bool placement::from_string (const string & str_, placement & pl_)
  {
    pl_.reset ();
    istringstream oss (str_);

    double x (0.0), y (0.0), z (0.0);
    double length_unit = CLHEP::mm;
    double phi (0.0), theta (0.0), delta (0.0);
    double angle_unit = CLHEP::degree;

    oss >> ws;
    if (oss.eof ())
      {
	return false;
      }

    // Position:
    oss >> x >> y >> z >> ws;
    // extract length unit:
    if (! oss.eof ())
      {
	char open = oss.peek ();
	if (open == '(')
	  {
	    string length_unit_str;
	    oss.get ();
	    getline (oss, length_unit_str, ')');
	    if (! oss)
	      {
		return false;
	      }
	    length_unit = datatools::utils::units::get_length_unit_from (length_unit_str);
	  }
	oss >> ws;
      }
    x *= length_unit;
    y *= length_unit;
    z *= length_unit;

    string rotation_axis_label;
    int rotation_axis = -1;
    bool simple_rotation = false;
    oss >> ws;
    if (! oss.eof ())
      {
	char pr_sep = oss.peek ();
	if (pr_sep != '@' && pr_sep != '/')
	  {
	    return false;
	  }
	oss.get ();
	if (pr_sep == '@' )
	  {
	    oss >> phi >> theta >> delta >> ws;
	  }
	if (pr_sep == '/' )
	  {
	    simple_rotation = true;
	    oss >> rotation_axis_label >> phi >> ws;;
	    rotation_axis = get_rotation_axis_from_label (rotation_axis_label);
	  }
	// extract angle unit:
	if (! oss.eof ())
	  {
	    char open = oss.peek ();
	    if (open == '(')
	      {
		string angle_unit_str;
		oss.get ();
		getline (oss, angle_unit_str, ')');
		if (! oss)
		  {
		    return false;
		  }
		angle_unit = datatools::utils::units::get_angle_unit_from (angle_unit_str);
	      }
	    oss >> ws;
	  }

      }
    phi *= angle_unit;
    if (! simple_rotation)
      {
	theta *= angle_unit;
	delta *= angle_unit;
	pl_.set (x, y, z, phi, theta, delta);
      }
    else
      {
	pl_.set (x, y, z, rotation_axis, phi);
      }
    return true;
  }

  void placement::test () const
  {
    clog << "DEVEL: placement::test: " << endl;
    double phi, theta, delta;
    extract_zyz_euler_angle_from_rotation (__rotation, phi, theta, delta);
    clog << "|-- phi   = " << get_phi () / CLHEP::degree << " deg" << endl;
    clog << "|-- theta = " << get_theta () / CLHEP::degree << " deg" << endl;
    clog << "|-- delta = " << get_delta () / CLHEP::degree << " deg" << endl;
    clog << "|-- test ZYZ: " << endl;
    clog << "|   |-- phi   = " << phi / CLHEP::degree << " deg" << endl;
    clog << "|   |-- theta = " << theta / CLHEP::degree << " deg" << endl;
    clog << "|   `-- delta = " << delta / CLHEP::degree << " deg" << endl;
    clog << "|--rotation: ";
    geomtools::tree_dump (__rotation, clog, "", "|   ");
    clog << "|--Inverse rotation: ";
    geomtools::tree_dump (__inverse_rotation, clog, "", "|   ");
    double a, b, c;
    extract_xyz_euler_angle_from_rotation (__rotation, a, b, c);
    clog << "|-- test XYZ: " << endl;
    clog << "|   |-- a = " << a / CLHEP::degree << " deg" << endl;
    clog << "|   |-- b = " << b / CLHEP::degree << " deg" << endl;
    clog << "|   `-- c = " << c / CLHEP::degree << " deg" << endl;
    clog << "|-- placement XYZ: " << endl;
    placement pxyz;
    pxyz.set_translation (0.,0.,0.);
    pxyz.set_orientation_xyz (a, b, c);
    pxyz.tree_dump (clog, "", "|   ");
    clog << "`-- Placement: " << pxyz << endl;
    clog << endl;

    return;
  }

} // end of namespace geomtools

// end of placement.cc

// -*- mode: c++; -*- 
/* placement.cc 
 */

#include <geomtools/placement.h>

namespace geomtools {

  using namespace std;
 
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
    __phi = __theta = __delta = std::numeric_limits<double>::quiet_NaN();
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
    geomtools::create_rotation_3d (__rotation, __phi, __theta, __delta);
    __inverse_rotation = __rotation.inverse ();
  }

  bool placement::has_angles () const
  {
    return ((__phi == __phi) && (__theta == __theta) && (__delta == __delta));
  }

  void placement::set_orientation (const rotation_3d & r_)
  {
    __phi = std::numeric_limits<double>::quiet_NaN();
    __theta = std::numeric_limits<double>::quiet_NaN();
    __delta = std::numeric_limits<double>::quiet_NaN();
    __rotation = r_;
    __inverse_rotation = __rotation.inverse ();    
  }

  void placement::set (double x_, double y_, double z_, 
		       double phi_, double theta_, double delta_)
  {
    set_translation (x_, y_, z_);
    set_orientation (phi_, theta_, delta_);
  }

  void placement::set (const vector_3d & t_, 
		       double phi_, double theta_, double delta_)
  {
    set_translation (t_);
    set_orientation (phi_, theta_, delta_);
  }

  void 
  placement::set_orientation (double phi_, double theta_, double delta_)
  {
    __phi = phi_;
    __theta = theta_;
    __delta = delta_;
    __compute_orientation ();
    /*
    rotation_3d r;
    geomtools::create_rotation_3d (r, phi_, theta_, delta_);
    __set_orientation (r);
    */
    /*
    geomtools::create_rotation_3d (__rotation, __phi, __theta, __delta);
    __inverse_rotation = __rotation.inverse ();
    */
  }
  
  const rotation_3d & 
  placement::get_inverse_rotation () const
  {
    return __inverse_rotation;
  }
 
  // i_placement interface:
  size_t placement::get_number_of_items () const
  {
    return 1;
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
 
  // ctor/dtor:
  placement::placement () : i_placement ()
  {
    __phi = __theta = __delta = 0.0;
  }

  placement::placement (const vector_3d & translation_, 
			double phi_,
			double theta_,
			double delta_)
  {
    __phi = __theta = __delta = 0.0;
    set_translation (translation_);
    set_orientation (phi_, theta_, delta_);
  }

  placement::placement (double x_,
			double y_,
			double z_, 
			double phi_,
			double theta_,
			double delta_)
  {
    set_translation (x_, y_, z_);
    set_orientation (phi_, theta_, delta_);
  }
  
  placement::~placement ()
  {
  }

  void placement::reset ()
  {
    set_translation (0., 0., 0.);
    set_orientation (0., 0., 0.);
  }
  
  // transformation methods:

  void 
  placement::mother_to_child (const vector_3d & mother_pos_, 
			     vector_3d & child_pos_) const
  {
    vector_3d v = mother_pos_ - __translation;
    child_pos_ = v.transform (__rotation);
  }

  vector_3d 
  placement::mother_to_child (const vector_3d & mother_pos_) const
  {
    vector_3d v;
    mother_to_child (mother_pos_, v);
    return v;
  }

  void 
  placement::child_to_mother (const vector_3d & child_pos_ , 
			     vector_3d & mother_pos_) const
  {
    vector_3d v (child_pos_);
    mother_pos_ = v.transform (__inverse_rotation) + __translation;
  }

  vector_3d 
  placement::child_to_mother (const vector_3d & child_pos_) const
  {
    vector_3d v;
    child_to_mother (child_pos_, v);
    return v;
  }

  void 
  placement::mother_to_child_direction (const vector_3d & mother_dir_, 
					vector_3d & child_dir_) const
  {
    vector_3d a;
    mother_to_child (vector_3d ( 0., 0., 0.), a);
    vector_3d b;
    mother_to_child (mother_dir_, b);
    child_dir_= b - a;
  }

  vector_3d 
  placement::mother_to_child_direction (const vector_3d & mother_dir_) const
  {
    vector_3d v;
    mother_to_child_direction (mother_dir_,v);
    return v;
  }
  
  void 
  placement::child_to_mother_direction (const vector_3d & child_dir_ , 
					vector_3d & mother_dir_) const
  {
    vector_3d a;
    child_to_mother (vector_3d ( 0., 0., 0.), a);
    vector_3d b;
    child_to_mother (mother_dir_, b);
    mother_dir_= b - a;
  }

  vector_3d 
  placement::child_to_mother_direction (const vector_3d & child_dir_) const
  {
    vector_3d v;
    child_to_mother_direction (child_dir_,v);
    return v;
  }

  void placement::mother_to_child (const rotation_3d & rm_, rotation_3d & rc_) const
  {
    rc_ = __inverse_rotation * rm_ * __rotation;
  }

  void placement::child_to_mother (const rotation_3d & rc_, rotation_3d & rm_) const
  {
    rm_ = __rotation * rc_ * __inverse_rotation;
  }

  void placement::mother_to_child (const placement & pm_, placement & pc_) const
  {
    mother_to_child (pm_.__translation, pc_.__translation);
    pc_.__phi = pc_.__theta = pc_.__delta = std::numeric_limits<double>::quiet_NaN ();
    mother_to_child (pm_.__rotation, pc_.__rotation);
    pc_.__inverse_rotation = pc_.__rotation.inverse ();
 }

  void placement::child_to_mother (const placement & pc_, placement & pm_) const
  {
    child_to_mother (pc_.__translation, pm_.__translation);
    pm_.__phi = pm_.__theta = pm_.__delta = std::numeric_limits<double>::quiet_NaN ();
    child_to_mother (pc_.__rotation, pm_.__rotation);
    pm_.__inverse_rotation = pm_.__rotation.inverse ();
  }

  void placement::tree_dump (ostream & out_, 
			     const string & title_, 
			     const string & indent_, 
			     bool inherit_) const
  {
    namespace du = datatools::utils;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    /*
    if (! title_.empty ()) 
      {
        out_ << indent << title_ << endl;
      }
    */
    this->i_placement::tree_dump (out_, title_, indent, true);

    out_ << indent << i_tree_dumpable::tag << "Translation : " 
	 << __translation  << endl;
    out_ << indent << i_tree_dumpable::tag << "Angles : "
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

} // end of namespace geomtools

// end of placement.cc

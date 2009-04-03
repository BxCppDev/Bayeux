// -*- mode: c++; -*- 
/* placement.cc
 */

#include <geomtools/placement.h>

namespace geomtools {

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
  
  void 
  placement::set_orientation (const rotation_3d & new_value_)
  {
    __rotation = new_value_;
    __inverse_rotation = __rotation.inverse ();
  }

  void 
  placement::set_orientation (double phi_, double theta_, double delta_)
  {
    rotation_3d r;
    geomtools::create_rotation_3d (r, phi_, theta_, delta_);
    set_orientation (r);
  }
  
  const rotation_3d & 
  placement::get_inverse_rotation () const
  {
    return __inverse_rotation;
  }
  
  // ctor/dtor:
  placement::placement ()
  {
  }

  placement::placement (const vector_3d & translation_, 
			double phi_,
			double theta_,
			double delta_)
  {
    set_translation (translation_);
    set_orientation (phi_, theta_, delta_);
  }
  
  placement::~placement ()
  {
  }

  void 
  placement::reset ()
  {
    set_translation (vector_3d (0., 0., 0.));
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

  void 
  placement::dump (std::ostream      & out_ , 
		   const std::string & title_, 
		   const std::string & indent_) const
  {
    using namespace std;
    string indent = indent_;    
    string title = title_;
    if ( title.empty () ) title = title_;
    out_ << title << std::endl;
    string tag       = "|-- ";
    string tagc      = "|   ";
    string last_tag  = "`-- ";
    string last_tagc = "    ";

    out_ << indent << tag << "Translation : " 
	 << __translation  << endl;
    //    ostringstream oss;
    //oss << 
    tree_dump (__rotation, out_, tag + "Rotation :", indent + tagc);
    tree_dump (__inverse_rotation, out_, last_tag + "Inverse rotation :", indent + last_tagc);
    /*
    out_ << indent << tag << "Rotation : \n" 
	 << indent << __rotation  << endl;
    out_ << indent << last_tag << "Inverse rotation : \n" 
	 << indent << __inverse_rotation  << std::endl;
    */
  }


  /*
    void 
    placement::dump ( std::ostream & out_ , 
    const std::string & title_ , 
    const std::string & indent_ ,
    bool inherit_ ) const
    {
    shape_data::dump (out_,title_,indent_,true);
    std::string indent=indent_;
    
    std::string title=title_;
    std::string indent=indent_;
    if ( title.empty () ) title=title_;
    out_ << title << std::endl;
  
    std::string tag="|-- ";
    std::string tagc="|   ";
    std::string last_tag="`-- ";
    std::string last_tagc="    ";
    std::string & atag=tag;
    std::string & atagc=tagc;
    out_ << indent << atag << "Translation :            " 
    << translation () << std::endl;
    geomtools::dump (rotation (),
    out_,
    indent+atag+"Rotation",
    indent+atagc);
    if ( !inherit_ ) {
    atag=last_tag;
    atagc=last_tagc; 
    }
    geomtools::dump (inverse_rotation (),
    out_,
    indent+atag+"Inverse rotation",
    indent+atagc);
    }
  */
 
} // end of namespace geomtools

// end of placement.cc
